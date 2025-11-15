/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    tim.c
  * @brief   This file provides code for the configuration
  *          of the TIM instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "tim.h"
#include "gpio.h"
#include "odrive.h"
#include "balance.h"
#include "imu.h"
#include "servo.h"
extern int c22;
int cnt_show = 0;
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

TIM_HandleTypeDef htim4;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim13;
#define PWM_FREQUENCE 50
#define PWM_RESOLUTION 10000
#define APB1_TIMER_CLOCKS 84000000
#define PWM_DEFAULT_DUTY 5000
#define APB2_TIMER_CLOCKS 168000000
#define TIM_PSC_APB1 ((APB1_TIMER_CLOCKS/PWM_FREQUENCE)/PWM_RESOLUTION -1)
#define TIM_PSC_APB2 ((APB2_TIMER_CLOCKS/PWM_FREQUENCE)/PWM_RESOLUTION -1)

/* TIM4 init function */
void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 840-1;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 200-1;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */
 HAL_TIM_Base_Start_IT(&htim4); //?????3????3?????TIM_IT_UPDATE
  /* USER CODE END TIM4_Init 2 */

}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* tim_baseHandle)
{

  if(tim_baseHandle->Instance==TIM4)
  {
  /* USER CODE BEGIN TIM4_MspInit 0 */

  /* USER CODE END TIM4_MspInit 0 */
    /* TIM4 clock enable */
    __HAL_RCC_TIM4_CLK_ENABLE();

    /* TIM4 interrupt Init */
    HAL_NVIC_SetPriority(TIM4_IRQn, 1, 3);
    HAL_NVIC_EnableIRQ(TIM4_IRQn);
  /* USER CODE BEGIN TIM4_MspInit 1 */

  /* USER CODE END TIM4_MspInit 1 */
  }

    if(tim_baseHandle->Instance==TIM13)
  {
  /* USER CODE BEGIN TIM13_MspInit 0 */

  /* USER CODE END TIM13_MspInit 0 */
    /* TIM13 clock enable */
    __HAL_RCC_TIM13_CLK_ENABLE();
  /* USER CODE BEGIN TIM13_MspInit 1 */

  /* USER CODE END TIM13_MspInit 1 */
  }
}

void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* tim_baseHandle)
{

  if(tim_baseHandle->Instance==TIM4)
  {
  /* USER CODE BEGIN TIM4_MspDeInit 0 */

  /* USER CODE END TIM4_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_TIM4_CLK_DISABLE();

    /* TIM4 interrupt Deinit */
    HAL_NVIC_DisableIRQ(TIM4_IRQn);
  /* USER CODE BEGIN TIM4_MspDeInit 1 */

  /* USER CODE END TIM4_MspDeInit 1 */
  }

  if(tim_baseHandle->Instance==TIM13)
  {
  /* USER CODE BEGIN TIM13_MspDeInit 0 */

  /* USER CODE END TIM13_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_TIM13_CLK_DISABLE();
  /* USER CODE BEGIN TIM13_MspDeInit 1 */

  /* USER CODE END TIM13_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

//?????3?§Ø??????
void TIM4_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&htim4);
}

int cnt22 = 0;
int if_go = 1;
extern int Flag_Stop;
int bef_stop = 1;
extern volatile int g_start1;
//???????????????§Ø??????????
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim == (&htim4))
    {
        cnt_show++;
        cnt22++;
        if (cnt_show == 1000)
            cnt_show = 0;
        if (cnt_show % 3 == 0){
            odrive_speed_ctl(1, odrive.set_speed0);
				}
        else if (cnt_show % 3 == 1)
        {
            if (cnt22 >= 25)
            {
                cnt22 = 0;
                odrive_feedback();
            }
        }
        balance();
        // HAL_GPIO_TogglePin(GPIOF, LED_GREEN_Pin);
				/*
				if(if_go == 1 && g_start1 == 1){
					if_go = 0;
					odrive_speed_ctl(0, 2);
					
					
				}
				if(bef_stop != Flag_Stop && Flag_Stop ==1){
					odrive_speed_ctl(0, 0);
				}
				
				bef_stop = Flag_Stop;
				*/
    }
				
}

/* USER CODE END 1 */



void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef* tim_pwmHandle)
{

  if(tim_pwmHandle->Instance==TIM2)
  {
  /* USER CODE BEGIN TIM2_MspInit 0 */

  /* USER CODE END TIM2_MspInit 0 */
    /* TIM2 clock enable */
    __HAL_RCC_TIM2_CLK_ENABLE();
  /* USER CODE BEGIN TIM2_MspInit 1 */

  /* USER CODE END TIM2_MspInit 1 */
  }
    if(tim_pwmHandle->Instance==TIM13)
  {
  /* USER CODE BEGIN TIM2_MspInit 0 */

  /* USER CODE END TIM2_MspInit 0 */
    /* TIM2 clock enable */
    __HAL_RCC_TIM13_CLK_ENABLE();
  /* USER CODE BEGIN TIM2_MspInit 1 */

  /* USER CODE END TIM2_MspInit 1 */
  }

}


