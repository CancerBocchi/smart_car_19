#include "locate_picture.h"


Pos_PID_t center_y_con;
Pos_PID_t center_x_con;

#define PUT_X 	120
#define PUT_Y 	130

#define CATCH_X 120
#define CATCH_Y 148

#define TARGET_X center_x_con.Ref
#define TARGET_Y center_y_con.Ref

#define Is_Located (fabs(center_x - TARGET_X)<=1 && fabs(center_y - TARGET_Y)<=1)

rt_thread_t locate_picture_thread;
rt_sem_t locate_picture_sem;

float Vx;
float Vy;

uint8_t put_flag;

uint8 error_detect_flag = 0;

/**
 * @brief ���ڶ�λץȡͼƬdebug�ĳ���
 * 
 */
int  locate_catch_flag = 0;
uint8_t locate_debug_flag;
int locate_put_flag = 0;
uint8_t locate_arr_flag = 0;
void locate_picture_debug(){

	static int begin_flag;
	static int located_n;
	if(!begin_flag){
		Car_Rotate(0);
		begin_flag = 1;
	}
	if(locate_catch_flag){
		TARGET_X = CATCH_X;
		TARGET_Y = CATCH_Y;

		int tick =rt_tick_get();
		MCX_Change_Mode(MCX_Location_Mode);
		while(located_n<30){
			if(MCX_rx_flag){
				Vy = Pos_PID_Controller(&center_y_con,center_y);
				Vx = Pos_PID_Controller(&center_x_con,center_x);
				Car_Change_Speed(Vx,Vy,0);
				located_n = Is_Located? located_n+1:0;
				MCX_Clear();
			}
			if(rt_tick_get()-tick >= 2000)
				break;
		}
		//ץȡ����
			
		Car_Change_Speed(0,0,0);

		Art_Change_Mode(Art_Classify_Mode2);

		while(Art_GetData() == Class_Null);

		servo_slow_ctrl(150, DOWN_MOTOR_INIT_ANGLE, 100);
		rt_thread_delay(100);

		Class_Six_AddOneThing(Art_GetData(),Class_Side);
		rt_kprintf("Classify:the class is %c\n",Art_GetData());

		Art_Change_Mode(Art_Reset_Mode);

		Step_Motor_Catch();
		locate_catch_flag = 0;
		located_n = 0;
	}

	if(locate_put_flag){

	static int located_n;//��¼������λ׼ȷ�Ĵ���
	static int begin_flag;

	TARGET_X = PUT_X;
	TARGET_Y = PUT_Y;
	
	int tick =rt_tick_get();
	while(located_n<1000){
		MCX_Change_Mode(MCX_Location_Mode);
		Vy = Pos_PID_Controller(&center_y_con,center_y);
		Vx = Pos_PID_Controller(&center_x_con,center_x);
		Car_Change_Speed(Vx,Vy,0);
		located_n = Is_Located? located_n+1:0;
		if(rt_tick_get()-tick >= 3000)
			break;
	}

	Car_Change_Speed(0,0,0);

	Art_Change_Mode(Art_NumLetter_Mode0);
	Art_DataClear();

	while(Art_GetData() == Class_Null);

	servo_slow_ctrl(140, 140, 50);
	rt_thread_delay(100);
	int class = Art_GetData();

	Art_Change_Mode(Art_Reset_Mode);
	rt_kprintf("Classify:the num/letter is %c\n",class);

	//while(Class_Six_FinalPut(class));
	// int ret = Class_Six_CirPut(class);

	// for(int i = 0;i<ret;i++){
	// 	servo_slow_ctrl(140, 140, 50);
	// 	rt_thread_delay(100);
	// 	Step_Motor_Put();
	// }
	// Step_Motor_Reset();
		while(Class_Six_FinalPut(class)){
		servo_slow_ctrl(140, 140, 30);
		rt_thread_delay(20);
	}
	Step_Motor_Reset();
		
	located_n = 0;
	locate_put_flag = 0;
	}
	
	if(locate_arr_flag){
		TARGET_X = CATCH_X;
		TARGET_Y = CATCH_Y;
		while(1){
			MCX_Change_Mode(MCX_Location_Mode);
			Vy = Pos_PID_Controller(&center_y_con,center_y);
			Vx = Pos_PID_Controller(&center_x_con,center_x);
			Car_Change_Speed(Vx,Vy,0);
			rt_thread_delay(1);
			if(!locate_arr_flag)
				break;

			if(locate_catch_flag){
				Car_Change_Speed(0,0,0);
				Step_Motor_Catch();
				locate_catch_flag = 0;
			}
			if(locate_put_flag){
				Car_Change_Speed(0,0,0);
				servo_slow_ctrl(140, 140, 50);
				rt_thread_delay(100);
				Step_Motor_Put();
				locate_put_flag = 0;
			}
		}
	}
	rt_thread_delay(1);
}

