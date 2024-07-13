#ifndef __LOCATE_PICTURE_H__
#define __LOCATE_PICTURE_H__

#include "zf_common_headfile.h"

extern Pos_PID_t center_y_con;
extern Pos_PID_t center_x_con;

extern rt_sem_t locate_picture_sem;
extern int locate_catch_flag;
extern int locate_put_flag;

extern uint8 error_detect_flag;
extern uint8_t put_flag;
extern uint8_t locate_arr_flag;


void locate_pic_init();



#endif