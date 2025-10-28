from serial import Serial
from typing import Optional

# Defaults
FRAME_SIZE = 64
DATA_CAP = FRAME_SIZE - 3

CMD_MAP = {
    "erase" :   ord('e'),    "e":   ord('e'),
    "program":  ord('p'),    "p":   ord('p'),
    "verify":   ord('v'),    "v":   ord('v'),
    "read":     ord('m'),    "m":   ord('m')
}



# ---------- RX and TX ------------------
def _checksum(cmd: bytes, data: bytes , length: bytes) -> bytes:
  """8-bit sum: CMD + LEN + first `length` bytes of DATA (mod 256)."""
  s = 0
  for i in data:
    s += i
  s += cmd + length
  return s & 0xFF

def build_packet(cmd: str, data: bytes = b"") -> bytes:

    # Accept bytes-like, normalize to byte

    length = len(data)
    cmd_byte = CMD_MAP[cmd]

    frame = bytearray(FRAME_SIZE)
    frame[0] = cmd_byte
    frame[1] = length
    frame[2:length+2] = data
    frame[FRAME_SIZE-1] = _checksum(cmd_byte, data, length)
    return bytes(frame)
  
def TX(ser, data: bytes)->None:
    ser.write(data)
    return 

def RX(ser: Serial) -> Optional[bytes]:
    line = ser.readline()
    return line

