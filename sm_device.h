//*******************************************************************************
// Project: SPI_COMM
// File: sm_device.h
// Description: device header file for RTD controller
// Author: Scott McElroy
// Date: 12-26-2022
// License: Not to be used without written consent directly from the author above
//*******************************************************************************

#ifndef __SM_DEVICE_H__
#define __SM_DEVICE_H__

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
    ON,
    OFF
}LED_STATUS;

//*****************/
// Functions
//*****************/
void Device_Init(void);
void Periph_Init(void);
void LED(LED_STATUS status);

//main program run
void Program(void);


#endif /* __SM_DEVICE_H__ */
//********************************************************
//********************End of File*************************
//********************************************************
