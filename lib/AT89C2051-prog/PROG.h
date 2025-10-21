
#ifndef PROG_H
#define PROG_H

#ifndef ARDUINO_H
#include <Arduino.h>
#endif


/*
D2, D3 -> RST/VPP , D2 low -> RST = 5V, D3 high -> RST = 12V

A0 - P3.2
A1 - P3.3
A2 - P3.4
A3 - P3.5
A4 - P3.7
A5 - XTAL1
*/


#define PORTD_DIR_MASK 0xFC // Digital pins D2 to D7 direction
#define PORTB_DIR_MASK 0x0F // Digital pins D8 to D11 direction
#define PORTC_DIR_MASK 0x3F // A0 to A5 data pins

// PORT D masks -----------------
#define RDY_BSYn 0x02 // D1 for RDY/nBSY (active low)
#define D2_MASK 0x04 
#define D3_MASK 0x08 
#define P1_0 0x10 // D4 for P1.0 (active high)
#define P1_1 0x20 // D5 for P1.1 (active high)
#define P1_2 0x40 // D6 for P1.2 (active high)
#define P1_3 0x80 // D7 for P1.3 (active high)

// PORT B masks -----------------
#define P1_4 0x01 // D8 for P1.4 (active high)
#define P1_5 0x02 // D9 for P1.5 (active high)
#define P1_6 0x04 // D10 for P1.6 (active high)
#define P1_7 0x08 // D11 for P1.7 (active high)


// PORT C masks -----------------
#define PROGn 0x01 // A0 for P3.2 (active low)
#define P3_3 0x02 // A1 for P3.3 (active high)
#define P3_4 0x04 // A2 for P3.4 (active high)
#define P3_5 0x08 // A3 for P3.5 (active high)
#define P3_7 0x10 // A4 for P3.7 (active high)
#define XTAL 0x20 // A5 for XTAL1 (active high)
//------------------------------



// Port identifiers
// These identifiers are used to specify which port to operate on
// when calling functions like writePortData, readPortData, etc.
// If you use the ports directly you can have issues in logic statements
// Use the PortId enum to specify the port you want to operate on.

enum Port {
    PORT_ID_D,
    PORT_ID_B,
    PORT_ID_C
};

enum PortDir{
    IN,
    OUT
};

enum RstState{
    RST_LOW,
    RST_HIGH,
    RST_12V
};
 

struct Prog{
   
    bool init(); 

    bool eraseChip(); 

    bool progChip(uint8_t data, bool verify); 
    
    bool verifyChip(uint8_t data); 

    void rst(); 


    //utility functions
    void setDataPortOutput(); // QUICKLY SET D4-D11 AS OUTPUT
    void setDataPortInput(); // QUICKLY SET D4-D11 AS INPUT

    void setPortDirection(Port port, uint8_t mask, PortDir direction); 
    void writePortData(Port port, uint8_t mask, uint8_t data); 
    uint8_t readPortData(Port port, uint8_t mask);

    void pulsePin(Port port, uint8_t mask);
    
    void setRST(RstState state);


    void setProgMode(); 
    void setVerifyMode(); 
    void setEraseMode(); 


};





#endif