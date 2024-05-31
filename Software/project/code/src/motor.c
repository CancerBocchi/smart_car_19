#include "motor.h"

//  
//			 |
//	 1	 |   2
//----------------
//			 |
//	 3	 |   4

//������ٱ�Ϊ70/14���������ٶȺ������ٶȵı���Ϊ70:30


// ������ƿ�
Motor_S Motor_1;
Motor_S Motor_2;
Motor_S Motor_3;
Motor_S Motor_4;


// ���PID���ƿ�
Pos_PID_t Motor_PID_1;
Pos_PID_t Motor_PID_2;
Pos_PID_t Motor_PID_3;
Pos_PID_t Motor_PID_4;


// ���Ŀ���ٶ�
float M1_target_speed;
float M2_target_speed;
float M3_target_speed;
float M4_target_speed;

//-------------------------------------------------------------------------------------------------------------------
//  @brief      ������к���
//  @param      void
//  @return     void
//  @e.g.       ��Ƶ1k
//-------------------------------------------------------------------------------------------------------------------
void motor_run()
{
		gpio_toggle_level(B9);
		encoder_getvalue();
		Motor_Pwm_cb();
		gpio_toggle_level(B9);
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      ����߳����
//  @param      void
//  @return     void
//  @e.g.       ��������ʱ��ʹ��
//-------------------------------------------------------------------------------------------------------------------
void motor_entry()
{
    while(1)
    {
        motor_run();
		rt_thread_delay(1);
    }
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      ����̳߳�ʼ��
//  @param      void
//  @return     void
//  @e.g.       
//-------------------------------------------------------------------------------------------------------------------
rt_thread_t motor_thread;
void motor_thread_init()
{
		motor_thread = rt_thread_create("motor",motor_entry,NULL,4096,1,1000);
    
    if(motor_thread!=NULL)
    {
		//rt_kprintf("motor_thread created successful!");
    	rt_thread_startup(motor_thread);
    }
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      电机用pit中断 1khz
//  @param      void
//  @return     void
//  @e.g.       使用pit ch1
//-------------------------------------------------------------------------------------------------------------------
void motor_pit_init()
{
	
	pit_init(MOTOR_PIT_CHANNEL, 74999);

}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      �����ʼ��
//  @param      void
//  @return     void
//  @e.g.       
//-------------------------------------------------------------------------------------------------------------------
void Motor_init()
{

	rt_kprintf("Motor Init\n");

	encoder_init();

	//pwm��ʼ��
	// m1
	pwm_init(PWM1_MODULE3_CHA_D0, 20000, 0);      				
  	pwm_init(PWM1_MODULE3_CHB_D1, 20000, 0);
	
	Pos_PID_Init(&Motor_PID_1,20,0.15,0);
	Motor_PID_1.Ref = 0;
	Motor_PID_1.Output_Max = 4000;
	Motor_PID_1.Output_Min = -4000;
	Motor_PID_1.Value_I_Max = 4000;
	// m2
	pwm_init(PWM2_MODULE3_CHA_D2, 20000, 0);      				
  	pwm_init(PWM2_MODULE3_CHB_D3, 20000, 0);
	
	Pos_PID_Init(&Motor_PID_2,20,0.15,0);
	Motor_PID_2.Ref = 0;
	Motor_PID_2.Output_Max = 4000;
	Motor_PID_2.Output_Min = -4000;
	Motor_PID_2.Value_I_Max = 4000;
	// m3
	pwm_init(PWM1_MODULE1_CHA_D14, 20000, 0);      				
  	pwm_init(PWM1_MODULE1_CHB_D15, 20000, 0);
	Pos_PID_Init(&Motor_PID_3,20,0.15,0);
	
	Motor_PID_3.Ref = 0;
	Motor_PID_3.Output_Max = 4000;
	Motor_PID_3.Output_Min = -4000;
	Motor_PID_3.Value_I_Max = 4000;
	// m4
	pwm_init(PWM1_MODULE2_CHA_D16, 20000, 0);      				
  	pwm_init(PWM1_MODULE2_CHB_D17, 20000, 0);
	
	Pos_PID_Init(&Motor_PID_4,20,0.15,0);
	Motor_PID_4.Ref = 0;
	Motor_PID_4.Output_Max = 4000;
	Motor_PID_4.Output_Min = -4000;
	Motor_PID_4.Value_I_Max = 4000;


#if MOTOR_LOOP_METHOD == MOTOR_THREAD

	motor_thread_init();
#elif MOTOR_LOOP_METHOD == MOTOR_PIT

	motor_pit_init();
#endif 

}



//-------------------------------------------------------------------------------------------------------------------
//  @brief      ����ջ�
//  @param      void
//  @return     void
//  @e.g.       Ƶ��1k����
//-------------------------------------------------------------------------------------------------------------------
void Motor_Pwm_cb()
{
	//����1000Ϊÿ�����е��ٶ�
	Motor_1.Act_Speed = RC_encoder1 * 1000 / 1024 * 3.14  * 6.1;
	Motor_2.Act_Speed = RC_encoder2 * 1000 / 1024 * 3.14  * 6.1;
	Motor_3.Act_Speed = RC_encoder3 * 1000 / 1024 * 3.14  * 6.1;
	Motor_4.Act_Speed = RC_encoder4 * 1000 / 1024 * 3.14  * 6.1;

	Motor1_Set_Pwm( (int)Pos_PID_Controller(&Motor_PID_1,Motor_1.Act_Speed) );
	Motor2_Set_Pwm( (int)Pos_PID_Controller(&Motor_PID_2,Motor_2.Act_Speed) );
	Motor3_Set_Pwm( (int)Pos_PID_Controller(&Motor_PID_3,Motor_3.Act_Speed) );
	Motor4_Set_Pwm( (int)Pos_PID_Controller(&Motor_PID_4,Motor_4.Act_Speed) );
	
	//rt_kprintf("%.2f,%.2f,%.2f,%.2f\n",Motor_1.Act_Speed,Motor_2.Act_Speed,Motor_3.Act_Speed,Motor_4.Act_Speed);
}


float Slew_Func(float *slewVal, float refVal, float slewRate)
{
  static float diff = 0;
  diff = refVal - *slewVal;
  if (diff >= slewRate)
  {
    *slewVal += slewRate;
    return (1);
  } else if (-diff >= slewRate)
  {
    *slewVal -= slewRate;
    return (-1);
  } else
  {
    *slewVal = refVal;
    return (0);
  }
}

void Motor1_Set_Pwm(int pwm)
{
	Slew_Func(&Motor_PID_1.Ref,M1_target_speed,MOTOR_SOFTBOOT_RATE);
	
	pwm_set_duty(PWM_CH3_A,5000 - pwm);
	pwm_set_duty(PWM_CH3_B,5000 + pwm);
	
//	DATA_Space_0.data0 = Motor_1.Act_Speed;
//	DATA_Space_0.data1 = pwm;
}

void Motor2_Set_Pwm(int pwm)
{
	Slew_Func(&Motor_PID_2.Ref,M2_target_speed,MOTOR_SOFTBOOT_RATE);
	
	pwm_set_duty(PWM_CH4_A,5000 + pwm);
	pwm_set_duty(PWM_CH4_B,5000 - pwm);
	
//	DATA_Space_0.data2 = Motor_2.Act_Speed;
//	DATA_Space_0.data3 = pwm;
}

void Motor3_Set_Pwm(int pwm)
{
	Slew_Func(&Motor_PID_3.Ref,M3_target_speed,MOTOR_SOFTBOOT_RATE);
	

	pwm_set_duty(PWM_CH1_A,5000 - pwm);
	pwm_set_duty(PWM_CH1_B,5000 + pwm);
	
//	DATA_Space_0.data4 = Motor_3.Act_Speed;
//	DATA_Space_0.data5 = pwm;
}

void Motor4_Set_Pwm(int pwm)
{
	Slew_Func(&Motor_PID_4.Ref,M4_target_speed,MOTOR_SOFTBOOT_RATE);
	
	pwm_set_duty(PWM_CH2_A,5000 + pwm);
	pwm_set_duty(PWM_CH2_B,5000 - pwm);

	
//	DATA_Space_0.data6 = Motor_4.Act_Speed;
//	DATA_Space_0.data7 = pwm;
}


void Motor_Set_Speed(uint8_t Motor_CH,float target_speed)
{
	if(Motor_CH == 1)
	{
		M1_target_speed = target_speed;
	}
	else if(Motor_CH == 2)
	{
		M2_target_speed = target_speed;
	}
	else if(Motor_CH == 3)
	{
		M3_target_speed = target_speed;
	}
	else if(Motor_CH == 4)
	{
		M4_target_speed = target_speed;
	}
}


