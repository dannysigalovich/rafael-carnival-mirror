/*
 * IO_handle.c
 *
 *  Created on: Aug 23, 2023
 *      Author: eladsez
 */

#include <IO/IO.h>


void config_elev_GPIO(){
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, MAOZ_1_UP_GPIO_PIN, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOA, MAOZ_2_UP_GPIO_PIN, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOA, MAOZ_3_UP_GPIO_PIN, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOA, MAOZ_4_UP_GPIO_PIN, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOA, MAOZ_1_DOWN_GPIO_PIN, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOA, MAOZ_2_DOWN_GPIO_PIN, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOA, MAOZ_3_DOWN_GPIO_PIN, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOA, MAOZ_4_DOWN_GPIO_PIN, GPIO_PIN_RESET);


                                                    /* Maoz 1 */
  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = MAOZ_1_UP_GPIO_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(MAOZ_1_GPIO_GROUP, &GPIO_InitStruct);

  /*Configure GPIO pin : PC14 */
  GPIO_InitStruct.Pin = MAOZ_1_DOWN_GPIO_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(MAOZ_1_GPIO_GROUP, &GPIO_InitStruct);

                                                    /* Maoz 2 */
  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9|GPIO_PIN_10, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC6 */
  GPIO_InitStruct.Pin = DISC_DOWN_GPIO_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PA5 PA6 */
  GPIO_InitStruct.Pin = DISC_UP_GPIO_PIN|DISC_MID_GPIO_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PA9 PA10 */
  GPIO_InitStruct.Pin = MAOZ_2_UP_GPIO_PIN|MAOZ_2_DOWN_GPIO_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(MAOZ_2_GPIO_GROUP, &GPIO_InitStruct);

                                                  /* Maoz 3 */
  /*Configure GPIO pin : PG2 */
  GPIO_InitStruct.Pin = MAOZ_3_UP_GPIO_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(MAOZ_3_GPIO_GROUP, &GPIO_InitStruct);

  /*Configure GPIO pin : PG3 */
  GPIO_InitStruct.Pin = MAOZ_3_DOWN_GPIO_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(MAOZ_3_GPIO_GROUP, &GPIO_InitStruct);

                                                  /* Maoz 4 */
  /*Configure GPIO pin : PD8 */
  GPIO_InitStruct.Pin = MAOZ_4_UP_GPIO_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(MAOZ_4_GPIO_GROUP, &GPIO_InitStruct);

  /*Configure GPIO pin : PD10 */
  GPIO_InitStruct.Pin = MAOZ_4_DOWN_GPIO_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(MAOZ_4_GPIO_GROUP, &GPIO_InitStruct);
  
}

void GPIO_Config(void){
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* Enable GPIOC clock */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  
  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(POWER_ON_GPIO_GROUP, POWER_ON_OUT_GPIO_PIN, GPIO_PIN_RESET);

  /*Configure GPIO pin : LAUNCH_GPIO pin pf7 */
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Pin = LAUNCH_GPIO_PIN;
  HAL_GPIO_Init(LAUNCH_GPIO_GROUP, &GPIO_InitStruct);

  /*Configure GPIO pin : POWER_ON_IN_GPIO_PIN */
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Pin = POWER_ON_IN_GPIO_PIN;
  HAL_GPIO_Init(POWER_ON_GPIO_GROUP, &GPIO_InitStruct);

  /*Configure GPIO pin : POWER_ON_OUT_GPIO_PIN */
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Pin = POWER_ON_OUT_GPIO_PIN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(POWER_ON_GPIO_GROUP, &GPIO_InitStruct);

  config_elev_GPIO();

  //  /* Enable and set EXTI9_5 lines Interrupt */
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
}

