#include "openart.h"

//art1 负责定位图片

//包头0x0A 包尾0 

lpuart_transfer_t ART1_receivexfer;
lpuart_handle_t ART1_g_lpuartHandle;
uint8_t ART1_uart_rx_buffer;

fifo_struct ART1_FIFO;

//此变量用于切换寻线和编写采集线程
uint8_t Art1_Detection_Flag;

//此变量用于保存帧
uint8_t ART1_rx_buffer[128];

uint8_t center_x;
uint8_t center_y;

void ART1_uart_callback(LPUART_Type *base, lpuart_handle_t *handle, status_t status, void *userData)
{
	//记录收到几个字节
	static uint8_t count;
	static uint8_t rx_state;

	if(kStatus_LPUART_RxIdle == status)
  {
		
		if(ART1_uart_rx_buffer == BUFFER_HEAD)
		{
			rx_state = 1;
		}
		else if(ART1_uart_rx_buffer == BUFFER_TAIL)
		{
			if(ART1_rx_buffer[1])
			{
				center_x = ART1_rx_buffer[2];
				center_y = ART1_rx_buffer[3];
				Art1_Detection_Flag = 1;
//				uart_write_byte(ART1_UART,'E');
			}
			else{
				center_x = 0;
				center_y = 0;
				
			}
				
			rx_state = 0;
			count = 0;
		}
		
		if(rx_state)
		{
			
			ART1_rx_buffer[count] = ART1_uart_rx_buffer;
			count++;
			if(count > 4)
			{
				rt_kprintf("ART1:buffer overload\n");
				count = 0;
				rx_state = 0;
			}
			
		}
		//rt_kprintf("%d\n",ART1_uart_rx_buffer);
		
	}
	handle->rxDataSize = ART1_receivexfer.dataSize;  
	handle->rxData = ART1_receivexfer.data; 
}



void ART1_UART_Init(void)
{
	rt_kprintf("Location and Find Camera Uart Init\n");

	uart_init(ART1_UART, 115200, ART1_UART_TX, ART1_UART_RX);
	NVIC_SetPriority(ART1_UART_IRQn, 1); // 设置串口中断优先级 范围0-15 越小优先级越高
	uart_rx_interrupt(ART1_UART, 1);
	// 配置串口接收的缓冲区及缓冲区长度
	ART1_receivexfer.dataSize = 1;
	ART1_receivexfer.data = &ART1_uart_rx_buffer;
	// 设置中断函数及其参数
	uart_set_handle(ART1_UART, &ART1_g_lpuartHandle, ART1_uart_callback, NULL, 0, ART1_receivexfer.data, 1);

	Art1_Detection_Flag = 0;
}
