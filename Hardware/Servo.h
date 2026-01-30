#ifndef __SERVO_H
#define __SERVO_H

void Servo_Init(void);
void PWM3_SetCompare2(uint16_t Compare);
void Servo_SetAngle(float Angle);

#endif
