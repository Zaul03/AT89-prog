#include <Arduino.h>
#include <SerialTransfer.h>
#include <Prog.h>

#define ACK 0x06
#define SERIAL_BUFFER_SIZE 64

#define CMD serialBuffer[0]
#define LENGTH_RX serialBuffer[1]
#define CHECKSUM_RX serialBuffer[LENGTH_RX - 1]
#define DATA_START_INDEX 2

uint8_t serialBuffer[SERIAL_BUFFER_SIZE]; // [cmd, length, data..., checksum]
uint8_t bufferIndex = 0;
uint8_t checksum = 0;


enum State {IDLE, RECEIVING_DATA, ERASE, PROGRAM, VERIFY};
State state = IDLE;
Prog prog;

unsigned long receiveStartTime = 0;


void setup() {
    Serial.begin(9600);
    
    if (!prog.init())
        return;
}

void loop() {
    
    switch (state){
        case IDLE:
            
            while(!Serial.available());
                
            state = RECEIVING_DATA;
            receiveStartTime = millis();
            checksum = 0;
            bufferIndex = 0; 

            break;
        case RECEIVING_DATA:

            while (bufferIndex < SERIAL_BUFFER_SIZE && Serial.available() > 0) {
                serialBuffer[bufferIndex] = Serial.read();
                if(bufferIndex < SERIAL_BUFFER_SIZE - 1)
                    checksum += serialBuffer[bufferIndex];
                bufferIndex++;
            }
    
            // Validate checksum
            if(checksum != CHECKSUM_RX) {
                Serial.println("Error: Checksum mismatch.");
                Serial.println("Received: " + String(CHECKSUM_RX, DEC) + ", Calculated: " + String(checksum, DEC));
                for(int i=0; i<SERIAL_BUFFER_SIZE-1; i++) {
                    Serial.print("Received packet" + String(serialBuffer[i], HEX));
                }
                checksum = 0;
                state = IDLE;
                bufferIndex = 0;
                return;
            }

            // Acknowledge receipt and validation
            Serial.println(ACK); 
            bufferIndex = 0;
            checksum = 0;
            
            

            // Process command
            switch (CMD){
                case 'p':
                        state = PROGRAM;
                    break;
                case 'e':
                        state = ERASE;
                    break;
                case 'v':
                        state = VERIFY;
                    break;
                default:
                        state = IDLE;
                    break;
            }
            
            break;
        case ERASE:
            
            if (prog.eraseChip())
                Serial.println(ACK);
            else
                Serial.println("Error: Chip erase failed."); 
                
            state = IDLE;
            break;
        case PROGRAM:
            
            for(uint8_t i = DATA_START_INDEX; i < LENGTH_RX - 1; i++) {
                if (!prog.progChip(serialBuffer[i], false)) {
                    Serial.println("Error: Programming failed at byte index " + String(i-2));
                    state = IDLE;
                    return;
                }
                else 
                    Serial.println(ACK); 
            }
            break;
        case VERIFY:
            
            for(uint8_t i = DATA_START_INDEX; i < LENGTH_RX - 1; i++) {
                if (!prog.verifyChip(serialBuffer[i])) {
                    Serial.println("Error: Verify failed at byte index " + String(i-2));
                    state = IDLE;
                    return;
                }
                else 
                    Serial.println(ACK); 
            }
            break;
        default:
            
            state = IDLE;
            break;
    }
}
