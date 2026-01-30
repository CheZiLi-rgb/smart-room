#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "sys.h"
#include "OLED.h"

#define	DHT11_DQ_OUT PAout(1) //数据端口	PA1
#define	DHT11_DQ_IN  PAin(1)  //数据端口

void DHT11_IO_IN(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitTypeDef  GPIO_InitStructure;		
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
 	GPIO_Init(GPIOA,&GPIO_InitStructure);
}

void DHT11_IO_OUT(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
}
//复位DHT11
void DHT11_Reset(void)	   
{                 
	DHT11_IO_OUT(); 	//SET OUTPUT
    DHT11_DQ_OUT=0; 	//拉低DQ
    Delay_ms(20);    	//拉低至少18ms
    DHT11_DQ_OUT=1; 	//DQ=1 
	Delay_us(30);     	//主机拉高20~40us
}
//等待DHT11的回应
//返回1:未检测到DHT11的存在
//返回0:存在
uint8_t DHT11_Check(void) 	   
{   
	uint8_t retry=0;
	DHT11_IO_IN();//SET INPUT	 
    while (DHT11_DQ_IN && retry<100)//DHT11会拉低40~80us
	{
		retry++;
		Delay_us(1);
	};	 
	if(retry>=100)return 1;
	else retry=0;
    while (!DHT11_DQ_IN&&retry<100)//DHT11拉低后会再次拉高40~80us
	{
		retry++;
		Delay_us(1);
	};
	if(retry>=100)return 1;	    
	return 0;
}
//从DHT11读取一个位
//返回值：1/0
uint8_t DHT11_Read_Bit(void) 			 
{
 	uint8_t retry=0;
	while(DHT11_DQ_IN&&retry<100)//等待变为低电平
	{
		retry++;
		Delay_us(1);
	}
	retry=0;
	while(!DHT11_DQ_IN&&retry<100)//等待变高电平
	{
		retry++;
		Delay_us(1);
	}
	Delay_us(40);//等待40us
	if(DHT11_DQ_IN)return 1;
	else return 0;		   
}
//从DHT11读取一个字节
//返回值：读到的数据
uint8_t DHT11_Read_Byte(void)    
{        
    uint8_t i,data;
    data=0;
	for (i = 0; i < 8 ;i ++) 
	{
   		data = data << 1; 
	    data = data | DHT11_Read_Bit();
    }						    
    return data;
}
//从DHT11读取一次数据
//temp:温度值(范围:0~50°)
//humi:湿度值(范围:20%~90%)
//返回值：0,正常;1,读取失败
uint8_t DHT11_Read_Data(uint8_t *temp,uint8_t *humi,uint8_t *temp_decimals,uint8_t *humi_decimals)    
{        
 	uint8_t buf[5];
	uint8_t i;
	DHT11_Reset();
	if(DHT11_Check()==0)
	{
		for(i=0;i<5;i++)//读取40位数据
		{
			buf[i]=DHT11_Read_Byte();
		}
		if((buf[0]+buf[1]+buf[2]+buf[3])==buf[4])
		{
			*humi=buf[0];
			*humi_decimals = buf[1];
			*temp=buf[2];
			*temp_decimals = buf[3];	
		}
	}else return 1;
	return 0;	    
}
//初始化DHT11的IO口 DQ 同时检测DHT11的存在
//返回1:不存在
//返回0:存在    	 
uint8_t DHT11_Init(void)
{	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
 	GPIO_InitTypeDef  GPIO_InitStructure; 	
	
 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOA,&GPIO_InitStructure);
	GPIO_SetBits(GPIOA,GPIO_Pin_1);
			    
	DHT11_Reset();  //复位DHT11
	return DHT11_Check();//等待DHT11的回应
} 

void DHT11_Show(void)
{
    while(DHT11_Init() == 0)														//DHT11初始化不成功
	{
		OLED_ShowString(1,1,"DHT11 ERROR!");
		Delay_ms(100);
	}
}
