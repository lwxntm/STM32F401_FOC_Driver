//
// Created by xiaotian on 2022/8/5.
//

#pragma once
#include "main.h"


extern uint8_t usart1_rx_dma_buffer[128];



void onboard_uart1_init(void);
void onboard_uart_transmit_byte_array(const uint8_t *pdata, uint32_t length);
void just_float_transmit(float f1, float f2);


extern uint8_t Serial_TxPacket[];
extern uint8_t Serial_RxPacket[];
extern uint8_t Serial_RxFlag;


void Serial_decode_RxPacket(const uint8_t *pRxPacket, uint8_t len);
        void Serial_SendPacket(void);

void DMA_Enable(DMA_Stream_TypeDef *DMA_Streamx,u16 ndtr);