import serial
import serial.tools.list_ports
import argparse
import sys
import time
from typing import Optional, List

# Defaults
DEFAULT_BAUD = 57600
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

def get_ports():
  ports = list(serial.tools.list_ports.comports())
  return ports

def pick_port_interactive():
  while True:
    ports = get_ports()
    
    if not ports:
      print('\nNo available serial ports')
      selection = input('[x] - close program, [r] - reload ports')
    else:
      for i, port in enumerate(ports):
        print(f"{i}: {port.device} - {port.description}")
      selection = input('[x] - close program, [r] - reload ports, [port number] - select port: ')
    
    if selection == 'x':
      sys.exit('Program has closed')
    elif selection == 'r':
      continue
    else:
      try:
        selection = int(selection)
      except ValueError:
        print("Please enter a valid number.")
        continue
      if 0<=selection<len(ports):
        return ports[selection].device
      else:
        print("Number out of range!")
        continue



def resolve_port(cli_port: Optional[str])-> Optional[str]:
  if cli_port:
    return cli_port
  return pick_port_interactive()

def open_serial(port, baud, time_out):
  try:
    print(port)
    ser = serial.Serial(port, baudrate=baud, bytesize=serial.EIGHTBITS, parity=serial.PARITY_NONE, stopbits=serial.STOPBITS_ONE)
  except serial.SerialException:
    print('Failed to open port. Closing program....')
    sys.exit(2)
  print('Port opened succesfully')
  return ser
