#include "stm32f10x.h"                  // Device header
#include "motor.h"
#include "Key.h"
#include "Delay.h"

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
extern uint8_t KeyNum;
extern uint8_t i_Mode;//用于统计Key_Mode按下的次数
extern uint8_t i_SET ;//用于统计Key_SET按下的次数
extern uint8_t i_Plus;//用于切换家居设备的工作状态
extern int8_t Speed1;
extern int8_t Speed2;
extern uint8_t HeatingRelay_Auto_Flag ; 
extern uint8_t WettingRelay_Auto_Flag ;
extern uint8_t Curtain_Motor_Auto_Flag;
extern uint8_t Fan_Motor_Auto_Flag    ;//0 - 关  1 - 开
extern uint16_t Light_Voltage_Value   ;

void HeatingRelay_Init(void) 
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    //PA12
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOA,GPIO_Pin_12);
}

void WettingRelay_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    //PA15
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOA,GPIO_Pin_15);
}

void HeatingRelay_Auto_ON(void)
{
	if (Temperature < TemperatureLow)
	{
		if (!(i_Mode == 3 && i_SET == 3 && KeyNum == 3 && HeatingRelay_Auto_Flag == 0 && i_Plus == 0) )
		{
	        GPIO_SetBits(GPIOA,GPIO_Pin_12);
            HeatingRelay_Auto_Flag = 1;		
		}
	}
}

void HeatingRelay_Auto_OFF(void)
{
	if( Temperature > TemperatureLow )
	{
		if(!(i_Mode == 3 && i_SET == 3 && KeyNum == 3 && HeatingRelay_Auto_Flag == 2 && i_Plus == 1))
		{
	        GPIO_ResetBits(GPIOA,GPIO_Pin_12);
		    HeatingRelay_Auto_Flag = 0;
		}
	}
}

void WettingRelay_Auto_ON(void)
{
	if( Humidity < HumidityLow )
	{
		if(!(i_Mode == 3 && i_SET == 4 && KeyNum == 3 && WettingRelay_Auto_Flag == 0 && i_Plus == 0))
        {
			GPIO_SetBits(GPIOA,GPIO_Pin_15);
		    WettingRelay_Auto_Flag = 1;
		}
	}
}

void WettingRelay_Auto_OFF(void)
{
	if( Humidity > HumidityLow ) 
	{
		if(!(i_Mode == 3 && i_SET == 4 && KeyNum == 3 && WettingRelay_Auto_Flag == 2 && i_Plus == 1))
		{
	        GPIO_ResetBits(GPIOA,GPIO_Pin_15);
		    WettingRelay_Auto_Flag = 0;
		}
	}
}

void Curtain_Motor_Auto_ON(void)//拉上窗帘
{
	if( Lightintensity > LightintensityHigh && Curtain_Motor_Auto_Flag == 0 )
	{
		if(!(i_Mode == 3 && i_SET == 2 && KeyNum == 3 && i_Plus == 0 && Curtain_Motor_Auto_Flag == 0))
		{
		    Delay_ms(500);
		    Speed2 = 10;
	        Motor2_SetSpeed(Speed2);		
		    Delay_s(1);
		    Speed2 = 0;
		    Motor2_SetSpeed(Speed2);
		    Delay_s(1);
		    Curtain_Motor_Auto_Flag = 1;
		}
	}
}

void Curtain_Motor_Auto_OFF(void)//拉开窗帘
{
	if( Lightintensity < LightintensityLow && Curtain_Motor_Auto_Flag != 0 )
	{//自动开始的时候 不能关
		if(!(i_Mode == 3 && i_SET == 2 && KeyNum == 3 && i_Plus == 1 && Curtain_Motor_Auto_Flag == 2))
		{		
		    Delay_ms(500);
		    Speed2 = 10;
	        Motor2_SetSpeed(-Speed2);
		    Delay_s(1);
		    Speed2 = 0;
		    Motor2_SetSpeed(Speed2);
		    Delay_s(1);
		    Curtain_Motor_Auto_Flag = 0;
		}
	}
}

void Fan_Motor_Auto_ON(void)
{
    if ( Temperature > TemperatureHigh || Humidity > HumidityHigh || Smoke > SmokeHigh  )    //风扇调速
	{//在手动模式下不受影响
		if(!( i_Mode == 3 && i_SET == 1 && KeyNum == 3 && Fan_Motor_Auto_Flag == 0 && i_Plus == 0))
		{
			Speed1 = 20;
		    Motor1_SetSpeed(Speed1);
		    Fan_Motor_Auto_Flag = 1;
		    if(Key_GetNum() == 5)
		    {
			    Speed1 = Speed1 +20;  //20 40 60 三档
		        if (Speed1 > 60)
		        {
			        Speed1 = 20;
		        }	
                Motor1_SetSpeed(Speed1);			
		    }
		}			
	}
}

