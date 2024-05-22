#include "my_vision.h"
#include "string.h"

int block = 7;
int adaptivePara = 6;

uint8 handle_image[IMAGE_ROW][IMAGE_COL];

RoadSymbol_type Current_Road;

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

//将图片拷贝到自己的图像中来
void Vision_GetMyImage()
{
    for(int i=0;i<imgRow;i++){
        for(int j=0;j<imgCol;j++)
        {
            handle_image[i][j] = mt9v03x_image[i][j];
        }
    }   
}

//
// @brief 大津法
//
int My_Adapt_Threshold(uint8*image,uint16 width, uint16 height)   //大津算法，注意计算阈值的一定要是原图像
{
    #define GrayScale 256
    int pixelCount[GrayScale];
    float pixelPro[GrayScale];
    int i, j;
    int pixelSum = width * height/4;
    int  threshold = 0;
    uint8* data = image;  //指向像素数据的指针
    for (i = 0; i < GrayScale; i++)
    {
        pixelCount[i] = 0;
        pixelPro[i] = 0;
    }
    uint32 gray_sum=0;
    for (i = 0; i < height; i+=2)//统计灰度级中每个像素在整幅图像中的个数
    {
        for (j = 0; j <width; j+=2)
        {
            pixelCount[(int)data[i * width + j]]++;  //将当前的点的像素值作为计数数组的下标
            gray_sum+=(int)data[i * width + j];       //灰度值总和
        }
    }
    for (i = 0; i < GrayScale; i++) //计算每个像素值的点在整幅图像中的比例
    {
        pixelPro[i] = (float)pixelCount[i] / pixelSum;
    }
    float w0, w1, u0tmp, u1tmp, u0, u1, u, deltaTmp, deltaMax = 0;
    w0 = w1 = u0tmp = u1tmp = u0 = u1 = u = deltaTmp = 0;
    for (j = 0; j < GrayScale; j++)//遍历灰度级[0,255]
    {
        w0 += pixelPro[j];  //背景部分每个灰度值的像素点所占比例之和   即背景部分的比例
        if(pixelPro[j] == 0)
            continue;

        u0tmp += j * pixelPro[j];  //背景部分 每个灰度值的点的比例 *灰度值
        w1=1-w0;
        u1tmp=gray_sum/pixelSum-u0tmp;
        u0 = u0tmp / w0;              //背景平均灰度
        u1 = u1tmp / w1;              //前景平均灰度
        u = u0tmp + u1tmp;            //全局平均灰度
        deltaTmp = w0 * pow((u0 - u), 2) + w1 * pow((u1 - u), 2);//平方
        if (deltaTmp > deltaMax)
        {
            deltaMax = deltaTmp;//最大类间方差法
            threshold = j;
        }
        if (deltaTmp < deltaMax)
        {
            break;
        }
    }
    if(threshold>255)
        threshold=255;
    if(threshold<0)
        threshold=0;
  return threshold;
}

void adaptiveThreshold(uint8_t *img_data, uint8_t *output_data, int width, int height, int block, uint8_t clip_value)
{
    assert(block % 2 == 1); // block必须为奇数
    int half_block = block / 2;
    for(int y=half_block; y<height-half_block; y++){
        for(int x=half_block; x<width-half_block; x++){
        // 计算局部阈值
        int thres = 0;
        for(int dy=-half_block; dy<=half_block; dy++){
            for(int dx=-half_block; dx<=half_block; dx++){
                thres += img_data[(x+dx)+(y+dy)*width];
            }
        }
        thres = thres / (block * block) - clip_value;
        // 进行二值化
        output_data[x+y*width] = img_data[x+y*width]>thres ? 255 : 0;
        }
    }
}

