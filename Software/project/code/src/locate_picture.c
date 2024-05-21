#include "locate_picture.h"

rt_thread_t locate_picture_thread;

rt_sem_t locate_picture_sem;

Pos_PID_t center_y_con;
Pos_PID_t center_x_con;

uint8_t target_x;
uint8_t target_y;

void locate_picture_entry()
{
	while(1)
	{
		rt_sem_take(locate_picture_sem,RT_WAITING_FOREVER);

	
	
	}
}


void locate_pic_init()
{
	locate_picture_sem = rt_sem_create("locate",0,RT_IPC_FLAG_FIFO);
	
	locate_picture_thread = rt_thread_create("locate",locate_picture_entry,RT_NULL,1024,3,1000);
	
	rt_thread_startup(locate_picture_thread);
	
	Pos_PID_Init(&center_y_con,0,0,0);
	center_y_con.Output_Max = 100;
	center_y_con.Output_Min = -100;
	center_y_con.Value_I_Max = 500;
	center_y_con.Ref = 0;
	
	Pos_PID_Init(&center_x_con,0,0,0);
	center_x_con.Output_Max = 100;
	center_x_con.Output_Min = -100;
	center_x_con.Value_I_Max = 500;
	center_x_con.Ref = 0;
	
	target_x = 0;
	target_y = 0;

}
