#include "debug_tool.h"
#include "zf_common_headfile.h"

float a = 1.3f;
int c = 1923;

static int arg_count;
static int arr_count;
void debug_tool_init()
{
    //��ʼ����������
    arg_change* p = arg_register;
    while(p->arg!=NULL)
    {
        p++;
        arg_count++;
    }
    //��ʼ�� �����ӡ����
    p = arr_register;
    while(p->arg!=NULL)
    {
        p++;
        arr_count++;
    }


}




static arg_change* match_arg(char* str,arg_change* register_,int arg_count)
{
    for(int i = 0;i<arg_count;i++)
    {
        if(!rt_strcmp(register_[i].name,str))
        {
            return register_+i;
        }
    }
    return NULL;
}

static void ClassState(){
	Class_Debug();
}
MSH_CMD_EXPORT(ClassState , View Class Meg);

/**
 * �ı�MCX��ģʽ
*/
static void MCXMode(int argc, char**argv){
	if(argc<3 && *argv[1] == MCX_Location_Mode || 
        *argv[1] == MCX_Reset_Mode || *argv[1] == MCX_Put_Mode
        ||*argv[1] == MCX_Detection_Mode){
            MCX_Change_Mode(*argv[1]);
            rt_kprintf("MCX:Mode Changed %c\n",*argv[1]);
    }
}
MSH_CMD_EXPORT(MCXMode , chang MCX Mode);

/**
 * @brief ��������
 *       
 * 
*/
static void  CarStart(){
	extern int Start_Flag;
    Start_Flag = 1;
}
MSH_CMD_EXPORT(CarStart , Start The Car);

/**
 * @brief ��������
 *       �Ƚ�������0
 * 
*/
static void  CarStop(){
    Car_Stop();
}
MSH_CMD_EXPORT(CarStop , Stop The Car);

/**
 * @brief ����״̬��ȡ����
 *        GetRS
 * 
*/
static void GetRS(void)
{
    switch(Current_Road)
    {
        case LoseRoads:
            rt_kprintf("CurrentRod:LoseRoads\n");
            break;

        case NormalRoads:
            rt_kprintf("CurrentRod:NormalRoads\n");
            break;

        case CrossRoads:
            rt_kprintf("CurrentRod:CrossRoads\n");
            break;

        case CirculeRoads:
            rt_kprintf("CurrentRod:CirculeRoads\n");
            break;

        case CornerRoads:
            rt_kprintf("CurrentRod:CornerRoads\n");
            break;

        default:
            Vision_ErrorLogin();
            Vision_BroderPrint();
            break;
    }
}

MSH_CMD_EXPORT(GetRS , Get Current RoadState);

/**
 * @brief �����ӡ����
 *
 *		arrprint arr_name
 *
 *
 */
arg_change arr_register[] ={

    {"leftb",imgRow,&(Image_S.leftBroder)},
    {"rightb",imgRow,&(Image_S.rightBroder)},
    {"midl",imgRow,&(Image_S.MID_Table)},
    {"cir",imgCol,&(cir_line)},
    {NULL,NULL,NULL}
};

void print_arr(arg_change* addr,int type)
{
		int16* p = addr->arg;
		for(int i = 0;i<addr->conp;i++)
		{
			rt_kprintf("%d\n",*(p+i));
		}
}

static void arrprint(int argc, char**argv)
{
	switch(argc)
	{
		case 1:
			goto help;
		break;
			
		case 2:
			if(!rt_strcmp("show",argv[1]))
			{
					rt_kprintf("|      name      |\n");
					for(int i = 0;i<arr_count;i++)
					{
							rt_kprintf("|%-16s|\n",arr_register[i].name);
					}
			}
			else
			{
					arg_change* p;
					p = match_arg(argv[1],arr_register,arr_count);
					if(p!=NULL)
					{
						print_arr(p,0);
					}
					else
						goto help;
			}
		break;
		
		default:
			goto help;
		break;
			
	};
	
	help:
    rt_kprintf("you can use like this:\n");
    rt_kprintf("arrprint <_target_>  -----   printf target content in an array\n");
		rt_kprintf("arrprint show        -----   show the array in the register\n");
    rt_kprintf("arrprint help        -----   get help information\n");
	return;

}
MSH_CMD_EXPORT(arrprint, arrprint sample: arrprint <_target_>);

/**
 * @brief ���Բ�������
 * 
 *      setarg show --- ��ʾע���
 *      setarg <name> <value> --- ��ӡ��Ӧ��ֵ
 */


static int arg_count;

