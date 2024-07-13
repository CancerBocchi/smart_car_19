#include "cross_handle.h"

//信号量控制块
rt_sem_t cross_handle_sem;
//线程控制块
rt_thread_t cross_handle_thread;

uint8_t cross_handle_flag;

void cross_handle_entry(){

    while(1){
        rt_sem_take(cross_handle_sem,RT_WAITING_FOREVER);
        cross_handle_flag = 1;

        MCX_Clear();
        MCX_Change_Mode(MCX_Put_Mode);
        rt_thread_delay(100);
        //右边
        if(center_x>160)
            Car_Rotate(-90);
        //左边
        else if(center_x<160)
            Car_Rotate(90);

        rt_thread_delay(300);

        Locate_SetMode(Locate_Catch);
        rt_sem_release(locate_picture_sem);
        rt_sem_take(cross_handle_sem,RT_WAITING_FOREVER);

        Car_Rotate(180);

        int cur_yaw = Att_CurrentYaw;
        int tar_angle = 0;
        while(1){
            if(mt9v03x_finish_flag)
                circule_trace_line();
            rt_thread_delay(1);

            if(Tool_IsFloatEqu(abs(Att_CurrentYaw - cur_yaw),tar_angle,0.01f)){
                Car_Change_Speed(0,0,0);
                rt_thread_delay(100);
                Car_DistanceMotion(0,-15,0.5);
                put_flag = 1;
                rt_sem_release(locate_picture_sem);
                rt_sem_take(circule_handle_sem,RT_WAITING_FOREVER);
                Car_DistanceMotion(0,25,0.5);
                if(tar_angle  == 240){
                    rt_kprintf("cro:return to the final yaw\n");
                    break;
                }
                tar_angle+=60;
            }
        }

        while(1);
    }

}

void cross_handle_init(){
    rt_kprintf("cross handle task init\n");
	cross_handle_sem = rt_sem_create("cross_handle_sem",0,RT_IPC_FLAG_FIFO);
	if(cross_handle_sem == RT_NULL){
		rt_kprintf("cross_handle_sem created failed\n");
		while(1);
	}
	cross_handle_thread = rt_thread_create("side_catch",cross_handle_entry,RT_NULL,2048,3,1000);
	if(cross_handle_thread == RT_NULL){
        rt_kprintf("cross_handle_thread created failed\n");
        while(1);
    }
}