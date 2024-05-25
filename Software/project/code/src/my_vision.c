#include "my_vision.h"
#include "zf_device_tft180.h"
#include "string.h"

int block = 7;
int adaptivePara = 6;

RoadSymbol_type Current_Road;

//·�������жϽṹ��
struct{
    segment_t my_segment_L[10];
    point_t feature_p_L[5];
    int FP_n_L;
    int segment_n_L;

    segment_t my_segment_R[10];
    point_t feature_p_R[5];
    int FP_n_R;
    int segment_n_R;
}F;

/**
 * @brief ��������Ԫ��ѡ����
 * 
 */
void Vision_RSHandle()
{
    switch (Current_Road)
    {
    case LoseRoads:
        //Car_Stop();
        //while(1){
            //error handle �Ѳ����� ����errorhandle
       // }
        break;

    case NormalRoads: 

        break;

    case CrossRoads:
        Vision_CrossHandle();
        break;
    
    case CirculeRoads:
        Vision_CirculeHandle();
        break;

    case CornerRoads:
        Vision_CornerHandle();
        break;

    default:
        break;
    }

}

/**
 * @brief ���·��״��
 * 
 */
void Vision_SymbolJudge()
{
    //��ȡ�߽�ֶ���Ϣ
    Vision_GetSegment(Image_S.leftBroder,1);
    Vision_GetSegment(Image_S.rightBroder,0);
    
    //Ѱ��������
    Vision_BroderFindFP(Image_S.leftBroder);
    Vision_BroderFindFP(Image_S.rightBroder);


    //ֻ�е���·���Ϊ������·ʱ����Ҫ�����ж�
    if(Current_Road == NormalRoads){
        //�����������ж�
        int state_code = F.FP_n_L*10+F.FP_n_R;
        switch (state_code)
        {
        case 0:
            //���߶�û��ϵ�
            //if(F.segment_n_L == 1)
            break;

        case 20:
            if(F.my_segment_R[0].type == lose_segment)
                Current_Road = CrossRoads;
            else if(F.my_segment_R[0].type == straight_segment
                    && F.segment_n_R == 1)
                Current_Road = CirculeRoads;
            break;

        case 2:
            if(F.my_segment_L[0].type == lose_segment)
                Current_Road = CrossRoads;
            else if(F.my_segment_L[0].type == straight_segment
                    && F.segment_n_L == 1)
                Current_Road = CirculeRoads;
            break;

        case 22:
            Current_Road = CrossRoads;
            break;

        case 21:
            Current_Road = CrossRoads;
            break;

        case 12:
            Current_Road = CrossRoads;
            break;

        default:
            if(F.segment_n_L == 1&&F.segment_n_R != 1){
                if(F.my_segment_L[0].type  == straight_segment)
                    Current_Road = CirculeRoads;
                else if(F.my_segment_L[0].type == lose_segment)
                    Current_Road = CornerRoads;
            }
            if(F.segment_n_R == 1&&F.segment_n_L != 1){
                if(F.my_segment_R[0].type  == straight_segment)
                    Current_Road = CirculeRoads;
                else if(F.my_segment_R[0].type == lose_segment)
                    Current_Road = CornerRoads;
            }
            break;
        }
    }
        
}

