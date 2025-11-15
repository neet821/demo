/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.c
  * @brief   This file provides code for the configuration
  *          of the USART instances.
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
#include "usart.h"
#include "math.h"
#include "odrive.h"
#include "servo.h"
#include "string.h"
#include "balance.h"

int tt = 0;
/* USER CODE BEGIN 0 */

uint8_t TX_str2[] = "Turn Right!\r\n";
uint8_t TX_str3[] = "Turn Left!\r\n";
uint8_t  RX_dat[] = "";
int sidewalk_flag = 0;
int turn_flag = 0;

/* USER CODE END 0 */

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart4;

unsigned short USART_RX_STA=0;       //½ÓÊÕ×´Ì¬±ê¼Ç	
unsigned short USART_RX_STA2=0;       //½ÓÊÕ×´Ì¬±ê¼Ç


unsigned char USART_RX_BUF1[USART_REC_LEN];     //½ÓÊÕ»º³å,×î´óUSART_REC_LEN¸ö×Ö½Ú
unsigned char USART_RX_BUF2[USART_REC_LEN];     //½ÓÊÕ»º³å,×î´óUSART_REC_LEN¸ö×Ö½Ú


unsigned char aRxBuffer1[RXBUFFERSIZE];//HAL¿âÊ¹ÓÃµÄ´®¿Ú½ÓÊÕ»º³å
unsigned char aRxBuffer2[RXBUFFERSIZE];//HAL¿âÊ¹ÓÃµÄ´®¿Ú½ÓÊÕ»º³å

/* UART4 init function */
void MX_UART4_Init(void)
{

  /* USER CODE BEGIN UART4_Init 0 */

  /* USER CODE END UART4_Init 0 */

  LL_USART_InitTypeDef USART_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* Peripheral clock enable */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_UART4);

  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
  /**UART4 GPIO Configuration
  PA0-WKUP   ------> UART4_TX
  PA1   ------> UART4_RX
  */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_0|LL_GPIO_PIN_1;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_8;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* UART4 interrupt Init */
  NVIC_SetPriority(UART4_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
  NVIC_EnableIRQ(UART4_IRQn);

  /* USER CODE BEGIN UART4_Init 1 */

  /* USER CODE END UART4_Init 1 */
  USART_InitStruct.BaudRate = 115200;
  USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
  USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
  USART_InitStruct.Parity = LL_USART_PARITY_NONE;
  USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
  USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
  USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;
  LL_USART_Init(UART4, &USART_InitStruct);
  LL_USART_ConfigAsyncMode(UART4);
  LL_USART_Enable(UART4);
  /* USER CODE BEGIN UART4_Init 2 */

  /* USER CODE END UART4_Init 2 */

}
/* USART1 init function */

void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 460800;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }else{
    // tt++;
    HAL_UART_Receive_IT(&huart1, (unsigned char *)&aRxBuffer1, 1);
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}
/* USART2 init function */

