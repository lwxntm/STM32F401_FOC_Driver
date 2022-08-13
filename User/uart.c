//
// Created by xiaotian on 2022/8/5.
//

#include <string.h>
#include "uart.h"
#include "stm32f4xx.h"

uint8_t usart1_rx_dma_buffer[128];
uint8_t usart1_tx_dma_buffer[128];
uint8_t usart1_tx_dma_size_counter = 0;

/**
 * 初始化UART1，板载daplink上的cdc串口
 */
void onboard_uart1_init(void) {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);  //电源开启
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);   //开启dma时钟
    //RCC_APB2PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);  //电源开启

    /* Connect PXx to USARTx_Tx*/
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_USART1);

    /* Connect PXx to USARTx_Rx*/
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_USART1);

    /*UART1 TX D<A*/
    /* DMA2_Stream7 channel4 configuration **************************************/
    DMA_DeInit(DMA2_Stream7);
    while (DMA_GetCmdStatus(DMA2_Stream7) != DISABLE);    //等待传输结束
    DMA_InitTypeDef DMA2_InitStruct_for_uart1_tx = {
            .DMA_Channel=DMA_Channel_4,
            .DMA_PeripheralBaseAddr=((uint32_t) &USART1->DR),
            .DMA_PeripheralDataSize=DMA_PeripheralDataSize_Byte,
            .DMA_PeripheralInc=DMA_PeripheralInc_Disable,           //外设地址是否自增
            .DMA_Memory0BaseAddr=(uint32_t) usart1_tx_dma_buffer,
            .DMA_MemoryDataSize=DMA_MemoryDataSize_Byte,               //每一个内存数据的大小，字节/半字/字
            .DMA_MemoryInc=DMA_MemoryInc_Enable,   //内存地址是否自增
            .DMA_DIR=DMA_DIR_MemoryToPeripheral,    //dma方向，内存到外设
            .DMA_BufferSize=128,     //需要dma搬运的数据数量
            .DMA_Mode=DMA_Mode_Normal,  //单次还是循环
            .DMA_Priority=DMA_Priority_Low,
            .DMA_FIFOMode=DMA_FIFOMode_Disable,           //不使用FIFO
            .DMA_FIFOThreshold = DMA_FIFOThreshold_Full,
            .DMA_MemoryBurst = DMA_MemoryBurst_Single,
            .DMA_PeripheralBurst = DMA_PeripheralBurst_Single
    };
    DMA_Init(DMA2_Stream7, &DMA2_InitStruct_for_uart1_tx);
    DMA_Cmd(DMA2_Stream7, ENABLE);


    /* UART1_RX DMA  */
    /* DMA2_Stream5 channel4 configuration **************************************/
    DMA_DeInit(DMA2_Stream5);
    while (DMA_GetCmdStatus(DMA2_Stream5) != DISABLE);    //等待传输结束
    DMA_InitTypeDef DMA_InitStruct = {
            .DMA_Channel=DMA_Channel_4,
            .DMA_PeripheralBaseAddr=((uint32_t) &USART1->DR),
            .DMA_PeripheralDataSize=DMA_PeripheralDataSize_Byte,
            .DMA_PeripheralInc=DMA_PeripheralInc_Disable,           //外设地址是否自增
            .DMA_Memory0BaseAddr=(uint32_t) usart1_rx_dma_buffer,
            .DMA_MemoryDataSize=DMA_MemoryDataSize_Byte,               //每一个内存数据的大小，字节/半字/字
            .DMA_MemoryInc=DMA_MemoryInc_Enable,   //内存地址是否自增
            .DMA_DIR=DMA_DIR_PeripheralToMemory,    //dma方向，外设到内存，内存到内存，内存到外设
            .DMA_BufferSize=128,     //需要dma搬运的数据数量
            .DMA_Mode=DMA_Mode_Normal,  //单次还是循环
            .DMA_Priority=DMA_Priority_High,
            .DMA_FIFOMode=DMA_FIFOMode_Disable,           //不使用FIFO
            .DMA_FIFOThreshold = DMA_FIFOThreshold_Full,
            .DMA_MemoryBurst = DMA_MemoryBurst_Single,
            .DMA_PeripheralBurst = DMA_PeripheralBurst_Single
    };
    DMA_Init(DMA2_Stream5, &DMA_InitStruct);
    DMA_Cmd(DMA2_Stream5, ENABLE);


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

    USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);
    USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);
    USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE); //启用发送dma
    NVIC_InitTypeDef NVIC_InitStruct = {
            .NVIC_IRQChannel=USART1_IRQn,
            .NVIC_IRQChannelCmd=ENABLE,
            .NVIC_IRQChannelPreemptionPriority=6,
    };
    NVIC_Init(&NVIC_InitStruct);

    USART_Cmd(USART1, ENABLE);
}

