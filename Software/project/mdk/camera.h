#ifndef _camera_h
#define _camera_h
#include "zf_common_headfile.h"
#include "user_math.h"

#define  imgRow          70      
#define  imgCol          160
#define MID_COL (imgCol>>1)

typedef enum LineState{
		Line_Straight,	//�ж�Ϊֱ��
		Line_Full_lose,	//�ж�Ϊ��붪ʧ
		Line_Angle_exit,//�ж�Ϊ�ǵ����	
		Line_Arc_exit,	//�ж���Բ������
}LineState_t;

//Ԫ�ر�־
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
    int16 leftBroder[imgRow];//��߽߱�
    int16 rightBroder[imgRow];//�ұ߽߱�
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