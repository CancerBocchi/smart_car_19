#ifndef __CAR_H__
#define __CAR_H__

/**
 * 	CAR 控制框架�? 
 * 	控制车的运动只需要调用此 h文件中的函数即可
 * 	最终运�?	       [ CAR MOTION ]
 * 	算法�?	    [ MOTOR PID ]    [ Att_Algo ]
 * 	传感�?		[ MOTOR ] [ encoder ] [ IMU ]
 * 	 外设		[ PWM ]   [ qtimer ]  [ spi ]
 */
 
#include "zf_common_headfile.h"

#define CAR_USE_THREAD 	0
#define CAR_USE_PIT		1

typedef struct{
	
	float Vx;
	float Vy;
	float Omega;

}mecanum_Speed;

typedef enum Omega_Right{
	
	Con_By_TraceLine,
	Con_By_AngleLoop,
	
}Speed_Con_Right;

extern mecanum_Speed Car_Speed;
extern Pos_PID_t Car_Yaw_Controller;
extern Speed_Con_Right Car_Speed_ConRight;
extern uint8_t Car_BootSwitch;

//�������أ�1Ϊ�� 0Ϊ�أ��Ҷ�ʱ�������ٶȿ��ƺ�����ʧЧ
#define Car_Switch(onoff)	(Car_BootSwitch = onoff)

//整体初始化，对外调用接口
void car_motion_Init();
// 车运动函数，调用此接口即�?
void car_motion_run();
// 改变车速度的函数，外部改变车的运行调用此接�?,当角度闭环时，最后一个参数失�?
void Car_Change_Speed(float xSpeed, float ySpeed, float aSpeed);
// 改变车的偏航角的函数
void Car_Change_Yaw(float Yaw_Ref);
// 使车目前旋转一个角度的函数
void Car_Rotate(float angle);
//��������
void Car_Start();
void Car_Stop();

#endif