/***********************************************************************
* һ����·����ƽ���ٶȣ�������Ϊ�����е���ͼ�����ߵ�ƫ������ƽ��ֵ
* ��һ����·���ƺ���ǣ�������Ϊ��һ����(LAST_OFFSET_POINT_NUM)���е�����ƽ��ֵ
* �ο����ӣ�https://www.bilibili.com/video/BV1ZT4y1D716/?spm_id_from=333.999.
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
//  @data ���ݲ���
//-------------------------------------------------------------------------------------------------------------------
//�߳̿��ƿ�
rt_thread_t trace_line_thread;
//�ź������ƿ�
rt_sem_t 		trace_line_sem;
//ԭʼͼ��
uint8 frame[MT9V03X_W][MT9V03X_H];
float mid_offset=1.65;
uint8_t row_begin = 20;
//PID���ƿ�
Pos_PID_t TraceLine_Yaw_Con;
Pos_PID_t TraceLine_Vx_Con;
int32 TraceLine_Aver_Offset;
int32 TraceLine_Last_Offset;
int32 TraceLine_Forward_V;

float speed_forward;

//-------------------------------------------------------------------------------------------------------------------
//  @brief      Ѳ�߳�ʼ������
//  @param      void
//  @return     void
//  @e.g.       ��ʼ��������ͷ��Ļ �Լ�ע�����߳�
//-------------------------------------------------------------------------------------------------------------------
void trace_line_init()
{
	rt_kprintf("trace line module init:\n");
	
	//��ʼ����Ļ
	tft180_set_dir(TFT180_CROSSWISE);                                           // ��Ҫ�Ⱥ��� ��Ȼ��ʾ����
	tft180_init();
	tft180_show_string(0, 0, "mt9v03x init.");
	
	//��ʼ������ͷ
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
			system_delay_ms(1000);                                                  // ���Ʊ�ʾ�쳣
	}
	tft180_show_string(0, 16, "init success.");
	tft180_clear();
	
	//��ʼ��PID���ƿ� 
	Pos_PID_Init(&TraceLine_Yaw_Con,-10,0,-10);
	TraceLine_Yaw_Con.Output_Max = 250;
	TraceLine_Yaw_Con.Output_Min = -250;
	TraceLine_Yaw_Con.Value_I = 200;
	TraceLine_Yaw_Con.Ref = 0;
	
	//��ʼ���ź���
	trace_line_sem = rt_sem_create("trace_line_sem",0,RT_IPC_FLAG_FIFO);
	//��ʼ���߳�
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
//  @brief      Ѳ���߳����к���
//  @param      void
//  @return     void
//  @e.g.       ��
//-------------------------------------------------------------------------------------------------------------------
void trace_line_entry()
{
	while(1)
	{
		if(mt9v03x_finish_flag)
		{	
			UseImage();
			//��ʾͼ��
			
			//Vision_GetMyImage();
			//adaptiveThreshold(handle_image,handle_image,IMAGE_COL,IMAGE_ROW,7,adaptivePara);
			tft180_show_gray_image(START_X, START_Y, (const uint8 *)my_image, imgCol, imgRow, 158, 70, 0);
			//tft180_show_gray_image(START_X, START_Y, (const uint8 *)handle_image, IMAGE_COL, IMAGE_ROW, 158, 70, 0);
			//tft180_show_gray_image(START_X, START_Y, (const uint8 *)mt9v03x_image, MT9V03X_W, MT9V03X_H, 158, 70, 0);
			//tft180_clear();
			for(int i=imgRow-1;i>=row_begin;i--)
			{
				tft180_draw_point(Image_S.MID_Table[i], 78-(imgRow-1)+i, RGB565_RED);
				tft180_draw_point(Image_S.leftBroder[i], 78-(imgRow-1)+i, RGB565_BLUE);
				tft180_draw_point(Image_S.rightBroder[i], 78-(imgRow-1)+i, RGB565_BROWN);
				tft180_draw_point((int)(160/1.65), 108-(imgRow-1)+i, RGB565_GREEN);
			}

			
			//�������һ�����ƫ��ֵ
//			for(int i = LAST_OFFSET_POINT_NUM-1; i>=0;i--)
//			{
//				TraceLine_Last_Offset += (imgCol/2 - 1 - Image_S.MID_Table[i]);
//			}
//			TraceLine_Last_Offset /= LAST_OFFSET_POINT_NUM;
			//������ƫ��ֵ
			for(int i = imgRow-1; i>=row_begin;i--)
			{
					TraceLine_Aver_Offset += (imgCol/mid_offset - 1 - Image_S.MID_Table[i])*(i+1)*(i+1)/imgRow/imgRow;
			}
			TraceLine_Aver_Offset /= imgRow;
//			if(TraceLine_Aver_Offset>50)
//			{
//					Car_Change_Speed(Car_Speed.Vx,speed_forward/2,Car_Speed.Omega);
//			}
//			else{
//					Car_Change_Speed(Car_Speed.Vx,speed_forward,Car_Speed.Omega);
//			}
//			
//			//���㻷·������
			// Car_Change_Yaw(Pos_PID_Controller(&TraceLine_Yaw_Con,TraceLine_Aver_Offset));
			
			//ֻ����������Ȩ�޵�ʱ��������л�·
			if(Car_Speed_ConRight == Con_By_TraceLine)
				Car_Change_Speed(Pos_PID_Controller(&TraceLine_Vx_Con,LAST_OFFSET_POINT_NUM),speed_forward,
													Pos_PID_Controller(&TraceLine_Yaw_Con,TraceLine_Aver_Offset));
			//rt_kprintf("%d,%.2f\n",TraceLine_Aver_Offset,Car_Speed.Omega);
			TraceLine_Aver_Offset = 0;
			
			//״̬�л����� ��artģ�鷢����ʶ��ͼƬ���źţ����������̣߳����б��ؼ���߳�
			if(Art1_Detection_Flag){
				// Car_Stop();
				rt_kprintf("found picture!x:%d,y:%d\n",center_x,center_y);
				rt_sem_release(side_catch_sem);
				rt_sem_take(trace_line_sem,RT_WAITING_FOREVER);
				Art1_Detection_Flag = 0;
			}
			
		}
		rt_thread_delay(1);
	}
		
	
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      Ѳ���߳����к���
//  @param      void
//  @return     void
//  @e.g.       ��
//-------------------------------------------------------------------------------------------------------------------



