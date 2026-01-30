#include "stm32f10x.h"
#include "OLED_Font.h"
#include "Control.h"
/*引脚配置*/
#define OLED_W_SCL(x)		GPIO_WriteBit(GPIOB, GPIO_Pin_6, (BitAction)(x))
#define OLED_W_SDA(x)		GPIO_WriteBit(GPIOB, GPIO_Pin_7, (BitAction)(x))
#define Baud1 9600
#define Baud2 115200
#define StopBits 1

/*引脚初始化*/
void OLED_I2C_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
	OLED_W_SCL(1);
	OLED_W_SDA(1);
}

/**
  * @brief  I2C开始
  * @param  无
  * @retval 无
  */
void OLED_I2C_Start(void)
{
	OLED_W_SDA(1);
	OLED_W_SCL(1);
	OLED_W_SDA(0);
	OLED_W_SCL(0);
}

/**
  * @brief  I2C停止
  * @param  无
  * @retval 无
  */
void OLED_I2C_Stop(void)
{
	OLED_W_SDA(0);
	OLED_W_SCL(1);
	OLED_W_SDA(1);
}

/**
  * @brief  I2C发送一个字节
  * @param  Byte 要发送的一个字节
  * @retval 无
  */
void OLED_I2C_SendByte(uint8_t Byte)
{
	uint8_t i;
	for (i = 0; i < 8; i++)
	{
		OLED_W_SDA(Byte & (0x80 >> i));
		OLED_W_SCL(1);
		OLED_W_SCL(0);
	}
	OLED_W_SCL(1);	//额外的一个时钟，不处理应答信号
	OLED_W_SCL(0);
}

/**
  * @brief  OLED写命令
  * @param  Command 要写入的命令
  * @retval 无
  */
void OLED_WriteCommand(uint8_t Command)
{
	OLED_I2C_Start();
	OLED_I2C_SendByte(0x78);		//从机地址
	OLED_I2C_SendByte(0x00);		//写命令
	OLED_I2C_SendByte(Command); 
	OLED_I2C_Stop();
}

/**
  * @brief  OLED写数据
  * @param  Data 要写入的数据
  * @retval 无
  */
void OLED_WriteData(uint8_t Data)
{
	OLED_I2C_Start();
	OLED_I2C_SendByte(0x78);		//从机地址
	OLED_I2C_SendByte(0x40);		//写数据
	OLED_I2C_SendByte(Data);
	OLED_I2C_Stop();
}

/**
  * @brief  OLED设置光标位置
  * @param  Y 以左上角为原点，向下方向的坐标，范围：0~7
  * @param  X 以左上角为原点，向右方向的坐标，范围：0~127
  * @retval 无
  */
void OLED_SetCursor(uint8_t Y, uint8_t X)
{
	OLED_WriteCommand(0xB0 | Y);					//设置Y位置
	OLED_WriteCommand(0x10 | ((X & 0xF0) >> 4));	//设置X位置高4位
	OLED_WriteCommand(0x00 | (X & 0x0F));			//设置X位置低4位
}

/**
  * @brief  OLED清屏
  * @param  无
  * @retval 无
  */
void OLED_Clear(void)
{  
	uint8_t i, j;
	for (j = 0; j < 8; j++)
	{
		OLED_SetCursor(j, 0);
		for(i = 0; i < 128; i++)
		{
			OLED_WriteData(0x00);
		}
	}
}

/**
  * @brief  OLED显示一个字符
  * @param  Line 行位置，范围：1~4
  * @param  Column 列位置，范围：1~16
  * @param  Char 要显示的一个字符，范围：ASCII可见字符
  * @retval 无
  */
void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char)
{      	
	uint8_t i;
	OLED_SetCursor((Line - 1) * 2, (Column - 1) * 8);		//设置光标位置在上半部分
	for (i = 0; i < 8; i++)
	{
		OLED_WriteData(OLED_F8x16[Char - ' '][i]);			//显示上半部分内容
	}
	OLED_SetCursor((Line - 1) * 2 + 1, (Column - 1) * 8);	//设置光标位置在下半部分
	for (i = 0; i < 8; i++)
	{
		OLED_WriteData(OLED_F8x16[Char - ' '][i + 8]);		//显示下半部分内容
	}
}

