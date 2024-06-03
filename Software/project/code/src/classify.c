#include "classify.h"


/** ������ʱ����Ϣ�Ļ������
 * 0 - ��ͨ����
 * 1 - ����
 * 2 - ����
 * 3 - ����
 * 4 - ����
 * 5 - ����
 */
static Class_Basket_t Class_Basket[6];

//������ʱ�����Ϣ
static Class_Info_t info_of_things[20];
//Ŀǰ����Ʒ����
int HowMany;

/**
 * @brief ��¼һ����Ʒ����Ϣ(������ʱ��)
 * 
 * @param class ��Ӧ����
 * @param circule_or_line ���߻���Բ�������
 * 
 */
void Class_AddOneThing(uint8_t class,uint8_t circule_or_line){
    
    info_of_things[HowMany].what = class;
    //�������
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

    //�����Լ�
    HowMany++;
}

/**
 * @brief ��ȡ�����һ����Ʒ����Ϣ
 * 
 * @return Class_Info_t* �����Ʒ��Ϣ�Ľṹ��ָ��
 */
Class_Info_t* Class_GetLastThing(){
    return &info_of_things[HowMany];
}

/**
 * @brief ����һ��������ɾ���ñ���
 * 
 */
void Class_DeleteOneThing(){  
    HowMany = HowMany == 0?0:HowMany-1;
}