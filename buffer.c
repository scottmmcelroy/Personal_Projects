//*******************************************************************************
// Project: SPI_COMM
// File: buffer.c
// Description: buffer source file for RTD controller
// Author: Scott McElroy
// Date: 11-6-2022
// License: Not to be used without written consent directly from the author above
//*******************************************************************************

//*****************/
// Includes
//*****************/
#include <msp430.h>
//#include <msp430g2553.h>
#include "buffer.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
//*****************/
// Defines
//*****************/
#define SIZE 5

//*****************/
// Functions
//*****************/
//circular buffer will grow by provided size, however, a full buffer will overwrite values are being full
//buffer init returns a pointer, and accepts a size need
//MSP430 has a ram of 512 bytes, so function need to limit the size of the buffer
circ_bbuf_t *buffer_init(uint8_t size){
    //initialize the buffer and size
    //first create a pointer to the circular buffer table
    circ_bbuf_t *new_buffer;
    //limit the buffer size to 25
    if(size > 25){
        return 0;
    }

    //memory creation for the buffer table
    new_buffer = malloc(sizeof(circ_bbuf_t));
    //fill the metadata table
    new_buffer->head = 0;
    new_buffer->tail = 0;
    new_buffer->count = 0;
    new_buffer->max = size;
    new_buffer->full = false;
    new_buffer->empty = true;
    //memory allocation for where the data will be held
    new_buffer->location = malloc(sizeof(uint8_t) * size);

    return new_buffer;
};

//**********************
RESULT_e buffer_add(circ_bbuf_t *buffer, uint8_t data){
    //check to see if the pointer is actual
    if(buffer == NULL){
        return NOT_PASS;
    }

    //check to see if the buffer is full and move the head
    //and tail indicators
    if((buffer->head == buffer->tail) && (buffer->empty == true)){
        //if the buffer is empty add right away
        *((buffer->location) + (buffer->head)) = data;
        //ensure that the buffer is no longer empty
        buffer->empty = false;
        buffer->count += 1;
    }else{
        //if the buffer is full both head and tail need to move before adding data
        if(buffer->full == true){
            //increase head but wrap around if = max
            if(buffer->head == ((buffer->max)-1)){
                buffer->head = 0;
            }else{
                buffer->head += 1;
            }
            //increase tail but wrap if =max
            if(buffer->tail == ((buffer->max)-1)){
                buffer->tail = 0;
            }else{
                buffer->tail += 1;
            }
        }else{
            //if buffer is not full, only the head needs to move to add data
            if(buffer->head == ((buffer->max)-1)){
                buffer->head = 0;
            }else{
                buffer->head += 1;
            }
            //check to see if this data fills the buffer
            buffer->count += 1;
            if(buffer->count == buffer->max){
                buffer->full = true;
            }
        }

        //above moves the offsets in the buffer table, data can be stored at the buffer storage location
        *((buffer->location) + (buffer->head)) = data;
    }

    return PASS;
};

//**********************
RESULT_e buffer_remove(circ_bbuf_t *buffer, uint8_t *data){

    //localized storage
    uint8_t data_local = 0;

    if(buffer == NULL){
        return NOT_PASS;
    }

    //if the buffer is full, set to not full
    if(buffer->full == true){
        buffer->full = false;
        //skip to below to remove data
    }else{
        //if the buffer is empty return EMPTY
        if((buffer->head == buffer->tail) && (buffer->empty == true)){
            //if the head and tail are the same the buffer is empty
            return EMPTY;
        }
    }

    //reduce count because data is being removed
    buffer->count -= 1; //at empty count should drop to zero
    //pull data from the location plus tail distance
    data_local = *((buffer->location) + (buffer->tail));
    *data = data_local;
    //move the tail location
    if(buffer->tail == buffer->head){
        //if the tail is already at the head only one spot is full
        buffer->empty = true;
        //tail does not need to move
    }else{
        //if there are more than one item tail needs to advance
        if(buffer->tail == ((buffer->max)-1)){
            buffer->tail = 0;
        }else{
            buffer->tail += 1;
        }
    }
    //return the data back to the function call
    return PASS;
};

//**********************
bool buffer_full(circ_bbuf_t *buffer){
    //check the buffer table whether the circular buffer is full
    if(buffer->full == true){
        return true;
    }
    //if not full return false
    return false;
};

//***********************
bool buffer_empty(circ_bbuf_t *buffer){
    //first check to see if the buffer is full
    if(buffer->empty == true){
        return true;
    }
    //if the above doesnt return properly
    return false;
};

//**********************
void buffer_free(circ_bbuf_t *buffer){
    //free both the pointer from the buffer and the buffer table
    free(buffer->location);
    buffer->location = NULL;
    free(buffer);
    buffer = NULL;
};

