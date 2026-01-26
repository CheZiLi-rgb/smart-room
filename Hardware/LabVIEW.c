#include "stm32f10x.h"                  // Device header
#include "USART.h"
#include "Delay.h"
#include "Motor.h"

extern uint8_t Temperature          ;
extern uint8_t Humidity             ;
extern uint8_t Temperature_Decimals ;
extern uint8_t Humidity_Decimals    ;
extern uint16_t Smoke               ;
extern uint16_t Lightintensity      ;
extern int8_t Speed1;
extern int8_t Speed2;
extern uint8_t HeatingRelay_Auto_Flag ; 
extern uint8_t WettingRelay_Auto_Flag ;
extern uint8_t Curtain_Motor_Auto_Flag;
extern uint8_t Fan_Motor_Auto_Flag    ;//0 - 关  1 - 开
extern uint16_t Light_Voltage_Value   ;

void LabVIEW_Receive(void)
{
	USART3_SendByte(0xFF);       //发送帧头
	//湿度整数位
	USART3_SendByte(Humidity / 10 );             
	USART3_SendByte(Humidity % 10 );
	//湿度小数位
	USART3_SendByte(Humidity_Decimals / 10 );    
	USART3_SendByte(Humidity_Decimals % 10 );
	USART3_SendByte(0x00);
	//温度整数位
	USART3_SendByte(Temperature / 10 );          
	USART3_SendByte(Temperature % 10 );
	//温度小数位
	USART3_SendByte(Temperature_Decimals / 10 ); 
	USART3_SendByte(Temperature_Decimals % 10 );
	USART3_SendByte(0x00);
	//烟雾浓度整数位
	USART3_SendByte(Smoke / 1000 );
	USART3_SendByte(Smoke / 100 % 10 );
	USART3_SendByte(Smoke / 10 % 10 );
	USART3_SendByte(Smoke % 10 );
	USART3_SendByte(0x00);
	//光照强度整数位
    USART3_SendByte(Lightintensity / 1000 );
	USART3_SendByte(Lightintensity / 100 % 10 );
	USART3_SendByte(Lightintensity / 10 % 10 );
	USART3_SendByte(Lightintensity % 10 );
    USART3_SendByte(0xEE);	
	Delay_s(2);
}

void LabVIEW_Control_System(void)
{
	uint8_t Res = 0;
    static char RecState = 0;
    
    if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  
    {
        Res = USART_ReceiveData(USART3);	   //读取接收到的数据
            
        if(RecState < 4)
        {
            if(Res == 0xAA)
			{
			    RecState = 1;
			}
			else if( RecState == 1 && Res == 0xBB )
			{
			    RecState = 2;
			}
			else if( RecState == 2 && Res == 0xCC )
			{
			    RecState = 3;
			}
			else if( RecState == 3 && Res == 0xDD )
			{
			    RecState = 4;
			}
            else
                RecState = 0;
        }
        else
        {
            if( Res == 0x00 )       //加热器打开
            {
                GPIO_SetBits(GPIOA,GPIO_Pin_12);
				HeatingRelay_Auto_Flag = 2;
            }
            else if( Res == 0x01 )  //加热器关闭
            {
                GPIO_ResetBits(GPIOA,GPIO_Pin_12);
				HeatingRelay_Auto_Flag = 0;
            }
            else if( Res == 0x02 )  //加湿器打开
			{
			    GPIO_SetBits(GPIOA,GPIO_Pin_15);
				WettingRelay_Auto_Flag = 2;
			}
			else if( Res == 0x03 )  //加湿器关闭
			{
			    GPIO_ResetBits(GPIOA,GPIO_Pin_15);
				WettingRelay_Auto_Flag = 0;
			}
			else if( Res == 0x04 )  //风扇打开
			{
			    Speed1 = 20;
		        Motor1_SetSpeed(Speed1);	
	            Fan_Motor_Auto_Flag = 2;
			}
			else if( Res == 0x05 )  //风扇关闭
			{
				Speed1 = 0;
	            Motor1_SetSpeed(Speed1);
	            Fan_Motor_Auto_Flag = 0;
			}
			else if( Res == 0x06 )  //窗帘打开
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
			else if( Res == 0x07 )  //窗帘关闭
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
            RecState = 0;
        }       
        USART_ClearITPendingBit(USART3, USART_IT_RXNE);  //清除接收中断标志
    }
}
