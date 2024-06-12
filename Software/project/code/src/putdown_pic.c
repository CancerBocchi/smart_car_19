#include "putdown_pic.h"


rt_thread_t putdown_pic_thread;
rt_sem_t putdown_pic_sem;


void putdown_pic_entry(){

    while(1){
        rt_sem_take(putdown_pic_sem,RT_WAITING_FOREVER);








    }
}

void putdown_pic_init(){
    rt_kprintf("putdown_pic task init\n");
	
	putdown_pic_sem = rt_sem_create("locate",0,RT_IPC_FLAG_FIFO);
	if(putdown_pic_sem == RT_NULL){
		rt_kprintf("putdown_pic_sem created failed\n");
		while(1);
	}

	putdown_pic_thread = rt_thread_create("locate",putdown_pic_entry,RT_NULL,1024,3,1000);
	rt_thread_startup(putdown_pic_thread);
}