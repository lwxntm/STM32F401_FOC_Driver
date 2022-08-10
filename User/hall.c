//
// Created by xiaotian on 2022/8/6.
//

#include "hall.h"


void hall_init(void) {
    // PC6 HALLA, PC7 HALLB, PC8 HALLC

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);  //
    GPIO_InitTypeDef GPIO_InitStruct = {
            .GPIO_Pin=GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8,
            .GPIO_PuPd=GPIO_PuPd_NOPULL,
            .GPIO_Mode=GPIO_Mode_AF  //复用功能
    };
    GPIO_Init(GPIOC, &GPIO_InitStruct);
    //复用为AF2-TIM3
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_TIM3);
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_TIM3);
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource8, GPIO_AF_TIM3);

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);  //  使能TIM3时钟
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM3, ENABLE);  // 清零TIM3全部寄存器
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM3, DISABLE); //结束清零

    CLR_BIT(TIM3->CR1, 3 << 8); //tDTS时钟不分频
    SET_BIT(TIM3->CR1, 1 << 7); //自动预装载使能
    CLR_BIT(TIM3->CR1, 3 << 5); //边缘对其模式
    CLR_BIT(TIM3->CR1, 1 << 4); //向上计数
    CLR_BIT(TIM3->CR1, 1 << 3); //持续计数模式
    CLR_BIT(TIM3->CR1, 3 << 1); //选择所有的事件都能触发更新中断和dma请求， 启用update event

    SET_BIT(TIM3->CR2, 1 << 7);  //TIMx_CH1、TIMx_CH2 和 TIMx_CH3 引脚连接到 TI1 输入（异或组合）

    MODIFY_REG(TIM3->SMCR, 7 << 4, 5 << 4);//选择 滤波后的定时器输入1 用于同步计数器的触发输入
    MODIFY_REG(TIM3->SMCR, 7 << 0, 4 << 0);//复位模式––在出现所选触发输入 (TRGI) 上升沿时，重新初始化计数器并生成一个寄存器更新事件。

    SET_BIT(TIM3->DIER, 1 << 1); //捕获/比较 1 中断使能 (Capture/Compare 1 interrupt enable)

    SET_BIT(TIM3->CCMR1, 0x0f >> 4); //Fsample=fDTS/32, N=8;    =120Mhz/32，采集到8次边缘跳动算一个事件
    CLR_BIT(TIM3->CCMR1, 3 << 2); // 0：无预分频器，捕获输入上每检测到一个边沿便执行捕获
    MODIFY_REG(TIM3->CCMR1, 3 << 0, 1 << 0); //CC1 通道配置为输入，IC1 映射到 TI1 上

    SET_BIT(TIM3->CCER, 1 << 1 | 1 << 3);//非反相/上升沿和下降沿均触发TI1FP1
    SET_BIT(TIM3->CCER, 1 << 0);//使能将计数器值捕获到输入捕获/比较寄存器 1 (TIMx_CCR1) 中
    TIM3->PSC = 0;//不分频， CK_CNT 等于 fCK_PSC / 1
    TIM3->ARR = 0xffff;//自动重载值 (Auto-reload value)最高

    //开启TIM3全局中断
    NVIC_InitTypeDef NVIC_InitStruct = {
            .NVIC_IRQChannel=TIM3_IRQn,
            .NVIC_IRQChannelCmd=ENABLE,
            .NVIC_IRQChannelPreemptionPriority=7,
            .NVIC_IRQChannelSubPriority=0
    };
    NVIC_Init(&NVIC_InitStruct);

    SET_BIT(TIM3->EGR, 1 << 0);//重新初始化计数器并生成寄存器更新事件
    TIM_ClearFlag(TIM3, TIM_FLAG_Update | TIM_FLAG_CC1 | TIM_FLAG_CC1OF);
    TIM_SetCounter(TIM3, 0); //计数器清零
    TIM_Cmd(TIM3, ENABLE);
}

void TIM3_IRQHandler(void) {
    if (TIM_GetFlagStatus(TIM3, TIM_FLAG_CC1) != RESET) {
        CLR_BIT(TIM3->SR, TIM_FLAG_Update | TIM_FLAG_CC1 | TIM_FLAG_CC1OF);
        dap_uart_debug_printf("CCR1 = %lu, HALLA= %d, HALLB= %d, HALLC= %d\n", TIM3->CCR1,
                              ((GPIOC->IDR) & GPIO_Pin_6) != 0, ((GPIOC->IDR) & GPIO_Pin_7) != 0,
                              ((GPIOC->IDR) & GPIO_Pin_8) != 0);
    }
}