#include <xc.h>            /* XC8 General Include File */
#include <stdint.h>        /* For uint8_t definition */
#include "hardware.h"



/*******************************************************************************
 * Pygmy_readDipSwitches() - reads values from dip switches and returns value as byte
 *******************************************************************************/
uint8_t Pygmy_readDipSwitches(){
    
    // values inverted for active low switch w/ pull-up resistor
    uint8_t dipSetting = 0;
    dipSetting |= !DIP1_PIN << 0;
    dipSetting |= !DIP2_PIN << 1;
    dipSetting |= !DIP3_PIN << 2;
    return dipSetting;
}

/*******************************************************************************
 * delay_ms() - ms delay funciton
 *******************************************************************************/
void Pygmy_delay_ms(uint16_t milliseconds) {

    /* crappy delay function to overcome __delay_ms() constraint
     * of "inline delay argument must be constant"  */

    while (milliseconds > 0) {
        __delay_ms(1);
        milliseconds--;
    }
}
