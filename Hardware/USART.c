#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include <stdarg.h>
#include "LabVIEW.h"

void USART1_Init(uint32_t USART_BaudRate)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	//PA-9  TX  USART1
	GPIO_InitTypeDef GPIO_InitStrucuture;
	GPIO_InitStrucuture.GPIO_Mode = GPIO_Mode_AF_PP;   //推挽输出
	GPIO_InitStrucuture.GPIO_Pin = GPIO_Pin_9;         //TX
	GPIO_InitStrucuture.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStrucuture);
   
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = USART_BaudRate;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART1,&USART_InitStructure);
	USART_Cmd(USART1, ENABLE);
}
//发送字节
void USART1_SendByte(uint8_t Byte)  
{
	USART_SendData(USART1,Byte);
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET); //读完后TXE置1 让他一直发送
}
//发送数组
void USART1_SendArray(uint8_t *Array,uint16_t Length)
{
    uint16_t i;
	for(i = 0;i < Length; i ++)
	{
	    USART1_SendByte(Array[i]);
	}
}
//发送字符串
void USART1_SendString(char *String)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i ++)
	{
		USART1_SendByte(String[i]);
	}
}
//十进制转二进制
uint32_t USART_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;
	while (Y --)
	{
		Result = Result * X;
	}
	return Result;
}

void USART1_SendNumber(uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i ++)
	{
		USART1_SendByte(Number / USART_Pow(10, Length - i - 1) % 10 + '0');
	}
}

int fputc1(int ch, FILE *f)
{
	USART1_SendByte(ch);
	return ch;
}

void USART1_Printf(char *format, ...)
{
	char String[100];
	va_list arg;
	va_start(arg, format);
	vsprintf(String, format, arg);
	va_end(arg);
	USART1_SendString(String);
}

void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    NVIC_Init(&NVIC_InitStructure);
}
//与上位机LabVIEW通信
void USART3_Init(uint32_t USART_BaudRate)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;		
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;      //复用推挽输出
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;           //PB10 TX
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;           //PB11 RX
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	USART_DeInit(USART3);     //串口复位 将外设USART3寄存器重设为缺省值 复位USART3                                 
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = USART_BaudRate;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_StopBits =USART_StopBits_1;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART3,&USART_InitStructure);	
	//初始化NVIC开启指定中断
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);    //中断优先级配置	
	NVIC_Configuration();
	USART_ITConfig(USART3,USART_IT_RXNE,ENABLE);//使能USART3的USART_IT_RXNE中断 意思是接收中断
	USART_Cmd(USART3,ENABLE);
}
void USART3_IRQHandler(void)
{
	LabVIEW_Control_System();
}

void USART3_SendByte(uint8_t Byte)
{
	USART_SendData(USART3,Byte);
	while(USART_GetFlagStatus(USART3,USART_FLAG_TXE) == RESET); //读完后TXE置1 让他一直发送
}

void USART3_SendArray(uint8_t *Array,uint16_t Length)
{
	uint16_t i;
	for(i = 0;i < Length; i ++)
	{
	    USART3_SendByte(Array[i]);
	}
}

void USART3_SendString(char *String)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i ++)
	{
		USART3_SendByte(String[i]);
	}
}

void USART3_SendNumber(uint32_t Number,uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i ++)
	{
		USART3_SendByte(Number / USART_Pow(10, Length - i - 1) % 10 + '0');
	}
}

void USART3_Printf(char *format, ...)
{
	char String[100];
	va_list arg;
	va_start(arg, format);
	vsprintf(String, format, arg);
	va_end(arg);
	USART3_SendString(String);
}

int fputc3(int ch, FILE *f)
{
	USART3_SendByte(ch);
	return ch;
}
