/*
 * IO_handle.c
 *
 *  Created on: Aug 23, 2023
 *      Author: eladsez
 */

#include "IO_handle.h"


#define LAUNCH_GPIO_PIN GPIO_PIN_7
#define LAUNCH_GPIO_GROUP GPIOF

uint8_t isLaunchStarted = 0;

void GPIO_Config(void){
  GPIO_InitTypeDef   GPIO_InitStructure;

  /* Enable GPIOC clock */
  __HAL_RCC_GPIOF_CLK_ENABLE();

  GPIO_InitStructure.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStructure.Pull = GPIO_PULLUP;
  GPIO_InitStructure.Pin = LAUNCH_GPIO_PIN;

  HAL_GPIO_Init(LAUNCH_GPIO_GROUP, &GPIO_InitStructure);

//  /* Enable and set EXTI lines Interrupt */
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
}

uint8_t isLaunchSwitchOn(){
	return isLaunchStarted;
}


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
  if (GPIO_Pin == LAUNCH_GPIO_PIN){
	  isLaunchStarted = 1;
  }
}