//ע���
arg_change arg_register[] = {
		{"Yaw",DEBUG_FLOAT,&(Car_Yaw_Controller.Ref)},
        {"Vx",DEBUG_FLOAT,&(cirucle_xspeed)},
        {"Vy",DEBUG_FLOAT,&(speed_forward)},
		{"VxKp",DEBUG_FLOAT,&(TraceLine_Vx_Con.Kp)},
		{"VxKi",DEBUG_FLOAT,&(TraceLine_Vx_Con.Ki)},
		{"YawKp",DEBUG_FLOAT,&(TraceLine_Yaw_Con.Kp)},
		{"YawKi",DEBUG_FLOAT,&(TraceLine_Yaw_Con.Ki)},
		{"YawKd",DEBUG_FLOAT,&(TraceLine_Yaw_Con.Kd)},
		{"AngleKp",DEBUG_FLOAT,&(Car_Yaw_Controller.Kp)},
		{"AngleKd",DEBUG_FLOAT,&(Car_Yaw_Controller.Kd)},
		{"adPara",DEBUG_INT,&(adaptivePara)},
		{"Car_Switch",DEBUG_INT,&(Car_BootSwitch)}, 
		{"LoVxKp",DEBUG_FLOAT,&(center_x_con.Kp)},
		{"LoVyKp",DEBUG_FLOAT,&(center_y_con.Kp)},
        {"CirYawKp",DEBUG_FLOAT,&(circule_Trace_Con_Omega.Kp)},
        {"CirVyKp",DEBUG_FLOAT,&(circule_Trace_Con_Vy.Kp)},
		{"xv",DEBUG_FLOAT,&(center_x_con.Ref)},
		{"yv",DEBUG_FLOAT,&(center_y_con.Ref)},
		{"catch_flag",DEBUG_INT,&locate_catch_flag},
        {"put_flag",DEBUG_INT,&locate_put_flag},
        {"loc_arr",DEBUG_INT,&locate_arr_flag},
        {"ex_time",DEBUG_INT,&exposure_time},
        {"heap",DEBUG_INT,&Heap_Flag},
        {"line_f",DEBUG_INT,&cir_or_main_line},
		{NULL,NULL,NULL}
};

static void setarg(int argc, char**argv)
{
    switch (argc)
    {
    case 1:
        goto help;
        break;
    case 2:
        if(!rt_strcmp("show",argv[1]))
        {
            rt_kprintf("|      name      |      value     |\n");
            for(int i = 0;i<arg_count;i++)
            {
                rt_kprintf("|%-16s|",arg_register[i].name);
                
                switch(arg_register[i].conp)
                {
                case DEBUG_FLOAT:
                    rt_kprintf("%-16.6f|\n",*(float*)arg_register[i].arg);
                    
                    break;
                case DEBUG_INT:
                    rt_kprintf("%-16d|\n",*(int*)arg_register[i].arg);
                    
                    break;
                }
            }
        }
        else
            goto help;
        break;

    case 3:
	{
        arg_change* p;
        p = match_arg(argv[1],arg_register,arg_count);
        if(p!=NULL)
        {
            switch (p->conp)
            {
            case DEBUG_FLOAT:
				*(float*)p->arg = atof(argv[2]);
				rt_kprintf("setarg: arg %s have been changed to %f !\n",p->name,*(float*)p->arg);
                break;
            case DEBUG_INT:
                *(int*)p->arg = atoi(argv[2]);
				rt_kprintf("setarg: arg %s have been changed to %d !\n",p->name,*(int*)p->arg);
                break;
            }
            
        }
	}
        break;
    
    default:
        goto help;
        break;
    }
	return;

help:
    rt_kprintf("you can use like this:\n");
    rt_kprintf("setarg show             ----- show the arg in the register \n");
    rt_kprintf("setarg <target> <value> ----- change the value of the target\n");
    rt_kprintf("setarg help             ----- get help information\n");
	return;
}

MSH_CMD_EXPORT(setarg, setarg sample: setarg <_target_> <number>);


/**
 * @brief ��������ָ��
*/
static void CarMove(int argc, char**argv){

    if(argc == 4){
        float dx = atof(argv[1]);
        float dy = atof(argv[2]);
        float dt = atof(argv[3]);

        Car_DistanceMotion(dx,dy,dt);
    }
    else 
        goto help;

help:
    rt_kprintf("you can use like this:\n");
    rt_kprintf("CarMove 100 0 1        ----- x move 100, y move 0 in 1s\n");
	return;
}

MSH_CMD_EXPORT(CarMove, CarMove sample: CarMove <dx> <dy> <dt>);


/**
 * @brief Ӳ�����Գ���
*/
static void HardwareTest(){
    rt_kprintf("Motion test begins\n");
    Car_Change_Speed(100,0,0);
    rt_thread_delay(500);
    Car_Change_Speed(-100,0,0);
    rt_thread_delay(500);

    rt_kprintf("Step Motor test begins\n");
    Step_Motor_Catch();

    rt_kprintf("Electromagnet test begins\n");
    Magnet_Catch;
    rt_thread_delay(1000);
    Magnet_Put;

    rt_kprintf("Buzzer test begins\n");
    BUZZER_SPEAK;
    rt_thread_delay(500);

    rt_kprintf("Hardware test ends\n");

}

MSH_CMD_EXPORT(HardwareTest, HardwareTest sample: HardwareTest);


/**
 * @brief ��������ָ��
*/
static void SetBasket(int argc, char**argv){
    int num = atoi(argv[1]);
    if(atoi(argv[1]) < 7 && atoi(argv[1]) > 0){
        Class_Change_Basket(num);
        rt_kprintf("Current Basket Changed to %d\n",num);
        return;
    }
     rt_kprintf("Error input:%d\n",num);
        
}

MSH_CMD_EXPORT(SetBasket, SetBasket sample: SetBasket <num>);


static void SetFinal(int argc, char**argv){
    if(argc == 2){
        if(*argv[1] == 'l')
            final_L_or_R = Final_LEFT;
        else if(*argv[1] == 'r')
            final_L_or_R = Final_RIGHT;

    }
   
}

MSH_CMD_EXPORT(SetFinal, SetFinal sample: SetFinal <l/r>);