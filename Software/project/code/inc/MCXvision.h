#ifndef __OPEN_ART_H__
#define __OPEN_ART_H__

#include "zf_common_headfile.h"

#define BUFFER_HEAD 0x0A
#define BUFFER_TAIL 0x0D
#define MCX_Detection_Mode  ('D')
#define MCX_Reset_Mode      ('R')
#define MCX_Location_Mode   ('L')

#define MCX_UART 			UART_3
#define MCX_UART_RX		UART3_RX_B23
#define MCX_UART_TX		UART3_TX_B22
#define MCX_UART_IRQn	LPUART3_IRQn

//记录当前模式
typedef enum{
    Reset_Mode,
    Detection_Mode,
    Location_Mode
}MCX_Current_Mode;

extern uint8_t MCX_Detection_Flag;
extern uint8_t center_x;
extern uint8_t center_y;

void MCX_Change_Mode(uint8_t mode);
void MCX_UART_Init(void);

#endif