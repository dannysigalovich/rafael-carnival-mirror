/*
 * ICD_handle.c
 *
 *  Created on: Aug 16, 2023
 *      Author: eladsez
 */


#include "ICD.h"
#include "I2C/i2c_config.h"
#include "main.h"

/* I2C handler declaration */
extern I2C_HandleTypeDef I2cHandle;

/* Buffer used for transmission */
extern uint8_t aTxBuffer[TXBUFFERSIZE];

/* Buffer used for reception */
extern uint8_t aRxBuffer[RXBUFFERSIZE];

void ICD_handle(void *args){

	while(1){

	/* The board receives the message and sends it back */

	  /*##-2- Put I2C peripheral in reception process ###########################*/
	  if(HAL_I2C_Slave_Receive_IT(&I2cHandle, (uint8_t *)aRxBuffer, RXBUFFERSIZE) != HAL_OK)
	  {
		/* Transfer error in reception process */
		Error_Handler();
	  }

	  /*##-3- Wait for the end of the transfer ###################################*/
	  /*  Before starting a new communication transfer, you need to check the current
		  state of the peripheral; if it�s busy you need to wait for the end of current
		  transfer before starting a new one.
		  For simplicity reasons, this example is just waiting till the end of the
		  transfer, but application may perform other tasks while transfer operation
		  is ongoing. */
	  while (HAL_I2C_GetState(&I2cHandle) != HAL_I2C_STATE_READY)
	  {
	  }

	  /*##-4- Start the transmission process #####################################*/
	  /* While the I2C in reception process, user can transmit data through
		 "aTxBuffer" buffer */
	  if(HAL_I2C_Slave_Transmit_IT(&I2cHandle, (uint8_t*)aTxBuffer, TXBUFFERSIZE)!= HAL_OK)
	  {
		/* Transfer error in transmission process */
		Error_Handler();
	  }
	  while(1){
		  sys_msleep(100);
	  }
	}

}
