#ifndef __CAR_H__
#define __CAR_H__

/**
 * 	CAR ���ƿ��
 * 	���Ƴ����˶�ֻ��Ҫ���ô� h�ļ��еĺ�������
 * 	���ճ����˶�	       [ CAR MOTION ]
 * 	�㷨��	    [ MOTOR PID ]    [ Att_Algo ]
 * 	������		[ MOTOR ] [ encoder ] [ IMU ]
 * 	 ����		[ PWM ]   [ qtimer ]  [ spi ]
 */
 
#include "zf_common_headfile.h"

#define CAR_USE_THREAD 	0
#define CAR_USE_PIT		1
//��������ٶ�
#define Car_Max_Speed	300

typedef struct{
	
	float Vx;
	float Vy;
	float Omega;

}mecanum_Speed;

//�����ٶȿ���Ȩ��
typedef enum Speed_Right{

	Con_By_TraceLine,
	Con_By_AngleLoop,
	
}Speed_Con_Right;

extern mecanum_Speed Car_Speed;
extern Pos_PID_t Car_Yaw_Controller;
extern Speed_Con_Right Car_Speed_ConRight;
extern uint8_t Car_BootSwitch;

//�������أ�1Ϊ�� 0Ϊ�أ��Ҷ�ʱ�������ٶȿ��ƺ�����ʧЧ
#define Car_Switch(onoff)	(Car_BootSwitch = onoff)

//�����˶���ʼ��
void car_motion_Init();
// �����˶����к���
void car_motion_run();
// �ٶȿ��ƽӿ�
void Car_Change_Speed(float xSpeed, float ySpeed, float aSpeed);
// ����ת��ӿ�
void Car_Rotate(float angle);
// �����������
void Car_DistanceMotion(float dx,float dy,float dt);
// ����ǿ���
void Car_Change_Yaw(float Yaw_Ref);
//��������
void Car_Start();
void Car_Stop();

#endif