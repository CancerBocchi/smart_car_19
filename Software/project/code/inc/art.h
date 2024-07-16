#ifndef __ART_H__
#define __ART_H__

#include "zf_common_headfile.h"

#define Art_UART        UART_5
#define Art_UART_TX     UART5_TX_C28
#define Art_UART_RX     UART5_RX_C29
#define Art_UART_IRQn	LPUART5_IRQn

#define Art_Classify_Mode0      (0x43)
#define Art_Classify_Mode2      (0x41)

#define Art_NumLetter_Mode0     (0x4D)
#define Art_NumLetter_Mode1     (0X4E)
#define Art_NumLetter_Mode2     (0x4F)

#define Art_Reset_Mode          (0x53)

uint8_t Art_GetData();
void Art_Change_Mode(uint8_t mode);
void Art_UART_Init(void);
void Art_DataClear();


#endif