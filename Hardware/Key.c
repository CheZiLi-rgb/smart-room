#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "DHT11.h"
#include "AD.h"
#include "motor.h"
#include "Control.h"
#include "LED.h"
#include "WakeupMode.h"
#include "SleepMode.h"
#include "HomeMode.h"
#include "Music.h"

extern uint8_t i_Mode ;//用于统计Key_Mode按下的次数
extern uint8_t i_SET  ;//用于统计Key_SET按下的次数
extern uint8_t i_Plus ;//用于切换家居设备的工作状态
extern uint8_t i_Plus_Enter;   //进入第一次按下确认键标志位
extern uint8_t KeyNum;
//用于判断那个按键按下
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
extern uint8_t Music_Time_Flag;
extern uint8_t Speed1;
extern uint8_t Fan_Motor_Auto_Flag;
extern uint16_t Light_Voltage_Value;
extern uint8_t i_Plus2;

extern uint8_t LED1_Flag      ;
extern uint8_t LED2_Flag      ;
extern uint8_t LED3_Flag      ;
extern uint8_t LED4_Flag      ;

void Key_Init(void)
{	
	//PB0 MODE PB1 SET PB3 PLUS PB12 MINUS PA8 MOTERSPEED
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);//PB3 是JTDO脚要配置成普通引脚使用
	DBGMCU->CR = DBGMCU->CR & ~((uint32_t)1<<5); 
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1  | GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	GPIO_SetBits(GPIOB,GPIO_Pin_0);
	GPIO_SetBits(GPIOB,GPIO_Pin_1);
	GPIO_SetBits(GPIOB,GPIO_Pin_12);
	GPIO_SetBits(GPIOA,GPIO_Pin_8);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

uint8_t Key_GetNum(void)
{
	if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0) == 0)
	{
		Delay_ms(1);
		if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0) == 0)
		{
			Delay_ms(1);
		    i_Mode = i_Mode + 1;
		    i_Mode = i_Mode % 7;
		    KeyNum = 1;//MODE
		}
	}
	if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == 0)
	{
		Delay_ms(1);
		if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == 0)
		{
			Delay_ms(1);
			i_SET = i_SET + 1;
			if( i_Mode == 0 || i_Mode == 1 || i_Mode == 3 || i_Mode == 6 || ( i_Mode == 5 && Smart_Home_Mode_Select_Flag == 2) )
			i_SET = i_SET % 5;
			if( i_Mode == 2 || i_Mode == 4 || (i_Mode == 5 && Smart_Home_Mode_Select_Flag == 0)|| (i_Mode == 5 && Smart_Home_Mode_Select_Flag == 3))            
			i_SET = i_SET % 4;// i_Mode = 5  //Smart_Home_Mode_Select_Flag = 0    FamilyMode  3行                
		    if( i_Mode == 5 && Smart_Home_Mode_Select_Flag == 1)
			i_SET = i_SET % 3;
			KeyNum = 2;//SET                 //Smart_Home_Mode_Select_Flag = 1    WakeupMode  2行
		}                                    //Smart_Home_Mode_Select_Flag = 2    SleepMode   4行
	}                                        //Smart_Home_Mode_Select_Flag = 3    HomeMode    3行
	if (GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_3) == 0)
	{
	    Delay_ms(1);
		if (GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_3) == 0)
		{
			Delay_ms(1);
			if( i_Plus_Enter == 0 )
			{
				i_Plus = 0;
			    i_Plus_Enter = 1;
			}
			i_Plus  = i_Plus  + 1;
			i_Plus  = i_Plus  % 2;
			i_Plus2 = i_Plus2 + 1;
			i_Plus2 = i_Plus2 % 3;
			KeyNum = 3;
		}
	}	
	if (GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_12) == 0)
	{
	    Delay_ms(1);
		if (GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_12) == 0)
		{
		    Delay_ms(1);
		    KeyNum = 4;
		}
	}	
	if (GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_8) == 0)
	{
		Delay_ms(1);
		if (GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_8) == 0)
		{
		    Delay_ms(1);
		    KeyNum = 5;
		}
	}
	return KeyNum;
}

