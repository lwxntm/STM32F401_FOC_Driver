//
// Created by xiaotian on 2022/8/5.
//

#include "uart.h"
#include "stm32f4xx.h"

/**
 * 初始化UART1，板载daplink上的cdc串口
 */
void onboard_uart1_init(void) {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);  //电源开启
    //RCC_APB2PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);  //电源开启

    /* Connect PXx to USARTx_Tx*/
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_USART1);

    /* Connect PXx to USARTx_Rx*/
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_USART1);

    GPIO_InitTypeDef GPIO_InitStructure = {
            .GPIO_Pin=GPIO_Pin_6,
            .GPIO_Mode=GPIO_Mode_AF,
            .GPIO_OType=GPIO_OType_PP,
            .GPIO_PuPd=GPIO_PuPd_UP,
            .GPIO_Speed=GPIO_Fast_Speed
    };
    ////发送引脚配置为复用模式，只能给复用外设使用
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    //接收引脚必须配置为复用模式！！不可以配置为输入模式，参考stm32F1的视频教程踩到坑了
    // GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN;

    GPIO_Init(GPIOB, &GPIO_InitStructure);



    /* USARTx configured as follows:
          - BaudRate = 2000000 baud
          - Word Length = 8 Bits
          - One Stop Bit
          - No parity
          - Hardware flow control disabled (RTS and CTS signals)
          - Receive and transmit enabled
    */
    USART_InitTypeDef USART_InitStructure = {
            .USART_BaudRate = 2000000,
            .USART_WordLength = USART_WordLength_8b,
            .USART_StopBits = USART_StopBits_1,
            .USART_Parity = USART_Parity_No,
            .USART_HardwareFlowControl = USART_HardwareFlowControl_None,
            .USART_Mode = USART_Mode_Rx | USART_Mode_Tx,
    };
    USART_Init(USART1, &USART_InitStructure);

    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

    NVIC_InitTypeDef NVIC_InitStruct = {
            .NVIC_IRQChannel=USART1_IRQn,
            .NVIC_IRQChannelCmd=ENABLE,
            .NVIC_IRQChannelPreemptionPriority=6,
    };

    NVIC_Init(&NVIC_InitStruct);

    USART_Cmd(USART1, ENABLE);
}