/**
 * @brief ��ӡ������
*/
void Vision_DrawFP(){
    
    for(int i = 0;i<F.FP_n_L;i++){
        tft180_draw_point(F.feature_p_L[i].y,78-(imgRow-1)+F.feature_p_L[i].x,RGB565_RED);
        if(78-(imgRow-1)+F.feature_p_L[i].x+1 < 128)
            tft180_draw_point(F.feature_p_L[i].y,78-(imgRow-1)+F.feature_p_L[i].x+1,RGB565_RED);
        if(78-(imgRow-1)+F.feature_p_L[i].x-1 >= 0)
            tft180_draw_point(F.feature_p_L[i].y,78-(imgRow-1)+F.feature_p_L[i].x-1,RGB565_RED);
        if(F.feature_p_L[i].y+1 < 160)
            tft180_draw_point(F.feature_p_L[i].y+1,78-(imgRow-1)+F.feature_p_L[i].x,RGB565_RED);
        if(F.feature_p_L[i].y-1 >= 0)
            tft180_draw_point(F.feature_p_L[i].y-1,78-(imgRow-1)+F.feature_p_L[i].x,RGB565_RED);
    }

    for(int i = 0;i<F.FP_n_R;i++){
        tft180_draw_point(F.feature_p_R[i].y,78-(imgRow-1)+F.feature_p_R[i].x,RGB565_RED);
        if(78-(imgRow-1)+F.feature_p_R[i].x+1 < 128)
            tft180_draw_point(F.feature_p_R[i].y,78-(imgRow-1)+F.feature_p_R[i].x+1,RGB565_RED);
        if(78-(imgRow-1)+F.feature_p_R[i].x-1 >= 0)
            tft180_draw_point(F.feature_p_R[i].y,78-(imgRow-1)+F.feature_p_R[i].x-1,RGB565_RED);
        if(F.feature_p_R[i].y+1 < 160)
            tft180_draw_point(F.feature_p_R[i].y+1,78-(imgRow-1)+F.feature_p_R[i].x,RGB565_RED);
        if(F.feature_p_R[i].y-1 >= 0)
            tft180_draw_point(F.feature_p_R[i].y-1,78-(imgRow-1)+F.feature_p_R[i].x,RGB565_RED);
    }
}

/**
 * @brief ���߽�ָ�������߽�ָ�Ϊ
 *          LorR 1---L 0---R
 * 
 * @param broder Ŀ��߽�
 * @param LorR   ���ұ߽� ��Ϊȱ�ߵ�ֵ��ͬ
 */
