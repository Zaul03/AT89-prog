from __future__ import annotations
import sys
from typing import Optional, List
import time


#Helper
from cli import arg_parse
from serialPort import resolve_port
from serialPort import open_serial
from serialCom import TX, RX
from serialCom import build_packet

file = None
resp = None

def main(argv: Optional[List[str]] = None):

  args = arg_parse().parse_args(argv)

  while True:
    port = resolve_port(args.port)
    print(f"Selected port {port}")

    ser = open_serial(port, args.baud, time_out=args.timeout)

    #allow arduino to reset
    while True:
      resp = str(RX(ser), 'ascii').strip("\r\n")
      print(resp)
      if resp == "Ready!":
        break

    data = "Hello World!"
    packet = build_packet(args.cmd, b'Hello World')
    print(f'Packet to be sent: {packet.hex()}')

    TX(ser, packet)
    time.sleep(0.05)
    while True:
        resp = str(RX(ser), 'ascii').strip("\r\n")
        match resp:
          case "Chip erased":
            print(resp)
            break
          case "6":
            print(f"Packet received, returned code: {resp}")
          case "Error: Checksum mismatch.":
            time.sleep(0.2) #timeout
            TX(ser,packet)
            time.sleep(0.05)

    ser.close()
  
  
if __name__== "__main__":
  sys.exit(main())   
    





