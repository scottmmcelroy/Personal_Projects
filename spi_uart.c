//*******************************************************************************
// Project: SPI_COMM
// File: spi_uart.c
// Description: spi and uart source file for spi_uart controller
// Author: Scott McElroy
// Date: 12-26-2022
// License: Not to be used without written consent directly from the author above
//*******************************************************************************

//*****************/
// Includes
//*****************/
#include <msp430.h>
//#include <msp430g2553.h>
//#include "buffer.h"
#include "spi_uart.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
//*****************/
// Defines
//*****************/
//SPI - USCI_A0
#define UARTTX 0x4  //port1.2 - transmit out
#define UARTRX 0x2  //port1.1 - receive in
//SPI - USCI_A0
#define SPIA_SIMO 0x04 //port1.2 - master out
#define SPIA_SOMI 0x02 //port1.1 - slave out
#define SPIA_CLK 0x10  //port1.4 - clock
#define SPIA_STE 0x20  //port1.5 - slave transmit enable (for multi-master)
#define SPIA_CS 0x01   //port2.0 - Chip Select (b/c UART not used)
//SPI - USCI_B0
#define SPIB_SIMO 0x80 //port1.7 - master out
#define SPIB_SOMI 0x40 //port1.6 - slave out
#define SPIB_CLK 0x20  //port1.5 - clock
#define SPIB_STE 0x10  //port1.4 - slave transmit enable (for multi-master)
#define SPIB_CS 0x08   //port1.3 - Chip Select (b/c 1.3 not used)

//Settings
#define SPI_PRESCALER_LOW 16 //at 16MHz, sets the bit rate to 1Mbps
#define SPI_PRESCALER_HIGH 0 //at 16MHz, sets the bit rate to 1Mbps
//UART
#define UART_BAUD_DIVIDER_LOW 104 //at 16MHz, sets BAUD to 9600
#define UART_BAUD_DIVIDER_HIGH 0
#define UCBRFX_MOD (0x70) //value of 3 shifted up to proper value
#define UCBRSX_MOD 0

//*******************/
// Globals
//*******************/
#define SPI_INT 0B10
//flags can be 8 different values 0b00000000
extern uint8_t flags = 0;
extern uint8_t uart_rcv = 0;
//*****************/
// Functions
// USCI Lines
// UART: RXD/TXD
// SPI: SOMI/SIMO/CLK/STE
//*****************/


//*****************************************//
// Initialization and port setup of the USCI
// Sets port function and direction
// SPI Needs a CS pin (STE is used for Multiple Master Mode)
//*****************************************//
void USCI_init(usci_port_e port, usci_type_e type){
    //**********
    switch(port){
    case USCI_A_e:
        //if A0 then always port 1
        if(type == SPIA_e){
            //SPIA:::::::
                //set direction
            P1OUT &= ~SPIA_CS;
            P1DIR |= (SPIA_SIMO + SPIA_CLK + SPIA_STE + SPIA_CS); //set outs
            P1DIR &= ~SPIA_SOMI; //set the input
               //set function - all 4 pins USCI
            P1SEL |= (SPIA_SIMO + SPIA_SOMI + SPIA_CLK + SPIA_STE);
            P1SEL2 |= (SPIA_SIMO + SPIA_SOMI + SPIA_CLK + SPIA_STE);
            //SOMI - P1.1 - IN
            //SIMO - P1.2 - OUT
            //CLK -  P1.4 - OUT
            //STE -  P1.5 - OUT
            //****************
        }else if(type == UARTA_e){
            //UARTA::::::
                //set direction
            P1DIR |= (UARTTX); //set outs
            P1DIR &= ~UARTRX; //set the input
                //set function
            P1SEL |= (UARTTX + UARTRX);
            P1SEL2 |= (UARTTX + UARTRX);
            //RXD -  P1.1 - In
            //TXD -  P1.2 - Out
            //****************
        }
        break;

    case USCI_B_e:
        //if B0 then always port 1
        if(type == SPIB_e){
            //SPIB::::::
                //set direction
            P1OUT &= ~SPIB_CS;
            P1DIR |= (SPIB_SIMO + SPIB_CLK + SPIB_STE + SPIB_CS); //set outs
            P1DIR &= ~SPIB_SOMI; //set the input
                //set function
            P1SEL |= (SPIB_SIMO + SPIB_SOMI + SPIB_CLK + SPIB_STE);
            P1SEL2 |= (SPIB_SIMO + SPIB_SOMI + SPIB_CLK + SPIB_STE);
            //SOMI - P1.6 - IN
            //SIMO - P1.7 - OUT
            //CLK -  P1.5 - OUT
            //STE -  P1.4 - OUT
            //*****************
        }else if(type == UARTB_e){
            //No UART on B0 of the MSP430G2553
            //**************//
        }
        break;

    case USCI_AB_e:
        //if B0 then always port 1
        if(type == SPIAB_e){
            //SPI cannot be used in both PORTs in MSP430G2553
            //***********//
        }else if(type == UARTA_SPIB_e){
            //UARTA::::::
                //set direction
            P1DIR |= (UARTTX); //set outs
            P1DIR &= ~UARTRX; //set the input
                //set function
            P1SEL |= (UARTTX + UARTRX);
            P1SEL2 |= (UARTTX + UARTRX);
            //RXD -  P1.1 - In
            //TXD -  P1.2 - Out
            //***************

            //SPIB:::::::
                //Set direction
            P1OUT &= ~SPIB_CS;
            P1DIR |= (SPIB_SIMO + SPIB_CLK + SPIB_STE + SPIB_CS); //set outs
            P1DIR &= ~SPIB_SOMI; //set the input
                //set function
            P1SEL |= (SPIB_SIMO + SPIB_SOMI + SPIB_CLK + SPIB_STE);
            P1SEL2 |= (SPIB_SIMO + SPIB_SOMI + SPIB_CLK + SPIB_STE);
            //SOMI - P1.6 - IN
            //SIMO - P1.7 - OUT
            //CLK -  P1.5 - OUT
            //STE -  P1.4 - OUT
            //*****************
        }
        break;
    }//end of case statement
}//end of function

