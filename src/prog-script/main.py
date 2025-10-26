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
from serialCom import _ack_resp
from intelhex import hex2bin


file = None

def main(argv: Optional[List[str]] = None):

  args = arg_parse().parse_args(argv)

  port = resolve_port(args.port)
  print(f"Selected port {port}")

  ser = open_serial(port, args.baud, time_out=args.timeout)
  

  #hex2bin(file, file.replace('.hex', '.bin'))
  packet = build_packet(args.cmd , b'')
  print(f"Built packet: {packet}")

  #send cmd
  TX(ser, packet)

  resp = None

  #wait for ack
  while True:
    resp = RX(ser)

    resp = str(resp, 'ascii').strip('\r\n')
    print (f"Response: {resp}")
    if resp == '6':
      print("Received ACK")
      break
    else:
      print("Received NACK, retrying...")
      TX(ser, packet)
    

      
  ser.close()
  

    
    
    
if __name__== "__main__":
  sys.exit(main())   
    