/**
 * @brief 双边最长白边界法
 *  
 * 
*/
int Longest_White_Column_Left[2];
int Longest_White_Column_Right[2];
int White_Column[IMAGE_COL];//每列白列长度
int Center;
void Vision_FindBoundery(){

    int start_column = 0;
    int end_column = IMAGE_COL;
    //从左到右，从下往上，遍历全图记录范围内的每一列白点数量
		for(int j =start_column; j<=end_column; j++ ){
				White_Column[j] = 0;
		}
	
    for (int j =start_column; j<=end_column; j++)
    {
        for (int i = IMAGE_ROW - 1; i >= 0; i--)
        {
            if(my_image[i][j] == 0){
                break;
						}
            else
                White_Column[j]++;
        }
    }
    //从左到右找左边最长白列
    Longest_White_Column_Left[0] = 0;
    for(int i=start_column;i<=end_column;i++)
    {
        if (Longest_White_Column_Left[0] < White_Column[i])//找最长的那一列
        {
            Longest_White_Column_Left[0] = White_Column[i];//【0】是白列长度
            Longest_White_Column_Left[1] = i;              //【1】是下标，第j列
        }
    }
    //从右到左找右左边最长白列
    Longest_White_Column_Right[0] = 0;
    for(int i=end_column;i>=start_column;i--)//从右往左，注意条件，找到左边最长白列位置就可以停了
    {
        if (Longest_White_Column_Right[0] < White_Column[i])//找最长的那一列
        {
            Longest_White_Column_Right[0] = White_Column[i];//【0】是白列长度
            Longest_White_Column_Right[1] = i;              //【1】是下标，第j列
        }

    }
    Center = (Longest_White_Column_Left[1]+Longest_White_Column_Right[1]) / 2;
}


