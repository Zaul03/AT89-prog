import serial
import serial.tools.list_ports
import sys
from utils import listPorts

# Initialize variables with default values
baud = 9600
path = None
command = None
userInput = None
ports = list(serial.tools.list_ports.comports())
selected_port = None
connection = False



# Iterate over arguments with their indices
for idx, arg in enumerate(sys.argv):
  match arg:
    case 'help':
      print('Available commands/args: ')
      print('-p: program the device. Must be followed by the path to the file. ex: prog-tool.py -p "path"')
      print('-v: verifiy the device. Must be followed by the path to the file. ex: prog-tool.py -v "path"')
      print('-e: erase the ROM of the device')
      print('-b: sets the Baud rate, it is 9600 by default')
      sys.exit()
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

# Ask user to select a port
while True:
    try:
        listPorts(ports)
        selection = int(input("Select a port by number, press 'r' to refresh: "))
        if 0 <= selection < len(ports):
            selected_port = ports[selection].device
            break
        else:
            print("Invalid selection. Try again.")
    except ValueError:
      print("Please enter a valid number.")
      

print(f"Selected port: {selected_port}")

# Now you can use selected_port to open the serial connection
ser = serial.Serial(selected_port, baudrate=baud, timeout=1)





# Main loop 
while True: 

  match command:
    case 'p':
      pass
    case 'v':
      pass
    case 'e':
      pass

  userInput = input()
  if(userInput == 'x'):
    break
        



  
print('Program is stopping...')

