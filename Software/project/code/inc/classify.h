#ifndef __CLASSIFY_H__
#define __CLASSIFY_H__

#include "zf_common_headfile.h"



//��ı��
#define Class_Basket1    0
#define Class_Basket2    60
#define Class_Basket3    120
#define Class_Basket4    180
#define Class_Basket5    240
#define Class_Basket6    300 

#define Class_Null 0

//��������Ʒ����
typedef enum{
    Class_Traffic_Tool = 1,
    Class_Weapons,
    Class_Supply
}Class_Three_t;

//ʮ������Ʒ����
typedef enum{

    Class_Gun           =   'a',    //ǹ
    Class_Boom          =   'b',    //ըҩ
    Class_Knife         =   'c',    //ذ��
    Class_Batons        =   'd',    //����
    Class_FireAxe       =   'e',    //������

    Class_FirstAid      =   'f',    //���Ȱ�
    Class_Flashlight    =   'g',    //�ֵ�Ͳ
    Class_WalkieTalk    =   'h',    //�Խ���
    Class_Vest          =   'i',    //��������
    Class_Binoculars    =   'j',    //��Զ��
    Class_Helmet        =   'k',    //ͷ��

    Class_FireTruck     =   'l',    //������
    Class_Ambulance     =   'm',    //�Ȼ���
    Class_ArmoredCar    =   'n',    //װ�׳�
    Class_Motorcycle    =   'o',    //Ħ�г�

}Class_Fifteen_t;

//�洢��Ʒ��Ϣ�Ľṹ��
typedef struct{
    //������
    Class_Fifteen_t what;
    Class_Three_t   in_witch_class;
    //����ṹ�ӿ�--���������ĸ��������
    int class_position;
    //Ӧ��ΪԲ����������
    uint8_t circule_or_final;

}Class_Info_t;

//��������Ϣ
typedef struct{
    int howmany;        //��������
    int CirculeClass;   //ϸ��
    int FinalClass;     //�����
    
}Class_Basket_t;

//������ʹ�ú���
void Class_AddOneThing(uint8_t class);
Class_Info_t* Class_GetLastThing();
void Class_DeleteOneThing();


#endif 