#include <Arduino.h>
#include <Prog.h>

#define BAUD_RATE 115200
#define ACK 0x06


#define SERIAL_BUFFER_SIZE 64
uint8_t serialBuffer[SERIAL_BUFFER_SIZE]; // [cmd, length, data..., checksum]
uint8_t checksum = 0;

#define CMD serialBuffer[0]
#define LENGTH_RX serialBuffer[1]
#define DATA_START_INDEX 2
#define CHECKSUM_RX serialBuffer[SERIAL_BUFFER_SIZE - 1]

#define ROM_SIZE 2048 // 2KB for AT89C2051


enum State {IDLE, RECEIVING_DATA, ERASE, PROGRAM, VERIFY, MEM_DUMP};
State state = IDLE;
Prog prog;

//fills the buffer and returns the checksum
uint8_t readIncomingPacket(uint8_t serialBuffer[], uint8_t len){
    uint8_t i = 0;
    uint8_t cs = 0;

    while (i < len) {
        if(Serial.available()){
            serialBuffer[i] = Serial.read();
            if(i < SERIAL_BUFFER_SIZE - 1)
                cs += serialBuffer[i]; 
            i++; 
        } 
    }
    return cs;
}

//sends back the received package usefull for debug

void returnPacket(uint8_t serialBuffer[], uint8_t len){
    Serial.print("Received package: ");
            for(int i=0; i<SERIAL_BUFFER_SIZE; i++) 
                    Serial.print(String(serialBuffer[i],HEX));
            Serial.println();
}




void setup() {
    Serial.begin(BAUD_RATE);
    
    if (!prog.init())
        return;

    Serial.println("Ready!");
}

void loop() {
    
    switch (state){
        case IDLE:
            
            while(Serial.available()<1);
            state = RECEIVING_DATA;
            break;

        case RECEIVING_DATA:  
            checksum = readIncomingPacket(serialBuffer, SERIAL_BUFFER_SIZE);
            //returnPacket(serialBuffer, SERIAL_BUFFER_SIZE);        
    
            // Validate checksum
            if(checksum != CHECKSUM_RX) {
                Serial.println("Error: Checksum mismatch.");
                Serial.println("Received: " + String(CHECKSUM_RX, HEX) + ", Calculated: " + String(checksum, HEX));
                state = IDLE;
                return;}

            // Acknowledge receipt and validation
            Serial.println(ACK); 
            
            // Process command
            switch (CMD){
                case 'p':   state = PROGRAM;    break;
                case 'e':   state = ERASE;      break;
                case 'v':   state = VERIFY;     break;
                case 'm':   state = MEM_DUMP;   break;
                default:    state = IDLE;       break;
            }
            
            break;
        case ERASE:
            
            if (prog.eraseChip())
                Serial.println("Chip erased");
            else
                Serial.println("Error: Chip erase failed."); 
                
            state = IDLE;
            break;
        case PROGRAM:

            
            for(int i = DATA_START_INDEX; i < (LENGTH_RX + DATA_START_INDEX); i++) {
                if (!prog.progChip(serialBuffer[i], false)) {
                    Serial.println("Error: Programming failed at byte index " + String(i-2));
                    state = IDLE;
                    return;
                }
                /*
                else {
                    Serial.print(String(serialBuffer[i], HEX)); // for debug
                } 
                */     
            }
            //Serial.println();
                 
            Serial.println(ACK);

            state = IDLE;
            break;
        case VERIFY:
            
            for(uint8_t i = DATA_START_INDEX; i < LENGTH_RX - 1; i++) {
                if (!prog.verifyChip(serialBuffer[i])) {
                    Serial.println("Error: Verify failed at byte index " + String(i-2));
                    state = IDLE;
                    return;} 
                }

            Serial.println(ACK);        
            state = IDLE;
            break;

        case MEM_DUMP:
            Serial.println("Memory: ");
            for(uint16_t i = 0; i < ROM_SIZE; i++) {
                if( i % 16 == 0){
                    Serial.println();
                    Serial.print(String(i,HEX) + ": ");
                }

                Serial.print(String(prog.readChip(), HEX) + ' ');
            }

            Serial.println();

            Serial.println(ACK);
            state = IDLE;
            break;
        default:
            state = IDLE;
            break;
    }
}
