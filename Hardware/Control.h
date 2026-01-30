#ifndef __CONTROL_H
#define __CONTROL_H

void HeatingRelay_Init(void);  //控制加热继电器初始化
void WettingRelay_Init(void);  //控制加湿继电器初始化
void HeatingRelay_Auto_ON(void);
void HeatingRelay_Auto_OFF(void);
void WettingRelay_Auto_ON(void);
void WettingRelay_Auto_OFF(void);
void Curtain_Motor_Auto_ON(void);
void Curtain_Motor_Auto_OFF(void);
void Fan_Motor_Auto_ON(void);
void Fan_Motor_Auto_OFF(void);
void HeatingRelay_Manual_ON(void);
void HeatingRelay_Manual_OFF(void);
void WettingRelay_Manual_ON(void);
void WettingRelay_Manual_OFF(void);
void Curtain_Motor_Manual_ON_Unconditional(void);
void Curtain_Motor_Manual_ON(void);
void Curtain_Motor_Manual_OFF_Unconditional(void);
void Curtain_Motor_Manual_OFF(void);
void Fan_Motor_Manual_ON_Unconditional(void);
void Fan_Motor_Manual_ON(void);
void Fan_Motor_Manual_OFF_Unconditional(void);
void Fan_Motor_Manual_OFF(void);

#endif
