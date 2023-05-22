//*******************************************************************************
// Project: SPI_COMM
// File: spi_uart.h
// Description: spi and uart header file for SPI_COMM
// Author: Scott McElroy
// Date: 12-26-2022
// License: Not to be used without written consent directly from the author above
//*******************************************************************************

#ifndef SPI_UART_H_
#define SPI_UART_H_

//*****************/
// Includes
//*****************/
#include <msp430.h>
//#include <msp430g2553.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

//*****************/
// Defines
//*****************/
typedef enum{
    CS_ON,
    CS_OFF
}CS_STATUS;

typedef enum{
    SPI_ON,
    SPI_OFF
}SPI_STATUS;

typedef enum{
    UART_ON,
    UART_OFF
}UART_STATUS;

typedef enum{
    USCI_A_e,
    USCI_B_e,
    USCI_AB_e
}usci_port_e;

typedef enum{
    SPIA_e,
    SPIB_e,
    SPIAB_e,
    SPIA_UARTB_e, //no UART_B0 ON 2553
    UARTA_SPIB_e,
    UARTA_e,
    UARTB_e,  //no UART on B0 for 2553
    UARTAB_e  //no UART on B0 for 2553
}usci_type_e;

#define UART_RCV_INT 0b1

//*****************/
// Global Variables
//*****************/
extern uint8_t flags;

//*****************/
// Functions
//*****************/
void USCI_init(usci_port_e port, usci_type_e type);
void SPI_Init(usci_type_e type);
void UART_Init(usci_type_e type);
void UART_Enable(usci_type_e type, UART_STATUS status);
void SPI_Enable(usci_type_e type, SPI_STATUS status);
void SPI_CS_Init(usci_type_e type);
void SPI_CS_ON(usci_type_e type, CS_STATUS status);
void SPI_send_blocking(usci_type_e type, uint8_t data);

#endif /* SPI_UART_H_ */

//********************************************************
//********************End of File*************************
//********************************************************
