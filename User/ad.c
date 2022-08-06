//
// Created by xiaotian on 2022/8/5.
//

#include "ad.h"

uint16_t ad_value[2];
uint16_t ad_amount = 2;

/**
 * 板载adc资源初始化
 */
void onboard_adc_init(void) {
    /* Enable peripheral clocks *************************************************/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,ENABLE);	                           //ADC1复位
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,DISABLE);                           //复位结束
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);   //开启dma时钟

    /* DMA2_Stream0 channel0 configuration **************************************/
    DMA_DeInit(DMA2_Stream0);
    DMA_InitTypeDef DMA_InitStruct = {
            .DMA_Channel=DMA_Channel_0,
            .DMA_PeripheralBaseAddr=((uint32_t)&ADC1->DR),     //这里如果用ADC1->DR前往要记得加个取地址的符号！！不然要踩坑了（不小心会忘记）
            .DMA_PeripheralDataSize=DMA_PeripheralDataSize_HalfWord,
            .DMA_PeripheralInc=DMA_PeripheralInc_Disable,           //外设地址是否自增
            .DMA_Memory0BaseAddr=(uint32_t) ad_value,
            .DMA_MemoryDataSize=DMA_MemoryDataSize_HalfWord,               //每一个内存数据的大小，字节/半字/字
            .DMA_MemoryInc=DMA_MemoryInc_Enable,   //内存地址是否自增
            .DMA_DIR=DMA_DIR_PeripheralToMemory,    //dma方向，外设到内存，内存到内存，内存到外设
            .DMA_BufferSize=ad_amount,     //需要dma搬运的数据数量
            .DMA_Mode=DMA_Mode_Circular,  //单次还是循环
            .DMA_Priority=DMA_Priority_High,
            .DMA_FIFOMode=DMA_FIFOMode_Disable,           //不使用FIFO
            .DMA_FIFOThreshold = DMA_FIFOThreshold_Full,
            .DMA_MemoryBurst = DMA_MemoryBurst_Single,
            .DMA_PeripheralBurst = DMA_PeripheralBurst_Single
    };
    DMA_Init(DMA2_Stream0, &DMA_InitStruct);
    DMA_Cmd(DMA2_Stream0, ENABLE);

    //配置pc0为模拟
    GPIO_InitTypeDef GPIO_InitStruct = {
            .GPIO_Mode=GPIO_Mode_AN,
            .GPIO_Pin=GPIO_Pin_0,
            .GPIO_PuPd=GPIO_PuPd_NOPULL
    };
    GPIO_Init(GPIOC, &GPIO_InitStruct);
    //配置pc1为模拟
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;
    GPIO_Init(GPIOC, &GPIO_InitStruct);


    //配置adc的时钟
    ADC_CommonInitTypeDef ADC_CommonInitStruct = {
            .ADC_Mode=ADC_Mode_Independent,   //F401只有一个adc，所以只能是独立模式
            .ADC_Prescaler=ADC_Prescaler_Div4,  //F adc=PCL2/Prescaler=120/4=30MHz
            .ADC_DMAAccessMode=ADC_DMAAccessMode_1,
            .ADC_TwoSamplingDelay=ADC_TwoSamplingDelay_5Cycles
    };
    ADC_CommonInit(&ADC_CommonInitStruct);


    ADC_InitTypeDef ADC_InitStruct = {
            .ADC_Resolution=ADC_Resolution_12b,
            .ADC_ScanConvMode=ENABLE,  //ENABLE表示扫描模式，DISABLE表示只转换规则组第一个通道，
            .ADC_DataAlign=ADC_DataAlign_Right,  //数据右对齐，这样直接读到的就是adc的值
            .ADC_ExternalTrigConvEdge=ADC_ExternalTrigConvEdge_None, //不使用外部触发
            .ADC_ContinuousConvMode=ENABLE, //连续转换模式
            .ADC_NbrOfConversion=ad_amount, //转换规则组的前两个通道
    };
    ADC_Init(ADC1, &ADC_InitStruct);


    //配置adc1的输入通道10为规则组的第一个采样目标，输入通道11为规则组的第2个采样目标. 采样时间为56个adc周期，采样时间越长数据越稳定（也就越慢）
    ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_15Cycles);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 2, ADC_SampleTime_15Cycles);

    ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);


    ADC_DMACmd(ADC1, ENABLE);
    //开启adc电源
    ADC_Cmd(ADC1, ENABLE);

    //STM32F4 adc 不需要再软件校准
    ADC_SoftwareStartConv(ADC1); //软件触发转换
}


void ad_getValue(void) {
//    DMA_Cmd(DMA2_Stream0, DISABLE);
//    DMA_SetCurrDataCounter(DMA2_Stream0, ad_amount);
//    DMA_Cmd(DMA2_Stream0, ENABLE);
    ADC_SoftwareStartConv(ADC1); //软件触发转换
//    while (DMA_GetFlagStatus(DMA2_Stream0, DMA_FLAG_TCIF0) == RESET); //等待转运完成
//    /**
//     * 此处非常重要！如果要软件触发dma转换，转换完成后必须手动清除TCIFx，否则系统不会进行下一次转换
//     */
//    DMA_ClearFlag(DMA2_Stream0, DMA_FLAG_TCIF0);
}