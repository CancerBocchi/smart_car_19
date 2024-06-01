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

/**
 * @brief ���ڶ�λץȡͼƬdebug�ĳ���
 * 
 */
uint8_t locate_catch_flag = 0;
uint8_t locate_debug_flag = 0;
void locate_picture_debug(){

	MCX_Change_Mode(MCX_Location_Mode);
	Vy = Pos_PID_Controller(&center_y_con,center_y);
	Vx = Pos_PID_Controller(&center_x_con,center_x);
	Car_Change_Speed(Vx,Vy,0);
	//ץȡ����
	if(locate_catch_flag){
		Car_Change_Speed(0,0,0);
		Step_Motor_Catch();
		locate_catch_flag = 0;
	}
	rt_thread_delay(1);
}

/**
 * @brief ��λͼƬ���к��� ������ʽ������
 * 
 */
void locate_picture_run(){

	static int located_n;//��¼������λ׼ȷ�Ĵ���
	rt_sem_take(locate_picture_sem,RT_WAITING_FOREVER);
		if(!begin_flag){
			Car_Rotate(0);
			begin_flag = 1;
		}
		
		//����ʮ�ζ�λ�ɹ������ץȡ
		while(1){
			while(located_n < 5){
				MCX_Change_Mode(MCX_Location_Mode);
				located_n = Is_Located? located_n + 1:0;
				Vy = Pos_PID_Controller(&center_y_con,center_y);
				Vx = Pos_PID_Controller(&center_x_con,center_x);
				Car_Change_Speed(Vx,Vy,0);
			}
			located_n = 0;
			Car_Change_Speed(0,0,0);
			rt_thread_delay(1);
			//ץȡ
			Step_Motor_Catch();
			//���񵽿�Ƭ��û�п�Ƭ�� 
			if(center_x == 0&&center_y == 0)
				break;
		}

		MCX_Change_Mode(MCX_Reset_Mode);
		//�����־λ
		begin_flag = 0;
		//���ر��ߴ����߳�
		if(side_catch_flag == 1){
			side_catch_flag = 0;
			rt_sem_release(side_catch_sem);
		}
		//����Բ�������߳�
		else if(circule_handle_flag == 1){
			circule_handle_flag = 0;
			rt_sem_release(circule_handle_sem);
		}
		
}

void locate_picture_entry()
{
	while(1)
	{
		if(!locate_debug_flag)
			locate_picture_run();
		else
			locate_picture_debug();
	}
}


void locate_pic_init()
{
	rt_kprintf("locate_pic task init\n");
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

	//���Ա�־λ 0---������ 1---����
	locate_debug_flag = 1;
}
