#include "cross_handle.h"

//信号量控制块
rt_sem_t cross_handle_sem;
//线程控制块
rt_thread_t cross_handle_thread;

uint8_t cross_handle_flag;

uint8_t L_or_R_Cross;

void cross_handle_entry(){

    while(1){
        rt_sem_take(cross_handle_sem,RT_WAITING_FOREVER);
        cross_handle_flag = 1;

        MCX_Clear();
        MCX_Change_Mode(MCX_Put_Mode);
        rt_thread_delay(100);
        //右边
        if(L_or_R_Cross == Right_Cross){
            Car_Rotate(-90);
            rt_thread_delay(400);
            Car_DistanceMotion(-50,0,1);
        }
            
            
        //左边
        else if(L_or_R_Cross == Left_Cross){
            Car_Rotate(90);
            rt_thread_delay(400);
            Car_DistanceMotion(50,0,1);
        }

        rt_thread_delay(500);

        if(cur_PicNum == 0){
            L_or_R_Cross = !L_or_R_Cross;
            Car_Rotate(180);
            rt_thread_delay(800);
        }
        
            

        Locate_SetMode(Locate_Catch);
        rt_sem_release(locate_picture_sem);
        rt_sem_take(cross_handle_sem,RT_WAITING_FOREVER);

        Car_Rotate(180);
        rt_thread_delay(800);

        Car_DistanceMotion(0,10,0.5);

        float cur_yaw = Att_CurrentYaw;
        int tar_angle = 10;
        rt_kprintf("cur_yaw:%.2f\n",cur_yaw);
        while(1){
            if(mt9v03x_finish_flag)
                circule_trace_line();
            rt_thread_delay(1);
            // rt_kprintf("%.2f\n",fabs(Att_CurrentYaw - cur_yaw));
            if(Tool_IsFloatEqu(fabs(Att_CurrentYaw - cur_yaw),tar_angle,1.0f)){
                Car_Change_Speed(0,0,0);
                rt_thread_delay(100);
                Car_DistanceMotion(0,-15,0.5);
                put_flag = 1;
                rt_sem_release(locate_picture_sem);
                rt_sem_take(cross_handle_sem,RT_WAITING_FOREVER);
                if(L_or_R_Cross == Left_Cross)
                    Car_DistanceMotion(25,40,1.3);
                else 
                    Car_DistanceMotion(0,40,1.2);
                if(tar_angle  == 250){
                    rt_kprintf("cro:return to the final yaw\n");
                    break;
                }
                tar_angle+=60;
            }
        }
        Car_Change_Speed(0,0,0);
        rt_thread_delay(100);
        

        if(L_or_R_Cross == Left_Cross){
            Car_DistanceMotion(-90,-20,1.5);
            Car_Rotate(90);
        }
           
        else if(L_or_R_Cross == Right_Cross){
             Car_DistanceMotion(90,-20,1.5);
            Car_Rotate(-90);
        }

        rt_thread_delay(500);
        cross_handle_flag = 0;
        Car_Speed_ConRight = Con_By_TraceLine;
        Class_Cir_Reset();
        rt_kprintf("cross handled successfully\n");
        rt_sem_release(trace_line_sem);
    }

}

void cross_handle_init(){

    rt_kprintf("cross handle task init\n");
	cross_handle_sem = rt_sem_create("cross_handle_sem",0,RT_IPC_FLAG_FIFO);
	if(cross_handle_sem == RT_NULL){
		rt_kprintf("cross_handle_sem created failed\n");
		while(1);
	}
	cross_handle_thread = rt_thread_create("cross_handle",cross_handle_entry,RT_NULL,2048,3,1000);
	if(cross_handle_thread == RT_NULL){
        rt_kprintf("cross_handle_thread created failed\n");
        while(1);
    }

    rt_thread_startup(cross_handle_thread);
}