
/* inputs */
#define DIP4 PORTCbits.RC6
#define DIP3 PORTCbits.RC3
#define DIP2 PORTCbits.RC4
#define DIP1 PORTCbits.RC5
#define DIN_PIR PORTAbits.RA5
#define DIN_USB_ON !PORTCbits.RC1 // inverted

/* outputs */
#define DOUT_CAM_S LATCbits.LATC7
#define DOUT_CAM_F LATBbits.LATB7
#define DOUT_BAT_CHECK_EN LATAbits.LATA2

/* Microcontroller MIPs (FCY) */
#define _XTAL_FREQ 32000000UL
#define FCY SYS_FREQ/4

/******************************************************************************/
/* Function Definitions                                                       */
/******************************************************************************/
void Hardware_ConfigureOscillator(void);
void Hardware_initIO(void);
void Hardware_initUART(void);
void Hardware_UARTsendByte(uint8_t);
void Hardware_UARTsendString(char[]);
