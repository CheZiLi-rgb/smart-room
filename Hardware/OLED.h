#ifndef __OLED_H
#define __OLED_H
#include <stdint.h>

void OLED_Init(void);
void OLED_Clear(void);
void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char);
void OLED_ShowString(uint8_t Line, uint8_t Column, char *String);
void OLED_ShowNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
void OLED_ShowSignedNum(uint8_t Line, uint8_t Column, int32_t Number, uint8_t Length);
void OLED_ShowHexNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
void OLED_ShowBinNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
void OLED_ShowEnvir(void);
void OLED_ShowEnvironmentalThresholds1(void);
void OLED_ShowEnvironmentalThresholds2(void);
void OLED_ShowCommunicationParameters(void);
void OLED_ShowDevice(void);
void OLED_ShowDeviceStatus(void);
void OLED_ShowFamilymMode(void);
void OLED_ShowHomeMode(void);
void OLED_ShowSleepMode(void);
void OLED_ShowWakeupMode(void);
void OLED_ShowLED(void);
void OLED_ShowLEDStatus(void);
#endif
