#include "circule_handle.h"

//Բ�����ĵ�ͼƬ�Եľ���
#define Circule_Distance1
#define Circule_Normal_xSpeed 100

static int cirucle_xspeed;

//�ź������ƿ�
rt_sem_t circule_handle_sem;
//�߳̿��ƿ�
rt_thread_t circule_handle_thread;
//���߶�λ�̷߳���circule_handle_flag�߳�
uint8_t circule_handle_flag = 0;
//Ѱ���� PID
Pos_PID_t circule_Trace_Con_Omega;
Pos_PID_t circule_Trace_Con_Vy;

void circule_trace_line();


void circule_handle_entry(){
    // static uint8_t begin_flag = 0;
    // static float init_yaw;
    // while(1){
    //     rt_sem_take(circule_handle_sem,RT_WAITING_FOREVER);
    //     rt_kprintf("task:get into the circule task\n");
    //     //��ʼ����
    //     if(!begin_flag){
    //         //�õ���ʼ�Ƕ�
    //         init_yaw = Att_CurrentYaw;
    //         //ת��
    //         if(Circule_LorR == LEFT_CIRCULE)
    //             Car_Rotate(90);
    //         else 
    //             Car_Rotate(-90);
    //         //����ʱ��ʹ�ó�ת��λ
    //         rt_thread_delay(500);
    //         //��λ��
    //         Car_DistanceMotion(0,-20,0.4);

    //     }

    //     // rt_thread_delay(2000);
    //     //������λץȡ�߳�
    //     rt_kprintf("task:start to catch things\n");
    //     circule_handle_flag = 1;
    //     rt_sem_release(locate_picture_sem);
    //     rt_sem_take(circule_handle_sem,RT_WAITING_FOREVER);
    //     rt_kprintf("task:return to the circule_handle thread\n");

    //     //ץȡ��� ��ת180�ȣ��ȷ�����÷��õĶ���
    //     Car_Rotate(180);
    //     rt_thread_delay(1000);
    //     Car_DistanceMotion(0,-30,0.8);
    //     //ʶ������
    //     Art_Change_Mode(Art_NumLetter_Mode);
    //     rt_thread_delay(10);
    //     Class_Six_CirPut(Art_GetData());
    //     Step_Motor_Put();
    //     //ת���һص�Բ������
    //     if(Circule_LorR == LEFT_CIRCULE){
    //         //��ת
    //         Car_Rotate(90);
    //         //����ʱ��ʹ�ó�ת��λ
    //         rt_thread_delay(500);
    //         Car_DistanceMotion(-30,0,0.8);
    //         // Car_DistanceMotion(30,-30,0.8);
    //     }
    //     else{
    //         //��ת
    //         Car_Rotate(-90);
    //         //����ʱ��ʹ�ó�ת��λ
    //         rt_thread_delay(500);
    //         Car_DistanceMotion(30,0,0.8);
    //         // Car_DistanceMotion(-30,-30,0.8);
    //     }
    //     //Ѳ�߷���
    //     while(1){
    //         circule_trace_line();
    //         //�ﵽĿ��Ƕ�
    //         if(Tool_IsMultiple(Att_GetYaw() - init_yaw,60,1.0f)){
    //             Car_Change_Speed(0,0,0);
    //             //��ǰ�ƶ����ÿ�Ƭ
    //             Car_DistanceMotion(0,-30,0.8);
    //             //art ʶ��
    //             Art_Change_Mode(Art_NumLetter_Mode);
    //             rt_thread_delay(10);
    //             //����
    //             int ret = Class_Six_CirPut(Art_GetData());
    //             //�Ѿ��������
    //             if(ret == 2)
    //                 break;
    //             //��û��û�и���
    //             else if(ret == 1)
    //                 continue;
    //             //�и��࣬����
    //             Step_Motor_Put();
    //         }
    //     }
    //     float Delta_Yaw = fabs(init_yaw - Att_GetYaw());
    //     //ת����ʼ�Ƕ�
    //     Car_Speed_ConRight = Con_By_AngleLoop;
    //     Car_Change_Yaw(init_yaw);
    //     //��Բ�ͬ�ĽǶȶ����ƶ�
    //     if(Tool_IsFloatEqu(Delta_Yaw,60,1.0f)){
    //         if(Circule_LorR == LEFT_CIRCULE)
    //             Car_DistanceMotion(30,-30,1);
    //         else
    //             Car_DistanceMotion(-30,-30,1);
    //     }
    //     else if(Tool_IsFloatEqu(Delta_Yaw,120,1.0f)){
    //         if(Circule_LorR == LEFT_CIRCULE)
    //             Car_DistanceMotion(60,-40,3);
    //         else
    //             Car_DistanceMotion(-60,-40,3);
    //     }
    //     else if(Tool_IsFloatEqu(Delta_Yaw,180,1.0f)){
    //         if(Circule_LorR == LEFT_CIRCULE)
    //             Car_DistanceMotion(30,-50,2);
    //         else
    //             Car_DistanceMotion(-30,-50,2);
    //     }
    //     else if(Tool_IsFloatEqu(Delta_Yaw,0,1.0f)){
    //         if(Circule_LorR == LEFT_CIRCULE)
    //             Car_DistanceMotion(30,-10,1);
    //         else
    //             Car_DistanceMotion(-30,-10,1);
    //     }
        
    //     //����Ѳ���߳�
    //     Car_Speed_ConRight = Con_By_TraceLine;
    //     rt_sem_release(trace_line_sem);

    //}
}

