import serial
import sys


# Initialize variables with default values
baud = 9600
path = None
command = None
userInput = None

# Iterate over arguments with their indices
for idx, arg in enumerate(sys.argv):
  match arg:
    case 'help':
      print('Available commands/args:\n')
      print('-p: program the device. Must be followed by the path to the file. ex: serialProg.py -p "path2file"')
      print('-v: verifiy the device. Must be followed by the path to the file. ex: serialProg.py -v "path2file"')
      print('-e: erase the ROM of the device')
      print('-b: sets the Baud rate, it is 9600 by default')
    case '-b':
      if idx + 1 < len(sys.argv):
        baud = int(sys.argv[idx + 1])
    case '-p':
      if idx + 1 < len(sys.argv):
        path = sys.argv[idx + 1]
        command = 'p'
    case '-v':
      if idx + 1 < len(sys.argv):
        path = sys.argv[idx + 1]
        command = 'v'
    case '-e':
      command = 'e'
    case _:
      path = sys.argv[idx]

# Main loop 
while True:
    userInput = input()
    if(userInput == 'x'):
      break
    pass
  
print('Program is stopping...')

