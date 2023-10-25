/*
 * monitor_helper.c
 *
 *  Created on: Aug 31, 2023
 *      Author: eladsez
 */

#include "IO.h"

#define DOWN_PULL_ACTION(GPIO_GROUP, PIN) do { \
    HAL_GPIO_WritePin(GPIO_GROUP, PIN, UP_DOWN_LOGIC_LEVEL); \
    sys_msleep(UP_DOWN_PULL_TIME); \
    HAL_GPIO_WritePin(GPIO_GROUP, PIN, !UP_DOWN_LOGIC_LEVEL); \
} while (0)

// TODO: replace UP_PULL_ACTION  with smart timeout (when defined by danny)
#define UP_PULL_ACTION(GPIO_GROUP, PIN, SPIKE) do { \
    HAL_GPIO_WritePin(GPIO_GROUP, PIN, UP_DOWN_LOGIC_LEVEL); \
	while (!is_spike_up(SPIKE)); \
    HAL_GPIO_WritePin(GPIO_GROUP, PIN, !UP_DOWN_LOGIC_LEVEL); \
} while (0)


uint8_t isLaunchStarted = 0;

void elev_up(uint8_t spike_num){
	switch(spike_num){
	case 0:
		UP_PULL_ACTION(SPIKE_1_GPIO_GROUP, SPIKE_1_UP_GPIO_PIN, spike_num);
		break;
	case 1:
		UP_PULL_ACTION(SPIKE_2_GPIO_GROUP, SPIKE_2_UP_GPIO_PIN, spike_num);
		break;
	case 2:
		UP_PULL_ACTION(SPIKE_3_GPIO_GROUP, SPIKE_3_UP_GPIO_PIN, spike_num);
		break;
	case 3:
		UP_PULL_ACTION(SPIKE_4_GPIO_GROUP, SPIKE_4_UP_GPIO_PIN, spike_num);
		break;
	}
}

void elev_down(uint8_t spike_num){

	switch(spike_num){
	case 0:

		break;
	case 1:

		break;
	case 2:

		break;
	case 3:

		break;
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


uint8_t isLaunchSwitchOn(){
	return isLaunchStarted;
}

uint8_t is_launch_master(){
	return !HAL_GPIO_ReadPin(LAUNCH_MASTER_GROUP, LAUNCH_MASTER_PIN);
}

void MasterToSlaveLaunch(){
	HAL_GPIO_WritePin(LAUNCH_SLAVE_GROUP, LAUNCH_SLAVE_PIN, 0);
}


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
  if (GPIO_Pin == LAUNCH_GPIO_PIN){
	  isLaunchStarted = 1;
	  HAL_NVIC_DisableIRQ(EXTI9_5_IRQn);
  }
}

