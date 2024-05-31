#include "circule_handle.h"

//信号量控制块
rt_sem_t circule_handle_sem;
//线程控制块
rt_thread_t circule_handle_thread;
//告诉定位线程返回circule_handle_flag线程
uint8_t circule_handle_flag = 0;


void circule_handle_entry(){
    static uint8_t begin_flag = 0;
    while(1){
        rt_sem_take(circule_handle_sem,RT_WAITING_FOREVER);
        //初始处理
        if(!begin_flag){
            //转向
            if(Cirule_LorR == LEFT_CIRCULE)
                Car_Rotate(90);
            else 
                Car_Rotate(-90);
            //定位跑
            Car_DistanceMotion(0,40,0.5);
            begin_flag = 1;
        }

        //启动定位抓取线程
        circule_handle_flag = 1;
        rt_sem_release(locate_picture_sem);
        rt_sem_take(circule_handle_sem,RT_WAITING_FOREVER);

        


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