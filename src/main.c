/** 
 *  @file main.c
 *  @brief main file to initialize components and initiate main loop
 *
 *  @author Thomas Evison
 *
 *  @date 28/10/2020
 */

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/
#include "configbits.h"
#include <xc.h>            /* XC8 General Include File */
#include <stdint.h>        /* For uint8_t definition */
#include "hardware.h"
#include <stdbool.h>       /* For true/false definition */
#include "system.h"        /* System funct/params, like osc/peripheral config */
#include "pygmy.h"         /* User funct/params */
#include "eusart.h"
#include <string.h>

void menuRun(void);

/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/

/**
*  Main entry point of program
*  @author Thomas Evison
*  @date 28/10/2020
*/
void main(void) {
    /* setup hardware */
    Hardware_ConfigureOscillator(); // Configure the oscillator for the device
    Hardware_initIO(); // Initialize I/O and Peripherals for application
    Hardware_initUART(); // init hardware UART signals
    EUSART_Initialize();

    /* flash LED to prove function */
    int i;
    for (i = 0; i < 5; i++) {
        DOUT_CAM_S = !DOUT_CAM_S; // invert output
        __delay_ms(500);
    }
    DOUT_CAM_S = false;

    menuRun();

    enum State {
        ST_IDLE, ST_INMSG, ST_AFTRESC
    };
    enum State state = ST_IDLE;

    uint8_t rxData;
    bool endOfFrame;
    int rxDataArrayIndex;
    uint8_t rxDataArray[16] = {0};
    
    /* for now, lets set the defaults.. */
    Pygmy_SetDefaultValues();

    /* enter main loop and wait for interrupt from PIR sensor */
    while (true) {

        if (!DIN_USB_ON) {

            if (EUSART_is_rx_ready()) {
                rxData = EUSART_Read();
                if (EUSART_is_tx_ready()) {
                    EUSART_Write(rxData); // echo received byte
                }

                /* UART buffer state machine */
                switch (rxData) {
                    case 0x50: // Begin of Transmission 0x50 = ASCI 'P'
                        if (state == ST_IDLE) {
                            rxDataArrayIndex = 0;
                            state = ST_INMSG;
                        }
                        break;
                    case 0x7D: // Escape Character - send this before a '0x04'
                        if (state == ST_INMSG) {
                            state = ST_AFTRESC;
                        } else if (state == ST_AFTRESC) {
                            state = ST_INMSG;
                        }
                        break;
                    case 0x04: // End of Transmission
                        if (state == ST_INMSG) {
                            endOfFrame = true;
                            state = ST_IDLE;
                        } else if (state == ST_AFTRESC) {
                            state = ST_INMSG;
                        }
                        break;
                    default:   // clear AFTRESC if required
                        if (state == ST_AFTRESC) {
                            state = ST_INMSG;
                        }
                        break;
                }

                /* add data to our frame */
                if (state == ST_INMSG) {
                    rxDataArray[rxDataArrayIndex++] = rxData;
                    if (rxData > 15) rxData = 0; // try not to overflow
                }

                /* handle complete messages */
                if (endOfFrame) {
                    uint8_t *response;

                    /* send response */
                    response = Pygmy_handleMsg(rxDataArray);
                    for (int i = 0; i < 16; i++) {
                        EUSART_Write(*(response + i));
                    }

                    /* clear the array */
                    for (int i = 0; i < 16; i++) {
                        rxDataArray[i] = 0;
                    }
                    rxDataArrayIndex = 0;

                    /* clear end frame flag */
                    endOfFrame = false;
                }
            }

        
        } else {
            EUSART_WriteString("Sleep\n");
            SLEEP(); // wake on PIR interrupt (or other interrupt))
        }
    }

}

/**
 *  Outputs a brief message to serial
 *  @author Thomas Evison
 *  @date 28/10/2020
 */
void menuRun() {
    /* introduce yourself */
    EUSART_WriteString("PygmyPossum v1.1 \n");
    EUSART_WriteString("Please enter a CMD: \n");

}