void Vision_GetSegment(int16* broder,uint8_t LorR)
{
    segment_t* target_segment = LorR? F.my_segment_L:F.my_segment_R;
    uint8_t begin_flag = 1;     //���м�¼��ʼ��־λ
    int segment_n = 0;          //���ڼ�¼�ڼ�������

    //�����һ�ε����� clear
    if(LorR)
        F.segment_n_L = 0;
    else
        F.segment_n_R = 0;

    for(int i = 0;i<10;i++)
    {
        target_segment[i].type = NULL_segment;
        target_segment[i].begin = 0;
        target_segment[i].end = 0;
        target_segment[i].position = NEAR;
    }
    //���߽�ֶ� ���ұ��
    for(int i = imgRow-1 ; i>0 ; i--)
    {
        //�Ե�һ������д���
        if(begin_flag){
            target_segment[segment_n].begin = i == imgRow-1? i:i+1;
            switch(segment_n){
                case 0:
                    target_segment[segment_n].position = NEAR;
                    break;
                case 1:
                    target_segment[segment_n].position = MEDIUM;
                    break;
                default:
                    target_segment[segment_n].position = FAR;
                    break;
            }
           
           //�궨���
           //�����ж����������ؾ��ж�Ϊ��������ΪΪ����
           //���Ƕ�������Ϊδ��λ
            if(LorR){ //���
                if(broder[i]<=3&&broder[i-1]<=3&&broder[i-2]<=3)
                    target_segment[segment_n].type = lose_segment;
                else
                    target_segment[segment_n].type = NULL_segment;
            }
            else if((!LorR)){//�ұ�
                if(broder[i]>=156&&broder[i-1]>=156&&broder[i-2]>=156)
                    target_segment[segment_n].type = lose_segment;
                else
                    target_segment[segment_n].type = NULL_segment;
            }
            begin_flag = 0;
        }

        //
        //��¼��β
        //
        if(target_segment[segment_n].type == lose_segment){
            //������󼸸��㲻���ж�
            if(i>=2){
                if((LorR && broder[i]>3 && broder[i-1]>3 && broder[i-2]>3)||    //���
                    ((!LorR) && broder[i]<156 && broder[i-1]<156 && broder[i-2]<156)){ //�ұ�
                    target_segment[segment_n].end = i+1;//��¼��β
                    segment_n++;
                    begin_flag = 1;
                }
            }
            else
            ;
        }
        //����һ�μ�⵽�Ĳ�Ϊ��ʧ�����ߺ��¼Ϊ��һ�ο�ʼ
        else if(target_segment[segment_n].type == NULL_segment){
            if(i>=2){
                if((LorR && broder[i]<=3 && broder[i-1]<=3 && broder[i-2]<=3)||    //���
                    ((!LorR) && broder[i]>=156 && broder[i-1]>=156 && broder[i-2]>=156)){ //�ұ�
                    target_segment[segment_n].end = i+1;//��¼��β
                    segment_n++;
                    begin_flag = 1;
                }
            }
            else
            ;
        }
    }

    //����һ������ֻ��һ��ֵ��ʱ�򣬽���һ�����к͵ڶ����ϲ�
    if(target_segment[0].begin - target_segment[0].end <= 5){
        for(int i = target_segment[0].begin;i>=target_segment[0].end;i--){
            broder[i] = broder[target_segment[1].end];
        }

        target_segment[0].end = target_segment[1].end;
				target_segment[0].type = target_segment[1].type;
        for(int i = 1 ;i<=segment_n;i++)
        {
            target_segment[i].begin = target_segment[i+1].begin;
            target_segment[i].end = target_segment[i+1].end;
            target_segment[i].type = target_segment[i+1].type;
        }
        segment_n -= 1;
    }
    //����һ������̫��ʱ����ڶ������кϲ�{}

    //��¼����
    if(LorR)
        F.segment_n_L = segment_n+1;
    else
        F.segment_n_R = segment_n+1;

    //���ڱ߽�����ж�
    for(int i = 0;i<segment_n+1;i++)
    {
       
        if(target_segment[i].type == NULL_segment){
             //�Ƿ�Ϊֱ��
            int straight = Line_IsStraight(broder,target_segment[i].begin,target_segment[i].end);
            if(straight){
                target_segment[i].type = straight_segment;
            }
            //���ݵ�������������Լ�Բ�������ж�
            else{
                int mono;
                mono = Line_IsMonotonous(broder,target_segment[i].begin,target_segment[i].end);
                //����������Ϊ�����������������ΪԲ��
                target_segment[i].type = mono? corner_segment : arc_segment;
            }
        }
    }


}


/**
 * @brief Ѱ��һ�λ��������㣬����˵�ǵ����Ա仯��
 * 
 * @param broder 
 * @param x1 
 * @param x2 
 * @return point_t 
 */
point_t Vision_FindArcFP(int16 *broder,int x1,int x2)
{
    int max = Tool_CmpMax(x1,x2);
    int min = Tool_CmpMin(x1,x2);

    int how_many = max - min + 1;
    int final_x;//�洢��������

    int p_distance = 5;
    float cosvalue;

    if(how_many >= p_distance + 2){
        float cosvalue = Vector_AngleGet((point_t){min,broder[min]},
                                (point_t){min+1,broder[min+1]},
                                (point_t){min + p_distance,broder[min + p_distance]});
    }
    else{
        float cosvalue = Vector_AngleGet((point_t){min,broder[min]},
                                (point_t){min+1,broder[min+1]},
                                (point_t){max,broder[max]});
    }
    //�߽�ĵ㲻����
    for(int i = min+2; i<max - 2 ; i++)
    {
        int low_x = (i-min)>=5?(i - 5):min;
        int high_x = (max - i)>=5?(i + 5):max;

        if(broder[i] == LEFT_LOSE_VALUE|| broder[i] == RIGHT_LOSE_VALUE)
            broder[i] = broder[i-1];

        //��⵽ cosvalue �����ڵ�� cosvalue С��ʱ���������ֵ
        float current_cos = Vector_AngleGet((point_t){low_x,broder[low_x]},
                                            (point_t){i,broder[i]},
                                            (point_t){high_x,broder[high_x]});
        if(cosvalue < current_cos){
            final_x =  i;    
            cosvalue = current_cos;                     
        }
        
                                           
    }
    return (point_t){final_x,broder[final_x]};
}