void Key_Mode()//MODE
{
	if ( (KeyNum == 1 && i_Mode == 0 ) || KeyNum == 0 )
	{
		OLED_Clear();
		OLED_ShowEnvir();
		Smart_Home_Mode_Select_Flag = 0;
		Music_Time_Flag = 0;
	}
    if (KeyNum == 1 && i_Mode == 1 )//显示温湿度阈值
	{
        OLED_Clear();
		OLED_ShowEnvironmentalThresholds1();
		Smart_Home_Mode_Select_Flag = 0;	
        Music_Time_Flag = 0;		
	}
    if (KeyNum == 1 && i_Mode == 2 )//显示烟雾和光照阈值
	{
		OLED_Clear();
		OLED_ShowEnvironmentalThresholds2();
		Smart_Home_Mode_Select_Flag = 0;	
		Music_Time_Flag = 0;
	}
    if (KeyNum == 1 && i_Mode == 3 )//显示各个家居设备工作状态
	{
		OLED_Clear();
		OLED_ShowDevice();
		OLED_ShowDeviceStatus();
		Smart_Home_Mode_Select_Flag = 0;
		Music_Time_Flag = 0;
	}
	if (KeyNum == 1 && i_Mode == 4 )//显示通信参数
	{
		OLED_Clear();
        OLED_ShowCommunicationParameters();
        Smart_Home_Mode_Select_Flag = 0;	
        Music_Time_Flag = 0;		
	}
	if (KeyNum == 1 && i_Mode == 5 && Smart_Home_Mode_Select_Flag == 0)//显示家居模式选择
	{
		OLED_Clear();
	    OLED_ShowFamilymMode();
		Music_Time_Flag = 0;
	}
	if (KeyNum == 1 && i_Mode == 6 )//显示通信参数
	{
		OLED_Clear();
        OLED_ShowLED();
		OLED_ShowLEDStatus();
        Smart_Home_Mode_Select_Flag = 0;	
        Music_Time_Flag = 0;		
	}
}

