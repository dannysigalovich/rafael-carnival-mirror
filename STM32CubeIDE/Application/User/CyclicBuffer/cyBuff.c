/*
 * cyBuff.c
 *
 *  Created on: Aug 13, 2023
 *      Author: eladsez
 */


#include "cyBuff.h"
#include "lwip/err.h"
#include <string.h>

void initCircularBuffer(CircularBuffer *cb, INSType type) {
    cb->head = 0;
    cb->tail = 0;
    cb->count = 0;
    cb->type = type;
    sys_mutex_new(&cb->mutex);
}

int isCircularBufferFull(CircularBuffer *cb) {
    return (cb->count == BUFFER_SIZE);
}

int isCircularBufferEmpty(CircularBuffer *cb) {
    return (cb->count == 0);
}

int writeToBuff(CircularBuffer *cb, const void *buff, uint8_t size) {
    sys_mutex_lock(&cb->mutex);

    void *cbData = cb->type == INSPVAType ? cb->data.PVAData : cb->data.STDData;

    if (cb->count == BUFFER_SIZE) {
        // Buffer is full, overwrite the oldest entry
        cb->tail = (cb->tail + 1) % BUFFER_SIZE;
        cb->count--;
    }

    // Copy the data from the input buffer to the circular buffer
    memcpy(&cbData[cb->head], buff, size);
    cb->head = (cb->head + 1) % BUFFER_SIZE;
    cb->count++;

    sys_mutex_unlock(&cb->mutex);
    return ERR_OK;
}

int readFromBuff(CircularBuffer *cb, void *buff, uint8_t size) {
    sys_mutex_lock(&cb->mutex);

    void *cbData = cb->type == INSPVAType ? cb->data.PVAData : cb->data.STDData;

    if (isCircularBufferEmpty(cb)) {
        sys_mutex_unlock(&cb->mutex);
        return ERR_BUF; // Buffer is empty, cannot read
    }

    // Copy the data from the circular buffer to the output buffer
    memcpy(buff, &cbData[cb->head], size);
    cb->tail = (cb->tail + 1) % BUFFER_SIZE;
    cb->count--;

    sys_mutex_unlock(&cb->mutex);
    return ERR_OK;
}


int readINSPVA(CircularBuffer *cb, INSPVA *ins){
	return readFromBuff(cb, ins, sizeof(INSPVA));
}


int readINSSTD(CircularBuffer *cb, INSSTDEV *ins){
	return readFromBuff(cb, ins, sizeof(INSSTDEV));
}
