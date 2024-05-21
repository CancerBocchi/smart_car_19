#ifndef _camera_h
#define _camera_h
#include "zf_common_headfile.h"
#include "user_math.h"

#define  imgRow          70      
#define  imgCol          160
#define MID_COL (imgCol>>1)

typedef enum LineState{
		Line_Straight,	//判断为直线
		Line_Full_lose,	//判断为大半丢失
		Line_Angle_exit,//判断为角点存在	
		Line_Arc_exit,	//判断有圆弧存在
}LineState_t;

//元素标志
typedef enum feature_flag
{
		left_cirque_flag,
		right_cirque_flag,
		straight_flag,
		cross_flag,
		turn_flag,
}feature_flag;

typedef struct {
		float pitch;
		float row;
		float yaw;
}_eulerAngle;

typedef struct{
    int16 MID_Table[imgRow];
    int16 leftBroder[imgRow];//左边边界
    int16 rightBroder[imgRow];//右边边界
}ImageStruct;

typedef struct{
	
	uint8_t* 				left_point[10];
	uint8_t 				left_line_state;
	LineState_t 		leftLine_State;
	uint8_t         left_pointnum;
	
	uint8_t*				right_point[10];
	uint8_t 				right_line_state;
	LineState_t 		rightLine_State;
	uint8_t         right_pointnum;
		
}feature_t;

extern int rrrrrr;
extern int16 MID_Table[imgCol];
extern ImageStruct Image_S;
extern uint8 my_image[imgRow][imgCol];   
extern int16 Threshold;

extern void UseImage(void);
void ReadMyImage(void);

void FindMidLine(void);


#endif