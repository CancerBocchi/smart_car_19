
#include "zf_common_headfile.h"

rt_thread_t led_thread;

void led_thread_entry()
{
	gpio_init(B9,GPO,0, GPO_PUSH_PULL);
	while(1)
	{
		gpio_toggle_level(B9);
		rt_thread_delay(100);
	}
}

rt_sem_t sem1;
rt_sem_t sem2;

rt_thread_t sem_demo_1;
rt_thread_t sem_demo_2;

uint8_t demo1;
uint8_t demo2;

void demo_1_entry()
{
	while(1)
	{
		rt_kprintf("demo1 run\n");
		rt_thread_delay(100);
		demo1 ++;
		if(demo1 == 10)
		{
			rt_kprintf("demo1 stop\n");
			rt_sem_release(sem2);
			rt_sem_take(sem1,RT_WAITING_FOREVER);
		}
		
	}
}

void demo_2_entry()
{
	while(1)
	{
		rt_sem_take(sem2,RT_WAITING_FOREVER);
		rt_kprintf("demo2 get and run\n");
		rt_thread_delay(1000);
		rt_kprintf("demo2 stop\n");
		demo1 = 0;
		rt_sem_release(sem1);
	}
	
}


int main()
{
	led_thread = rt_thread_create("blink",led_thread_entry,NULL,1024,10,1000);
	rt_thread_startup(led_thread);
//	
//	sem1 = rt_sem_create("demo1_sem",0,RT_IPC_FLAG_FIFO);
//	sem2 = rt_sem_create("demo2_sem",0,RT_IPC_FLAG_FIFO);
//	
//	sem_demo_1 = rt_thread_create("demo1",demo_1_entry,RT_NULL,512,9,1000);
//	sem_demo_2 = rt_thread_create("demo2",demo_2_entry,RT_NULL,512,9,1000);
//	
//	rt_thread_startup(sem_demo_1);
//	rt_thread_startup(sem_demo_2);
//	

	
	debug_tool_init();
	
	rt_kprintf("\nSystem_Init:\n");
	Step_Motor_Init();
	buzzer_init();
	car_motion_Init();
	trace_line_init();
	//side_catch_init();
	//ART1_UART_Init();

}
