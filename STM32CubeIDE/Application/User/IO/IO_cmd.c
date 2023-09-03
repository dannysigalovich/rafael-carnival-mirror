/*
 * monitor_helper.c
 *
 *  Created on: Aug 31, 2023
 *      Author: eladsez
 */

#include "IO.h"

void elev_up(uint8_t maoz_num){

	switch(maoz_num){

	case 2:
		HAL_GPIO_WritePin(GPIOA, ELEV_UP_GPIO_PIN, 1);
		sys_msleep(UP_DOWN_PULL_TIME);
		HAL_GPIO_WritePin(GPIOA, ELEV_UP_GPIO_PIN, 0);
		return;

	}
}

void elev_down(uint8_t maoz_num){

	switch(maoz_num){

	case 2:
		HAL_GPIO_WritePin(GPIOA, ELEV_DOWN_GPIO_PIN, 1);
		sys_msleep(UP_DOWN_PULL_TIME);
		HAL_GPIO_WritePin(GPIOA, ELEV_DOWN_GPIO_PIN, 0);
		return;

	}

}

uint8_t is_maoz_up(uint8_t maoz_num){

	switch (maoz_num){

	case 2:
		return !HAL_GPIO_ReadPin(GPIOA, DISC_UP_GPIO_PIN);

	default:
			return 0;

	}
}

uint8_t is_maoz_mid(uint8_t maoz_num){

	switch (maoz_num){

	case 2:
		return !HAL_GPIO_ReadPin(GPIOA, DISC_MID_GPIO_PIN);

	default:
		return 0;
	}
}

uint8_t is_maoz_down(uint8_t maoz_num){


	switch (maoz_num){

	case 2:
		return !HAL_GPIO_ReadPin(GPIOC, DISC_DOWN_GPIO_PIN);

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

