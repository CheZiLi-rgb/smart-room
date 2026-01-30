#include "stm32f10x.h"                  // Device header
#include "OLED.h"
#include "LED.h"
#include "Key.h"
#include "Music.h"
#include "AlarmCircuit.h"

extern uint8_t i_Mode ;//用于统计Key_Mode按下的次数
extern uint8_t i_SET  ;//用于统计Key_SET按下的次数
extern uint8_t i_Plus ;//用于切换家居设备的工作状态
extern uint8_t KeyNum ;
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
extern uint8_t LED6_Breathing_Flag;   //呼吸灯标志位
extern uint8_t Music_Time_Flag    ;   //蜂鸣器音乐标志位
extern uint8_t Smart_Home_Mode_Select_Flag;
extern uint8_t Music_Time_Statistics;

void HomeMode(void)                    
{
    if( i_Mode == 5 && i_SET == 1 && KeyNum == 4 && Smart_Home_Mode_Select_Flag == 3)	
    {
	    OLED_Clear();
		OLED_ShowString(1,1,"Breath-LED-ON");
		if( i_Mode == 5 && i_SET == 1 && KeyNum == 4  && LED6_Breathing_Flag == 0 && Smart_Home_Mode_Select_Flag == 3 )
		{
			LED6_Breathing_Init();
			LED6_Breathing_Flag = 1;	
		}	
	    LED6_Breathing();
	}	
	if( i_Mode == 5 && i_SET == 2 && KeyNum == 4 && Smart_Home_Mode_Select_Flag == 3)
    {
		OLED_Clear();
	    OLED_ShowString(1,1,"Water-Light-ON");
	    Running_Water_Light();
	}
	if( i_Mode == 5 && i_SET == 3 && KeyNum == 4 && Smart_Home_Mode_Select_Flag == 3)
	{
	    OLED_Clear();
	    OLED_ShowString(1,1,"Music Show");	
		if(i_Mode == 5 && i_SET == 3 && KeyNum == 4 && Smart_Home_Mode_Select_Flag ==3 && Music_Time_Flag == 0)
		{
			Music_Init();		
		    Music_Time_Flag = 1;		
		}
		if(i_Mode == 5 && i_SET == 3 && KeyNum == 4 && Smart_Home_Mode_Select_Flag ==3 && Music_Time_Flag == 1)
		{
			Music_Show();	
		}
	}	
}