/* TIM13 init function */
void MX_TIM13_Init(void)
{
  TIM_MasterConfigTypeDef sMasterConfig;

  /* USER CODE BEGIN TIM13_Init 0 */

  /* USER CODE END TIM13_Init 0 */

  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM13_Init 1 */

  /* USER CODE END TIM13_Init 1 */
  htim13.Instance = TIM13;
  htim13.Init.Prescaler = 84-1;
  htim13.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim13.Init.Period = 20000-1;
  htim13.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim13.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim13) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim13) != HAL_OK)
  {
    Error_Handler();
  }

    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim13, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }

  sConfigOC.OCMode = TIM_OCMODE_PWM2;
  sConfigOC.Pulse = PWM_DEFAULT_DUTY;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim13, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM13_Init 2 */

  /* USER CODE END TIM13_Init 2 */
  HAL_TIM_MspPostInit(&htim13);

}

void HAL_TIM_MspPostInit(TIM_HandleTypeDef* timHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct;
  if(timHandle->Instance==TIM13)
  {
  /* USER CODE BEGIN TIM2_MspPostInit 0 */
__HAL_RCC_GPIOF_CLK_ENABLE();
  /* USER CODE END TIM2_MspPostInit 0 */
    /**TIM2 GPIO Configuration    
    PA1     ------> TIM2_CH2
    PA0/WKUP     ------> TIM2_CH1
    PA2     ------> TIM2_CH3
    PA3     ------> TIM2_CH4 
    */

    GPIO_InitStruct.Pin = GPIO_PIN_8;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF9_TIM13;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
c22 = 22;
  /* USER CODE BEGIN TIM2_MspPostInit 1 */

  /* USER CODE END TIM2_MspPostInit 1 */
  }
}


void HAL_TIM_PWM_MspDeInit(TIM_HandleTypeDef* tim_pwmHandle)
{

  if(tim_pwmHandle->Instance==TIM13)
  {
  /* USER CODE BEGIN TIM2_MspDeInit 0 */

  /* USER CODE END TIM2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_TIM13_CLK_DISABLE();
  /* USER CODE BEGIN TIM2_MspDeInit 1 */

  /* USER CODE END TIM2_MspDeInit 1 */
  }
} 

/* TIM2 init function */
// void MX_TIM2_Init(void)
// {
//   TIM_MasterConfigTypeDef sMasterConfig;
//   TIM_OC_InitTypeDef sConfigOC;

//   htim2.Instance = TIM2;
//   htim2.Init.Prescaler = 84-1;
//   htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
//   htim2.Init.Period = 20000-1;
//   htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;

//   htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
//   if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
//   {
//     Error_Handler();
//   }

//   sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
//   sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
//   if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
//   {
//     Error_Handler();
//   }

//   sConfigOC.OCMode = TIM_OCMODE_PWM1;
//   sConfigOC.Pulse = PWM_DEFAULT_DUTY;
//   sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
//   sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
//   if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
//   {
//     Error_Handler();
//   }

//   if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
//   {
//     Error_Handler();
//   }

//   if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
//   {
//     Error_Handler();
//   }

//   if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
//   {
//     Error_Handler();
//   }

//   HAL_TIM_MspPostInit(&htim2);

// }

void set_pwm_duty(float duty){
    duty > 1 ? duty = 1 : duty;
    duty < 0 ? duty = 0 : duty;
    duty /= 2;
    duty = 1-duty;
  PWM_SetDuty(&htim13,TIM_CHANNEL_1,duty); //PA 0
	// PWM_SetDuty(&htim13,TIM_CHANNEL_2,duty); //PA 1
	// PWM_SetDuty(&htim13,TIM_CHANNEL_3,duty); //PA 2
	// PWM_SetDuty(&htim13,TIM_CHANNEL_4,duty); //PA 3
}

void PWM_SetDuty(TIM_HandleTypeDef *tim,unsigned int tim_channel,float duty){
	
	switch(tim_channel){	
		// case TIM_CHANNEL_1: tim->Instance->CCR1 = (PWM_RESOLUTION*duty) - 1;break;
    case TIM_CHANNEL_1: tim->Instance->CCR1 = (2*PWM_RESOLUTION*duty) - 1;break;
		// case TIM_CHANNEL_2: tim->Instance->CCR2 = (PWM_RESOLUTION*duty) - 1;break;
		// case TIM_CHANNEL_3: tim->Instance->CCR3 = (PWM_RESOLUTION*duty) - 1;break;
		// case TIM_CHANNEL_4: tim->Instance->CCR4 = (PWM_RESOLUTION*duty) - 1;break;
	}
	
}

/* ??????? */
void set_servo_position(float position) {
  position = position > 1 ? 1 : position;
  position = position < 0 ? 0 : position;
  unsigned int pulse = position * 20000;
  htim13.Instance->CCR1 = pulse - 1;
}
