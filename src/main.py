from __future__ import annotations
import argparse
import sys
import os
from typing import Optional, List


import serial
from serial import Serial, SerialException

#Helper
from prog_utils import arg_parse
from prog_utils import resolve_port
from prog_utils import open_serial


#Vars
path = None


def main(argv: Optional[List[str]] = None):
  args = arg_parse().parse_args(argv)

  port = resolve_port(args.port)
  print(f"Selected port {port}")

  ser = open_serial(port, args.baud, args.timeout)

  try:
      while True:
        s = ser.read(1)
        print(s)
  except KeyboardInterrupt:
    sys.exit()
    


if __name__== "__main__":
  sys.exit(main())     




