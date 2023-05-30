//*******************************************************************************
// Project: SPI_COMM
// File: sm_device.c
// Description: device source file for RTD controller
// Author: Scott McElroy
// Date: 12-26-2022
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
#include "timer.h"
//*****************/
// Defines
//*****************/
#define LED1 0x1 //port1.0

#define DATA 0xA5 //SPI data for toggling the outputs
//*****Reflow profile (Chipquick SMD291SNL15T4)********
#define RAMP_TO_SOAK_TIME 90 //90 seconds
#define RAMP_TO_SOAK_TEMP 150 //150C
#define SOAK_TIME 90 //90 seconds
#define SOAK_TEMP 175 //175C
#define RAMP_TO_PEAK_TIME 50 //50 seconds
#define RAMP_TO_PEAK_TEMP 250 //250C
#define PEAK_TIME 20 //20 seconds
#define PEAK_TEMP 250 //250C
#define RAMP_TO_OFF_TIME 30 //30 seconds
#define RAMP_TO_OFF_TEMP 25 //25C
//*******************/
// Globals
//*******************/
//*****reflow profile**********
//Ramp to Soak
uint16_t ramp_to_soak_time = RAMP_TO_SOAK_TIME;
uint16_t ramp_to_soak_temp = RAMP_TO_SOAK_TEMP;
uint16_t current_ramp_to_soak_temp = 0;
//Soak time
uint16_t soak_time = SOAK_TIME;
uint16_t soak_temp = SOAK_TEMP;
//Ramp to peak
uint16_t ramp_to_peak_time = RAMP_TO_PEAK_TIME;
uint16_t ramp_to_peak_temp = RAMP_TO_PEAK_TEMP;
uint16_t current_ramp_to_peak_temp = 0;
//Ramp Down
uint16_t ramp_to_off_time = RAMP_TO_OFF_TIME;
uint16_t ramp_to_off_temp = RAMP_TO_OFF_TEMP;
uint16_t current_ramp_to_off_temp = 0;
//start of reflow profile
extern bool start_trigger = false;
//LED state
extern bool LED_state = false;
//*****************/
// Functions
//*****************/
void Device_Init(void){

    //turn off watchdog
    WDTCTL = WDTHOLD | WDTPW;

    //set clocks to 16MHz
    if(CALDCO_1MHZ ==0xFF || CALBC1_1MHZ ==0xFF){
        while(1); //Erased Calibration data? Trap!
    }
    DCOCTL = CALDCO_1MHZ; //(0x2, 0b10)
    BCSCTL1 = CALBC1_1MHZ; //(0x3, 0b11)
    //set SMCLK and MCLK to run at 16MHz
    BCSCTL2 = 0; //MCLK and SMCLK run from DCO without dividers

    //setup the SMCLK = 16MHz
    //default source is DCOCLK
    //setup ACLK for RTC input, default setup is ACLK from crystal with /1

    //setup second timer clock

    //setup of ports for the LED
    P1OUT &= ~LED1; //set LED output to 0
    P1DIR |= LED1; //set LED1 to an output

};

//******************
// Periph Init sets
// up SPI and UART
// periphs
//******************
void Periph_Init(void){
    //setup gpio
    ///**********
    Timer_setup();
    Timer_clear();
    //Setup usci
    USCI_init(USCI_AB_e, UARTA_SPIB_e);
    //setup spi and uart
    SPI_Init(SPIB_e);
    //setup a UART
    UART_Init(UARTA_e);
    //enable UART
    UART_Enable(UARTA_e, UART_ON);
    //enable SPI
    SPI_Enable(SPIB_e, SPI_ON);
};

//****led***
void LED(LED_STATUS status){
    //setup LED on or off
    //on
    if(status == ON){
        P1OUT |= LED1;
        LED_state = true;
    }
    //off
    if(status == OFF){
        P1OUT &= ~LED1;
        LED_state = false;
    }
    //return if neither
    return;
};

//*****************************
// Program is where the SPI
// sending happens
//*****************************
void Program(void){
    //initialize the buffer for storage
    //circ_bbuf_t *buffer_table;
    //uint8_t count;
    //uint8_t *data_store = malloc(sizeof(uint8_t));
    //RESULT_e results;

    //initialize the buffer_table with 10 locations
    //buffer_table = buffer_init(10);

    //enable global interrupt
    __bis_SR_register(GIE); //enable global interrupts

    //send the ADS1247 device init to setup the ADC for PT100 measurements
    RTC_init();

    //timer start
    Timer_start(TIMER_ON);
    //*******************
    //kernal while loop for interrupt operation
    while(1){
        //if the flags are triggered from interrupts
        switch(flags){
            case UART_RCV_INT:
                //LED ON
                LED(ON);
                //turn off kernal flag so tx only happens once
                flags &= ~UART_RCV_INT;
                //send SPI data out
                SPI_send_blocking(SPIB_e, uart_rcv);
                break;
            default:
                LED(OFF);
                break;
        }

        //check trigger button
        //check_button();

        //if the trigger is on start program
        if(start_trigger == true){
            //if the start trigger is push
            //start reflow_program()
            continue;
        }
    }
    //end of kernal loop
    //**************
}

//********************************************************
//********************End of File*************************
//********************************************************
