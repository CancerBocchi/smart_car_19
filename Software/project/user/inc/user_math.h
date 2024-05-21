#ifndef __USER_MATH_H__
#define __USER_MATH_H__

#include "stdint.h"
#include "math.h"

//������
#define MONO_EXIST		1
#define MONO_NOEXIST	0

#define ISSTRAIGHT		1
#define NOTSTRAIGHT		0

#define int16 short 

typedef struct point{
	int16 x;
	int16 y;
}point_t;

//�����ṹ��
typedef point_t vector;
//��ģ
#define Vector_Module(vec1)					(sqrt(vec1.x*vec1.x+vec1.y*vec1.y))
//�ڻ�
#define Vector_Dot_Product(vec1,vec2) 		(vec1.x*vec2.x+vec1.y*vec2.y)
//�����
#define Point_GetDistance(p1,p2)			(sqrtf((float)((p1.x - p2.x)*(p1.x-p2.x)+(p1.y - p2.y)*(p1.y-p2.y))))
//����б��
#define Line_CalK(p1,p2)					((float)(p1.y - p2.y)/(float)(p1.x - p2.x))

//�Ƚϴ�С
#define Tool_CmpMax(num1,num2)				(num1>num2?num1:num2)
#define Tool_CmpMin(num1,num2)				(num1>num2?num2:num1)

//��cos�Ƕ�
float Vector_AngleGet(point_t p1,point_t p2,point_t p3);
//��б��
float Point_CalSlope(point_t p1,point_t p2);
//�жϵ�����
uint8_t Line_IsMonotonous(int16* broder,int16 x1,int16 x2);
//Ѱ�Ҽ�ֵ��
int16 Line_FindMaxPoint(int16* broder,int x1,int x2);
int16 Line_FindMinPoint(int16* broder,int x1,int x2);


#endif