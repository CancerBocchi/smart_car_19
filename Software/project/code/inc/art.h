#ifndef __ART_H__
#define __ART_H__

#include "zf_common_headfile.h"

#define Art_UART        UART_5
#define Art_UART_TX     UART5_TX_C28
#define Art_UART_RX     UART5_RX_C29
#define Art_UART_IRQn	LPUART5_IRQn

#define Art_Classify_Mode   'C'
#define Art_Reset_Mode      'S'
#define Art_NumLetter_Mode  'N'

uint8_t Art_GetData();
void Art_Change_Mode(uint8_t mode);
void Art_UART_Init(void);


#endif