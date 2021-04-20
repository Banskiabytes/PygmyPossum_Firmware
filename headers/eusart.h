
/**
  Section: Included Files
 */

#include <xc.h>
#include <stdbool.h>
#include <stdint.h>

/**
  Section: Macro Declarations
 */
#define EUSART_DataReady  (EUSART_is_rx_ready())

/**
  Section: Data Type Definitions
 */
typedef union {

    struct {
        unsigned perr : 1;
        unsigned ferr : 1;
        unsigned oerr : 1;
        unsigned reserved : 5;
    };
    uint8_t status;
} eusart_status_t;

/**
 Section: Global variables
 */
extern volatile uint8_t eusartTxBufferRemaining;
extern volatile uint8_t eusartRxCount;

/**
  Section: EUSART APIs
 */
extern void (*EUSART_TxDefaultInterruptHandler)(void);
extern void (*EUSART_RxDefaultInterruptHandler)(void);


void EUSART_Initialize(void);
bool EUSART_is_tx_ready(void);
bool EUSART_is_rx_ready(void);
bool EUSART_is_tx_done(void);
eusart_status_t EUSART_get_last_status(void);
uint8_t EUSART_Read(void);
void EUSART_Write(uint8_t txData);
void EUSART_WriteString(uint8_t[] );
void EUSART_Transmit_ISR(void);
void EUSART_Receive_ISR(void);
void EUSART_RxDataHandler(void);
void EUSART_SetFramingErrorHandler(void (* interruptHandler)(void));
void EUSART_SetOverrunErrorHandler(void (* interruptHandler)(void));
void EUSART_SetErrorHandler(void (* interruptHandler)(void));
void EUSART_SetTxInterruptHandler(void (* interruptHandler)(void));
void EUSART_SetRxInterruptHandler(void (* interruptHandler)(void));

