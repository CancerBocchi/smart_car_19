#include "buzzer.h"

rt_thread_t buzzer_thread;

rt_mailbox_t buzzer_mail;

uint32_t buzzer_buffer;


void buzzer_entry()
{
	while(1)
	{
		rt_mb_recv(buzzer_mail,&buzzer_buffer,RT_WAITING_FOREVER);
		
		gpio_set_level(BUZZER_PIN,0);
		rt_thread_delay(500);
		gpio_set_level(BUZZER_PIN,1);
	}
}

void buzzer_init()
{
	gpio_init(BUZZER_PIN,GPO,1,GPO_PUSH_PULL);
	gpio_init(B17,GPO,1,GPO_PUSH_PULL);
//	
	buzzer_thread = rt_thread_create("buzzer",buzzer_entry,RT_NULL,512,10,100);
	buzzer_mail = rt_mb_create("buzzer_mail",1,RT_IPC_FLAG_FIFO);
	
	rt_thread_startup(buzzer_thread);

}