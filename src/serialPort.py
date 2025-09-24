import serial
import serial.tools.list_ports
import sys
from typing import Optional



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

