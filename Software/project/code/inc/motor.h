#ifndef _MOTOR_H
#define _MOTOR_H

#include "encoder.h"
#include "zf_common_headfile.h"
#include "PID.h"

// 锟斤拷锟轿伙拷锟斤拷锟酵硷拷锟绞�
//			 |
//	 1	 |   2
//----------------
//			 |
//	 3	 |   4


// 鐢垫満妗嗘灦
//
//     [MOTOR PID]
//     [Encoder][PWM]
//


/*
* 电机闭环控制选项 常规的闭环使用车辆环路就行
*	前两者用于调试使用
*
*/
#define MOTOR_THREAD 			0	//基于单独的线程
#define MOTOR_PIT    			1	//基于定时器中断
#define MOTOR_USE_CAR_LOOP	 	2	//基于车辆环路


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



//电机运行函数
void motor_run();
//电机初始化
void Motor_init();
//对外接口
void Motor_Set_Speed(uint8_t Motor_CH,float target_speed);

void Motor_switch();



//电机内部函数
void motor_pit_init();
void Motor_Pwm_cb();

void Motor1_Set_Pwm(int pwm);
void Motor2_Set_Pwm(int pwm);
void Motor3_Set_Pwm(int pwm);
void Motor4_Set_Pwm(int pwm);


#endif