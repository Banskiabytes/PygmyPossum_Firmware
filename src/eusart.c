/**
  @file eusart.c
  @brief EUSART Generated Driver API Header File

  @author Microchip Technology Inc.
  @date 28/10/2020

  
    This is the generated header file for the EUSART driver using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  
    This header file provides APIs for driver for EUSART.
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.81.6
        Device            :  PIC16F18313
        Driver Version    :  2.1.0
    The generated drivers are tested against the following:
        Compiler          :  XC8 2.30 and above
        MPLAB 	          :  MPLAB X 5.40
 */

#include "eusart.h"
#include <string.h>

#define EUSART_TX_BUFFER_SIZE 8
#define EUSART_RX_BUFFER_SIZE 8

/**
  Section: Global Variables
 */
volatile uint8_t eusartTxHead = 0;
volatile uint8_t eusartTxTail = 0;
volatile uint8_t eusartTxBuffer[EUSART_TX_BUFFER_SIZE];
volatile uint8_t eusartTxBufferRemaining;

volatile uint8_t eusartRxHead = 0;
volatile uint8_t eusartRxTail = 0;
volatile uint8_t eusartRxBuffer[EUSART_RX_BUFFER_SIZE];
volatile eusart_status_t eusartRxStatusBuffer[EUSART_RX_BUFFER_SIZE];
volatile uint8_t eusartRxCount;
volatile eusart_status_t eusartRxLastError;

/**
  Section: EUSART APIs
 */
void (*EUSART_TxDefaultInterruptHandler)(void);
void (*EUSART_RxDefaultInterruptHandler)(void);

void (*EUSART_FramingErrorHandler)(void);
void (*EUSART_OverrunErrorHandler)(void);
void (*EUSART_ErrorHandler)(void);

void EUSART_DefaultFramingErrorHandler(void);
void EUSART_DefaultOverrunErrorHandler(void);
void EUSART_DefaultErrorHandler(void);

/**
  @brief
    Initialization routine that takes inputs from the EUSART GUI.

  
    This routine initializes the EUSART driver.
    This routine must be called before any other EUSART routine is called.

  @Preconditions
    None

  @Param
    None

  @Returns
    None

  @Comment
    
 */
void EUSART_Initialize(void) {
    // disable interrupts before changing states
    PIE1bits.RCIE = 0;
    EUSART_SetRxInterruptHandler(EUSART_Receive_ISR);
    PIE1bits.TXIE = 0;
    EUSART_SetTxInterruptHandler(EUSART_Transmit_ISR);

    // Set the EUSART module to the options selected in the user interface.
    EUSART_SetFramingErrorHandler(EUSART_DefaultFramingErrorHandler);
    EUSART_SetOverrunErrorHandler(EUSART_DefaultOverrunErrorHandler);
    EUSART_SetErrorHandler(EUSART_DefaultErrorHandler);
    eusartRxLastError.status = 0;

    // initializing the driver state
    eusartTxHead = 0;
    eusartTxTail = 0;
    eusartTxBufferRemaining = sizeof (eusartTxBuffer);

    eusartRxHead = 0;
    eusartRxTail = 0;
    eusartRxCount = 0;

    // enable receive interrupt
    PIE1bits.RCIE = 1;
}

/**
  @brief Checks if the EUSART receiver ready for reading

    This routine checks if EUSART receiver has received data 
    and ready to be read

  @Preconditions
    EUSART_Initialize() function should be called
    before calling this function
    EUSART receiver should be enabled before calling this 
    function

  @Param
    None

  @Returns
    Status of EUSART receiver
    TRUE: EUSART receiver is ready for reading
    FALSE: EUSART receiver is not ready for reading
    
  @Example
    <code>
    void main(void)
    {
        volatile uint8_t rxData;
        
        // Initialize the device
        SYSTEM_Initialize();
        
        while(1)
        {
            // Logic to echo received data
            if(EUSART_is_rx_ready())
            {
                rxData = UART1_Read();
                if(EUSART_is_tx_ready())
                {
                    EUSART_Write(rxData);
                }
            }
        }
    }
    </code>
 */
bool EUSART_is_tx_ready(void) {
    return (eusartTxBufferRemaining ? true : false);
}

/**
  @brief
    Checks if the EUSART receiver ready for reading

  
    This routine checks if EUSART receiver has received data 
    and ready to be read

  @Preconditions
    EUSART_Initialize() function should be called
    before calling this function
    EUSART receiver should be enabled before calling this 
    function

  @Param
    None

  @Returns
    Status of EUSART receiver
    TRUE: EUSART receiver is ready for reading
    FALSE: EUSART receiver is not ready for reading
    
  @Example
    <code>
    void main(void)
    {
        volatile uint8_t rxData;
        
        // Initialize the device
        SYSTEM_Initialize();
        
        while(1)
        {
            // Logic to echo received data
            if(EUSART_is_rx_ready())
            {
                rxData = UART1_Read();
                if(EUSART_is_tx_ready())
                {
                    EUSART_Write(rxData);
                }
            }
        }
    }
    </code>
 */
