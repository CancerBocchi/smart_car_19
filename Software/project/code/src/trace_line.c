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
int32 TraceLine_Aver_Offset;
int32 TraceLine_Last_Offset;
int32 TraceLine_Forward_V;

float speed_forward;

//
// @brief 大津法
//
// int My_Adapt_Threshold(uint8*image,uint16 width, uint16 height)   //大津算法，注意计算阈值的一定要是原图像
// {
//     #define GrayScale 256
//     int pixelCount[GrayScale];
//     float pixelPro[GrayScale];
//     int i, j;
//     int pixelSum = width * height/4;
//     int  threshold = 0;
//     uint8* data = image;  //指向像素数据的指针
//     for (i = 0; i < GrayScale; i++)
//     {
//         pixelCount[i] = 0;
//         pixelPro[i] = 0;
//     }
//     uint32 gray_sum=0;
//     for (i = 0; i < height; i+=2)//统计灰度级中每个像素在整幅图像中的个数
//     {
//         for (j = 0; j <width; j+=2)
//         {
//             pixelCount[(int)data[i * width + j]]++;  //将当前的点的像素值作为计数数组的下标
//             gray_sum+=(int)data[i * width + j];       //灰度值总和
//         }
//     }
//     for (i = 0; i < GrayScale; i++) //计算每个像素值的点在整幅图像中的比例
//     {
//         pixelPro[i] = (float)pixelCount[i] / pixelSum;
//     }
//     float w0, w1, u0tmp, u1tmp, u0, u1, u, deltaTmp, deltaMax = 0;
//     w0 = w1 = u0tmp = u1tmp = u0 = u1 = u = deltaTmp = 0;
//     for (j = 0; j < GrayScale; j++)//遍历灰度级[0,255]
//     {
//         w0 += pixelPro[j];  //背景部分每个灰度值的像素点所占比例之和   即背景部分的比例
// 				if(pixelPro[j] == 0)
// 					continue;
//         u0tmp += j * pixelPro[j];  //背景部分 每个灰度值的点的比例 *灰度值
//         w1=1-w0;
//         u1tmp=gray_sum/pixelSum-u0tmp;
//         u0 = u0tmp / w0;              //背景平均灰度
//         u1 = u1tmp / w1;              //前景平均灰度
//         u = u0tmp + u1tmp;            //全局平均灰度
//         deltaTmp = w0 * pow((u0 - u), 2) + w1 * pow((u1 - u), 2);//平方
//         if (deltaTmp > deltaMax)
//         {
//             deltaMax = deltaTmp;//最大类间方差法
//             threshold = j;
//         }
//         if (deltaTmp < deltaMax)
//         {
//             break;
//         }
//     }
//     if(threshold>255)
//         threshold=255;
//     if(threshold<0)
//         threshold=0;
//   return threshold;
// }

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
			UseImage();

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
			
			//计算最后一个块的偏差值
//			for(int i = LAST_OFFSET_POINT_NUM-1; i>=0;i--)
//			{
//				TraceLine_Last_Offset += (imgCol/2 - 1 - Image_S.MID_Table[i]);
//			}
//			TraceLine_Last_Offset /= LAST_OFFSET_POINT_NUM;
			//计算总偏差值
			// for(int i = imgRow-1; i>=row_begin;i--)
			// {
			// 		TraceLine_Aver_Offset += (imgCol/mid_offset - 1 - Image_S.MID_Table[i])*(i+1)*(i+1)/imgRow/imgRow;
			// }
			// TraceLine_Aver_Offset /= imgRow;
//			if(TraceLine_Aver_Offset>50)
//			{
//					Car_Change_Speed(Car_Speed.Vx,speed_forward/2,Car_Speed.Omega);
//			}
//			else{
//					Car_Change_Speed(Car_Speed.Vx,speed_forward,Car_Speed.Omega);
//			}
//			
//			//计算环路输出结果
			// Car_Change_Yaw(Pos_PID_Controller(&TraceLine_Yaw_Con,TraceLine_Aver_Offset));
			
			//只有在有运行权限的时候才能运行环路
			if(Car_Speed_ConRight == Con_By_TraceLine){
				float vx = Pos_PID_Controller(&TraceLine_Vx_Con,Center);
				float yaw = Pos_PID_Controller(&TraceLine_Yaw_Con,Center);
				Car_Change_Speed(vx,speed_forward,yaw);
			}
	
			//TraceLine_Aver_Offset = 0;
			
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



