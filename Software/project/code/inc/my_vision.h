#ifndef __MY_VISION_H__
#define __MY_VISION_H__

#include "zf_common_headfile.h"
#include "user_math.h"

#define IMAGE_COL 188
#define IMAGE_ROW 70

#define NEAR    0
#define MEDIUM  1
#define FAR     2

#define LEFT_LOSE_VALUE     0
#define RIGHT_LOSE_VALUE    187

#define LEFT_CIRCULE        0
#define RIGHT_CIRCULE       1

//�߶������ж�
typedef enum segment_type{
    NULL_segment        = 0,   //δ���״̬
    lose_segment        = 1,   //����ȱ��
    straight_segment    = 2,   //�ж�Ϊֱ��
    arc_segment         = 3,   //�ж�ΪԲ��
    corner_segment      = 4    //�ж�Ϊ���

}segment_type;

#define IsArcCorner(seg)        (seg.type == arc_segment||seg.type == corner_segment)
#define IsLose(seg)             (seg.type == lose_segment)
#define IsStrai(seg)            (seg.type == straight_segment)
#define IsNull(seg)             (seg.type == NULL_segment)
#define IsArc(seg)              (seg.type == arc_segment)
#define IsCorner(seg)           (seg.type == corner_segment)

//·���ж�
typedef enum{  
    NormalRoads,    //��������
    LoseRoads,      //��ʧ����
    CrossRoads,     //ʮ��
    CirculeRoads,   //Բ��
    CornerRoads,    //���

}RoadSymbol_type;

//�߶��ж�
typedef struct segment{
    int16 begin;
    int16 end;
    segment_type type;
    uint8 position;
}segment_t;

extern int adaptivePara;
extern uint8 handle_image[IMAGE_ROW][IMAGE_COL];
extern RoadSymbol_type Current_Road;
extern uint8_t Cirule_LorR;

//�ж������Ƿ��㹻��
#define Vision_IsLone(seg) (seg.begin - seg.end>=15)

//Ԥ�����Լ�����������һ���õ���
void Vision_GetMyImage();
void adaptiveThreshold(uint8_t* img_data, uint8_t* output_data, int width, int height, int block, uint8_t clip_value);

//����
void Vision_ErrorLogin();
void Vision_BroderPrint();
void Vision_set_AdditionalLine(int16 p1,int16 p2,int16 *broder);

//�ָ�߽�
void Vision_GetSegment(int16* broder,uint8_t LorR);
//��ȡԪ��
void Vision_SymbolJudge();
//�����·Ԫ��
void Vision_RSHandle();
//ʶ��ͼ��������
void Vision_BroderFindFP(int16* broder);
//ʮ�ִ�����
void Vision_CrossHandle();
//Բ��������
void Vision_CirculeHandle();
//�������
void Vision_CornerHandle();
//��ӡ������
void Vision_DrawFP();

#endif