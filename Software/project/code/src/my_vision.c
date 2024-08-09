#include "my_vision.h"
#include "zf_device_tft180.h"
#include "string.h"

int block = 7;
int adaptivePara = 6;

RoadSymbol_type Current_Road;

// 圆环左右标识位
uint8_t Circule_LorR;

//路况特征判断结构体
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
 * @brief 根据赛道元素选择处理
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
 * @brief 检测路段状况
 * 
 */
//双边弧
#define CornerState1    (IsArcCorner(F.my_segment_L[0])&&IsArcCorner(F.my_segment_L[0])&&F.segment_n_L == 1&&F.segment_n_R == 1)
//双边弧+缺陷
#define CornerState2    (IsArcCorner(F.my_segment_L[0])&&IsArcCorner(F.my_segment_L[0])\
                        &&IsLose(F.my_segment_L[1])&&IsLose(F.my_segment_R[1])\
                        && F.segment_n_L == 2 && F.my_segment_R == 2) 
//单边缺线
#define CornerState3    (IsArcCorner(F.my_segment_L[0])&&IsLose(F.my_segment_L[1])&&IsLose(F.my_segment_R[0])&&F.segment_n_R == 1\
                       ||IsArcCorner(F.my_segment_R[0])&&IsLose(F.my_segment_R[1])&&IsLose(F.my_segment_L[0])&&F.my_segment_L == 1)

#define CrossCon1       (F.FP_n_L == 2&&F.FP_n_R == 2)
#define CrossCon2       ((F.FP_n_L == 1&&F.FP_n_R == 2)||(F.FP_n_R == 1&&F.FP_n_L == 2))
#define CrossCon3       ((IsLose(F.my_segment_L[0])&&F.segment_n_L == 1&&F.FP_n_L == 0&&F.FP_n_R == 2)||\
                         (IsLose(F.my_segment_R[0])&&F.segment_n_R == 1&&F.FP_n_R == 0&&F.FP_n_L == 2))
//十字特殊情况：识别到了圆环的弯道
#define CrossCon4       (((IsLose(F.my_segment_R[0])&&!IsNull(F.my_segment_R[1]))&&(F.segment_n_L <= 2&&IsArc(F.my_segment_L[0])&&F.FP_n_L))||\
                         ((IsLose(F.my_segment_L[0])&&!IsNull(F.my_segment_L[1]))&&(F.segment_n_R <= 2&&IsArc(F.my_segment_R[0])&&F.FP_n_L)))

