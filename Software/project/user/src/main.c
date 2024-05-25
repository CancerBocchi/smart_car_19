
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

int main()
{
	led_thread = rt_thread_create("blink",led_thread_entry,NULL,1024,10,1000);
	rt_thread_startup(led_thread);

	debug_tool_init();
	
	rt_kprintf("\nSystem_Init:\n");
	Step_Motor_Init();
	buzzer_init();
	//car_motion_Init();
	trace_line_init();
	//side_catch_init();
	//ART1_UART_Init();

}
