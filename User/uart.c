//
// Created by xiaotian on 2022/8/5.
//

#include <string.h>
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

void onboard_uart_transmit(const uint8_t *pdata, uint32_t length) {
    for (int i = 0; i < length; ++i) {
        USART1->DR = *(pdata + i);
        /*当数据从发送数据寄存器转移到发送移位寄存器时，TXE会置高，就可以把下一个数据写入到发送数据寄存器了，虽然此时可能并没有真正发送完成*/
        while ((USART1->SR & USART_FLAG_TXE) == (uint16_t) RESET);  //对DR寄存器写入后该位会自动清零，不需要手动清零
    }
}

float tempFloat[2];                    //定义的临时变量
uint8_t tempData[12] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0x00, 0x00, 0x80, 0x7f
};                    //定义的传输Buffer

void just_float_transmit(float f1, float f2) {
    tempFloat[0] = f1;    //转成浮点数
    tempFloat[1] = f2;
    memcpy(tempData, (uint8_t *) tempFloat, sizeof(tempFloat));//通过拷贝把数据重新整理
    onboard_uart_transmit(tempData, 12);
}