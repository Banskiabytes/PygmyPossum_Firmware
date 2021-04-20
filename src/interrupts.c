/** 
 *  @file interrupts.c
 *  @brief interrupt handler for all interrupt triggers
 *
 *  @author Thomas Evison
 *  @date 28/10/2020
 */

/******************************************************************************/
/*Files to Include                                                            */
/******************************************************************************/
#include <xc.h>         /* XC8 General Include File */
#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */
#include "pygmy.h"
#include <string.h>
#include "eusart.h"

void UART_sendString(char[]);
void UART_sendByte(uint8_t);

volatile extern uint8_t array[16];

/**
 *  Interrupt handler - first checks which flags have been set, then will call
 * corresponding interrupt subroutine
 *  @author Thomas Evison
 *  @date 28/10/2020
 */
void __interrupt() isr(void) {

    /* interrupt handler */
    /* Determine which flag generated the interrupt */
    
    // falling edge on PIR sensor
    if (IOCIF && IOCAF5) {
        GIE = false;              // disable global interrupts
        IOCAF5 = false;           // Clear Interrupt Flag
        Pygmy_TriggeredPIR();     // Fire Camera
        GIE = true;               // enable global interrupts
    }
    
    // falling edge on TP1 (test pin - set default values)
    if(IOCIF && IOCAF4) {
        GIE = false;              // disable global interrupts
        IOCAF4 = false;           // Clear Interrupt Flag
        Pygmy_SetDefaultValues(); // write default values to EEPROM
        GIE = true;               // enable global interrupts
    }
    
    if (INTCONbits.PEIE == 1) {
        if (PIE1bits.TXIE == 1 && PIR1bits.TXIF == 1) {
            EUSART_TxDefaultInterruptHandler();
        }
        else if (PIE1bits.RCIE == 1 && PIR1bits.RCIF == 1) {
            EUSART_RxDefaultInterruptHandler();
        }
        else {
            //Unhandled Interrupt
        }
    }
    else {
        //Unhandled Interrupt
    }

}







