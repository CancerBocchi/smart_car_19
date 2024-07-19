#ifndef __FINAL_C__
#define __FINAL_C__

#include "zf_common_headfile.h"

extern int final_flag;

extern rt_sem_t final_sem;
extern rt_thread_t final_thread;

extern uint8_t final_L_or_R;

#define Final_LEFT      0
#define Final_RIGHT     1

void final_init();


#endif // !__FINAL_C__
