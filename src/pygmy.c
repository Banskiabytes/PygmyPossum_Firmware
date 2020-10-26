#include <xc.h>            /* XC8 General Include File */
#include <stdint.h>        /* For uint8_t definition */
#include <stdlib.h>        /* For uint8_t definition */
#include "hardware.h"
#include <stdbool.h>       /* For true/false definition */
#include <string.h>

#define USRPROG_START_MEM (uint8_t)128 /* address byte of EEPROM for user programs

/* user defined types */
struct {
    bool wakeCamera;        // wake camera after PIR event
    uint8_t numOfSnaps;     // amount of pics to take after PIR event
    uint16_t snapPeriod;    // ms - frequency of burst after event
    uint8_t minEventPeriod; // s - minimum time before another shutter is trig
} camParams;

/*******************************************************************************
 * Pygmy_readDipSwitches() - reads values from dip switches and returns value as byte
 *******************************************************************************/
uint8_t Pygmy_readDipSwitches() {

    // values inverted for active low switch w/ pull-up resistor
    uint8_t dipSetting = 0;
    //dipSetting |= !DIP1_PIN << 0;
    //    dipSetting |= !DIP2_PIN << 1;
    dipSetting |= !DIP3_PIN << 0;
    return dipSetting;
}

/*******************************************************************************
 * delay_ms() - ms delay funciton
 ******************************************************************************/
void Pygmy_delay_ms(uint16_t milliseconds) {

    /* crappy delay function to overcome __delay_ms() constraint
     * of "inline delay argument must be constant"  */
    while (milliseconds > 0) {
        __delay_ms(1);
        milliseconds--;
    }
}

/*******************************************************************************
 * Pygmy_setCamParams() - setup camera timing parameters
 ******************************************************************************/
void Pygmy_setCamParams(uint8_t dipSetting) {
    
    uint8_t usrProgStartAddress = USRPROG_START_MEM + dipSetting*8;
    
    /* get the camera parameters from EEPROM */
    camParams.numOfSnaps = eeprom_read(usrProgStartAddress + 1);
    
    /* snap period is stored across 2 bytes */
    uint8_t snapPeriodLSB = eeprom_read(usrProgStartAddress + 2);
    uint8_t snapPeriodMSB = eeprom_read(usrProgStartAddress + 3);
    camParams.snapPeriod = (snapPeriodMSB << 8) | snapPeriodLSB;
    
    camParams.minEventPeriod = eeprom_read(usrProgStartAddress + 4);
}

/*******************************************************************************
 * Pygmy_handleMsg() - handle UART messages
 *******************************************************************************/
uint8_t * Pygmy_handleMsg(uint8_t cmd[]) {

    /* start crafting the response message */
    uint8_t response[16] = {0};
    response[0] = 'R';    // standard response flag
    response[1] = cmd[1]; // return the cmd code

    /* get frame command */
    switch (cmd[1]) {
        case 'A': // get usr prog
            response[2] = cmd[2]; 
            for (int i = 0; i < 8; i++) {
                response[3+i] = (eeprom_read(USRPROG_START_MEM + cmd[2]*8 + i));
            }
            break;
        case 'B': // set usr prog
            response[2] = cmd[2]; 
            for (int i = 0; i < 8; i++) {
                eeprom_write(USRPROG_START_MEM + cmd[2]*8 + i , cmd[3+i]);
            }
            break;
        case 'D': // get dip switches
            response[2] = Pygmy_readDipSwitches();
            break;
        case 'V': // set default usrProg values
            for (int i = 0; i < 16; i++) {
                eeprom_write(USRPROG_START_MEM + (i * 8) + 0, 0x00); // config byte
                eeprom_write(USRPROG_START_MEM + (i * 8) + 1, 0x03); // numOfSnaps
                eeprom_write(USRPROG_START_MEM + (i * 8) + 2, 0xE8); // snapPeriod LSB
                eeprom_write(USRPROG_START_MEM + (i * 8) + 3, 0x03); // snapPeriod MSB
                eeprom_write(USRPROG_START_MEM + (i * 8) + 4, 10);   // minEventPeriod
                eeprom_write(USRPROG_START_MEM + (i * 8) + 5, 0xFF); // undefined
                eeprom_write(USRPROG_START_MEM + (i * 8) + 6, 0xFF); // undefined
                eeprom_write(USRPROG_START_MEM + (i * 8) + 7, 0xFF); // undefined
            }
        default:
            break;
    }

    return response;

}

/*******************************************************************************
 * Pygmy_TriggeredPIR() - handle camera after motion event
 *******************************************************************************/
void Pygmy_TriggeredPIR() {

    if (!PIR_PIN) {

        /* wake camera */
        SHUTTER_PIN = true;
        Pygmy_delay_ms(50);
        SHUTTER_PIN = false;
        Pygmy_delay_ms(100);

        /* read and apply camera settings */
        Pygmy_setCamParams(Pygmy_readDipSwitches());

        /* take burst of images */
        for (int i = 0; i < camParams.numOfSnaps; i++) {
            SHUTTER_PIN = true; // WAP WAP WAP!
            Pygmy_delay_ms(200);
            SHUTTER_PIN = false;
            Pygmy_delay_ms(camParams.snapPeriod);
        }

        /* block takePictures() for set period */
        Pygmy_delay_ms(camParams.minEventPeriod * 1000);
    }
}
