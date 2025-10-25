from serial import Serial
import time
import os

from intelhex import hex2bin

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
  
def TX(ser, data: bytes)->None:
  ser.write(data)
  return 

def RX(ser: Serial) -> Optional[str | bytes]:
    line = ser.readline()
    return line

def _ack_resp(resp) -> bool:
    
    if isinstance(resp, (bytes, bytearray)):
        try:
            s = bytes(resp).decode("ascii", errors="ignore")
        except Exception:
            return False
    else:
        s = str(resp) if resp is not None else ""

    s = s.strip()
    # Some devices use ASCII ACK (0x06); accept that too:
    if isinstance(resp, (bytes, bytearray)) and resp == b"\x06":
        return True
    return s

