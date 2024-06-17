#include "classify.h"


/** ������ʱ����Ϣ�Ļ������
 * 0 - ��ͨ����
 * 1 - ����
 * 2 - ����
 * 3 - ����
 * 4 - ����
 * 5 - ����
 * Բ�������ʱ�����ν� 5��4��3��2��1 ����ΪԲ�������
 */
static Class_Basket_t Class_Basket[6];
//��¼Բ������ʱ��񵽵ڼ�����Ƭ��
static int Class_CirNum = 5;
//��¼Ŀǰ������ Ϊ 1-6 ���Ҫ���ʶ�Ӧ����Ԫ�� ����Ҫ�� 1
static int current_basket;

/**
 * @brief �ṹ���ʼ��
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
    //���ڿ�ĳ�ʼ��
    current_basket = 1;
    Turntable_Rotate(Class_Basket[current_basket - 1].angle);

}

/**
 * @brief ��С����ֵ������
 * 
 * @return Class_Three_t �����
 */
Class_Three_t Class_ClassifyTheDetailed(int class){
    //�������
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
 * @brief �����������Ʒ����
 * 
 * @param CirculeClass ϸ����
 * @param cir_side_flag Բ�����Ǳ���
 */
void Class_Six_AddOneThing(int DetailClass,int cir_side_flag){
    
    if(cir_side_flag == Class_Cir){
        //����
        if(Class_CirNum == 0){
            rt_kprintf("Fault:Class Cir overload\n");
            while(1);
        }

        //��Բ����Ǻ�
        Class_Basket[Class_CirNum].DetailClass = DetailClass;
        Class_CirNum--;
        Turntable_Rotate(Class_Basket[Class_CirNum].angle);
        current_basket = Class_CirNum+1;
    }

    else if(cir_side_flag == Class_Side){
        //��ȡ����
        int class = Class_ClassifyTheDetailed(DetailClass);
        //ת��ת����Ӧ�Ƕ�
        Turntable_Rotate(Class_Basket[class - 1].angle);
        current_basket = class;
        //��¼����
        Class_Basket[class - 1].howmany++;
    }
    //�ǵ�Բ��ת��λ
    rt_thread_delay(200);
}

/**
 * @brief ���շ��ຯ����������ܻظĳɵ�������ã�
 * 
 * @param FinalClass ʶ�𵽵����
 * @return int ����ʣ�������
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
 * @brief Բ�����ú��������ƿ򣬲��ҽ�ת��ת��λ
 * 
 * @param DetailClass ʶ�𵽿������
 * 
 * @retval �Ƿ���ڷ��� ���� �����Ƿ��Ѿ���� 1--���� 0--������ 2--�����Ѿ����
 */
uint8_t Class_Six_CirPut(int DetailClass){
    //�ж��Ƿ��Ѿ��������
    for(int i = 5;i>Class_CirNum;i--){
        if(Class_Basket[i].DetailClass != Class_Null)
            break;
        if(i == Class_CirNum)
            return 2;
    }   

    //����Ѱ�ҿ�
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
 * @brief ��Բ�������Ľṹ�����ݻָ�
 * 
 */
void Class_Six_CirRest(){
    Class_CirNum = 5;
}


/********************************* ����Ϊ������ʱ������� ***********************************/
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