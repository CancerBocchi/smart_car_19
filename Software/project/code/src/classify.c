#include "classify.h"


/** 六分类时框信息的基本情况
 * 0 - 交通工具
 * 1 - 武器
 * 2 - 物资
 * 3 - 待定
 * 4 - 待定
 * 5 - 待定
 * 圆环处理的时候依次将 5，4，3，2，1 定义为圆环分类框
 */
static Class_Basket_t Class_Basket[6];
//记录圆环分类时候捡到第几个卡片了
static int Class_CirNum = 5;
//记录目前的篮筐 为 1-6 如果要访问对应数组元素 则需要减 1
static int current_basket;

/**
 * @brief 结构体初始化
 * 
 */
void Class_Init(){

    Class_Basket[0].FinalClass = Class_Traffic_Tool;
    Class_Basket[1].FinalClass = Class_Weapons;
    Class_Basket[2].FinalClass = Class_Supply;

    Class_Basket[0].angle = 23.6;
    Class_Basket[1].angle = 60+23.6;
    Class_Basket[2].angle = 120+23.6;
    Class_Basket[3].angle = 180+23.6;
    Class_Basket[4].angle = 240+23.6;
    Class_Basket[5].angle = 300+23.6;
    //对于框的初始化
    current_basket = 1;
    Turntable_Rotate(Class_Basket[current_basket - 1].angle);

}

/**
 * @brief 将小分类分到大分类
 * 
 * @return Class_Three_t 大分类
 */
Class_Three_t Class_ClassifyTheDetailed(int class){
    //大类分类
    if(class == Class_Ambulance||class == Class_ArmoredCar||
        class == Class_FireTruck||class == Class_Motorcycle)
        return Class_Traffic_Tool;
    
    else if(class == Class_Boom||class == Class_Gun||class == Class_Batons
            || class == Class_Knife||Class_FireAxe)
        return Class_Weapons;

    else if(class == Class_FirstAid||class == Class_Flashlight||
            class == Class_WalkieTalk||class == Class_Binoculars||
            class == Class_Helmet||class == Class_Vest)
        return Class_Supply;
}

/**
 * @brief 六分类添加物品函数
 * 
 * @param CirculeClass 细分类
 * @param cir_side_flag 圆环还是边线
 */
void Class_Six_AddOneThing(int DetailClass,int cir_side_flag){
    
    if(cir_side_flag == Class_Cir){
        //出错
        if(Class_CirNum == 0){
            rt_kprintf("Fault:Class Cir overload\n");
            while(1);
        }

        //将圆环标记好
        Class_Basket[Class_CirNum].DetailClass = DetailClass;
        Class_CirNum--;
        Turntable_Rotate(Class_Basket[Class_CirNum].angle);
        current_basket = Class_CirNum+1;
    }

    else if(cir_side_flag == Class_Side){
        //获取大类
        int class = Class_ClassifyTheDetailed(DetailClass);
        //转盘转到对应角度
        Turntable_Rotate(Class_Basket[class - 1].angle);
        current_basket = class;
        //记录数量
        Class_Basket[class - 1].howmany++;
    }
    //是的圆环转到位
    rt_thread_delay(200);
}

/**
 * @brief 最终分类函数（后面可能回改成电磁铁放置）
 * 
 * @param FinalClass 识别到的类别
 * @return int 框内剩余的数量
 */
int Class_Six_FinalPut(int FinalClass){
    if(current_basket != FinalClass){

        Turntable_Rotate(Class_Basket[FinalClass - 1].angle);
        current_basket = FinalClass;

        rt_thread_delay(200);
    }
    Class_Basket[FinalClass - 1].howmany--;

    return Class_Basket[FinalClass - 1].howmany--;
}

/**
 * @brief 圆环放置函数，重制框，并且将转盘转到位
 * 
 * @param DetailClass 识别到框的类型
 * 
 * @retval 是否存在分类 并且 分类是否已经完成 1--存在 0--不存在 2--分类已经完成
 */
uint8_t Class_Six_CirPut(int DetailClass){
    //判断是否已经分类完成
    for(int i = 5;i>Class_CirNum;i--){
        if(Class_Basket[i].DetailClass != Class_Null)
            break;
        if(i == Class_CirNum)
            return 2;
    }   

    //倒序寻找框
    for(int i = 5;i>Class_CirNum;i--){
        if(Class_Basket[i].DetailClass == DetailClass){
            Turntable_Rotate(Class_Basket[i].angle);
            current_basket = i+1;

            rt_thread_delay(200);
            Class_Basket[i].DetailClass = Class_Null;
            return 1;
        }
    }
    return 0;
}

/**
 * @brief 将圆环分类后的结构体数据恢复
 * 
 */
void Class_Six_CirRest(){
    Class_CirNum = 5;
}


/********************************* 以下为单分类时候的情形 ***********************************/
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