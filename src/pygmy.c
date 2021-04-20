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

#define USRSETTINGS_START_MEM (uint8_t)112
#define USRPROG_START_MEM (uint8_t)128 // address byte of EEPROM for user programs

/* user defined types */


/**
 * Structure containing parameters for camera during event
 */
struct {
    // global settings
    uint8_t wakeCameraDelay;// how long to wait after waking the camera
    uint8_t shutterPulseWidth;  // how long to 'hold' the shutter button, default 200ms
    
    // usrProg specific
    bool videoModeEnable;   // enable video mode (default is camera mode)
    uint8_t numOfSnaps;     // amount of pics to take after PIR event
    uint16_t snapPeriod;    // ms - frequency of burst after event
    uint8_t minEventPeriod; // s - minimum time before another shutter is trig
    uint8_t videoLength;    // s - video length, note, VIDEO_MODE in configbyte must be true
} camParams;

/**
 *  Reads hardware dip switches and resolves to an unsigned int.
 *  @return The the dip switches setting as an unsigned integer
 */
uint8_t Pygmy_getDipSwitches() {

    // values inverted for active low switch w/ pull-up resistor
    uint8_t dipSetting = 0;
    dipSetting |= !DIP1 << 0;
    dipSetting |= !DIP2 << 1;
    dipSetting |= !DIP3 << 2;
    dipSetting |= !DIP4 << 3;
    return dipSetting;
}

/**
 *  Crappy delay function to overcome __delay_ms() constraint 
 *  of "inline delay argument must be constant"
 *  max delay is 4294967.296s == 49.7 days
 *  @param milliseconds The time to delay in milliseconds
 */
void Pygmy_delay_ms(uint32_t milliseconds) {

    while (milliseconds > 0) {
        __delay_ms(1);
        milliseconds--;
    }
}

/**
 *  Setup camera timing parameters - stored in EEPROM
 *  @param dipSetting The current value of the hardware dip switches
 */
void Pygmy_setCamParams(uint8_t dipSetting) {
    
    // global settings ---------------
    camParams.wakeCameraDelay = eeprom_read(USRSETTINGS_START_MEM);
    camParams.shutterPulseWidth = eeprom_read(USRPROG_START_MEM);
    
    // usrProg specific ---------------

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
 *  Reads hardware dip switches and resolves to an unsigned int.
 *  @return The the dip switches setting as an unsigned integer
 */
uint16_t Pygmy_getBattVoltage() {
    
    DOUT_BAT_CHECK_EN = true;   // energize battery checking circuit
    
    uint16_t result; 
    ADCON0bits.ADON = true;     // ADC is enabled
    uint32_t ADCconvertDelay = 32;
    Pygmy_delay_ms(ADCconvertDelay);
    ADCON0bits.GO = true;
    
    while (ADCON0bits.ADGO);
    
    result = (ADRESH << 8) | ADRESL;
    
    ADCON0bits.ADON = false;     // ADC is disabled and consumes no operating current
    
    DOUT_BAT_CHECK_EN = false;   // de-energize battery checking circuit
    
    return result;
}

/**
 *  handle camera after motion event. Called in ISR() 
 *  after change on PIR input pin
 */
void Pygmy_TriggeredPIR() {

    if (!DIN_PIR) {

        /* wake camera */
        DOUT_CAM_S = true;
        Pygmy_delay_ms(camParams.shutterPulseWidth);
        DOUT_CAM_S = false;
        Pygmy_delay_ms(camParams.wakeCameraDelay);

        /* read and apply camera settings */
        Pygmy_setCamParams(Pygmy_getDipSwitches());

        /* take burst of images */
        for (int i = 0; i < camParams.numOfSnaps; i++) {
            DOUT_CAM_S = true; // WAP WAP WAP!
            Pygmy_delay_ms(camParams.shutterPulseWidth);
            DOUT_CAM_S = false;
            Pygmy_delay_ms(camParams.snapPeriod);
        }

        /* block takePictures() for set period */
        Pygmy_delay_ms(camParams.minEventPeriod * 1000);
    }
}

/**
 *  set default values to all usrProgs
 */
void Pygmy_SetDefaultValues() {
    
    uint8_t defaultConfigBytes[16] = {0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1};
    uint16_t defaultSnapPeriods[16] = {50, 100, 250, 500, 750, 1000, 1500, 2000, 3000, 5000, 10000, 1000, 1000, 1000, 1000, 1000};
    uint8_t defaultVideoLen[16] = {0,0,0,0,0,0,0,0,0,0,0,5,10,20,60};
    
    eeprom_write(USRSETTINGS_START_MEM, 0x64);               // wakeCameraDelay - 100ms
    eeprom_write(USRSETTINGS_START_MEM + 1, 0xC8);           // shutterPulseWidth - 200ms

    for (uint8_t i = 0; i < 16; i++) {
        eeprom_write(USRPROG_START_MEM + (i * 8) + 0, defaultConfigBytes[i]); // config byte
        eeprom_write(USRPROG_START_MEM + (i * 8) + 1, 0x03); // numOfSnaps

        // split value into 2 bytes
        uint8_t snapPeriodLSB = defaultSnapPeriods[i] & 0x00FF;        
        uint8_t snapPeriodMSB = defaultSnapPeriods[i] >> 8;

        eeprom_write(USRPROG_START_MEM + (i * 8) + 2, snapPeriodLSB); // snapPeriod LSB
        eeprom_write(USRPROG_START_MEM + (i * 8) + 3, snapPeriodMSB); // snapPeriod MSB
        eeprom_write(USRPROG_START_MEM + (i * 8) + 4, 8);   // minEventPeriod
        eeprom_write(USRPROG_START_MEM + (i * 8) + 5, defaultVideoLen[i]); // videoLen
        eeprom_write(USRPROG_START_MEM + (i * 8) + 6, 0xFF); // undefined
        eeprom_write(USRPROG_START_MEM + (i * 8) + 7, 0xFF); // undefined
    }
    
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
            response[2] = Pygmy_getDipSwitches();
            break;
        case 'E': // get the battery voltage
            
            battVoltage = Pygmy_getBattVoltage();
            response[2]= battVoltage & 0xff; // LSB
            response[3]= (battVoltage >> 8); // MSB
            break;
        case 'F': // get the settings
            response[2] = eeprom_read(USRSETTINGS_START_MEM);        // wakeCameraDelay
            response[3] = eeprom_read(USRSETTINGS_START_MEM+1);      // shutterPulseWidth
            response[4] = eeprom_read(USRSETTINGS_START_MEM+2);      // firmwareMajor
            response[5] = eeprom_read(USRSETTINGS_START_MEM+3);      // firmwareMinor
            response[6] = eeprom_read(USRSETTINGS_START_MEM+4);      // firmwareMinor
            break;
        case 'G': // set the settings
            eeprom_write(USRSETTINGS_START_MEM, response[2]);        // wakeCameraDelay
            eeprom_write(USRSETTINGS_START_MEM+1, response[3]);      // shutterPulseWidth

            break;    
        case 'V': // set default usrProg values
            Pygmy_SetDefaultValues();
            
        default:
            break;
    }

    return response;

}
