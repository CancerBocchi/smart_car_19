//
// Created by Jay on 2022/7/11.
//
#include "carmove.h"
#include "location.h"
#include "motor.h"
#include "buzzer.h"
#include "attitude_solution.h"
#include "smotor.h"
#include "openart_mini.h"
#include "sendmsg.h"

extern const float PI;
extern uint16 msecond,time_second;
int nextpoint=0;
bool isgetpicture;

int angelTarget;
/*
***************************************************************
*	函 数 名: Charge_Locate
* 功能说明: 坐标判断得到当前的坐标,在行进模式中循环进行？
*	形    参: 无
*	返 回 值: 自身坐标
***************************************************************
*/

void charge_locate(void)
{
    //Car.x=Car.x1;
    //Car.y=Car.y1;
//    //获取当前坐标
//    if (Car.Position_Pointer == 0){ //位于起点1
//        Car.x=0;
//        Car.y=0;
//    }else if(Car.Position_Pointer < locate_sz){
//        Car.x=Car_Location_Route[Car.Position_Pointer-1][0];
//        Car.y=Car_Location_Route[Car.Position_Pointer-1][1];
//    } else if (Car.Position_Pointer==locate_sz){
//        Car.x=Car_Location_Route[locate_sz-1][0];
//        Car.y=Car_Location_Route[locate_sz-1][1];
//    }

/*    Car.x+=Car.MileageX/20;
    Car.y+=Car.MileageY/20;*/
    //修正函数可以放在这后面
}

/*
***************************************************************
*	函 数 名: Get_Target
* 功能说明: 赋予结构体下一个数组元素的坐标,每次行进模式之前使用
*	形    参: 暂无
*	返 回 值: 无
***************************************************************
*/

void get_target() {
    //赋予新的目标坐标点
    /*
    nextpoint=locate_route();
    Car.x1=originMap[nextpoint][0];
    Car.y1=originMap[nextpoint][1];
    */

    if(Car.Position_Pointer<locate_sz){//locate_sz => true_sz改
        get_route();
    }
//    if(Car.Position_Pointer<locate_sz){
//        Car.x1=Car_Location_Route[Car.Position_Pointer][0];
//        Car.y1=Car_Location_Route[Car.Position_Pointer][1];
//    }else if(Car.Position_Pointer==locate_sz){
//        Car.x1=0;
//        Car.y1=0;
//    }

}

/*
***************************************************************
*	函 数 名: Get_Road
* 功能说明: 计算路径：角度和里程
*	形    参: 无
*	返 回 值: 无
***************************************************************
*/

void get_detection(){
    int n=0,dection;
    while(1){
        uart_putstr(USART_1,"1\n");
        if(isDetection){
            dection=picture_type;
            uart_putstr(USART_1,"1\n");
            rt_thread_delay(100);
            if (dection==picture_type) {
                break;
            }
        }else{
            n++;
            Car.Speed_X = picture_xerror_pid(picture_location[0], 78);
            Car.Speed_Y = picture_yerror_pid(picture_location[1], 80);
            //carmove_mileage(1,1);
            if(n>=500){
               break;
            }
        }
        rt_thread_delay(2);
    }
    isDetection=false;
}

void get_location(void){
    charge_locate();
    get_target();
    //用两点式计算角度和距离
    Car.Angel_Target=atan2((Car.x1-Car.x),(Car.y1-Car.y))*180/PI;
    Car.DistanceX=20*(Car.x1-Car.x);
    Car.DistanceY=20*(Car.y1-Car.y);
    //Car.DistanceY=sqrt(Car.DistanceX*Car.DistanceX+Car.DistanceY*Car.DistanceY);
}

void omni_banyun(){
    //车头向前将图片搬运到指定位置
    reset_mileage();
    switch (pictureBigType) {
        case animal:
            angelTarget=90;
            rt_thread_delay(800);
            Car.DistanceY=20*(Car.x-1);
            Car.x=1;
            break;
        case vehicle:
            angelTarget=0;
            rt_thread_delay(800);
            Car.DistanceY=20*(21-Car.y);
            Car.y=22;
            break;
        case fruit:
            angelTarget=-90;
            rt_thread_delay(800);
            Car.DistanceY=20*(26-Car.x);
            Car.x=26;

            break;
        default:break;
    }
    //Car.DistanceY=sqrt(Car.DistanceX*Car.DistanceX+Car.DistanceY*Car.DistanceY);
    carmove_mileage(Car.DistanceX,Car.DistanceY);
    car_stop();
    reset_mileage();
    //这里要改变Car.x,Car.y
}

