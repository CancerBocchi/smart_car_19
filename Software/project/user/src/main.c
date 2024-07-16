#include "zf_common_headfile.h"

rt_thread_t led_thread;

int Start_Flag = 0;

int exposure_time = 128;


void led_thread_entry()
{
	gpio_init(B9,GPO,0, GPO_PUSH_PULL);
	while(1)
	{
		gpio_toggle_level(B9);
		rt_thread_delay(100);
	}
}

int main()
{
	led_thread = rt_thread_create("blink",led_thread_entry,NULL,1024,10,1000);
	rt_thread_startup(led_thread);

	debug_tool_init();
	
	rt_kprintf("\nSystem_Init:\n");

	rt_kprintf("----------  Basic Hardware Init ----------\n");
	//基础外设初始化

	Step_Motor_Init();
	rt_thread_delay(200);
	Class_Init();
	buzzer_init();
	Camera_and_Screen_Init();
	
	while(!Start_Flag){
		if(mt9v03x_finish_flag){
			Vision_Handle();
		}
		mt9v03x_set_exposure_time(exposure_time);
			
		rt_thread_delay(1);
	}

	rt_kprintf("---------- task init ----------\n");
	//任务初始化
	MCX_UART_Init();
	Art_UART_Init();
	car_motion_Init();
	locate_pic_init();
	side_catch_init();
	final_init();
	circule_handle_init();
	cross_handle_init();
	trace_line_init();


	rt_kprintf("--------- init end ----------\n");

	// while(1){
	// 	if(mt9v03x_finish_flag){
	// 		circule_trace_line();
	// 		for(int i = 0;i<imgCol-1;i++){
	// 			ips200_draw_point(i,cir_line[i],RGB565_RED);
	// 		}
	// 	}
	// 	rt_thread_delay(1);
	// }

}