void Key_SET(void)
{
	//第一页
	if (i_Mode == 0 && i_SET == 0)
	{
        OLED_Clear();		
	    OLED_ShowEnvir();	
	}
    if (i_Mode == 0 && i_SET == 1) 
	{
		OLED_Clear();
        OLED_ShowEnvir();		
        OLED_ShowString(1,14,"<-");	
	}
	if (i_Mode == 0 && i_SET == 2)  
	{
		OLED_Clear();
	    OLED_ShowEnvir();
		OLED_ShowString(2,14,"<-");	
	}
	if (i_Mode == 0 && i_SET == 3)  
	{
		OLED_Clear();
	    OLED_ShowEnvir();
		OLED_ShowString(3,14,"<-");	
	}
	if (i_Mode == 0 && i_SET == 4)  
	{
		OLED_Clear();
	    OLED_ShowEnvir();
		OLED_ShowString(4,14,"<-");	
	}
	//第二页
	if (i_Mode == 1 && i_SET == 0)
	{
		OLED_Clear();
	    OLED_ShowEnvironmentalThresholds1();
	}
    if (i_Mode == 1 && i_SET == 1)  
	{
		OLED_Clear();
        OLED_ShowEnvironmentalThresholds1();
        OLED_ShowString(1,14,"<-");	
	}
	if (i_Mode == 1 && i_SET == 2)  
	{
		OLED_Clear();
	    OLED_ShowEnvironmentalThresholds1();
		OLED_ShowString(2,14,"<-");	
	}
	if (i_Mode == 1 && i_SET == 3)  
	{
		OLED_Clear();
	    OLED_ShowEnvironmentalThresholds1();
		OLED_ShowString(3,14,"<-");	
	}
	if (i_Mode == 1 && i_SET == 4)
	{
		OLED_Clear();
	    OLED_ShowEnvironmentalThresholds1();
		OLED_ShowString(4,14,"<-");	
	}
	//第三页
	if (i_Mode == 2 && i_SET == 0)
	{
		OLED_Clear();
	    OLED_ShowEnvironmentalThresholds2();
	}
    if (i_Mode == 2 && i_SET == 1)  
	{
		OLED_Clear();
        OLED_ShowEnvironmentalThresholds2();
        OLED_ShowString(1,14,"<-");	
	}
	if (i_Mode == 2 && i_SET == 2)  
	{
		OLED_Clear();
	    OLED_ShowEnvironmentalThresholds2();
		OLED_ShowString(2,14,"<-");	
	}
	if (i_Mode == 2 && i_SET == 3)  
	{
		OLED_Clear();
	    OLED_ShowEnvironmentalThresholds2();
		OLED_ShowString(3,14,"<-");	
	}
	//第四页
	if (i_Mode == 3 && i_SET == 0)
	{
	    OLED_Clear();
		OLED_ShowDevice();
		OLED_ShowDeviceStatus();
	}	
	if (i_Mode == 3 && i_SET == 1)
	{
		OLED_Clear();
		OLED_ShowDevice();
		OLED_ShowDeviceStatus();
		OLED_ShowString(1,14,"<-");		
	}
	if (i_Mode == 3 && i_SET == 2)
	{
		OLED_Clear();
		OLED_ShowDevice();
		OLED_ShowDeviceStatus();
		OLED_ShowString(2,14,"<-");	
	}
	if (i_Mode == 3 && i_SET == 3)
	{
		OLED_Clear();
		OLED_ShowDevice();
		OLED_ShowDeviceStatus();
		OLED_ShowString(3,14,"<-");		
	}	
	if (i_Mode == 3 && i_SET == 4)
	{
		OLED_Clear();
		OLED_ShowDevice();
		OLED_ShowDeviceStatus();
		OLED_ShowString(4,14,"<-");	
	}
	//第五页
	if (i_Mode == 4 && i_SET == 0 )
	{
		OLED_Clear();
	    OLED_ShowCommunicationParameters();
	}
    if (i_Mode == 4 && i_SET == 1 )  
	{
		OLED_Clear();
        OLED_ShowCommunicationParameters();
        OLED_ShowString(1,14,"<-");	
	}
	if (i_Mode == 4 && i_SET == 2 )  
	{
		OLED_Clear();
	    OLED_ShowCommunicationParameters();
		OLED_ShowString(2,14,"<-");	
	}
	if (i_Mode == 4 && i_SET == 3)  
	{
		OLED_Clear();
	    OLED_ShowCommunicationParameters();
		OLED_ShowString(3,14,"<-");	
	}
	//第六页
	if ( i_Mode == 5 && i_SET == 0 && Smart_Home_Mode_Select_Flag == 0 )
	{
		OLED_Clear();
	    OLED_ShowFamilymMode();
	}
	if ( i_Mode == 5 && i_SET == 1 && Smart_Home_Mode_Select_Flag == 0)
	{
		OLED_Clear();
	    OLED_ShowFamilymMode();
		OLED_ShowString(1,14,"<-");	
	}
	if ( i_Mode == 5 && i_SET == 2 && Smart_Home_Mode_Select_Flag == 0)  
	{
		OLED_Clear();
	    OLED_ShowFamilymMode();
		OLED_ShowString(2,14,"<-");	
	}
	if ( i_Mode == 5 && i_SET == 3 && Smart_Home_Mode_Select_Flag == 0)
	{
		OLED_Clear();
	    OLED_ShowFamilymMode();
		OLED_ShowString(3,14,"<-");	
	}
	if ( i_Mode == 5 && i_SET == 0 && Smart_Home_Mode_Select_Flag == 1)
	{
		OLED_Clear();
		OLED_ShowWakeupMode();
	}
	if ( i_Mode == 5 && i_SET == 1 && Smart_Home_Mode_Select_Flag == 1 && KeyNum != 4)
	{
		OLED_Clear();
		OLED_ShowWakeupMode();
		OLED_ShowString(1,14,"<-");
	}
	if ( i_Mode == 5 && i_SET == 2 && Smart_Home_Mode_Select_Flag == 1 && KeyNum != 4)
	{
		OLED_Clear();
		OLED_ShowWakeupMode();
		OLED_ShowString(2,14,"<-");
	}
	if ( i_Mode == 5 && i_SET == 0 && Smart_Home_Mode_Select_Flag == 2 && KeyNum != 4)
	{
		OLED_Clear();
		OLED_ShowSleepMode();
	}
	if ( i_Mode == 5 && i_SET == 1 && Smart_Home_Mode_Select_Flag == 2 && KeyNum != 4)
	{
		OLED_Clear();
		OLED_ShowSleepMode();
		OLED_ShowString(1,14,"<-");
	}
	if ( i_Mode == 5 && i_SET == 2 && Smart_Home_Mode_Select_Flag == 2 && KeyNum != 4)
	{
		OLED_Clear();
		OLED_ShowSleepMode();
		OLED_ShowString(2,14,"<-");
	}
	if ( i_Mode == 5 && i_SET == 3 && Smart_Home_Mode_Select_Flag == 2 && KeyNum != 4)
	{
		OLED_Clear();
		OLED_ShowSleepMode();
		OLED_ShowString(3,14,"<-");
	}
	if ( i_Mode == 5 && i_SET == 4 && Smart_Home_Mode_Select_Flag == 2 && KeyNum != 4)
	{
		OLED_Clear();
		OLED_ShowSleepMode();
		OLED_ShowString(4,14,"<-");
	}			
	if ( i_Mode == 5 && i_SET == 0 && Smart_Home_Mode_Select_Flag == 3 && KeyNum != 4)
	{
		OLED_Clear();
		OLED_ShowHomeMode();
	}
	if ( i_Mode == 5 && i_SET == 1 && Smart_Home_Mode_Select_Flag == 3 && KeyNum != 4)
	{
		OLED_Clear();
		OLED_ShowHomeMode();
		OLED_ShowString(1,14,"<-");
	}
	if ( i_Mode == 5 && i_SET == 2 && Smart_Home_Mode_Select_Flag == 3 && KeyNum != 4)
	{
		OLED_Clear();
		OLED_ShowHomeMode();
		OLED_ShowString(2,14,"<-");
	}
	if ( i_Mode == 5 && i_SET == 3 && Smart_Home_Mode_Select_Flag == 3 && KeyNum != 4)
	{
		OLED_Clear();
		OLED_ShowHomeMode();
		OLED_ShowString(3,14,"<-");
	}
	if ( i_Mode == 6 && i_SET == 1)
	{
		OLED_Clear();
		OLED_ShowLED();
		OLED_ShowLEDStatus();
		OLED_ShowString(1,14,"<-");		
	}
	if ( i_Mode == 6 && i_SET == 2)
	{
		OLED_Clear();
		OLED_ShowLED();
		OLED_ShowLEDStatus();
		OLED_ShowString(2,14,"<-");		
	}
	if ( i_Mode == 6 && i_SET == 3)
	{
		OLED_Clear();
		OLED_ShowLED();
		OLED_ShowLEDStatus();
		OLED_ShowString(3,14,"<-");		
	}
	if ( i_Mode == 6 && i_SET == 4)
	{
		OLED_Clear();
		OLED_ShowLED();
		OLED_ShowLEDStatus();
		OLED_ShowString(4,14,"<-");		
	}
}