void search_picture(int tmp){
    switch (tmp) {
        case 0:Car.DistanceX=20;break;
        case 1:Car.DistanceY=-40;break;
        case 2:Car.DistanceY=20;break;
        case 3:Car.DistanceX=-20;break;
        case 4:Car.DistanceX=40;break;
        case 5:Car.DistanceX=-20;break;
    }
    carmove_mileage(Car.DistanceX,Car.DistanceY);
}

void car_recmode(){
    int n=0;
    int tmp=9;

    //根据图片位置改变车身位置,本来打算通过总钻风来识别位置的，但是我太菜了，目前的想法是通过openart来识别图片用过串口发来位置再矫正位置
    while(1){
        uart_putstr(USART_1,"1\n");
        if(havePicture) {
            Car.Speed_X = picture_xerror_pid(picture_location[0], 78);
            Car.Speed_Y = picture_yerror_pid(picture_location[1], 79);
            n=0;
        }else{
            //没有图片，搜索附近,没有则直接退出
            reset_mileage();
//            search_picture(n);
            n++;
            if (n <= tmp * 20) {
                Car.Speed_Y = 5;
            }else if (n <= tmp * 60) {
                Car.Speed_Y = -5;
            }else if (n <= tmp * 80) {
                Car.Speed_Y = 5;
            }else if (n <= tmp * 120) {
                Car.Speed_X = 5;
            }else if (n <= tmp * 140) {
                Car.Speed_X = -5;
            }else if (n <= tmp * 180) {
                Car.Speed_X = 5;
            }else if(n<=tmp*300){
                reset_mileage();
                //无法找到直接下一个吧。。。
                Car.x = Car.x1;
                Car.y = Car.y1;
                break;
            }
        }

        if(Car.Speed_X==0&&Car.Speed_Y==0&&Car.Speed_Z==0){
            reset_mileage();
            rt_thread_delay(200);
            Car.Speed_X=picture_xerror_pid(picture_location[0],78);
            Car.Speed_Y=picture_yerror_pid(picture_location[1],79);

            if(Car.Speed_X==0&&Car.Speed_Y==0&&Car.Speed_Z==0) {
                Car.x=Car.x1;
                Car.y=Car.y1;
                break;
            }
        }
        rt_thread_delay(5);
    }
    get_detection();

    grab_picture();//等待识别完成后电磁铁吸取图片
    //rt_thread_delay(2500);
    rt_mb_send(buzzer_mailbox,233);
    switch (picture_type) {
        case apple:send_msg(USART_8,0,1,(int)Car.x,(int)Car.y,APPLE);break;
        case banana:send_msg(USART_8,0,1,(int)Car.x,(int)Car.y,BANANA);break;
        case durian:send_msg(USART_8,0,1,(int)Car.x,(int)Car.y,DURIAN);break;
        case grape:send_msg(USART_8,0,1,(int)Car.x,(int)Car.y,GRAPE);break;
        case orange:send_msg(USART_8,0,1,(int)Car.x,(int)Car.y,ORANGE);break;
        case cat:send_msg(USART_8,0,1,(int)Car.x,(int)Car.y,CAT);break;
        case cow:send_msg(USART_8,0,1,(int)Car.x,(int)Car.y,CATTLE);break;
        case dog:send_msg(USART_8,0,1,(int)Car.x,(int)Car.y,DOG);break;
        case horse:send_msg(USART_8,0,1,(int)Car.x,(int)Car.y,HORSE);break;
        case pig:send_msg(USART_8,0,1,(int)Car.x,(int)Car.y,PIG);break;
        case bus:send_msg(USART_8,0,1,(int)Car.x,(int)Car.y,COACH);break;
        case car:send_msg(USART_8,0,1,(int)Car.x,(int)Car.y,CAR);break;
        case plane:send_msg(USART_8,0,1,(int)Car.x,(int)Car.y,PLANE);break;
        case ship:send_msg(USART_8,0,1,(int)Car.x,(int)Car.y,STEAMSHIP);break;
        case trains:send_msg(USART_8,0,1,(int)Car.x,(int)Car.y,TRAINS);break;
        default:break;
    }
    omni_banyun();//吸取完成后搬运图片到指定位置
    place_picture();
    angelTarget=0;
    rt_thread_delay(800);
}

