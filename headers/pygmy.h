/* 
 * File:   pygmy.h
 * Author: me
 *
 * Created on 13 October 2020, 10:15 PM
 */


void Pygmy_init(void);
uint8_t Pygmy_readDipSwitches(void);
void Pygmy_delay_ms(uint16_t);
void Pygmy_TriggeredPIR(void);
void Pygmy_camParamsToString(void);
bool Pygmy_validCmd(char);
uint8_t * Pygmy_handleMsg(uint8_t[]);
uint16_t Pygmy_readBattVoltage();



