
/* inputs */
#define DIP3_PIN PORTAbits.RA0
#define DIP1_PIN PORTAbits.RA1
#define DIP2_PIN PORTAbits.RA2
#define PIR_PIN PORTAbits.RA5

/* outputs */
#define SHUTTER_PIN LATAbits.LATA4


/* Microcontroller MIPs (FCY) */
#define _XTAL_FREQ        4000000
#define FCY             SYS_FREQ/4

/******************************************************************************/
/* Function Definitions                                                       */
/******************************************************************************/
void Hardware_ConfigureOscillator(void);
void Hardware_initIO(void);