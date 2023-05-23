//*******************************************************************************
// Project: SPI_COMM
// File: ads1247.c
// Description: ADS1247 header file for SPI_COMM
// Author: Scott McElroy
// Date: 4-10-2023
// License: Not to be used without written consent directly from the author above
//*******************************************************************************

//*****************/
// Includes
//*****************/
#include <msp430.h>
//#include <msp430g2553.h>
#include "sm_device.h"
#include "buffer.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "spi_uart.h"
#include "ads1247.h"
//*****************/
// Defines
//*****************/


//*****************/
// Functions
//*****************/

//*****************************************
//***SPI Commands for temp sensor**********
// The data from the ADS1247 is output at
// 24 bits (0x7FFFFFh to 0x800000h)
// in 2's complement format
// The input commands
//*****************************************
void RTC_init(void){
    //variables to use for init
    uint8_t reg = 0;
    uint8_t data = 0;

    //START needs to be pulled high
    RTC_START(START_ON);
    
    //wakeup the RTC device
    RTC_command(SPIB_e, RTC_WAKEUP);
    //Stop the continuous conversion data
    RTC_command(SPIB_e, RTC_SDATAC);
    //set the data ready pin function

    //set start low to wait for another expected conversation
    RTC_START(START_OFF);
}

//RDATA is 24 bits output so the return needs to be redone for 24 bits
uint8_t RTC_temp_read(void){
    uint8_t received_data = 0;
    //in order to read RDATA need to be written and than read from
    //RDATA returns a signal write
    return received_data;
}

void RTC_command(usci_type_e type, uint8_t command){
    //send register
    //CS needs to be on for command to be called
    SPI_send_blocking(type, command);
}

//will only read one 8-bit at a time, no multiple register read
uint8_t RTC_reg_read(uint8_t reg){
    //return placeholder
    uint8_t received_data = 0;
    //the reg should be a 4 bit register
    uint8_t register_read = RTC_RREG + reg;
    //number of bytes written is the number_of_bytes + 1
    uint8_t number_of_bytes = 0;

    //send register
    SPI_send_blocking(SPIB_e, register_read);
    //send number of bytes written
    SPI_send_blocking(SPIB_e,number_of_bytes);

    //write NOPs output so that the data comes back to the MSP
    SPI_send_blocking(SPIB_e, RTC_NOP);
    //pull data received and return from function
    received_data = UCB0RXBUF;
    //return the data back from the function
    return received_data;
}

//will only write one 8-bit at a time, no multiple register writes
void RTC_reg_write(uint8_t reg, uint8_t data){
    //the reg should be a 4 bit register
    uint8_t register_write = RTC_WREG + reg;
    //number of bytes written is the number_of_bytes + 1
    uint8_t number_of_bytes = 0;
    //send register
    SPI_send_blocking(SPIB_e, register_write);
    //send number of bytes written
    SPI_send_blocking(SPIB_e, number_of_bytes);
    //send data
    SPI_send_blocking(SPIB_e, data);
}

void RTC_START_init(void){
    //set the direction and output for the ADS1247 START
    P2OUT |= RTC_START_PIN;
    P2DIR |= RTC_START_PIN;
}

void RTC_START(status_e status){
    //if the status is on or off set output
    if(status == START_ON){
        P2OUT |= RTC_START_PIN;
    }else if(status == START_OFF){
        P2OUT &= ~RTC_START_PIN;
    }    
}    
//********************************************************
//********************End of File*************************
//********************************************************
