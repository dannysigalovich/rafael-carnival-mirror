/*
 * monitor_helper.c
 *
 *  Created on: Aug 31, 2023
 *      Author: eladsez
 */

#include "IO.h"


#define UP_DOWN_PULL_ACTION(GPIO_GROUP, PIN) do { \
    HAL_GPIO_WritePin(GPIO_GROUP, PIN, 1); \
    sys_msleep(UP_DOWN_PULL_TIME); \
    HAL_GPIO_WritePin(GPIO_GROUP, PIN, 0); \
} while (0)

void elev_up(uint8_t maoz_num){

	HAL_GPIO_WritePin(ELEV_SAFE_GPIO_GROUP, ELEV_SAFE_GPIO_PIN, 1);

	switch(maoz_num){
	case 1:
		UP_DOWN_PULL_ACTION(MAOZ_1_GPIO_GROUP, MAOZ_1_UP_GPIO_PIN);
		break;
	case 2:
		UP_DOWN_PULL_ACTION(MAOZ_2_GPIO_GROUP, MAOZ_2_UP_GPIO_PIN);
		break;
	case 3:
		UP_DOWN_PULL_ACTION(MAOZ_3_GPIO_GROUP, MAOZ_3_UP_GPIO_PIN);
		break;
	case 4:
		UP_DOWN_PULL_ACTION(MAOZ_4_GPIO_GROUP, MAOZ_4_UP_GPIO_PIN);
		break;
	}

	HAL_GPIO_WritePin(ELEV_SAFE_GPIO_GROUP, ELEV_SAFE_GPIO_PIN, 0);
}

void elev_down(uint8_t maoz_num){

	HAL_GPIO_WritePin(ELEV_SAFE_GPIO_GROUP, ELEV_SAFE_GPIO_PIN, 1);

	switch(maoz_num){
	case 1:
		UP_DOWN_PULL_ACTION(MAOZ_1_GPIO_GROUP, MAOZ_1_DOWN_GPIO_PIN);
		break;
	case 2:
		UP_DOWN_PULL_ACTION(MAOZ_2_GPIO_GROUP, MAOZ_2_DOWN_GPIO_PIN);
		break;
	case 3:
		UP_DOWN_PULL_ACTION(MAOZ_3_GPIO_GROUP, MAOZ_3_DOWN_GPIO_PIN);
		break;
	case 4:
		UP_DOWN_PULL_ACTION(MAOZ_4_GPIO_GROUP, MAOZ_4_DOWN_GPIO_PIN);
		break;
	}

	HAL_GPIO_WritePin(ELEV_SAFE_GPIO_GROUP, ELEV_SAFE_GPIO_PIN, 0);
}

uint8_t is_maoz_up(uint8_t maoz_num){
	switch (maoz_num){
	case 1:
		return !HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_3);
	case 2:
		return !HAL_GPIO_ReadPin(GPIOA, DISC_UP_GPIO_PIN);
	case 3:
		return !HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_4);
	case 4:
		return !HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_11);
	default:
		return 0;
	}
}

uint8_t is_maoz_mid(uint8_t maoz_num){
	switch (maoz_num){
	case 1:
		return !HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_4);
	case 2:
		return !HAL_GPIO_ReadPin(GPIOA, DISC_MID_GPIO_PIN);
	case 3:
		return !HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_5);
	case 4:
		return !HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_12);
	default:
		return 0;
	}
}

uint8_t is_maoz_down(uint8_t maoz_num){
	switch (maoz_num){
	case 1:
		return !HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_5);
	case 2:
		return !HAL_GPIO_ReadPin(GPIOC, DISC_DOWN_GPIO_PIN);
	case 3:
		return !HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_6);
	case 4:
		return !HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_13);
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

