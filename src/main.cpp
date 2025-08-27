#include <Arduino.h>
#include "../lib/AT89C2051-prog/AT89C2051-prog.h"

#define SERIAL_BUFFER_SIZE 128
#define ACK 0x06

AT89C2051Prog Prog;

uint8_t serialBuffer[SERIAL_BUFFER_SIZE];
uint8_t bufferIndex = 0;
uint8_t expectedLen = 0;
bool receivingData = false;
bool progMode = false;
bool allMatch = true;
unsigned long receiveStartTime = 0;
const char* data = ":050000007590AA80FECE:00000001FF";

bool receiveBytes();



void setup() {

    Serial.begin(9600);
    
    if (!Prog.init()) {
        Serial.println("Failed to initialize AT89C2051 programmer.");
        return;
    } else 
        Serial.println("AT89C2051 programmer initialized successfully.");
}

void loop() {

    if(receivingData){
        //Check for timeout
        if (millis() - receiveStartTime > 2000) {
            Serial.println("Error: Data receive timeout.");
            receivingData = false;
            bufferIndex = 0;
            return;}


        // Read incoming data
        while (Serial.available() && bufferIndex < expectedLen) {
            serialBuffer[bufferIndex++] = Serial.read();}
        

        // If we've received the expected length, process the data
        if (bufferIndex >= expectedLen) {

            Serial.println(ACK);

            uint8_t checksum = 0;
            for (uint8_t i = 0; i < expectedLen - 1; i++) checksum += serialBuffer[i];

            if (checksum != serialBuffer[expectedLen - 1]) {
                Serial.println("Error: Checksum mismatch.");} 
            else 
                Serial.println("Checksum OK");


            if(progMode) {
                for (uint8_t i = 0; i < expectedLen - 1; i++) 
                    if (!Prog.progChip(serialBuffer[i])){
                        Serial.println("Program OK"); 
                        break;} }
            else {
                allMatch = true;
                for (uint8_t i = 0; i < expectedLen - 1; i++) {
                    if (!Prog.verifyChip(serialBuffer[i])) {
                        Serial.println("Verification failed");
                        allMatch = false;
                        break;}
                }
                if (allMatch) 
                    Serial.println("Verification OK");
            }
            receivingData = false;
            bufferIndex = 0;
        }
        return;

    }

    if(Serial.available()) {

        char c = Serial.read();
        switch(c) {
            case 'e': //erase
                receivingData = false;
                if (Prog.eraseChip()) 
                    Serial.println(ACK);
            break;

            case 'p': // 'p' for program
                if(!receiveBytes())
                  break;
                progMode = true;
            break;

            case 'v': // 'v' for verify
                allMatch = true;
                if(!receiveBytes())
                    break;
                progMode = false;
            break;

            case 'd': // 'd' for data upload
                receiveBytes();
                break;

            default:
                Serial.println("Unknown command. Use 'e' to erase, 'p' to program, 'v' to verify., 'd' for data");
                Serial.println();
                break;
        }
    
    }
    

}

bool receiveBytes() {

    while (!Serial.available());
    expectedLen = Serial.read();

    if (expectedLen < 2 || expectedLen > SERIAL_BUFFER_SIZE) 
        return false;
    
    bufferIndex = 0;
    receivingData = true;
    receiveStartTime = millis();   
    
    return true;
}