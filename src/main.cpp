#include <Arduino.h>
#include "../lib/AT89C2051-prog/AT89C2051-prog.h"

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
AT89C2051Prog Prog;

unsigned long receiveStartTime = 0;


void setup() {
    Serial.begin(9600);
    
    if (!Prog.init())
        return;
}

void loop() {

    switch (state){
        case IDLE:
            while (!Serial.available());

            state = RECEIVING_DATA;
            receiveStartTime = millis();
            checksum = 0;
            bufferIndex = 0; 

            break;
        case RECEIVING_DATA:
            // Timeout
            if (millis() - receiveStartTime > 2000) {
                Serial.println("Error: Data receive timeout.");
                state = IDLE;
                bufferIndex = 0;
                return;}
            
            // Read command and length
            while (!Serial.available());
            CMD = Serial.read(); //cmd
            checksum += CMD;
            bufferIndex++;
            
            while (!Serial.available());
            LENGTH_RX = Serial.read(); //length
            checksum += LENGTH_RX;
            bufferIndex++;
            

            //Validate packet length
            if (LENGTH_RX < 2 || LENGTH_RX > SERIAL_BUFFER_SIZE) {
                Serial.println("Error: Invalid packet length.");
                state = IDLE;
                bufferIndex = 0;
                return;
            }
            
            
            // Read remaining data into the buffer
            while (Serial.available() && bufferIndex < SERIAL_BUFFER_SIZE) {
                serialBuffer[bufferIndex++] = Serial.read();
            }
            

            //Calculate checksum
            for (uint8_t i = DATA_START_INDEX; i < LENGTH_RX - 1; i++) {
                checksum += serialBuffer[i];}
            
            // Validate checksum
            if(checksum != CHECKSUM_RX) {
                Serial.println("Error: Checksum mismatch.");
                state = IDLE;
                bufferIndex = 0;
                return;
            }

            // Acknowledge receipt and validation
            Serial.write(ACK); 
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
            if (Prog.eraseChip())
                Serial.write(ACK); 
            else
                Serial.println("Error: Chip erase failed."); 
            break;
        case PROGRAM:
            for(uint8_t i = DATA_START_INDEX; i < LENGTH_RX - 1; i++) {
                if (!Prog.progChip(serialBuffer[i], false)) {
                    Serial.println("Error: Programming failed at byte index " + String(i-2));
                    state = IDLE;
                    return;
                }
                else 
                    Serial.write(ACK); 
            }
            break;
        case VERIFY:
        for(uint8_t i = DATA_START_INDEX; i < LENGTH_RX - 1; i++) {
                if (!Prog.verifyChip(serialBuffer[i])) {
                    Serial.println("Error: Verify failed at byte index " + String(i-2));
                    state = IDLE;
                    return;
                }
                else 
                    Serial.write(ACK); 
            }
            break;
        default:
            state = IDLE;
            break;
    }
}
