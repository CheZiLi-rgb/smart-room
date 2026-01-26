#ifndef __DHT11_H
#define __DHT11_H

void DHT11_IO_IN(void);                            //DHT11数据输入端口
void DHT11_IO_OUT(void);                            //DHT11输出初始化

void DHT11_Show(void);                                //DHT11初始化不成功/成功
uint8_t DHT11_Reset(void);                            //DHT11复位
uint8_t DHT11_Check(void);                            //检测DHT11是否存在
uint8_t DHT11_Read_Data(uint8_t *temp,uint8_t *humi,uint8_t *temp_decimals,uint8_t *humi_decimals); 
                                                      //DHT11读取温湿度
uint8_t DHT11_Init(void);                             //初始化DHT11
uint8_t DHT11_Read_Byte(void);                        //读出一个字节
uint8_t DHT11_Read_Bit(void);                         //读出一个位

#endif
