#ifndef __ADC_H

#include "main.h"

/* Definition for ADCx clock resources */
#define ADCx                            ADC1
#define ADCx_CLK_ENABLE()               __HAL_RCC_ADC12_CLK_ENABLE()
#define ADCx_CHANNEL_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOC_CLK_ENABLE()

#define ADCx_FORCE_RESET()              __HAL_RCC_ADC12_FORCE_RESET()
#define ADCx_RELEASE_RESET()            __HAL_RCC_ADC12_RELEASE_RESET()

/* Definition for ADCx Channel Pin */

#define ADC_PRESSURE_PIN                GPIO_PIN_0
#define ADC_CURRENT_PIN                 GPIO_PIN_2
#define ADC_VOLTAGE_PIN                 GPIO_PIN_3
#define ADC_GPIO_GROUP                  GPIOC

/* Definition for ADCx's Channel */
#define ADC_PRESSURE_CHANNEL            ADC_CHANNEL_10
#define ADC_CURRENT_CHANNEL             ADC_CHANNEL_12
#define ADC_VOLTAGE_CHANNEL             ADC_CHANNEL_13
#define SAMPLINGTIME                    ADC_SAMPLETIME_8CYCLES_5



void ADC_Init();

uint16_t ADC_read_current();
uint16_t ADC_read_voltage();
uint16_t ADC_read_pressure();

void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc);
void HAL_ADC_MspDeInit(ADC_HandleTypeDef *hadc);

#endif /* __ADC_H */