/**
  ******************************************************************************
  * @file    Project/STM32F4xx_StdPeriph_Templates/stm32f4xx_it.c 
  * @author  MCD Application Team
  * @version V1.8.1
  * @date    27-January-2022
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2016 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/

#include <string.h>
#include "stm32f4xx_it.h"
#include "main.h"

/** @addtogroup Template_Project
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void) {
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void) {
    /* Go to infinite loop when Hard Fault exception occurs */
    while (1) {
    }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void) {
    /* Go to infinite loop when Memory Manage exception occurs */
    while (1) {
    }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void) {
    /* Go to infinite loop when Bus Fault exception occurs */
    while (1) {
    }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void) {
    /* Go to infinite loop when Usage Fault exception occurs */
    while (1) {
    }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void) {
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void) {
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void) {
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void) {
    user_code_insert_to_systick_handler();
}

void __ensure_systick_wrapper(void) {
}

/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f4xx.s).                                               */
/******************************************************************************/
volatile uint8_t data1 = 0;

void USART1_IRQHandler(void) {
    static volatile uint8_t RxState = 0;  //接收状态机
    static uint8_t pRxPacket = 0;  //指示接收到哪一个了

    volatile uint8_t rc_tmp;
    uint16_t rc_len;
    uint16_t i;
    if (USART_GetITStatus(USART1, USART_IT_IDLE) != RESET) {
        rc_tmp = USART1->SR;
        rc_tmp = USART1->DR;//软件序列清除IDLE标志位
        DMA_Cmd(DMA2_Stream5, DISABLE);//关闭DMA，准备重新配置
        DMA_ClearITPendingBit(DMA2_Stream5, DMA_IT_TCIF5);    // Clear Transfer Complete flag
        DMA_ClearITPendingBit(DMA2_Stream5, DMA_IT_TEIF5);    // Clear Transfer error flag
        rc_len = 128 - DMA_GetCurrDataCounter(DMA2_Stream5);//计算接收数据长度

        Serial_decode_RxPacket(usart1_rx_dma_buffer, rc_len);
    }

    DMA_Cmd(DMA2_Stream5, DISABLE);                      //先关闭DMA,才能设置它
// 清除DMA中断标志位
    // DMA_ClearFlag(DMA2_Stream5, DMA_FLAG_TCIF5 | DMA_FLAG_FEIF5 | DMA_FLAG_DMEIF5 | DMA_FLAG_TEIF5 |
    //                             DMA_FLAG_HTIF5);//清除DMA2_Steam7传输完成标志

    while (DMA_GetCmdStatus(DMA2_Stream5) != DISABLE) ;    //等待传输结束

    DMA_SetCurrDataCounter(DMA2_Stream5, 128);          //设置传输数据长度

    DMA_Cmd(DMA2_Stream5, ENABLE);                      //开启DMA
}

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 


