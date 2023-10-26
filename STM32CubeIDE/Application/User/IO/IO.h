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
#include "udp_util/udp_ICD.h"
#include "I2C/i2c_config.h"

#define LAUNCH_SEQ_THREAD_PRIO	( osPriorityNormal )

#define UP_DOWN_PULL_TIME 16000 /* The time we set the elevator pins to pull up in ms */
#define ELEV_TIMEOUT 2000  /* The time to wait for the elevator when going up or down until timeout */
#define ELEV_ACTION_WAIT 15000 /* time to wait when the elevator is up until we pull the elevator down again (wait x sec in the state machine) */
#define MAX_SPIKE		 4

#define UP_DOWN_LOGIC_LEVEL 1 /* 0 = pull down, 1 = pull up */
#define SPIKE_PWR_LOGIC_LEVEL 1 /* 0 = pull down, 1 = pull up */

#define POWER_ON_IN_GPIO_PIN	GPIO_PIN_6
#define POWER_ON_OUT_GPIO_PIN	GPIO_PIN_8
#define POWER_ON_GPIO_GROUP		GPIOF

#define LAUNCH_GPIO_PIN 		GPIO_PIN_7
#define LAUNCH_GPIO_GROUP 		GPIOF

#define SPIKE_1_UP_GPIO_PIN 	GPIO_PIN_11
#define BNET1_PWR_GPIO_PIN 		GPIO_PIN_12 // the group is GPIOC
#define SPIKE_1_GPIO_GROUP 		GPIOC

#define SPIKE_2_UP_GPIO_PIN 	GPIO_PIN_9
#define BNET2_PWR_GPIO_PIN 		GPIO_PIN_10  // the group is GPIOA
#define SPIKE_2_GPIO_GROUP 		GPIOA

#define SPIKE_3_UP_GPIO_PIN 	GPIO_PIN_1
#define SPIKE_3_GPIO_GROUP 		GPIOG

#define SPIKE_4_UP_GPIO_PIN 	GPIO_PIN_8
#define SPIKE_4_GPIO_GROUP 		GPIOD

#define SPIKE1_DISC_UP_PIN 		GPIO_PIN_3
#define SPIKE1_DISC_MID_PIN 	GPIO_PIN_4
#define SPIKE1_PWR_PIN 			GPIO_PIN_5
#define SPIKE1_DISC_GPIO_GROUP 	GPIOE

#define SPIKE2_DISC_UP_PIN 			GPIO_PIN_6
#define SPIKE2_DISC_MID_PIN 		GPIO_PIN_5
#define SPIKE2_PWR_PIN 				GPIO_PIN_6
#define SPIKE2_UM_DISC_GPIO_GROUP 	GPIOA  /* UM = UP, MID */
#define SPIKE2_PWR_GPIO_GROUP 		GPIOC

#define SPIKE3_DISC_UP_PIN 		GPIO_PIN_4
#define SPIKE3_DISC_MID_PIN 	GPIO_PIN_5
#define SPIKE3_PWR_PIN 			GPIO_PIN_6
#define SPIKE3_DISC_GPIO_GROUP 	GPIOG

#define SPIKE4_DISC_UP_PIN 		GPIO_PIN_11
#define SPIKE4_DISC_MID_PIN 	GPIO_PIN_12
#define SPIKE4_PWR_PIN 			GPIO_PIN_13
#define SPIKE4_DISC_GPIO_GROUP 	GPIOD

#define LAUNCH_MASTER_PIN		GPIO_PIN_2
#define LAUNCH_MASTER_GROUP		GPIOG

#define LAUNCH_SLAVE_PIN		GPIO_PIN_0
#define LAUNCH_SLAVE_GROUP		GPIOG



typedef enum LuanchError{
	NoError,
	ElevStaysUp,
	ElevStaysDown,
	SpikeNotFreeAndElevDown,
	SpikeNotFreeAndElevUp,
}LaunchError;


void GPIO_Config(void);

uint8_t isLaunchSwitchOn();

void startLaunchSequence();

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);

void MasterToSlaveLaunch();

uint8_t is_power_on();

uint8_t is_launch_master();

void power_on_realy();

void elev_up(uint8_t spike_num);

void elev_down(uint8_t spike_num);

uint8_t is_spike_up(uint8_t spike_num);

uint8_t is_spike_mid(uint8_t spike_num);

uint8_t is_BNET_on(uint8_t bnet_num);

void turn_on_BNET(uint8_t bnet_num);

void turn_on_spike(uint8_t spike_num);

#endif /* APPLICATION_USER_IO_IO_H_ */
