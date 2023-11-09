/*
 * ADC_config.c
 *
 *  Created on: Nov 9, 2023
 *      Author: eladsez
 */

#include "ADC.h"

ADC_HandleTypeDef    AdcHandle;


void ADC_Init(){

  /*##-1- Configure the ADC peripheral #######################################*/
  AdcHandle.Instance                      = ADCx;
  
  if (HAL_ADC_DeInit(&AdcHandle) != HAL_OK)
  {
    /* ADC de-initialization Error */
    Error_Handler();
  }
  
  AdcHandle.Init.ClockPrescaler           = ADC_CLOCK_ASYNC_DIV2;          /* Asynchronous clock mode, input ADC clock divided by 2*/
  AdcHandle.Init.Resolution               = ADC_RESOLUTION_16B;            /* 16-bit resolution for converted data */
  AdcHandle.Init.ScanConvMode             = DISABLE;                       /* Sequencer disabled (ADC conversion on only 1 channel: channel set on rank 1) */
  AdcHandle.Init.EOCSelection             = ADC_EOC_SINGLE_CONV;           /* EOC flag picked-up to indicate conversion end */
  AdcHandle.Init.LowPowerAutoWait         = DISABLE;                       /* Auto-delayed conversion feature disabled */
  AdcHandle.Init.ContinuousConvMode       = DISABLE;                       /* Continuous mode disabled to have only 1 conversion at each conversion trig */
  AdcHandle.Init.NbrOfConversion          = 1;                             /* Parameter discarded because sequencer is disabled */
  AdcHandle.Init.DiscontinuousConvMode    = DISABLE;                       /* Parameter discarded because sequencer is disabled */
  AdcHandle.Init.NbrOfDiscConversion      = 1;                             /* Parameter discarded because sequencer is disabled */
  AdcHandle.Init.ExternalTrigConv         = ADC_SOFTWARE_START;            /* Software start to trig the 1st conversion manually, without external event */
  AdcHandle.Init.ExternalTrigConvEdge     = ADC_EXTERNALTRIGCONVEDGE_NONE; /* Parameter discarded because software trigger chosen */
  AdcHandle.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DR;         /* Regular Conversion data stored in DR register only */
  AdcHandle.Init.Overrun                  = ADC_OVR_DATA_OVERWRITTEN;      /* DR register is overwritten with the last conversion result in case of overrun */
  AdcHandle.Init.OversamplingMode         = DISABLE;                       /* No oversampling */

  if (HAL_ADC_Init(&AdcHandle) != HAL_OK)
  {
    /* ADC initialization Error */
    Error_Handler();
  }
}


void ADC_config_channel(uint32_t channel){

  ADC_ChannelConfTypeDef sConfig;

  /*##-2- Configure ADC regular channel ######################################*/
  sConfig.Channel      = channel;                /* Sampled channel number */
  sConfig.Rank         = ADC_REGULAR_RANK_1;          /* Rank of sampled channel number ADCx_CHANNEL */
  sConfig.SamplingTime = ADC_SAMPLETIME_8CYCLES_5;    /* Sampling time (number of clock cycles unit) */
  sConfig.SingleDiff   = ADC_SINGLE_ENDED;            /* Single-ended input channel */
  sConfig.OffsetNumber = ADC_OFFSET_NONE;             /* No offset subtraction */ 
  sConfig.Offset = 0;                                 /* Parameter discarded because offset correction is disabled */


  if (HAL_ADC_ConfigChannel(&AdcHandle, &sConfig) != HAL_OK)
  {
    /* Channel Configuration Error */
    Error_Handler();
  }
}

uint16_t ADC_read_channel(uint32_t channel){

  uint16_t retval = -1;

  ADC_config_channel(channel);

  /* Run the ADC calibration in single-ended mode */
  if (HAL_ADCEx_Calibration_Start(&AdcHandle, ADC_CALIB_OFFSET_LINEARITY, ADC_SINGLE_ENDED) != HAL_OK)
  {
    /* Calibration Error */
    Error_Handler();
  }
  
  /*##-3- Start the conversion process #######################################*/
  if (HAL_ADC_Start(&AdcHandle) != HAL_OK)
  {
    /* Start Conversation Error */
    Error_Handler();
  }

  /*##-4- Wait for the end of conversion #####################################*/
  /*  For simplicity reasons, this example is just waiting till the end of the
      conversion, but application may perform other tasks while conversion
      operation is ongoing. */
  if (HAL_ADC_PollForConversion(&AdcHandle, 10) != HAL_OK)
  {
    /* End Of Conversion flag not set on time */
    Error_Handler();
  }
  else
  {
    /* ADC conversion completed */
    /*##-5- Get the converted value of regular channel  ########################*/
    retval = HAL_ADC_GetValue(&AdcHandle);
  }

  if (HAL_ADC_Stop(&AdcHandle) != HAL_OK){
    Error_Handler();
  }

  return retval;
}

uint16_t ADC_read_pressure(){
  return ADC_read_channel(ADC_PRESSURE_CHANNEL);
}

uint16_t ADC_read_current(){
  return ADC_read_channel(ADC_CURRENT_CHANNEL);
}

uint16_t ADC_read_voltage(){
  return ADC_read_channel(ADC_VOLTAGE_CHANNEL);
}

/**
  * @brief ADC MSP Initialization
  *        This function configures the hardware resources used in this example:
  *           - Peripheral's clock enable
  *           - Peripheral's GPIO Configuration
  * @param hadc: ADC handle pointer
  * @retval None
  */
void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc)
{
  GPIO_InitTypeDef          GPIO_InitStruct;

  /*##-1- Enable peripherals and GPIO Clocks #################################*/
  /* ADC Periph clock enable */
  ADCx_CLK_ENABLE();
  /* ADC Periph interface clock configuration */
  __HAL_RCC_ADC_CONFIG(RCC_ADCCLKSOURCE_CLKP);
  /* Enable GPIO clock ****************************************/
  ADCx_CHANNEL_GPIO_CLK_ENABLE();

  /*##-2- Configure peripheral GPIO ##########################################*/
  /* ADC Channel GPIO pin configuration */
  GPIO_InitStruct.Pin = ADC_PRESSURE_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(ADC_GPIO_GROUP, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = ADC_CURRENT_PIN;
  HAL_GPIO_Init(ADC_GPIO_GROUP, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = ADC_VOLTAGE_PIN;
  HAL_GPIO_Init(ADC_GPIO_GROUP, &GPIO_InitStruct);
}

/**
  * @brief ADC MSP De-Initialization
  *        This function frees the hardware resources used in this example:
  *          - Disable the Peripheral's clock
  *          - Revert GPIO to their default state
  * @param hadc: ADC handle pointer
  * @retval None
  */
void HAL_ADC_MspDeInit(ADC_HandleTypeDef *hadc)
{

  /*##-1- Reset peripherals ##################################################*/
  ADCx_FORCE_RESET();
  ADCx_RELEASE_RESET();
  /* ADC Periph clock disable
   (automatically reset all ADC's) */
  __HAL_RCC_ADC12_CLK_DISABLE();

  /*##-2- Disable peripherals and GPIO Clocks ################################*/
  /* De-initialize the ADC Channel GPIO pin */
  HAL_GPIO_DeInit(ADC_GPIO_GROUP, ADC_PRESSURE_PIN);
  HAL_GPIO_DeInit(ADC_GPIO_GROUP, ADC_CURRENT_PIN);
  HAL_GPIO_DeInit(ADC_GPIO_GROUP, ADC_VOLTAGE_PIN);
}