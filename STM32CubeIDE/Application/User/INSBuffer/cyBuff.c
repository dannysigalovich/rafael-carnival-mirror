/*
 * cyBuff.c
 *
 *  Created on: Aug 13, 2023
 *      Author: eladsez
 */


#include <INSBuffer/cyBuff.h>
#include "lwip/err.h"
#include <string.h>

void initCircularBuffer(CircularBuffer *cb, INSType type) {
    cb->type = type;
    sys_mutex_new(&cb->mutex);
}


int writeToBuff(CircularBuffer *cb, const void *buff, uint8_t size) {
    sys_mutex_lock(&cb->mutex);

    void *cbData = cb->type == INSPVAType ? (void *) &(cb->data.PVAData) : (void *) &(cb->data.STDData);

    // Copy the data from the input buffer to the buffer
    memcpy(cbData, buff, size);

    sys_mutex_unlock(&cb->mutex);
    return ERR_OK;
}

int readFromBuff(CircularBuffer *cb, void *buff, uint8_t size) {
    sys_mutex_lock(&cb->mutex);

    void *cbData = cb->type == INSPVAType ? (void *) &(cb->data.PVAData) : (void *) &(cb->data.STDData);

    // Copy the data from the  buffer to the output buffer
    memcpy(buff, cbData, size);

    sys_mutex_unlock(&cb->mutex);
    return ERR_OK;
}


int readINSPVA(CircularBuffer *cb, INSPVA *ins){
	return readFromBuff(cb, ins, sizeof(INSPVA));
}


int readINSSTD(CircularBuffer *cb, INSSTDEV *ins){
	return readFromBuff(cb, ins, sizeof(INSSTDEV));
}