/**
 * @brief Ѱ�ұ߽�������
 * 
 * @param broder 
 */
void Vision_BroderFindFP(int16* broder)
{
    segment_t *target_seg;
    int* target_n;
    point_t* target_FP;
    int lose_value;

    if(broder == Image_S.leftBroder){
        target_seg = F.my_segment_L;
        target_n = &(F.FP_n_L);
        target_FP = F.feature_p_L;
        lose_value = LEFT_LOSE_VALUE;
    }
    else if(broder == Image_S.rightBroder){
        target_seg = F.my_segment_R;
        target_n = &(F.FP_n_R);
        target_FP = F.feature_p_R;
        lose_value = RIGHT_LOSE_VALUE;
    }

    //�������
    (*target_n) = 0;
    for(int i = 0;i<4;i++){
        target_FP[0].x = 0;
        target_FP[0].y = 0;
    }

    //��Գ��������Ѱ��������
    if(target_seg[1].type == lose_segment){
        //�����жϱ߽���Ƿ��Ǽ�ϵ�
        if(fabs(broder[target_seg[0].end] - lose_value) > 15){
            target_FP[0].x = target_seg[0].end;
            target_FP[0].y = broder[target_FP[0].x];
            (*target_n)++;
            //Ѱ��Զ���ļ�ϵ�
            if(target_seg[2].type != NULL_segment && target_seg[2].type != lose_segment){
                point_t pf[2];
                //�߽���ж�

                // //ͨ���Ƕ�Ѱ�ҽǵ�
                pf[1] = Vision_FindArcFP(broder,target_seg[2].begin,target_seg[2].end);

                (*target_n)++;
            }
        }
        //���߽�㲻�� ��ͨ���Ƕ���Ѱ�Ҽ�ϵ�
        else{
            target_FP[0] = Vision_FindArcFP(broder,target_seg[0].begin,target_seg[0].end);
            (*target_n)++;
            //Ѱ��Զ���ļ�ϵ�
            if(target_seg[2].type != NULL_segment && target_seg[2].type != lose_segment){
                //�߽���ж�
                // if(fabs(broder[target_seg[2].begin] - lose_value) >= 15){
                //     target_FP[1].x = target_seg[2].begin;
                //     target_FP[1].y = broder[target_FP[1].x];
                //     (*target_n)++;
                // }
                //ͨ���Ƕ�Ѱ�ҽǵ�
                // else{
                    target_FP[1] = Vision_FindArcFP(broder,target_seg[2].begin,target_seg[2].end);
                    (*target_n)++;
                // }
            }
        }
        
    }
    //���б�Ž���ʮ�� ֻ���ҵ�һ����ϵ�
    //�������ʮ���м��״̬��Ҳֻ���ҵ�һ����ϵ�
    else if(target_seg[0].type == lose_segment){
        if(target_seg[1].type == arc_segment){
            target_FP[0] = Vision_FindArcFP(broder,target_seg[1].begin,target_seg[1].end);
            (*target_n)++;
        }
        else if(target_seg[1].type == straight_segment){
            target_FP[0].x = target_seg[1].begin;
            target_FP[0].y = broder[target_FP[1].x]; 
            (*target_n)++;
        }
        else if(target_seg[1].type == corner_segment){
            if(fabs(broder[target_seg[1].begin] - broder[target_seg[0].end]) > 15){
                target_FP[0].x = target_seg[1].begin;
                target_FP[0].y = broder[target_FP[1].x]; 
                (*target_n)++;
            }
            else{
                target_FP[0] = Vision_FindArcFP(broder,target_seg[1].begin,target_seg[1].end);
                (*target_n)++;
            }
        }
    }

}

