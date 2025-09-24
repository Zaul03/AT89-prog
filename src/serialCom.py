from serial import Serial
import time
import os

from typing import Optional

# Defaults
FRAME_SIZE = 64
DATA_CAP = FRAME_SIZE - 3

CMD_MAP = {
    "erase" :  ord("e"), "e": ord("e"),
    "program": ord("p"), "p": ord("p"),
    "verify": ord("v"), "v":ord("v"),
    "data":  ord("d"), "d":ord("d")
}



# ---------- RX and TX ------------------
def _checksum(cmd: int, data: bytes , length: int) -> int:
  """8-bit sum: CMD + LEN + first `length` bytes of DATA (mod 256)."""
  s = 0
  for i in data:
    s += i
  s += cmd + length
  return s & 0xFF

def build_packet(cmd: str, data: bytes = b"") -> bytes:
    cmd = cmd.lower().strip()
    if cmd not in CMD_MAP:
        raise ValueError(f"Unknown command: {cmd}")

    # Accept bytes-like, normalize to bytes
    if not isinstance(data, (bytes, bytearray, memoryview)):
        raise TypeError("data must be bytes-like")
    data = bytes(data)

    length = len(data)
    if length > DATA_CAP:
        raise ValueError(f"data too long: {length} > {DATA_CAP}")

    cmd_byte = CMD_MAP[cmd]

    frame = bytearray(FRAME_SIZE)
    frame[0] = cmd_byte
    frame[1] = length
    if length:
        frame[2:2+length] = data
    frame[FRAME_SIZE - 1] = _checksum(cmd_byte, data, length)
    return bytes(frame)
  
def TX(ser, cmd: str, data =b"")->None:
  packet = build_packet(cmd, data)
  while True:
      ser.reset_input_buffer()
      ser.write(packet)
      ser.flush()
      return 

def RX_reply(ser: Serial ,as_text: bool = True) -> Optional[str | bytes]:
  line = ser.readline()  
  if line:
      return line.decode(errors="replace").rstrip("\r\n") if as_text else bytes(line)
  buf = ser.read(ser.in_waiting or 1) 
  if not buf:
      return None
  return buf.decode(errors="replace") if as_text else buf

def TX_RX (ser: Serial, cmd: str, data: bytes = b"") -> Optional[str]:
  while True:
      TX(ser, cmd, data)
      reply = RX_reply(ser, as_text=True)
      
      if reply is None:
        return None
      else:
        return str(reply)

def _ack_resp(resp) -> bool:
    
    if isinstance(resp, (bytes, bytearray)):
        try:
            s = bytes(resp).decode("ascii", errors="ignore")
        except Exception:
            return False
    else:
        s = str(resp) if resp is not None else ""

    s = s.strip().lower()
    # Some devices use ASCII ACK (0x06); accept that too:
    if isinstance(resp, (bytes, bytearray)) and resp == b"\x06":
        return True
    return s

def send_file_cmd(
    ser,
    cmd: str,

    path: str,
    erase_first: Optional[bool] = True,
    pad_last: bool = True,   # set True if device expects fixed-size frames
    pad_byte: int = 0xFF,
    max_retries: int = 0,
    inter_chunk_delay: float = 0.4, #400ms
) -> None:
    
    # check for file
    if not os.path.isfile(path):
        print(f"File not found: {path}")
        return
    
    #check if file empty
    total = os.path.getsize(path)
    if total == 0:
        print("File is empty.")
        return

    # start message
    print(f"Sending {path} ({total} bytes) in chunks of ≤{DATA_CAP} …")
    start_ts = time.perf_counter()

    # optional: erase first
    if erase_first:
      r = TX_RX(ser, 'erase', b"")
      if not _ack_resp(r):
          raise RuntimeError(f"ERASE failed: {repr(r)}")
      print(r)

    sent = 0
    chunk_idx = 0
    verification = True

    with open(path, "rb") as f:
        # first packet sets the mode
        chunk = f.read(DATA_CAP)
        if not chunk:
          return
        if pad_last and len(chunk) < DATA_CAP:
            chunk = chunk + bytes([pad_byte]) * (DATA_CAP - len(chunk))


        while  not _ack_resp(r):
          r = RX_reply(ser, True)
          print(r)
        print('Erase complete')

        print('Programming...')
        r = TX_RX(ser, cmd, chunk)
        if not _ack_resp(r):
          raise RuntimeError(f"NACK or unexpected response: {repr(r)}")
        print(r) # ussualy just ACK

        while  not _ack_resp(r):
          r = RX_reply(ser, True)
          print(r)

        while _ack_resp(r) != "Program OK":
          r = RX_reply(ser, True)
          print(r)

        r = RX_reply(ser,True) # verify / prog
        print(r)

        if r != "Verification OK":
          verification = False
      
        if r != "Program OK":
          print('Failed to write the FLASH')
          return

        # send the rest of the data
        while True:
            chunk = f.read(DATA_CAP)
            if not chunk:
                break
            if pad_last and len(chunk) < DATA_CAP:
                chunk = chunk + bytes([pad_byte]) * (DATA_CAP - len(chunk))

            # retry per chunk
            attempt = 0

            # chunk 2 to N
            while True:
                try:
                    r = TX_RX(ser, "data", chunk)
                    print(r) # ussualy just ACK
                    
                    r = RX_reply(ser,True) # checksum
                    print(r)

                    if r != "Checksum OK":
                       time.sleep(0.1)
                       continue
                    
                    r = RX_reply(ser,True) # verify / prog
                    print(r)

                    if r != "Verification OK":
                      verification = False
                  
                    if r != "Program OK":
                      print('Failed to write the FLASH')
                      return

                    break  # got ACK
                except Exception as e:
                    attempt += 1
                    if attempt > max_retries:
                        print(f"\nChunk {chunk_idx+1} failed: {e}")
                        return
                    time.sleep(0.001)

            # progress
            chunk_idx += 1
            sent += min(len(chunk), total - sent)
            if (chunk_idx % 16) == 0 or sent == total:
                pct = sent / total * 100.0
                print(f"[{chunk_idx}] {sent}/{total} bytes ({pct:0.1f}%)")
            else:
                print(".", end="", flush=True)

            if inter_chunk_delay:
                time.sleep(inter_chunk_delay)


    print()  # newline after dot
       
    dur = time.perf_counter() - start_ts
    rate = (total / 1024) / dur if dur > 0 else 0
    print(f"Done in {dur:0.2f}s ({rate:0.1f} KiB/s).")