//****************************
// Initialize the SPI clocking
// Function also Initializes CS
//****************************
void SPI_Init(usci_type_e type){
    //*********
    if(type == SPIA_e){
        //turn off the USCI
        UCA0CTL1 |= UCSWRST;
        //setup USCI_A0 SPI interface for the ADS1247
        //set data length, LSB or MSB first, 3 pin operation, MSB first, Master Mode,
        UCA0CTL0 = UCCKPH + UCMSB + UCMST + UCSYNC; //(0b10101001)
        UCA0CTL1 = UCSSEL1 + UCSSEL0; //set the clock to be SMCLK
        //start the SPI interface by UCSWRST to 0b1
        UCA0BR0 = SPI_PRESCALER_LOW; //sets the bit rate of the SPI interface
        UCA0BR1 = SPI_PRESCALER_HIGH;
        UCA0CTL1 &= ~UCSWRST;

    }else if(type == SPIB_e){
        //turn off the USCI
        UCB0CTL1 |= UCSWRST;
        //setup USCI_B0 SPI interface for the ADS1247
        //set data length, LSB or MSB first, 3 pin operation, MSB first, Master Mode,
        UCB0CTL0 = UCCKPH + UCMSB + UCMST + UCSYNC; //(0b10101001)
        UCB0CTL1 = UCSSEL1 + UCSSEL0; //set the clock to be SMCLK
        //start the SPI interface by UCSWRST to 0b1
        UCB0BR0 = SPI_PRESCALER_LOW; //sets the bit rate of the SPI interface
        UCB0BR1 = SPI_PRESCALER_HIGH;
        UCB0CTL1 &= ~UCSWRST;
    }
    //initialize the CS
    SPI_CS_Init(type);
}//end of function

//****************************
// Initialize the UART clocking
//
//****************************
void UART_Init(usci_type_e type){
    //*********
    if(type == UARTA_e){
        //setup the USCI_A0 UART interface
        UCA0CTL1 |= UCSWRST; // put the device into reset
        UCA0CTL0 = 0; //no parity, LSB first, 8-bit, One Stop Bit, UART, Async
        UCA0CTL1 = UCSSEL1 + UCSSEL0 + UCSWRST; //set the clock to be SMCLK = BRCLK
        //UCBRx = 104
        //UCBRSx = 0
        //UCBRFx = 3
        //UCA0BR0 = UART_BAUD_DIVIDER_LOW;
        UCA0BR0 = 104;
        //UCA0BR1 = UART_BAUD_DIVIDER_HIGH;
        UCA0BR1 = 0;
        //UCA0MCTL = UCBRFX_MOD + UCBRSX_MOD;
        UCA0MCTL = UCBRS0; //modulation set to 1
        UCA0CTL1 &= ~UCSWRST;

    }else if(type == UARTB_e){
        //UARTB0 doesn't exist in in MSP430G2553
    }
}//end of function

