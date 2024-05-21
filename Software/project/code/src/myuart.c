#include "myuart.h"

void my_usart_init()
{
	uart_init(UART_2,115200,UART2_TX_B18,UART2_RX_B19);

}