#include "Attitude_algorithm.h"
#include "zf_device_icm20602.h""

/*********閸欐﹢鍣虹€规矮锟�??*********/
State Att;//閺堚偓缂佸牏娈戠憴鎺戝
State Att_A;//闁俺绻冮崝鐘烩偓鐔峰濞村鐣婚惃鍕潡鎼达拷
State Att_G;//闁俺绻冪憴鎺椻偓鐔峰濞村鐣婚惃鍕潡鎼达拷

float D_Gyro[3][3]={0};//鐟欐帡鈧喎瀹抽弮瀣祮閻晠妯€
Gyro_Offset GyroOffset;

float ICS_Gyro_x;
float ICS_Gyro_y;
float ICS_Gyro_z;//閹垱鈧冨棘閼板啰閮存稉瀣畱鐟欐帡鈧喎锟�??

int previous_t;//閸楁洑缍呭顔绢潡
int current_t;//閸楁洑缍呭顔绢潡
float dt;//閸楁洑缍呯粔锟�??

//閸椻€崇毜閺囧吋鎶ゅ▔锟�??
raw_data raw_imu_data;

KalmanInfo Kal;

void imu_data_convertion(int16 acc_x,int16 acc_y,int16 acc_z,int16 gyro_x,int16 gyro_y,int16 gyro_z)
{
	static float fliter_value[2];
	float a = 0.2f;

	icm20602_get_acc();
	icm20602_get_gyro();

	raw_imu_data.acc_data[0] = (float)acc_x*acc_range/IMU_RESOLUTION;
	raw_imu_data.acc_data[1] = (float)acc_y*acc_range/IMU_RESOLUTION;
	raw_imu_data.acc_data[2] = (float)acc_z*acc_range/IMU_RESOLUTION;

	raw_imu_data.gyr_data[0] = ((float)gyro_x - GyroOffset.x)*gyro_range/IMU_RESOLUTION*angle_to_rad;
	raw_imu_data.gyr_data[1] = ((float)gyro_y - GyroOffset.y)*gyro_range/IMU_RESOLUTION*angle_to_rad;
	raw_imu_data.gyr_data[2] = ((float)gyro_z - GyroOffset.z)*gyro_range/IMU_RESOLUTION*angle_to_rad;

//一阶低通滤波器
	fliter_value[1] = (float)((1.0f - a) * (float)fliter_value[0] + a*raw_imu_data.gyr_data[2]);
	fliter_value[0] = fliter_value[1];

	raw_imu_data.gyr_data[2] = fliter_value[1];
}

/*********閸戣姤鏆熺紓鏍у晸*********/

/*
*鐠侊紕鐣荤憴鎺椻偓鐔峰鏉烆剚宕查惌鈺呮█
*/
void Cancer_GetDGyro(float D[3][3])
{
	float tanp=tan(Att.pitch);
	float sinr=sin(Att.roll);
	float cosr=cos(Att.roll);
	float cosp=cos(Att.pitch);
	D[0][0]=1; D[0][1]=tanp*sinr; D[0][2]=tanp*cosr;
	D[1][0]=0; D[1][1]=cosr;      D[1][2]=-sinr;
	D[2][0]=0; D[2][1]=sinr/cosp; D[2][2]=cosr/cosp;

}


/*
*濞村鍣洪崗顒€绱￠敍锟�?? 
*roll=arctan(ay/az)  pitch=-arctan(ax/sqrt(ay^2+az^2))
*閺嶈宓侀崝鐘烩偓鐔峰鐠佲€崇繁閸掓媽顫囧ù瀣櫤
*/
void Cancer_GetState_Accel(State*att)
{
	imu_data_convertion(icm20602_gyro_x,icm20602_gyro_y,icm20602_gyro_z,icm20602_gyro_x,icm20602_gyro_y,icm20602_gyro_z);

	att->roll=atan2(raw_imu_data.acc_data[1],raw_imu_data.acc_data[2])*180/PI;//濞村鍣虹紙缁樼泊鐟欙拷

	//濞村鍣烘穱顖欒瘽鐟欙拷
	float x= raw_imu_data.acc_data[1] * raw_imu_data.acc_data[1] + raw_imu_data.acc_data[2]*raw_imu_data.acc_data[2];
	att->pitch=(-atan2(raw_imu_data.acc_data[0],sqrt(x)))*180/PI;
	
}


