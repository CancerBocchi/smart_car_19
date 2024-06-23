#ifndef __CONFIG_H__
#define __CONFIG_H__

//
//电机相关配置
//

/**
 * @brief   电机�?�?选项
 * 
 *  MOTOR_THREAD 0	        使用线程�?�?
 *  MOTOR_PIT    1	        使用定时器中�?�?�?
 *  MOTOR_USE_CAR_LOOP      使用上层�?�?
 */

#define MOTOR_LOOP_METHOD MOTOR_USE_CAR_LOOP 

/**
 * @brief 电机�?�?通道
 *          此通道仅仅影响电机初�?�化
 *          代码运�?�位�?需要自己向�?�?�?添加函数
 */
#define MOTOR_PIT_CHANNEL PIT_CH1

/**
 * @brief 电机�?起动斜率
 *          
 */
#define MOTOR_SOFTBOOT_RATE 1.0f

/**
 * @brief MOTOR PWM 通道函数
 * 
 */



//
//车运动相关配�?
//

/**
 * @brief   车辆�?�?选项
 * 
 *      CAR_USE_PIT			1	  使用定时器中�?
 *      CAR_USE_THREAD 	0	  使用线程
 *  
 */
 
#define CAR_USE_THREAD 	0
#define CAR_USE_PIT			1

#define CAR_LOOP_METHOD CAR_USE_THREAD
/**
 * @brief 车辆�?�?通道
 *          此通道仅仅影响电机初�?�化 
 *          代码运�?�位�?需要自己向�?�?�?添加函数
 */
#define CAR_PIT_CHANNEL PIT_CH1

/**
 * @brief 车辆使用角度�?�?
 *          1-----使能
 *          0-----禁用
 */
#define CAR_USE_ANGLE_LOOP 0 

#define CAR_TRACELINE_VFORWARD 100


//debug 相关

#define VOFA_LPUART_CHANNEL USART_2

#define VOFA_LPUART_TX_PIN  UART2_TX_B18
#define VOFA_LPUART_RX_PIN  UART2_RX_B19

//
//机�?�臂相关
//

/**
* @brief 机�?�臂调试选项 
*          1-----使能 将会单独为机械臂开一�?线程
 *          0-----禁用
 */
#define ARM_DEBUG_SWITCH 0

#endif // !__CONFIG_H__