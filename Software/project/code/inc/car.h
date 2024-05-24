#ifndef __CAR_H__
#define __CAR_H__

/**
 * 	CAR 鎺у埗妗嗘灦鍥? 
 * 	鎺у埗杞︾殑杩愬姩鍙渶瑕佽皟鐢ㄦ h鏂囦欢涓殑鍑芥暟鍗冲彲
 * 	鏈€缁堣繍鍔?	       [ CAR MOTION ]
 * 	绠楁硶灞?	    [ MOTOR PID ]    [ Att_Algo ]
 * 	浼犳劅鍣?		[ MOTOR ] [ encoder ] [ IMU ]
 * 	 澶栬		[ PWM ]   [ qtimer ]  [ spi ]
 */
 
#include "zf_common_headfile.h"

#define CAR_USE_THREAD 	0
#define CAR_USE_PIT		1

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

//鏁翠綋鍒濆鍖栵紝瀵瑰璋冪敤鎺ュ彛
void car_motion_Init();
// 杞﹁繍鍔ㄥ嚱鏁帮紝璋冪敤姝ゆ帴鍙ｅ嵆鍙?
void car_motion_run();
// 速度控制接口
void Car_Change_Speed(float xSpeed, float ySpeed, float aSpeed);
// 鏀瑰彉杞︾殑鍋忚埅瑙掔殑鍑芥暟
void Car_Change_Yaw(float Yaw_Ref);
// 浣胯溅鐩墠鏃嬭浆涓€涓搴︾殑鍑芥暟
void Car_Rotate(float angle);
//车辆开关
void Car_Start();
void Car_Stop();

#endif