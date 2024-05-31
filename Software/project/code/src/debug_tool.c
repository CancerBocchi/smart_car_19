#include "debug_tool.h"
#include "zf_common_headfile.h"

float a = 1.3f;
int c = 1923;

static int arg_count;
static int arr_count;
void debug_tool_init()
{
    //初始化调参命令
    arg_change* p = arg_register;
    while(p->arg!=NULL)
    {
        p++;
        arg_count++;
    }
    //初始化 数组打印命令
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

/**
 * @brief 车辆启动
 *       
 * 
*/
static void  CarStart(){
    Car_Start();
    Car_Change_Speed(0,0,0);
}
MSH_CMD_EXPORT(CarStart , Start The Car);

/**
 * @brief 车辆启动
 *       先将启动置0
 * 
*/
static void  CarStop(){
    Car_Stop();
}
MSH_CMD_EXPORT(CarStop , Stop The Car);

/**
 * @brief 车辆状态获取命令
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
 * @brief 数组打印命令
 *
 *		arrprint arr_name
 *
 *
 */
arg_change arr_register[] ={

    {"leftb",imgRow,&(Image_S.leftBroder)},
    {"rightb",imgRow,&(Image_S.rightBroder)},
    {"midl",imgRow,&(Image_S.MID_Table)},
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
 * @brief 调试参数命令
 * 
 *      setarg show --- 显示注册表
 *      setarg <name> <value> --- 打印对应的值
 */


static int arg_count;

//注册表
arg_change arg_register[] = {
		{"Yaw",DEBUG_FLOAT,&(Car_Yaw_Controller.Ref)},
        {"Vx",DEBUG_FLOAT,&(Car_Speed.Vx)},
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
		{"LoVxKi",DEBUG_FLOAT,&(center_x_con.Ki)},
		{"LoVxKd",DEBUG_FLOAT,&(center_x_con.Kd)},
		{"LoVyKp",DEBUG_FLOAT,&(center_y_con.Kp)},
		{"LoVyKi",DEBUG_FLOAT,&(center_y_con.Ki)},
		{"LoVyKd",DEBUG_FLOAT,&(center_y_con.Ki)},
		{"xv",DEBUG_FLOAT,&(center_x_con.Ref)},
		{"yv",DEBUG_FLOAT,&(center_y_con.Ref)},
        {"catch_flag",DEBUG_INT,&locate_catch_flag},
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


