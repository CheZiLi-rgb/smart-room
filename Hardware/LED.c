#include "stm32f10x.h"                  // Device header
#include "Delay.h"
//光照报警电路

extern uint8_t LED1_Flag      ;
extern uint8_t LED2_Flag      ;
extern uint8_t LED3_Flag      ;
extern uint8_t LED4_Flag      ;

void LED_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	//PA4 PA5 PC13 PC14 PC15
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
    //初始化PA4 PA5
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC,&GPIO_InitStructure);
	//初始PC13 PC14 PC15
	GPIO_SetBits(GPIOC,GPIO_Pin_13);
}

void LED1_ON(void)
{
	LED1_Flag = 1;
	GPIO_SetBits(GPIOA, GPIO_Pin_4);	
}

void LED1_OFF(void)
{
	LED1_Flag = 0;
	GPIO_ResetBits(GPIOA, GPIO_Pin_4);	
}

void LED1_Turn(void)
{
	LED1_Flag = 2;
	GPIO_SetBits(GPIOA, GPIO_Pin_4);
    Delay_ms(500);
	GPIO_ResetBits(GPIOA, GPIO_Pin_4);
	Delay_ms(500);		
}

void LED2_ON(void)
{
	GPIO_SetBits(GPIOA, GPIO_Pin_5);
	LED2_Flag = 1;
}

void LED2_OFF(void)
{
	GPIO_ResetBits(GPIOA, GPIO_Pin_5);
	LED2_Flag = 0;
}

void LED2_Turn(void)
{
	GPIO_SetBits(GPIOA, GPIO_Pin_5);
    Delay_ms(500);
	GPIO_ResetBits(GPIOA, GPIO_Pin_5);
	Delay_ms(500);
	LED2_Flag = 2;
}

void LED3_ON(void)
{
	GPIO_SetBits(GPIOC, GPIO_Pin_14);
	LED3_Flag = 1;
}

void LED3_OFF(void)
{
	GPIO_ResetBits(GPIOC, GPIO_Pin_14);
	LED3_Flag = 0;
}

void LED3_Turn(void)
{
	GPIO_SetBits(GPIOC, GPIO_Pin_14);
    Delay_ms(500);
	GPIO_ResetBits(GPIOC, GPIO_Pin_14);
	Delay_ms(500);
	LED3_Flag = 2;
}

void LED4_ON(void)
{
	GPIO_SetBits(GPIOC, GPIO_Pin_15);
	LED4_Flag = 1;
}

void LED4_OFF(void)
{
	GPIO_ResetBits(GPIOC, GPIO_Pin_15);
	LED4_Flag = 0;
}

void LED4_Turn(void)
{
	GPIO_SetBits(GPIOC, GPIO_Pin_15);
    Delay_ms(500);
	GPIO_ResetBits(GPIOC, GPIO_Pin_15);
	Delay_ms(500);
	LED4_Flag = 2;
}
	
void LED5_OFF(void)
{
    GPIO_SetBits(GPIOC,GPIO_Pin_13);
}

void LED5_ON(void)
{
    GPIO_ResetBits(GPIOC,GPIO_Pin_13);
}

void LED5_Turn(void)
{
    LED5_ON();
    Delay_ms(500);
	LED5_OFF();
	Delay_ms(500);
}

void LED6_Breathing_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	TIM_InternalClockConfig(TIM4);
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = 100 -1;
	TIM_TimeBaseInitStructure.TIM_Prescaler = 720 - 1;
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseInitStructure);
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCStructInit(&TIM_OCInitStructure);
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;		//CCR
	TIM_OC3Init(TIM4, &TIM_OCInitStructure);
    TIM_Cmd(TIM4,ENABLE);
}

void PWM_SetCompare3(uint16_t Compare)
{
	TIM_SetCompare3(TIM4, Compare);
}

extern uint8_t LED6_Breathing_Number;

void LED6_Breathing(void)
{	
	for (LED6_Breathing_Number = 0; LED6_Breathing_Number <= 100; LED6_Breathing_Number ++)
	{
		PWM_SetCompare3(LED6_Breathing_Number);
		Delay_ms(10);
	}
	for (LED6_Breathing_Number = 0; LED6_Breathing_Number <= 100; LED6_Breathing_Number ++)
	{
		PWM_SetCompare3(100 - LED6_Breathing_Number);
		Delay_ms(10);
	}
}

void Running_Water_Light(void)
{
	LED1_ON();
	Delay_ms(500);
	LED1_OFF();
	LED2_ON();
	Delay_ms(500);
	LED2_OFF();
	LED3_ON();
	Delay_ms(500);
	LED3_OFF();
	LED4_ON();
	Delay_ms(500);
	LED4_OFF();
	LED5_ON();
	Delay_ms(500);
	LED5_OFF();
}

void All_LED_OFF(void)
{
	LED1_OFF();
	LED2_OFF();
	LED3_OFF();
	LED4_OFF();
	LED5_OFF();
	PWM_SetCompare3(0);
}

void All_LED_ON(void)
{
	LED1_ON();
	LED2_ON();
	LED3_ON();
	LED4_ON();
	LED5_ON();
	PWM_SetCompare3(100);
}
