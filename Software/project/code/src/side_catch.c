#include "side_catch.h"

rt_sem_t side_catch_sem;
rt_thread_t side_catch_thread;



//线程运行函数
void side_catch_entry()
{
	while(1)
	{
		//阻塞线程
		rt_sem_take(side_catch_sem,RT_WAITING_FOREVER);
		
		// //
		Car_Change_Speed(0,0,0);
		// rt_thread_delay(1000);
		BUZZER_SPEAK;
		
		if(center_x > 125){
			Car_Rotate(-90);
			rt_thread_delay(2000);
			Car_Rotate(90);
			rt_thread_delay(2000);
		}
		else if(center_x < 125){
			Car_Rotate(90);
			rt_thread_delay(2000);
			Car_Rotate(-90);
			rt_thread_delay(2000);
		}

		uart_write_byte(ART1_UART,'R');
		rt_kprintf("handle success\n");
		// rt_thread_delay(1000);
		Car_Speed_ConRight = Con_By_TraceLine;
		
		rt_sem_release(trace_line_sem);
	
	
	}


}

//初始化函数
void side_catch_init()
{
	side_catch_sem = rt_sem_create("side_catch_sem",0,RT_IPC_FLAG_FIFO);
	
	side_catch_thread = rt_thread_create("side_catch",side_catch_entry,RT_NULL,1024,3,1000);
	
	rt_thread_startup(side_catch_thread);
}