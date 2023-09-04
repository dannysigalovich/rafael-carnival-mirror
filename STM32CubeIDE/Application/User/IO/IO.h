/*
 * IO_handle.h
 *
 *  Created on: Aug 23, 2023
 *      Author: eladsez
 */

#ifndef APPLICATION_USER_IO_IO_H_
#define APPLICATION_USER_IO_IO_H_

#include "main.h"
#include "lwip/sys.h"

#define LAUNCH_SEQ_THREAD_PRIO	( osPriorityNormal )

#define UP_DOWN_PULL_TIME 5000 /* The time we set the elevator pins to pull up in ms */
#define ELEV_TIMEOUT 5000  /* The time to wait for the elevator when going up or down until timeout */
#define ELEV_ACTION_WAIT 10000 /* time to wait when the elevator is up until we pull the elevator down again (wait x sec in the state machine) */

#define POWER_ON_IN_GPIO_PIN	GPIO_PIN_6
#define POWER_ON_OUT_GPIO_PIN	GPIO_PIN_8
#define POWER_ON_GPIO_GROUP		GPIOF

#define LAUNCH_GPIO_PIN 		GPIO_PIN_7
#define LAUNCH_GPIO_GROUP 		GPIOF

#define MAOZ_1_UP_GPIO_PIN 		GPIO_PIN_13
#define MAOZ_1_DOWN_GPIO_PIN 	GPIO_PIN_14
#define MAOZ_1_GPIO_GROUP 		GPIOC

#define MAOZ_2_UP_GPIO_PIN 		GPIO_PIN_9
#define MAOZ_2_DOWN_GPIO_PIN 	GPIO_PIN_10
#define MAOZ_2_GPIO_GROUP 		GPIOA

#define MAOZ_3_UP_GPIO_PIN 		GPIO_PIN_2
#define MAOZ_3_DOWN_GPIO_PIN 	GPIO_PIN_3
#define MAOZ_3_GPIO_GROUP 		GPIOG

#define MAOZ_4_UP_GPIO_PIN 		GPIO_PIN_8
#define MAOZ_4_DOWN_GPIO_PIN 	GPIO_PIN_10
#define MAOZ_4_GPIO_GROUP 		GPIOD

#define ELEV_SAFE_GPIO_PIN 		GPIO_PIN_8
#define ELEV_SAFE_GPIO_GROUP 	GPIOG

#define DISC_UP_GPIO_PIN 		GPIO_PIN_6
#define DISC_MID_GPIO_PIN 		GPIO_PIN_5
#define DISC_DOWN_GPIO_PIN 		GPIO_PIN_6 


typedef enum LuanchError{
	NoError,
	ElevStaysUp,
	ElevStaysDown,
	MaozNotFree
}LaunchError;


void GPIO_Config(void);

uint8_t isLaunchSwitchOn();

void startLaunchSequence();

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);


uint8_t is_power_on();

void power_on_realy();

void elev_up(uint8_t maoz_num);

void elev_down(uint8_t maoz_num);

uint8_t is_maoz_up(uint8_t maoz_num);

uint8_t is_maoz_mid(uint8_t maoz_num);

uint8_t is_maoz_down(uint8_t maoz_num);


#endif /* APPLICATION_USER_IO_IO_H_ */
