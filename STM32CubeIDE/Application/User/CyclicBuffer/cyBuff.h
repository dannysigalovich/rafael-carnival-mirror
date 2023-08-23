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

typedef union INSData{
	INSPVA PVAData[BUFFER_SIZE];
	INSSTDEV STDData[BUFFER_SIZE];
}INSData;

typedef struct {
	INSData data; // Change the data type to hold INSPVA structures
    uint32_t head;
    uint32_t tail;
    uint32_t count;
    sys_mutex_t mutex; // Add a mutex for synchronization
    INSType type;
} CircularBuffer;

void initCircularBuffer(CircularBuffer *cb, INSType type);
int isCircularBufferFull(CircularBuffer *cb);
int isCircularBufferEmpty(CircularBuffer *cb);
int writeToBuff(CircularBuffer *cb, const void *buff, uint8_t size);
int readINSPVA(CircularBuffer *cb, INSPVA *ins);
int readINSSTD(CircularBuffer *cb, INSSTDEV *ins);

#endif /* APPLICATION_USER_CYCLICBUFFER_CYBUFF_H_ */
