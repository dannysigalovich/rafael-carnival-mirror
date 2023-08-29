/*
 * cyBuff.c
 *
 *  Created on: Aug 13, 2023
 *      Author: eladsez
 */


#include <INSBuffer/cyBuff.h>
#include "lwip/err.h"
#include <string.h>

void initCircularBuffer(CircularBuffer *cb) {
    sys_mutex_new(&cb->mutex);
}


int writeToBuff(CircularBuffer *cb, const void *buff, uint8_t size) {
    sys_mutex_lock(&cb->mutex);

    // Copy the data from the input buffer to the buffer
    memcpy(&(cb->data), buff, size);

    sys_mutex_unlock(&(cb->mutex));
    return ERR_OK;
}

int readFromBuff(CircularBuffer *cb, void *buff, uint8_t size) {
    sys_mutex_lock(&cb->mutex);

    // Copy the data from the  buffer to the output buffer
    memcpy(buff, &(cb->data), size);

    sys_mutex_unlock(&(cb->mutex));
    return ERR_OK;
}

int readINSPVAX(CircularBuffer *cb, INSPVAX *inspvax){
	return readFromBuff(cb, inspvax, sizeof(INSPVAX));
}


