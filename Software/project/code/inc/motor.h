#ifndef _MOTOR_H
#define _MOTOR_H

#include "encoder.h"
#include "zf_common_headfile.h"
#include "PID.h"

// ���λ����ͼ��ʾ
//			 |
//	 1	 |   2
//----------------
//			 |
//	 3	 |   4


// 电机框架
//
//     [MOTOR PID]
//     [Encoder][PWM]
//


/*
* 使用定时器闭环或者定时器中断闭环仅仅在调试时候使用
* 上层线程中闭环包括麦轮解算
*
*/
#define MOTOR_THREAD 			0	//使用线程闭环
#define MOTOR_PIT    			1	//使用定时器中断闭环
#define MOTOR_USE_CAR_LOOP	 	2	//使用上层闭环


typedef struct Motor
{
	float Set_Speed;
	float Act_Speed;

}Motor_S;


extern Motor_S Motor_1;
extern Motor_S Motor_2;
extern Motor_S Motor_3;
extern Motor_S Motor_4;

extern Pos_PID_t Motor_PID_1;
extern Pos_PID_t Motor_PID_2;
extern Pos_PID_t Motor_PID_3;
extern Pos_PID_t Motor_PID_4;

#define PWM_CH1_A PWM1_MODULE3_CHA_D0
#define PWM_CH1_B PWM1_MODULE3_CHB_D1

#define PWM_CH2_A PWM2_MODULE3_CHA_D2
#define PWM_CH2_B PWM2_MODULE3_CHB_D3

#define PWM_CH3_A PWM1_MODULE1_CHA_D14
#define PWM_CH3_B PWM1_MODULE1_CHB_D15

#define PWM_CH4_A PWM1_MODULE2_CHA_D16
#define PWM_CH4_B PWM1_MODULE2_CHB_D17

/**
*	电机外部调用函数，在外部调用电机请使用以下功能
*
*/

//电机入口函数，thread以及定时器中断调用此函数
void motor_run();
//电机初始化函数
void Motor_init();
//电机调速函数
void Motor_Set_Speed(uint8_t Motor_CH,float target_speed);
//电机开关函数
void Motor_switch();



/*
*	电机内部调用函数，不要在外部调用！！！！！
* 
*/
void motor_pit_init();
void Motor_Pwm_cb();

void Motor1_Set_Pwm(int pwm);
void Motor2_Set_Pwm(int pwm);
void Motor3_Set_Pwm(int pwm);
void Motor4_Set_Pwm(int pwm);


#endif