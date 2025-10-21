#include "PROG.h"
#include <Arduino.h>

bool Prog::init() {

    setDataPortOutput();
    setPortDirection(PORT_ID_C, PORTC_DIR_MASK, OUT); 
    writePortData(PORT_ID_C, PORTC_DIR_MASK, PORTC_DIR_MASK); //Pull A0-A5 high
    writePortData(PORT_ID_D, PORTD_DIR_MASK, PORTD_DIR_MASK); // Pull D2-D7 high
    writePortData(PORT_ID_B, PORTB_DIR_MASK, PORTB_DIR_MASK); // Pull D8-D11 high
    setRST(RST_HIGH); 

    return true; 
}

bool Prog::eraseChip() {
    setEraseMode();
    setRST(RST_12V); 
    pulsePin(PORT_ID_C, PROGn);
    delay(15); // Wait for the erase operation to complete
    pulsePin(PORT_ID_C, PROGn); 
    setRST(RST_HIGH); 
    return true; 
}

bool Prog::progChip(uint8_t data, bool verify) {

    setProgMode();

    writePortData(PORT_ID_D, PORTD_DIR_MASK, data << 4);
    writePortData(PORT_ID_B, PORTB_DIR_MASK, data >> 4); 

    pulsePin(PORT_ID_C, PROGn); 
    
    while(!(RDY_BSYn & readPortData(PORT_ID_D, RDY_BSYn))); //poll RDY/nBSY pin

    /*
    if(verify) {
        setVerifyMode();
        if(!verifyChip(data)) return false; 
        setProgMode();}
    */
    
    //ensure 1us delay
    __asm__ __volatile__("nop");
    __asm__ __volatile__("nop");

    pulsePin(PORT_ID_C, XTAL); // Next byte in ROM
    

    return true;
}

bool Prog::verifyChip(uint8_t data) {

    setVerifyMode();
    
    uint8_t readData =  readPortData(PORT_ID_D, 0xF0); // Read data from P1.0-P1.7
    readData = ((readData & 0xF0)>>4)  | (readPortData(PORT_ID_B, PORTB_DIR_MASK) << 4); // Combine with P1.4-P1.7
    
    pulsePin(PORT_ID_C, XTAL);

    Serial.println(data,HEX);
    
    if (readData != data) 
        return false; 
    else
        return true; 
}

void Prog::rst() {
    setRST(RST_LOW);
    delay(1);
    setRST(RST_HIGH);
    
}

//-------------------Utility functions---------------------

void Prog::setDataPortOutput(){
    // Set P1.0-P1.7 as output
    setPortDirection(PORT_ID_D, PORTD_DIR_MASK, OUT); // Set D4-D7 as output
    setPortDirection(PORT_ID_B, PORTB_DIR_MASK, OUT); // Set D8-D11 as output
  
}

void Prog::setDataPortInput(){
    // Set P1.0-P1.7 as input
    setPortDirection(PORT_ID_D, PORTD_DIR_MASK - 0x0C, IN);
    setPortDirection(PORT_ID_B, PORTB_DIR_MASK, IN);
    
}

void Prog::setPortDirection(Port port, uint8_t mask, PortDir dir) {
    if(dir == IN) {
        if(port == PORT_ID_D) {DDRD |= mask;}
        else if(port == PORT_ID_B) {DDRB |= mask;} 
        else if(port == PORT_ID_C) {DDRC |= mask;}} 
    else {
        if(port == PORT_ID_D) {DDRD &= ~mask;}
        else if(port == PORT_ID_B) {DDRB &= ~mask;} 
        else if(port == PORT_ID_C) {DDRC &= ~mask;}}
    
    __asm__ __volatile__("nop");
}

void Prog::writePortData(Port port, uint8_t mask, uint8_t data) {

    if (port == PORT_ID_D){
        PORTD = (PORTD & ~mask) | (data & mask);}
    else if (port == PORT_ID_B){
        PORTB = (PORTB & ~mask) | (data & mask);}
    else if(port == PORT_ID_C) {
        PORTC = (PORTC & ~mask) | (data & mask);}
    else {
        Serial.println("Invalid port specified for writePortData.");}

    __asm__ __volatile__("nop"); //wait 1 clock cycle
}

uint8_t Prog::readPortData(Port port, uint8_t mask) {

    setPortDirection(port, mask, IN);
    
    if (port == PORT_ID_D) {
        return PIND;} 
    else if (port == PORT_ID_B) {
        return PINB;}
    else if (port == PORT_ID_C) {
        return PINC;}
    else 
        return 0; // Invalid port
}

void Prog::pulsePin(Port port, uint8_t mask) {

    setPortDirection(port, mask, IN);
    uint8_t data  = readPortData(port, mask); // Ensure the port is read before pulsing
    setPortDirection(port, mask, OUT);

    writePortData(port, mask, ~data);
    writePortData(port, mask, data);
 
}

//TODO: Edit the mask after final schematic is ready
void Prog::setRST(RstState state) {

if(state == RST_HIGH)
    writePortData(PORT_ID_D, D2_MASK | D3_MASK, 0x00); 
else if(state == RST_LOW)
    writePortData(PORT_ID_D, D2_MASK | D3_MASK, 0x00); 
else if(state == RST_12V)
    writePortData(PORT_ID_D, D2_MASK | D3_MASK, 0x00);
    
}

void Prog::setProgMode() {
    
    setDataPortOutput();
    setPortDirection(PORT_ID_D, RDY_BSYn, IN);
    writePortData(PORT_ID_C, PORTC_DIR_MASK, PROGn | P3_4 | P3_5 | P3_7); // Set A0, A2, A3, A4 high
    setRST(RST_12V); // Set RST to 12V
    
}

void Prog::setVerifyMode() {
    
    setRST(RST_HIGH); // Set RST to high
    writePortData(PORT_ID_C, PORTC_DIR_MASK, PROGn  | P3_5 | P3_7); // Set A0, A3, A4 high
    setDataPortInput();
}

void Prog::setEraseMode() {
    writePortData(PORT_ID_C, PORTC_DIR_MASK, PROGn | P3_3 ); // Set A0, A1, A2, A3, A4 high
    setRST(RST_12V); // Set RST to 12V
}