/**
 * @brief ��λͼƬ���к��� ������ʽ������
 * 
 */
uint8_t num_letter_flag; //���ж�Ϊ������������ĸ
void locate_picture_catch(){

	static int located_n;//��¼������λ׼ȷ�Ĵ���
	static int begin_flag;
	static int fail_to_catch_flag = 0;
	static int error_n;//�����ж��Ƿ�����ʶ��
	static int Is_NumLetter_n;//�����ж��Ƿ�����ĸ��Ƭ
	TARGET_X = CATCH_X;
	TARGET_Y = CATCH_Y;
	if(!begin_flag){
		Car_Rotate(0);
		begin_flag = 1;
		rt_thread_delay(500);
	}
	
	//����ʮ�ζ�λ�ɹ������ץȡ
	
	while(1){
		MCX_Change_Mode(MCX_Location_Mode);
		uint32_t tick = rt_tick_get();
		while(located_n < 50){
			if(MCX_rx_flag){
				error_n = (!cur_PicNum && side_catch_flag)?error_n + 1:0;
				if(error_n == 4){
					error_detect_flag = 1;
					break;
				}	
				located_n = Is_Located? located_n + 1:0;
				Vy = Pos_PID_Controller(&center_y_con,center_y);
				Vx = Pos_PID_Controller(&center_x_con,center_x);
				Car_Change_Speed(Vx,Vy,0);
				MCX_rx_flag = 0;
			}

			if(rt_tick_get() - tick >= 3000)
				break;
		}

		
		if(error_detect_flag&&side_catch_flag)
			break;

		if(cross_handle_flag || circule_handle_flag)
			located_n = 1000;
		else 
			located_n = 0;

		Car_Change_Speed(0,0,0);
		MCX_Clear();
		//����Ǳ���û��ʰȡ�������������
		if(!fail_to_catch_flag){
			//ʶ��
			Art_DataClear();
			Art_Change_Mode(Art_Classify_Mode2);
			while(Art_GetData() == Class_Null);

			servo_slow_ctrl(140, DOWN_MOTOR_INIT_ANGLE, 30);

			if(side_catch_flag)
				Class_Six_AddOneThing(Art_GetData(),Class_Side);
			else if(circule_handle_flag || cross_handle_flag)
				Class_Six_AddOneThing(Art_GetData(),Class_Cir);

			rt_kprintf("Classify:the class is %c\n",Art_GetData());
			Art_Change_Mode(Art_Reset_Mode);
		}

		//ץȡ
		Step_Motor_Catch();
		MCX_Clear();
		rt_thread_delay(50);
		while(!MCX_rx_flag);
		//���񵽿�Ƭ��û�п�Ƭ�� 
		if(cur_PicNum == 0)
			break;
		else if(cur_PicNum&&side_catch_flag){
			fail_to_catch_flag = 1;
			Is_NumLetter_n++;
			rt_kprintf("side: failed to catch\n");
		}

		if(Is_NumLetter_n >= 3){
			rt_kprintf("side: catch letter/num card\n");
			error_detect_flag = 1;
			break;
		}
	}

	MCX_Change_Mode(MCX_Reset_Mode);
	//�����־λ
	begin_flag = 0;
	located_n = 0;
	fail_to_catch_flag = 0;
	error_n = 0;
	Is_NumLetter_n = 0;
	//���ر��ߴ����߳�
	if(side_catch_flag == 1){
		rt_kprintf("task:ready to return to side_catch thread\n");
		side_catch_flag = 0;
		MCX_Detection_Flag = 0;
		rt_sem_release(side_catch_sem);
	}
	//����Բ�������߳�
	if(circule_handle_flag == 1){
		rt_kprintf("task:ready to return to circule_handle thread\n");
		rt_sem_release(circule_handle_sem);
	}
	// ����ʮ�ֳ��߳�
	if(cross_handle_flag == 1){
		rt_kprintf("task:ready to return to cross_handle thread\n");
		rt_sem_release(cross_handle_sem);
	}

		
}

