/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    tim.h
  * @brief   This file contains all the function prototypes for
  *          the tim.c file
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TIM_H__
#define __TIM_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "servo.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim13;

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void MX_TIM4_Init(void);
void MX_TIM2_Init(void);
void MX_TIM13_Init(void);
void set_servo_position(float position) ;

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

void TIM3_Init(unsigned short arr,unsigned short psc);
enum {BEGINE, BALANCE, SEND, RUN, STOP, END, LINE_END};
enum {CAMERA, RIGHT, GO , LEFT, GO1 , CAMERA1 , LEFT1 , GO2 , RIGHT1 , GO3 , THREEEND , ENDSTOP , ENDTOBALANCE};
//             0      1    1     0                 1      0       0      1  
void set_pwm_duty(float duty);
void PWM_SetDuty(TIM_HandleTypeDef *tim,unsigned int tim_channel,float duty);

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __TIM_H__ */

