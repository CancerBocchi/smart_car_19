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

//
void Vision_Handle();
//Ѳ�߲��Է�ʽһ
void Camera_LongestWight();
//Ԥ����
void Camera_PreProcess(void);
//������Ѱ��
void Camera_FindMidLine(void);
//���
int Camera_My_Adapt_Threshold(uint8*image,uint16 width, uint16 height);


#endif