/**
 * @brief �����߳�
 * 
 */
void locate_picture_put(){

	static int located_n;//��¼������λ׼ȷ�Ĵ���
	static int begin_flag;

	TARGET_X = PUT_X;
	TARGET_Y = PUT_Y;
	MCX_Change_Mode(MCX_Location_Mode);
	//����ʱֻ��λx
	uint32_t tick =rt_tick_get();
	while(located_n<1000){
		if(MCX_rx_flag){
			Vx = Pos_PID_Controller(&center_x_con,center_x);
			Vy = Pos_PID_Controller(&center_y_con,center_y);
			Car_Change_Speed(Vx,Vy,0);
			located_n = Is_Located? located_n+1:0;
			MCX_Clear();
		}
			
		if(rt_tick_get()-tick >= 2000)
			break;
	}

	Car_Change_Speed(0,0,0);

	Art_DataClear();
	Art_Change_Mode(Art_NumLetter_Mode0);

	while(Art_GetData() == Class_Null);

	servo_slow_ctrl(140, 140, 30);
	int class = Art_GetData();

	Art_Change_Mode(Art_Reset_Mode);
	rt_kprintf("Classify:the num/letter is %c\n",class);

	rt_kprintf("final_flag:%d,circule_flag:%d\n",final_flag,circule_handle_flag);

	located_n = 0;

	//����
	if(final_flag){
		while(Class_Six_FinalPut(class)){
			servo_slow_ctrl(140, 140, 30);
			rt_thread_delay(20);
		}
		Step_Motor_Reset();
		put_flag = 0;
		rt_kprintf("locate_pic:ready to return to the final_thread\n");
		rt_sem_release(final_sem);
	}
		
	else if(circule_handle_flag){
		rt_kprintf("circule\n");
		int ret = Class_Six_CirPut(class);
		Car_DistanceMotion(10,10,0.4);
		for(int i = 0;i<ret;i++){
			servo_slow_ctrl(140, 140, 30);
			rt_thread_delay(50);
			Step_Motor_Put();
		}
		Step_Motor_Reset();
			
		put_flag = 0;
		rt_kprintf("locate_pic:ready to return to the cir_thread\n");
		rt_sem_release(circule_handle_sem);
	}

	else if(cross_handle_flag){
		rt_kprintf("cross\n");
		int ret = Class_Six_CirPut(class);
		Car_DistanceMotion(20,5,0.3);
		for(int i = 0;i<ret;i++){
			servo_slow_ctrl(140, 140, 30);
			rt_thread_delay(50);	
			Step_Motor_Put();
		}
		Step_Motor_Reset();
			
		put_flag = 0;
		rt_kprintf("locate_pic:ready to return to the cro_thread\n");
		rt_sem_release(cross_handle_sem);
	}
		
	
}

void locate_picture_entry()
{
	while(1)
	{
		if(locate_debug_flag == 0){
			rt_sem_take(locate_picture_sem,RT_WAITING_FOREVER);
			rt_kprintf("task:get into the locate task\n");
			if(!put_flag)
				locate_picture_catch();
			else if(put_flag){
				locate_picture_put();
			}
		}
			
		else
			locate_picture_debug();
	}
}


void locate_pic_init()
{
	rt_kprintf("locate_pic task init\n");
	
	//���Ա�־λ 0---������ 1---����
	locate_debug_flag = 0;
	
	locate_picture_sem = rt_sem_create("locate",0,RT_IPC_FLAG_FIFO);
	if(locate_picture_sem == RT_NULL){
		rt_kprintf("locate_picture_sem created failed\n");
		while(1);
	}

	locate_picture_thread = rt_thread_create("locate",locate_picture_entry,RT_NULL,1024,3,1000);
	rt_thread_startup(locate_picture_thread);
	
	Pos_PID_Init(&center_y_con,-1.3,0,0);
	center_y_con.Output_Max = 100;
	center_y_con.Output_Min = -100;
	center_y_con.Value_I_Max = 500;
	center_y_con.Ref = 145;
	
	Pos_PID_Init(&center_x_con,-1.3,0,0);
	center_x_con.Output_Max = 100;
	center_x_con.Output_Min = -100;
	center_x_con.Value_I_Max = 500;
	center_x_con.Ref = 120;

}
