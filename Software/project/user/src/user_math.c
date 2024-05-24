#include "user_math.h"
#include "zf_common_headfile.h"

 /**
  * @brief 计算三点的角度 以p2为角的顶点
  * 
  * @param p1 	点 1
  * @param p2 	点 2
  * @param p3 	点 3
  * @return float 角度的cos值
  */
float Vector_AngleGet(point_t p1,point_t p2,point_t p3)
{
	vector vec1 = {p1.x - p2.x,p1.y - p2.y};
	vector vec2 = {p3.x - p2.x,p3.y - p2.y};
	
	float cos_value = Vector_Dot_Product(vec1,vec2)/(Vector_Module(vec1)*Vector_Module(vec2));//计算cos的值		

	return cos_value;
}

/**
 * @brief 计算斜率
 * 
 * @param p1 点1坐标
 * @param p2 点二坐标
 * @return float 斜率
 */
float Point_CalSlope(point_t p1,point_t p2)
{
	return (float)(p1.y-p2.y)/ (float)(p1.x-p2.x);
}

/**
 * @brief 判断直线
 * 		输入两点的横坐标和函数，判断这两点之间是否是直线
 * 		1---直线  0---非直线
 * 		思路：线计算出理想的直线，当点与理想直线的点相差大与一个阈值的时候判定为非直线
 * 
 * @param broder 输入的线的数组，下表为横坐标
 * @param x1 	横坐标 1
 * @param x2 	横坐标 2
 * @return uint8_t 
 */
 int Math_StraightJudgeValue = 10; //阈值
uint8_t Line_IsStraight(int16* broder,int16 x1,int16 x2)
{
	//斜率
	point_t p1 = {x1,broder[x1]};
	point_t p2 = {x2,broder[x2]};
	float K_ =  Line_CalK(p1,p2);

	int16 max = Tool_CmpMax(x1,x2);
	int16 min = Tool_CmpMin(x1,x2);

	int howmany_point = max - min;

	//对应位置的理想横坐标
	int16 target_value;

	for(int i = 0;i<howmany_point;i++)
	{
		target_value = broder[min]+K_*i;
		if(fabsf(broder[i] - target_value) > Math_StraightJudgeValue)
		{
			return NOTSTRAIGHT;
		}

	}
	return ISSTRAIGHT;
	
}

/**
 * @brief 检测单调性
 *		输入两点的横坐标和边界，判断这两点之间的单调性
 *		1---单重单调性  0---双重单调性 
 *		加入了突变点的置信判断，相当于滤除了值变化巨大的点
 * 
 * @param broder 输入判断的线
 * @param x1 	横坐标 1
 * @param x2 	横坐标 2
 * @return uint8_t 
 */
uint8_t Line_IsMonotonous(int16* broder,int16 x1,int16 x2)
{
	int16 df[160];//求导
	int aver_df;//平均导数
	uint8_t add_flag = 0;//单增标志位
	uint8_t dec_flag = 0;//单减标志位

	int16 max = Tool_CmpMax(x1,x2);
	int16 min = Tool_CmpMin(x1,x2);

	int howmany_point = max - min + 1;
	//记录导数值
	for(int i = 0 ; i < howmany_point-1;i++)
	{
		df[i] = broder[max-i] - broder[max-i-1];
		aver_df += df; 
		//因为寻线的点有时候不稳定，所以当单调性突然很大时，不予置信
		if(fabs(df[i]) >= fabs(aver_df/(i+1))+20) //累加平均滤波
			df[i] = df[i-1];
	}
	
	for(int i = 0 ; i < howmany_point-1;i++){
		//对于第一个元素进行判定 判断刚开始是单增还是单减
		if(i == 0){
			add_flag = df[i]>0? 1:0;
			dec_flag = df[i]>0? 0:1;
		}
		//单调性变化
		if(i < howmany_point -3){
			//当发现一个大于零 之后最近的非零值小于零，认为并不单调
			if(df[i]>0){
				int k = 1;
				while(df[i+k] <= 0){
					k++;
					if(df[i+k]<0)
						return MONO_NOEXIST;
					else if(df[i+k]>0||i+k == max)
						break;
				}
			}
			else if(df[i]<0){
				int k = 1;
				while(df[i+k] >= 0){
					k++;
					if(df[i+k]>0)
						return MONO_NOEXIST;
					else if(df[i+k]<0||i+k == max)
						break;
				}
			}

		}
	}
	
	return MONO_EXIST;
}

/**
 * @brief 已知三点坐标 求外接圆半径
 *		  以P2为中心点
 * 
 * @param p1 点 1 坐标
 * @param p2 点 2 坐标
 * @param p3 点 3 坐标
 * @return float 
 */
float Point_GetCurvity(point_t p1,point_t p2,point_t p3)
{
	float cosvalue = Vector_AngleGet(p1,p2,p3);
	float sinvalue = sqrtf(1 - (cosvalue*cosvalue));
	//求解距离
	float distance = Point_GetDistance(p1,p3);
	float r = 2*sinvalue/distance;
}

/**
 * @brief 寻找最大值/最小值点
 * 
 * @param broder 目标函数
 * @param x1 	范围1
 * @param x2 	范围2
 * @return int16 
 */
int16 Line_FindMaxPoint(int16* broder,int x1,int x2)
{
	int min = Tool_CmpMin(x1,x2);
	int max = Tool_CmpMax(x1,x2);

	int max_point = min;

	for(int i = min; i<max ;i++)
	{
		max_point = broder[i]>broder[max_point]? i:max_point;
	}
	return max_point;
}

int16 Line_FindMinPoint(int16 *broder, int x1, int x2)
{
	int min = Tool_CmpMin(x1,x2);
	int max = Tool_CmpMax(x1,x2);

	int min_point = min;

	for(int i = min; i<max ;i++)
	{
		min_point = broder[i]<broder[min_point]? i:min_point;
	}
	return min_point;
}
