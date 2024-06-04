#include "locate_picture.h"

#define TARGET_X 105
#define TARGET_Y 160

#define Is_Located (fabs(center_x - TARGET_X)<=2 && fabs(center_y - TARGET_Y)<=2)

rt_thread_t locate_picture_thread;
rt_sem_t locate_picture_sem;

Pos_PID_t center_y_con;
Pos_PID_t center_x_con;

float Vx;
float Vy;

/**
 * @brief 用于定位抓取图片debug的程序
 * 
 */
int locate_catch_flag = 0;
uint8_t locate_debug_flag;
void locate_picture_debug(){

	static int begin_flag;
	if(!begin_flag){
		Car_Rotate(0);
		begin_flag = 1;
	}
	MCX_Change_Mode(MCX_Location_Mode);
	Vy = Pos_PID_Controller(&center_y_con,center_y);
	Vx = Pos_PID_Controller(&center_x_con,center_x);
	Car_Change_Speed(Vx,Vy,0);
	//抓取测试
	if(locate_catch_flag){
		Car_Change_Speed(0,0,0);
		Step_Motor_Catch();
		locate_catch_flag = 0;
	}
	rt_thread_delay(1);
}

/**
 * @brief 定位图片运行函数 用于正式做任务
 * 
 */
void locate_picture_run(){

	static int located_n;//记录连续定位准确的次数
	static int begin_flag;
	rt_sem_take(locate_picture_sem,RT_WAITING_FOREVER);
	rt_kprintf("task:get into the locate task\n");
	if(!begin_flag){
		Car_Rotate(0);
		begin_flag = 1;
	}
	
	//连续十次定位成功后进行抓取
	while(1){
		while(located_n < 15){
			MCX_Change_Mode(MCX_Location_Mode);
			located_n = Is_Located? located_n + 1:0;
			Vy = Pos_PID_Controller(&center_y_con,center_y);
			Vx = Pos_PID_Controller(&center_x_con,center_x);
			Car_Change_Speed(Vx,Vy,0);
			rt_thread_delay(1);
		}
		located_n = 0;
		Car_Change_Speed(0,0,0);
		//抓取
		Step_Motor_Catch();
		//若捡到卡片后没有卡片了 
		if(center_x == 0&&center_y == 0)
			break;
	}

	MCX_Change_Mode(MCX_Reset_Mode);
	//清除标志位
	begin_flag = 0;
	//返回边线处理线程
	if(side_catch_flag == 1){
		rt_kprintf("task:ready to return to side_catch thread\n");
		side_catch_flag = 0;
		rt_sem_release(side_catch_sem);
	}
	//返回圆环处理线程
	if(circule_handle_flag == 1){
		rt_kprintf("task:ready to return to circule_handle thread\n");
		circule_handle_flag = 0;
		MCX_Detection_Flag = 0;
		rt_sem_release(circule_handle_sem);
	}
		
}

void locate_picture_entry()
{
	while(1)
	{
		if(locate_debug_flag == 0)
			locate_picture_run();
		else
			locate_picture_debug();
	}
}


void locate_pic_init()
{
	rt_kprintf("locate_pic task init\n");
	
	//调试标志位 0---不调试 1---调试
	locate_debug_flag = 1;
	
	locate_picture_sem = rt_sem_create("locate",0,RT_IPC_FLAG_FIFO);
	if(locate_picture_sem == RT_NULL){
		rt_kprintf("locate_picture_sem created failed\n");
		while(1);
	}

	locate_picture_thread = rt_thread_create("locate",locate_picture_entry,RT_NULL,1024,3,1000);
	rt_thread_startup(locate_picture_thread);
	
	Pos_PID_Init(&center_y_con,-1.4,0,0);
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
