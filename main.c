//*******************************************************************************/
// Project: SPI_COMM
// File: main.c
// Description: main source file for SPI_COMM controller
// Author: Scott McElroy
// Date: 12-26-2022
// License: Not to be used without written consent directly from the author above
//*******************************************************************************/

//*****************/
// Includes
//*****************/
#include <msp430.h>
//#include <msp430g2553.h>
#include "sm_device.h"
#include "buffer.h"
#include "spi_uart.h"
//*****************/
// Defines
//*****************/

//*****************/
// Functions
//*****************/
void main(void)
{
    //device initialization
    Device_Init();

    //Peripherial Initialization
    Periph_Init();

    //run the program
    Program();

    //heart beat while loop
    while(1){
      LED(ON);
      __delay_cycles(16000000);
      LED(OFF);
      __delay_cycles(16000000);
    };
}

//********************************************************
//********************End of File*************************
//********************************************************
