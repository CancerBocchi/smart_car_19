#include "circule_handle.h"

//Բ�����ĵ�ͼƬ�Եľ���
#define Circule_Distance1

//�ź������ƿ�
rt_sem_t circule_handle_sem;
//�߳̿��ƿ�
rt_thread_t circule_handle_thread;
//���߶�λ�̷߳���circule_handle_flag�߳�
uint8_t circule_handle_flag = 0;


void circule_handle_entry(){
    static uint8_t begin_flag = 0;
    static float init_yaw;
    while(1){
        rt_sem_take(circule_handle_sem,RT_WAITING_FOREVER);
        rt_kprintf("task:get into the circule task\n");
        //��ʼ����
        if(!begin_flag){
            //�õ���ʼ�Ƕ�
            init_yaw = Att_CurrentYaw;
            //ת��
            if(Cirule_LorR == LEFT_CIRCULE)
                Car_Rotate(90);
            else 
                Car_Rotate(-90);
            //����ʱ��ʹ�ó�ת��λ
            rt_thread_delay(500);
            //��λ��
            Car_DistanceMotion(0,-20,0.4);

        }

        // rt_thread_delay(2000);
        //������λץȡ�߳�
        rt_kprintf("task:start to catch things\n");
        circule_handle_flag = 1;
        rt_sem_release(locate_picture_sem);
        rt_sem_take(circule_handle_sem,RT_WAITING_FOREVER);
        rt_kprintf("task:return to the circule_handle thread\n");

        //ץȡ���
        if(Cirule_LorR == LEFT_CIRCULE){
            //��ת
            Car_Rotate(-90);
            //����ʱ��ʹ�ó�ת��λ
            rt_thread_delay(500);
            Car_DistanceMotion(30,-30,0.8);
        }
        else{
            Car_Rotate(90);
            //����ʱ��ʹ�ó�ת��λ
            rt_thread_delay(500);
            Car_DistanceMotion(-30,-30,0.8);
        }
        //����Ѳ���߳�
        Car_Speed_ConRight = Con_By_TraceLine;
        rt_sem_release(trace_line_sem);

    }
}

/**
 * @brief Բ�������ʼ������
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