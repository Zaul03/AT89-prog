import serial
import serial.tools.list_ports
import sys
from prog_utils import get_ports

# Initialize variables with default values
baud = 9600
path = None
command = None
userInput = None
selected_port = None




# Iterate over arguments with their indices
for idx, arg in enumerate(sys.argv):
  match arg:
    case '-h':
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
  ports = get_ports()
  
  if not ports:
    print('No available serial ports')
    selection = input('If you wish to refresh press \'r\' \n If you wish to exit press \'x\'')
  else:
    for i, port in enumerate(ports):
      print(f"{i}: {port.device} - {port.description}")
    selection = input('If you wish to refresh press \'r\' \n If you wish to exit press \'x\' \n If you wish to select a port enter the port number:')
  
  if selection == 'x':
    sys.exit('Program has closed')
  elif selection == 'r':
    continue
  else:
    try:
      selection = int(selection)
    except ValueError:
      selection = int(input("Please enter a valid number."))

    
      

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
        



ser.close()
print('Program is stopping...')