void Key_PLUS(void)
{
	//第二页
    if (i_Mode == 1 && i_SET == 1 && KeyNum == 3 && (GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_3) == 0))  
	{
		OLED_Clear();
		TemperatureHigh = TemperatureHigh + 1;
		if(TemperatureHigh > 35)
		{
		   TemperatureHigh = 20;
		}
        OLED_ShowEnvironmentalThresholds1();
        OLED_ShowString(1,14,"<-");	
	}
	if (i_Mode == 1 && i_SET == 2 && KeyNum == 3 && (GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_3) == 0)  )  
	{
		OLED_Clear();
		TemperatureLow = TemperatureLow + 1;
		if( TemperatureLow > TemperatureHigh )
        {
		    TemperatureLow = 10;
		}			
	    OLED_ShowEnvironmentalThresholds1();
		OLED_ShowString(2,14,"<-");	
	}
	if (i_Mode == 1 && i_SET == 3 && KeyNum == 3 && (GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_3) == 0))  
	{
		OLED_Clear();
		HumidityHigh =HumidityHigh + 1;
		if (HumidityHigh > 99)
		{
		    HumidityHigh = 50;
		}
	    OLED_ShowEnvironmentalThresholds1();
		OLED_ShowString(3,14,"<-");	
	}
	if (i_Mode == 1 && i_SET == 4 && KeyNum == 3 && (GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_3) == 0))
	{
		OLED_Clear();
	    OLED_ShowEnvironmentalThresholds1();
		HumidityLow = HumidityLow + 1;
		if(HumidityLow > HumidityHigh)
		{
		   HumidityLow = 30;
		}
		OLED_ShowString(4,14,"<-");	
	}
	//第三页
    if (i_Mode == 2 && i_SET == 1 && KeyNum == 3 && (GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_3) == 0))  
	{
		OLED_Clear();
		SmokeHigh = SmokeHigh + 1;
		if (SmokeHigh > 1000)
		{
		    SmokeHigh = 500;
		}
        OLED_ShowEnvironmentalThresholds2();
        OLED_ShowString(1,14,"<-");	
	}
	if (i_Mode == 2 && i_SET == 2 && KeyNum == 3 && (GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_3) == 0))  
	{
		OLED_Clear();
		LightintensityHigh = LightintensityHigh + 1;
		if (LightintensityHigh > 200 )
		{
		    LightintensityHigh = 150;
		}
	    OLED_ShowEnvironmentalThresholds2();
		OLED_ShowString(2,14,"<-");	
	}
	if (i_Mode == 2 && i_SET == 3 && KeyNum == 3 && (GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_3) == 0))  
	{
		OLED_Clear();
		LightintensityLow = LightintensityLow + 1;
		if( LightintensityLow < 50 || LightintensityLow > LightintensityHigh)
		{
			LightintensityLow = 50;
		}
	    OLED_ShowEnvironmentalThresholds2();
		OLED_ShowString(3,14,"<-");	
	}
	//第四页
	if ( i_Mode == 3 && i_SET == 1 && KeyNum == 3 && (GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_3) == 0))
	{
	    OLED_Clear();	
		if( i_Plus == 0 )        // 0 代表关闭
		{
			OLED_ShowDevice();
			OLED_ShowDeviceStatus();
			OLED_ShowString(1,5,"   ");
			OLED_ShowString(1,5,"OFF");
            Fan_Motor_Manual_OFF();		    
		}
		else                     // 1 代表打开
        {
			OLED_ShowDevice();
			OLED_ShowDeviceStatus();
			OLED_ShowString(1,5,"   ");
			OLED_ShowString(1,5,"ON");	
		   	Fan_Motor_Manual_ON();		
		}			
		OLED_ShowString(1,14,"<-");
	}
	if ( i_Mode == 3 && i_SET == 2 && KeyNum == 3 && (GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_3) == 0))
	{
	    OLED_Clear();	
		if( i_Plus == 0 )        // 0 代表关闭
		{
			OLED_ShowDevice();
			OLED_ShowDeviceStatus();
			OLED_ShowString(2,9,"   ");
			OLED_ShowString(2,9,"OFF");	
            Curtain_Motor_Manual_OFF();			
		}
		else                     // 1 代表打开
        {
			OLED_ShowDevice();
			OLED_ShowDeviceStatus();
			OLED_ShowString(2,9,"   ");
			OLED_ShowString(2,9,"ON");	
		   	Curtain_Motor_Manual_ON();		
		}			
		OLED_ShowString(2,14,"<-");
	}
	if ( i_Mode == 3 && i_SET == 3 && KeyNum == 3 && (GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_3) == 0))
	{
	    OLED_Clear();	
		if( i_Plus == 0 )        // 0 代表关闭
		{
			OLED_ShowDevice();
			OLED_ShowDeviceStatus();
			OLED_ShowString(3,8,"   ");
			OLED_ShowString(3,8,"OFF");	
            HeatingRelay_Manual_OFF();			
		}
		else                     // 1 代表打开
        {
			OLED_ShowDevice();
			OLED_ShowDeviceStatus();
			OLED_ShowString(3,8,"   ");
			OLED_ShowString(3,8,"ON");	
		   	HeatingRelay_Manual_ON();		
		}			
		OLED_ShowString(3,14,"<-");
	}
	if ( i_Mode == 3 && i_SET == 4 && KeyNum == 3 && (GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_3) == 0))
	{
	    OLED_Clear();	
		if( i_Plus == 0 )        // 0 代表关闭
		{
			OLED_ShowDevice();
			OLED_ShowDeviceStatus();
			OLED_ShowString(4,9,"   ");
			OLED_ShowString(4,9,"OFF");	
            WettingRelay_Manual_OFF();			
		}
		else                     // 1 代表打开
        {
			OLED_ShowDevice();
			OLED_ShowDeviceStatus();
			OLED_ShowString(4,9,"   ");
			OLED_ShowString(4,9,"ON");	
		   	WettingRelay_Manual_ON();		
		}			
		OLED_ShowString(4,14,"<-");
	}
	//第六页
	if ( i_Mode == 5 && i_SET == 1 && KeyNum == 3 && ((GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_3) == 0)  || Smart_Home_Mode_Select_Flag != 0 ))   //确认在家模式
	{
		OLED_Clear();
		Smart_Home_Mode_Select_Flag = 1;
		OLED_ShowWakeupMode();
        OLED_ShowString(1,14,"<-");		
	}
	WakeupMode();	
	if ( i_Mode == 5 && i_SET == 2 && KeyNum == 3 && ((GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_3) == 0)   || Smart_Home_Mode_Select_Flag != 0)  ) //确认睡眠模式
	{
		OLED_Clear();
		Smart_Home_Mode_Select_Flag = 2;
        OLED_ShowSleepMode();	
		OLED_ShowString(2,14,"<-");			
	}
	SleepMode();
	if ( i_Mode == 5 && i_SET == 3 && KeyNum == 3 && ((GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_3) == 0)  || Smart_Home_Mode_Select_Flag != 0))   //确认起床模式
	{
		OLED_Clear();
		Smart_Home_Mode_Select_Flag = 3;
	    OLED_ShowHomeMode();	
	    OLED_ShowString(3,14,"<-");				
	}
	HomeMode();
	//第七页
	if ( i_Mode == 6 && i_SET == 1 && KeyNum == 3 && (GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_3) == 0))
	{
	    OLED_Clear();	
		if( i_Plus2 == 0 )                          // 0 代表关闭
		{
			OLED_ShowLED();
			OLED_ShowLEDStatus();
			OLED_ShowString(1,6,"    ");
			OLED_ShowString(1,6,"OFF");
            LED1_OFF(); 		    
		}
		else if( i_Plus2 == 1 )                     // 1 代表打开
        {
			OLED_ShowLED();
			OLED_ShowLEDStatus();
			OLED_ShowString(1,6,"    ");
			OLED_ShowString(1,6,"ON");	
		   	LED1_ON();		
		}	
        else
		{
		    OLED_ShowLED();
			OLED_ShowLEDStatus();
			OLED_ShowString(1,6,"    ");
			OLED_ShowString(1,6,"Turn");
            LED1_Turn();
            if(LED1_Flag == 2)	
			LED1_Turn();	
		}			
		OLED_ShowString(1,14,"<-");
	}
	if ( i_Mode == 6 && i_SET == 2 && KeyNum == 3 && (GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_3) == 0))
	{
	    OLED_Clear();	
		if( i_Plus2 == 0 )                          // 0 代表关闭
		{
			OLED_ShowLED();
			OLED_ShowLEDStatus();
			OLED_ShowString(2,6,"    ");
			OLED_ShowString(2,6,"OFF");
            LED2_OFF();  		    
		}
		else if( i_Plus2 == 1 )                     // 1 代表打开
        {
			OLED_ShowLED();
			OLED_ShowLEDStatus();
			OLED_ShowString(2,6,"    ");
			OLED_ShowString(2,6,"ON");	
		   	LED2_ON();			
		}	
        else
		{
		    OLED_ShowLED();
			OLED_ShowLEDStatus();
			OLED_ShowString(2,6,"    ");
			OLED_ShowString(2,6,"Turn");	
		   	LED2_Turn();
            if(LED2_Flag == 2)	
			LED2_Turn();			
		}			
		OLED_ShowString(2,14,"<-");
	}
	if ( i_Mode == 6 && i_SET == 3 && KeyNum == 3 && (GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_3) == 0))
	{
	    OLED_Clear();	
		if( i_Plus2 == 0 )                          // 0 代表关闭
		{
			OLED_ShowLED();
			OLED_ShowLEDStatus();
			OLED_ShowString(3,6,"    ");
			OLED_ShowString(3,6,"OFF");
            LED3_OFF();  		    
		}
		else if( i_Plus2 == 1 )                     // 1 代表打开
        {
			OLED_ShowLED();
			OLED_ShowLEDStatus();
			OLED_ShowString(3,6,"    ");
			OLED_ShowString(3,6,"ON");	
		   	LED3_ON();		
		}	
        else
		{
		    OLED_ShowLED();
			OLED_ShowLEDStatus();
			OLED_ShowString(3,6,"    ");
			OLED_ShowString(3,6,"Turn");	
		   	LED3_Turn();
            if(LED3_Flag == 2)	
			LED3_Turn();			
		}			
		OLED_ShowString(3,14,"<-");
	}
	if ( i_Mode == 6 && i_SET == 4 && KeyNum == 3 && (GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_3) == 0))
	{
	    OLED_Clear();	
		if( i_Plus2 == 0 )                          // 0 代表关闭
		{
			OLED_ShowLED();
			OLED_ShowLEDStatus();
			OLED_ShowString(4,6,"    ");
			OLED_ShowString(4,6,"OFF");
            LED4_OFF(); 
            		    
		}
		else if( i_Plus2 == 1 )                     // 1 代表打开
        {
			OLED_ShowLED();
			OLED_ShowLEDStatus();
			OLED_ShowString(1,6,"    ");
			OLED_ShowString(1,6,"ON");	
		   	LED4_ON();		
		}	
        else
		{
		    OLED_ShowLED();
			OLED_ShowLEDStatus();
			OLED_ShowString(1,6,"    ");
			OLED_ShowString(1,6,"Turn");			 
			LED4_Turn(); 
            if(LED4_Flag == 2)	
			LED4_Turn();			
		}			
		OLED_ShowString(4,14,"<-");
	}
}

