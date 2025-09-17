from __future__ import annotations
import sys
import time
from typing import Optional, List


#Helper
from prog_utils import arg_parse
from prog_utils import resolve_port
from prog_utils import open_serial
from prog_utils import RX_reply
from prog_utils import TX_RX
from prog_utils import send_file_cmd


file = None

def main(argv: Optional[List[str]] = None):
  args = arg_parse().parse_args(argv)

  port = resolve_port(args.port)
  print(f"Selected port {port}")

  ser = open_serial(port, args.baud, args.timeout)
  cmd = args.cmd
  file = args.hexfile

  
  try:
    while True:
      r = RX_reply(ser, True)
      print(r)
      if r == "AT89C2051 programmer initialized successfully.":
        match cmd:
           case 'program':
              send_file_cmd(ser, cmd, file, True, True, 0xFF, 0.4)
              break
           case 'verify':
              send_file_cmd(ser, cmd, file, False, True, 0xFF, 0.4)
              break
           case 'erase':
              if TX_RX(ser, cmd, None) == 0x06:
                 print("Chip erased")
                 break
  except KeyboardInterrupt:
    print("\nInterrupted by user.")
  finally:
      try:
          ser.close()
      except Exception:
          pass
      print("Serial port closed.")
    
    
if __name__== "__main__":
  sys.exit(main())   
    




