/*
 * IO_cmd.c
 *
 *  Created on: Aug 31, 2023
 *      Author: eladsez
 */

#include "IO.h"

extern SpikeTaskData spikeData[MAX_SPIKES];



void elev_up(uint8_t spike_num){
	spikeData[spike_num].elevGoUp = true;
	switch(spike_num){
	case 0:
		HAL_GPIO_WritePin(SPIKE_1_GPIO_GROUP, SPIKE_1_UP_GPIO_PIN, UP_DOWN_LOGIC_LEVEL);
		break;
	case 1:
		HAL_GPIO_WritePin(SPIKE_2_GPIO_GROUP, SPIKE_2_UP_GPIO_PIN, UP_DOWN_LOGIC_LEVEL);
		break;
	case 2:
		HAL_GPIO_WritePin(SPIKE_3_GPIO_GROUP, SPIKE_3_UP_GPIO_PIN, UP_DOWN_LOGIC_LEVEL);
		break;
	case 3:
		HAL_GPIO_WritePin(SPIKE_4_GPIO_GROUP, SPIKE_4_UP_GPIO_PIN, UP_DOWN_LOGIC_LEVEL);
		break;
	}
}

void elev_down(uint8_t spike_num){
	spikeData[spike_num].elevGoUp = false; // just in case is coming from the udp_icd
	switch(spike_num){
	case 0:
		HAL_GPIO_WritePin(SPIKE_1_GPIO_GROUP, SPIKE_1_UP_GPIO_PIN, !UP_DOWN_LOGIC_LEVEL);
		break;
	case 1:
		HAL_GPIO_WritePin(SPIKE_2_GPIO_GROUP, SPIKE_2_UP_GPIO_PIN, !UP_DOWN_LOGIC_LEVEL);
		break;
	case 2:
		HAL_GPIO_WritePin(SPIKE_3_GPIO_GROUP, SPIKE_3_UP_GPIO_PIN, !UP_DOWN_LOGIC_LEVEL);
		break;
	case 3:
		HAL_GPIO_WritePin(SPIKE_4_GPIO_GROUP, SPIKE_4_UP_GPIO_PIN, !UP_DOWN_LOGIC_LEVEL);
		break;
	}
}

void turn_on_spike(uint8_t spike_num){

	switch(spike_num){
	case 0:
		HAL_GPIO_WritePin(SPIKE1_DISC_GPIO_GROUP, SPIKE1_PWR_PIN, SPIKE_PWR_LOGIC_LEVEL);
		break;
	case 1:
		HAL_GPIO_WritePin(SPIKE2_PWR_GPIO_GROUP, SPIKE2_PWR_PIN, SPIKE_PWR_LOGIC_LEVEL);
		break;
	case 2:
		HAL_GPIO_WritePin(SPIKE3_DISC_GPIO_GROUP, SPIKE3_PWR_PIN, SPIKE_PWR_LOGIC_LEVEL);
		break;
	case 3:
		HAL_GPIO_WritePin(SPIKE4_DISC_GPIO_GROUP, SPIKE4_PWR_PIN, SPIKE_PWR_LOGIC_LEVEL);
		break;
	default:
		return;
	}
	spikeData[spike_num].initState = SpikeRelayStarted;
}

void turn_on_BNET(uint8_t bnet_num){

	switch (bnet_num){
	case 0:
		HAL_GPIO_WritePin(SPIKE_1_GPIO_GROUP, BNET1_PWR_GPIO_PIN, SPIKE_PWR_LOGIC_LEVEL);
		break;
	case 1:
		HAL_GPIO_WritePin(SPIKE_2_GPIO_GROUP, BNET2_PWR_GPIO_PIN, SPIKE_PWR_LOGIC_LEVEL);
		break;
	}
}

void turn_off_BNET(uint8_t bnet_num){
	switch (bnet_num){
		case 0:
			HAL_GPIO_WritePin(SPIKE_1_GPIO_GROUP, BNET1_PWR_GPIO_PIN, !SPIKE_PWR_LOGIC_LEVEL);
			break;
		case 1:
			HAL_GPIO_WritePin(SPIKE_2_GPIO_GROUP, BNET2_PWR_GPIO_PIN, !SPIKE_PWR_LOGIC_LEVEL);
			break;
	}
}

uint8_t is_BNET_on(uint8_t bnet_num){
	switch (bnet_num){
	case 0:
		return HAL_GPIO_ReadPin(SPIKE_1_GPIO_GROUP, BNET1_PWR_GPIO_PIN);
	case 1:
		return HAL_GPIO_ReadPin(SPIKE_2_GPIO_GROUP, BNET2_PWR_GPIO_PIN);
	default:
		return 0;
	}
}

uint8_t is_spike_up(uint8_t spike_num){
	switch (spike_num){
	case 0:
		return !HAL_GPIO_ReadPin(SPIKE1_DISC_GPIO_GROUP, SPIKE1_DISC_UP_PIN);
	case 1:
		return !HAL_GPIO_ReadPin(SPIKE2_UM_DISC_GPIO_GROUP, SPIKE2_DISC_UP_PIN);
	case 2:
		return !HAL_GPIO_ReadPin(SPIKE3_DISC_GPIO_GROUP, SPIKE3_DISC_UP_PIN);
	case 3:
		return !HAL_GPIO_ReadPin(SPIKE4_DISC_GPIO_GROUP, SPIKE4_DISC_UP_PIN);
	default:
		return 0;
	}
}

uint8_t is_spike_mid(uint8_t spike_num){
	switch (spike_num){
	case 0:
		return !HAL_GPIO_ReadPin(SPIKE1_DISC_GPIO_GROUP, SPIKE1_DISC_MID_PIN);
	case 1:
		return !HAL_GPIO_ReadPin(SPIKE2_UM_DISC_GPIO_GROUP, SPIKE2_DISC_MID_PIN);
	case 2:
		return !HAL_GPIO_ReadPin(SPIKE3_DISC_GPIO_GROUP, SPIKE3_DISC_MID_PIN);
	case 3:
		return !HAL_GPIO_ReadPin(SPIKE4_DISC_GPIO_GROUP, SPIKE4_DISC_MID_PIN);
	default:
		return 0;
	}
}


uint8_t is_power_on(){
	return !HAL_GPIO_ReadPin(POWER_ON_GPIO_GROUP, POWER_ON_IN_GPIO_PIN);
}

void power_on_realy(){
	HAL_GPIO_WritePin(POWER_ON_GPIO_GROUP, POWER_ON_OUT_GPIO_PIN, 1);
}


uint8_t isLaunchSwitchOn(uint8_t spike_num){
	return spikeData[spike_num].launchApprove;
}

uint8_t is_launch_master(){
	return !HAL_GPIO_ReadPin(LAUNCH_MASTER_GROUP, LAUNCH_MASTER_PIN);
}

void MasterToSlaveLaunch(){
	HAL_GPIO_WritePin(LAUNCH_SLAVE_GROUP, LAUNCH_SLAVE_PIN, 0);
}