void Key_MINUS(void)
{
    //第二页
    if (i_Mode == 1 && i_SET == 1 && KeyNum == 4 && (GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_12) == 0))  
	{
		OLED_Clear();
		TemperatureHigh = TemperatureHigh - 1;
		if( (TemperatureHigh < 15) || (TemperatureHigh < TemperatureLow))
        {
		    TemperatureHigh = 30;
		}		
        OLED_ShowEnvironmentalThresholds1();
        OLED_ShowString(1,14,"<-");	
	}
	if (i_Mode == 1 && i_SET == 2 && KeyNum == 4 && (GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_12) == 0))  
	{
		OLED_Clear();
		TemperatureLow = TemperatureLow - 1;
		if( TemperatureLow == 0)
		{
			TemperatureLow = 15;
		}
	    OLED_ShowEnvironmentalThresholds1();
		OLED_ShowString(2,14,"<-");	
	}
	if (i_Mode == 1 && i_SET == 3 && KeyNum == 4 && (GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_12) == 0))  
	{
		OLED_Clear();
		HumidityHigh =HumidityHigh - 1;
		if( HumidityHigh > 100 || HumidityHigh < HumidityLow)
		{
		    HumidityHigh = 85;	
		}
	    OLED_ShowEnvironmentalThresholds1();
		OLED_ShowString(3,14,"<-");	
	}
	if (i_Mode == 1 && i_SET == 4 && KeyNum == 4 && (GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_12) == 0))
	{
		OLED_Clear();
	    OLED_ShowEnvironmentalThresholds1();
		HumidityLow = HumidityLow - 1;
		if(HumidityLow < 30)
		{
		   HumidityLow = 40;		
		}
		OLED_ShowString(4,14,"<-");	
	}
	//第三页
    if (i_Mode == 2 && i_SET == 1 && KeyNum == 4 && (GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_12) == 0))  
	{
		OLED_Clear();
		SmokeHigh = SmokeHigh - 1;
		if(SmokeHigh < 400)
		{
		   SmokeHigh = 500;
		}
        OLED_ShowEnvironmentalThresholds2();
        OLED_ShowString(1,14,"<-");	
	}
	if (i_Mode == 2 && i_SET == 2 && KeyNum == 4 && (GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_12) == 0))  
	{
		OLED_Clear();
		LightintensityHigh = LightintensityHigh - 1;
		if(LightintensityHigh > 1200 || LightintensityHigh < LightintensityLow )
		{
		   LightintensityHigh = 900;
		}
	    OLED_ShowEnvironmentalThresholds2();
		OLED_ShowString(2,14,"<-");	
	}
	if (i_Mode == 2 && i_SET == 3 && KeyNum == 4 && (GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_12) == 0))  
	{
		OLED_Clear();
		LightintensityLow = LightintensityLow - 1;
		if(LightintensityLow < 80)
		{
		   LightintensityLow = 90;
		}
	    OLED_ShowEnvironmentalThresholds2();
		OLED_ShowString(3,14,"<-");	
	}
}

void Key_SetSpeed(void)
{
	if( i_Mode == 3 && i_SET == 1 && KeyNum == 5 && i_Plus == 1 && Fan_Motor_Auto_Flag != 0 && ( GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_8) == 0) )
	{			
		Speed1 = Speed1 +20;  //20 40 60 三档
		if (Speed1 > 60)
		{
			Speed1 = 20;
		}	
        Motor1_SetSpeed(Speed1);		
	}
}
