//
// Created by xiaotian on 2022/8/5.
//

#include "uart.h"
#include "stm32f4xx.h"


#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE {
    /* Place your implementation of fputc here */
    /* e.g. write a character to the USART */
    /* Transmit Data */
    USART1->DR = ch;
    // USART_SendData(USART1, (uint8_t) ch);

    /* Loop until the end of transmission */
    while ((USART1->SR & USART_FLAG_TC) == (uint16_t) RESET);

    return ch;
}

/**
 * 初始化UART1，板载daplink上的cdc串口
 */
void USART_Config(void) {
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
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    USART_InitTypeDef USART_InitStructure;

    /* USARTx configured as follows:
          - BaudRate = 115200 baud
          - Word Length = 8 Bits
          - One Stop Bit
          - No parity
          - Hardware flow control disabled (RTS and CTS signals)
          - Receive and transmit enabled
    */
    USART_InitStructure.USART_BaudRate = 2000000;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    USART_Init(USART1, &USART_InitStructure);

    USART_Cmd(USART1, ENABLE);
}