#ifndef __CAR_H__
#define __CAR_H__

/**
 * 	CAR 控制框架
 * 	控制车的运动只需要调用此 h文件中的函数即可
 * 	最终车辆运动	       [ CAR MOTION ]
 * 	算法层	    [ MOTOR PID ]    [ Att_Algo ]
 * 	传感器		[ MOTOR ] [ encoder ] [ IMU ]
 * 	 外设		[ PWM ]   [ qtimer ]  [ spi ]
 */
 
#include "zf_common_headfile.h"

#define CAR_USE_THREAD 	0
#define CAR_USE_PIT		1
//车辆最大速度
#define Car_Max_Speed	300

typedef struct{
	
	float Vx;
	float Vy;
	float Omega;

}mecanum_Speed;

//车辆速度控制权限
typedef enum Speed_Right{

	Con_By_TraceLine,
	Con_By_AngleLoop,
	
}Speed_Con_Right;

extern mecanum_Speed Car_Speed;
extern Pos_PID_t Car_Yaw_Controller;
extern Speed_Con_Right Car_Speed_ConRight;
extern uint8_t Car_BootSwitch;

//车辆开关，1为开 0为关，挂断时，车的速度控制函数将失效
#define Car_Switch(onoff)	(Car_BootSwitch = onoff)

//车辆运动初始化
void car_motion_Init();
// 车辆运动运行函数
void car_motion_run();
// 速度控制接口
void Car_Change_Speed(float xSpeed, float ySpeed, float aSpeed);
// 汽车转向接口
void Car_Rotate(float angle);
// 车辆定距控制
void Car_DistanceMotion(float dx,float dy,float dt);
// 航向角控制
void Car_Change_Yaw(float Yaw_Ref);
//车辆开关
void Car_Start();
void Car_Stop();

#endif