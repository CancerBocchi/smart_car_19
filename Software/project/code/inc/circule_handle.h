#ifndef __CIRCULE_HANDLE_H__
#define __CIRCULE_HANDLE_H__

#include "zf_common_headfile.h"

#define Circule_Frame1  

extern rt_sem_t circule_handle_sem;
extern uint8_t circule_handle_flag;

void circule_handle_init();

#endif