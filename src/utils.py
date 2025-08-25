import serial
import sys

def listPorts(ports):
  print("Available serial ports:")
  for i, port in enumerate(ports):
      print(f"{i}: {port.device} - {port.description}")