void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  LL_USART_InitTypeDef USART_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* Peripheral clock enable */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART3);

  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
  /**USART3 GPIO Configuration
  PB10   ------> USART3_TX
  PB11   ------> USART3_RX
  */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_10|LL_GPIO_PIN_11;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_7;
  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USART3 interrupt Init */
  NVIC_SetPriority(USART3_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),1, 1));
  NVIC_EnableIRQ(USART3_IRQn);

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  USART_InitStruct.BaudRate = 460800;
  USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
  USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
  USART_InitStruct.Parity = LL_USART_PARITY_NONE;
  USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
  USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
  USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;
  LL_USART_Init(USART3, &USART_InitStruct);
  LL_USART_ConfigAsyncMode(USART3);
  LL_USART_Enable(USART3);
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{
  
  
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspInit 0 */
  
  /* USER CODE END USART1_MspInit 0 */
    /* USART1 clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART1 interrupt Init */
    HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspInit 1 */

  /* USER CODE END USART1_MspInit 1 */
  }
  else if(uartHandle->Instance==USART2)
  {
  /* USER CODE BEGIN USART2_MspInit 0 */

  /* USER CODE END USART2_MspInit 0 */
    /* USART2 clock enable */
     __HAL_RCC_USART2_CLK_ENABLE();

    __HAL_RCC_GPIOD_CLK_ENABLE();
    /**USART2 GPIO Configuration
    PD5     ------> USART2_TX
    PD6     ------> USART2_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    /* USART2 interrupt Init */
    // HAL_NVIC_SetPriority(USART2_IRQn, 0, 0);
    // HAL_NVIC_EnableIRQ(USART2_IRQn);
    /* USART6 interrupt Init */
    HAL_NVIC_SetPriority(USART2_IRQn, 2, 2);
    HAL_NVIC_EnableIRQ(USART2_IRQn);
  /* USER CODE BEGIN USART2_MspInit 1 */

  /* USER CODE END USART2_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{
// tt++;
  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspDeInit 0 */

  /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();

    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10);

    /* USART1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspDeInit 1 */

  /* USER CODE END USART1_MspDeInit 1 */
  }
  else if(uartHandle->Instance==USART2)
  {
  /* USER CODE BEGIN USART2_MspDeInit 0 */

  /* USER CODE END USART2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART2_CLK_DISABLE();

    /**USART2 GPIO Configuration
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2|GPIO_PIN_3);

    /* USART2 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART2_IRQn);
  /* USER CODE BEGIN USART2_MspDeInit 1 */

  /* USER CODE END USART2_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
/* USER CODE END 1 */
// // ´®¿Ú1ÖÐ¶Ï·þÎñ³ÌÐò

void USART1_IRQHandler(void)                	
{ 
	unsigned int timeout=0;

	
	HAL_UART_IRQHandler(&huart1);	//µ÷ÓÃHAL¿âÖÐ¶Ï´¦Àí¹«ÓÃº¯Êý
	
//	timeout=0;
//    while (HAL_UART_GetState(&huart1) != HAL_UART_STATE_READY)//µÈ´ý¾ÍÐ÷
//	{
//	 timeout++;////³¬Ê±´¦Àí
//     if(timeout>HAL_MAX_DELAY) break;		
//	
//	}
     
	timeout=0;
	while(HAL_UART_Receive_IT(&huart1, (unsigned char *)aRxBuffer1, RXBUFFERSIZE) != HAL_OK)//Ò»´Î´¦ÀíÍê³ÉÖ®ºó£¬ÖØÐÂ¿ªÆôÖÐ¶Ï²¢ÉèÖÃRxXferCountÎª1
	{
	 timeout++; //³¬Ê±´¦Àí
	 if(timeout>HAL_MAX_DELAY) break;	
	}
} 
extern int c22;
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{

	if(huart->Instance==USART1)//Èç¹ûÊÇ´®¿Ú1
	{
		if((USART_RX_STA&0x8000)==0)//½ÓÊÕÎ´Íê³É
		{
			if(USART_RX_STA&0x4000)//½ÓÊÕµ½ÁË0x0d
			{
				if(aRxBuffer1[0]!=0x0a)USART_RX_STA=0;//½ÓÊÕ´íÎó,ÖØÐÂ¿ªÊ¼
				else USART_RX_STA|=0x8000;	//½ÓÊÕÍê³ÉÁË 
			}
			else //»¹Ã»ÊÕµ½0X0D
			{	
				if(aRxBuffer1[0]==0x0d)USART_RX_STA|=0x4000;
				else
				{
					USART_RX_BUF1[USART_RX_STA&0X3FFF]=aRxBuffer1[0] ;
					USART_RX_STA++;
					if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;//½ÓÊÕÊý¾Ý´íÎó,ÖØÐÂ¿ªÊ¼½ÓÊÕ	  
				}		 
			}
		}else{
      int len;
      len=USART_RX_STA&0x3fff;//µÃµ½´Ë´Î½ÓÊÕµ½µÄÊý¾Ý³¤¶È
      odrive_analyze_speed((char*)USART_RX_BUF1, len);
      // scope_show();
      //  HAL_UART_Transmit(&huart2,(unsigned char*)USART_RX_BUF6,len,55);	//·¢ËÍ½ÓÊÕµ½µÄÊý¾Ý/
        while(__HAL_UART_GET_FLAG(&huart1,UART_FLAG_TC)!=SET);		//µÈ´ý·¢ËÍ½áÊø

			USART_RX_STA=0;
    }

	}else if(huart->Instance==USART2){
		/***********************************/
		if(huart->Instance == USART2)
		{
			if ((strcmp((char*)RX_dat, "-1") > 0) && (strncmp((char*)RX_dat,"blue",1)!=0) && (strncmp((char*)RX_dat,"red",1)!=0) && (strncmp((char*)RX_dat,"turn_left",1)!=0) && (strncmp((char*)RX_dat,"sidewalk",1)!=0)) {
				HAL_UART_Transmit(&huart2, TX_str2, sizeof(TX_str2), 10000);
				HAL_UART_Receive_IT(&huart2, (uint8_t *)&RX_dat, 1);
				servo_set_duty((int)RX_dat);
			} else if((strcmp((char*)RX_dat, "-1") < 0) && (strncmp((char*)RX_dat,"blue",1)!=0) && (strncmp((char*)RX_dat,"red",1)!=0) && (strncmp((char*)RX_dat,"turn_left",1)!=0) && (strncmp((char*)RX_dat,"sidewalk",1)!=0)) {  // RX_dat å°äºŽæˆ–ç­‰äºŽ -1
				HAL_UART_Transmit(&huart2, TX_str3, sizeof(TX_str3), 10000);
				HAL_UART_Receive_IT(&huart2, (uint8_t *)&RX_dat, 1);
				servo_set_duty((int)RX_dat);
			}else if(strcmp((char*)RX_dat, "blue") == 0){
				HAL_UART_Receive_IT(&huart2, (uint8_t *)&RX_dat, 1);
				servo_set_duty(8);
			}else if(strcmp((char*)RX_dat, "red") == 0){
				HAL_UART_Receive_IT(&huart2, (uint8_t *)&RX_dat, 1);
				servo_set_duty(-8);
			}else if(strcmp((char*)RX_dat, "turn_left") == 0){
				HAL_UART_Receive_IT(&huart2, (uint8_t *)&RX_dat, 1);
				servo_set_duty(8);
				turn_flag = 1;
			}else if(strcmp((char*)RX_dat, "sidewalk") == 0){
				HAL_UART_Receive_IT(&huart2, (uint8_t *)&RX_dat, 1);
				RunFlag = 3;
				sidewalk_flag = 1;
			}
			
		}
		/************************************/
  }
}
/* USER CODE END 1 */

void USART2_IRQHandler(void)                	
{ 
	HAL_UART_IRQHandler(&huart2);	//µ÷ÓÃHAL¿âÖÐ¶Ï´¦Àí¹«ÓÃº¯Êý


} 




