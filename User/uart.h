//
// Created by xiaotian on 2022/8/5.
//

#pragma once
#include "main.h"
void onboard_uart1_init(void);
void onboard_uart_transmit(const uint8_t *pdata, uint32_t length);
void just_float_transmit(float f1, float f2);