#include "classify.h"


/** 六分类时框信息的基本情况
 * 0 - 交通工具
 * 1 - 武器
 * 2 - 物资
 * 3 - 待定
 * 4 - 待定
 * 5 - 待定
 */
static Class_Basket_t Class_Basket[6];

//单分类时候的信息
static Class_Info_t info_of_things[20];
//目前的物品个数
int HowMany;

/**
 * @brief 记录一个物品的信息(单分类时候)
 * 
 * @param class 对应的类
 * @param circule_or_line 边线还是圆环捡起的
 * 
 */
void Class_AddOneThing(uint8_t class,uint8_t circule_or_line){
    
    info_of_things[HowMany].what = class;
    //大类分类
    if(class == Class_Ambulance||class == Class_ArmoredCar||
        class == Class_FireTruck||class == Class_Motorcycle)
        info_of_things[HowMany].in_witch_class = Class_Traffic_Tool;
    
    else if(class == Class_Boom||class == Class_Gun||class == Class_Batons
            || class == Class_Knife||Class_FireAxe)
        info_of_things[HowMany].in_witch_class = Class_Weapons;

    else if(class == Class_FirstAid||class == Class_Flashlight||
            class == Class_WalkieTalk||class == Class_Binoculars||
            class == Class_Helmet||class == Class_Vest)
        info_of_things[HowMany].in_witch_class = Class_Supply;

    //分类自加
    HowMany++;
}

/**
 * @brief 获取最近的一个物品的信息
 * 
 * @return Class_Info_t* 最近物品信息的结构体指针
 */
Class_Info_t* Class_GetLastThing(){
    return &info_of_things[HowMany];
}

/**
 * @brief 放下一个变量后删除该变量
 * 
 */
void Class_DeleteOneThing(){  
    HowMany = HowMany == 0?0:HowMany-1;
}