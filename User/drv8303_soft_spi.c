//
// Created by xiaotian on 2022/8/5.
//
#include "stdint.h"
#include "stm32f4xx.h"

/**
 * 初始化PC10 11 12 PD2为 drv8303spi通信用的引脚
 */
void softspi_init(void) {
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);  //spi 引脚在gpioc上
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);//cs 引脚在gpiod上

    GPIO_InitTypeDef GPIO_InitStructure = {
            .GPIO_Pin=GPIO_Pin_10,
            .GPIO_Mode=GPIO_Mode_OUT,
            .GPIO_OType=GPIO_OType_PP,
            .GPIO_PuPd=GPIO_PuPd_NOPULL,
            .GPIO_Speed=GPIO_Fast_Speed
    };
    GPIO_Init(GPIOC, &GPIO_InitStructure); //初始化pc10为 sck
    GPIO_ResetBits(GPIOC, GPIO_Pin_10);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_Init(GPIOC, &GPIO_InitStructure); //初始化pc12为 mosi
    GPIO_ResetBits(GPIOC, GPIO_Pin_12);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_Init(GPIOD, &GPIO_InitStructure); //初始化pd2为 cs
    GPIO_SetBits(GPIOD, GPIO_Pin_2);


    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_Init(GPIOC, &GPIO_InitStructure); //初始化pc11为 MISO
}

void chipselect(uint8_t status) {
    if (status) {
        GPIOD->BSRR = (1 << 2);
    } else {
        GPIOD->BSRR = (1 << 2) << 16;
    }
}

void spi3sck(uint8_t clk) {
    if (clk) {
        GPIOC->BSRR = (1 << 10);
    } else {
        GPIOC->BSRR = (1 << 10) << 16;
    }
}

//write data
void spi3writeword(uint16_t data) {
    chipselect(0);
    for (int j = 0; j < 16; ++j) {
        spi3sck(1);
        if (data & 0x8000 >> j) {
            GPIOC->BSRR = (1 << 12);
        } else {
            GPIOC->BSRR = (1 << 12) << 16;
        }
        spi3sck(0);
    }
    GPIOC->BSRR = (1 << 12) << 16;  //MOSI空闲时低电平
    chipselect(1);
}

uint16_t spi3readword(void) {
    uint16_t data = 0;
    chipselect(0);
    for (int i = 0; i < 16; ++i) {
        spi3sck(1);
        spi3sck(0);
        if (GPIOC->IDR & (1 << 11)) //如果c11是高电平
        {
            data |= 0x8000 >> i; //MSB在前，先接收高位字节
        }
    }
    chipselect(1);
    return data;
}

uint16_t drv8303_read_reg(uint8_t reg) {
    uint16_t data = 0x8000 | (reg << 11);
    chipselect(0);

    for (int j = 0; j < 16; ++j) {
        spi3sck(1);
        if (data & 0x8000 >> j) {
            GPIOC->BSRR = (1 << 12);
        } else {
            GPIOC->BSRR = (1 << 12) << 16;
        }
        spi3sck(0);
    }
    GPIOC->BSRR = (1 << 12) << 16;  //MOSI空闲时低电平
    chipselect(1);
    chipselect(0);
    data = 0;
    for (int i = 0; i < 16; ++i) {
        spi3sck(1);
        spi3sck(0);
        if (GPIOC->IDR & (1 << 11)) //如果c11是高电平
        {
            data |= 0x8000 >> i; //MSB在前，先接收高位字节
        }
    }
    chipselect(1);
    return data;
}

uint16_t drv8303IDread(void) {
    uint16_t command = 0;
    uint16_t regvalue = 0;
    command = 0x8800;
    spi3writeword(command);
    regvalue = spi3readword();
    return regvalue & 0x0f;
}

/**
 * drv8303 寄存器测试
 * @return  0为正常，1为错误
 */
uint8_t drv8303_reg_test(void) {
    uint16_t reg_will_be_write = 0;
    uint16_t command = 0;
    command = 0x0000 |//写入模式
              (0x02 << 11) | //要写入的寄存器
              reg_will_be_write; //写入的值
    spi3writeword(command);
    uint16_t ret = drv8303_read_reg(0x02);
    if ((ret & (0x07FF))  //取读出数据的D10-D0
        == reg_will_be_write)
        return 0;
    else
        return 1;
}