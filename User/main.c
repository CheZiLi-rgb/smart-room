#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "DHT11.h"
#include "USART.h"
#include "MQ2.h"
#include "key.h"
#include "LED.h"
#include "AD.h"
#include "Beep.h"
#include "YH-LDR.h"
#include "HC-SR501.h"
#include "motor.h"
#include "Control.h"
#include "AlarmCircuit.h"
#include "WakeupMode.h"
#include "SleepMode.h"
#include "HomeMode.h"
#include "Music.h"
#include "LabVIEW.h"
#include "math.h"
#include "Servo.h"

uint8_t Temperature = 0;
//温度
uint8_t Humidity = 0;
//湿度
uint16_t Smoke = 0;
//烟雾浓度
uint16_t Lightintensity = 0;
//光照强度
uint8_t TemperatureHigh = 29;
//温度最高阈值
uint8_t TemperatureLow  = 8;
//温度最低阈值
uint8_t HumidityHigh = 75;
//湿度最高阈值
uint8_t HumidityLow = 35;
//湿度最低阈值
uint16_t SmokeHigh = 500;
//烟雾报警
uint16_t LightintensityHigh = 1000;
//光照最高阈值
uint16_t LightintensityLow = 100;
//光照最低阈值
uint8_t KeyNum = 0;
//按下键码值
uint8_t LED6_Breathing_Number= 0;
uint8_t Beep_Number = 0;
uint8_t HeatingRelay_Auto_Flag      = 0; 
uint8_t WettingRelay_Auto_Flag      = 0;
uint8_t Curtain_Motor_Auto_Flag     = 0;
uint8_t Fan_Motor_Auto_Flag         = 0;//0 - 关  1 - 开
uint8_t Smart_Home_Mode_Select_Flag = 0;
uint8_t Temperature_Decimals = 0;
uint8_t Humidity_Decimals    = 0;
int8_t Speed1 = 0;
//Motor1风扇电机转速
int8_t Speed2 = 0;
//Motor2窗帘电机恒定转速
uint8_t i_Mode = 0;//用于统计Key_Mode按下的次数
uint8_t i_SET  = 0;//用于统计Key_SET按下的次数
uint8_t i_Plus = 2;//用于切换家居设备的工作状态
uint8_t i_Plus_Enter          = 0;   //进入第一次按下确认键标志位
uint8_t LED6_Breathing_Flag   = 0;   //呼吸灯标志位
uint8_t Music_Time_Flag       = 0;   //蜂鸣器音乐标志位
uint8_t Beep_Time_Flag        = 0;
uint8_t Music_Time_Statistics = 0;
uint16_t Light_Value  = 0;
uint16_t Smoke_Value  = 0;

uint8_t LED1_Flag      = 0;
uint8_t LED2_Flag      = 0;
uint8_t LED3_Flag      = 0;
uint8_t LED4_Flag      = 0;
uint8_t i_Plus2        = 0;

float Smoke_Voltage_Value = 0;//MQ-2的电压输出
float RS    = 0;
float Angle = 0;
const uint16_t Tone[21] = {3817,3401,3030,2865,2551,2272,2024,   //低音 0 - 6
                           1912,1703,1517,1432,1275,1136,1012,   //中音 7 - 13
                            956, 851, 758, 715, 637, 568, 506};  //高音 14- 20 
const uint8_t Music_Tone[25] = {4,4,5,4,7,6,4,4,5,4,8,7,4,4,11,9,7,6,5,10,10,9,7,8,7};
const uint8_t Music_Time[25] = {2,2,4,4,4,8,2,2,4,4,4,8,2,2,4,4,4,4,4,6,2,4,4,4,8};	

#define RL 10
#define R0 26
#define Baud1 9600
#define Baud2 115200
#define StopBits 1

int main(void)               
{
	OLED_Init();
	USART1_Init(Baud1);
	USART3_Init(Baud2);
	DHT11_Init(); 
    LED_Init();	
	AD_Init();
    DHT11_Show();
	Delay_s(2);                                        //DHT11 上电后，要等待2秒以越过不稳定状态，在此期间不能发送任何指令
	HCSR_Init();
	Key_Init();
	Beep_Init();
	Motor1_Init();
	Motor2_Init();
	HeatingRelay_Init();
	WettingRelay_Init();
	Servo_Init();
	while(1)
	{
	       DHT11_Read_Data(&Temperature,&Humidity,&Temperature_Decimals,&Humidity_Decimals);             
		   //DHT11不断读取温度和湿度
		   Smoke_Value = Get_Adc_Average(ADC_Channel_0,10);  //PA0		
		   Light_Value = Get_Adc_Average(ADC_Channel_6,10);  //PA6	
		   MQ2_Get_Voltage_Value();
		   MQ2_Get_PPM_Value();
		   YH_LDR_Get_LUX_Value();
   		   KeyNum = Key_GetNum(); 
	       Key_Mode();
		   Key_SET();
		   Key_PLUS();
		   Key_MINUS();	
           Key_SetSpeed();
		   Temperature_High_Alarm();
		   Temperature_Low_Alarm();
		   Humidity_High_Alarm();
		   Humidity_Low_Alarm();
		   Lighting_High_Treatment();
		   Lighting_Low_Treatment();
		   Smoke_High_Alarm();
		   Human_Detection();                                     //人体检测
		   Alarm_Message();
           LabVIEW_Receive();	   	   
	} 
}	
