#include "MCXvision.h"

//MCX ����λͼƬ

//��ͷ0x0A ��β0 
lpuart_transfer_t MCX_receivexfer;
lpuart_handle_t MCX_g_lpuartHandle;
uint8_t MCX_uart_rx_buffer;


//�˱��������л�Ѱ�ߺͱ�д�ɼ��߳�
uint8_t MCX_Detection_Flag;
MCX_Current_Mode mcxCurrent_Mode;

//�˱������ڱ���֡
uint8_t MCX_rx_buffer[128];

int16_t center_x;
int16_t center_y;

void MCX_uart_callback(LPUART_Type *base, lpuart_handle_t *handle, status_t status, void *userData)
{
	//��¼�յ������ֽ�
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
			if(MCX_rx_buffer[1])
			{
				center_x = MCX_rx_buffer[2];
				center_y = MCX_rx_buffer[3];
				MCX_Detection_Flag = 1;
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
			MCX_rx_buffer[count] = MCX_uart_rx_buffer;
			count++;
			if(count > 4)
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
 * @brief MCX�л�״̬����
 * 
 * @param mode ״ֵ̬ ������궨��
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
	}
}

/**
 * @brief ����ģʽѡ��������
 * 
 */
void MCX_uart_handle(){
	switch (mcxCurrent_Mode)
	{
		case Location_Mode:
			center_x = MCX_rx_buffer[2];
			center_y = MCX_rx_buffer[3];
		break;

		case Reset_Mode:

		break;

		case Detection_Mode:
			if(MCX_rx_buffer[0]){
				center_x = MCX_rx_buffer[2];
				center_y = MCX_rx_buffer[3];
				MCX_Detection_Flag = 1;
				MCX_Change_Mode(MCX_Reset_Mode);
			}
		break;
	}
}

/**
 * @brief ��ʼ������
 * 
 */
void MCX_UART_Init(void)
{
	rt_kprintf("Location and Find Camera Uart Init\n");

	uart_init(MCX_UART, 115200, MCX_UART_TX, MCX_UART_RX);
	NVIC_SetPriority(MCX_UART_IRQn, 1); // ���ô����ж����ȼ� ��Χ0-15 ԽС���ȼ�Խ��
	uart_rx_interrupt(MCX_UART, 1);
	// ���ô��ڽ��յĻ�����������������
	MCX_receivexfer.dataSize = 1;
	MCX_receivexfer.data = &MCX_uart_rx_buffer;
	// �����жϺ����������
	uart_set_handle(MCX_UART, &MCX_g_lpuartHandle, MCX_uart_callback, NULL, 0, MCX_receivexfer.data, 1);

	MCX_Detection_Flag = 0;

	MCX_Change_Mode(MCX_Reset_Mode);
}
