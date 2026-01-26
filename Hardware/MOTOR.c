#include "stm32f10x.h"                  // Device header
#include "Key.h"

void PWM1_Init(void)
{
	//PA2  风扇电机  TIM2 - CH3 
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	
    GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//选择复用推挽输出
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;      //PA2  窗帘电机
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);

	TIM_InternalClockConfig(TIM2);
	
	//配置时基单元
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_Period = 100 - 1;                //预分频器 ARR
	TIM_TimeBaseInitStructure.TIM_Prescaler = 36 - 1;                //周期 PSC
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStructure);	

	//配置好输出比较单元,包括CCR的值输出比较模式 极性选择等	
	TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_OCStructInit(&TIM_OCInitStructure); 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;             //设置输出比较的模式
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;     //设置输出比较的极性
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//设置输出使能
	TIM_OCInitStructure.TIM_Pulse = 0; //设置CCR寄存器的值   
	TIM_OC3Init(TIM2,&TIM_OCInitStructure);
	TIM_Cmd(TIM2,ENABLE);
}

void PWM1_SetCompare3(uint16_t Compare)
{
    TIM_SetCompare3(TIM2,Compare);//用于比较定时器计数值的函数 (定时器计数值和比较值)
}

void PWM2_Init(void)
{
	//PA3  窗帘电机   TIM2 - CH4
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//选择复用推挽输出
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;      //PA3 风扇电机
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);

	TIM_InternalClockConfig(TIM2);
	
	//配置时基单元
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_Period = 100 - 1;                //预分频器 ARR
	TIM_TimeBaseInitStructure.TIM_Prescaler = 36 - 1;                //周期 PSC
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStructure);	

	//配置好输出比较单元,包括CCR的值输出比较模式 极性选择等	
	TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_OCStructInit(&TIM_OCInitStructure); 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;             //设置输出比较的模式
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;     //设置输出比较的极性
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//设置输出使能
	TIM_OCInitStructure.TIM_Pulse = 50; //设置CCR寄存器的值   
	TIM_OC4Init(TIM2,&TIM_OCInitStructure);
	TIM_Cmd(TIM2,ENABLE);
}

void PWM2_SetCompare4(uint16_t Compare)
{
    TIM_SetCompare4(TIM2,Compare);//用于比较定时器计数值的函数 (定时器计数值和比较值)
}

void Motor1_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//电机方向控制的两个引脚
	GPIO_Init(GPIOA,&GPIO_InitStructure);
    PWM1_Init();   //初始化电机的PWM        
}

void Motor1_SetSpeed(int8_t Speed)
{
	
	if (Speed > 0)
	{
		GPIO_SetBits(GPIOA, GPIO_Pin_10);
		GPIO_ResetBits(GPIOA, GPIO_Pin_11);
		PWM1_SetCompare3(Speed);
	}
	else if(Speed == 0)
	{
		GPIO_ResetBits(GPIOA,GPIO_Pin_10);
		GPIO_ResetBits(GPIOA,GPIO_Pin_11);
		PWM1_SetCompare3(Speed);
	}
	else
	{
		GPIO_ResetBits(GPIOA, GPIO_Pin_10);
		GPIO_SetBits(GPIOA, GPIO_Pin_11);
		PWM1_SetCompare3(-Speed);
	}
}

void Motor2_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//电机方向控制的两个引脚
	GPIO_Init(GPIOB,&GPIO_InitStructure);
    PWM2_Init();   //初始化电机的PWM     
}

void Motor2_SetSpeed(int8_t Speed)     //窗帘电机调速
{
	if (Speed > 0)
	{
		GPIO_SetBits(GPIOB, GPIO_Pin_14);
		GPIO_ResetBits(GPIOB, GPIO_Pin_15);
		PWM2_SetCompare4(Speed);
	}
    else if(Speed == 0)
	{
		GPIO_ResetBits(GPIOB,GPIO_Pin_14);
		GPIO_ResetBits(GPIOB,GPIO_Pin_15);
		PWM2_SetCompare4(Speed);
	}		
	else
	{
		GPIO_ResetBits(GPIOB, GPIO_Pin_14);
		GPIO_SetBits(GPIOB, GPIO_Pin_15);
		PWM2_SetCompare4(-Speed);
	}
}
