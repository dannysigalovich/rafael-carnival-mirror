/*
 * ICD_handle.c
 *
 *  Created on: Aug 16, 2023
 *      Author: eladsez
 */


#include <INSBuffer/cyBuff.h>
#include <IO/IO.h>
#include "I2C/i2c_config.h"
#include "ICD.h"
#include "stm32h7xx_hal_i2c.h"
#include <string.h>
#include "Novatel/navMesseging.h"
#include "missionManager/missionManager.h"

/* I2C handler declaration */
extern I2C_HandleTypeDef I2cHandle;

/* Buffer used for transmission */
extern uint8_t aTxBuffer[TXBUFFERSIZE];
uint8_t currTransmitSize = 0;

/* Buffer used for reception */
extern uint8_t aRxBuffer[RXBUFFERSIZE];

/* the current status of mauz updated every 100ms */
FireFlyStatus currStatus;

/* missions and secret words got from  */
extern MissionManager misManager;
extern char secret_words[2][MAX_SECRET_SIZE];

uint8_t msgId = 0; // get increment every time we send something

extern CircularBuffer INSPVAXBuff;

enum FlowState flow = Recv;


void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c){flow = Process;}
void HAL_I2C_SlaveTxCpltCallback(I2C_HandleTypeDef *hi2c){flow = Recv;}

void convertINSPVAXToNavFrameINS(INSPVAX *inspvax, NavFrameINS *navFrame) {
    navFrame->weeknumber = inspvax->header.week;
    navFrame->mSec = inspvax->header.ms;
    navFrame->PositionLatitudeGeoRad = inspvax->latitude;
    navFrame->PositionLongitudeGeoRad = inspvax->longitude;
    navFrame->PositionAltitudeMeterAEL = inspvax->height;
    navFrame->VelocityNorthMeterPerSec = inspvax->northVelocity;
    navFrame->VelocityEastMeterPerSec = inspvax->eastVelocity;
    navFrame->VelocityDownMeterPerSec = -inspvax->upVelocity;
    navFrame->AzimuthDeg = inspvax->azimuth;
    navFrame->PitchDeg = inspvax->pitch;
    navFrame->RollDeg = inspvax->roll;
    navFrame->Status = inspvax->status;
    navFrame->cs = 1;
}

void convertINSPVAXToNavFrameINSSTD(INSPVAX *inspvax, NavFrameINSSTD *navFrame){
	navFrame->StdLatitudeMeter = inspvax->latStdDev;
	navFrame->StdLongitudeMeter = inspvax->longStdDev;
	navFrame->StdAltitudeMeter = inspvax->heightStdDev;
	navFrame->StdVelocityNorth = inspvax->northVelStdDev;
	navFrame->StdVelocityEast = inspvax->eastVelStdDev;
	navFrame->StdVelocityDown = inspvax->upVelStdDev;
	navFrame->StdTrueHeadingDeg = inspvax->azimuthStdDev;
	navFrame->StdPitchDeg = inspvax->pitchStdDev;
	navFrame->StdRollDeg = inspvax->rollStdDev;
	navFrame->cs = 1;

}

void buildINSFrame(NavFrameINS *frame){
	INSPVAX inspvax;
	readINSPVAX(&INSPVAXBuff, &inspvax);

	frame->msgType = NavInsEnum;
	frame->msgId = msgId++;

	convertINSPVAXToNavFrameINS(&inspvax, frame); // check how the real convert works
}

void buildINSSTDFrame(NavFrameINSSTD *frame){
	INSPVAX inspvax;
	readINSPVAX(&INSPVAXBuff, &inspvax);

	frame->msgType = InsStdEnum;
	frame->msgId = msgId++;

	convertINSPVAXToNavFrameINSSTD(&inspvax, frame);
}

void buildLaunchCmd(LaunchCmd *cmd){

	cmd->msgType = LaunchCmdEnum;
	cmd->msgId = msgId++;

	if (currStatus.isReadyToLaunch && isLaunchSwitchOn()){
		// build real launch command
		TaskHandle_t currHandle = xTaskGetCurrentTaskHandle();
		cmd->missionId = missionAssigned(&misManager, getMauzNumber(currHandle));
	}
	else{
	 // build fake launch command (fill with zero or something similar)
		cmd->missionId = 0;
	}
	cmd->cs = 1;
}

void buildSecretCmd(SecretCmd *secret){
	secret->msgType = SecretCmdEnum;
	memcpy(secret->secret1, secret_words[0], MAX_SECRET_SIZE);
	memcpy(secret->secret2, secret_words[1], MAX_SECRET_SIZE);
	secret->cs = 1;
}


void handle_request(RequestMessage *req){

	switch (req->requestedType){

	case NavInsEnum:
		NavFrameINS PVAFrame;
		buildINSFrame(&PVAFrame);
		memcpy(aTxBuffer, &PVAFrame, sizeof(PVAFrame));
		currTransmitSize = sizeof(PVAFrame);
		break;
	case LaunchCmdEnum:
		LaunchCmd cmd = {0};
		buildLaunchCmd(&cmd);
		memcpy(aTxBuffer, &cmd, sizeof(cmd));
		currTransmitSize = sizeof(cmd);
		break;
	case InsStdEnum:
		NavFrameINSSTD STDFrame;
		buildINSSTDFrame(&STDFrame);
		memcpy(aTxBuffer, &STDFrame, sizeof(STDFrame));
		currTransmitSize = sizeof(STDFrame);
		break;
	case SecretCmdEnum:
		SecretCmd secret;
		buildSecretCmd(&secret);
		memcpy(aTxBuffer, &secret, sizeof(secret));
		currTransmitSize = sizeof(secret);
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
		currTransmitSize = 100;
		break;
	}
	currTransmitSize = req->msgSize;
}


void ICD_process(){
	uint8_t msgType = *aRxBuffer;

	switch (msgType){

	case RequestEnum:
		handle_request((RequestMessage *)aRxBuffer);
		flow = Transmit;
		break;

	case FirFlyStatus:
		memcpy(&currStatus, aRxBuffer, sizeof(FireFlyStatus));
		flow = Recv;
		break;

	default:
		flow = Recv; // in case of us not recognize the msg we return to recv state
		break;
	}
}


void ICD_handle(void *args){

	currStatus.BITStatus = 1; // in order to not launch immediately

	while(1){

		switch (flow){

		case Recv:
			/*Put I2C peripheral in reception process ###########################*/
			if(HAL_I2C_Slave_Receive_IT(&I2cHandle, (uint8_t *)aRxBuffer, RXBUFFERSIZE) != HAL_OK){ // size set to RXBUFFERSIZE and will change later by the driver
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
			if(HAL_I2C_Slave_Transmit_IT(&I2cHandle, (uint8_t*)aTxBuffer, currTransmitSize) != HAL_OK)	{
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
			sys_msleep(1); /* for the cpu to not poll */
		}
	}
}
