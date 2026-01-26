#include "stm32f10x.h"                  // Device header
#include "math.h"
#include "AD.h"
#include "Delay.h"

#define RL 10
#define R0 26

extern uint16_t Smoke_Value     ;
extern float Smoke_Voltage_Value;//MQ-2的电压输出
extern uint16_t Smoke           ;
extern float RS                 ;
//读取MQ-2传感器的电压值
void MQ2_Get_Voltage_Value(void)
{
	Smoke_Voltage_Value = (3.3 / 4096) * (Smoke_Value);
}
//实际的PPM值
void MQ2_Get_PPM_Value(void)
{
    RS     =  (3.3f - Smoke_Voltage_Value) / Smoke_Voltage_Value * RL;
	Smoke  =  98.322f * pow(RS/R0, -1.458f);
}
