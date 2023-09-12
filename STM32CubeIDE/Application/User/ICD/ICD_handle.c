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

extern MissionManager misManager;
extern char secret_words[2][MAX_SECRET_SIZE];
extern CircularBuffer INSPVAXBuff;
extern SpikeTaskData spikeData[MAX_SPIKES];

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

	convertINSPVAXToNavFrameINS(&inspvax, frame); // check how the real convert works
}

void buildINSSTDFrame(NavFrameINSSTD *frame){
	INSPVAX inspvax;
	readINSPVAX(&INSPVAXBuff, &inspvax);

	frame->msgType = InsStdEnum;

	convertINSPVAXToNavFrameINSSTD(&inspvax, frame);
}

void buildLaunchCmd(LaunchCmd *cmd){

	cmd->msgType = LaunchCmdEnum;

	uint8_t spike_num = getSpikeNumber(xTaskGetCurrentTaskHandle());

	_Bool part_decision = spikeData[spike_num].currStatus.isReadyToLaunch && isLaunchSwitchOn();
	_Bool decision = part_decision && spikeData[spike_num].elevIsUp;

	if (!decision && part_decision){
		missionAssigned(&misManager, spike_num); // assign a mission for the launch sequence to start
	}

	if (decision){
		// build real launch command
		cmd->missionId = missionAssigned(&misManager, spike_num);
		cmd->secureLaunch = cmd->missionId == 0 ? 0 : SECURE_LAUNCH;
		printf("sending launch command with mission %d to spike %d\n", cmd->missionId, spike_num + 1);
	}
	else{
	 // build fake launch command (fill with zero or something similar)
		cmd->missionId = 0;
		cmd->secureLaunch = 0;
	}
	cmd->cs = 1;
}

void buildSecretCmd(SecretCmd *secret){
	secret->msgType = SecretCmdEnum;
	memcpy(secret->secret1, secret_words[0], MAX_SECRET_SIZE);
	memcpy(secret->secret2, secret_words[1], MAX_SECRET_SIZE);
	secret->cs = 1;
}


void handle_request(SpikeTaskData* spikeData){
	RequestMessage *req = (RequestMessage *) spikeData->aRxBuffer;
	uint8_t *aTxBuffer = spikeData->aTxBuffer;
	
	switch (req->requestedType){

	case NavInsEnum:
		NavFrameINS PVAFrame;
		buildINSFrame(&PVAFrame);
		PVAFrame.msgId = spikeData->msgId++;
		memcpy(aTxBuffer, &PVAFrame, sizeof(PVAFrame));
		break;
	case LaunchCmdEnum:
		LaunchCmd cmd = {0};
		buildLaunchCmd(&cmd);
		cmd.msgId = spikeData->msgId++;
		memcpy(aTxBuffer, &cmd, sizeof(cmd));
		break;
	case InsStdEnum:
		NavFrameINSSTD STDFrame;
		buildINSSTDFrame(&STDFrame);
		STDFrame.msgId = spikeData->msgId++;
		memcpy(aTxBuffer, &STDFrame, sizeof(STDFrame));
		break;
	case SecretCmdEnum:
		SecretCmd secret;
		buildSecretCmd(&secret);
		memcpy(aTxBuffer, &secret, sizeof(secret));
		break;
	default:
		break;
	}
	spikeData->currTransmitSize = req->msgSize;
}

void save_FireFlyStatus(SpikeTaskData *spikeData){

	if (!spikeData->currStatus.isReadyToLaunch && ((FireFlyStatus *) spikeData->aRxBuffer)->isReadyToLaunch){
		printf("Spike status is changed to 1\r\n");
	}

	memcpy(&(spikeData->currStatus), spikeData->aRxBuffer, sizeof(FireFlyStatus));
}


void ICD_process(SpikeTaskData *spikeData){

	uint8_t msgType = *(spikeData->aRxBuffer);

	switch (msgType){

	case RequestEnum:
		handle_request(spikeData);
		spikeData->flow = Transmit;
		break;

	case FirFlyStatus:
		save_FireFlyStatus(spikeData);
		spikeData->flow = Recv;
		break;

	default:
		spikeData->flow = Recv; // in case of us not recognize the msg we return to recv state
		break;
	}
}


void ICD_handle(void *args){

	SpikeTaskData *spikeData = (SpikeTaskData *)args;

	while(1){

		switch (spikeData->flow){

		case Recv:
			/*Put I2C peripheral in reception process ###########################*/
			if(HAL_I2C_Slave_Receive_IT(&(spikeData->I2cHandle), spikeData->aRxBuffer, RXBUFFERSIZE) != HAL_OK){ // size set to RXBUFFERSIZE and will change later by the driver
				Error_Handler();
			}
		    break;

		case Process:
			ICD_process(spikeData);
			break;

		case Transmit:
			if(HAL_I2C_Slave_Transmit(&(spikeData->I2cHandle), spikeData->aTxBuffer, spikeData->currTransmitSize, 30) != HAL_OK)	{
//				Error_Handler(); // the handle Error is not good in this case because it can be timeout (then the return code wont be HAL_OK)
			}
			spikeData->flow = Recv;
			break;

		default:

		}
		/* to not go into recv state more than once in a row */
		while (HAL_I2C_GetState(&(spikeData->I2cHandle)) != HAL_I2C_STATE_READY){
			sys_msleep(1); /* for the cpu to not poll */
		}
	}
}
