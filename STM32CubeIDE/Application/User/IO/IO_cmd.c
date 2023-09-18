/*
 * monitor_helper.c
 *
 *  Created on: Aug 31, 2023
 *      Author: eladsez
 */

#include "IO.h"


#define UP_DOWN_PULL_ACTION(GPIO_GROUP, PIN) do { \
    HAL_GPIO_WritePin(GPIO_GROUP, PIN, UP_DOWN_LOGIC_LEVEL); \
    sys_msleep(UP_DOWN_PULL_TIME); \
    HAL_GPIO_WritePin(GPIO_GROUP, PIN, !UP_DOWN_LOGIC_LEVEL); \
} while (0)


uint8_t isLaunchStarted = 0;

void elev_up(uint8_t spike_num){

	HAL_GPIO_WritePin(ELEV_SAFE_GPIO_GROUP, ELEV_SAFE_GPIO_PIN, 1);

	switch(spike_num){
	case 0:
		UP_DOWN_PULL_ACTION(SPIKE_1_GPIO_GROUP, SPIKE_1_UP_GPIO_PIN);
		break;
	case 1:
		UP_DOWN_PULL_ACTION(SPIKE_2_GPIO_GROUP, SPIKE_2_UP_GPIO_PIN);
		break;
	case 2:
		UP_DOWN_PULL_ACTION(SPIKE_3_GPIO_GROUP, SPIKE_3_UP_GPIO_PIN);
		break;
	case 3:
		UP_DOWN_PULL_ACTION(SPIKE_4_GPIO_GROUP, SPIKE_4_UP_GPIO_PIN);
		break;
	}

	HAL_GPIO_WritePin(ELEV_SAFE_GPIO_GROUP, ELEV_SAFE_GPIO_PIN, 0);
}

void elev_down(uint8_t spike_num){

	HAL_GPIO_WritePin(ELEV_SAFE_GPIO_GROUP, ELEV_SAFE_GPIO_PIN, 1);

	switch(spike_num){
	case 0:
		UP_DOWN_PULL_ACTION(SPIKE_1_GPIO_GROUP, SPIKE_1_DOWN_GPIO_PIN);
		break;
	case 1:
		UP_DOWN_PULL_ACTION(SPIKE_2_GPIO_GROUP, SPIKE_2_DOWN_GPIO_PIN);
		break;
	case 2:
		UP_DOWN_PULL_ACTION(SPIKE_3_GPIO_GROUP, SPIKE_3_DOWN_GPIO_PIN);
		break;
	case 3:
		UP_DOWN_PULL_ACTION(SPIKE_4_GPIO_GROUP, SPIKE_4_DOWN_GPIO_PIN);
		break;
	}

	HAL_GPIO_WritePin(ELEV_SAFE_GPIO_GROUP, ELEV_SAFE_GPIO_PIN, 0);
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

uint8_t is_spike_down(uint8_t spike_num){
	switch (spike_num){
	case 0:
		return !HAL_GPIO_ReadPin(SPIKE1_DISC_GPIO_GROUP, SPIKE1_DISC_DOWN_PIN);
	case 1:
		return !HAL_GPIO_ReadPin(SPIKE2_DOWN_DISC_GPIO_GROUP, SPIKE2_DISC_DOWN_PIN);
	case 2:
		return !HAL_GPIO_ReadPin(SPIKE3_DISC_GPIO_GROUP, SPIKE3_DISC_DOWN_PIN);
	case 3:
		return !HAL_GPIO_ReadPin(SPIKE4_DISC_GPIO_GROUP, SPIKE4_DISC_DOWN_PIN);
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


uint8_t isLaunchSwitchOn(){
	return isLaunchStarted;
}


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
  if (GPIO_Pin == LAUNCH_GPIO_PIN){
	  isLaunchStarted = 1;
	  HAL_NVIC_DisableIRQ(EXTI9_5_IRQn);
  }
}

