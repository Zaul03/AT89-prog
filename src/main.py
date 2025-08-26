from __future__ import annotations
import sys
from typing import Optional, List


#Helper
from prog_utils import arg_parse
from prog_utils import resolve_port
from prog_utils import open_serial
from prog_utils import RX_reply


def main(argv: Optional[List[str]] = None):
  args = arg_parse().parse_args(argv)

  port = resolve_port(args.port)
  print(f"Selected port {port}")

  ser = open_serial(port, args.baud, args.timeout)

  while True:
    print(RX_reply(ser, True))
  

    


if __name__== "__main__":
  sys.exit(main())   
    




