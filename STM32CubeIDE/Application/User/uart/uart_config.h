/*
 * uart_config.h
 *
 *  Created on: Aug 10, 2023
 *      Author: eladsez
 */

#ifndef APPLICATION_USER_UART_UART_CONFIG_H_
#define APPLICATION_USER_UART_UART_CONFIG_H_


#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern UART_HandleTypeDef huart2;

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void MX_USART2_UART_Init(void);

int __io_putchar(int ch);

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif


#endif /* APPLICATION_USER_UART_UART_CONFIG_H_ */
