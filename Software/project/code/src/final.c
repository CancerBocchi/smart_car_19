#include "final.h"

rt_sem_t final_sem;
rt_thread_t final_thread;

int final_flag = 0;
uint8_t final_debug_mode = 0;

/**
 * @brief �����������к���
 * 
 */
void final_entry(){
    while(1){
        if(!final_debug_mode)
            rt_sem_take(final_sem,RT_WAITING_FOREVER);

        rt_kprintf("final_task:start!\n");
        final_flag = 1;
        
        MCX_Change_Mode(MCX_Put_Mode);
        Art_Change_Mode(Art_Reset_Mode);
        //ͣ����ת
        Car_Change_Speed(0,0,0);
        rt_thread_delay(200);
        Car_Rotate(-90);
        rt_thread_delay(1000);

        
        // while(1){
        //     Car_DistanceMotion(150,0,6);
        //     rt_thread_delay(3000);
        //     Car_DistanceMotion(-150,0,6);
        //     rt_thread_delay(3000);
        // }

        Car_Change_Speed(50,0,0);
        
        int class_num = 0;
        int ignore_flag = 0;
        int i = 0,one_num = 0;
        while(class_num <3){
            MCX_Change_Mode(MCX_Put_Mode);
            if(one_num>=8 && !ignore_flag){
                Car_Change_Speed(0,0,0);
                put_flag = 1;
                rt_sem_release(locate_picture_sem);
                rt_sem_take(final_sem,RT_WAITING_FOREVER);
                rt_kprintf("final_thread:return from location\n");
                rt_thread_delay(1000);
                ignore_flag = 1;
                class_num += 1;
            }
            Car_Change_Speed(50,0,0);
            rt_thread_delay(10);
            one_num = 0;
            i = 0;
            while(i < 10){
                if(cur_PicNum == 1)
                    one_num ++;
                rt_thread_delay(30);
                i++;
            }
            if(one_num <= 3)
                ignore_flag = 0;
           
        }
        Car_Change_Speed(0,0,0);
        Car_Rotate(90);
        while(1);
        rt_thread_delay(500);
        rt_sem_release(trace_line_sem);
    }
}

/**
 * @brief  ��ʼ����������
 * 
 */
void final_init(){
    //��ʼ���ź���
    final_debug_mode = 0;
    rt_kprintf("final task init\n");
	final_sem = rt_sem_create("final_sem",0,RT_IPC_FLAG_FIFO);
	if(final_sem == RT_NULL){
		rt_kprintf("final_sem created failed\n");
		while(1);
	}
	//��ʼ���߳�
	final_thread = rt_thread_create("trace line",final_entry,NULL,4096,2,1000);

	if(final_thread != RT_NULL){
		rt_kprintf("trace line thread created successfully!\n");
		rt_thread_startup(final_thread);
	}
	else 
		rt_kprintf("final_thread created failed\n");
}