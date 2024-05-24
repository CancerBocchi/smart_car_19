/***********************************************************************
* 一个环路控制平移速度，控制量为所有中点与图像中线的偏差量的平均值
* 另一个环路控制航向角，控制量为第一个块(LAST_OFFSET_POINT_NUM)与中点距离的平均值
* 参考链接：https://www.bilibili.com/video/BV1ZT4y1D716/?spm_id_from=333.999.
						list.card_archive.click&vd_source=16cf9fe495e6537559260c9d32da5153
*
************************************************************************/

#include "trace_line.h"
#include "camera.h"
#define START_X 1
#define START_Y 10
#define BLACK 0
#define WHITE 255

//-------------------------------------------------------------------------------------------------------------------
//  @data 数据部分
//-------------------------------------------------------------------------------------------------------------------
//线程控制块
rt_thread_t trace_line_thread;
//信号量控制块
rt_sem_t 		trace_line_sem;
//原始图像
uint8 frame[MT9V03X_W][MT9V03X_H];
float mid_offset=1.65;
uint8_t row_begin = 20;
//PID控制块
Pos_PID_t TraceLine_Yaw_Con;
Pos_PID_t TraceLine_Vx_Con;
Pos_PID_t TraceLine_Normal_Con;

int32 TraceLine_Aver_Offset;
int32 TraceLine_Last_Offset;
int32 TraceLine_Forward_V;

//基础速度
float speed_forward;

/**
 * @brief 巡线策略函数
 * 
 */
