#ifndef _camera_h
#define _camera_h
#include "zf_common_headfile.h"
#include "user_math.h"

#define START_X 1
#define START_Y 10
#define BLACK 0
#define WHITE 255
#define  imgRow          70      
#define  imgCol          160
#define MID_COL (imgCol>>1)

typedef enum LineState{
		Line_Straight,	//判断为直线
		Line_Full_lose,	//判断为大半丢失
		Line_Angle_exit,//判断为角点存在	
		Line_Arc_exit,	//判断有圆弧存在
}LineState_t;


typedef struct{
    int16 MID_Table[imgRow];
    int16 leftBroder[imgRow];//左边边界
    int16 rightBroder[imgRow];//右边边界
}RoadLine_t;


extern RoadLine_t Image_S;
extern uint8 my_image[imgRow][imgCol];   
extern uint8 my_image_BW[imgRow][imgCol];
extern int16 Threshold;

extern int Longest_White_Column_Left[2];
extern int Longest_White_Column_Right[2];
extern int White_Column[imgCol];//每列白列长度
extern int Center;

//
void Vision_Handle();
//巡线策略方式一
void Camera_LongestWight();
//预处理
void Camera_PreProcess(void);
//八领域寻线
void Camera_FindMidLine(void);
//大津法
int Camera_My_Adapt_Threshold(uint8*image,uint16 width, uint16 height);


#endif