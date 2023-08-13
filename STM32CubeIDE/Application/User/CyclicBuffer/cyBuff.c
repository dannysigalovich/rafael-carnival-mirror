/*
 * cyBuff.c
 *
 *  Created on: Aug 13, 2023
 *      Author: eladsez
 */


#include "cyBuff.h"
#include "lwip/err.h"

void initCircularBuffer(CircularBuffer *cb) {
    cb->head = 0;
    cb->tail = 0;
    cb->count = 0;
}

int isCircularBufferFull(CircularBuffer *cb) {
    return (cb->count == BUFFER_SIZE);
}

int isCircularBufferEmpty(CircularBuffer *cb) {
    return (cb->count == 0);
}

int writeToBuff(CircularBuffer *cb, const char *buff, uint32_t size) {
    if (isCircularBufferFull(cb)) {
        return ERR_MEM; // Buffer is full, cannot write
    }

    for (uint32_t i = 0; i < size; i++) {
        cb->data[cb->head] = buff[i];
        cb->head = (cb->head + 1) % BUFFER_SIZE;
        cb->count++;
    }

    return ERR_OK; // Success
}

int readFromBuff(CircularBuffer *cb, char *buff, uint32_t size) {
    if (isCircularBufferEmpty(cb)) {
        return ERR_BUF; // Buffer is empty, cannot read
    }

    for (uint32_t i = 0; i < size; i++) {
        buff[i] = cb->data[cb->tail];
        cb->tail = (cb->tail + 1) % BUFFER_SIZE;
        cb->count--;
    }

    return ERR_OK; // Success
}
