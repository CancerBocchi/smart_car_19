#ifndef __TRACE_LINE_H__
#define __TRACE_LINE_H__

#include "zf_common_headfile.h"


#define LAST_OFFSET_POINT_NUM 14 

extern float speed_forward;

extern Pos_PID_t TraceLine_Yaw_Con;
extern Pos_PID_t TraceLine_Vx_Con;
extern int32 TraceLine_Aver_Offset;
extern int32 TraceLine_Last_Offset;
extern int32 TraceLine_Forward_V;
extern float mid_offset;
extern rt_sem_t trace_line_sem;

void trace_line_entry();
void trace_line_init();


#endif