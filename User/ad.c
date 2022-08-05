//
// Created by xiaotian on 2022/8/5.
//

#include "ad.h"

/**
 * 板载adc资源初始化
 */
void onboard_adc_init(void) {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

    //配置adc的时钟
    ADC_CommonInitTypeDef ADC_CommonInitStruct = {
            .ADC_Mode=ADC_Mode_Independent,   //F401只有一个adc，所以只能是独立模式
            .ADC_Prescaler=ADC_Prescaler_Div4,  //F adc=PCL2/Prescaler=120/4=30MHz
            .ADC_DMAAccessMode=ADC_DMAAccessMode_Disabled,
            .ADC_TwoSamplingDelay=ADC_TwoSamplingDelay_5Cycles
    };
    ADC_CommonInit(&ADC_CommonInitStruct);

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
    //配置adc1的输入通道10为规则组的第一个采样目标，输入通道11为规则组的第2个采样目标. 采样时间为56个adc周期，采样时间越长数据越稳定（也就越慢）
    ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_56Cycles);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 2, ADC_SampleTime_56Cycles);

    ADC_InitTypeDef ADC_InitStruct = {
            .ADC_Resolution=ADC_Resolution_12b,
            .ADC_ScanConvMode=DISABLE,  //ENABLE表示扫描模式，DISABLE表示只转换规则组第一个通道，
            .ADC_DataAlign=ADC_DataAlign_Right,  //数据右对齐，这样直接读到的就是adc的值
            .ADC_ExternalTrigConvEdge=ADC_ExternalTrigConvEdge_None, //不使用外部触发
            .ADC_ContinuousConvMode=DISABLE, //连续转换模式
            .ADC_NbrOfConversion=2, //转换规则组的前两个通道
    };
    ADC_Init(ADC1, &ADC_InitStruct);

    //开启adc电源
    ADC_Cmd(ADC1, ENABLE);

    //STM32F4 adc 不需要再软件校准
}


uint16_t ad_getValue(void) {
    ADC_SoftwareStartConv(ADC1); //软件触发转换
    while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET); //等待转换完成
    return ADC_GetConversionValue(ADC1);
}