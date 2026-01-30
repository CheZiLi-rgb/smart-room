#include "stm32f10x.h"                  // Device header
#include "Beep.h"
#include "LED.h"
#include "HC-SR501.h"
#include "Key.h"
#include "OLED.h"
#include "Control.h"
#include "USART.h"
#include "Delay.h"
#include "LabVIEW.h"

extern uint8_t Temperature;
//温度
extern uint8_t Humidity;
//湿度
extern uint16_t Smoke;
//烟雾浓度
extern uint16_t Lightintensity;
//光照强度
extern uint8_t TemperatureHigh;
//温度最高阈值
extern uint8_t TemperatureLow;
//温度最低阈值
extern uint8_t HumidityHigh;
//湿度最高阈值
extern uint8_t HumidityLow;
//湿度最低阈值
extern uint16_t SmokeHigh;
//烟雾报警
extern uint16_t LightintensityHigh;
//光照最高阈值
extern uint16_t LightintensityLow;
//光照最低阈值
extern uint8_t Beep_Number;
extern uint8_t Beep_Time_Flag;
extern uint8_t HeatingRelay_Auto_Flag ; 
extern uint8_t WettingRelay_Auto_Flag ;
extern uint8_t Curtain_Motor_Auto_Flag;
extern uint8_t Fan_Motor_Auto_Flag    ;//0 - 关  1 - 开
extern uint16_t Light_Voltage_Value   ;
extern uint8_t Temperature_Decimals ;
extern uint8_t Humidity_Decimals    ;


void Temperature_High_Alarm(void)
{	
	if(Temperature > TemperatureHigh)
    {
		LED1_Turn(); 
        LED2_OFF();		
		HeatingRelay_Auto_OFF();		
        Fan_Motor_Auto_ON();			//开启控制风扇的电机	
	}
}


void Temperature_Low_Alarm(void)
{
	if(Temperature < TemperatureLow)
    {		
		LED2_Turn();
		LED1_OFF();
		HeatingRelay_Auto_ON();         //开启控制加热继电器
	}
}

void Humidity_High_Alarm(void)
{
    if(Humidity > HumidityHigh)
    {
		LED3_Turn();
        LED4_OFF();		
		WettingRelay_Auto_OFF();
		Fan_Motor_Auto_ON();//开启风扇减湿
	}
}

void Humidity_Low_Alarm(void)
{
	if(Humidity < HumidityLow)
    {	
		LED4_Turn();
        LED3_OFF();		
		WettingRelay_Auto_ON();//开启加湿器加湿		
	}
}

void Lighting_High_Treatment(void)
{
	if( Lightintensity > LightintensityHigh)
	{
		Curtain_Motor_Auto_ON();	    
	}
}

void Lighting_Low_Treatment(void)
{
	if( Lightintensity < LightintensityLow)
	{
	    Curtain_Motor_Auto_OFF();	
	}
}

void Smoke_High_Alarm(void)
{
	if(Smoke > SmokeHigh)
    {	
		LED5_Turn();		
		Fan_Motor_Auto_ON();//开启风扇
	}
}

void Alarm_Message(void)
{
	if( Temperature > TemperatureHigh )
	{
	    USART1_SendString("The Temperature is too High:");
	    USART1_SendNumber(Temperature,2);
        USART1_SendString(".");
		USART1_SendNumber(Temperature_Decimals,2);
        USART1_SendString("\r\n");
	}
	if( Temperature < TemperatureLow )
	{
		USART1_SendString("The Temperature is too Low:");
	    USART1_SendNumber(Temperature,2);
		USART1_SendString(".");
		USART1_SendNumber(Temperature_Decimals,2);	
		USART1_SendString("\r\n");
	}
	if( Humidity > HumidityHigh )
	{
	    USART1_SendString("The Humidity is too High:");
	    USART1_SendNumber(Humidity,2);
		USART1_SendString(".");
		USART1_SendNumber(Humidity_Decimals,2);	
        USART1_SendString("\r\n");	
	}	
	if( Humidity < HumidityLow )
	{
	    USART1_SendString("The Humidity is too Low:");
        USART1_SendNumber(Humidity,2);
		USART1_SendString(".");
		USART1_SendNumber(Humidity_Decimals,2);
        USART1_SendString("\r\n");
	}
    if( Smoke > SmokeHigh )
	{
		USART1_SendString("The Smoke is too High:");
		USART1_SendNumber(Smoke,4);	
		USART1_SendString("\r\n");
	}	
	if( Lightintensity > LightintensityHigh)
	{
	    USART1_SendString("The Lightintensity is too High:");
		USART1_SendNumber(Lightintensity,4);
		USART1_SendString("\r\n");
	}
	if(Lightintensity < LightintensityLow)
	{
	    USART1_SendString("The Lightintensity is too Low:");
		USART1_SendNumber(Lightintensity,4);
		USART1_SendString("\r\n");
	}	
	if(HCSR_Get() == 1)
	{
		USART1_SendString("There are strangers who break into the house illegally!");
		USART1_SendString("\r\n");	
	}
}

void Human_Detection(void)
{
	if( HCSR_Get() == 1)
	{	
		if( HCSR_Get() == 1 && Beep_Time_Flag == 0)
		{
			Beep_Init();
			Beep_Time_Flag = 1;		
		}
        for( Beep_Number = 0 ; Beep_Number < 5 ;Beep_Number ++)		
		{
			Beep_Turn();
	        Alarm_Message();
			LabVIEW_Receive();
		}
	}
}
