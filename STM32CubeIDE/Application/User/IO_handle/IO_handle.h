/*
 * IO_handle.h
 *
 *  Created on: Aug 23, 2023
 *      Author: eladsez
 */

#ifndef APPLICATION_USER_IO_HANDLE_IO_HANDLE_H_
#define APPLICATION_USER_IO_HANDLE_IO_HANDLE_H_

#include "main.h"

void GPIO_Config(void);
uint8_t isLaunchSwitchOn();
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);


#endif /* APPLICATION_USER_IO_HANDLE_IO_HANDLE_H_ */
