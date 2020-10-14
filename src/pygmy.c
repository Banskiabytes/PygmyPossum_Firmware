#include <xc.h>            /* XC8 General Include File */
#include <stdint.h>        /* For uint8_t definition */
#include "hardware.h"
#include <stdbool.h>       /* For true/false definition */

/* user defined types */
struct {
    uint8_t  numOfSnaps;     // amount of pics to take after PIR event
    uint16_t snapPeriod;     // ms - frequency of burst after event
    uint8_t  minEventPeriod; // s - minimum time before another shutter is trig
} camParams;

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

/*******************************************************************************
 * Pygmy_setCamParams() - setup camera timing parameters
 *******************************************************************************/
void Pygmy_setCamParams(uint8_t dipSetting) {
    switch (dipSetting) {
        case 0:
            camParams.numOfSnaps = 3;
            camParams.snapPeriod = 50;
            camParams.minEventPeriod = 10;
            break;
        case 1:
            camParams.numOfSnaps = 3;
            camParams.snapPeriod = 100;
            camParams.minEventPeriod = 10;
            break;
        case 2:
            camParams.numOfSnaps = 3;
            camParams.snapPeriod = 250;
            camParams.minEventPeriod = 10;
            break;
        case 3:
            camParams.numOfSnaps = 3;
            camParams.snapPeriod = 500;
            camParams.minEventPeriod = 10;
            break;
        case 4:
            camParams.numOfSnaps = 3;
            camParams.snapPeriod = 750;
            camParams.minEventPeriod = 10;
            break;
        case 5:
            camParams.numOfSnaps = 3;
            camParams.snapPeriod = 1000;
            camParams.minEventPeriod = 10;
            break;
        case 6:
            camParams.numOfSnaps = 3;
            camParams.snapPeriod = 1500;
            camParams.minEventPeriod = 10;
            break;
        case 7:
            camParams.numOfSnaps = 3;
            camParams.snapPeriod = 3000;
            camParams.minEventPeriod = 10;
            break;
        default:
            camParams.numOfSnaps = 3;
            camParams.snapPeriod = 1000;
            camParams.minEventPeriod = 10;
            break;
    }
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
