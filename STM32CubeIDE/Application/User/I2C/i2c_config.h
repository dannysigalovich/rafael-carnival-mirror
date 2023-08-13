/*
 * i2c.h
 *
 *  Created on: Aug 13, 2023
 *      Author: eladsez
 */

#ifndef APPLICATION_USER_I2C_I2C_C_
#define APPLICATION_USER_I2C_I2C_C_


#include "stm32h7xx_hal.h"
#include "stm32h7xx_nucleo.h"


/* User can use this section to tailor I2Cx/I2Cx instance used and associated
   resources */
/* Definition for I2Cx clock resources */
#define I2Cx                            I2C2
#define I2Cx_CLK_ENABLE()               __HAL_RCC_I2C2_CLK_ENABLE()
#define I2Cx_SDA_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOF_CLK_ENABLE()
#define I2Cx_SCL_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOF_CLK_ENABLE()

#define I2Cx_FORCE_RESET()              __HAL_RCC_I2C2_FORCE_RESET()
#define I2Cx_RELEASE_RESET()            __HAL_RCC_I2C2_RELEASE_RESET()

/* Definition for I2Cx Pins */
#define I2Cx_SCL_PIN                    GPIO_PIN_1
#define I2Cx_SCL_GPIO_PORT              GPIOF
#define I2Cx_SDA_PIN                    GPIO_PIN_0
#define I2Cx_SDA_GPIO_PORT              GPIOF
#define I2Cx_SCL_SDA_AF                 GPIO_AF4_I2C2

/* Definition for I2Cx's NVIC */
#define I2Cx_EV_IRQn                    I2C2_EV_IRQn
#define I2Cx_ER_IRQn                    I2C2_ER_IRQn
#define I2Cx_EV_IRQHandler              I2C2_EV_IRQHandler
#define I2Cx_ER_IRQHandler              I2C2_ER_IRQHandler

/* Size of Transmission buffer */
#define TXBUFFERSIZE                      1024
/* Size of Reception buffer */
#define RXBUFFERSIZE                      TXBUFFERSIZE

#define I2C_ADDRESS        0x30F

/* I2C TIMING Register define when I2C clock source is APB1 (SYSCLK/4) */
/* I2C TIMING is calculated in case of the I2C Clock source is the APB1CLK = 100 MHz */
/* This example use TIMING to 0x00901954 to reach 400 kHz speed (Rise time = 100 ns, Fall time = 10 ns) */
#define I2C_TIMING      0x00901954


void I2C_Init();

int I2C_start_listen();


#endif /* APPLICATION_USER_I2C_I2C_C_ */
