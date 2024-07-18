#include "side_catch.h"

rt_sem_t side_catch_sem;
rt_thread_t side_catch_thread;
//告诉定位线程返回side catch线程
uint8_t side_catch_flag = 0;

//线程运行函数
void side_catch_entry()
{
	while(1)
	{
		int LorR = 0;
		//阻塞线程
		rt_sem_take(side_catch_sem,RT_WAITING_FOREVER);
		
		rt_kprintf("task:get into side catch task\n");

		Car_Change_Speed(0,0,0);
		rt_thread_delay(100);
		BUZZER_SPEAK;
		
		if(L_or_R_pic == RIGHT_PIC){
			// Car_Rotate(-90);//右转
			rt_kprintf("side:right pic:%d\n",center_x);
			Car_DistanceMotion(35,15,0.8);
			LorR = 1;
		}
		else if(L_or_R_pic == LEFT_PIC){
			//Car_Rotate(90);//左转
			rt_kprintf("side:left pic x:%d\n",center_x);
			Car_DistanceMotion(-35,15,0.8);
			LorR = 0;
		}

		//延迟是的车辆转弯完
		MCX_Change_Mode(MCX_Location_Mode);
		//启动定位抓取线程 挂起边线线程
		
		side_catch_flag = 1;
		Locate_SetMode(Locate_Catch);
		rt_sem_release(locate_picture_sem);
		rt_sem_take(side_catch_sem,RT_WAITING_FOREVER);

		//如果没有误识别
		if(!error_detect_flag){
			if(LorR == 1)
				Car_DistanceMotion(-30,10,0.8);
			else if(LorR == 0)
				Car_DistanceMotion(30,10,0.8);
		}
		else{
			if(LorR == 1)
				Car_DistanceMotion(-30,-10,0.8);
			else if(LorR == 0)
				Car_DistanceMotion(30,-10,0.8);
		}

		
		rt_thread_delay(100);
		MCX_Clear();
		//转换 MCX 工作模式
		MCX_Change_Mode(MCX_Detection_Mode);
		rt_kprintf("handle success\n");
		//将角速度控制权归还给巡线
		Car_Speed_ConRight = Con_By_TraceLine;
		
		rt_sem_release(trace_line_sem);
	
	
	}


}

//初始化函数
void side_catch_init()
{
	rt_kprintf("side catch task init\n");
	side_catch_sem = rt_sem_create("side_catch_sem",0,RT_IPC_FLAG_FIFO);
	
	if(side_catch_sem == RT_NULL){
		rt_kprintf("side_catch_sem created failed\n");
		while(1);
	}

	side_catch_thread = rt_thread_create("side_catch",side_catch_entry,RT_NULL,1024,3,1000);
	
	rt_thread_startup(side_catch_thread);

	MCX_Change_Mode(MCX_Detection_Mode);
}