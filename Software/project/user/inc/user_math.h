#ifndef __USER_MATH_H__
#define __USER_MATH_H__

#include "stdint.h"
#include "math.h"

//单调性
#define MONO_EXIST		1
#define MONO_NOEXIST	0

#define ISSTRAIGHT		1
#define NOTSTRAIGHT		0

#define int16 short 

typedef struct point{
	int16 x;
	int16 y;
}point_t;

//向量结构体
typedef point_t vector;
//求模
#define Vector_Module(vec1)					(sqrt(vec1.x*vec1.x+vec1.y*vec1.y))
//内积
#define Vector_Dot_Product(vec1,vec2) 		(vec1.x*vec2.x+vec1.y*vec2.y)
//求距离
#define Point_GetDistance(p1,p2)			(sqrtf((float)((p1.x - p2.x)*(p1.x-p2.x)+(p1.y - p2.y)*(p1.y-p2.y))))
//计算斜率
#define Line_CalK(p1,p2)					((float)(p1.y - p2.y)/(float)(p1.x - p2.x))

//比较大小
#define Tool_CmpMax(num1,num2)				(num1>num2?num1:num2)
#define Tool_CmpMin(num1,num2)				(num1>num2?num2:num1)

//求cos角度
float Vector_AngleGet(point_t p1,point_t p2,point_t p3);
//求斜率
float Point_CalSlope(point_t p1,point_t p2);
//判断单调性
uint8_t Line_IsMonotonous(int16* broder,int16 x1,int16 x2);
//寻找极值点
int16 Line_FindMaxPoint(int16* broder,int x1,int x2);
int16 Line_FindMinPoint(int16* broder,int x1,int x2);


#endif