/**
  * @brief  OLED显示字符串
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  String 要显示的字符串，范围：ASCII可见字符
  * @retval 无
  */
void OLED_ShowString(uint8_t Line, uint8_t Column, char *String)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i++)
	{
		OLED_ShowChar(Line, Column + i, String[i]);
	}
}

/**
  * @brief  OLED次方函数
  * @retval 返回值等于X的Y次方
  */
uint32_t OLED_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;
	while (Y--)
	{
		Result *= X;
	}
	return Result;
}

/**
  * @brief  OLED显示数字（十进制，正数）
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  Number 要显示的数字，范围：0~4294967295
  * @param  Length 要显示数字的长度，范围：1~10
  * @retval 无
  */
void OLED_ShowNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i++)							
	{
		OLED_ShowChar(Line, Column + i, Number / OLED_Pow(10, Length - i - 1) % 10 + '0');
	}
}

/**
  * @brief  OLED显示数字（十进制，带符号数）
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  Number 要显示的数字，范围：-2147483648~2147483647
  * @param  Length 要显示数字的长度，范围：1~10
  * @retval 无
  */
void OLED_ShowSignedNum(uint8_t Line, uint8_t Column, int32_t Number, uint8_t Length)
{
	uint8_t i;
	uint32_t Number1;
	if (Number >= 0)
	{
		OLED_ShowChar(Line, Column, '+');
		Number1 = Number;
	}
	else
	{
		OLED_ShowChar(Line, Column, '-');
		Number1 = -Number;
	}
	for (i = 0; i < Length; i++)							
	{
		OLED_ShowChar(Line, Column + i + 1, Number1 / OLED_Pow(10, Length - i - 1) % 10 + '0');
	}
}

/**
  * @brief  OLED显示数字（十六进制，正数）
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  Number 要显示的数字，范围：0~0xFFFFFFFF
  * @param  Length 要显示数字的长度，范围：1~8
  * @retval 无
  */
void OLED_ShowHexNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
	uint8_t i, SingleNumber;
	for (i = 0; i < Length; i++)							
	{
		SingleNumber = Number / OLED_Pow(16, Length - i - 1) % 16;
		if (SingleNumber < 10)
		{
			OLED_ShowChar(Line, Column + i, SingleNumber + '0');
		}
		else
		{
			OLED_ShowChar(Line, Column + i, SingleNumber - 10 + 'A');
		}
	}
}

/**
  * @brief  OLED显示数字（二进制，正数）
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  Number 要显示的数字，范围：0~1111 1111 1111 1111
  * @param  Length 要显示数字的长度，范围：1~16
  * @retval 无
  */
void OLED_ShowBinNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i++)							
	{
		OLED_ShowChar(Line, Column + i, Number / OLED_Pow(2, Length - i - 1) % 2 + '0');
	}
}

/**
  * @brief  OLED初始化
  * @param  无
  * @retval 无
  */
void OLED_Init(void)
{
	uint32_t i, j;
	
	for (i = 0; i < 1000; i++)			//上电延时
	{
		for (j = 0; j < 1000; j++);
	}
	
	OLED_I2C_Init();			//端口初始化
	
	OLED_WriteCommand(0xAE);	//关闭显示
	
	OLED_WriteCommand(0xD5);	//设置显示时钟分频比/振荡器频率
	OLED_WriteCommand(0x80);
	
	OLED_WriteCommand(0xA8);	//设置多路复用率
	OLED_WriteCommand(0x3F);
	
	OLED_WriteCommand(0xD3);	//设置显示偏移
	OLED_WriteCommand(0x00);
	
	OLED_WriteCommand(0x40);	//设置显示开始行
	
	OLED_WriteCommand(0xA1);	//设置左右方向，0xA1正常 0xA0左右反置
	
	OLED_WriteCommand(0xC8);	//设置上下方向，0xC8正常 0xC0上下反置

	OLED_WriteCommand(0xDA);	//设置COM引脚硬件配置
	OLED_WriteCommand(0x12);
	
	OLED_WriteCommand(0x81);	//设置对比度控制
	OLED_WriteCommand(0xCF);

	OLED_WriteCommand(0xD9);	//设置预充电周期
	OLED_WriteCommand(0xF1);

	OLED_WriteCommand(0xDB);	//设置VCOMH取消选择级别
	OLED_WriteCommand(0x30);

	OLED_WriteCommand(0xA4);	//设置整个显示打开/关闭

	OLED_WriteCommand(0xA6);	//设置正常/倒转显示

	OLED_WriteCommand(0x8D);	//设置充电泵
	OLED_WriteCommand(0x14);

	OLED_WriteCommand(0xAF);	//开启显示
		
	OLED_Clear();				//OLED清屏
}

