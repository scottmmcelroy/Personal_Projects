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
#define SPIx SPIB_e //pre-processor switch for SPI calls
#define ZERO_TEMP_VALUE 0x7CE15 //expected ADC output value at 0C (100Ohm)
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
    //initial the start pin control
    RTC_START_init();
    //START needs to be pulled high
    RTC_START(START_ON);
    
    //wakeup the RTC device
    RTC_command(RTC_WAKEUP);
    //Stop the continuous conversion data
    RTC_command(RTC_SDATAC);

    //<<<<<Register setup>>>>>>>>>>>>
    //setup the RTC inputs for AIN1 and AIN2
    RTC_reg_write(RTC_REG_MUX0, 1, MUX0);
    //setup the reference source
    RTC_reg_write(RTC_REG_MUX1, 1, MUX1);
    //setup the IDAC outputs currents
    RTC_reg_write(RTC_REG_IDAC0, 1, IDAC0);
    //setup the IDAC output channels
    RTC_reg_write(RTC_REG_IDAC1, 1, IDAC1);

    //set start low to wait for another expected conversation
    RTC_START(START_OFF);
}

//RDATA is 24 bits output so the return needs to be redone for 24 bits
//in order to read the the data, the RDATA needs to be called and 24bits returned and put into a pointer that get 24bits
//data storage location will require Malloc and 3 locations of 8 bits each
//*************1 LSB = 195.5nV
uint8_t RTC_temp_read(void){
    //local vars
    uint32_t temp = 0;
    uint8_t return_val = 0;
    //need to send a START conversion command
    RTC_START(START_ON);
    //in order to read RDATA need to be written and than read from
    SPI_send_blocking(SPIx, RTC_RDATA);
    //after command is called, NOPs are required to get data

    //in order receive all 24 bits NOP needs to be called 3 times
    //************
    SPI_send_blocking(SPIx, RTC_NOP);
    //RDATA returns a signal write
    temp = ((uint32_t)UCB0RXBUF << 16); //SHIFT COUNT IS TOO LARGE
    SPI_send_blocking(SPIx, RTC_NOP);
    //RDATA returns a signal write
    temp |= ((uint32_t)UCB0RXBUF << 8);
    SPI_send_blocking(SPIx, RTC_NOP);
    //RDATA returns a signal write
    temp |= ((uint32_t)UCB0RXBUF);
    //***********

    //need conversion function for RTC to temp
    //((Value - Original)*195.5n)/0.385 = DegreeC value
    // Equation should return a temperature value in C
    return_val = ((temp - ZERO_TEMP_VALUE)*0.0000001955)/0.385;
    //Need to stop START conversion command
    RTC_START(START_OFF);

    //return the value
    return return_val;
}

void RTC_command(uint8_t command){
    //send register
    //CS needs to be on for command to be called
    SPI_send_blocking(SPIx, command);
}

//will only read one 8-bit at a time, no multiple register read
//currently only works for 1 byte
uint8_t RTC_reg_read(uint8_t reg, uint8_t bytes){

    //uint8_t i = 0;
    //return placeholder
    uint8_t received_data = 0;
    //the reg should be a 4 bit register
    uint8_t register_read = RTC_RREG + reg;
    //number of bytes written is the number_of_bytes + 1
    uint8_t number_of_bytes = bytes;

    //send register
    SPI_send_blocking(SPIx, register_read);
    //send number of bytes written
    SPI_send_blocking(SPIx,number_of_bytes);

    //for loop for returning  number of bytes
    //for(i=0; i<number_of_bytes; i++){
    //  SPI_send_blocking();
    //  received_data = UCB0RXBUF
    //}

    //write NOPs output so that the data comes back to the MSP
    SPI_send_blocking(SPIx, RTC_NOP);
    //pull data received and return from function
    received_data = UCB0RXBUF;
    //return the data back from the function
    return received_data;
}

//will only write one 8-bit at a time, no multiple register writes
void RTC_reg_write(uint8_t reg, uint8_t bytes, uint8_t data){
    //the reg should be a 4 bit register
    uint8_t register_write = RTC_WREG + reg;
    //number of bytes written is the number_of_bytes + 1
    uint8_t number_of_bytes = bytes;
    //send register
    SPI_send_blocking(SPIx, register_write);
    //send number of bytes written
    SPI_send_blocking(SPIx, number_of_bytes);
    //send data
    SPI_send_blocking(SPIx, data);
}

void RTC_START_init(void){
    //set the direction and output for the ADS1247 START
    P2OUT &= ~RTC_START_PIN;
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
