#ifndef _ENCODER_H
#define _ENCODER_H

#include "zf_common_headfile.h"

// 编码器位置如图所示
//			 |
//	 1	 |   2
//----------------
//			 |
//	 3	 |   4

extern int32 encoder1;
extern int32 encoder2;
extern int32 encoder3;
extern int32 encoder4;

extern float RC_encoder1,RC_encoder2,RC_encoder3,RC_encoder4;

void encoder_init();
void encoder_getvalue();



#endif