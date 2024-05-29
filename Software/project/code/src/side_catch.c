#include "side_catch.h"

rt_sem_t side_catch_sem;
rt_thread_t side_catch_thread;



//�߳����к���
void side_catch_entry()
{
	while(1)
	{
		int LorR = 0;
		//�����߳�
		rt_sem_take(side_catch_sem,RT_WAITING_FOREVER);
		
		// //
		Car_Change_Speed(0,0,0);
		// rt_thread_delay(1000);
		BUZZER_SPEAK;
		
		if(center_x > 125){
			Car_Rotate(-90);
			LorR = 1;
		}
		else if(center_x < 125){
			Car_Rotate(90);
			LorR = 0;
		}

		//�ӳ��ǵĳ���ת�����
		rt_thread_delay(1000);

		//������λץȡ�߳� ��������߳�
		rt_sem_release(locate_picture_sem);
		rt_sem_take(side_catch_sem,RT_WAITING_FOREVER);
		
		if(LorR)
			Car_Rotate(90);
		else if(!LorR)
			Car_Rotate(-90);
		
		rt_thread_delay(1000);

		uart_write_byte(ART1_UART,'R');
		rt_kprintf("handle success\n");
		//�����ٶȿ���Ȩ�黹��Ѳ��
		
		Car_Speed_ConRight = Con_By_TraceLine;
		
		rt_sem_release(trace_line_sem);
	
	
	}


}

//��ʼ������
void side_catch_init()
{
	side_catch_sem = rt_sem_create("side_catch_sem",0,RT_IPC_FLAG_FIFO);
	
	if(side_catch_sem == RT_NULL){
		rt_kprintf("side_catch_sem created failed\n");
		while(1);
	}

	side_catch_thread = rt_thread_create("side_catch",side_catch_entry,RT_NULL,1024,3,1000);
	
	rt_thread_startup(side_catch_thread);
}