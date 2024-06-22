#include "art.h"

//Art 负责定位图片

//包头0x0A 包尾0 
lpuart_transfer_t Art_receivexfer;
lpuart_handle_t Art_g_lpuartHandle;
uint8_t Art_uart_rx_buffer;

typedef enum{
    Classify_Mode,
    NumLetter_Mode,
    Reset_Mode,
}Art_Mode_t;

//此变量用于保存帧
uint8_t Art_rx_buffer[128];

Art_Mode_t ArtCurrent_Mode;

static uint8_t Art_Data;

void Art_uart_callback(LPUART_Type *base, lpuart_handle_t *handle, status_t status, void *userData)
{
	//记录收到几个字节
	static uint8_t count;
	static uint8_t rx_state;

	if(kStatus_LPUART_RxIdle == status)
    {
		
		if(Art_uart_rx_buffer == BUFFER_HEAD)
		{
			rx_state = 1;
		}
		else if(Art_uart_rx_buffer == BUFFER_TAIL)
		{
            Art_Data = Art_rx_buffer[1];
			count = 0;
            rx_state = 0;
		}
		
		if(rx_state)
		{
			Art_rx_buffer[count] = Art_uart_rx_buffer;
			count++;
			if(count > 4)
			{
				rt_kprintf("Art:buffer overload\n");
				count = 0;
				rx_state = 0;
			}
		}
		
	}
	handle->rxDataSize = Art_receivexfer.dataSize;  
	handle->rxData = Art_receivexfer.data; 
}

/**
 * @brief Art切换状态函数
 * 
 * @param mode 状态值 具体见宏定义
 */
void Art_Change_Mode(uint8_t mode){
	uart_write_byte(Art_UART,mode);
	switch (mode)
	{
		case 'C':
			ArtCurrent_Mode = Classify_Mode;
		break;

		case 'R':
			Art_Data = 0;
			ArtCurrent_Mode = Reset_Mode;
		break;

		case 'N':
			ArtCurrent_Mode = NumLetter_Mode;
		break;
	}
}

/**
 * @brief 根据模式选择处理数据
 * 
 */
void Art_uart_handle(){
	switch (ArtCurrent_Mode)
	{
		case Classify_Mode:

		break;

		case Reset_Mode:

		break;

		case NumLetter_Mode:

		break;
	}
}

/**
 * @brief 获取art数据函数
 * 
 * @return uint8_t art得到的数据
 */
uint8_t Art_GetData(){
    return Art_Data;
}

/**
 * @brief 初始化函数
 * 
 */
void Art_UART_Init(void)
{
	rt_kprintf("Classify Uart Init\n");

	uart_init(Art_UART, 115200, Art_UART_TX, Art_UART_RX);
	NVIC_SetPriority(Art_UART_IRQn, 1); // 设置串口中断优先级 范围0-15 越小优先级越高
	uart_rx_interrupt(Art_UART, 1);
	// 配置串口接收的缓冲区及缓冲区长度
	Art_receivexfer.dataSize = 1;
	Art_receivexfer.data = &Art_uart_rx_buffer;
	// 设置中断函数及其参数
	uart_set_handle(Art_UART, &Art_g_lpuartHandle, Art_uart_callback, NULL, 0, Art_receivexfer.data, 1);
}
