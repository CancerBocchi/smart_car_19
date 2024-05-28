#include "locate_picture.h"

rt_thread_t locate_picture_thread;

rt_sem_t locate_picture_sem;

Pos_PID_t center_y_con;
Pos_PID_t center_x_con;

int begin_flag;

float Vx;
float Vy;

uint8_t target_x;
uint8_t target_y;

void locate_picture_entry()
{
	while(1)
	{
		//rt_sem_take(locate_picture_sem,RT_WAITING_FOREVER);
		if(!begin_flag){
			Car_Rotate(0);
			begin_flag = 1;
		}
			
		uart_write_byte(ART1_UART,'L');	

		if(Art1_Detection_Flag){
			Vy = Pos_PID_Controller(&center_y_con,center_y);
			Vx = Pos_PID_Controller(&center_x_con,center_x);
			
			Car_Change_Speed(Vx,Vy,0);
		}

		rt_thread_delay(2);
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
	center_y_con.Ref = 120;
	
	Pos_PID_Init(&center_x_con,0,0,0);
	center_x_con.Output_Max = 100;
	center_x_con.Output_Min = -100;
	center_x_con.Value_I_Max = 500;
	center_x_con.Ref = 160;
	
	target_x = 0;
	target_y = 0;


}
