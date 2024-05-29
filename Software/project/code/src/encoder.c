#include "encoder.h"

int32 encoder1=0;
int32 encoder2=0;
int32 encoder3=0;
int32 encoder4=0;

float RC_encoder1,RC_encoder2,RC_encoder3,RC_encoder4;//滤波之后encoder的值

// 编码器位置如图所示
//			 |
//	 1	 |   2
//----------------
//			 |
//	 3	 |   4

//---------------------结构体---------------------//
struct RC_Para
{
    float temp;  //暂存值,存储RC_baro
    float value; //滤波值
    float RC;    //低通滤波参数
};
typedef struct RC_Para *RC_Filter_pt;

//-------------------------------------------------------------------------------------------------------------------
//  @brief      低通滤波
//  @param      待滤波的值
//  @return     滤波值
//  Sample usage:
//-------------------------------------------------------------------------------------------------------------------

float RCFilter(float value,RC_Filter_pt Filter)
{
    Filter->temp = value;
    Filter->value = (1 - Filter->RC) * Filter->value + Filter->RC * Filter->temp;
//	temp = RC * value + (1 - RC) * temp;
    return Filter->value;
}

struct RC_Para Encoder1_Para = {0,0,0.07};
struct RC_Para Encoder2_Para = {0,0,0.07};
struct RC_Para Encoder3_Para = {0,0,0.07};
struct RC_Para Encoder4_Para = {0,0,0.07};

RC_Filter_pt RC_Encoder1 = &Encoder1_Para;
RC_Filter_pt RC_Encoder2 = &Encoder2_Para;
RC_Filter_pt RC_Encoder3 = &Encoder3_Para;
RC_Filter_pt RC_Encoder4 = &Encoder4_Para;

//-------------------------------------------------------------------------------------------------------------------
//  @brief      初始化编码器
//  @param      void
//  @return     void
//  @e.g.       
//-------------------------------------------------------------------------------------------------------------------
void encoder_init()
{
  rt_kprintf("Encoder Init\n");
	qtimer_quad_init(QTIMER_1,QTIMER1_TIMER0_C0,QTIMER1_TIMER1_C1);
	qtimer_quad_init(QTIMER_2,QTIMER2_TIMER1_C4,QTIMER2_TIMER2_C5);
	qtimer_quad_init(QTIMER_3,QTIMER3_TIMER1_C7,QTIMER3_TIMER2_C8);
	qtimer_quad_init(QTIMER_4,QTIMER4_TIMER1_C10,QTIMER4_TIMER2_C11);
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      定时调用编码器
//  @param      void
//  @return     void
//  @e.g.       定时调用
//-------------------------------------------------------------------------------------------------------------------
void encoder_getvalue()
{
	encoder1 = qtimer_quad_get(QTIMER_1,QTIMER1_TIMER0_C0 );  //这里需要注意第二个参数务必填写A相引脚
	encoder2 = -qtimer_quad_get(QTIMER_2,QTIMER2_TIMER1_C4);   //这里需要注意第二个参数务必填写A相引脚
  encoder3 = qtimer_quad_get(QTIMER_3,QTIMER3_TIMER1_C7);  //这里需要注意第二个参数务必填写A相引脚
  encoder4 = -qtimer_quad_get(QTIMER_4,QTIMER4_TIMER1_C10 );//这里需要注意第二个参数务必填写A相引脚
	
	RC_encoder1 = RCFilter(encoder1,RC_Encoder1);
  RC_encoder2 = RCFilter(encoder2,RC_Encoder2);
  RC_encoder3 = RCFilter(encoder3,RC_Encoder3);
  RC_encoder4 = RCFilter(encoder4,RC_Encoder4);
	
	qtimer_quad_clear(QTIMER_1,QTIMER1_TIMER0_C0 );
	qtimer_quad_clear(QTIMER_2,QTIMER2_TIMER1_C4 );
	qtimer_quad_clear(QTIMER_3,QTIMER3_TIMER1_C7 );
	qtimer_quad_clear(QTIMER_4,QTIMER4_TIMER1_C10 );
	

	
}