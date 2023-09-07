/*
 * i2c.h
 *
 *  Created on: Aug 13, 2023
 *      Author: eladsez
 */

#ifndef APPLICATION_USER_I2C_I2C_C_
#define APPLICATION_USER_I2C_I2C_C_

#include "stm32h7xx_hal.h"
#include "stm32h7xx_nucleo.h"
#include "stm32h7xx_hal_i2c.h"
#include "cmsis_os.h"
#include "main.h"
#include "ICD/ICD.h"
#include "stdbool.h"

/* Size of Transmission buffer */
#define TXBUFFERSIZE                    256 /* should be enough for all messages */
/* Size of Reception buffer */
#define RXBUFFERSIZE					TXBUFFERSIZE

#define I2C_ADDRESS        				0x8
#define I2C_ADDRESS_FINAL        		(I2C_ADDRESS << 1)

/* I2C TIMING Register define when I2C clock source is APB1 (SYSCLK/4) */
/* I2C TIMING is calculated in case of the I2C Clock source is the APB1CLK = 100 MHz */
/* This example use TIMING to 0x00901954 to reach 400 kHz speed (Rise time = 100 ns, Fall time = 10 ns) */
#define I2C_TIMING      				0x00901954
#define I2C_HANDLE_THREAD_PRIO    		( osPriorityNormal )
#define MAX_SPIKES 4

typedef struct SpikeTaskData{
   I2C_HandleTypeDef I2cHandle; /* I2C handler declaration */
   uint8_t aTxBuffer[TXBUFFERSIZE]; /* Buffer used for transmission */
   uint8_t aRxBuffer[RXBUFFERSIZE]; /* Buffer used for reception */
   uint8_t currTransmitSize;
   FireFlyStatus currStatus;
   uint8_t msgId; // get increment every time we send something
   FlowState flow;
   bool elevIsUp;
} SpikeTaskData;


void I2C_Init();

void I2C_start_listen();

void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c);

void HAL_I2C_SlaveTxCpltCallback(I2C_HandleTypeDef *hi2c);


#endif /* APPLICATION_USER_I2C_I2C_C_ */
