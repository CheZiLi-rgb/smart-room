#ifndef __USART_H
#define __USART_H

#include <stdio.h>
//与串口助手的短信通讯
void USART1_Init(uint32_t USART_BaudRate);
void USART1_SendByte(uint8_t Byte);
void USART1_SendArray(uint8_t *Array,uint16_t Length);
void USART1_SendString(char *String);
void USART1_SendNumber(uint32_t Number,uint8_t Length);
void USART1_Printf(char *format, ...);
//与上位机LabVIEW的通讯
void USART3_Init(uint32_t USART_BaudRate);
void NVIC_Configuration(void);
void USART3_IRQHandler(void);
void USART3_SendByte(uint8_t Byte);
void USART3_SendArray(uint8_t *Array,uint16_t Length);
void USART3_SendString(char *String);
void USART3_SendNumber(uint32_t Number,uint8_t Length);
void USART3_Printf(char *format, ...);
//其他功能函数
uint32_t USART_Pow(uint32_t X, uint32_t Y);
int fputc1(int ch, FILE *f);
int fputc3(int ch, FILE *f);

uint8_t USART1_GetRxFlag(void);
uint8_t USART1_GetRxData(void);

#endif
