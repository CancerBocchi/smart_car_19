#ifndef __CLASSIFY_H__
#define __CLASSIFY_H__

#include "zf_common_headfile.h"

#define Class_Null          0

#define Class_Cir           1
#define Class_Side          2


#define Turntable_Rotate(angle) Step_angle_con(angle,150)

//三大类物品分类
typedef enum{
    Class_Traffic_Tool = 3,
    Class_Weapons = 1,
    Class_Supply = 2
}Class_Three_t;

//十五类物品分类
typedef enum{

    Class_Gun           =   'a',    //枪
    Class_Boom          =   'b',    //炸药
    Class_Knife         =   'c',    //匕首
    Class_Batons        =   'd',    //警棍
    Class_FireAxe       =   'e',    //消防斧

    Class_FirstAid      =   'f',    //急救包
    Class_Flashlight    =   'g',    //手电筒
    Class_WalkieTalk    =   'h',    //对讲机
    Class_Vest          =   'i',    //防弹背心
    Class_Binoculars    =   'j',    //望远镜
    Class_Helmet        =   'k',    //头盔

    Class_FireTruck     =   'l',    //消防车
    Class_Ambulance     =   'm',    //救护车
    Class_ArmoredCar    =   'n',    //装甲车
    Class_Motorcycle    =   'o',    //摩托车

}Class_Fifteen_t;

//存储物品信息的结构体
typedef struct{
    //分类标记
    Class_Fifteen_t what;
    Class_Three_t   in_witch_class;
    //分类结构接口--决定放在哪个分类框里
    int class_position;
    //应当为圆环还是最后的
    uint8_t circule_or_final;
    

}Class_Info_t;

//框的相关信息
typedef struct{
    int howmany;        //框内数量
    float angle;
    int DetailClass;   //细分
    int DetailNum;
    int FinalClass;     //大分类
    
}Class_Basket_t;

//单分类使用函数
void Class_AddOneThing(uint8_t class,uint8_t circule_or_line);
Class_Info_t* Class_GetLastThing();
void Class_DeleteOneThing();

extern Class_Basket_t Class_Basket[6];
//小类分为大类
Class_Three_t Class_ClassifyTheDetailed(int class);
//像框中添加物品
void Class_Six_AddOneThing(int DetailClass,int cir_side_flag);
//最终分类
int Class_Six_FinalPut(int FinalClass);
//圆环分类
uint8_t Class_Six_CirPut(int DetailClass);

void Class_Init();

void Class_Debug();

void Class_Cir_Reset();

void Class_Change_Basket(int basketNum);

#endif 