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

/******************************************************************************/
/* Interrupt Routines                                                         */

/******************************************************************************/
void __interrupt() isr(void) {

    /* Determine which flag generated the interrupt */
    if (IOCIF && IOCAF5) {
        GIE = false; // disable global interrupts
        IOCAF5 = false; // Clear Interrupt Flag
        Pygmy_TriggeredPIR(); // Fire Camera
        GIE = true; // enable global interrupts
    }
    // interrupt handler
    if(INTCONbits.PEIE == 1)
    {
        if(PIE1bits.TXIE == 1 && PIR1bits.TXIF == 1)
        {
            EUSART_TxDefaultInterruptHandler();
        } 
        else if(PIE1bits.RCIE == 1 && PIR1bits.RCIF == 1)
        {
            EUSART_RxDefaultInterruptHandler();
        } 
        else
        {
            //Unhandled Interrupt
        }
    }      
    else
    {
        //Unhandled Interrupt
    }

}







