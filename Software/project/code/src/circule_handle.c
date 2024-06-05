#include "circule_handle.h"

//圆环中心到图片对的距离
#define Circule_Distance1

//信号量控制块
rt_sem_t circule_handle_sem;
//线程控制块
rt_thread_t circule_handle_thread;
//告诉定位线程返回circule_handle_flag线程
uint8_t circule_handle_flag = 0;


void circule_handle_entry(){
    static uint8_t begin_flag = 0;
    static float init_yaw;
    while(1){
        rt_sem_take(circule_handle_sem,RT_WAITING_FOREVER);
        rt_kprintf("task:get into the circule task\n");
        //初始处理
        if(!begin_flag){
            //得到初始角度
            init_yaw = Att_CurrentYaw;
            //转向
            if(Cirule_LorR == LEFT_CIRCULE)
                Car_Rotate(90);
            else 
                Car_Rotate(-90);
            //给足时间使得车转到位
            rt_thread_delay(500);
            //定位跑
            Car_DistanceMotion(0,-20,0.4);

        }

        // rt_thread_delay(2000);
        //启动定位抓取线程
        rt_kprintf("task:start to catch things\n");
        circule_handle_flag = 1;
        rt_sem_release(locate_picture_sem);
        rt_sem_take(circule_handle_sem,RT_WAITING_FOREVER);
        rt_kprintf("task:return to the circule_handle thread\n");

        //抓取完毕
        if(Cirule_LorR == LEFT_CIRCULE){
            //右转
            Car_Rotate(-90);
            //给足时间使得车转到位
            rt_thread_delay(500);
            Car_DistanceMotion(30,-30,0.8);
        }
        else{
            Car_Rotate(90);
            //给足时间使得车转到位
            rt_thread_delay(500);
            Car_DistanceMotion(-30,-30,0.8);
        }
        //返回巡线线程
        Car_Speed_ConRight = Con_By_TraceLine;
        rt_sem_release(trace_line_sem);

    }
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
}