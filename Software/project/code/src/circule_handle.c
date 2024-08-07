#include "circule_handle.h"

//圆环中心到图片对的距离
#define Circule_Distance1
#define Circule_Normal_xSpeed 50

float cirucle_xspeed;

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
    static uint8_t begin_flag = 0;
    static float init_yaw;
    static uint8_t error_flag = 0;
    while(1){
        rt_sem_take(circule_handle_sem,RT_WAITING_FOREVER);
        rt_kprintf("task:get into the circule task\n");
        circule_handle_flag = 1;
        //初始处理
        if(!begin_flag){
            //得到初始角度
            init_yaw = Att_CurrentYaw;
            rt_kprintf("init_yaw is %.2f\n",init_yaw);
            //转向
            if(Circule_LorR == LEFT_CIRCULE)
                Car_Rotate(90);
            else 
                Car_Rotate(-90);
            //给足时间使得车转到位
            rt_thread_delay(500);
            //定位跑
            Car_DistanceMotion(0,-10,0.4);
            MCX_Change_Mode(MCX_Location_Mode);
            MCX_Clear();
            
            rt_thread_delay(100);
            while(!MCX_rx_flag);

            rt_kprintf("x:%d,y:%d,n:%d\n",center_x,center_y,cur_PicNum);

            if(cur_PicNum){
                error_flag = 0;
                rt_kprintf("cir_handle:Start to catch\n");
                rt_sem_release(locate_picture_sem);
                rt_sem_take(circule_handle_sem,RT_WAITING_FOREVER);
            }

            else{
                Car_DistanceMotion(0,10,0.4);
                if(Circule_LorR == LEFT_CIRCULE)
                    Car_Rotate(-90);
                else 
                    Car_Rotate(90);
                rt_thread_delay(500);
                Car_DistanceMotion(0,-50,0.4);
                error_flag = 1;
            }

        }

        if(!error_flag){
                        //抓取完毕 旋转180度，先放置最不好放置的东西
            Car_Rotate(180);
            rt_thread_delay(700);
            Car_DistanceMotion(0,-20,0.5);
            
            put_flag = 1;
            rt_sem_release(locate_picture_sem);
            rt_sem_take(circule_handle_sem,RT_WAITING_FOREVER);
            
    

            //转向并且回到圆环中心
            if(Circule_LorR == LEFT_CIRCULE){
                //右转
                Car_Rotate(-90);
                //给足时间使得车转到位
                rt_thread_delay(500);
                Car_DistanceMotion(65,-25,1.0);
                rt_thread_delay(500);
                // Car_DistanceMotion(30,-30,0.8);
            }
            else{
                //右转
                Car_Rotate(90);
                //给足时间使得车转到位
                rt_thread_delay(500);
                Car_DistanceMotion(-65,-25,1.0);
                rt_thread_delay(500);
                // Car_DistanceMotion(-30,-30,0.8);
            }
            //巡线放置
            int cur_yaw = Att_CurrentYaw;
            int tar_angle = 0;
            while(1){
                if(mt9v03x_finish_flag)
                    circule_trace_line();
                rt_thread_delay(1);
        

                if(Tool_IsFloatEqu(fabs(Att_CurrentYaw - cur_yaw),tar_angle,1.0f)){
                    Car_Change_Speed(0,0,0);
                    rt_thread_delay(100);
                    Car_DistanceMotion(0,-20,0.4);
                    put_flag = 1;
                    rt_sem_release(locate_picture_sem);
                    rt_sem_take(circule_handle_sem,RT_WAITING_FOREVER);
                    Car_DistanceMotion(0,35,0.6);
                    if(tar_angle  == 180){
                        rt_kprintf("cir:return to the init yaw\n");
                        break;
                    }
                    tar_angle+=60;
                }



            }
            Car_Change_Speed(0,0,0);
            
            
            if(Circule_LorR == LEFT_CIRCULE)
                Car_DistanceMotion(70,-70,1.8);
            else if(Circule_LorR == RIGHT_CIRCULE)
                Car_DistanceMotion(-70,-70,1.8);
        }


        //while(1);
        //返回巡线线程
        error_flag = 0;
        Class_Cir_Reset();
        MCX_Clear();
        Car_Speed_ConRight = Con_By_TraceLine;
        circule_handle_flag = 0;
        rt_sem_release(trace_line_sem);

    }
}

/**
 * @brief 圆环巡线
 * 
 */
void circule_trace_line(){

    Camera_CopyMyImage();
    Camera_CirFindLine(my_image);
    Vision_Draw();
    for(int i = 0;i<imgCol;i++)
        ips200_draw_point(i,cir_line[i],RGB565_RED);

    //将速度控制权限交给圆环
    Car_Speed_ConRight = Con_By_Circule;

    // //计算误差
    float current_error = 0;
    float left_right_error = 0;
    for(int i = 60;i<128;i++)
        current_error += cir_line[i] - 40;
    current_error /= 68.0f;

    for(int i = 60;i<94;i++){
        left_right_error += cir_line[i];
        left_right_error -= cir_line[i+34];
    }
    //左边圆环速度应当向左，右边圆环速度应当向右
    if(circule_handle_flag == 1)
        cirucle_xspeed = Circule_LorR? -Circule_Normal_xSpeed : Circule_Normal_xSpeed;
    else if(cross_handle_flag == 1)
        cirucle_xspeed = L_or_R_Cross? Circule_Normal_xSpeed : -Circule_Normal_xSpeed;

    //PID闭环控制
    Car_Change_Speed(cirucle_xspeed,
                    Pos_PID_Controller(&circule_Trace_Con_Vy,current_error),
                    Pos_PID_Controller(&circule_Trace_Con_Omega,left_right_error));
    left_right_error = 0;
    current_error = 0;
}

/**
 * @brief 圆环处理初始化函数
 * 
 */
// uint8_t circule_trace_debug = 0;
void circule_handle_init(){

    rt_kprintf("circule handle task init\n");
	circule_handle_sem = rt_sem_create("circule_handle_sem",0,RT_IPC_FLAG_FIFO);
	if(circule_handle_sem == RT_NULL){
		rt_kprintf("circule_handle_sem created failed\n");
		while(1);
	}
	circule_handle_thread = rt_thread_create("side_catch",circule_handle_entry,RT_NULL,2048,3,1000);
	if(circule_handle_thread == RT_NULL){
        rt_kprintf("circule_handle_thread created failed\n");
        while(1);
    }

	rt_thread_startup(circule_handle_thread);

    Pos_PID_Init(&circule_Trace_Con_Omega,-0.7,0,0);
    circule_Trace_Con_Omega.Output_Max = 70;
    circule_Trace_Con_Omega.Output_Min = -70;
    circule_Trace_Con_Omega.Value_I_Max = 200;
    circule_Trace_Con_Omega.Ref = 0;

    Pos_PID_Init(&circule_Trace_Con_Vy,-1.5,0,0);
    circule_Trace_Con_Vy.Output_Max = 60;
    circule_Trace_Con_Vy.Output_Min = -60;
    circule_Trace_Con_Vy.Value_I_Max = 200;
    circule_Trace_Con_Vy.Ref = 0;

}