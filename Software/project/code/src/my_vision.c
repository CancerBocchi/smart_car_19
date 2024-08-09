#include "my_vision.h"
#include "zf_device_tft180.h"
#include "string.h"

int block = 7;
int adaptivePara = 6;

RoadSymbol_type Current_Road;

// Բ�����ұ�ʶλ
uint8_t Circule_LorR;

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
        Current_Road = NormalRoads;
        
        break;

    case NormalRoads: 
        ips200_show_string(200,50,"Nor");
        break;

    case CrossRoads:
        Vision_CrossHandle();
        ips200_show_string(200,50,"Cro");
        break;
    
    case CirculeRoads:
        Vision_CirculeHandle();
        ips200_show_string(200,50,"Cir");
        break;

    case CornerRoads:
        Vision_CornerHandle();
        ips200_show_string(200,50,"Cor");
        break;

    case ZebraRoads:
        Vision_ZebraHandle();
        ips200_show_string(200,50,"Zeb");
        break;

    default:
        break;
    }

}

/**
 * @brief ���·��״��
 * 
 */
//˫�߻�
#define CornerState1    (IsArcCorner(F.my_segment_L[0])&&IsArcCorner(F.my_segment_L[0])&&F.segment_n_L == 1&&F.segment_n_R == 1)
//˫�߻�+ȱ��
#define CornerState2    (IsArcCorner(F.my_segment_L[0])&&IsArcCorner(F.my_segment_L[0])\
                        &&IsLose(F.my_segment_L[1])&&IsLose(F.my_segment_R[1])\
                        && F.segment_n_L == 2 && F.my_segment_R == 2) 
//����ȱ��
#define CornerState3    (IsArcCorner(F.my_segment_L[0])&&IsLose(F.my_segment_L[1])&&IsLose(F.my_segment_R[0])&&F.segment_n_R == 1\
                       ||IsArcCorner(F.my_segment_R[0])&&IsLose(F.my_segment_R[1])&&IsLose(F.my_segment_L[0])&&F.my_segment_L == 1)

#define CrossCon1       (F.FP_n_L == 2&&F.FP_n_R == 2)
#define CrossCon2       ((F.FP_n_L == 1&&F.FP_n_R == 2)||(F.FP_n_R == 1&&F.FP_n_L == 2))
#define CrossCon3       ((IsLose(F.my_segment_L[0])&&F.segment_n_L == 1&&F.FP_n_L == 0&&F.FP_n_R == 2)||\
                         (IsLose(F.my_segment_R[0])&&F.segment_n_R == 1&&F.FP_n_R == 0&&F.FP_n_L == 2))
//ʮ�����������ʶ����Բ�������
#define CrossCon4       (((IsLose(F.my_segment_R[0])&&!IsNull(F.my_segment_R[1]))&&(F.segment_n_L <= 2&&IsArc(F.my_segment_L[0])&&F.FP_n_L))||\
                         ((IsLose(F.my_segment_L[0])&&!IsNull(F.my_segment_L[1]))&&(F.segment_n_R <= 2&&IsArc(F.my_segment_R[0])&&F.FP_n_L)))

void Vision_SymbolJudge()
{
    //��ȡ�߽�ֶ���Ϣ
    Vision_GetSegment(Image_S.leftBroder,1);
    Vision_GetSegment(Image_S.rightBroder,0);
    
    //Ѱ��������
    Vision_BroderFindFP(Image_S.leftBroder);
    Vision_BroderFindFP(Image_S.rightBroder);

    char str[100];
    rt_sprintf(str,"left_p:%d",F.FP_n_L);
    ips200_show_string(200,35,str);
    rt_sprintf(str,"right_p:%d",F.FP_n_R);
    ips200_show_string(200,20,str);

    //ֻ�е���·���Ϊ������·ʱ����Ҫ�����ж�
    if(Current_Road == NormalRoads){
        if(Vision_IsZebra()){
            Current_Road = ZebraRoads;
        }
        else if( (F.segment_n_L == 1 && IsStrai(F.my_segment_L[0]) && IsLose(F.my_segment_R[1]))||
            (F.segment_n_R == 1 && IsStrai(F.my_segment_R[0]) && IsLose(F.my_segment_L[1])) )
            Current_Road = CirculeRoads;

        else if(CornerState1||CornerState2||CornerState3) 
            Current_Road = CornerRoads;

        else if(CrossCon1||CrossCon2||CrossCon3||CrossCon4)
            Current_Road = CrossRoads;
        
        else if(F.segment_n_L == 1&&F.segment_n_R == 1 &&
                IsLose(F.my_segment_L[0])&&IsLose(F.my_segment_R[0]))
            Current_Road = LoseRoads;

        else 
            Current_Road = NormalRoads;
        
        
    }
        
}

