#include "circule_handle.h"

//圆环中心到图片对的距离
#define Circule_Distance1
#define Circule_Normal_xSpeed 100

static int cirucle_xspeed;

//信号量控制块
rt_sem_t circule_handle_sem;
//线程控制块
rt_thread_t circule_handle_thread;
//告诉定位线程返回circule_handle_flag线程
uint8_t circule_handle_flag = 0;
//寻边线 PID
Pos_PID_t circule_Trace_Con_Omega;
Pos_PID_t circule_Trace_Con_Vy;

void circule_trace_line();


void circule_handle_entry(){
    // static uint8_t begin_flag = 0;
    // static float init_yaw;
    // while(1){
    //     rt_sem_take(circule_handle_sem,RT_WAITING_FOREVER);
    //     rt_kprintf("task:get into the circule task\n");
    //     //初始处理
    //     if(!begin_flag){
    //         //得到初始角度
    //         init_yaw = Att_CurrentYaw;
    //         //转向
    //         if(Circule_LorR == LEFT_CIRCULE)
    //             Car_Rotate(90);
    //         else 
    //             Car_Rotate(-90);
    //         //给足时间使得车转到位
    //         rt_thread_delay(500);
    //         //定位跑
    //         Car_DistanceMotion(0,-20,0.4);

    //     }

    //     // rt_thread_delay(2000);
    //     //启动定位抓取线程
    //     rt_kprintf("task:start to catch things\n");
    //     circule_handle_flag = 1;
    //     rt_sem_release(locate_picture_sem);
    //     rt_sem_take(circule_handle_sem,RT_WAITING_FOREVER);
    //     rt_kprintf("task:return to the circule_handle thread\n");

    //     //抓取完毕 旋转180度，先放置最不好放置的东西
    //     Car_Rotate(180);
    //     rt_thread_delay(1000);
    //     Car_DistanceMotion(0,-30,0.8);
    //     //识别种类
    //     Art_Change_Mode(Art_NumLetter_Mode);
    //     rt_thread_delay(10);
    //     Class_Six_CirPut(Art_GetData());
    //     Step_Motor_Put();
    //     //转向并且回到圆环中心
    //     if(Circule_LorR == LEFT_CIRCULE){
    //         //左转
    //         Car_Rotate(90);
    //         //给足时间使得车转到位
    //         rt_thread_delay(500);
    //         Car_DistanceMotion(-30,0,0.8);
    //         // Car_DistanceMotion(30,-30,0.8);
    //     }
    //     else{
    //         //右转
    //         Car_Rotate(-90);
    //         //给足时间使得车转到位
    //         rt_thread_delay(500);
    //         Car_DistanceMotion(30,0,0.8);
    //         // Car_DistanceMotion(-30,-30,0.8);
    //     }
    //     //巡线放置
    //     while(1){
    //         circule_trace_line();
    //         //达到目标角度
    //         if(Tool_IsMultiple(Att_GetYaw() - init_yaw,60,1.0f)){
    //             Car_Change_Speed(0,0,0);
    //             //向前移动放置卡片
    //             Car_DistanceMotion(0,-30,0.8);
    //             //art 识别
    //             Art_Change_Mode(Art_NumLetter_Mode);
    //             rt_thread_delay(10);
    //             //分类
    //             int ret = Class_Six_CirPut(Art_GetData());
    //             //已经分类完毕
    //             if(ret == 2)
    //                 break;
    //             //还没有没有该类
    //             else if(ret == 1)
    //                 continue;
    //             //有该类，放下
    //             Step_Motor_Put();
    //         }
    //     }
    //     float Delta_Yaw = fabs(init_yaw - Att_GetYaw());
    //     //转到初始角度
    //     Car_Speed_ConRight = Con_By_AngleLoop;
    //     Car_Change_Yaw(init_yaw);
    //     //针对不同的角度定距移动
    //     if(Tool_IsFloatEqu(Delta_Yaw,60,1.0f)){
    //         if(Circule_LorR == LEFT_CIRCULE)
    //             Car_DistanceMotion(30,-30,1);
    //         else
    //             Car_DistanceMotion(-30,-30,1);
    //     }
    //     else if(Tool_IsFloatEqu(Delta_Yaw,120,1.0f)){
    //         if(Circule_LorR == LEFT_CIRCULE)
    //             Car_DistanceMotion(60,-40,3);
    //         else
    //             Car_DistanceMotion(-60,-40,3);
    //     }
    //     else if(Tool_IsFloatEqu(Delta_Yaw,180,1.0f)){
    //         if(Circule_LorR == LEFT_CIRCULE)
    //             Car_DistanceMotion(30,-50,2);
    //         else
    //             Car_DistanceMotion(-30,-50,2);
    //     }
    //     else if(Tool_IsFloatEqu(Delta_Yaw,0,1.0f)){
    //         if(Circule_LorR == LEFT_CIRCULE)
    //             Car_DistanceMotion(30,-10,1);
    //         else
    //             Car_DistanceMotion(-30,-10,1);
    //     }
        
    //     //返回巡线线程
    //     Car_Speed_ConRight = Con_By_TraceLine;
    //     rt_sem_release(trace_line_sem);

    //}
}

/**
 * @brief 圆环巡线
 * 
 */
void circule_trace_line(){
    //将速度控制权限交给圆环
    Car_Speed_ConRight = Con_By_Circule;
    //扫线
    int line[IMAGE_COL];
    Camera_CirculeFindLine(my_image,line);

    // //计算误差
    // float current_error;
    // for(int i = 60;i<128;i++)
    //     current_error += line[i] - 35;
    // current_error /= 68.0f;

    // //左边圆环速度应当向左，右边圆环速度应当向右
    // cirucle_xspeed = Circule_LorR? Circule_Normal_xSpeed : -Circule_Normal_xSpeed;

    // //PID闭环控制
    // Car_Change_Speed(cirucle_xspeed,
    //                 Pos_PID_Controller(&circule_Trace_Con_Vy,current_error),
    //                 Pos_PID_Controller(&circule_Trace_Con_Omega,current_error));

}

/**
 * @brief 圆环处理初始化函数
 * 
 */
void circule_handle_init(){

    rt_kprintf("circule handle task init\n");
	circule_handle_sem = rt_sem_create("circule_handle_sem",0,RT_IPC_FLAG_FIFO);
	if(circule_handle_sem == RT_NULL){
		rt_kprintf("circule_handle_sem created failed\n");
		while(1);
	}
	circule_handle_thread = rt_thread_create("side_catch",circule_handle_entry,RT_NULL,1024,3,1000);
	if(circule_handle_thread == RT_NULL){
        rt_kprintf("circule_handle_thread created failed\n");
        while(1);
    }

	rt_thread_startup(circule_handle_thread);

    Pos_PID_Init(&circule_Trace_Con_Omega,0,0,0);
    circule_Trace_Con_Omega.Output_Max = 50;
    circule_Trace_Con_Omega.Output_Min = -50;
    circule_Trace_Con_Omega.Value_I_Max = 200;
    circule_Trace_Con_Omega.Ref = 0;

    Pos_PID_Init(&circule_Trace_Con_Vy,0,0,0);
    circule_Trace_Con_Vy.Output_Max = 50;
    circule_Trace_Con_Vy.Output_Min = -50;
    circule_Trace_Con_Vy.Value_I_Max = 200;
    circule_Trace_Con_Vy.Ref = 0;

}