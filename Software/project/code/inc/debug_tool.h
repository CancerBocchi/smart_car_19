#ifndef __DEBUG_TOOL_H__
#define __DEBUG_TOOL_H__

#include "rtthread.h"
#include "finsh.h"
#include "shell.h"
#include "zf_common_headfile.h"

//标记变量类型宏定义
#define DEBUG_FLOAT 	0
#define DEBUG_INT 		1
#define DEBUG_UINT8 	2
#define DEBUG_UINT16 	3


/**
 * @brief 调参命令相关变量
 * 
 */
//conp 在调参中是类型，在打印数组中是长度
typedef struct argu_change
{
    char* name;
    uint8_t conp;
    void* arg;

}arg_change;


extern arg_change arg_register[];

extern arg_change arr_register[];


/**
 * @brief 所有工具初始化
 * 
 */
void debug_tool_init();

#endif