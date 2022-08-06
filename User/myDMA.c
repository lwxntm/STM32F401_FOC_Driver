//
// Created by xiaotian on 2022/8/6.
//

#include "myDMA.h"

uint16_t data_amount;



/**
 * dma内存到内存，按字节转运初始化
 * @param aSRC  内存源地址
 * @param aDST  内存目标地址
 * @param amount 需要转运的数据数量
 */
void myDMA_init(uint32_t aSRC, uint32_t aDST, uint32_t amount) {
    data_amount = amount;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
    DMA_InitTypeDef DMA_InitStruct = {
            .DMA_Channel=DMA_Channel_0,
            .DMA_PeripheralBaseAddr=aSRC,
            .DMA_PeripheralDataSize=DMA_PeripheralDataSize_Byte,
            .DMA_PeripheralInc=DMA_PeripheralInc_Enable,           //外设地址是否自增
            .DMA_Memory0BaseAddr=aDST,
            .DMA_MemoryDataSize=DMA_MemoryDataSize_Byte,               //每一个内存数据的大小，字节/半字/字
            .DMA_MemoryInc=DMA_MemoryInc_Enable,   //内存地址是否自增
            .DMA_DIR=DMA_DIR_MemoryToMemory,    //dma方向，外设到内存，内存到内存，内存到外设
            .DMA_BufferSize=data_amount,     //需要dma搬运的数据数量
            .DMA_Mode=DMA_Mode_Normal,  //单次还是循环
            .DMA_Priority=DMA_Priority_High,
            .DMA_FIFOMode=DMA_FIFOMode_Disable,           //不使用FIFO
            .DMA_FIFOThreshold = DMA_FIFOThreshold_Full,
            .DMA_MemoryBurst = DMA_MemoryBurst_Single,
            .DMA_PeripheralBurst = DMA_PeripheralBurst_Single
    };
    DMA_Init(DMA2_Stream0, &DMA_InitStruct);
    DMA_Cmd(DMA2_Stream0, DISABLE);
}

void myDMA_transfer(void) {
    DMA_SetCurrDataCounter(DMA2_Stream0, data_amount);
    DMA_Cmd(DMA2_Stream0, ENABLE);

    /**
     * 此处代码检测DMA是否成功使能，配置失败可能会导致使能失败
     */
    __IO uint32_t Timeout = 10000;
    while ((DMA_GetCmdStatus(DMA2_Stream0) != ENABLE) && (Timeout-- > 0)) {
    }
    /* Check if a timeout condition occurred */
    if (Timeout == 0) {
        /* Manage the error: to simplify the code enter an infinite loop */
        while (1) {
            dap_uart_debug_printf("dma_start error\n");
        }
    }

    while (DMA_GetFlagStatus(DMA2_Stream0, DMA_FLAG_TCIF0) == RESET); //等待转运完成
    /**
     * 此处非常重要！如果要软件触发dma转换，转换完成后必须手动清除TCIFx，否则系统不会进行下一次转换
     */
    DMA_ClearFlag(DMA2_Stream0, DMA_FLAG_TCIF0);

    DMA_Cmd(DMA2_Stream0, DISABLE);
}

