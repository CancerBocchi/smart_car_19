#ifndef __OPEN_ART_H__
#define __OPEN_ART_H__

#include "zf_common_headfile.h"

#define BUFFER_HEAD 0x0A
#define BUFFER_TAIL 0x0D

#define ART1_UART 			UART_3
#define ART1_UART_RX		UART3_RX_B23
#define ART1_UART_TX		UART3_TX_B22
#define ART1_UART_IRQn	LPUART3_IRQn

extern uint8_t Art1_Detection_Flag;
extern uint8_t center_x;
extern uint8_t center_y;

void ART1_UART_Init(void);

#endif