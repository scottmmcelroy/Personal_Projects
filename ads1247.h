//*******************************************************************************
// Project: SPI_COMM
// File: ads1247.h
// Description: ADS1247 header file for SPI_COMM
// Author: Scott McElroy
// Date: 4-10-2023
// License: Not to be used without written consent directly from the author above
//*******************************************************************************

#ifndef ADS1247_H_
#define ADS1247_H_
//*****************/
// Includes
//*****************/
#include <msp430.h>
//#include <msp430g2553.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "spi_uart.h"

//*****************/
// Defines
//*****************/
typedef enum{
    ON,
    OFF
}status_e;
  
#define RTC_START_PIN 0x20 //start pin is P2.0
//******RTC COMMANDS******
//****ADS1247 commands
#define RTC_WAKEUP 0x01 //exit power-down mode
#define RTC_SLEEP 0x03 //enter power-down mode
#define RTC_SYNC 0x05 //send this twice, sync ADC conversation
#define RTC_RESET 0x07 //reset to the default values
#define RTC_NOP 0xFF //no operation
#define RTC_RDATA 0x13 //read data once
#define RTC_RDATAC 0x15 //read data continuously
#define RTC_SDATAC 0x17 //Stop reading data continuously
#define RTC_RREG 0x20 //read from register
#define RTC_WREG 0x40 //write to register
#define RTC_SYSOCAL 0x60 //system offset calibration
#define RTC_SYSGCAL 0x61 //system gain calibration
#define RTC_SELFOCAL 0x62 //self calibration
//***ADS1247 registers
#define RTC_REG_BCS 0x00 //burn-out current source
#define RTC_REG_VBIAS 0x01 //voltage bias
#define RTC_REG_MUX1 0x02 //ADC mux control register
#define RTC_REG_SYS0 0x03 //PGA set register and sample data rate
#define RTC_REG_OFC0 0x04 //offset calibration low byte
#define RTC_REG_OFC1 0x05 //offset calibration mid byte
#define RTC_REG_OFC2 0x06 //offset calibration high byte
#define RTC_REG_FSC0 0x07 //full scale calibration low byte
#define RTC_REG_FSC1 0x08 //full scale calibration mid byte
#define RTC_REF_FSC2 0x09 //fulle scale calibration high byte
#define RTC_REG_IDAC0 0x0A //ID, data ready pin, IDAC source settings
#define RTC_REG_IDAC1 0x0B //IDAC direction setting pin
#define RTC_REG_GPIOCFG 0x0C //gpio setting register
#define RTC_REG_GPIODIR 0x0D //gpio direction reg
#define RTC_REG_GPIODAT 0x0E //gpio data settings

//*****************/
// Functions
//*****************/
void RTC_init(void);
uint8_t RTC_temp_read(void);
void RTC_command(usci_type_e type, uint8_t command);
uint8_t RTC_reg_read(uint8_t reg);
void RTC_reg_write(uint8_t reg, uint8_t data);
void RTC_START_init(void);
void RTC_START(status_e status);

#endif /* ADS1247_H_ */

//********************************************************
//********************End of File*************************
//********************************************************
