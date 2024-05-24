#include "user_math.h"
#include "zf_common_headfile.h"

 /**
  * @brief ��������ĽǶ� ��p2Ϊ�ǵĶ���
  * 
  * @param p1 	�� 1
  * @param p2 	�� 2
  * @param p3 	�� 3
  * @return float �Ƕȵ�cosֵ
  */
float Vector_AngleGet(point_t p1,point_t p2,point_t p3)
{
	vector vec1 = {p1.x - p2.x,p1.y - p2.y};
	vector vec2 = {p3.x - p2.x,p3.y - p2.y};
	
	float cos_value = Vector_Dot_Product(vec1,vec2)/(Vector_Module(vec1)*Vector_Module(vec2));//����cos��ֵ		

	return cos_value;
}

/**
 * @brief ����б��
 * 
 * @param p1 ��1����
 * @param p2 �������
 * @return float б��
 */
float Point_CalSlope(point_t p1,point_t p2)
{
	return (float)(p1.y-p2.y)/ (float)(p1.x-p2.x);
}

/**
 * @brief �ж�ֱ��
 * 		��������ĺ�����ͺ������ж�������֮���Ƿ���ֱ��
 * 		1---ֱ��  0---��ֱ��
 * 		˼·���߼���������ֱ�ߣ�����������ֱ�ߵĵ�������һ����ֵ��ʱ���ж�Ϊ��ֱ��
 * 
 * @param broder ������ߵ����飬�±�Ϊ������
 * @param x1 	������ 1
 * @param x2 	������ 2
 * @return uint8_t 
 */
 int Math_StraightJudgeValue = 10; //��ֵ
uint8_t Line_IsStraight(int16* broder,int16 x1,int16 x2)
{
	//б��
	point_t p1 = {x1,broder[x1]};
	point_t p2 = {x2,broder[x2]};
	float K_ =  Line_CalK(p1,p2);

	int16 max = Tool_CmpMax(x1,x2);
	int16 min = Tool_CmpMin(x1,x2);

	int howmany_point = max - min;

	//��Ӧλ�õ����������
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
 * @brief ��ⵥ����
 *		��������ĺ�����ͱ߽磬�ж�������֮��ĵ�����
 *		1---���ص�����  0---˫�ص����� 
 *		������ͻ���������жϣ��൱���˳���ֵ�仯�޴�ĵ�
 * 
 * @param broder �����жϵ���
 * @param x1 	������ 1
 * @param x2 	������ 2
 * @return uint8_t 
 */
uint8_t Line_IsMonotonous(int16* broder,int16 x1,int16 x2)
{
	int16 df[160];//��
	int aver_df;//ƽ������
	uint8_t add_flag = 0;//������־λ
	uint8_t dec_flag = 0;//������־λ

	int16 max = Tool_CmpMax(x1,x2);
	int16 min = Tool_CmpMin(x1,x2);

	int howmany_point = max - min + 1;
	//��¼����ֵ
	for(int i = 0 ; i < howmany_point-1;i++)
	{
		df[i] = broder[max-i] - broder[max-i-1];
		aver_df += df; 
		//��ΪѰ�ߵĵ���ʱ���ȶ������Ե�������ͻȻ�ܴ�ʱ����������
		if(fabs(df[i]) >= fabs(aver_df/(i+1))+20) //�ۼ�ƽ���˲�
			df[i] = df[i-1];
	}
	
	for(int i = 0 ; i < howmany_point-1;i++){
		//���ڵ�һ��Ԫ�ؽ����ж� �жϸտ�ʼ�ǵ������ǵ���
		if(i == 0){
			add_flag = df[i]>0? 1:0;
			dec_flag = df[i]>0? 0:1;
		}
		//�����Ա仯
		if(i < howmany_point -3){
			//������һ�������� ֮������ķ���ֵС���㣬��Ϊ��������
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
 * @brief ��֪�������� �����Բ�뾶
 *		  ��P2Ϊ���ĵ�
 * 
 * @param p1 �� 1 ����
 * @param p2 �� 2 ����
 * @param p3 �� 3 ����
 * @return float 
 */
float Point_GetCurvity(point_t p1,point_t p2,point_t p3)
{
	float cosvalue = Vector_AngleGet(p1,p2,p3);
	float sinvalue = sqrtf(1 - (cosvalue*cosvalue));
	//������
	float distance = Point_GetDistance(p1,p3);
	float r = 2*sinvalue/distance;
}

/**
 * @brief Ѱ�����ֵ/��Сֵ��
 * 
 * @param broder Ŀ�꺯��
 * @param x1 	��Χ1
 * @param x2 	��Χ2
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
