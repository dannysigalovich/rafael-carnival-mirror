/*
 * IO_monitoring.c
 *
 *  Created on: Aug 30, 2023
 *      Author: eladsez
 */


#include <IO/IO.h>





uint8_t isLaunchStarted = 0;

uint8_t isLaunchSwitchOn(){
	return isLaunchStarted;
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
  if (GPIO_Pin == LAUNCH_GPIO_PIN){
	  isLaunchStarted = 1;
  }
}
