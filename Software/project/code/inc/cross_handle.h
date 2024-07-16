#ifndef __CROSS_HANDLE_H__
#define __CROSS_HANDLE_H__

#include "zf_common_headfile.h"

extern uint8_t cross_handle_flag;
extern uint8_t L_or_R_Cross;
extern rt_sem_t cross_handle_sem;

#define Left_Cross  0
#define Right_Cross 1

void cross_handle_init();

#endif // !__CROSS_HANDLE_H__