//**********************
//unit test the buffer
RESULT_e buffer_test_add_remove(void){
    //initialize the circular buffer
    RESULT_e status;
    uint8_t count = 0;
    uint8_t *data_store = malloc(sizeof(uint8_t));
    //buffer table
    circ_bbuf_t *buffer_table;
    //data to test add or remove
    uint8_t data[SIZE] = {'A','B','C','D','E'};

    //************
    //test 1a: add a data item and remove it
    //initial a circular buffer to use
    buffer_table = buffer_init(SIZE);
    //buffer add
    buffer_add(buffer_table, data[0]);
    //if UART is enable send to console what value is being tested
    //UCA0TXBUF = data[0];
    //buffer remove
    if((buffer_table->head == 0 && buffer_table->tail == 0) && (buffer_table->empty == false)){
        //if the head moved the table is working correctly
        buffer_remove(buffer_table, data_store);
    }else{
        data_store = 0;
        return NOT_PASS;
    }

    //check results
    if(*data_store == data[0]){
        //if UART is enable send to console
        UCA0TXBUF = *data_store;
        //check that the head and tail are moving
        if(buffer_table->head == 0 && buffer_table->tail == 0){
            status = PASS;
        }

    }else{
        //if UART is connected send incorrect value to console
        UCA0TXBUF = 'F';
        return NOT_PASS;
    }
    //*************

    //*************
    //test 1b: load and remove
    //add a bunch of data to the buffer
    //buffer add
    for( count=0; count<5; count++ ){
        //store the data array into the buffer
        buffer_add(buffer_table, data[count]);
    }
    //remove a bunch of data from the buffer
    for( count=0; count<5; count++ ){
        //first remove each value
        buffer_remove(buffer_table, data_store);
        //check to see if each value is correct
        if( *data_store != data[count]){
            return NOT_PASS;
        }
        //transmit value over UART
        UCA0TXBUF = *data_store;
    }

    //free the buffer to be able to use the data again
    buffer_free(buffer_table);
    free(data_store);
    data_store = NULL;
    //did the results pass

    //*************
    return status;
};

//**********************
//unit test the buffer
RESULT_e buffer_test_empty_remove(void){
    //*************
    RESULT_e status;
    uint8_t *data_store = malloc(sizeof(uint8_t));
    circ_bbuf_t *buffer_table;
    //test 2: removing if empty
    //initial a circular buffer to use
    buffer_table = buffer_init(SIZE);
    status = buffer_remove(buffer_table, data_store);
    //free the buffer to be able to use the data again
    buffer_free(buffer_table);
    free(data_store);
    data_store = NULL;
    //did the results pass
    if(status != EMPTY){
        return NOT_PASS;
    }
    //*************
    return status;
};

//**********************
//unit test the buffer
RESULT_e buffer_test_overflow(void){
    //*************
    //test 3: overflowing buffer and see if the data is lost and the tail continues to move
    //*************
    //initialize the circular buffer
    RESULT_e status;
    uint8_t count = 0;
    uint8_t *data_store = malloc(sizeof(uint8_t));
    //buffer table
    circ_bbuf_t *buffer_table;
    //data to test add or remove
    uint8_t data[SIZE + 1] = {'A','B','C','D','E','F'};

    //initial a circular buffer to use
    buffer_table = buffer_init(SIZE);

    //buffer is only 5 spaces long, so fill the buffer to 6
    //the buffer should wrap and see an overwrite of the first value of F
    //buffer add to wrap around and store the first value
    for( count=0; count<6; count++ ){
        //store the data array into the buffer
        buffer_add(buffer_table, data[count]);
    }
    //remove the first input value
    buffer_remove(buffer_table, data_store);

    //check to see if the data was overwritten
    if( *data_store == data[1]){
        //the value that returns will have the tail moved up to the second spot
        //so the return value should be 'B'
        status = PASS;
    }else{
        status = NOT_PASS;
    }

    //free the buffer_table and the data_store
    buffer_free(buffer_table);
    free(data_store);
    data_store = NULL;

    return status;
};


//**********************
//unit test the buffer
RESULT_e buffer_test_middle_add_remove(void){
    //*************
    //test 4: fill the buffer 150% to check to see if the buffer proper loads and unload not from zero
    //This test will help determine if the head and tail are working correctly
    //*************
    //initialize the circular buffer
    RESULT_e status;
    bool output;
    uint8_t count = 0;
    uint8_t *data_store = malloc(sizeof(uint8_t));
    //buffer table
    circ_bbuf_t *buffer_table;
    //data to test add or remove
    uint8_t data[SIZE - 2] = {'A','B','C'};

    //initial a circular buffer to use
    buffer_table = buffer_init(SIZE);

    //change the head and tail location so data can be added to the middle of the buffer
    buffer_table->head = 2;
    buffer_table->tail = 2;

    //check to make sure the buffer still reads empty when the head and tail are moved
    output = buffer_empty(buffer_table);
    if(output == true){
        status = PASS;
    }

    //add the data to the buffer, this will add 3 values to the buffer in the middle
    for( count=0; count<3; count++ ){
        //store the data array into the buffer
        buffer_add(buffer_table, data[count]);
    }
    //remove the values and see if the buffer removes properly
    //the tail should start pulling data from where the data was put
    for( count=0; count<3; count++ ){
        //first remove each value
        buffer_remove(buffer_table, data_store);
        //check to see if each value is correct
        if( *data_store != data[count]){
            return NOT_PASS;
        }
    }

    //if we got to this point the buffer has completed
    return status;
};


//********************************************************
//********************End of File*************************
//********************************************************

