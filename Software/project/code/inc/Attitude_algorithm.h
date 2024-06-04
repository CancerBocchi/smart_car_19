#ifndef __MPU6050_Attitude_algorithm__
#define  __MPU6050_Attitude_algorithm__

#include "math.h"
#include "zf_common_headfile.h"

#define K 0.4f
#define angle_to_rad 0.0174532925f

#ifndef PI
	#define PI 3.1415926f
#endif // !

#define acc_range  16		//g 加速度单位�? g 
#define gyro_range 4000		//degree ps 角速度单位为度每秒
#define IMU_RESOLUTION 65536 	//2^16 分辨�?

#define IMU_INIT icm20602_init()
#define IMU_UNIT_TIME 0.001 //单位为s eg 定义�? 0.001即定义为 1毫秒

#define Att_CurrentYaw 		(Att.yaw)

#define imu_get_tick rt_tick_get()

typedef struct gyro_offset{
	float x;
	float y;
	float z;

}Gyro_Offset;

typedef struct state 
{
    float roll;//横滚 x
    float yaw;//俯仰 y
    float pitch;//偏航 z
}State;

typedef struct 
{
	float P;//协方�??
	float Ka;//卡尔曼增�??
	float Qk;//估计噪声的方�??
	float Rk;//观测噪声的方�??
	float H;//测量矩阵
}KalmanInfo;

typedef struct raw_data
{
	float acc_data[3];
	float gyr_data[3];
}raw_data;

extern State Att;
extern KalmanInfo Kal;
extern raw_data raw_imu_data;

void Cancer_GetState_Accel(State*att);
void Cancer_KalmanInit(KalmanInfo* Kal);
void Cancer_Kalman_Algo(KalmanInfo* Kal);
//智能车只需要调用此函数即可
void Att_Algo_Init();
float Att_GetYaw();

#endif 