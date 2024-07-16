#include "side_catch.h"

rt_sem_t side_catch_sem;
rt_thread_t side_catch_thread;
//���߶�λ�̷߳���side catch�߳�
uint8_t side_catch_flag = 0;

//�߳����к���
void side_catch_entry()
{
	while(1)
	{
		int LorR = 0;
		//�����߳�
		rt_sem_take(side_catch_sem,RT_WAITING_FOREVER);
		
		rt_kprintf("task:get into side catch task\n");

		Car_Change_Speed(0,0,0);
		rt_thread_delay(100);
		BUZZER_SPEAK;
		
		if(center_x > 160){
			// Car_Rotate(-90);//��ת
			Car_DistanceMotion(35,15,0.8);
			LorR = 1;
		}
		else if(center_x < 160){
			//Car_Rotate(90);//��ת
			Car_DistanceMotion(-35,15,0.8);
			LorR = 0;
		}

		//�ӳ��ǵĳ���ת����
		MCX_Change_Mode(MCX_Location_Mode);
		rt_thread_delay(200);
		//������λץȡ�߳� ��������߳�
		
		side_catch_flag = 1;
		Locate_SetMode(Locate_Catch);
		rt_sem_release(locate_picture_sem);
		rt_sem_take(side_catch_sem,RT_WAITING_FOREVER);

		//���û����ʶ��
		if(!error_detect_flag){
			if(LorR)
				Car_DistanceMotion(-30,10,0.8);
			else if(!LorR)
				Car_DistanceMotion(30,10,0.8);
		}
		else{
			if(LorR)
				Car_DistanceMotion(-30,-10,0.8);
			else if(!LorR)
				Car_DistanceMotion(30,-10,0.8);
			error_detect_flag = 0;
		}

		
		rt_thread_delay(350);
		//ת�� MCX ����ģʽ
		MCX_Change_Mode(MCX_Detection_Mode);
		rt_kprintf("handle success\n");
		//�����ٶȿ���Ȩ�黹��Ѳ��
		Car_Speed_ConRight = Con_By_TraceLine;
		
		rt_sem_release(trace_line_sem);
	
	
	}


}

//��ʼ������
void side_catch_init()
{
	rt_kprintf("side catch task init\n");
	side_catch_sem = rt_sem_create("side_catch_sem",0,RT_IPC_FLAG_FIFO);
	
	if(side_catch_sem == RT_NULL){
		rt_kprintf("side_catch_sem created failed\n");
		while(1);
	}

	side_catch_thread = rt_thread_create("side_catch",side_catch_entry,RT_NULL,1024,3,1000);
	
	rt_thread_startup(side_catch_thread);
}