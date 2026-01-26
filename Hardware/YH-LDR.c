#include "stm32f10x.h"                  // Device header
#include "math.h"

extern uint16_t Lightintensity;
extern uint16_t Light_Value   ;

void YH_LDR_Get_LUX_Value(void)
{
	Lightintensity = 39810 * pow(2.71828,-0.01534 * Light_Value) + 3467 * pow(2.71828,-0.002379 * Light_Value);
}
//  Lightintensity = 33180 * pow(2.71828,-0.01301 * Light_Value) + 2438 * pow(2.71828,-0.001922 * Light_Value);
