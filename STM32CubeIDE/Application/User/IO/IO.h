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

#define POWER_ON_IN_GPIO_PIN	GPIO_PIN_6
#define POWER_ON_OUT_GPIO_PIN	GPIO_PIN_8
#define POWER_ON_GPIO_GROUP		GPIOF


#define UP_DOWN_PULL_TIME 5000 /* The time we set the elevator pins to pull up in ms */
#define ELEV_TIMEOUT 5000  /* The time to wait for the elevator when going up or down until timeout */
#define ELEV_ACTION_WAIT 10000 /* time to wait when the elevator is up until we pull the elevator down again (wait x sec in the state machine) */

#define ELEV_UP_GPIO_PIN		GPIO_PIN_9
#define ELEV_DOWN_GPIO_PIN		GPIO_PIN_10

#define DISC_UP_GPIO_PIN 		GPIO_PIN_6
#define DISC_MID_GPIO_PIN 		GPIO_PIN_5
#define DISC_DOWN_GPIO_PIN 		GPIO_PIN_6  /* different group then DISC_UP_GPIO_PIN */

#define LAUNCH_GPIO_PIN 		GPIO_PIN_7
#define LAUNCH_GPIO_GROUP 		GPIOF



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