/**
 * @brief ��ӡ�߽�����
 * 
 */
void Vision_BroderPrint()
{
    rt_kprintf("left:\n");
    for(int i = 0; i< 69; i++){
        rt_kprintf("%d\n",Image_S.leftBroder[i]);
    }
    rt_kprintf("right:\n");
    for(int i = 0; i< 69; i++){
        rt_kprintf("%d\n",Image_S.rightBroder[i]);
    }
}

/**
 * @brief ���������ӡ��Ϣ
 * 
 */
void Vision_ErrorLogin()
{
    rt_kprintf("Judge Error\n");
    rt_kprintf("left ==> seg_num:%d ",F.segment_n_L);
    for(int i = 0;i<F.segment_n_L;i++){
        char s[128];
        switch (F.my_segment_L[i].type)
        {
        case NULL_segment:
            strcpy(s,"NULL_segment");
            break;

        case straight_segment:
            strcpy(s,"straight_segment");
            break;

        case lose_segment:
            strcpy(s,"lose_segment");
            break;

        case arc_segment:
            strcpy(s,"arc_segment");
            break;
        
        case corner_segment:
            strcpy(s,"corner_segment");
            break;

        default:
            strcpy(s,"error_type");
            rt_kprintf("(%d)",F.my_segment_L[i].type);
            break;
        }
        rt_kprintf("segment%d_type:%s ",i,s);
    }

    rt_kprintf("\nright ==> seg_num:%d ",F.segment_n_R);

    for(int i = 0;i<F.segment_n_R;i++){
        char s[128];
        switch (F.my_segment_R[i].type)
        {
        case NULL_segment:
            strcpy(s,"NULL_segment");
            break;

        case straight_segment:
            strcpy(s,"straight_segment");
            break;

        case lose_segment:
            strcpy(s,"lose_segment");
            break;

        case arc_segment:
            strcpy(s,"arc_segment");
            break;
        
        case corner_segment:
            strcpy(s,"corner_segment");
            break;

        default:
            strcpy(s,"error_type");
            rt_kprintf("(%d)",F.my_segment_L[i].type);
            break;
        }
        rt_kprintf("segment%d_type:%s ",i,s);
    }
		rt_kprintf("\n");
}


/**
 * @brief �ӳ��߶ε��߽�
*   direction --- ���߷���
*   1---��Զ������ 0---���������
*/
void Vision_ExtendLine(int16 *broder,int x,int direction)
{   
    float this_K;
    //��Զ������
    if(direction){

        this_K = (x+5<=imgRow-1)?(float)(broder[x + 5] - broder[x])/5.0f:(float)(broder[imgRow-1] - broder[x])/(float)(imgRow - 1 - x);
        for(int i = x-1 ; i > 0 ; i--)
        {
            broder[i] = broder[x] - this_K * (x - i);
            broder[i] = broder[i]<0? 0:broder[i];
            broder[i] = broder[i]>imgCol-1? imgCol-1:broder[i];
        }
    }
    //���������
    else{
        
        this_K = (x-5 >= 0)?(float)(broder[x] - broder[x - 5])/5.0f:(float)(broder[x] - broder[0])/(float)x;

        for(int i = 0 ; i < imgRow-1-x ; i++)
        {
            broder[x+i] = broder[x] + this_K * i;
            broder[x+i] = broder[x+i]<0? 0:broder[x+i];
            broder[x+i] = broder[x+i]>imgCol-1? imgCol-1:broder[x+i];
        }
    }

}

/**
 * @brief ���ߺ���������������������±�������Ӧ�߽�������ֵ����Ϊֱ��
 * 
 * @param p1 ������1
 * @param p2 ������2
 * @param broder �߽�
 */
