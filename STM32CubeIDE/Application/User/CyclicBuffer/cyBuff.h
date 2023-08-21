/*
 * cyBuff.h
 *
 *  Created on: Aug 13, 2023
 *      Author: eladsez
 */

#ifndef APPLICATION_USER_CYCLICBUFFER_CYBUFF_H_
#define APPLICATION_USER_CYCLICBUFFER_CYBUFF_H_

#include <stdint.h>
#include "lwip/sys.h" // Include the necessary header for STM32 semaphore functions
#include "Novatel/navMesseging.h"

#define BUFFER_SIZE 2

typedef struct {
    INSPVA data[BUFFER_SIZE]; // Change the data type to hold INSPVA structures
    uint32_t head;
    uint32_t tail;
    uint32_t count;
    sys_mutex_t mutex; // Add a mutex for synchronization
} CircularBuffer;

void initCircularBuffer(CircularBuffer *cb);
int isCircularBufferFull(CircularBuffer *cb);
int isCircularBufferEmpty(CircularBuffer *cb);
int writeToBuff(CircularBuffer *cb, const INSPVA *buff);
int readFromBuff(CircularBuffer *cb, INSPVA *buff);

#endif /* APPLICATION_USER_CYCLICBUFFER_CYBUFF_H_ */
