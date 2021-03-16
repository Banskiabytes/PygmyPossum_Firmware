/** 
 *  @file pygmy.c
 *  @brief Contains all functions and utilities to load settings and 
 *  control the camera
 *
 *  @author Thomas Evison
 *  @date 28/10/2020
 */

#include <xc.h>            /* XC8 General Include File */
#include <stdint.h>        /* For uint8_t definition */
#include <stdlib.h>        /* For uint8_t definition */
#include "hardware.h"
#include <stdbool.h>       /* For true/false definition */
#include <string.h>

#define USRPROG_START_MEM (uint8_t)128 /* address byte of EEPROM for user programs

/* user defined types */

/**
 * Structure containing parameters for camera during event
 */
struct {
    bool wakeCamera;        ///< wake camera after PIR event
    uint8_t numOfSnaps;     ///< amount of pics to take after PIR event
    uint16_t snapPeriod;    ///< ms - frequency of burst after event
    uint8_t minEventPeriod; ///< s - minimum time before another shutter is trig
} camParams;



/**
 *  Reads hardware dip switches and resolves to an unsigned int.
 *  @return The the dip switches setting as an unsigned integer
 */
uint8_t Pygmy_readDipSwitches() {

    // values inverted for active low switch w/ pull-up resistor
    uint8_t dipSetting = 0;
    dipSetting |= !DIP1 << 0;
    dipSetting |= !DIP2 << 1;
    dipSetting |= !DIP3 << 2;
    dipSetting |= !DIP4 << 3;
    return dipSetting;
}

/**
 *  Reads hardware dip switches and resolves to an unsigned int.
 *  @return The the dip switches setting as an unsigned integer
 */
uint16_t Pygmy_readBattVoltage() {
    
    DOUT_BAT_CHECK_EN = true;   // energize battery checking circuit
    
    uint16_t result; 
    
    ADCON0bits.ADON = true;     // ADC is enabled
    ADCON0bits.GO = true;
    
    while (ADCON0bits.ADGO);
    

    result = (ADRESH << 8) | ADRESL;
    
    ADCON0bits.ADON = false;     // ADC is disabled and consumes no operating current
    
    DOUT_BAT_CHECK_EN = false;   // de-energize battery checking circuit
    
    return result;
}

/**
 *  Crappy delay function to overcome __delay_ms() constraint 
 *  of "inline delay argument must be constant"
 *  @param milliseconds The time to delay in milliseconds
 */
void Pygmy_delay_ms(uint16_t milliseconds) {


    while (milliseconds > 0) {
        __delay_ms(1);
        milliseconds--;
    }
}

/**
 *  Setup camera timing parameters
 *  @param dipSetting The current value of the hardware dip switches
 */
void Pygmy_setCamParams(uint8_t dipSetting) {

    uint8_t usrProgStartAddress = USRPROG_START_MEM + dipSetting * 8;

    /* get the camera parameters from EEPROM */
    camParams.numOfSnaps = eeprom_read(usrProgStartAddress + 1);

    /* snap period is stored across 2 bytes */
    uint8_t snapPeriodLSB = eeprom_read(usrProgStartAddress + 2);
    uint8_t snapPeriodMSB = eeprom_read(usrProgStartAddress + 3);
    camParams.snapPeriod = (snapPeriodMSB << 8) | snapPeriodLSB;

    camParams.minEventPeriod = eeprom_read(usrProgStartAddress + 4);
}

/**
 *  handle incomming UART messages
 *  @param cmd 16 byte wide array, incoming packet from PC
 */
uint8_t * Pygmy_handleMsg(uint8_t cmd[]) {

    /* start crafting the response message */
    uint8_t response[16] = {0};
    response[0] = 'R'; ///< standard response flag
    response[1] = cmd[1]; ///< return the cmd code
    
    uint16_t battVoltage;

    /* get frame command */
    switch (cmd[1]) {
        case 'A': // get usr prog
            response[2] = cmd[2];
            for (uint8_t i = 0; i < 8; i++) {
                response[3 + i] = (eeprom_read(USRPROG_START_MEM + cmd[2]*8 + i));
            }
            break;
        case 'B': // set usr prog
            response[2] = cmd[2];
            uint16_t index = 8*response[2];
            for (uint8_t i = 0; i < 8; i++) {
                eeprom_write(USRPROG_START_MEM + index + i, cmd[2 + i]);
                response[2 + i] = cmd[2 + i];
            }
            break;
        case 'D': // get dip switches
            response[2] = Pygmy_readDipSwitches();
            break;
        case 'E': // get the battery voltage
            
            battVoltage = Pygmy_readBattVoltage();
            response[2]= battVoltage & 0xff;
            response[3]= (battVoltage >> 8);
            break;
        case 'V': // set default usrProg values
            for (uint8_t i = 0; i < 16; i++) {
                eeprom_write(USRPROG_START_MEM + (i * 8) + 0, 0x00); // config byte
                eeprom_write(USRPROG_START_MEM + (i * 8) + 1, 0x03); // numOfSnaps
                eeprom_write(USRPROG_START_MEM + (i * 8) + 2, 0xF4); // snapPeriod LSB
                eeprom_write(USRPROG_START_MEM + (i * 8) + 3, 0x01); // snapPeriod MSB
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

/**
 *  handle camera after motion event. Called in ISR() 
 *  after change on PIR input pin
 */
void Pygmy_TriggeredPIR() {

    if (!DIN_PIR) {

        /* wake camera */
        DOUT_CAM_S = true;
        Pygmy_delay_ms(50);
        DOUT_CAM_S = false;
        Pygmy_delay_ms(100);

        /* read and apply camera settings */
        Pygmy_setCamParams(Pygmy_readDipSwitches());

        /* take burst of images */
        for (int i = 0; i < camParams.numOfSnaps; i++) {
            DOUT_CAM_S = true; // WAP WAP WAP!
            Pygmy_delay_ms(200);
            DOUT_CAM_S = false;
            Pygmy_delay_ms(camParams.snapPeriod);
        }

        /* block takePictures() for set period */
        Pygmy_delay_ms(camParams.minEventPeriod * 1000);
    }
}
