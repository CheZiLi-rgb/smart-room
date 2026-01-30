#include "stm32f10x.h"                  // Device header
#include "Delay.h"

extern const uint16_t Tone[21];
extern const uint8_t Music_Tone[25];
extern const uint8_t Music_Time[25];
extern uint8_t Music_Time_Statistics;
extern uint8_t i_Mode ;            //用于统计Key_Mode按下的次数
extern uint8_t Music_Time_Flag ;   //蜂鸣器音乐标志位

void Music_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	TIM_InternalClockConfig(TIM4);
	//定时器配置 产生频率 根据音符和频率关系
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = 1000000 / 262 - 1;  //0 - 3816
	TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1;   //1MHZ 1000000
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseInitStructure);
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCStructInit(&TIM_OCInitStructure);
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 3816 / 2;		//CCR 设置比较值产生占空比
	TIM_OC4Init(TIM4, &TIM_OCInitStructure);
    TIM_OC4PreloadConfig(TIM4,TIM_OCPreload_Enable);
	TIM_OC4PreloadConfig(TIM4,ENABLE);
	TIM_Cmd(TIM4,ENABLE);
}

void Set_PWM(uint16_t Period,uint16_t Pulse)
{
	TIM_SetAutoreload(TIM4,Period);
	TIM_SetCompare4(TIM4,Pulse);
}

void Music_Show(void)
{
    for(Music_Time_Statistics = 0; Music_Time_Statistics < 25; Music_Time_Statistics ++)
	{
	    Set_PWM(Tone[Music_Tone[Music_Time_Statistics]],Tone[Music_Tone[Music_Time_Statistics]] / 2);
	    Delay_ms(Music_Time[Music_Time_Statistics]* 62.5);
	}
	TIM_SetCompare4(TIM4,0);
	Delay_ms(2000);
}
