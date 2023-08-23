/*
 * cyBuff.h
 *
 *  Created on: Aug 13, 2023
 *      Author: eladsez
 */

#ifndef APPLICATION_USER_INSBUFFER_CYBUFF_H_
#define APPLICATION_USER_INSBUFFER_CYBUFF_H_

#include <stdint.h>
#include "lwip/sys.h" // Include the necessary header for STM32 semaphore functions
#include "Novatel/navMesseging.h"

typedef union INSData{
	INSPVA PVAData;
	INSSTDEV STDData;
}INSData;

typedef struct {
	INSData data; // Change the data type to hold INSPVA structures
    sys_mutex_t mutex; // Add a mutex for synchronization
    INSType type;
} CircularBuffer;

void initCircularBuffer(CircularBuffer *cb, INSType type);
int isCircularBufferFull(CircularBuffer *cb);
int isCircularBufferEmpty(CircularBuffer *cb);
int writeToBuff(CircularBuffer *cb, const void *buff, uint8_t size);
int readINSPVA(CircularBuffer *cb, INSPVA *ins);
int readINSSTD(CircularBuffer *cb, INSSTDEV *ins);

#endif /* APPLICATION_USER_INSBUFFER_CYBUFF_H_ */
