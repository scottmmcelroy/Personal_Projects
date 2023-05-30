//*******************************************************************************
// Project: SPI_COMM
// File: timer.h
// Description: timer header file for RTD controller
// Author: Scott McElroy
// Date: 5-29-2023
// License: Not to be used without written consent directly from the author above
//*******************************************************************************

//*****************/
// Includes
//*****************/
#include "timer.h"
#include <msp430.h>
#include <stdint.h>
#include <stdbool.h>
#include "sm_device.h"
//*****************/
// Defines
//*****************/

//*****************/
// Functions
//*****************/
// timer setup for ACLK driven by RTC
void Timer_setup(void){
    //selecting clock and divider, clear timer and enable interrupt
    TACTL = TASSEL0 + TACLR;
    //enable timer interrupt
    //TACTL = TAIE;
    //enable compare interrupt
    TACCTL1 = CCIE;
    //start clock command in up count
    //TACTL |= MC0;
}

//start or stop timer
void Timer_start(timer_status_e status){
    //set status timer up of off
    if(status == TIMER_ON){
        TACTL |= MC0;
    }else if(status == TIMER_OFF){
        TACTL &= ~(MC1+MC0);
    }
}

//clear timer
void Timer_clear(void){
    //clear the timer count
    TACTL |= TACLR;
}

void Timer_compare(uint16_t value){
    //load the timer compare value
    TACCR1 = value;
}

//***********
// Interrupt for timer
//***********
#pragma vector=TIMER1_A0_VECTOR
__interrupt void TIMER1_A0_ISR(void){
    //interrupt for a transmit buffer is empty
    if(LED_state == true){
        LED(ON);
    }else if(LED_state == false){
        LED(OFF);
    }
}

//********************************************************
//********************End of File*************************
//********************************************************
