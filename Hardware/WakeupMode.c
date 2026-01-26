#include "stm32f10x.h"                  // Device header
#include "OLED.h"
#include "LED.h"
#include "Key.h"
#include "Motor.h"
#include "Control.h"
#include "Delay.h"

extern uint8_t i_Mode ;//用于统计Key_Mode按下的次数
extern uint8_t i_SET  ;//用于统计Key_SET按下的次数
extern uint8_t i_Plus ;//用于切换家居设备的工作状态
extern uint8_t KeyNum ;
extern uint8_t Smart_Home_Mode_Flag;
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
extern uint8_t Smart_Home_Mode_Select_Flag;

void WakeupMode(void)
{
	if( i_Mode == 5 && i_SET == 1 && Smart_Home_Mode_Select_Flag == 1 && KeyNum == 4 )	
    {
	    OLED_Clear();
		OLED_ShowString(1,1,"All Lights ON");
	    All_LED_ON();
	}	
	if( i_Mode == 5 && i_SET == 2 && Smart_Home_Mode_Select_Flag == 1 && KeyNum == 4 )	
    {
		OLED_Clear();
		OLED_ShowString(1,1,"Curtain ON");
        Curtain_Motor_Manual_ON_Unconditional();
	}	       
}
