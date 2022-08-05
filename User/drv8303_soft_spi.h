//
// Created by xiaotian on 2022/8/5.
//

#pragma once


typedef struct DRV8303CFG{
    uint16_t gate_current;//门级驱动电流大小
    uint16_t gate_reset;//门级驱动重启状态

} drv8303cfg;


void softspi_init(void);

uint16_t drv8303IDread(void);
uint8_t drv8303_reg_test(void );