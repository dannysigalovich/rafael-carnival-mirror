/*
 * ICD_handle.c
 *
 *  Created on: Aug 16, 2023
 *      Author: eladsez
 */


#include "I2C/i2c_config.h"
#include "ICD.h"
#include "stm32h7xx_hal_i2c.h"
#include <string.h>
#include "Novatel/navMesseging.h"
#include "CyclicBuffer/cyBuff.h"


/* I2C handler declaration */
extern I2C_HandleTypeDef I2cHandle;

/* Buffer used for transmission */
extern uint8_t aTxBuffer[TXBUFFERSIZE];
uint8_t currTransmitSize = 0;

/* Buffer used for reception */
extern uint8_t aRxBuffer[RXBUFFERSIZE];

/* the current status of mauz updated every 100ms */
FireFlyStatus currStatus;

uint8_t msgId = 0; // get increment every time we send something

extern CircularBuffer CPTcircularBuff; // the buffer CPT7 is filling (only two slots at the moment)

enum FlowState flow = Recv;


uint8_t indication = 0;


void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c){}
void HAL_I2C_SlaveTxCpltCallback(I2C_HandleTypeDef *hi2c){}

void next(){
	if (flow == ToTransmit) {
		flow = Transmit;
		return;
	}
	if (flow == ToRecv){
		flow = Recv;
		return;
	}
	flow += 1;
	flow = flow % 3;
}


void convertINSPVAToNavFrameINS(INSPVA *inspva, NavFrameINS *navFrame) {

    navFrame->INS_TimeTagSec = inspva->week * 604800.0 + inspva->seconds; // Week to seconds conversion
    navFrame->PositionLatitudeGeoRad = inspva->latitude;
    navFrame->PositionLongitudeGeoRad = inspva->longitude;
    navFrame->PositionAltitudeMeterAEL = inspva->height;
    navFrame->VelocityNorthMeterPerSec = inspva->northVelocity;
    navFrame->VelocityEastMeterPerSec = inspva->eastVelocity;
    navFrame->VelocityDownMeterPerSec = inspva->upVelocity;
    navFrame->AzimuthDeg = inspva->azimuth;
    navFrame->PitchDeg = inspva->pitch;
    navFrame->RollDeg = inspva->roll;
    navFrame->Status = inspva->status;
    navFrame->cs = 1;

}

void buildINSFrame(NavFrameINS *frame){

	INSPVA inspva;
	readFromBuff(&CPTcircularBuff, &inspva);

	frame->msgType = NavInsEnum;
	frame->msgId = msgId++;

	convertINSPVAToNavFrameINS(&inspva, frame); // check how the real convert works
}

void buildLaunchCmd(LaunchCmd *cmd){

	cmd->msgType = LaunchCmdEnum;
	cmd->msgId = msgId++;
	cmd->missionId = 2;
	// TODO: check logic and fill the rest

}


void handle_request(RequestMessage *req){


	switch (req->requestedType){

	case NavInsEnum:
		NavFrameINS frame;
		buildINSFrame(&frame);
		memcpy(aTxBuffer, &frame, sizeof(frame));
		currTransmitSize = sizeof(frame);
		break;
	case LaunchCmdEnum:
		LaunchCmd cmd = {0};
		buildLaunchCmd(&cmd);
		memcpy(aTxBuffer, &cmd, sizeof(cmd));
		currTransmitSize = sizeof(cmd);
		break;
	default:
			char data[100] = {0};
			data[0] = 2;
			data[1] = 12; // msnId(H)
			data[2] = 34; // msnId(L)
			data[3] = 4;
			data[4] = 1;
			data[5] = 255; // CS
			memcpy(aTxBuffer, data, 100);
	}
}


void ICD_process(){
	uint8_t msgType = *aRxBuffer;

	switch (msgType){

	case RequestEnum:
		handle_request((RequestMessage *)aRxBuffer);
		flow = ToTransmit;
		break;

	case FirFlyStatus:
		memcpy(&currStatus, aRxBuffer, sizeof(FireFlyStatus));
		flow = ToRecv;
		break;

	default:
		break;
	}
}


void ICD_handle(void *args){

	while(1){

		switch (flow){

		case Recv:
			/*Put I2C peripheral in reception process ###########################*/
			if(HAL_I2C_Slave_Receive_IT(&I2cHandle, (uint8_t *)aRxBuffer, RXBUFFERSIZE) != HAL_OK){
				/* Transfer error in reception process */
				Error_Handler();
			}
		    break;

		case Process:
			ICD_process();
			break;

		case Transmit:
			/*Start the transmission process #####################################*/
			/* While the I2C in reception process, user can transmit data through
			 "aTxBuffer" buffer */
			if(HAL_I2C_Slave_Transmit_IT(&I2cHandle, (uint8_t*)aTxBuffer, currTransmitSize)!= HAL_OK)	{
				/* Transfer error in transmission process */
				Error_Handler();
			}
			break;

		default:

		}

		/*Wait for the end of the transfer ###################################*/
		/*  Before starting a new communication transfer, you need to check the current
		  state of the peripheral; if it�s busy you need to wait for the end of current
		  transfer before starting a new one.
		  For simplicity reasons, this example is just waiting till the end of the
		  transfer, but application may perform other tasks while transfer operation
		  is ongoing. */
		while (HAL_I2C_GetState(&I2cHandle) != HAL_I2C_STATE_READY){
			sys_msleep(50);
		}

	    next();

	}
}
