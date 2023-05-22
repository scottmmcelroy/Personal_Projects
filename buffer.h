//*******************************************************************************
// Project: SPI_COMM
// File: buffer.h
// Description: buffer header file for RTD controller
// Author: Scott McElroy
// Date: 12-26-2022
// License: Not to be used without written consent directly from the author above
//*******************************************************************************

#ifndef BUFFER_H_
#define BUFFER_H_

//*****************/
// Includes
//*****************/
#include <msp430.h>
//#include <msp430g2553.h>
#include <stdint.h>
#include <stdbool.h>
//*****************/
// Defines
//*****************/
typedef struct{
    uint16_t *location; //pointer value of the start of the buffer
    uint8_t head; //first data point that went in
    uint8_t tail; //first data point to be removed
    uint8_t count; //total count in the buffer
    uint8_t max; //max length of the buffer
    bool full; //truth statement for whether the buffer is full
    bool empty;
}circ_bbuf_t;

typedef enum{
    eNULL,
    PASS,
    NOT_PASS,
    FULL,
    EMPTY
}RESULT_e;

//*****************/
// Functions
//*****************/
//buffer init returns a pointer, and excepts a size need
circ_bbuf_t *buffer_init(uint8_t size);

//provide data value to the buffer
RESULT_e buffer_add(circ_bbuf_t *buffer, uint8_t data);

//provide a pointer to where the data is to be put
RESULT_e buffer_remove(circ_bbuf_t *buffer, uint8_t *data);

//is the buffer full
bool buffer_full(circ_bbuf_t *buffer);

//is the buffer empty
bool buffer_empty(circ_bbuf_t *buffer);

//free the buffer pointer
void buffer_free(circ_bbuf_t *buffer);

//buffer test
RESULT_e buffer_test_add_remove(void);

RESULT_e buffer_test_empty_remove(void);

RESULT_e buffer_test_overflow(void);

RESULT_e buffer_test_middle_add_remove(void);

#endif /* BUFFER_H_ */
//********************************************************
//********************End of File*************************
//********************************************************
