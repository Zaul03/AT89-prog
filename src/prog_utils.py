import serial
import serial.tools.list_ports

def get_ports():
  ports = list(serial.tools.list_ports.comports())
  return ports

