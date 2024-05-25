#ifndef ___STEP_MOTOR_H__
#define ___STEP_MOTOR_H__

#include "zf_common_headfile.h"
//�������� ����Ϊ c6 c9 b10 b11 d12 d13
//�����Ƕȣ�up10 down5
//���ýǶ� down����

#define DOWN_MOTOR_CON_PIN 					PWM1_MODULE0_CHA_D12
#define UP_MOTOR_CON_PIN 						PWM2_MODULE1_CHB_C9

#define STEP_MOTOR_FRE     					(50)

#define SERVO_MOTOR_DUTY(x)         ((float)PWM_DUTY_MAX/(1000.0/(float)STEP_MOTOR_FRE)*(0.5+(float)(x)/90.0))

#define DOWN_MOTOR_INIT_ANGLE					(140)
#define UP_MOTOR_INIT_ANGLE						(100)

void servo_slow_ctrl(uint16 _servo1_angle, uint16 _servo2_angle, float _step_count);
void Step_Motor_Init();
void Step_Motor_Reset();
void Step_Motor_Catch();
void Step_Motor_Put();


#endif