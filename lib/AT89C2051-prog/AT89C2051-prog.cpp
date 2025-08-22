#include "AT89C2051-prog.h"
#include <Arduino.h>

bool AT89C2051Prog::init() {

    setDataPinsOutput();
    setPortDirection(PORT_ID_D, RST_VPP_DIR_MASK, true);
    setPortDirection(PORT_ID_C, PORTC_DIR_MASK, true); 
    writePortData(PORT_ID_C, PORTC_DIR_MASK, PORTC_DIR_MASK); 
    writePortData(PORT_ID_D, PORTD_DIR_MASK, 0xF0);
    writePortData(PORT_ID_B, PORTB_DIR_MASK, 0x0F);
    setRSTHigh(); 

    return true; 
}

bool AT89C2051Prog::eraseChip() {
    setEraseMode();
    setRST12V(); 
    //delay(1);
    pulsePin(PORT_ID_C, A0_MASK);
    delay(15); // Wait for the erase operation to complete
    pulsePin(PORT_ID_C, A0_MASK); 
    setRSTHigh(); 
    return true; 
}

bool AT89C2051Prog::progChip(uint8_t data) {

    setProgMode();

    writePortData(PORT_ID_D, PORTD_DIR_MASK, data << 4);
    writePortData(PORT_ID_B, PORTB_DIR_MASK, data >> 4); 

    delayMicroseconds(1200); // Wait for the data to be set

    pulsePin(PORT_ID_C, A0_MASK); 
    delayMicroseconds(120); 
    pulsePin(PORT_ID_C, A0_MASK);

    delay(3);

    pulsePin(PORT_ID_C, A5_MASK); // Next byte in ROM
    delayMicroseconds(10); 
    pulsePin(PORT_ID_C, A5_MASK);

    return true;
}

bool AT89C2051Prog::verifyChip(uint8_t data) {
    setVerifyMode();
    delayMicroseconds(100);
    setDataPinsInput(); 
    uint8_t readData =  readPortData(PORT_ID_D); // Read data from P1.0-P1.7
    readData = ((readData & 0xF0)>>4)  | (readPortData(PORT_ID_B) << 4); // Combine with P1.4-P1.7
    
    pulsePin(PORT_ID_C, A5_MASK);
    pulsePin(PORT_ID_C, A5_MASK);  //next byte in ROM
    
    setDataPinsOutput();
    Serial.println("Read Data: ");
    Serial.println(readData, HEX);

    if (readData != data) 
        return false; 
    else
        return true; 
}

void AT89C2051Prog::RST() {
    setRSTLow(); // Set RST low
    delay(1000); // Wait for a short time
    setRSTHigh(); // Set RST high
    delayMicroseconds(500); // Wait for a short time
}
//-------------------Utility functions---------------------

void AT89C2051Prog::setDataPinsOutput(){
    // Set P1.0-P1.7 as output
    setPortDirection(PORT_ID_D, PORTD_DIR_MASK, true); // Set D4-D7 as output
    setPortDirection(PORT_ID_B, PORTB_DIR_MASK, true); // Set D8-D11 as output
}

void AT89C2051Prog::setDataPinsInput(){
    // Set P1.0-P1.7 as input
    setPortDirection(PORT_ID_D, PORTD_DIR_MASK, false);
    setPortDirection(PORT_ID_B, PORTB_DIR_MASK, false);
}

void AT89C2051Prog::setPortDirection(PortId port, uint8_t mask, bool output) {
    if(output) {
        if(port == PORT_ID_D) {DDRD |= mask;}
        else if(port == PORT_ID_B) {DDRB |= mask;} 
        else if(port == PORT_ID_C) {DDRC |= mask;}} 
    else {
        if(port == PORT_ID_D) {DDRD &= ~mask;}
        else if(port == PORT_ID_B) {DDRB &= ~mask;} 
        else if(port == PORT_ID_C) {DDRC &= ~mask;}}
}

void AT89C2051Prog::writePortData(PortId port, uint8_t mask, uint8_t data) {
    if (port == PORT_ID_D){
        PORTD = (PORTD & ~mask) | (data & mask);}
    else if (port == PORT_ID_B){
        PORTB = (PORTB & ~mask) | (data & mask);}
    else if(port == PORT_ID_C) {
        PORTC = (PORTC & ~mask) | (data & mask);}
    else {
        Serial.println("Invalid port specified for writePortData.");}
}

uint8_t AT89C2051Prog::readPortData(PortId port) {
    if (port == PORT_ID_D) {
        return PIND;} 
    else if (port == PORT_ID_B) {
        return PINB;}
    else if (port == PORT_ID_C) {
        return PINC;}
    else 
        return 0; // Invalid port
}

void AT89C2051Prog::pulsePin(PortId port, uint8_t mask) {

    uint8_t data  = readPortData(port); // Ensure the port is read before pulsing

    if(data & mask) 
        writePortData(port, mask, 0); // Set pin low
    else 
        writePortData(port, mask, mask); // Set pin high
 
}

void AT89C2051Prog::setRSTHigh() {
    writePortData(PORT_ID_D, RST_VPP_DIR_MASK, 0x00); // Set D3 low and D2 low
}
void AT89C2051Prog::setRSTLow() {
    writePortData(PORT_ID_D, RST_VPP_DIR_MASK, D2_MASK); // Set D3 low and D2 high
}
void AT89C2051Prog::setRST12V() {
    writePortData(PORT_ID_D, RST_VPP_DIR_MASK, D2_MASK | D3_MASK ); // Set D3 high and D2 HIGH
 }

void AT89C2051Prog::setProgMode() {
    writePortData(PORT_ID_C, PORTC_DIR_MASK, A0_MASK  | A2_MASK | A3_MASK | A4_MASK); // Set A0, A2, A3, A4 high
    setRST12V(); // Set RST to 12V
}

void AT89C2051Prog::setVerifyMode() {
    writePortData(PORT_ID_C, PORTC_DIR_MASK, A0_MASK  | A3_MASK | A4_MASK); // Set A0, A1, A2, A3, A4 high
    setRSTHigh(); // Set RST to high
}

void AT89C2051Prog::setEraseMode() {
    writePortData(PORT_ID_C, PORTC_DIR_MASK, A0_MASK | A1_MASK ); // Set A0, A1, A2, A3, A4 high
    setRST12V(); // Set RST to 12V
}