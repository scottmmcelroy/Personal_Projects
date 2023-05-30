//*******************************************************************************
// Project: SPI_COMM
// File: timer.h
// Description: timer header file for RTD controller
// Author: Scott McElroy
// Date: 5-29-2023
// License: Not to be used without written consent directly from the author above
//*******************************************************************************

#ifndef TIMER_H_
#define TIMER_H_

//*****************/
// Includes
//*****************/
#include <msp430.h>
#include <stdint.h>
#include <stdbool.h>
#include <sm_device.h>
//*****************/
// Defines
//*****************/
typedef enum{
   TIMER_ON,
   TIMER_OFF
}timer_status_e;

//*****************/
// Functions
//*****************/
void Timer_setup(void);
void Timer_start(timer_status_e status);
void Timer_clear(void);
void Timer_compare(uint16_t value);

#endif /* TIMER_H_ */
//********************************************************
//********************End of File*************************
//********************************************************
