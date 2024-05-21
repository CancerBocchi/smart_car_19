#include "vofa.h"

DATA_S DATA_Space_0;
DATA_S DATA_Space_1;

//void vofa_send_data_init()
//{
//	uart_init(VOFA_LPUART_CHANNEL,115200,VOFA_LPUART_TX_PIN,VOFA_LPUART_RX_PIN);
//}

void vofa_send_data(DATA_S *DATA)
{
	uint8_t data_buff[44];
	uint8_t data_point = 0;
	float d_buff;
	
	d_buff = (float)DATA->data0;
	memcpy(data_buff + data_point, &d_buff, sizeof(float));
	data_point+=4;
	
	d_buff = (float)DATA->data1;
	memcpy(data_buff + data_point, &d_buff, sizeof(float));
	data_point+=4;
	
	d_buff = (float)DATA->data2;
	memcpy(data_buff + data_point, &d_buff, sizeof(float));
	data_point+=4;
	
	d_buff = (float)DATA->data3;
	memcpy(data_buff + data_point, &d_buff, sizeof(float));
	data_point+=4;
	
	d_buff = (float)DATA->data4;
	memcpy(data_buff + data_point, &d_buff, sizeof(float));
	data_point+=4;
	
	d_buff = (float)DATA->data5;
	memcpy(data_buff + data_point, &d_buff, sizeof(float));
	data_point+=4;
	
	d_buff = (float)DATA->data6;
	memcpy(data_buff + data_point, &d_buff, sizeof(float));
	data_point+=4;
	
	d_buff = (float)DATA->data7;
	memcpy(data_buff + data_point, &d_buff, sizeof(float));
	data_point+=4;
	
	d_buff = (float)DATA->data8;
	memcpy(data_buff + data_point, &d_buff, sizeof(float));
	data_point+=4;
	
	d_buff = (float)DATA->data9;
	memcpy(data_buff + data_point, &d_buff, sizeof(float));
	data_point+=4;
	
	data_buff[40] = 0x00;
	data_buff[41] = 0x00;
	data_buff[42] = 0x80;
	data_buff[43] = 0x7f;
	
	//uart_putbuff(VOFA_LPUART_CHANNEL,data_buff,sizeof(data_buff));
}