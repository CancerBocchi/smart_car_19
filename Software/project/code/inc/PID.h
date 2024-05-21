#ifndef __PID_H__
#define __PID_H__

#include "math.h"

typedef struct{
	float Kp;
	float Ki;
	float Kd;
	float Value_D;
	float Value_I;
	float Error;
	float Past_Error;
	float Output;
	float Ref;
	float Output_Max;
	float Output_Min;
	//积分限幅
	float Value_I_Max;
	//积分隔离
	float Value_I_Iso;
	
}Pos_PID_t;

typedef struct 
{
	/* data */
	float Kp;
	float Ki;
	float Kd;
	float Output;
	float Ref;
	float Output_Max;
	float Output_Min;
	float error_current;
	float error_past1;
	float error_past2;
}Step_PID_t;

void Pos_PID_Init(Pos_PID_t* plant,float Kp,float Ki,float Kd);
void Step_PID_Init(Step_PID_t* plant,float Kp,float Ki,float Kd);
float Pos_PID_Controller(Pos_PID_t* plant ,float ValueCurrent);
float Step_PID_Controller(Step_PID_t* plant,float ValueCurrent);

#endif