void Vision_set_AdditionalLine(int16 p1,int16 p2,int16 *broder)
{
		float slope = Point_CalSlope((point_t){p1,broder[p1]},(point_t){p2,broder[p2]});
		
		int pmin = p1>p2? p2:p1;
		int pmax = p1>p2? p1:p2;
		
		for(int i = pmin ; i<=pmax ; i++)
		{
			broder[i] = (int16)((float)(i-pmin)*slope + (float)broder[pmin]);
		}
}

/**
 * @brief ʮ����������
 * 
 */
#define Cross_State_1 1
#define Cross_State_2 2
#define Leave_Cross   3
void Vision_CrossHandle()
{
    static int state;
    //һ���ǵ����������
    //�����ǵ�����
    switch (F.FP_n_L)
    {
    case 1:
        Vision_ExtendLine(Image_S.leftBroder,F.feature_p_L[0].x,0);
        break;
    case 2:
        Vision_set_AdditionalLine(F.feature_p_L[0].x,F.feature_p_L[1].x,Image_S.leftBroder);
        break;
    default:
        break;
    }
    switch (F.FP_n_L)
    {
    case 1:
        Vision_ExtendLine(Image_S.rightBroder,F.feature_p_R[0].x,0);
        break;
    case 2:
        Vision_set_AdditionalLine(F.feature_p_R[0].x,F.feature_p_R[1].x,Image_S.rightBroder);
        break;
    default:
        break;
    }
    //BUZZER_SPEAK;
    //�����ʱ���߶�ȱ��
    if((F.my_segment_L[0].type == lose_segment && F.segment_n_L != 1)&&
        F.my_segment_R[0].type == lose_segment && F.segment_n_R != 1)
        state = Cross_State_2;
    //����ʱȱ�ߺ����³�����
    if(state == Cross_State_2 && 
        F.my_segment_L[0].type != lose_segment && 
        F.my_segment_R[0].type != lose_segment){
            Current_Road = NormalRoads;
            state = Cross_State_1;
        }


    // if(F.segment_n_L == 2 && F.segment_n_R == 2){
    //     //���3 ʮ�ֵ����׶� ������+����
    //     if(F.my_segment_L[0].type == lose_segment && F.my_segment_R[0].type == lose_segment){
    //         //���������
    //         Vision_ExtendLine(Image_S.leftBroder,F.my_segment_L[1].begin,0);
    //         Vision_ExtendLine(Image_S.rightBroder,F.my_segment_R[1].begin,0);
    //     }
    //     //���1 ʮ�ֵ�һ�׶Σ�����+��������
    //     else if(F.my_segment_L[1].type == lose_segment && F.my_segment_R[1].type == lose_segment){
    //         //��Զ������
    //         Vision_ExtendLine(Image_S.leftBroder,F.my_segment_L[0].end,1);
    //         Vision_ExtendLine(Image_S.rightBroder,F.my_segment_R[0].end,1);
    //     }
    //     //���4 б�Ž���
    //     else if(F.my_segment_L[0].type == arc_segment){
    //         //�ҵ�ͻ���
    //         int maxpoint = Line_FindMaxPoint(Image_S.leftBroder,F.my_segment_L[0].begin,F.my_segment_L[0].end);
    //         //ͻ��㲹��
    //         Vision_ExtendLine(Image_S.leftBroder,maxpoint,1);
    //     }
    //     else if(F.my_segment_R[0].type == arc_segment){
    //         //�ҵ�ͻ���
    //         int maxpoint = Line_FindMaxPoint(Image_S.rightBroder,F.my_segment_R[0].begin,F.my_segment_R[0].end);
    //         //ͻ��㲹��
    //         Vision_ExtendLine(Image_S.rightBroder,maxpoint,1);
    //     }
    // }

    // //���2 ʮ�ֵڶ��׶Σ�����+������+���� ������б�Ž���
    // else if(F.segment_n_L == 3 && F.segment_n_R == 3){
    //     if(F.my_segment_L[0].type == arc_segment && F.my_segment_R[0].type == lose_segment)
    //     {
    //         //�ҵ�ͻ���
    //         int maxpoint = Line_FindMaxPoint(Image_S.leftBroder,F.my_segment_L[0].begin,F.my_segment_L[0].end);
    //         //ͻ��㲹��
    //         Vision_ExtendLine(Image_S.leftBroder,maxpoint,1);
    //     }
    //     else if(F.my_segment_R[0].type == arc_segment && F.my_segment_L[0].type == lose_segment)
    //     {
    //         //�ҵ�ͻ���
    //         int maxpoint = Line_FindMaxPoint(Image_S.rightBroder,F.my_segment_R[0].begin,F.my_segment_R[0].end);
    //         //ͻ��㲹��
    //         Vision_ExtendLine(Image_S.rightBroder,maxpoint,1);
    //     }
    //     else
    //     {
    //         //��ʱ��ʮ�ֱ��ֿ��ܲ���˲��ȱ��
    //         int begin = F.my_segment_L[0].end;
    //         int end = F.my_segment_L[2].begin;
    //         //���õ��б�ʹ��ڴ�ʱ���϶�Ϊ�õ�Ϊ�����ϵ�һ�㣬�ҽ��õ�ǰ��
    //         while(fabs(Image_S.leftBroder[begin] - Image_S.leftBroder[begin+1])>15){
    //             begin = begin+1;//begin ǰ��
    //         }

    //         while((fabs(Image_S.leftBroder[end] - Image_S.leftBroder[end-1])>15)){
    //             end = end - 1;//end ����
    //         }
    //         //���в���
    //         Vision_set_AdditionalLine(begin,end,Image_S.leftBroder);

    //         begin = F.my_segment_R[0].end;
    //         end = F.my_segment_R[2].begin;
    //         //���õ��б�ʹ��ڴ�ʱ���϶�Ϊ�õ�Ϊ�����ϵ�һ�㣬�ҽ��õ�ǰ��
    //         while(fabs(Image_S.rightBroder[begin] - Image_S.rightBroder[begin+1])>15){
    //             begin = begin+1;//begin ǰ��
    //         }

    //         while((fabs(Image_S.rightBroder[end] - Image_S.rightBroder[end-1])>15)){
    //             end = end - 1;//end ����
    //         }
    //         Vision_set_AdditionalLine(begin,end,Image_S.rightBroder);
    //     }
    // }
    // //�����ʮ��ǰ����������
    // else if(F.segment_n_L == 3 && F.segment_n_R == 2){
    //     if((F.my_segment_L[0].type == lose_segment && F.my_segment_L[2].type == lose_segment &&
    //         F.my_segment_R[0].type == lose_segment) ){
    //             Vision_ExtendLine(Image_S.leftBroder,F.my_segment_L[1].begin,0);
    //             Vision_ExtendLine(Image_S.rightBroder,F.my_segment_R[1].begin,0);
    //         }
    // }
    // else if(F.segment_n_L == 2 && F.segment_n_R == 3){
    //     if((F.my_segment_R[0].type == lose_segment && F.my_segment_R[2].type == lose_segment &&
    //         F.my_segment_L[0].type == lose_segment) ){
    //             Vision_ExtendLine(Image_S.leftBroder,F.my_segment_L[1].begin,0);
    //             Vision_ExtendLine(Image_S.rightBroder,F.my_segment_R[1].begin,0);
    //         }
    // }
    // //б�Ž���ʮ��
    // else if(F.my_segment_L[0].type == arc_segment){
    //     //�ҵ�ͻ���
    //     int maxpoint = Line_FindMaxPoint(Image_S.leftBroder,F.my_segment_L[0].begin,F.my_segment_L[0].end);
    //     //ͻ��㲹��
    //     Vision_ExtendLine(Image_S.leftBroder,maxpoint,1);
    // }
    
    // else if(F.my_segment_R[0].type == arc_segment){
    //     //�ҵ�ͻ���
    //     int maxpoint = Line_FindMaxPoint(Image_S.rightBroder,F.my_segment_R[0].begin,F.my_segment_R[0].end);
    //     //ͻ��㲹��
    //     Vision_ExtendLine(Image_S.rightBroder,maxpoint,1);
    // }

    // else{
    //     Vision_ErrorLogin();
    //     Vision_BroderPrint();
    // }
   
}

