#include "MCXvision.h"

//MCX 负责定位图片

//包头0x0A 包尾0 
lpuart_transfer_t MCX_receivexfer;
lpuart_handle_t MCX_g_lpuartHandle;
uint8_t MCX_uart_rx_buffer;


//记录当前模式
typedef enum{
    Reset_Mode,
    Detection_Mode,
    Location_Mode,
	Put_Mode
}MCX_Current_Mode;

//此变量用于切换寻线和编写采集线程
uint8_t MCX_Detection_Flag;
MCX_Current_Mode mcxCurrent_Mode;

//此变量用于保存帧
uint8_t MCX_rx_buffer[128];

int16_t center_x;
int16_t center_y;
uint8_t cur_PicNum;

void MCX_uart_callback(LPUART_Type *base, lpuart_handle_t *handle, status_t status, void *userData)
{
	//记录收到几个字节
	static uint8_t count;
	static uint8_t rx_state;

	if(kStatus_LPUART_RxIdle == status)
  	{
		
		if(MCX_uart_rx_buffer == BUFFER_HEAD)
		{
			rx_state = 1;
		}
		else if(MCX_uart_rx_buffer == BUFFER_TAIL)
		{
			MCX_uart_handle();
			rx_state = 0;
			count = 0;
		}
		
		if(rx_state)
		{
			MCX_rx_buffer[count] = MCX_uart_rx_buffer;
			count++;
			if(count > 5)
			{
				rt_kprintf("MCX:buffer overload\n");
				count = 0;
				rx_state = 0;
			}
			
		}
		//rt_kprintf("%d\n",MCX_uart_rx_buffer);
		
	}
	handle->rxDataSize = MCX_receivexfer.dataSize;  
	handle->rxData = MCX_receivexfer.data; 
}

/**
 * @brief MCX切换状态函数
 * 
 * @param mode 状态值 具体见宏定义
 */
void MCX_Change_Mode(uint8_t mode){
	uart_write_byte(MCX_UART,mode);
	switch (mode)
	{
		case 'L':
			mcxCurrent_Mode = Location_Mode;
		break;

		case 'R':
			mcxCurrent_Mode = Reset_Mode;
		break;

		case 'D':
			mcxCurrent_Mode = Detection_Mode;
		break;

		case 'F':
			mcxCurrent_Mode = Put_Mode;
		break;
	}
}

/**
 * @brief 根据模式选择处理数据
 * 
 */
void MCX_uart_handle(){
	switch (mcxCurrent_Mode)
	{
		case Location_Mode:
			center_x = MCX_rx_buffer[2]*1.5;
			center_y = MCX_rx_buffer[3]*1.5;
			// rt_kprintf("%d,%d\n",center_x,center_y);
		break;

		case Reset_Mode:

		break;

		case Detection_Mode:
			if(MCX_rx_buffer[1] == 1){
				center_x = MCX_rx_buffer[2]*1.5;
				center_y = MCX_rx_buffer[3]*1.5;
				MCX_Detection_Flag = 1;
				MCX_Change_Mode(MCX_Reset_Mode);
				rt_kprintf("MCX:(%d,%d)\n",center_x,center_y);
			}
		break;

		case Put_Mode:
			center_x = MCX_rx_buffer[2]*1.5;
			center_y = MCX_rx_buffer[3]*1.5;
			cur_PicNum = MCX_rx_buffer[4];
			rt_kprintf("%d,%d,%d\n",center_x,center_y,cur_PicNum);
	}
}

/**
 * @brief 初始化函数
 * 
 */
void MCX_UART_Init(void)
{
	rt_kprintf("Location and Find Camera Uart Init\n");

	uart_init(MCX_UART, 115200, MCX_UART_TX, MCX_UART_RX);
	NVIC_SetPriority(MCX_UART_IRQn, 1); // 设置串口中断优先级 范围0-15 越小优先级越高
	uart_rx_interrupt(MCX_UART, 1);
	// 配置串口接收的缓冲区及缓冲区长度
	MCX_receivexfer.dataSize = 1;
	MCX_receivexfer.data = &MCX_uart_rx_buffer;
	// 设置中断函数及其参数
	uart_set_handle(MCX_UART, &MCX_g_lpuartHandle, MCX_uart_callback, NULL, 0, MCX_receivexfer.data, 1);

	MCX_Detection_Flag = 0;

	MCX_Change_Mode(MCX_Reset_Mode);
}
