/******************************************************************************/
/*Files to Include                                                            */
/******************************************************************************/
#include <xc.h>            /* XC8 General Include File */
#include <stdint.h>        /* For uint8_t definition */
#include <stdbool.h>       /* For true/false definition */
#include "system.h"

/******************************************************************************/
/* Hardware_ConfigureOscillator() -                                     */
/******************************************************************************/
void Hardware_ConfigureOscillator(){
    /* TODO Add clock switching code if appropriate.  */

    /* Typical actions in this function are to tweak the oscillator tuning
    register, select new clock sources, and to wait until new clock sources
    are stable before resuming execution of the main project. */
}

/******************************************************************************/
/* Hardware_initIO() -                                               */
/******************************************************************************/
void Hardware_initIO(){
    
    /* setup I/O registers */
    TRISA = 0B00101111;  // set PORTA directions
    WPUA = TRISA;        // weak pullups on all inputs
    ANSELA = 0x00;       // Pin is assigned Digital I/O
    PORTA = 0x00;        // set all outputs false
    
    /* set interrupts */
    IOCAP = 0B00100000;  // look for rising edge on RA5
    IOCAN = 0x00;        // ignore all falling edge
    GIE = true;          // enable global interrupts
    PEIE = true;         // enable peripheral interrupts
    INTEDG = true;       // look for rising edge on Interrupt pin
    PIE0bits.IOCIE = true; // Interrupt-on-Change Interrupt Enable bit
    PIE1 = 0x00;         // turn off all other interrupts
    PIE2 = 0x00;
    PIE3 = 0x00;
    PIE4 = 0x00;
}