extern uint8_t Temperature;
//温度
extern uint8_t Humidity;
//湿度
extern uint16_t Smoke;
//烟雾浓度
extern uint16_t Lightintensity;
//光强
extern uint8_t TemperatureHigh;
//ÎÂ¶È×î¸ßãÐÖµ
extern uint8_t TemperatureLow;
//ÎÂ¶È×îµÍãÐÖµ
extern uint8_t HumidityHigh;
//Êª¶È×î¸ßãÐÖµ
extern uint8_t HumidityLow;
//Êª¶È×îµÍãÐÖµ
extern uint16_t SmokeHigh;
//ÑÌÎí±¨¾¯
extern uint16_t LightintensityHigh;
//¹âÕÕ×î¸ßãÐÖµ
extern uint16_t LightintensityLow;
//¹âÕÕ×îµÍãÐÖµ
extern uint8_t i_Plus;
extern uint8_t HeatingRelay_Auto_Flag ; 
extern uint8_t WettingRelay_Auto_Flag ;
extern uint8_t Curtain_Motor_Auto_Flag;
extern uint8_t Fan_Motor_Auto_Flag    ;//0 - 关  1 - 开
extern int8_t Speed1;
extern int8_t Speed2;
extern uint8_t Temperature_Decimals;
extern uint8_t Humidity_Decimals;
extern uint8_t KeyNum;
extern uint16_t Light_Voltage_Value   ;
extern uint8_t LED1_Flag      ;
extern uint8_t LED2_Flag      ;
extern uint8_t LED3_Flag      ;
extern uint8_t LED4_Flag      ;

void OLED_ShowEnvir(void)
{
    OLED_ShowString(1,1,"Temp:");
    OLED_ShowString(2,1,"Humi:");
	OLED_ShowString(3,1,"Smoke:");
    OLED_ShowString(4,1,"Light:");
					
	OLED_ShowNum(1, 6, Temperature, 2);
	OLED_ShowNum(2, 6, Humidity, 2);
	OLED_ShowString(1,8,".");
	OLED_ShowString(2,8,".");
	OLED_ShowNum(1,9,Temperature_Decimals,2);
	OLED_ShowNum(2,9,Humidity_Decimals,2);
	OLED_ShowNum(3, 7, Smoke,4);
	OLED_ShowNum(4, 7, Lightintensity, 4);
	OLED_ShowString(1,11,"C");
	OLED_ShowString(2,11,"%");
	OLED_ShowString(3,11,"ppm");
	OLED_ShowString(4,11,"LUX");
}

void OLED_ShowEnvironmentalThresholds1(void)
{   
	OLED_ShowString(1,1,"TempH:");
	OLED_ShowString(2,1,"TempL:");
    OLED_ShowString(3,1,"HumiH:");
	OLED_ShowString(4,1,"HumiL:");
	        
	OLED_ShowNum(1,7,TemperatureHigh,2);
	OLED_ShowNum(2,7,TemperatureLow,2);
	OLED_ShowNum(3,7,HumidityHigh,2);
	OLED_ShowNum(4,7,HumidityLow,2);
	OLED_ShowString(1,10,"C");
	OLED_ShowString(2,10,"C");
	OLED_ShowString(3,10,"%");
	OLED_ShowString(4,10,"%");
}

void OLED_ShowEnvironmentalThresholds2(void)
{
	OLED_ShowString(1,1,"SmokeH:");
    OLED_ShowString(2,1,"LightH:");
	OLED_ShowString(3,1,"LightL:");  
    OLED_ShowString(4,1,"Speed1:");    
	OLED_ShowNum(1,8,SmokeHigh,4);
	OLED_ShowNum(2,8,LightintensityHigh,4);
	OLED_ShowNum(3,8,LightintensityLow,4);
	OLED_ShowNum(4,8,Speed1,3);
}