void trace_line_method()
{
	//速度控制权判定
	if(Car_Speed_ConRight == Con_By_TraceLine){
		//策略1 常规巡线
		//计算总偏差值

		for(int i = imgRow-1; i>=row_begin;i--)
			TraceLine_Aver_Offset += (imgCol/mid_offset - 1 - Image_S.MID_Table[i])*(i+1)*(i+1)/imgRow/imgRow;

		TraceLine_Aver_Offset /= imgRow;

		float yaw_now = Pos_PID_Controller(&TraceLine_Normal_Con,TraceLine_Aver_Offset);

		if(TraceLine_Aver_Offset>50)
			Car_Change_Speed(Car_Speed.Vx,speed_forward/2,yaw_now);
		else
			Car_Change_Speed(Car_Speed.Vx,speed_forward,yaw_now);
		TraceLine_Aver_Offset = 0;	


		//策略2 最长白线法
		float vx = Pos_PID_Controller(&TraceLine_Vx_Con,Center);
		float yaw = Pos_PID_Controller(&TraceLine_Yaw_Con,Center);
		Car_Change_Speed(vx,speed_forward,yaw);

		
	}
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      巡线初始化函数
//  @param      void
//  @return     void
//  @e.g.       初始化了摄像头屏幕 以及注册了线程
//-------------------------------------------------------------------------------------------------------------------
void trace_line_init()
{
	rt_kprintf("trace line module init:\n");
	
	//初始化屏幕
	tft180_set_dir(TFT180_CROSSWISE);                                           // 需要先横屏 不然显示不下
	tft180_init();
	tft180_show_string(0, 0, "mt9v03x init.");
	
	//初始化摄像头
	while(1)
	{
			if(mt9v03x_init())
			{
					tft180_show_string(0, 16, "mt9v03x reinit.");
					rt_kprintf("mt9v03x failed try to reinit\n");
			}
			else
			{
					rt_kprintf("mt9v03x init successfully\n");
					break;
			}
			system_delay_ms(1000);                                                  // 闪灯表示异常
	}
	tft180_show_string(0, 16, "init success.");
	tft180_clear();
	
	//初始化PID控制块 
	// Pos_PID_Init(&TraceLine_Yaw_Con,-10,0,-10);//八邻 时的参数

	Pos_PID_Init(&TraceLine_Yaw_Con,3,1,0);
	TraceLine_Yaw_Con.Output_Max = 150;
	TraceLine_Yaw_Con.Output_Min = -150;
	TraceLine_Yaw_Con.Value_I = 200;
	TraceLine_Yaw_Con.Ref = 96;
	
	Pos_PID_Init(&TraceLine_Vx_Con,1,0,0);
	TraceLine_Vx_Con.Output_Max = 50;
	TraceLine_Vx_Con.Output_Min = -50;
	TraceLine_Vx_Con.Value_I = 200;
	TraceLine_Vx_Con.Ref = 96;

	Pos_PID_Init(&TraceLine_Normal_Con,-10,0,-10);
	TraceLine_Normal_Con.Output_Max = 200;
	TraceLine_Normal_Con.Output_Min = -200;
	TraceLine_Normal_Con.Value_I = 200;
	TraceLine_Normal_Con.Ref = 0;

	
	//初始化信号量
	trace_line_sem = rt_sem_create("trace_line_sem",0,RT_IPC_FLAG_FIFO);
	//初始化线程
	trace_line_thread = rt_thread_create("trace line",trace_line_entry,NULL,4096,2,1000);

	if(trace_line_thread != NULL)
	{
		rt_thread_startup(trace_line_thread);
	}
	else 
		rt_kprintf("trace line thread created successfully!\n");

	interrupt_global_enable(0);
}


//-------------------------------------------------------------------------------------------------------------------
//  @brief      巡线线程运行函数
//  @param      void
//  @return     void
//  @e.g.       无
//-------------------------------------------------------------------------------------------------------------------
void trace_line_entry()
{
	while(1)
	{
		if(mt9v03x_finish_flag)
		{	
			Vision_Handle();

			//Vision_GetMyImage();
			//adaptiveThreshold(handle_image,handle_image,IMAGE_COL,IMAGE_ROW,7,adaptivePara);
			tft180_show_gray_image(START_X, START_Y, (const uint8 *)my_image, imgCol, imgRow, 158, 70, 0);
			//tft180_show_gray_image(START_X, START_Y, (const uint8 *)handle_image, IMAGE_COL, IMAGE_ROW, 158, 70, 0);
			//tft180_show_gray_image(START_X, START_Y, (const uint8 *)mt9v03x_image, MT9V03X_W, MT9V03X_H, 158, 70, 0);
			//tft180_clear();
			for(int i=imgRow-1;i>=row_begin;i--)
			{
				// tft180_draw_point(Image_S.MID_Table[i], 78-(imgRow-1)+i, RGB565_RED);
				// tft180_draw_point(Image_S.leftBroder[i], 78-(imgRow-1)+i, RGB565_BLUE);
				// tft180_draw_point(Image_S.rightBroder[i], 78-(imgRow-1)+i, RGB565_BROWN);
				tft180_draw_point((int)(160/1.65), 108-(imgRow-1)+i, RGB565_GREEN);
				
			}
			tft180_draw_line(Longest_White_Column_Right[1],78-(imgRow-1),Longest_White_Column_Right[1],78-(imgRow-1)+Longest_White_Column_Right[0],RGB565_RED);
			tft180_draw_line(Longest_White_Column_Left[1],78-(imgRow-1),Longest_White_Column_Left[1],78-(imgRow-1)+Longest_White_Column_Left[0],RGB565_RED);
			tft180_draw_line(Center,78-(imgRow-1),Center,78-(imgRow-1)+Longest_White_Column_Left[0],RGB565_RED);
			

			trace_line_method();

			
			//状态切换管理 若art模块发出了识别到图片的信号，则阻塞该线程，运行边沿检测线程
			// if(Art1_Detection_Flag){
			// 	// Car_Stop();
			// 	rt_kprintf("found picture!x:%d,y:%d\n",center_x,center_y);
			// 	rt_sem_release(side_catch_sem);
			// 	rt_sem_take(trace_line_sem,RT_WAITING_FOREVER);
			// 	Art1_Detection_Flag = 0;
			// }
			
		}
		rt_thread_delay(1);
	}
		
	
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      巡线线程运行函数
//  @param      void
//  @return     void
//  @e.g.       无
//-------------------------------------------------------------------------------------------------------------------