void Fan_Motor_Auto_OFF(void)
{
	if (!((Temperature > TemperatureHigh) && (Humidity > HumidityHigh) && (Smoke > SmokeHigh) ))    
	{//在手动模式下不受影响
		if(!(i_Mode == 3 && i_SET == 1 && KeyNum == 3 && i_Plus == 1 && Fan_Motor_Auto_Flag == 2))
		{
		    Speed1 = 0;
		    Motor1_SetSpeed(Speed1);
		    Fan_Motor_Auto_Flag = 0;
		}
	}
}

void HeatingRelay_Manual_ON(void)
{
	if( i_Mode == 3 && i_SET == 3 && KeyNum == 3 && (GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_3) == 0) && HeatingRelay_Auto_Flag == 0 && i_Plus == 1)
	{
		GPIO_SetBits(GPIOA,GPIO_Pin_12);	
	    HeatingRelay_Auto_Flag = 2;
	}
}

void HeatingRelay_Manual_OFF(void)
{
	if( i_Mode == 3 && i_SET == 3 && KeyNum == 3 && (GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_3) == 0) && HeatingRelay_Auto_Flag != 0 && i_Plus == 0)
	{
		GPIO_ResetBits(GPIOA,GPIO_Pin_12);	
	    HeatingRelay_Auto_Flag = 0;
	}
}

void WettingRelay_Manual_ON(void)
{
	if( i_Mode == 3 && i_SET == 4 && KeyNum == 3 && (GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_3) == 0) && WettingRelay_Auto_Flag == 0 && i_Plus == 1)
	{
		GPIO_SetBits(GPIOA,GPIO_Pin_15);	
	    WettingRelay_Auto_Flag = 2;
	}
}	

void WettingRelay_Manual_OFF(void)
{
	if( i_Mode == 3 && i_SET == 4 && KeyNum == 3 && (GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_3) == 0) && WettingRelay_Auto_Flag != 0 && i_Plus == 0)
	{
		GPIO_ResetBits(GPIOA,GPIO_Pin_15);	
	    WettingRelay_Auto_Flag = 0;
	}
}

void Curtain_Motor_Manual_ON_Unconditional(void)
{
	Delay_ms(500);
	Speed2 = 10;
	Motor2_SetSpeed(Speed2);
    Delay_s(1);
	Speed2 = 0;
	Motor2_SetSpeed(Speed2);
	Delay_s(1);
	Curtain_Motor_Auto_Flag = 2; 
}

void Curtain_Motor_Manual_ON(void)//窗帘人工手动打开
{
	if( i_Mode == 3 && i_SET == 2 && KeyNum == 3 && i_Plus == 1 && Curtain_Motor_Auto_Flag == 0 )
	{
		Curtain_Motor_Manual_ON_Unconditional();
	}		
}

void Curtain_Motor_Manual_OFF_Unconditional(void)
{
    Delay_ms(500);
	Speed2 = 10;
	Motor2_SetSpeed(-Speed2);	
    Delay_s(1);
	Speed2 = 0;
	Motor2_SetSpeed(Speed2);
	Delay_s(1);
	Curtain_Motor_Auto_Flag = 0; //人工手动关闭
}

void Curtain_Motor_Manual_OFF(void)
{
	if( i_Mode == 3 && i_SET == 2 && KeyNum == 3 && i_Plus == 0 && Curtain_Motor_Auto_Flag != 0 )
    {
		Curtain_Motor_Manual_OFF_Unconditional();
	}		
}

void Fan_Motor_Manual_ON_Unconditional(void)
{
    Speed1 = 20;
	Motor1_SetSpeed(Speed1);		
	Fan_Motor_Auto_Flag = 2;
	if(Key_GetNum() == 5)
	{
		Speed1 = Speed1 +20;  //20 40 60 三档
		if (Speed1 > 60)
		{
			Speed1 = 20;
		}	
        Motor1_SetSpeed(Speed1);			
	}
}

void Fan_Motor_Manual_ON(void)
{
	if( i_Mode == 3 && i_SET == 1 && KeyNum == 3 && i_Plus == 1 && Fan_Motor_Auto_Flag == 0)
	{
		Fan_Motor_Manual_ON_Unconditional();
	}
}

void Fan_Motor_Manual_OFF_Unconditional(void)
{
	Speed1 = 0;
	Motor1_SetSpeed(Speed1);
	Fan_Motor_Auto_Flag = 0;
}

void Fan_Motor_Manual_OFF(void)
{
	if( i_Mode == 3 && i_SET == 1 && KeyNum == 3 && Fan_Motor_Auto_Flag != 0 && i_Plus == 0)
	{
		Fan_Motor_Manual_OFF_Unconditional();
	}		
}
