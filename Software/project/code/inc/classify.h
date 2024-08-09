#ifndef __CLASSIFY_H__
#define __CLASSIFY_H__

#include "zf_common_headfile.h"

#define Class_Null          0

#define Class_Cir           1
#define Class_Side          2


#define Turntable_Rotate(angle) Step_angle_con(angle,150)

//��������Ʒ����
typedef enum{
    Class_Traffic_Tool = 3,
    Class_Weapons = 1,
    Class_Supply = 2
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
    float angle;
    int DetailClass;   //ϸ��
    int DetailNum;
    int FinalClass;     //�����
    
}Class_Basket_t;

//������ʹ�ú���
void Class_AddOneThing(uint8_t class,uint8_t circule_or_line);
Class_Info_t* Class_GetLastThing();
void Class_DeleteOneThing();

extern Class_Basket_t Class_Basket[6];
//С���Ϊ����
Class_Three_t Class_ClassifyTheDetailed(int class);
//����������Ʒ
void Class_Six_AddOneThing(int DetailClass,int cir_side_flag);
//���շ���
int Class_Six_FinalPut(int FinalClass);
//Բ������
uint8_t Class_Six_CirPut(int DetailClass);

void Class_Init();

void Class_Debug();

void Class_Cir_Reset();

void Class_Change_Basket(int basketNum);

#endif 