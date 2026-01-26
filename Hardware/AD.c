#include "stm32f10x.h"                  // Device header
#include "Delay.h"

void AD_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);

	GPIO_InitTypeDef  GPIO_InitStructure;
    ADC_InitTypeDef ADC_InitStructure;

	//PA0  PA2
 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_6;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOA,&GPIO_InitStructure);
		
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	    //单次转换
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;           //扫描模式
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;  //数据右对齐
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//外部触发方式
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;     
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	ADC_Init(ADC1,&ADC_InitStructure);
    ADC_Cmd(ADC1,ENABLE);
    //校准ADC
    ADC_ResetCalibration(ADC1);                            //复位校准
    while(ADC_GetResetCalibrationStatus(ADC1) == SET);     //等待复位校准
    ADC_StartCalibration(ADC1);                              
	while(ADC_GetCalibrationStatus(ADC1)== SET);           //等待校准完成
}
//启动转换获得结果
uint16_t AD_GetValue(uint8_t ADC_Channel)
{
	//设置指定ADC的规则组通道 序列 采样时间
  	ADC_RegularChannelConfig(ADC1, ADC_Channel, 1, ADC_SampleTime_55Cycles5);
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);                   //使能指定ADC1软件启动转换功能
	while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);   //等待EOC标志位为1
	return ADC_GetConversionValue(ADC1);                      //返回最近一次ADC1规则组的转换结果
}
//获取通道ADC的平均值
uint16_t Get_Adc_Average(uint8_t channel_number,uint8_t times)  //通道号,采集时间周期
{
	uint32_t temp_value = 0;
	uint8_t time;
	for(time = 0;time < times;time ++)
	{
	    temp_value = temp_value + AD_GetValue(channel_number);
        Delay_ms(5);
	}
	return temp_value / times;  //返回平均值
}