/**
 * @brief �������
 * 
 */
void Vision_CornerHandle()
{
    if(F.segment_n_L == 1 && F.segment_n_R == 1){
        //��ʱ��Ϊ������߶���1�Ļ�����Ҫ����
        //���㵥��ȫ���� ��������һ����Ȼ�ܹ�ʹ������ƫ��
    }
    else if(F.segment_n_L == 2 && F.segment_n_R == 1){
        if(F.my_segment_R[0].type == lose_segment&&F.my_segment_L[1].type == lose_segment){
            //�ұ����ȱʧ�������ȱʧ�ε�ֵ����
            for(int i = F.my_segment_L[1].begin;i>F.my_segment_L[1].end;i--){
                Image_S.leftBroder[i] = RIGHT_LOSE_VALUE;
            }
        }
    }
    else if(F.segment_n_R == 2 && F.segment_n_L == 1){
            if(F.my_segment_L[0].type == lose_segment&&F.my_segment_R[1].type == lose_segment){
            //������ȱʧ�����ұ�ȱʧ�ε�ֵ����
            for(int i = F.my_segment_R[1].begin;i>F.my_segment_R[1].end;i--){
                Image_S.rightBroder[i] = LEFT_LOSE_VALUE;
            }
        }
    }

    Current_Road = NormalRoads;
}