void OLED_ShowDevice(void)
{    
	OLED_ShowString(1,1,"Fan:");
	OLED_ShowString(2,1,"Curtain:");
	OLED_ShowString(3,1,"Heater:");
	OLED_ShowString(4,1,"Humi-er:");
}

void OLED_ShowDeviceStatus(void)
{
	//先初次判断设备的工作状态
	if( Fan_Motor_Auto_Flag == 0 )
	{
	    OLED_ShowString(1,5,"OFF");
	}
	else
	{
	    OLED_ShowString(1,5,"ON");
	}	
    if( Curtain_Motor_Auto_Flag == 0 )
	{
	    OLED_ShowString(2,9,"OFF");
	}
    else
	{
	    OLED_ShowString(2,9,"ON");
	}
	if( HeatingRelay_Auto_Flag == 0 )
	{
	    OLED_ShowString(3,8,"OFF");
	}
	else
	{
	    OLED_ShowString(3,8,"ON");
	}
	if( WettingRelay_Auto_Flag == 0 )
	{
	    OLED_ShowString(4,9,"OFF");
	}
	else
	{
	    OLED_ShowString(4,9,"ON");
	}
}

void OLED_ShowCommunicationParameters(void)
{
	OLED_ShowString(1,1,"Baud1:");
	OLED_ShowString(2,1,"Baud2:");
	OLED_ShowString(3,1,"StopBits:");
	OLED_ShowNum(1,7,Baud1,6);
	OLED_ShowNum(2,7,Baud2,6);
	OLED_ShowNum(3,10,StopBits,1);
}

void OLED_ShowFamilymMode(void)
{
	OLED_ShowString(1,1,"WakeupMode");
	OLED_ShowString(2,1,"SleepMode");
	OLED_ShowString(3,1,"HomeMode");
}

void OLED_ShowHomeMode(void)
{
	OLED_ShowString(1,1,"Breath-Light");
	OLED_ShowString(2,1,"Water-Light");
	OLED_ShowString(3,1,"Music-Play");
}

void OLED_ShowSleepMode(void)
{
	OLED_ShowString(1,1,"Close Lights");
	OLED_ShowString(2,1,"Open Fan");
	OLED_ShowString(3,1,"Open Door");
	OLED_ShowString(4,1,"Close Door");
}

void OLED_ShowWakeupMode(void)
{
	OLED_ShowString(1,1,"Open Lights");
	OLED_ShowString(2,1,"Open Curtain");	
}

void OLED_ShowLED(void)
{    
	OLED_ShowString(1,1,"LED1:");
	OLED_ShowString(2,1,"LED2:");
	OLED_ShowString(3,1,"LED3:");
	OLED_ShowString(4,1,"LED4:");
}

void OLED_ShowLEDStatus(void)
{
	//先初次判断LED的工作状态
	if( LED1_Flag == 0 )
	{
	    OLED_ShowString(1,6,"OFF");
	}
	else if (LED1_Flag == 1)
	{
	    OLED_ShowString(1,6,"ON");
	}
	else
	{
	    OLED_ShowString(1,6,"Turn");
	}	
    if( LED2_Flag == 0 )
	{
	    OLED_ShowString(2,6,"OFF");
	}
	else if (LED2_Flag == 1)
	{
	    OLED_ShowString(2,6,"ON");
	}
	else
	{
	    OLED_ShowString(2,6,"Turn");
	}	
	if( LED3_Flag == 0 )
	{
	    OLED_ShowString(3,6,"OFF");
	}
	else if (LED3_Flag == 1)
	{
	    OLED_ShowString(3,6,"ON");
	}
	else
	{
	    OLED_ShowString(3,6,"Turn");
	}	
	if( LED4_Flag == 0 )
	{
	    OLED_ShowString(4,6,"OFF");
	}
	else if (LED4_Flag == 1)
	{
	    OLED_ShowString(4,6,"ON");
	}
	else
	{
	    OLED_ShowString(4,6,"Turn");
	}	
}