//*****************************
// Enable the UART function
// Accepts UART on and UART off
//*****************************
void UART_Enable(usci_type_e type, UART_STATUS status){
    //turn on or off with interrupts
    if(type == UARTA_e){
        if(status == UART_ON){
            UCA0CTL1 &= ~UCSWRST;
            UC0IE |= UCA0RXIE; //Enable USCI_A0 RX interrupt
        }
        //off
        if(status == UART_OFF){
            UCA0CTL1 |= UCSWRST;
            UC0IE &= ~UCA0RXIE; //Enable USCI_A0 RX interrupt
        }
    }else if(type == UARTB_e){
        //UARTB0 doesn't exist in in MSP430G2553
    }
    //*************
}//end of function

//*****************************
// Enable the UART function
// Accepts UART on and UART off
//*****************************
void SPI_Enable(usci_type_e type, SPI_STATUS status){
    //turn on or off
    if(type == SPIA_e){
        if(status == SPI_ON){
            UCA0CTL1 &= ~UCSWRST;
        }
        //off
        if(status == SPI_OFF){
            UCA0CTL1 |= ~UCSWRST;
        }
    }else if(type == SPIB_e){
        if(status == SPI_ON){
            UCB0CTL1 &= ~UCSWRST;
        }
        //off
        if(status == SPI_OFF){
            UCB0CTL1 |= UCSWRST;
        }
    }
    //************
}//end of function

//************************************
// Initialize the Chip select pin
//
//************************************
void SPI_CS_Init(usci_type_e type){
    //which port is being used
    if(type == SPIA_e){
        //set gpio
        //chip select is port2.0
        P2SEL &= ~SPIA_CS;
        P2SEL2 &= ~SPIA_CS;
        //set direction
        P2DIR |= SPIA_CS;
        //write high first since /CS (Active Low)
        P2OUT |= SPIA_CS;
    }else if(type == SPIB_e){
        //set gpio
        //chip select is port1.3
        P1SEL &= ~SPIB_CS;
        P1SEL2 &= ~SPIB_CS;
        //set direction
        P1DIR |= SPIB_CS;
        //write high first since /CS (Active Low)
        P1OUT |= SPIB_CS;
    }
    //************
}//end of function

//************************************
// Set the Chip select pin high or low
//
//************************************
void SPI_CS_ON(usci_type_e type, CS_STATUS status){
    //check to see if CS pin needs to be high or low
    //CS needs to stay active for entire SPI transaction
    //deactivating CS resets SPI communication
    if(type == SPIA_e){
        if(status == CS_ON){
            P2OUT &= ~SPIA_CS;
        }else if(status == CS_OFF){
            P2OUT |= SPIA_CS;
        }
    }else if(type == SPIB_e){
        if(status == CS_ON){
            P1OUT &= ~SPIB_CS;
        }else if(status == CS_OFF){
            P1OUT |= SPIB_CS;
        }
    }
    //************
}//end of function

//====================================
//************************************
//*****Send SPI data - blocking*******
// 8- bits of data at a time
//************************************
//====================================
void SPI_send_blocking(usci_type_e type, uint8_t data){
    //determine which SPI
    if(type == SPIA_e){
        //trigger the CS pin CS needs to stay low for multiple packets
        SPI_CS_ON(SPIA_e, CS_ON);
        //wait for USBUSY to be complete
        while(UCA0STAT & UCBUSY);
        //load the UCB0TXBUF
        UCA0TXBUF = data;
        //wait for USBUSY to be complete
        while(UCA0STAT & UCBUSY);
        //turn off the CS
        SPI_CS_ON(SPIA_e, CS_OFF);

    }else if(type == SPIB_e){
        //trigger the CS pin CS needs to stay low for multiple packets
        SPI_CS_ON(SPIB_e, CS_ON);
        //wait for USBUSY to be complete
        while(UCB0STAT & UCBUSY);
        //load the UCB0TXBUF
        UCB0TXBUF = data;
        //wait for USBUSY to be complete
        while(UCB0STAT & UCBUSY);
        //turn off the CS
        SPI_CS_ON(SPIB_e, CS_OFF);
    }
}//end of send blocking



//*****************************
//interrupt service vectors
//*****************************
//uart receive
#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void){
    //interrupt because a character was received
    //store kernal flags
    flags = UART_RCV_INT;
    uart_rcv = UCA0RXBUF;   //store the value into a global
    //if user input, always send it back to the terminal
    while(!(IFG2&UCA0TXIFG)); //USCI_A0 TX buffer ready?
    UCA0TXBUF = uart_rcv;      //TX -> RXed character
}

//uart transmit
#pragma vector=USCIAB0TX_VECTOR
__interrupt void USCI0TX_ISR(void){
    //interrupt for a transmit buffer is empty
    while(!(IFG2&UCA0TXIFG));
    UCA0TXBUF = 'A';
}

//********************************************************
//********************End of File*************************
//********************************************************