/**
 * @brief Բ������
 * 
 */
#define Circule_Begin 0
#define Circule_State1 1
#define Circule_State2 2
#define Circule_State3 3
#define Circule_Stop   4
void Vision_CirculeHandle()
{   
    static int state = Circule_Begin;
    static int LorR;//1--L 0--R
    if(!state){
        BUZZER_SPEAK;
        if(F.my_segment_L[0].type == straight_segment && F.segment_n_L == 1)
            LorR = 1;
        else if(F.my_segment_R[0].type == straight_segment && F.segment_n_R == 1)
            LorR = 0;
        state = Circule_State1;
    }
    if(LorR){
				if(state == Circule_State1){
            if(F.my_segment_R[0].type == lose_segment)
                state = Circule_State2;
        }
        else if(state == Circule_State2){
            if(F.my_segment_R[0].type != lose_segment)
                state = Circule_State3;
        }
        else if(state == Circule_State3){
            if(F.my_segment_R[0].type == lose_segment){
                state = Circule_Stop;
                BUZZER_SPEAK;
                Car_Change_Speed(0,0,0);
                rt_thread_delay(2000);
            }    
        }
        else if(state == Circule_Stop){
            if(F.my_segment_R[0].type != lose_segment){
                state = Circule_Begin;
                Current_Road = NormalRoads;
            }
                
        }

    }
    else{
        if(state == Circule_State1){
            if(F.my_segment_L[0].type == lose_segment)
                state = Circule_State2;
        }
        else if(state == Circule_State2){
            if(F.my_segment_L[0].type != lose_segment)
                state = Circule_State3;
        }
        else if(state == Circule_State3){
            if(F.my_segment_L[0].type == lose_segment){
                state = Circule_Stop;
                BUZZER_SPEAK;
                Car_Change_Speed(0,0,0);
                rt_thread_delay(2000);
            }    
        }
        else if(state == Circule_Stop){
            if(F.my_segment_L[0].type != lose_segment){
                state = Circule_Begin;
                Current_Road = NormalRoads;
            }
                
        }
    }
        

}

