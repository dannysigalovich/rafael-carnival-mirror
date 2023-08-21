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
    sys_mutex_new(&cb->mutex);
}

int isCircularBufferFull(CircularBuffer *cb) {
    return (cb->count == BUFFER_SIZE);
}

int isCircularBufferEmpty(CircularBuffer *cb) {
    return (cb->count == 0);
}

int writeToBuff(CircularBuffer *cb, const INSPVA *buff) {
    sys_mutex_lock(&cb->mutex);

    if (cb->count == BUFFER_SIZE) {
        // Buffer is full, overwrite the oldest entry
        cb->tail = (cb->tail + 1) % BUFFER_SIZE;
        cb->count--;
    }

    // Copy the data from the input buffer to the circular buffer
    cb->data[cb->head] = *buff;
    cb->head = (cb->head + 1) % BUFFER_SIZE;
    cb->count++;

    sys_mutex_unlock(&cb->mutex);
    return ERR_OK;
}

int readFromBuff(CircularBuffer *cb, INSPVA *buff) {
    sys_mutex_lock(&cb->mutex);

    if (isCircularBufferEmpty(cb)) {
        sys_mutex_unlock(&cb->mutex);
        return ERR_BUF; // Buffer is empty, cannot read
    }

    // Copy the data from the circular buffer to the output buffer
    *buff = cb->data[cb->tail];
    cb->tail = (cb->tail + 1) % BUFFER_SIZE;
    cb->count--;

    sys_mutex_unlock(&cb->mutex);
    return ERR_OK;
}