/**
 * @brief ��ӡ������
*/
void Vision_DrawFP(){
    
    for(int i = 0;i<F.FP_n_L;i++){
//        ips200_draw_point(F.feature_p_L[i].y*158/188,78-(imgRow-1)+F.feature_p_L[i].x,RGB565_RED);
//        if(78-(imgRow-1)+F.feature_p_L[i].x+1 < 128)
//            ips200_draw_point(F.feature_p_L[i].y*158/188,78-(imgRow-1)+F.feature_p_L[i].x+1,RGB565_RED);
//        if(78-(imgRow-1)+F.feature_p_L[i].x-1 >= 0)
//            ips200_draw_point(F.feature_p_L[i].y*158/188,78-(imgRow-1)+F.feature_p_L[i].x-1,RGB565_RED);
//        if((F.feature_p_L[i].y+1)*158/188 < 160)
//            ips200_draw_point((F.feature_p_L[i].y+1)*158/188,78-(imgRow-1)+F.feature_p_L[i].x,RGB565_RED);
//        if((F.feature_p_L[i].y+1)*158/188 >= 0)
//            ips200_draw_point((F.feature_p_L[i].y-1)*158/188,78-(imgRow-1)+F.feature_p_L[i].x,RGB565_RED);
    }

    for(int i = 0;i<F.FP_n_R;i++){
//        ips200_draw_point(F.feature_p_R[i].y*158/188,78-(imgRow-1)+F.feature_p_R[i].x,RGB565_RED);
//        if(78-(imgRow-1)+F.feature_p_R[i].x+1 < 128)
//            ips200_draw_point(F.feature_p_R[i].y*158/188,78-(imgRow-1)+F.feature_p_R[i].x+1,RGB565_RED);
//        if(78-(imgRow-1)+F.feature_p_R[i].x-1 >= 0)
//            ips200_draw_point(F.feature_p_R[i].y*158/188,78-(imgRow-1)+F.feature_p_R[i].x-1,RGB565_RED);
//        if((F.feature_p_R[i].y+1)*158/188 < 160)
//            ips200_draw_point((F.feature_p_R[i].y+1)*158/188,78-(imgRow-1)+F.feature_p_R[i].x,RGB565_RED);
//        if((F.feature_p_R[i].y-1)*158/188 >= 0)
//            ips200_draw_point((F.feature_p_R[i].y-1)*158/188,78-(imgRow-1)+F.feature_p_R[i].x,RGB565_RED);
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
           
           //�궨���
           //�����ж����������ؾ��ж�Ϊ��������ΪΪ����
           //���Ƕ�������Ϊδ��λ
            if(LorR){ //���
                if((i-2>=0)&&broder[i]<=3&&broder[i-1]<=3){
                    target_segment[segment_n].type = lose_segment;
                    broder[i] = LEFT_LOSE_VALUE;
                    broder[i-1] = LEFT_LOSE_VALUE;
                    broder[i-2] = LEFT_LOSE_VALUE;
                }
                else
                    target_segment[segment_n].type = NULL_segment;
            }
            else if((!LorR)){//�ұ�
                if((i-2>=0)&&broder[i]>=RIGHT_LOSE_VALUE-5&&broder[i-1]>=RIGHT_LOSE_VALUE-5){
                    target_segment[segment_n].type = lose_segment;
                    broder[i] = RIGHT_LOSE_VALUE;
                    broder[i-1] = RIGHT_LOSE_VALUE;
                    broder[i-2] = RIGHT_LOSE_VALUE;
                }
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
            if(i>=3){
                //��ֹͻ�� ��������������߽��Զʱ��Ϊ��lose
                if((LorR && broder[i]>5 && broder[i-1]>5 && broder[i-2]>5 && broder[i-3]>5)||    //���
                    ((!LorR) && broder[i]<RIGHT_LOSE_VALUE-5 && broder[i-1]<RIGHT_LOSE_VALUE-5 && broder[i-2]<RIGHT_LOSE_VALUE-5 && broder[i-3]<RIGHT_LOSE_VALUE-5)){ //�ұ�
                    target_segment[segment_n].end = i+1;//��¼��β
                    segment_n++;
                    begin_flag = 1;
                }
                else 
                    broder[i] = (LorR)? LEFT_LOSE_VALUE:RIGHT_LOSE_VALUE;
            }
            else
            ;
        }
        //����һ�μ�⵽�Ĳ�Ϊ��ʧ�����ߺ��¼Ϊ��һ�ο�ʼ
        else if(target_segment[segment_n].type == NULL_segment){
            if(i>=2){
                //��ֹͻ�� ������������С��Ϊ��lose
                if((LorR && broder[i]<=5 && broder[i-1]<=5 && broder[i-2]<=5)||    //���
                    ((!LorR) && broder[i]>=RIGHT_LOSE_VALUE-5 && broder[i-1]>=RIGHT_LOSE_VALUE-5 && broder[i-2]>=RIGHT_LOSE_VALUE-5)){ //�ұ�
                    broder[i] = LorR? LEFT_LOSE_VALUE:RIGHT_LOSE_VALUE;
                    broder[i-1] = LorR? LEFT_LOSE_VALUE:RIGHT_LOSE_VALUE;
                    broder[i-2] = LorR? LEFT_LOSE_VALUE:RIGHT_LOSE_VALUE;
                    target_segment[segment_n].end = i+1;//��¼��β
                    segment_n++;
                    begin_flag = 1;
                }
                else 
                    broder[i] = (broder[i]<=LEFT_LOSE_VALUE+5||broder[i]>=RIGHT_LOSE_VALUE - 5)? broder[i+1]:broder[i];
            }
            else
            ;
        }
    }

    // ����һ������̫��ʱ����ڶ������кϲ�
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
 * @brief Ѱ��һ�λ��ĵ�һ�������㣬�����ɴ�С
 * 
 * @param broder �߽�
 * @param x1    ����
 * @param x2    ������
 * @return point_t ���ص�һ������������� ������ֵΪ-1����û�з���������
 */
point_t Vision_FindArcFP(int16 *broder,int x1,int x2)
{
    int max = Tool_CmpMax(x1,x2);
    int min = Tool_CmpMin(x1,x2);

    int how_many = max - min + 1;
    int final_x = max - 1;

    int p_distance = 5;//�������
    float p_th = -0.3;//�ж�Ϊ�ǵ����ֵ
    float cosvalue;

    if(how_many >= p_distance + 2){
        cosvalue = Vector_AngleGet((point_t){max,broder[max]},
                                (point_t){max-1,broder[max-1]},
                                (point_t){max - p_distance,broder[max - p_distance]});
    }
    else{
        cosvalue = Vector_AngleGet((point_t){max,broder[max]},
                                (point_t){max-1,broder[max-1]},
                                (point_t){min,broder[min]});
    }

    //�߽�ĵ㲻����
    for(int i = max - 2; i>min+1; i--)
    {
        int low_x = (i-min)>=5?(i - 5):min;
        int high_x = (max - i)>=5?(i + 5):max;

        //��ȥͻ����
        if(broder[i] == LEFT_LOSE_VALUE|| broder[i] == RIGHT_LOSE_VALUE)
            broder[i] = broder[i-1];

        float currentcos = Vector_AngleGet((point_t){low_x,broder[low_x]},
                                            (point_t){i,broder[i]},
                                            (point_t){high_x,broder[high_x]});
        
        if(currentcos > cosvalue){
            final_x = i;
            cosvalue = currentcos;
        }
                                           
    }
    //�ҵ�������
    if(cosvalue > p_th)
        return (point_t){final_x,broder[final_x]};
    //δ�ҵ�������
    else
        return (point_t){-1,-1};
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
    int segment_n;

    if(broder == Image_S.leftBroder){
        target_seg = F.my_segment_L;
        target_n = &(F.FP_n_L);
        target_FP = F.feature_p_L;
        segment_n = F.segment_n_L;
        lose_value = LEFT_LOSE_VALUE;
    }
    else if(broder == Image_S.rightBroder){
        target_seg = F.my_segment_R;
        target_n = &(F.FP_n_R);
        target_FP = F.feature_p_R;
        segment_n = F.segment_n_R;
        lose_value = RIGHT_LOSE_VALUE;
    }

    //�������
    (*target_n) = 0;
    for(int i = 0;i<3;i++){
        target_FP[i].x = 0;
        target_FP[i].y = 0;
    }

    /*---------------�ǵ���---------------*/
    int aver1;
    int aver2;
    point_t pf;
    //�м�ȱ�� ��ȱ�ߴﵽ��ֵ
    if(IsLose(target_seg[1])&&Vision_IsLone(target_seg[1])){
        aver1 = Line_GetAverage(broder,target_seg[0].begin,target_seg[0].end);
        
        //���������еĽǵ�
        pf = Vision_FindArcFP(broder,target_seg[0].begin,target_seg[0].end);
        //��û�з��ֽǵ�
        if((pf.x == -1 && pf.y == -1)){
            target_FP[0].x = target_seg[0].end;
            target_FP[0].y = broder[target_seg[0].end];
            (*target_n)++;
            //����������е�ƽ��б�� ����Ԥ��u��������
        
            //*���Զ�����еĽǵ�  ��Զ��������ֵ��ʱ���� ��ֵ��ʱ��ض��нǵ�*
            if(!IsNull(target_seg[2])){
                //��Զ�����кͽ������в��̫����ΪΪͻ��
                aver2 = Line_GetAverage(broder,target_seg[2].begin,target_seg[2].end);
                if(fabs(aver1 - aver2)>80){
                    target_FP[0].x = -1;
                    target_FP[0].y = -1;
                    (*target_n)--;
                    return;
                }
                //ֻѰ�ҽϽ����е�������
                pf = (target_seg[2].begin - target_seg[2].end >= 15)?
                    Vision_FindArcFP(broder,target_seg[2].begin,target_seg[2].begin - 15) :
                    Vision_FindArcFP(broder,target_seg[2].begin,target_seg[2].end);
                    //*���ڽǵ��¼�ǵ㣬�����ڼ�¼�߽��*
                target_FP[1].x = (pf.x == -1) ? target_seg[2].begin : pf.x ;
                target_FP[1].y = (pf.y == -1) ? broder[target_seg[2].begin] : pf.y;
                (*target_n)++;
            }

        }//�����˽ǵ�
        else{
            target_FP[0] = pf;
            (*target_n)++;

           //*���Զ�����еĽǵ�  ��Զ��������ֵ��ʱ���� ��ֵ��ʱ��ض��нǵ�*
          if(!IsNull(target_seg[2])){
                //��Զ�����кͽ������в��̫����ΪΪͻ��
                aver2 = Line_GetAverage(broder,target_seg[2].begin,target_seg[2].end);
                if(fabs(aver1 - aver2)>80){
                    target_FP[0].x = -1;
                    target_FP[0].y = -1;
                    (*target_n)--;
                    return;
                }
                //ֻѰ�ҽϽ����е�������
                pf = (target_seg[2].begin - target_seg[2].end >= 15)?
                    Vision_FindArcFP(broder,target_seg[2].begin,target_seg[2].begin - 15) :
                    Vision_FindArcFP(broder,target_seg[2].begin,target_seg[2].end);
                //*���ڽǵ��¼�ǵ㣬�����ڼ�¼�߽��*
                target_FP[1].x = (pf.x == -1) ? target_seg[2].begin : pf.x ;
                target_FP[1].y = (pf.y == -1) ? broder[target_seg[2].begin] : pf.y;
                (*target_n)++;
            }
        }
    }

    //��ͷȱ��
    else if(target_seg[0].type == lose_segment){
        //���Զ�����еĽǵ�  ��Զ��������ֵ��ʱ����
        if(!IsNull(target_seg[1])){
            pf = (target_seg[1].begin - target_seg[1].end >= 15)?
                Vision_FindArcFP(broder,target_seg[1].begin,target_seg[1].begin - 15) :
                Vision_FindArcFP(broder,target_seg[1].begin,target_seg[1].end);
            //��δ���ֽǵ�
            target_FP[0].x = (pf.x == -1) ? target_seg[1].begin : pf.x ;
            target_FP[0].y = (pf.y == -1) ? broder[target_seg[1].begin] : pf.y;
            (*target_n)++;
        }
    }
    //б��ʮ�����������ʮ�ֽӻ����� ֻѰ�ҽ����Ľǵ�
    else if(IsArc(target_seg[0])&&segment_n == 1){
        pf = Vision_FindArcFP(broder,target_seg[0].begin,25);
        if(pf.x != -1&&pf.y != -1){
					target_FP[0] = pf;
					(*target_n)++;
        }
    }
}

/**
 * @brief �ж�ǰ���Ƿ��а�����
 * 
 * @return uint8_t ����1Ϊ����ǰ��������
 */
#define Zebra_Y 60
#define Zebra_TH 10
#define Zebra_Range 10
uint8_t Vision_IsZebra(){
    int change_num = 0;
    int mid = (Image_S.leftBroder[30]+Image_S.rightBroder[30])/2;

    for(int i = mid;i<140;i++){
        if(my_image[Zebra_Y][i] > Threshold+Zebra_Range && my_image[Zebra_Y][i+1] < Threshold-Zebra_Range||
            my_image[Zebra_Y][i] < Threshold-Zebra_Range && my_image[Zebra_Y][i+1] > Threshold+Zebra_Range){
            change_num++;
        }
    }

    for(int i = mid;i>46;i--){
        if(my_image[Zebra_Y][i] > Threshold+Zebra_Range && my_image[Zebra_Y][i+1] < Threshold-Zebra_Range||
            my_image[Zebra_Y][i] < Threshold-Zebra_Range && my_image[Zebra_Y][i+1] > Threshold+Zebra_Range){
            change_num++;
        }
    }

    if(change_num >= Zebra_TH)
        return 1;

    return 0;
    
}

/**
 * @brief ��ӡ�߽�����
 * 
 */
void Vision_BroderPrint()
{
    rt_kprintf("left:\n");
    for(int i = 0; i< imgRow-1; i++){
        rt_kprintf("%d\n",Image_S.leftBroder[i]);
    }
    rt_kprintf("right:\n");
    for(int i = 0; i< imgRow-1; i++){
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
 * @brief �ӳ��߶ε��߽� ���ǹ̶�Kֵ
 *          direction --- ���߷���
 *          1---��Զ������ 0---���������
 * 
 * @param broder �߽�����
 * @param x ��ʼ��
 * @param direction ���߷���
 * @param slope б��
 */
#define FAR_Extend 1
#define NEAR_Extend 0
void Vision_ExtendLineK(int16 *broder,int x,int direction,float slope){
    //��Զ������
    if(direction){
        for(int i = x-1 ; i > 0 ; i--)
        {
            broder[i] = broder[x] - slope * (x - i);

            broder[i] = broder[i]<0? 0:broder[i];
            broder[i] = broder[i]>159?159:broder[i];
            
            broder[i] = broder[i]>imgCol-1? imgCol-1:broder[i];
        }
    }
    //���������
    else{
        for(int i = 0 ; i < imgRow-1-x ; i++)
        {
            broder[x+i] = broder[x] + slope * i;

            broder[x+i] = broder[x+i]<0? 0:broder[x+i];
            broder[x+i] = broder[x+i]>159? 159:broder[x+i];

            broder[x+i] = broder[x+i]>imgCol-1? imgCol-1:broder[x+i];
        }
    }

}

/**
 * @brief ����߽�ȱ�ݶȺ���
 * 
 * @param broder ��Ӧ�߽�
 * @return float ȱ����
 */
float Vision_GetLoseRate(int16* broder){
    int LorR = (broder == Image_S.leftBroder)?1:0;
    int lose_num;
    for(int i = 0;i<IMAGE_ROW;i++)
    {
        lose_num = (LorR&&broder[i]==LEFT_LOSE_VALUE)||(!LorR&&broder[i]==RIGHT_LOSE_VALUE)? 
                    lose_num+1:lose_num;
    }
    return (float)lose_num/IMAGE_ROW;
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
 * @brief ���ߺ���������������������±�����Ӧ�߽�������ֵ����Ϊֱ��
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
 * @brief ���߽��Ӧ������ȱ��״̬
 * 
 * @param broder �߽�
 * @param x1 ����
 * @param x2 ��β
 */
void Vision_SetLose(int16* broder,int16 x1,int16 x2){
    int max = Tool_CmpMax(x1,x2);
    int min = Tool_CmpMin(x1,x2);

    for(int i = min;i<max;i++){
        broder[i] = (broder == Image_S.leftBroder)? LEFT_LOSE_VALUE:RIGHT_LOSE_VALUE;
    }
}

/**
 * @brief ����һ�����һ���̶���б�ʻ�ֱ��
 * 
 * @param broder �߽�
 * @param seed   ĳһ����������
 * @param slope  б��
 * @param x1     ֱ�߷�Χ1
 * @param x2     ֱ�߷�Χ2
*/
void Vision_SetLineWithPointK(int16* broder,int seed,float slope,int x1,int x2){
    int max = Tool_CmpMax(x1,x2);
    int min = Tool_CmpMin(x1,x2);

    for(int i = min; i<=max ;i++){
        broder[i] = broder[seed] + (i - seed)*slope;
    }
}

/**
 * @brief ʮ����������
 * 
 */
#define Cross_Begin   0
#define Cross_State_1 1
void Vision_CrossHandle()
{
    static int state = Cross_Begin;

        //״̬�л�
    if(state == Cross_Begin){
        //������ʮ�ֵ�ʱ�������߶�δ�ܷ��ֽǵ� �˳�ʮ��
        //ʮ�ֵ�ʱ��Ӧ�������ԵĽǵ�
        // rt_kprintf("RS:Cross\n");
        if(F.FP_n_L == 0&&F.FP_n_R == 0){
            Current_Road = NormalRoads;
            return;
        }
        if(IsLose(F.my_segment_L[0])&&IsLose(F.my_segment_R[0])){
             state = Cross_State_1;
             BUZZER_SPEAK;
        }
    }
    else if(state == Cross_State_1){
        if(!IsLose(!F.my_segment_L[0])&&!IsLose(F.my_segment_R[0])){
            BUZZER_SPEAK;
            if(Start_Flag){
                Car_Change_Speed(0,0,0);
                point_t first_point = {0,(Image_S.leftBroder[0]+Image_S.rightBroder[0])/2};
                point_t last_point = {30,(Image_S.leftBroder[30]+Image_S.rightBroder[30])/2};
                float k = Line_CalK(first_point,last_point);
                rt_kprintf("%d,%d,%.2f\n",(Image_S.leftBroder[30]+Image_S.rightBroder[30])/2,(Image_S.leftBroder[0]+Image_S.rightBroder[0])/2,k);
                // for(int i = 0;i<imgRow;i++)
                //     rt_kprintf("%d\n",Image_S.leftBroder[i]);
                //  for(int i = 0;i<imgRow;i++)
                //     rt_kprintf("%d\n",Image_S.rightBroder[i]);
                // while(1);
                if(k<0){
                    L_or_R_Cross = Right_Cross;
                }
                else 
                    L_or_R_Cross = Left_Cross;
                rt_kprintf("trace:ready to get into cross thread\n");
                rt_sem_release(cross_handle_sem);
                rt_sem_take(trace_line_sem,RT_WAITING_FOREVER);
            }
            state = Cross_Begin;
            Current_Road = NormalRoads;
            
        }
    }
    //����
    //б��ʮ��
    if(CrossCon4){
        //���
        if (IsArc(F.my_segment_L[0])&&F.segment_n_L == 1){
            //��ʱӦ������ҵ�һ�������� ���������㵽��һ�߽ǵ��б��
            Vision_ExtendLine(Image_S.leftBroder,F.feature_p_L[0].x,1);
            Vision_ExtendLine(Image_S.rightBroder,F.feature_p_R[0].x,0);
        }
        //�ұ�
        else if(IsArc(F.my_segment_R[0])&&F.segment_n_R == 1){
            Vision_ExtendLine(Image_S.leftBroder,F.feature_p_L[0].x,0);
            Vision_ExtendLine(Image_S.rightBroder,F.feature_p_R[0].x,1);
        }
    }
    // else{
    //     if(F.FP_n_L == 2)
    //         Vision_set_AdditionalLine(F.feature_p_L[0].x,F.feature_p_L[1].x,Image_S.leftBroder);
    //     else if(F.FP_n_L == 1){
    //         if(IsLose(F.my_segment_L[0]))
    //             Vision_ExtendLine(Image_S.leftBroder,F.feature_p_L[0].x,0);
    //         else
    //             Vision_ExtendLine(Image_S.leftBroder,F.feature_p_L[0].x,1);
    //     }

    //     if(F.FP_n_R == 2)
    //         Vision_set_AdditionalLine(F.feature_p_R[0].x,F.feature_p_R[1].x,Image_S.rightBroder);
    //     else if(F.FP_n_R == 1){
    //         if(IsLose(F.my_segment_R[0]))
    //             Vision_ExtendLine(Image_S.rightBroder,F.feature_p_R[0].x,0);
    //         else
    //             Vision_ExtendLine(Image_S.rightBroder,F.feature_p_R[0].x,1);
    //     }
    // }
   
}

/**
 * @brief �������
 * 
 */
void Vision_CornerHandle()
{
    if(CornerState1){
        //��ʱ��Ϊ������߶���1�Ļ�����Ҫ����
        //���㵥��ȫ���� ��������һ����Ȼ�ܹ�ʹ������ƫ��
    }
    else if(CornerState2){
        if(F.my_segment_L[1].end - F.my_segment_L[1].begin > F.my_segment_R[1].end - F.my_segment_R[1].begin){
            //�ұ�ȱ���� ��ת 
            for(int i = F.my_segment_R[1].begin;i>F.my_segment_R[1].end;i--){
                Image_S.rightBroder[i] = LEFT_LOSE_VALUE;
            }
        }
        else if(F.my_segment_L[1].end - F.my_segment_L[1].begin < F.my_segment_R[1].end - F.my_segment_R[1].begin){
            //���ȱ���� �ұ�ת 
            for(int i = F.my_segment_L[1].begin;i>F.my_segment_L[1].end;i--){
                Image_S.leftBroder[i] = LEFT_LOSE_VALUE;
            }
        }
    }
    else if(CornerState3){
        //���ȱ�� ���ұ߲���������
        if(IsLose(F.my_segment_L[0]))
            for(int i = F.my_segment_R[1].begin;i>F.my_segment_R[1].end;i--){
                Image_S.rightBroder[i] = LEFT_LOSE_VALUE;
            }
        //�ұ�ȱ�� ����߲���������
        else if(IsLose(F.my_segment_R[0]))
            for(int i = F.my_segment_L[1].begin;i>F.my_segment_L[1].end;i--){
                Image_S.leftBroder[i] = LEFT_LOSE_VALUE;
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
    if(!state){
        rt_kprintf("RS:Circule\n");
        BUZZER_SPEAK;
        //��ֹ��ʶ��
        // MCX_Change_Mode(MCX_Reset_Mode);
        MCX_Clear();
        if(IsStrai(F.my_segment_L[0]) && F.segment_n_L == 1)
            Circule_LorR = RIGHT_CIRCULE;
        else if(IsStrai(F.my_segment_R[0]) && F.segment_n_R == 1)
            Circule_LorR = LEFT_CIRCULE;
        state = Circule_State1;
    }
    //��ֹ�����ж�
    switch(Circule_LorR){
        static int out_n;
        case RIGHT_CIRCULE:
            out_n = (!IsStrai(F.my_segment_L[0])&&(Vision_GetSegLenghth(F.my_segment_L[0]) >= 20))? out_n+1:0;
            if(out_n == 4){
                Current_Road = NormalRoads;
                state = Circule_Begin;
                out_n = 0;
                MCX_Change_Mode(MCX_Detection_Mode);
                rt_kprintf("RS:Out of Cir\n");
            }
        break;
        case LEFT_CIRCULE:
            out_n = (!IsStrai(F.my_segment_R[0])&&(Vision_GetSegLenghth(F.my_segment_R[0]) >= 20))? out_n+1:0;
            if(out_n == 4){
                Current_Road = NormalRoads;
                state = Circule_Begin;
                MCX_Change_Mode(MCX_Detection_Mode);
                rt_kprintf("RS:Out of Cir\n");
            }
        break;
    }

    if(Circule_LorR == RIGHT_CIRCULE){//�ұ�Բ��
        if(state == Circule_State1){
            if(F.FP_n_R && !IsLose(F.my_segment_R[0]))
                Vision_ExtendLine(Image_S.rightBroder,F.feature_p_R[0].x,1);
            if(IsLose(F.my_segment_R[0])){
                state = Circule_State2; 
                rt_kprintf("RS:Cir State2\n");
            }
                
        }
        else if(state == Circule_State2){
            //��ֱ��
            MCX_Change_Mode(MCX_Reset_Mode);
            if(IsLose(F.my_segment_R[0])){
                //����ֱ�ߵ�ƽ��б��
                float slope = Point_CalSlope((point_t){0,Image_S.leftBroder[0]},(point_t){69,Image_S.leftBroder[69]});
                //�õ�Բ������С��
                int seed = Line_FindMinPoint(Image_S.rightBroder,F.my_segment_R[1].begin,F.my_segment_R[1].end);
                //����б����Ϊû��͸�ӱ任
                Vision_SetLineWithPointK(Image_S.rightBroder,seed,-slope,0,69);
            }
                
            else if(!IsLose(F.my_segment_R[0])){
                rt_kprintf("RS:Cir State3\n");
                state = Circule_State3;
            }
                
        }
        else if(state == Circule_State3){
            if(!IsLose(F.my_segment_R[0])){
                //����ֱ�ߵ�ƽ��б��
                float slope = Point_CalSlope((point_t){0,Image_S.leftBroder[0]},(point_t){69,Image_S.leftBroder[69]});
                //����Զ���Ľǵ���в���
                if(IsArc(F.my_segment_R[0])){
                    int seed = Line_FindMinPoint(Image_S.rightBroder,F.my_segment_R[0].begin,F.my_segment_R[0].end);
                    Vision_SetLineWithPointK(Image_S.rightBroder,seed,-slope,0,69);
                }
                else if(IsCorner(F.my_segment_R[0]))
                    Vision_SetLineWithPointK(Image_S.rightBroder,F.my_segment_R[2].begin,-slope,0,69);
            }
            else if(IsLose(F.my_segment_R[0])){
                state = Circule_Stop;
                BUZZER_SPEAK;
                if(Start_Flag){
                    Car_Change_Speed(0,0,0);
                    //����Բ�� ͬʱ����Ѱ��
                    rt_kprintf("task:ready to get into the circulehandle task\n");
                    rt_sem_release(circule_handle_sem);
                    rt_sem_take(trace_line_sem,RT_WAITING_FOREVER);
                    // rt_thread_delay(1000);
                    rt_kprintf("task:return to the traceline thread\n");
                }
                
            }
        }
        else if(state == Circule_Stop){
            if(IsLose(F.my_segment_R[0])){
                float slope = Point_CalSlope((point_t){0,Image_S.leftBroder[0]},(point_t){69,Image_S.leftBroder[69]});
                Vision_SetLineWithPointK(Image_S.rightBroder,F.feature_p_R[0].x,slope,0,69);
            }
            else if(!IsLose(F.my_segment_R[0])){
                state = Circule_Begin;
                Current_Road = NormalRoads;
                MCX_Change_Mode(MCX_Detection_Mode);
                rt_kprintf("RS:Out of Cir\n");
            }
        }

    }
    
    else{//���Բ��
         if(state == Circule_State1){
            if(F.FP_n_L && !IsLose(F.my_segment_L[0]))
                Vision_ExtendLine(Image_S.leftBroder,F.feature_p_L[0].x,1);
            if(IsLose(F.my_segment_L[0])){
                rt_kprintf("RS:Cir State2\n");
                state = Circule_State2;
            }     
        }
        else if(state == Circule_State2){
            //��ֱ��
            MCX_Change_Mode(MCX_Reset_Mode);
            if(IsLose(F.my_segment_L[0])){
                //����ֱ�ߵ�ƽ��б��
                float slope = Point_CalSlope((point_t){0,Image_S.rightBroder[0]},(point_t){69,Image_S.rightBroder[69]});
                //�õ�Բ������С�� 
                int seed = Line_FindMaxPoint(Image_S.leftBroder,F.my_segment_L[1].begin,F.my_segment_L[1].end);
                Vision_SetLineWithPointK(Image_S.leftBroder,seed,-slope,0,69);
            }
                
            else if(!IsLose(F.my_segment_L[0])){
                rt_kprintf("RS:Cir State3\n");
                state = Circule_State3;
            }       
        }
        else if(state == Circule_State3){
            if(!IsLose(F.my_segment_L[0])){
                //����ֱ�ߵ�ƽ��б��
                float slope = Point_CalSlope((point_t){0,Image_S.rightBroder[0]},(point_t){69,Image_S.rightBroder[69]});
                //����Զ���Ľǵ���в���
                if(IsArc(F.my_segment_L[0])){
                    int seed = Line_FindMaxPoint(Image_S.leftBroder,F.my_segment_L[0].begin,F.my_segment_L[0].end);
                    Vision_SetLineWithPointK(Image_S.leftBroder,seed,-slope,0,69);
                }
                else if(IsCorner(F.my_segment_L[0]))
                    Vision_SetLineWithPointK(Image_S.leftBroder,F.my_segment_L[2].begin,-slope,0,69);
                    
            }
            else if(IsLose(F.my_segment_L[0])){
                state = Circule_Stop;
                BUZZER_SPEAK;

                if(Start_Flag){
                    Car_Change_Speed(0,0,0);
                    //����Բ�� ͬʱ����Ѱ��
                    rt_kprintf("task:ready to get into the circulehandle task\n");
                    rt_sem_release(circule_handle_sem);
                    rt_sem_take(trace_line_sem,RT_WAITING_FOREVER);
                    // rt_thread_delay(1000);
                    rt_kprintf("task:return to the traceline thread\n");
                }
            }
        }
        else if(state == Circule_Stop){
            if(IsLose(F.my_segment_L[0])){
                float slope = Point_CalSlope((point_t){0,Image_S.rightBroder[0]},(point_t){69,Image_S.rightBroder[69]});
                Vision_SetLineWithPointK(Image_S.leftBroder,F.feature_p_L[0].x,slope,0,69);
            }
            else if(!IsLose(F.my_segment_L[0])){
                state = Circule_Begin;
                Current_Road = NormalRoads;
                MCX_Change_Mode(MCX_Detection_Mode);
                rt_kprintf("RS:Out of Cir\n");
            }
						
        }
    }
}

/**
 * @brief �����ߴ�����
 * 
 */
void Vision_ZebraHandle(){
    if(Start_Flag){
        if(!final_flag){
            rt_kprintf("Vision:Zebra Detected\n");
            rt_sem_release(final_sem);
            rt_sem_take(trace_line_sem,RT_WAITING_FOREVER);
            rt_kprintf("return from the final thread\n");
            MCX_Change_Mode(MCX_Reset_Mode);
            Current_Road = NormalRoads;
        }
        else{
        //�յ�ͣ��
            Car_Change_Speed(0,0,0);
            rt_thread_delay(10);
            Car_DistanceMotion(0,-50,0.8);
            rt_kprintf("Task Finished\n");
            while(1){
                rt_thread_delay(10);
            }

        }
    }
    Current_Road = NormalRoads;

}


