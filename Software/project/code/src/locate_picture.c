#include "locate_picture.h"

#define TARGET_X 85
#define TARGET_Y 170

#define Is_Located (fabs(center_x - TARGET_X)<=5 && fabs(center_y - TARGET_Y)<=5)

rt_thread_t locate_picture_thread;
rt_sem_t locate_picture_sem;

Pos_PID_t center_y_con;
Pos_PID_t center_x_con;

int begin_flag;

float Vx;
float Vy;

void locate_picture_entry()
{
	static int located_n;//记录连续定位准确的次数
	while(1)
	{
		//阻塞线程
		rt_sem_take(locate_picture_sem,RT_WAITING_FOREVER);
		if(!begin_flag){
			Car_Rotate(0);
			begin_flag = 1;
		}
		
		//连续十次定位成功后进行抓取
		while(located_n <= 10){
			uart_write_byte(ART1_UART,'L');	
			located_n = Is_Located? located_n + 1:0;
			Vy = Pos_PID_Controller(&center_y_con,center_y);
			Vx = Pos_PID_Controller(&center_x_con,center_x);
			Car_Change_Speed(Vx,Vy,0);
		}
		
		Car_Change_Speed(0,0,0);
		rt_thread_delay(1);
		uart_write_byte(ART1_UART,'N');
		//抓取
		Step_Motor_Catch();
		//清除标志位
		begin_flag = 0;
		located_n = 0;
		//返回边线处理线程
		rt_sem_release(side_catch_sem);
	}
}


void locate_pic_init()
{
	locate_picture_sem = rt_sem_create("locate",0,RT_IPC_FLAG_FIFO);
	if(locate_picture_sem == RT_NULL){
		rt_kprintf("locate_picture_sem created failed\n");
		while(1);
	}

	locate_picture_thread = rt_thread_create("locate",locate_picture_entry,RT_NULL,1024,3,1000);
	rt_thread_startup(locate_picture_thread);
	
	Pos_PID_Init(&center_y_con,-1.5,0,0);
	center_y_con.Output_Max = 100;
	center_y_con.Output_Min = -100;
	center_y_con.Value_I_Max = 500;
	center_y_con.Ref = TARGET_Y;
	
	Pos_PID_Init(&center_x_con,-1.2,0,0);
	center_x_con.Output_Max = 100;
	center_x_con.Output_Min = -100;
	center_x_con.Value_I_Max = 500;
	center_x_con.Ref = TARGET_X;


}