void car_return(){
    reset_mileage();
    if(Car.x>15){
        angelTarget=-2;
    }else{
        angelTarget=0;
    }

    speed_tar=20;
    rt_thread_delay(800);
    Car.DistanceY=-20*(Car.y+1);
    Car.DistanceX=-20*(Car.x-1.5f);


    carmove_mileage(Car.DistanceX,Car.DistanceY);
    Car.x=1.5f;
    Car.y=-1;
    reset_mileage();
}

void car_noheadmove(){
    bool isArrive=false;
    angelTarget=-Car.Angel_Target;
    
    if(abs(Car.MileageY) < abs(Car.DistanceY)){
        Car.Speed_Y=speed_tar;
        isArrive=false;
    }else{
        Car.Speed_Y=0;
        isArrive=true;
    }

    if(isArrive){
        reset_mileage();
        car_recmode();
        isArrive=false;
        Car.Position_Pointer++;
        get_location();
        angelTarget=-Car.Angel_Target;
        rt_thread_delay(200);

    }
    rt_thread_delay(1000);
}
void car_omnimove(){
    bool x_flag=false,y_flag=false;
    angelTarget=0;
    if(abs(Car.MileageX)<abs(Car.DistanceX)){
        nextpoint++;
        Car.Speed_X=(speed_tar * sin(Car.Angel_Target/180 *PI));//((float)speed_tar * sin(Car.Angel_Target/180 *PI)),((float)speed_tar * cos(Car.Angel_Target/180 *PI)),0);
        if(Car.Speed_X>-1&&Car.Speed_X<0){
            Car.Speed_X=-1;
        }
        if(Car.Speed_X>0&&Car.Speed_X<1){
            Car.Speed_X=1;
        }
        x_flag=false;
    }else{
        Car.Speed_X=0;
        x_flag=true;
    }
    if(abs(Car.MileageY)<abs(Car.DistanceY)){
        Car.Speed_Y=(speed_tar * cos(Car.Angel_Target/180 *PI));
        if(Car.Speed_Y<0&&Car.Speed_Y>-1){
            Car.Speed_Y=-1;
        }
        if(Car.Speed_Y>0&&Car.Speed_Y<1){
            Car.Speed_Y=1;
        }
        y_flag=false;
    }else{
        Car.Speed_Y=0;
        y_flag=true;
    }

    //Car.Speed_Z=-angel_pid(  Car.Angel,-Car.Angel_Target);//速度环

    if(x_flag && y_flag){
        car_stop();
        reset_mileage();
        car_recmode();

        //rt_mb_send(buzzer_mailbox,1000);
        x_flag=false;
        y_flag=false;
        rt_thread_delay(300);
        get_location();
        Car.MileageX=0;
        Car.MileageY=0;
        Car.Position_Pointer++;
    }
}

void reset_mileage(){
    Car.Speed_X=0;
    Car.Speed_Y=0;
    Car.Speed_Z=0;

    Car.MileageX=0;
    Car.MileageY=0;
    Car.DistanceX=0;
    Car.DistanceY=0;
}


void carmove_mileage(float x,float y){
    Car.DistanceX=x;
    Car.DistanceY=y;
    while(abs(Car.MileageX)<abs(Car.DistanceX)) {
        if (x > 0) {
            Car.Speed_X = speed_tar;
        } else if (x == 0) {
            Car.Speed_X = 0;
        } else if (x < 0) {
            Car.Speed_X = -speed_tar;
        }
        rt_thread_delay(2);
    }
    Car.Speed_X=0;
    car_stop();
    rt_thread_delay(400);
    while(abs(Car.MileageY)< abs(Car.DistanceY)){
        if (y > 0) {
            Car.Speed_Y = speed_tar;
        } else if (y == 0) {
            Car.Speed_Y = 0;
        } else if (y < 0) {
            Car.Speed_Y = -speed_tar;
        }
        rt_thread_delay(2);
    }
    reset_mileage();
}
