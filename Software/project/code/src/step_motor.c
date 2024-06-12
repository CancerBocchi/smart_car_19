#include "step_motor.h"

static uint16 servo1_duty = 0, servo2_duty = 0;
//用于控制转盘的舵机
static uint16 step_duty = 0;

int catch_flag = 0;

rt_thread_t *S_Motor_test_Thread;

void servo_slow_ctrl(uint16 _servo1_angle, uint16 _servo2_angle, float _step_count);

void Catch_Entry()
{
		while(1)
		{
			if(!catch_flag)
			{
				Step_Motor_Catch();
				Step_Motor_Catch();
				Step_Motor_Catch();
				Step_Motor_Catch();
					
				gpio_set_level(B10,1);
//			rt_thread_delay(1000);
	//		
				Step_Motor_Put();
				Step_Motor_Put();
				Step_Motor_Put();
					
				Step_Motor_Reset();
				
				catch_flag = 1;
				//servo_slow_ctrl(120, 180, 100);
			}
			
			rt_thread_delay(1);
		}
}

void Step_Motor_Init()
{
	rt_kprintf("Catch Arm Init\n");
	pwm_init(DOWN_MOTOR_CON_PIN, STEP_MOTOR_FRE, (uint32)SERVO_MOTOR_DUTY(DOWN_MOTOR_INIT_ANGLE));   
	pwm_init(UP_MOTOR_CON_PIN, STEP_MOTOR_FRE, (uint32)SERVO_MOTOR_DUTY(UP_MOTOR_INIT_ANGLE));
	
	gpio_init(B10, GPO, 0, GPO_PUSH_PULL);
	
	servo1_duty = UP_MOTOR_INIT_ANGLE;servo2_duty = DOWN_MOTOR_INIT_ANGLE;
	
	S_Motor_test_Thread = rt_thread_create("S_motor",Catch_Entry,NULL,1024,2,1000);
	
	// servo_slow_ctrl(0, 18, 100);
#if ARM_DEBUG_SWITCH == 1
		if(S_Motor_test_Thread!=NULL)
			rt_thread_startup(S_Motor_test_Thread);
		
		else
			rt_kprintf("SMotor create failed\n");
#endif
			
}

void Step_Motor_Reset()
{
		servo_slow_ctrl(UP_MOTOR_INIT_ANGLE, DOWN_MOTOR_INIT_ANGLE, 100);
}

void Step_Motor_Catch()
{
//		if(servo1_duty != UP_MOTOR_INIT_ANGLE || servo2_duty != UP_MOTOR_INIT_ANGLE)
//		{
//				Step_Motor_Reset();
//		}
		rt_thread_delay(100);
		servo_slow_ctrl(30, 20, 100);
		rt_thread_delay(100);
		servo_slow_ctrl(5, 20, 50);
		gpio_set_level(B10,1);
		rt_thread_delay(400);
		servo_slow_ctrl(80, 50, 100);
		rt_thread_delay(90);
		servo_slow_ctrl(80, 95, 50);
		rt_thread_delay(50);
		servo_slow_ctrl(90, 160, 100);
		rt_thread_delay(90);
		servo_slow_ctrl(140, 160, 100);
		rt_thread_delay(200);
		gpio_set_level(B10,0);
		rt_thread_delay(100);
		Step_Motor_Reset();
		
}

void Step_Motor_Put()
{
		rt_thread_delay(100);
		gpio_set_level(B10,1);
		servo_slow_ctrl(150, 150, 100);
		rt_thread_delay(200);
		servo_slow_ctrl(120, 150, 50);
		rt_thread_delay(50);
		servo_slow_ctrl(20, 20, 100);
//		rt_thread_delay(100);
//		servo_slow_ctrl(20,20,100);
		rt_thread_delay(100);
		gpio_set_level(B10,0);

}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     舵机连续控制函数
// 参数说明     _servo1_angle               舵机1的目标角度
// 参数说明     _servo2_angle               舵机2的目标角度
// 返回参数     _step_count                 舵机连续控制间隔次数
// 使用示例     servo_slow_ctrl(90, 90, 100);
// 备注信息     
//-------------------------------------------------------------------------------------------------------------------
void servo_slow_ctrl(uint16 _servo1_angle, uint16 _servo2_angle, float _step_count)
{
    float servo1_start = (float)servo1_duty, servo2_start = (float)servo2_duty;
    float servo1_step = (float)(_servo1_angle - servo1_duty)/_step_count, servo2_step = (float)(_servo2_angle - servo2_duty)/_step_count;
    while(1)
    {
        system_delay_ms(5);
        if(fabsf(servo1_start - (float)_servo1_angle) >= servo1_step)servo1_start += servo1_step;
        else servo1_start = _servo1_angle;
        pwm_set_duty(DOWN_MOTOR_CON_PIN, (uint32)SERVO_MOTOR_DUTY((uint16)servo1_start));
        
        if(fabsf(servo2_start - (float)_servo2_angle) >= servo2_step)servo2_start += servo2_step;
        else servo2_start = _servo2_angle;
        pwm_set_duty(UP_MOTOR_CON_PIN, (uint32)SERVO_MOTOR_DUTY((uint16)servo2_start));
        
        if(fabsf(servo1_start - (float)_servo1_angle) < 1 && fabsf(servo2_start - (float)_servo2_angle) < 1)
        {
            servo1_duty = (uint16)_servo1_angle;
            servo2_duty = (uint16)_servo2_angle;
            return;
        }
    }
}

/**
 * @brief 用于控制转盘舵机
 * 
 * @param target_angle 
 * @param count 
 */
void Step_angle_con(uint16_t target_angle,int count){
	float servo1_start = (float)step_duty;
    float servo1_step = (float)(target_angle - step_duty)/count;
    while(1)
    {
        system_delay_ms(5);
        if(fabsf(servo1_start - (float)target_angle) >= servo1_step)servo1_start += servo1_step;
        else servo1_start = target_angle;
        pwm_set_duty(DOWN_MOTOR_CON_PIN, (uint32)SERVO_MOTOR_DUTY((uint16)servo1_start));
        
        if(fabsf(servo1_start - (float)target_angle) < 1){
            step_duty = (uint16)target_angle;
            return;
        }
    }

}