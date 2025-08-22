
#ifndef AT89C2051_PROG_H
#define AT89C2051_PROG_H

#ifndef ARDUINO_H
#include <Arduino.h>
#endif


/*
D4 - D11 --> P1.0 - P1.7
D2, D3 -> RST/VPP , D2 low -> RST = 5V, D3 high -> RST = 12V

A0 - P3.2
A1 - P3.3
A2 - P3.4
A3 - P3.5
A4 - P3.7
A5 - XTAL1
*/

#define RST_VPP_DIR_MASK 0x0C // Digital pins D2 and D3 direction
#define PORTD_DIR_MASK 0xF0 // Digital pins D4 to D7 direction
#define PORTB_DIR_MASK 0x0F // Digital pins D8 to D11 direction
#define PORTC_DIR_MASK 0x3F // A0 to A5 data pins

 

// PORT D masks -----------------
#define D2_MASK 0x04 // D2 for P1.4 (active low)
#define D3_MASK 0x08 // D3 for P1.5 (active high)
#define D4_MASK 0x10 // D4 for P1.0 (active high)
#define D5_MASK 0x20 // D5 for P1.1 (active high)
#define D6_MASK 0x40 // D6 for P1.2 (active high)
#define D7_MASK 0x80 // D7 for P1.3 (active high)

// PORT B masks -----------------
#define D8_MASK 0x01 // D8 for P1.4 (active high)
#define D9_MASK 0x02 // D9 for P1.5 (active high)
#define D10_MASK 0x04 // D10 for P1.6 (active high)
#define D11_MASK 0x08 // D11 for P1.7 (active high)


// PORT C masks -----------------
#define A0_MASK 0x01 // A0 for P3.2 (active low)
#define A1_MASK 0x02 // A1 for P3.3 (active high)
#define A2_MASK 0x04 // A2 for P3.4 (active high)
#define A3_MASK 0x08 // A3 for P3.5 (active high)
#define A4_MASK 0x10 // A4 for P3.7 (active high)
#define A5_MASK 0x20 // A5 for XTAL1 (active high)
//------------------------------


// Port identifiers
// These identifiers are used to specify which port to operate on
// when calling functions like writePortData, readPortData, etc.
//If you use the ports directly you can have issues in logic statements
// Use the PortId enum to specify the port you want to operate on.
enum PortId {
    PORT_ID_D,
    PORT_ID_B,
    PORT_ID_C
};

struct AT89C2051Prog{
   
    bool init(); 

    bool eraseChip(); 

    bool progChip(uint8_t data); 
    
    bool verifyChip(uint8_t data); 

    void RST(); // Move cursor to top of ROM


    //utility functions
    private:
    void setDataPinsOutput(); // Set P1.0-P1.7 as output, quickly sets D4-D11 as output
    void setDataPinsInput(); // Set P1.0-P1.7 as input, quickly sets D4-D11 as input

    void setPortDirection(PortId port, uint8_t mask, bool output); 
    void writePortData(PortId port, uint8_t mask, uint8_t data); 

    uint8_t readPortData(PortId port);

    void pulsePin(PortId port, uint8_t mask);
    
    void setRSTHigh(); 
    void setRSTLow(); 
    void setRST12V(); 


    void setProgMode(); 
    void setVerifyMode(); 
    void setEraseMode(); 


};





#endif