/**
 * @brief Բ��Ѳ��
 * 
 */
void circule_trace_line(){
    //���ٶȿ���Ȩ�޽���Բ��
    Car_Speed_ConRight = Con_By_Circule;
    //ɨ��
    int line[IMAGE_COL];
    Camera_CirculeFindLine(my_image,line);

    // //�������
    // float current_error;
    // for(int i = 60;i<128;i++)
    //     current_error += line[i] - 35;
    // current_error /= 68.0f;

    // //���Բ���ٶ�Ӧ�������ұ�Բ���ٶ�Ӧ������
    // cirucle_xspeed = Circule_LorR? Circule_Normal_xSpeed : -Circule_Normal_xSpeed;

    // //PID�ջ�����
    // Car_Change_Speed(cirucle_xspeed,
    //                 Pos_PID_Controller(&circule_Trace_Con_Vy,current_error),
    //                 Pos_PID_Controller(&circule_Trace_Con_Omega,current_error));

}

/**
 * @brief Բ�������ʼ������
 * 
 */
void circule_handle_init(){

    rt_kprintf("circule handle task init\n");
	circule_handle_sem = rt_sem_create("circule_handle_sem",0,RT_IPC_FLAG_FIFO);
	if(circule_handle_sem == RT_NULL){
		rt_kprintf("circule_handle_sem created failed\n");
		while(1);
	}
	circule_handle_thread = rt_thread_create("side_catch",circule_handle_entry,RT_NULL,1024,3,1000);
	if(circule_handle_thread == RT_NULL){
        rt_kprintf("circule_handle_thread created failed\n");
        while(1);
    }

	rt_thread_startup(circule_handle_thread);

    Pos_PID_Init(&circule_Trace_Con_Omega,0,0,0);
    circule_Trace_Con_Omega.Output_Max = 50;
    circule_Trace_Con_Omega.Output_Min = -50;
    circule_Trace_Con_Omega.Value_I_Max = 200;
    circule_Trace_Con_Omega.Ref = 0;

    Pos_PID_Init(&circule_Trace_Con_Vy,0,0,0);
    circule_Trace_Con_Vy.Output_Max = 50;
    circule_Trace_Con_Vy.Output_Min = -50;
    circule_Trace_Con_Vy.Value_I_Max = 200;
    circule_Trace_Con_Vy.Ref = 0;

}