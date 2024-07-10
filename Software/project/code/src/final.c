#include "final.h"

rt_sem_t final_sem;
rt_thread_t final_thread;

int final_flag = 0;

/**
 * @brief �����������к���
 * 
 */
void final_entry(){
    while(1){
        rt_sem_take(final_sem,RT_WAITING_FOREVER);
        rt_kprintf("final_task:start!\n");
        final_flag = 1;
        MCX_Change_Mode(MCX_Reset_Mode);
        Art_Change_Mode(Art_Reset_Mode);
        //ͣ����ת
        Car_Change_Speed(0,0,0);
        rt_thread_delay(10);
        Car_Rotate(-90);

        Car_Change_Speed(30,0,0);
        
        int class_num = 0;
        while(class_num <3){
            MCX_Change_Mode(MCX_Location_Mode);
            if(center_x!=0||center_y!=0){
                Car_Change_Speed(0,0,0);
                rt_sem_release(locate_catch_flag);
                rt_sem_take(final_sem,RT_WAITING_FOREVER);
                rt_kprintf("final_thread:return from location\n");
                class_num += 1;
            }
            Car_Change_Speed(30,0,0);
        }
        Car_Change_Speed(0,0,0);
        Car_Rotate(90);
        
    }

}

/**
 * @brief  ��ʼ����������
 * 
 */
void final_init(){
    //��ʼ���ź���
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