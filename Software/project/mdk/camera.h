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


typedef struct{
    int16 MID_Table[imgRow];
    int16 leftBroder[imgRow];//��߽߱�
    int16 rightBroder[imgRow];//�ұ߽߱�
}RoadLine_t;


extern RoadLine_t Image_S;
extern uint8 my_image[imgRow][imgCol];   
extern uint8 my_image_BW[imgRow][imgCol];
extern int16 Threshold;

extern int Longest_White_Column_Left[2];
extern int Longest_White_Column_Right[2];
extern int White_Column[imgCol];//ÿ�а��г���
extern int Center;

//Ѳ�߲��Է�ʽһ
void Camera_LongestWight(int8_t * my_image);
void Camera_PreProcess(void);
void Camera_FindMidLine(void);
void Vision_Handle();


#endif