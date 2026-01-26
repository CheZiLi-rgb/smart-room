#include "stm32f10x.h"                  // Device header

void HCSR_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    //PA7
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;//œ¬¿≠ ‰»Î
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOA,GPIO_Pin_7);
}

uint8_t HCSR_Get(void)
{
    return GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_7);
}