void Vision_SymbolJudge()
{
    //获取边界分段信息
    Vision_GetSegment(Image_S.leftBroder,1);
    Vision_GetSegment(Image_S.rightBroder,0);
    
    //寻找特征点
    Vision_BroderFindFP(Image_S.leftBroder);
    Vision_BroderFindFP(Image_S.rightBroder);

    char str[100];
    rt_sprintf(str,"left_p:%d",F.FP_n_L);
    ips200_show_string(200,35,str);
    rt_sprintf(str,"right_p:%d",F.FP_n_R);
    ips200_show_string(200,20,str);

    //只有当道路情况为正常道路时才需要进行判断
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
 * @brief 打印特征点
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
 * @brief 将边界分割函数，将边界分割为
 *          LorR 1---L 0---R
 * 
 * @param broder 目标边界
 * @param LorR   左右边界 因为缺线的值不同
 */
void Vision_GetSegment(int16* broder,uint8_t LorR)
{
    segment_t* target_segment = LorR? F.my_segment_L:F.my_segment_R;
    uint8_t begin_flag = 1;     //序列记录开始标志位
    int segment_n = 0;          //用于记录第几段序列

    //清空上一次的数据 clear
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
    //将边界分段 并且标记
    for(int i = imgRow-1 ; i>0 ; i--)
    {
        //对第一个点进行处理
        if(begin_flag){
            target_segment[segment_n].begin = i == imgRow-1? i:i+1;
           
           //标定结果
           //丢线判定：三个像素均判定为丢线则认为为丢线
           //不是丢线则标记为未定位
            if(LorR){ //左边
                if((i-2>=0)&&broder[i]<=3&&broder[i-1]<=3){
                    target_segment[segment_n].type = lose_segment;
                    broder[i] = LEFT_LOSE_VALUE;
                    broder[i-1] = LEFT_LOSE_VALUE;
                    broder[i-2] = LEFT_LOSE_VALUE;
                }
                else
                    target_segment[segment_n].type = NULL_segment;
            }
            else if((!LorR)){//右边
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
        //记录结尾
        //
        if(target_segment[segment_n].type == lose_segment){
            //对于最后几个点不做判断
            if(i>=3){
                //防止突变 连续三个量都离边界较远时认为是lose
                if((LorR && broder[i]>5 && broder[i-1]>5 && broder[i-2]>5 && broder[i-3]>5)||    //左边
                    ((!LorR) && broder[i]<RIGHT_LOSE_VALUE-5 && broder[i-1]<RIGHT_LOSE_VALUE-5 && broder[i-2]<RIGHT_LOSE_VALUE-5 && broder[i-3]<RIGHT_LOSE_VALUE-5)){ //右边
                    target_segment[segment_n].end = i+1;//记录结尾
                    segment_n++;
                    begin_flag = 1;
                }
                else 
                    broder[i] = (LorR)? LEFT_LOSE_VALUE:RIGHT_LOSE_VALUE;
            }
            else
            ;
        }
        //若第一次检测到的不为丢失，则丢线后记录为下一段开始
        else if(target_segment[segment_n].type == NULL_segment){
            if(i>=2){
                //防止突变 连续三个量很小认为是lose
                if((LorR && broder[i]<=5 && broder[i-1]<=5 && broder[i-2]<=5)||    //左边
                    ((!LorR) && broder[i]>=RIGHT_LOSE_VALUE-5 && broder[i-1]>=RIGHT_LOSE_VALUE-5 && broder[i-2]>=RIGHT_LOSE_VALUE-5)){ //右边
                    broder[i] = LorR? LEFT_LOSE_VALUE:RIGHT_LOSE_VALUE;
                    broder[i-1] = LorR? LEFT_LOSE_VALUE:RIGHT_LOSE_VALUE;
                    broder[i-2] = LorR? LEFT_LOSE_VALUE:RIGHT_LOSE_VALUE;
                    target_segment[segment_n].end = i+1;//记录结尾
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

    // 当第一个序列太短时，与第二个序列合并
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


    //记录数量
    if(LorR)
        F.segment_n_L = segment_n+1;
    else
        F.segment_n_R = segment_n+1;

    //对于边界进行判断
    for(int i = 0;i<segment_n+1;i++)
    {
       
        if(target_segment[i].type == NULL_segment){
             //是否为直线
            int straight = Line_IsStraight(broder,target_segment[i].begin,target_segment[i].end);
            if(straight){
                target_segment[i].type = straight_segment;
            }
            //根据单调性来对弯道以及圆弧进行判定
            else{
                int mono;
                mono = Line_IsMonotonous(broder,target_segment[i].begin,target_segment[i].end);
                //函数单调则为弯道，函数不单调则为圆弧
                target_segment[i].type = mono? corner_segment : arc_segment;
            }
        }
    }


}

/**
 * @brief 寻找一段弧的第一个特征点，方向由大到小
 * 
 * @param broder 边界
 * @param x1    上域
 * @param x2    下区域
 * @return point_t 返回第一个特征点的坐标 若返回值为-1，则没有发现特征点
 */
point_t Vision_FindArcFP(int16 *broder,int x1,int x2)
{
    int max = Tool_CmpMax(x1,x2);
    int min = Tool_CmpMin(x1,x2);

    int how_many = max - min + 1;
    int final_x = max - 1;

    int p_distance = 5;//检测区域
    float p_th = -0.3;//判定为角点的阈值
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

    //边界的点不考虑
    for(int i = max - 2; i>min+1; i--)
    {
        int low_x = (i-min)>=5?(i - 5):min;
        int high_x = (max - i)>=5?(i + 5):max;

        //滤去突变量
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
    //找到特征点
    if(cosvalue > p_th)
        return (point_t){final_x,broder[final_x]};
    //未找到特征点
    else
        return (point_t){-1,-1};
}

/**
 * @brief 寻找边界特征点
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

    //清空数据
    (*target_n) = 0;
    for(int i = 0;i<3;i++){
        target_FP[i].x = 0;
        target_FP[i].y = 0;
    }

    /*---------------角点检测---------------*/
    int aver1;
    int aver2;
    point_t pf;
    //中间缺线 且缺线达到阈值
    if(IsLose(target_seg[1])&&Vision_IsLone(target_seg[1])){
        aver1 = Line_GetAverage(broder,target_seg[0].begin,target_seg[0].end);
        
        //检测近处序列的角点
        pf = Vision_FindArcFP(broder,target_seg[0].begin,target_seg[0].end);
        //若没有发现角点
        if((pf.x == -1 && pf.y == -1)){
            target_FP[0].x = target_seg[0].end;
            target_FP[0].y = broder[target_seg[0].end];
            (*target_n)++;
            //计算近处序列的平均斜率 用于预防u形弯误判
        
            //*检测远处序列的角点  当远处序列有值的时候检测 有值的时候必定有角点*
            if(!IsNull(target_seg[2])){
                //若远处序列和近处序列差别太大，认为为突变
                aver2 = Line_GetAverage(broder,target_seg[2].begin,target_seg[2].end);
                if(fabs(aver1 - aver2)>80){
                    target_FP[0].x = -1;
                    target_FP[0].y = -1;
                    (*target_n)--;
                    return;
                }
                //只寻找较近序列的特征点
                pf = (target_seg[2].begin - target_seg[2].end >= 15)?
                    Vision_FindArcFP(broder,target_seg[2].begin,target_seg[2].begin - 15) :
                    Vision_FindArcFP(broder,target_seg[2].begin,target_seg[2].end);
                    //*存在角点记录角点，不存在记录边界点*
                target_FP[1].x = (pf.x == -1) ? target_seg[2].begin : pf.x ;
                target_FP[1].y = (pf.y == -1) ? broder[target_seg[2].begin] : pf.y;
                (*target_n)++;
            }

        }//发现了角点
        else{
            target_FP[0] = pf;
            (*target_n)++;

           //*检测远处序列的角点  当远处序列有值的时候检测 有值的时候必定有角点*
          if(!IsNull(target_seg[2])){
                //若远处序列和近处序列差别太大，认为为突变
                aver2 = Line_GetAverage(broder,target_seg[2].begin,target_seg[2].end);
                if(fabs(aver1 - aver2)>80){
                    target_FP[0].x = -1;
                    target_FP[0].y = -1;
                    (*target_n)--;
                    return;
                }
                //只寻找较近序列的特征点
                pf = (target_seg[2].begin - target_seg[2].end >= 15)?
                    Vision_FindArcFP(broder,target_seg[2].begin,target_seg[2].begin - 15) :
                    Vision_FindArcFP(broder,target_seg[2].begin,target_seg[2].end);
                //*存在角点记录角点，不存在记录边界点*
                target_FP[1].x = (pf.x == -1) ? target_seg[2].begin : pf.x ;
                target_FP[1].y = (pf.y == -1) ? broder[target_seg[2].begin] : pf.y;
                (*target_n)++;
            }
        }
    }

    //开头缺线
    else if(target_seg[0].type == lose_segment){
        //检测远处序列的角点  当远处序列有值的时候检测
        if(!IsNull(target_seg[1])){
            pf = (target_seg[1].begin - target_seg[1].end >= 15)?
                Vision_FindArcFP(broder,target_seg[1].begin,target_seg[1].begin - 15) :
                Vision_FindArcFP(broder,target_seg[1].begin,target_seg[1].end);
            //当未发现角点
            target_FP[0].x = (pf.x == -1) ? target_seg[1].begin : pf.x ;
            target_FP[0].y = (pf.y == -1) ? broder[target_seg[1].begin] : pf.y;
            (*target_n)++;
        }
    }
    //斜入十字特殊情况（十字接环岛） 只寻找近处的角点
    else if(IsArc(target_seg[0])&&segment_n == 1){
        pf = Vision_FindArcFP(broder,target_seg[0].begin,25);
        if(pf.x != -1&&pf.y != -1){
					target_FP[0] = pf;
					(*target_n)++;
        }
    }
}

/**
 * @brief 判断前方是否有斑马线
 * 
 * @return uint8_t 返回1为发现前方斑马线
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
 * @brief 打印边界数组
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
 * @brief 意外情况打印信息
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
 * @brief 延长线段到边界 但是固定K值
 *          direction --- 补线方向
 *          1---向远处补线 0---向近处补线
 * 
 * @param broder 边界数组
 * @param x 起始点
 * @param direction 补线方向
 * @param slope 斜率
 */
#define FAR_Extend 1
#define NEAR_Extend 0
void Vision_ExtendLineK(int16 *broder,int x,int direction,float slope){
    //向远处补线
    if(direction){
        for(int i = x-1 ; i > 0 ; i--)
        {
            broder[i] = broder[x] - slope * (x - i);

            broder[i] = broder[i]<0? 0:broder[i];
            broder[i] = broder[i]>159?159:broder[i];
            
            broder[i] = broder[i]>imgCol-1? imgCol-1:broder[i];
        }
    }
    //向近处补线
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
 * @brief 计算边界缺陷度函数
 * 
 * @param broder 对应边界
 * @return float 缺陷率
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
 * @brief 延长线段到边界
*   direction --- 补线方向
*   1---向远处补线 0---向近处补线
*/
void Vision_ExtendLine(int16 *broder,int x,int direction)
{   
    float this_K;
    //向远处补线
    if(direction){

        this_K = (x+5<=imgRow-1)?(float)(broder[x + 5] - broder[x])/5.0f:(float)(broder[imgRow-1] - broder[x])/(float)(imgRow - 1 - x);
        for(int i = x-1 ; i > 0 ; i--)
        {
            broder[i] = broder[x] - this_K * (x - i);
            broder[i] = broder[i]<0? 0:broder[i];
            broder[i] = broder[i]>imgCol-1? imgCol-1:broder[i];
        }
    }
    //向近处补线
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
 * @brief 补线函数，告诉两个点的数组下表，将对应边界两点间的值补充为直线
 * 
 * @param p1 横坐标1
 * @param p2 横坐标2
 * @param broder 边界
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
 * @brief 将边界对应段置于缺陷状态
 * 
 * @param broder 边界
 * @param x1 段首
 * @param x2 段尾
 */
void Vision_SetLose(int16* broder,int16 x1,int16 x2){
    int max = Tool_CmpMax(x1,x2);
    int min = Tool_CmpMin(x1,x2);

    for(int i = min;i<max;i++){
        broder[i] = (broder == Image_S.leftBroder)? LEFT_LOSE_VALUE:RIGHT_LOSE_VALUE;
    }
}

/**
 * @brief 基于一个点和一个固定的斜率画直线
 * 
 * @param broder 边界
 * @param seed   某一点坐标坐标
 * @param slope  斜率
 * @param x1     直线范围1
 * @param x2     直线范围2
*/
void Vision_SetLineWithPointK(int16* broder,int seed,float slope,int x1,int x2){
    int max = Tool_CmpMax(x1,x2);
    int min = Tool_CmpMin(x1,x2);

    for(int i = min; i<=max ;i++){
        broder[i] = broder[seed] + (i - seed)*slope;
    }
}

/**
 * @brief 十字赛道处理
 * 
 */
#define Cross_Begin   0
#define Cross_State_1 1
void Vision_CrossHandle()
{
    static int state = Cross_Begin;

        //状态切换
    if(state == Cross_Begin){
        //当进入十字的时候发现两边都未能发现角点 退出十字
        //十字的时候应当有明显的角点
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
    //补线
    //斜入十字
    if(CrossCon4){
        //左边
        if (IsArc(F.my_segment_L[0])&&F.segment_n_L == 1){
            //此时应当左边找到一个特征点 计算特征点到另一边角点的斜率
            Vision_ExtendLine(Image_S.leftBroder,F.feature_p_L[0].x,1);
            Vision_ExtendLine(Image_S.rightBroder,F.feature_p_R[0].x,0);
        }
        //右边
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
 * @brief 弯道处理
 * 
 */
void Vision_CornerHandle()
{
    if(CornerState1){
        //暂时认为如果两边都是1的话不需要处理
        //就算单边全丢线 ，但是另一边仍然能够使得中线偏离
    }
    else if(CornerState2){
        if(F.my_segment_L[1].end - F.my_segment_L[1].begin > F.my_segment_R[1].end - F.my_segment_R[1].begin){
            //右边缺的少 左转 
            for(int i = F.my_segment_R[1].begin;i>F.my_segment_R[1].end;i--){
                Image_S.rightBroder[i] = LEFT_LOSE_VALUE;
            }
        }
        else if(F.my_segment_L[1].end - F.my_segment_L[1].begin < F.my_segment_R[1].end - F.my_segment_R[1].begin){
            //左边缺的少 右边转 
            for(int i = F.my_segment_L[1].begin;i>F.my_segment_L[1].end;i--){
                Image_S.leftBroder[i] = LEFT_LOSE_VALUE;
            }
        }
    }
    else if(CornerState3){
        //左边缺线 将右边部分向左移
        if(IsLose(F.my_segment_L[0]))
            for(int i = F.my_segment_R[1].begin;i>F.my_segment_R[1].end;i--){
                Image_S.rightBroder[i] = LEFT_LOSE_VALUE;
            }
        //右边缺线 将左边部分向右移
        else if(IsLose(F.my_segment_R[0]))
            for(int i = F.my_segment_L[1].begin;i>F.my_segment_L[1].end;i--){
                Image_S.leftBroder[i] = LEFT_LOSE_VALUE;
            }
    }

    Current_Road = NormalRoads;
}

/**
 * @brief 圆环处理
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
        //防止误识别
        // MCX_Change_Mode(MCX_Reset_Mode);
        MCX_Clear();
        if(IsStrai(F.my_segment_L[0]) && F.segment_n_L == 1)
            Circule_LorR = RIGHT_CIRCULE;
        else if(IsStrai(F.my_segment_R[0]) && F.segment_n_R == 1)
            Circule_LorR = LEFT_CIRCULE;
        state = Circule_State1;
    }
    //防止错误判断
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

    if(Circule_LorR == RIGHT_CIRCULE){//右边圆环
        if(state == Circule_State1){
            if(F.FP_n_R && !IsLose(F.my_segment_R[0]))
                Vision_ExtendLine(Image_S.rightBroder,F.feature_p_R[0].x,1);
            if(IsLose(F.my_segment_R[0])){
                state = Circule_State2; 
                rt_kprintf("RS:Cir State2\n");
            }
                
        }
        else if(state == Circule_State2){
            //做直线
            MCX_Change_Mode(MCX_Reset_Mode);
            if(IsLose(F.my_segment_R[0])){
                //计算直线的平均斜率
                float slope = Point_CalSlope((point_t){0,Image_S.leftBroder[0]},(point_t){69,Image_S.leftBroder[69]});
                //得到圆弧的最小点
                int seed = Line_FindMinPoint(Image_S.rightBroder,F.my_segment_R[1].begin,F.my_segment_R[1].end);
                //负的斜率因为没做透视变换
                Vision_SetLineWithPointK(Image_S.rightBroder,seed,-slope,0,69);
            }
                
            else if(!IsLose(F.my_segment_R[0])){
                rt_kprintf("RS:Cir State3\n");
                state = Circule_State3;
            }
                
        }
        else if(state == Circule_State3){
            if(!IsLose(F.my_segment_R[0])){
                //计算直线的平均斜率
                float slope = Point_CalSlope((point_t){0,Image_S.leftBroder[0]},(point_t){69,Image_S.leftBroder[69]});
                //根据远处的角点进行补线
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
                    //启动圆环 同时阻塞寻仙
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
    
    else{//左边圆环
         if(state == Circule_State1){
            if(F.FP_n_L && !IsLose(F.my_segment_L[0]))
                Vision_ExtendLine(Image_S.leftBroder,F.feature_p_L[0].x,1);
            if(IsLose(F.my_segment_L[0])){
                rt_kprintf("RS:Cir State2\n");
                state = Circule_State2;
            }     
        }
        else if(state == Circule_State2){
            //做直线
            MCX_Change_Mode(MCX_Reset_Mode);
            if(IsLose(F.my_segment_L[0])){
                //计算直线的平均斜率
                float slope = Point_CalSlope((point_t){0,Image_S.rightBroder[0]},(point_t){69,Image_S.rightBroder[69]});
                //得到圆弧的最小点 
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
                //计算直线的平均斜率
                float slope = Point_CalSlope((point_t){0,Image_S.rightBroder[0]},(point_t){69,Image_S.rightBroder[69]});
                //根据远处的角点进行补线
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
                    //启动圆环 同时阻塞寻仙
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
 * @brief 斑马线处理函数
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
        //终点停车
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


