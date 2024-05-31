#ifndef _MOTOR_H
#define _MOTOR_H

#include "encoder.h"
#include "zf_common_headfile.h"
#include "PID.h"

// ���λ����ͼ��ʄ1�7
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
* ����ջ�����ѡ�� ����ıջ�ʹ�ó�����·����
*	ǰ�������ڵ���ʹ��
*
*/
#define MOTOR_THREAD 			0	//���ڵ������߳�
#define MOTOR_PIT    			1	//���ڶ�ʱ���ж�
#define MOTOR_USE_CAR_LOOP	 	2	//���ڳ�����·


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



//������к���
void motor_run();
//�����ʼ��
void Motor_init();
//����ӿ�
void Motor_Set_Speed(uint8_t Motor_CH,float target_speed);

void Motor_switch();



//����ڲ�����
void motor_pit_init();
void Motor_Pwm_cb();

void Motor1_Set_Pwm(int pwm);
void Motor2_Set_Pwm(int pwm);
void Motor3_Set_Pwm(int pwm);
void Motor4_Set_Pwm(int pwm);


#endif