#ifndef __SIDE_CATCH_H__
#define __SIDE_CATCH_H__

#include "zf_common_headfile.h"

extern rt_sem_t side_catch_sem;
extern rt_thread_t side_catch_thread;

void side_catch_init();

#endif