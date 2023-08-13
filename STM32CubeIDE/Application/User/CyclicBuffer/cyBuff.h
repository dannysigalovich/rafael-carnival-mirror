/*
 * cyBuff.h
 *
 *  Created on: Aug 13, 2023
 *      Author: eladsez
 */

#ifndef APPLICATION_USER_CYCLICBUFFER_CYBUFF_H_
#define APPLICATION_USER_CYCLICBUFFER_CYBUFF_H_

#include <stdint.h>

#define BUFFER_SIZE 1024

typedef struct {
    char data[BUFFER_SIZE];
    uint32_t head;
    uint32_t tail;
    uint32_t count;
} CircularBuffer;

void initCircularBuffer(CircularBuffer *cb);
int isCircularBufferFull(CircularBuffer *cb);
int isCircularBufferEmpty(CircularBuffer *cb);
int writeToBuff(CircularBuffer *cb, const char *buff, uint32_t size);
int readFromBuff(CircularBuffer *cb, char *buff, uint32_t size);

#endif /* APPLICATION_USER_CYCLICBUFFER_CYBUFF_H_ */
