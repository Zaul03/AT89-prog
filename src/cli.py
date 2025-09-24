import argparse

# Defaults
DEFAULT_BAUD = 9600
DEFAULT_TIMEOUT = 2.0

def arg_parse() -> argparse.ArgumentParser:
  p = argparse.ArgumentParser(
    prog = "main",
    description="AT89C2051/4051 serial programmer (Arduino bridge).")

  p.add_argument("-p", "--port", help = 'Serial port( e.g. COM5 or /dev/ttyACM0)')
  p.add_argument("-b", "--baud", type=int, default=DEFAULT_BAUD, help=f"Baud rate (default: {DEFAULT_BAUD})")
  p.add_argument("--timeout", type=float, default=DEFAULT_TIMEOUT, help=f"Serial timeout seconds (default: {DEFAULT_TIMEOUT})")

  sub = p.add_subparsers(dest="cmd", required=True)

  #erase
  sub.add_parser("erase", help="Chip erase")

  # program
  sp_prog = sub.add_parser("program", help="Program from Intel HEX (auto-erase + verify unless disabled)")
  sp_prog.add_argument("hexfile", help="Input Intel HEX file")
  sp_prog.add_argument("--no-verify", action="store_true", help="Skip verify after programming")
  sp_prog.add_argument("--no-erase",  action="store_true", help="Skip chip erase before programming")

  # verify
  sp_ver = sub.add_parser("verify", help="Verify device against Intel HEX")
  sp_ver.add_argument("hexfile", help="Input Intel HEX file")

  return p
