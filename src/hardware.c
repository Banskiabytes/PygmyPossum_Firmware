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
/* Hardware_init_IO() -                                               */
/******************************************************************************/
void Hardware_initIO(){
    TRISA = 0B00101111;  // set PORTA directions
    WPUA = TRISA;        // weak pullups on all inputs
    ANSELA = 0x00;       // Pin is assigned Digital I/O
    PORTA = 0x00;        // set all outputs false
}