void onboard_uart_transmit_byte(const uint8_t data) {
    USART1->DR = data;
    /*当数据从发送数据寄存器转移到发送移位寄存器时，TXE会置高，就可以把下一个数据写入到发送数据寄存器了，虽然此时可能并没有真正发送完成*/
    while ((USART1->SR & USART_FLAG_TXE) == (uint16_t) RESET);  //对DR寄存器写入后该位会自动清零，不需要手动清零
}

void onboard_uart_transmit_byte_array(const uint8_t *pdata, uint32_t length) {
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
    onboard_uart_transmit_byte_array(tempData, 12);
}


/*  数据包部分*/
#define packet_len 4
uint8_t Serial_TxPacket[packet_len];
uint8_t Serial_RxPacket[packet_len];
uint8_t Serial_RxFlag = 0;

void Serial_SendPacket(void) {
    onboard_uart_transmit_byte(0xFF);
    onboard_uart_transmit_byte_array(Serial_TxPacket, 4);
    onboard_uart_transmit_byte(0xFE);
}

void Serial_rx_packet_handler(void) {
    printf_("usart1 接收到了数据包: ");
    for (int i = 0; i < 4; ++i) {
        printf_("%02X ", Serial_RxPacket[i]);
    }
    printf_("\n");
}

void Serial_decode_RxPacket(const uint8_t *pRxPacket, uint8_t len) {
    static volatile uint8_t RxState = 0;  //接收状态机
    static uint8_t pRxPacket_len = 0;  //指示接收到哪一个了

    uint8_t current_rxdata = 0;
    for (int i = 0; i < len; ++i) {
        current_rxdata = pRxPacket[i];
        switch (RxState) {
            case 0:
                if (current_rxdata == 0xff) {
                    RxState = 1;
                    pRxPacket_len = 0;
                }
                break;
            case 1:
                Serial_RxPacket[pRxPacket_len] = current_rxdata;
                pRxPacket_len++;
                if (pRxPacket_len >= 4) {
                    RxState = 2;
                    Serial_RxFlag = 1;
                }
                break;
            case 2:
                if (current_rxdata == 0xfe) {
                    RxState = 0;
                    Serial_rx_packet_handler();
                }
                break;
        }
    }
}

/**
 * 启动一次uart1dma发送
 */
void uart1_tx_dma(uint8_t cnt) {

    DMA_SetCurrDataCounter(DMA2_Stream7, cnt);
    DMA_Cmd(DMA2_Stream7, ENABLE);

    /**
     * 此处代码检测DMA是否成功使能，配置失败可能会导致使能失败
     */
    __IO uint32_t Timeout = 10000;
    while ((DMA_GetCmdStatus(DMA2_Stream7) != ENABLE) && (Timeout-- > 0)) {
    }
    /* Check if a timeout condition occurred */
    if (Timeout == 0) {
        /* Manage the error: to simplify the code enter an infinite loop */
        while (1) {
            dap_uart_debug_printf("dma_start error\n");
        }
    }

    while (DMA_GetFlagStatus(DMA2_Stream7, DMA_FLAG_TCIF7) == RESET); //等待转运完成
    /**
     * 此处非常重要！如果要软件触发dma转换，转换完成后必须手动清除TCIFx，否则系统不会进行下一次转换
     */
    DMA_ClearFlag(DMA2_Stream7, DMA_FLAG_TCIF7);

    DMA_Cmd(DMA2_Stream7, DISABLE);
}