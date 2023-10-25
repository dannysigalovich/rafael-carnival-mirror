/*
 * IO_handle.c
 *
 *  Created on: Aug 23, 2023
 *      Author: eladsez
 */

#include <IO/IO.h>

#define CONFIG_DISC(GPIO_GROUP, PIN) do { \
	GPIO_InitStruct.Pin = PIN; \
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT; \
	GPIO_InitStruct.Pull = GPIO_PULLUP; \
	HAL_GPIO_Init(GPIO_GROUP, &GPIO_InitStruct); \
} while (0)

void config_disc_GPIO(){
	GPIO_InitTypeDef GPIO_InitStruct = {0};
													/* Maoz 1 */
	/*Configure GPIO pin : PC6 PA5 PA6 */
	CONFIG_DISC(SPIKE1_DISC_GPIO_GROUP, SPIKE1_DISC_UP_PIN);
	CONFIG_DISC(SPIKE1_DISC_GPIO_GROUP, SPIKE1_DISC_MID_PIN);
    												/* Maoz 2 */
	/*Configure GPIO pin : PC6 PA5 PA6 */
	CONFIG_DISC(SPIKE2_UM_DISC_GPIO_GROUP, SPIKE2_DISC_UP_PIN);
	CONFIG_DISC(SPIKE2_UM_DISC_GPIO_GROUP, SPIKE2_DISC_MID_PIN);
													/* Maoz 3 */
	/*Configure GPIO pin : PC6 PA5 PA6 */
	CONFIG_DISC(SPIKE3_DISC_GPIO_GROUP, SPIKE3_DISC_UP_PIN);
	CONFIG_DISC(SPIKE3_DISC_GPIO_GROUP, SPIKE3_DISC_MID_PIN);
													/* Maoz 4 */
	/*Configure GPIO pin : PC6 PA5 PA6 */
	CONFIG_DISC(SPIKE4_DISC_GPIO_GROUP, SPIKE4_DISC_UP_PIN);
	CONFIG_DISC(SPIKE4_DISC_GPIO_GROUP, SPIKE4_DISC_MID_PIN);
}

void config_SPIKE_PWR(){
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	HAL_GPIO_WritePin(SPIKE1_DISC_GPIO_GROUP, SPIKE1_PWR_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(SPIKE2_PWR_GPIO_GROUP, SPIKE2_PWR_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(SPIKE3_DISC_GPIO_GROUP, SPIKE3_PWR_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(SPIKE4_DISC_GPIO_GROUP, SPIKE4_PWR_PIN, GPIO_PIN_RESET);

	GPIO_InitStruct.Pin = SPIKE1_PWR_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(SPIKE1_DISC_GPIO_GROUP, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = SPIKE2_PWR_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(SPIKE2_PWR_GPIO_GROUP, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = SPIKE3_PWR_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(SPIKE3_DISC_GPIO_GROUP, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = SPIKE4_PWR_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(SPIKE4_DISC_GPIO_GROUP, &GPIO_InitStruct);


}

void config_BNET_PWR(){
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	HAL_GPIO_WritePin(SPIKE_1_GPIO_GROUP, BNET1_PWR_GPIO_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(SPIKE_2_GPIO_GROUP, BNET2_PWR_GPIO_PIN, GPIO_PIN_RESET);

	GPIO_InitStruct.Pin = BNET1_PWR_GPIO_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(SPIKE_1_GPIO_GROUP, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = BNET2_PWR_GPIO_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(SPIKE_2_GPIO_GROUP, &GPIO_InitStruct);
}

void config_elev_GPIO(){
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(SPIKE_1_GPIO_GROUP, SPIKE_1_UP_GPIO_PIN, !UP_DOWN_LOGIC_LEVEL);
  HAL_GPIO_WritePin(SPIKE_2_GPIO_GROUP, SPIKE_2_UP_GPIO_PIN, !UP_DOWN_LOGIC_LEVEL);
  HAL_GPIO_WritePin(SPIKE_3_GPIO_GROUP, SPIKE_3_UP_GPIO_PIN, !UP_DOWN_LOGIC_LEVEL);
  HAL_GPIO_WritePin(SPIKE_4_GPIO_GROUP, SPIKE_4_UP_GPIO_PIN, !UP_DOWN_LOGIC_LEVEL);
                                                    /* Maoz 2 */
  /*Configure GPIO pins : PA9 */
  GPIO_InitStruct.Pin = SPIKE_2_UP_GPIO_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(SPIKE_2_GPIO_GROUP, &GPIO_InitStruct);

	  	  	  	  	  	  	  	  	  	  	  	   /* Maoz 1 */
	/*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = SPIKE_1_UP_GPIO_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(SPIKE_1_GPIO_GROUP, &GPIO_InitStruct);

                                                  /* Maoz 3 */
  /*Configure GPIO pin : PG2 */
  GPIO_InitStruct.Pin = SPIKE_3_UP_GPIO_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(SPIKE_3_GPIO_GROUP, &GPIO_InitStruct);

                                                  /* Maoz 4 */
  /*Configure GPIO pin : PD8 */
  GPIO_InitStruct.Pin = SPIKE_4_UP_GPIO_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(SPIKE_4_GPIO_GROUP, &GPIO_InitStruct);
  
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
  HAL_GPIO_WritePin(LAUNCH_SLAVE_GROUP, LAUNCH_SLAVE_PIN, GPIO_PIN_SET);

  /*Configure GPIO pin : LAUNCH_GPIO pin pf7 */
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Pin = LAUNCH_GPIO_PIN;
  HAL_GPIO_Init(LAUNCH_GPIO_GROUP, &GPIO_InitStruct);

  /*Configure GPIO pin : POWER_ON_IN_GPIO_PIN */
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Pin = LAUNCH_MASTER_PIN;
  HAL_GPIO_Init(LAUNCH_MASTER_GROUP, &GPIO_InitStruct);

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

  /*Configure GPIO pin : LAUNCH_SLAVE_PIN */
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Pin = LAUNCH_SLAVE_PIN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LAUNCH_SLAVE_GROUP, &GPIO_InitStruct);

  config_SPIKE_PWR();
  config_BNET_PWR();
  config_elev_GPIO();
  config_disc_GPIO();

  //  /* Enable and set EXTI9_5 lines Interrupt */
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
}

