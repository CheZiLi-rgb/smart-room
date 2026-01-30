#ifndef __MOTOR_H
#define __MOTOR_H

void PWM1_Init(void);
void PWM2_Init(void);
void PWM1_SetCompare3(uint16_t Compare);
void PWM2_SetCompare4(uint16_t Compare);
void Motor1_Init(void);
void Motor2_Init(void);
void Motor1_SetSpeed(int8_t Speed);       //风扇电机需要调速
void Motor2_SetSpeed(int8_t Speed);       //窗帘电机恒速

#endif