bool EUSART_is_rx_ready(void) {
    return (eusartRxCount ? true : false);
}

/**
  @brief
    Checks if EUSART data is transmitted

  
    This function return the status of transmit shift register

  @Preconditions
    EUSART_Initialize() function should be called
    before calling this function
    EUSART transmitter should be enabled and EUSART_Write
    should be called before calling this function

  @Param
    None

  @Returns
    Status of EUSART receiver
    TRUE: Data completely shifted out if the USART shift register
    FALSE: Data is not completely shifted out of the shift register
    
  @Example
    <code>
    void main(void)
    {
        volatile uint8_t rxData;
        
        // Initialize the device
        SYSTEM_Initialize();
        
        while(1)
        {
            if(EUSART_is_tx_ready())
            {
                LED_0_SetHigh();
                EUSARTWrite(rxData);
            }
            if(EUSART_is_tx_done()
            {
                LED_0_SetLow();
            }
        }
    }
    </code>
 */
bool EUSART_is_tx_done(void) {
    return TX1STAbits.TRMT;
}

/**
  @brief
    Gets the error status of the last read byte.

  
    This routine gets the error status of the last read byte.

  @Preconditions
    EUSART_Initialize() function should have been called
    before calling this function. The returned value is only
    updated after a read is called.

  @Param
    None

  @Returns
    the status of the last read byte

  @Example
    <code>
    void main(void)
    {
        volatile uint8_t rxData;
        volatile eusart_status_t rxStatus;
        
        // Initialize the device
        SYSTEM_Initialize();
        
        // Enable the Global Interrupts
        INTERRUPT_GlobalInterruptEnable();
        
        while(1)
        {
            // Logic to echo received data
            if(EUSART_is_rx_ready())
            {
                rxData = EUSART_Read();
                rxStatus = EUSART_get_last_status();
                if(rxStatus.ferr){
                    LED_0_SetHigh();
                }
            }
        }
    }
    </code>
 */
eusart_status_t EUSART_get_last_status(void) {
    return eusartRxLastError;
}

/**
  @brief
    Read a byte of data from the EUSART.

  
    This routine reads a byte of data from the EUSART.

  @Preconditions
    EUSART_Initialize() function should have been called
    before calling this function. The transfer status should be checked to see
    if the receiver is not empty before calling this function.

  @Param
    None

  @Returns
    A data byte received by the driver.
 */
uint8_t EUSART_Read(void) {
    uint8_t readValue = 0;

    while (0 == eusartRxCount) {
    }

    eusartRxLastError = eusartRxStatusBuffer[eusartRxTail];

    readValue = eusartRxBuffer[eusartRxTail++];
    if (sizeof (eusartRxBuffer) <= eusartRxTail) {
        eusartRxTail = 0;
    }
    PIE1bits.RCIE = 0;
    eusartRxCount--;
    PIE1bits.RCIE = 1;

    return readValue;
}

/**
  @brief
    Writes a byte of data to the EUSART.

  
    This routine writes a byte of data to the EUSART.

  @Preconditions
    EUSART_Initialize() function should have been called
    before calling this function. The transfer status should be checked to see
    if transmitter is not busy before calling this function.

  @Param
    txData  - Data byte to write to the EUSART

  @Returns
    None
 */
void EUSART_Write(uint8_t txData) {
    while (0 == eusartTxBufferRemaining) {
    }

    if (0 == PIE1bits.TXIE) {
        TX1REG = txData;
    } else {
        PIE1bits.TXIE = 0;
        eusartTxBuffer[eusartTxHead++] = txData;
        if (sizeof (eusartTxBuffer) <= eusartTxHead) {
            eusartTxHead = 0;
        }
        eusartTxBufferRemaining--;
    }
    PIE1bits.TXIE = 1;
}

void EUSART_WriteString(uint8_t txData[]) {
    int txDataLen = strlen(txData);
    for (int i = 0; i < txDataLen; i++) {
        while (!EUSART_is_tx_ready());
        EUSART_Write(txData[i]);
    }
}

/**
  @brief
    Maintains the driver's transmitter state machine and implements its ISR.

  
    This routine is used to maintain the driver's internal transmitter state
    machine.This interrupt service routine is called when the state of the
    transmitter needs to be maintained in a non polled manner.

  @Preconditions
    EUSART_Initialize() function should have been called
    for the ISR to execute correctly.

  @Param
    None

  @Returns
    None
 */