//
//@breif 将边界分割函数，将边界分割为
//      LorR 1---L 0---R
//
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
           
           //标定结果
           //丢线判定：三个像素均判定为丢线则认为为丢线
           //不是丢线则标记为未定位
            if(LorR){ //左边
                if(broder[i]<=3&&broder[i-1]<=3&&broder[i-2]<=3)
                    target_segment[segment_n].type = lose_segment;
                else
                    target_segment[segment_n].type = NULL_segment;
            }
            else if((!LorR)){//右边
                if(broder[i]>=156&&broder[i-1]>=156&&broder[i-2]>=156)
                    target_segment[segment_n].type = lose_segment;
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
            if(i>=2){
                if((LorR && broder[i]>3 && broder[i-1]>3 && broder[i-2]>3)||    //左边
                    ((!LorR) && broder[i]<156 && broder[i-1]<156 && broder[i-2]<156)){ //右边
                    target_segment[segment_n].end = i+1;//记录结尾
                    segment_n++;
                    begin_flag = 1;
                }
            }
            else
            ;
        }
        //若第一次检测到的不为丢失，则丢线后记录为下一段开始
        else if(target_segment[segment_n].type == NULL_segment){
            if(i>=2){
                if((LorR && broder[i]<=3 && broder[i-1]<=3 && broder[i-2]<=3)||    //左边
                    ((!LorR) && broder[i]>=156 && broder[i-1]>=156 && broder[i-2]>=156)){ //右边
                    target_segment[segment_n].end = i+1;//记录结尾
                    segment_n++;
                    begin_flag = 1;
                }
            }
            else
            ;
        }
    }

    //当第一个序列只有一个值的时候，将第一个序列和第二个合并
    if(target_segment[0].begin == target_segment[0].end){
        broder[target_segment[0].begin] = broder[target_segment[0].begin + 1];
        target_segment[0].end = target_segment[1].end;
        for(int i = 1 ;i<segment_n;i++)
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

//
//寻找弯道的特征点
//
point_t Vision_FindCornerFP(int16 *broder,int x1,int x2)
{
    int max = Tool_CmpMax(x1,x2);
    int min = Tool_CmpMin(x1,x2);


}

//
//寻找弧的特征点
//
point_t Vision_FindArcFP(int16 *broder,int x1,int x2)
{
    int max = Tool_CmpMax(x1,x2);
    int min = Tool_CmpMin(x1,x2);

    int how_many = max - min + 1;
    int final_x;//存储最后的坐标

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
    //边界的点不考虑
    for(int i = min+2; i<max - 2 ; i++)
    {
        int low_x = (i-min)>=5?(i - 5):min;
        int high_x = (max - i)>=5?(i + 5):max;

        //检测到cos小的时候更新坐标值
        final_x = (cosvalue>Vector_AngleGet((point_t){low_x,broder[low_x]},
                                            (point_t){i,broder[i]},
                                            (point_t){high_x,broder[high_x]}))?
                                            i:final_x;
    }
    return (point_t){final_x,broder[final_x]};
}

//
// 寻找边界特征点
//
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

    //清空数据
    (*target_n) = 0;
    for(int i = 0;i<4;i++){
        target_FP[0].x = 0;
        target_FP[0].y = 0;
    }

    //针对常见情况的寻找特征点
    if(target_seg[1].type == lose_segment){
        //首先判断边界点是否是间断点
        if(fabs(broder[target_seg[0].end] - lose_value) > 15){
            target_FP[0].x = target_seg[0].end;
            target_FP[0].y = broder[target_FP[0].x];
            (*target_n)++;
            //寻找远处的间断点
            if(target_seg[2].type != NULL_segment){
                //边界点判断
                if(fabs(broder[target_seg[2].begin] - lose_value) >= 15){
                    target_FP[1].x = target_seg[2].begin;
                    target_FP[1].y = broder[target_FP[1].x];
                    (*target_n)++;
                }
                //通过角度寻找角点
                else{
                    target_FP[1] = Vision_FindArcFP(broder,target_seg[0].begin,target_seg[0].end);
                    (*target_n)++;
                }
            }
        }
        //若边界点不是 则通过角度来寻找间断点
        else{
            target_FP[0] = Vision_FindArcFP(broder,target_seg[0].begin,target_seg[0].end);
            (*target_n)++;
            //寻找远处的间断点
            if(target_seg[2].type != NULL_segment){
                //边界点判断
                if(fabs(broder[target_seg[2].begin] - lose_value) >= 15){
                    target_FP[1].x = target_seg[2].begin;
                    target_FP[1].y = broder[target_FP[1].x];
                    (*target_n)++;
                }
                //通过角度寻找角点
                else{
                    target_FP[0] = Vision_FindArcFP(broder,target_seg[0].begin,target_seg[0].end);
                    (*target_n)++;
                }
            }
        }
        
    }
    //针对斜着进入十字 只能找到一个间断点
    //并且针对十字中间的状态，也只能找到一个间断点
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

//
//打印边界数组
//
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

//
//@brief 意外状况 打印信息
//
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

//
//@brief 检测路段状况
//
void Vision_SymbolJudge()
{
    //获取边界分段信息
    Vision_GetSegment(Image_S.leftBroder,1);
    Vision_GetSegment(Image_S.rightBroder,0);
    
    //寻找特征点

    Vision_BroderFindFP(Image_S.leftBroder);
    Vision_BroderFindFP(Image_S.rightBroder);

    //只有当道路情况为正常道路时才需要进行判断
    if(Current_Road == NormalRoads){
        //根据特征点判断
        int state_code = F.FP_n_L*10+F.FP_n_R;
        switch (state_code)
        {
        case 0:
            //两边都没间断点
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

#define Cross_State_1 1
#define Cross_State_2 2
#define Leave_Cross   3
//
//@breif 十字赛道解决
//
void Vision_CrossHandle()
{
    static int state;
    //一个角点向近处补线
    //两个角点连线
    switch (F.FP_n_L)
    {
    case 1:
        //Vision_ExtendLine(Image_S.leftBroder,F.feature_p_L[0].x,0);
        break;
    case 2:
        //Vision_set_AdditionalLine(F.feature_p_L[0].x,F.feature_p_L[1].x,Image_S.leftBroder);
        break;
    default:
        break;
    }
    switch (F.FP_n_L)
    {
    case 1:
        //Vision_ExtendLine(Image_S.rightBroder,F.feature_p_R[0].x,0);
        break;
    case 2:
        //Vision_set_AdditionalLine(F.feature_p_R[0].x,F.feature_p_R[1].x,Image_S.rightBroder);
        break;
    default:
        break;
    }
    //如果此时两边都缺线
    if((F.my_segment_L[0].type == lose_segment && F.segment_n_L != 1)&&
        F.my_segment_R[0].type == lose_segment && F.segment_n_R != 1)
        state = Cross_State_2;
    //若此时缺线后重新出现线
    if(state == Cross_State_2 && 
        F.my_segment_L[0].type != lose_segment && 
        F.my_segment_R[0].type != lose_segment){
            Current_Road = NormalRoads;
            state = Cross_State_1;
        }


    // if(F.segment_n_L == 2 && F.segment_n_R == 2){
    //     //情况3 十字第三阶段 不存在+存在
    //     if(F.my_segment_L[0].type == lose_segment && F.my_segment_R[0].type == lose_segment){
    //         //向近处补线
    //         Vision_ExtendLine(Image_S.leftBroder,F.my_segment_L[1].begin,0);
    //         Vision_ExtendLine(Image_S.rightBroder,F.my_segment_R[1].begin,0);
    //     }
    //     //情况1 十字第一阶段，存在+不存在线
    //     else if(F.my_segment_L[1].type == lose_segment && F.my_segment_R[1].type == lose_segment){
    //         //向远处补线
    //         Vision_ExtendLine(Image_S.leftBroder,F.my_segment_L[0].end,1);
    //         Vision_ExtendLine(Image_S.rightBroder,F.my_segment_R[0].end,1);
    //     }
    //     //情况4 斜着进入
    //     else if(F.my_segment_L[0].type == arc_segment){
    //         //找到突变点
    //         int maxpoint = Line_FindMaxPoint(Image_S.leftBroder,F.my_segment_L[0].begin,F.my_segment_L[0].end);
    //         //突变点补线
    //         Vision_ExtendLine(Image_S.leftBroder,maxpoint,1);
    //     }
    //     else if(F.my_segment_R[0].type == arc_segment){
    //         //找到突变点
    //         int maxpoint = Line_FindMaxPoint(Image_S.rightBroder,F.my_segment_R[0].begin,F.my_segment_R[0].end);
    //         //突变点补线
    //         Vision_ExtendLine(Image_S.rightBroder,maxpoint,1);
    //     }
    // }

    // //情况2 十字第二阶段，存在+不存在+存在 或者是斜着进入
    // else if(F.segment_n_L == 3 && F.segment_n_R == 3){
    //     if(F.my_segment_L[0].type == arc_segment && F.my_segment_R[0].type == lose_segment)
    //     {
    //         //找到突变点
    //         int maxpoint = Line_FindMaxPoint(Image_S.leftBroder,F.my_segment_L[0].begin,F.my_segment_L[0].end);
    //         //突变点补线
    //         Vision_ExtendLine(Image_S.leftBroder,maxpoint,1);
    //     }
    //     else if(F.my_segment_R[0].type == arc_segment && F.my_segment_L[0].type == lose_segment)
    //     {
    //         //找到突变点
    //         int maxpoint = Line_FindMaxPoint(Image_S.rightBroder,F.my_segment_R[0].begin,F.my_segment_R[0].end);
    //         //突变点补线
    //         Vision_ExtendLine(Image_S.rightBroder,maxpoint,1);
    //     }
    //     else
    //     {
    //         //有时候十字表现可能不是瞬间缺线
    //         int begin = F.my_segment_L[0].end;
    //         int end = F.my_segment_L[2].begin;
    //         //当该点的斜率过于大时，认定为该点为悬崖上的一点，且将该点前移
    //         while(fabs(Image_S.leftBroder[begin] - Image_S.leftBroder[begin+1])>15){
    //             begin = begin+1;//begin 前移
    //         }

    //         while((fabs(Image_S.leftBroder[end] - Image_S.leftBroder[end-1])>15)){
    //             end = end - 1;//end 后移
    //         }
    //         //进行补线
    //         Vision_set_AdditionalLine(begin,end,Image_S.leftBroder);

    //         begin = F.my_segment_R[0].end;
    //         end = F.my_segment_R[2].begin;
    //         //当该点的斜率过于大时，认定为该点为悬崖上的一点，且将该点前移
    //         while(fabs(Image_S.rightBroder[begin] - Image_S.rightBroder[begin+1])>15){
    //             begin = begin+1;//begin 前移
    //         }

    //         while((fabs(Image_S.rightBroder[end] - Image_S.rightBroder[end-1])>15)){
    //             end = end - 1;//end 后移
    //         }
    //         Vision_set_AdditionalLine(begin,end,Image_S.rightBroder);
    //     }
    // }
    // //情况：十字前面接了弯道，
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
    // //斜着进入十字
    // else if(F.my_segment_L[0].type == arc_segment){
    //     //找到突变点
    //     int maxpoint = Line_FindMaxPoint(Image_S.leftBroder,F.my_segment_L[0].begin,F.my_segment_L[0].end);
    //     //突变点补线
    //     Vision_ExtendLine(Image_S.leftBroder,maxpoint,1);
    // }
    
    // else if(F.my_segment_R[0].type == arc_segment){
    //     //找到突变点
    //     int maxpoint = Line_FindMaxPoint(Image_S.rightBroder,F.my_segment_R[0].begin,F.my_segment_R[0].end);
    //     //突变点补线
    //     Vision_ExtendLine(Image_S.rightBroder,maxpoint,1);
    // }

    // else{
    //     Vision_ErrorLogin();
    //     Vision_BroderPrint();
    // }
   
}

//
//@breif 弯道处理方式
//
void Vision_CornerHandle()
{
    if(F.segment_n_L == 1 && F.segment_n_R == 1){
        //暂时认为如果两边都是1的话不需要处理
        //就算单边全丢线 ，但是另一边仍然能够使得中线偏离
    }
    else if(F.segment_n_L == 2 && F.segment_n_R == 1){
        if(F.my_segment_R[0].type == lose_segment&&F.my_segment_L[1].type == lose_segment){
            //右边弯道缺失，将左边缺失段的值右移
            for(int i = F.my_segment_L[1].begin;i>F.my_segment_L[1].end;i--){
                Image_S.leftBroder[i] = RIGHT_LOSE_VALUE;
            }
        }
    }
    else if(F.segment_n_R == 2 && F.segment_n_L == 1){
            if(F.my_segment_L[0].type == lose_segment&&F.my_segment_R[1].type == lose_segment){
            //左边弯道缺失，将右边缺失段的值左移
            for(int i = F.my_segment_R[1].begin;i>F.my_segment_R[1].end;i--){
                Image_S.rightBroder[i] = LEFT_LOSE_VALUE;
            }
        }
    }

    Current_Road = NormalRoads;
}

//
// @brief 圆环处理方式
//
void Vision_CirculeHandle()
{   
    if(F.segment_n_L == 1){
        if(F.my_segment_R[0].type != lose_segment){
            Vision_ExtendLine(Image_S.rightBroder,F.feature_p_R[0].x,1);
        }
    }
}


//
//@brief 根据道路元素
//
void Vision_RSHandle()
{
    switch (Current_Road)
    {
    case LoseRoads:
        //Car_Stop();
        //while(1){
            //error handle 搜不到线 进入errorhandle
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

//
//@brief 补线函数，告诉两个点的数组下表，将对应边界两点间的值补充为直线
//
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