/*
*閸椻€崇毜閺囧吋鎶ゅ▔锟�??
*/

void Cancer_KalmanInit(KalmanInfo* Kal)
{
	Kal->Qk=0.0025;//濞村鍣洪弬鐟版▕ 
	Kal->Rk=0.3;//鐟欏倹绁撮弬鐟版▕ 
	Kal->H=1;
	Kal->P=1;
	Cancer_GetDGyro(D_Gyro);//閼惧嘲褰囩憴鎺戝閸欐ɑ宕查惌鈺呮█
	Cancer_GetState_Accel(&Att);//閼惧嘲褰囩挧宄邦潗閻樿埖鈧拷
}

void Cancer_Kalman_Algo(KalmanInfo* Kal)
{
	
	/******閻樿埖鈧椒鍙婄拋锟�??******/
	imu_data_convertion(icm20602_acc_x,icm20602_acc_y,icm20602_acc_z,icm20602_gyro_x,icm20602_gyro_y,icm20602_gyro_z);
	current_t=imu_get_tick;	//閼惧嘲褰囬弮鍫曟？
	dt=(float)(current_t-previous_t)/1000;
	previous_t=current_t;//閺囧瓨鏌婇弮鍫曟？
	
	//閼惧嘲褰囩憴鎺椻偓鐔峰閻ㄥ嫪鍙婄拋锛勭波閺嬶拷
	Att_G.pitch = Att.pitch+(raw_imu_data.gyr_data[0]+D_Gyro[0][1]*raw_imu_data.gyr_data[1]+D_Gyro[0][2]*raw_imu_data.gyr_data[2])*dt;
	Att_G.roll = Att.roll+(D_Gyro[1][1]* raw_imu_data.gyr_data[1] +D_Gyro[1][2]*raw_imu_data.gyr_data[2])*dt;

	if(fabs(raw_imu_data.gyr_data[2])>0.01f)
		Att.yaw += raw_imu_data.gyr_data[2]*dt/angle_to_rad;

	/******閺傜懓妯婃导鎷岊吀******/
	Kal->P+=Kal->Qk;
	
	/******Ka娴兼媽锟�??******/
	Kal->Ka=Kal->P/((Kal->P)+(Kal->Rk));
	
	/******娣囶喗顒滅紒鎾寸亯******/
	Cancer_GetState_Accel(&Att_A);//閼惧嘲褰囧ù瀣櫤閸婏拷
	Att.roll=Att.roll+Kal->Ka*(Att_A.roll-Att_G.roll);
	Att.pitch=Att.pitch+Kal->Ka*(Att_A.pitch-Att_G.pitch);


	
	/******閺囧瓨鏌婇弬鐟版▕******/
	Kal->P=(1-Kal->Ka)*Kal->P;
	
}

void Att_Algo_Init()
{
		IMU_INIT;
		
		GyroOffset.x = 0;
    GyroOffset.y = 0;
    GyroOffset.z = 0;
    for (uint16_t i = 0; i < 100; ++i) {
        icm20602_get_gyro();
        GyroOffset.x += icm20602_gyro_x;
        GyroOffset.y += icm20602_gyro_y;
        GyroOffset.z += icm20602_gyro_z;
        rt_thread_mdelay(10);
    }

    GyroOffset.x /= 100;
    GyroOffset.y /= 100;
    GyroOffset.z /= 100;
	
}

float Att_GetYaw()
{
	imu_data_convertion(icm20602_acc_x,icm20602_acc_y,icm20602_acc_z,icm20602_gyro_x,icm20602_gyro_y,icm20602_gyro_z);
	current_t=imu_get_tick;	//获取系统时间
	dt=(float)(current_t-previous_t) * IMU_UNIT_TIME;//计算时间差,计数器相差的值 * 计数器的单位时间
	previous_t=current_t;//更新时间

	if(fabs(raw_imu_data.gyr_data[2])>0.01f)//略去微小分量
		Att.yaw += raw_imu_data.gyr_data[2]*dt/angle_to_rad;//转化为弧度

//	Att.yaw = Att.yaw>360.0f? (Att.yaw - 360.0f) : Att.yaw;
//	Att.yaw = Att.yaw<0.0f? (Att.yaw + 360.0f) : Att.yaw;

	return Att.yaw;
}