void EUSART_Transmit_ISR(void) {

    // add your EUSART interrupt custom code
    if (sizeof (eusartTxBuffer) > eusartTxBufferRemaining) {
        TX1REG = eusartTxBuffer[eusartTxTail++];
        if (sizeof (eusartTxBuffer) <= eusartTxTail) {
            eusartTxTail = 0;
        }
        eusartTxBufferRemaining++;
    } else {
        PIE1bits.TXIE = 0;
    }
}

/**
  @brief
    Maintains the driver's receiver state machine and implements its ISR

  
    This routine is used to maintain the driver's internal receiver state
    machine.This interrupt service routine is called when the state of the
    receiver needs to be maintained in a non polled manner.

  @Preconditions
    EUSART_Initialize() function should have been called
    for the ISR to execute correctly.

  @Param
    None

  @Returns
    None
 */
void EUSART_Receive_ISR(void) {

    eusartRxStatusBuffer[eusartRxHead].status = 0;

    if (RC1STAbits.FERR) {
        eusartRxStatusBuffer[eusartRxHead].ferr = 1;
        EUSART_FramingErrorHandler();
    }

    if (RC1STAbits.OERR) {
        eusartRxStatusBuffer[eusartRxHead].oerr = 1;
        EUSART_OverrunErrorHandler();
    }

    if (eusartRxStatusBuffer[eusartRxHead].status) {
        EUSART_ErrorHandler();
    } else {
        EUSART_RxDataHandler();
    }

    // or set custom function using EUSART_SetRxInterruptHandler()
}

/**
  @brief
    Maintains the driver's receiver state machine

  
    This routine is called by the receive state routine and is used to maintain 
    the driver's internal receiver state machine. It should be called by a custom
    ISR to maintain normal behavior

  @Preconditions
    EUSART_Initialize() function should have been called
    for the ISR to execute correctly.

  @Param
    None

  @Returns
    None
 */
void EUSART_RxDataHandler(void) {
    // use this default receive interrupt handler code
    eusartRxBuffer[eusartRxHead++] = RC1REG;
    if (sizeof (eusartRxBuffer) <= eusartRxHead) {
        eusartRxHead = 0;
    }
    eusartRxCount++;
}

void EUSART_DefaultFramingErrorHandler(void) {
}

void EUSART_DefaultOverrunErrorHandler(void) {
    // EUSART error - restart

    RC1STAbits.CREN = 0;
    RC1STAbits.CREN = 1;

}

void EUSART_DefaultErrorHandler(void) {
    EUSART_RxDataHandler();
}

/**
  @brief
    Set EUSART Framing Error Handler

  
    This API sets the function to be called upon EUSART framing error

  @Preconditions
    Initialize  the EUSART before calling this API

  @Param
    Address of function to be set as framing error handler

  @Returns
    None
 */
void EUSART_SetFramingErrorHandler(void (* interruptHandler)(void)) {
    EUSART_FramingErrorHandler = interruptHandler;
}

/**
  @brief
    Set EUSART Overrun Error Handler

  
    This API sets the function to be called upon EUSART overrun error

  @Preconditions
    Initialize  the EUSART module before calling this API

  @Param
    Address of function to be set as overrun error handler

  @Returns
    None
 */
void EUSART_SetOverrunErrorHandler(void (* interruptHandler)(void)) {
    EUSART_OverrunErrorHandler = interruptHandler;
}

/**
  @brief
    Set EUSART Error Handler

  
    This API sets the function to be called upon EUSART error

  @Preconditions
    Initialize  the EUSART module before calling this API

  @Param
    Address of function to be set as error handler

  @Returns
    None
 */
void EUSART_SetErrorHandler(void (* interruptHandler)(void)) {
    EUSART_ErrorHandler = interruptHandler;
}

/**
  @brief
    Sets the transmit handler function to be called by the interrupt service

  
    Calling this function will set a new custom function that will be 
    called when the transmit interrupt needs servicing.

  @Preconditions
    EUSART_Initialize() function should have been called
    for the ISR to execute correctly.

  @Param
    A pointer to the new function

  @Returns
    None
 */
void EUSART_SetTxInterruptHandler(void (* interruptHandler)(void)) {
    EUSART_TxDefaultInterruptHandler = interruptHandler;
}

/**
  @brief
    Sets the receive handler function to be called by the interrupt service

  
    Calling this function will set a new custom function that will be 
    called when the receive interrupt needs servicing.

  @Preconditions
    EUSART_Initialize() function should have been called
    for the ISR to execute correctly.

  @Param
    A pointer to the new function

  @Returns
    None
 */
void EUSART_SetRxInterruptHandler(void (* interruptHandler)(void)) {
    EUSART_RxDefaultInterruptHandler = interruptHandler;
}