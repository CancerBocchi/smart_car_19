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

//��ͼƬ�������Լ���ͼ������
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
// @brief ���
//
int My_Adapt_Threshold(uint8*image,uint16 width, uint16 height)   //����㷨��ע�������ֵ��һ��Ҫ��ԭͼ��
{
    #define GrayScale 256
    int pixelCount[GrayScale];
    float pixelPro[GrayScale];
    int i, j;
    int pixelSum = width * height/4;
    int  threshold = 0;
    uint8* data = image;  //ָ���������ݵ�ָ��
    for (i = 0; i < GrayScale; i++)
    {
        pixelCount[i] = 0;
        pixelPro[i] = 0;
    }
    uint32 gray_sum=0;
    for (i = 0; i < height; i+=2)//ͳ�ƻҶȼ���ÿ������������ͼ���еĸ���
    {
        for (j = 0; j <width; j+=2)
        {
            pixelCount[(int)data[i * width + j]]++;  //����ǰ�ĵ������ֵ��Ϊ����������±�
            gray_sum+=(int)data[i * width + j];       //�Ҷ�ֵ�ܺ�
        }
    }
    for (i = 0; i < GrayScale; i++) //����ÿ������ֵ�ĵ�������ͼ���еı���
    {
        pixelPro[i] = (float)pixelCount[i] / pixelSum;
    }
    float w0, w1, u0tmp, u1tmp, u0, u1, u, deltaTmp, deltaMax = 0;
    w0 = w1 = u0tmp = u1tmp = u0 = u1 = u = deltaTmp = 0;
    for (j = 0; j < GrayScale; j++)//�����Ҷȼ�[0,255]
    {
        w0 += pixelPro[j];  //��������ÿ���Ҷ�ֵ�����ص���ռ����֮��   ���������ֵı���
        if(pixelPro[j] == 0)
            continue;

        u0tmp += j * pixelPro[j];  //�������� ÿ���Ҷ�ֵ�ĵ�ı��� *�Ҷ�ֵ
        w1=1-w0;
        u1tmp=gray_sum/pixelSum-u0tmp;
        u0 = u0tmp / w0;              //����ƽ���Ҷ�
        u1 = u1tmp / w1;              //ǰ��ƽ���Ҷ�
        u = u0tmp + u1tmp;            //ȫ��ƽ���Ҷ�
        deltaTmp = w0 * pow((u0 - u), 2) + w1 * pow((u1 - u), 2);//ƽ��
        if (deltaTmp > deltaMax)
        {
            deltaMax = deltaTmp;//�����䷽�
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
    assert(block % 2 == 1); // block����Ϊ����
    int half_block = block / 2;
    for(int y=half_block; y<height-half_block; y++){
        for(int x=half_block; x<width-half_block; x++){
        // ����ֲ���ֵ
        int thres = 0;
        for(int dy=-half_block; dy<=half_block; dy++){
            for(int dx=-half_block; dx<=half_block; dx++){
                thres += img_data[(x+dx)+(y+dy)*width];
            }
        }
        thres = thres / (block * block) - clip_value;
        // ���ж�ֵ��
        output_data[x+y*width] = img_data[x+y*width]>thres ? 255 : 0;
        }
    }
}

/**
 * @brief ˫����ױ߽編
 *  
 * 
*/
int Longest_White_Column_Left[2];
int Longest_White_Column_Right[2];
int White_Column[IMAGE_COL];//ÿ�а��г���
int Center;
void Vision_FindBoundery(){

    int start_column = 0;
    int end_column = IMAGE_COL;
    //�����ң��������ϣ�����ȫͼ��¼��Χ�ڵ�ÿһ�а׵�����
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
    //����������������
    Longest_White_Column_Left[0] = 0;
    for(int i=start_column;i<=end_column;i++)
    {
        if (Longest_White_Column_Left[0] < White_Column[i])//�������һ��
        {
            Longest_White_Column_Left[0] = White_Column[i];//��0���ǰ��г���
            Longest_White_Column_Left[1] = i;              //��1�����±꣬��j��
        }
    }
    //���ҵ���������������
    Longest_White_Column_Right[0] = 0;
    for(int i=end_column;i>=start_column;i--)//��������ע���������ҵ���������λ�þͿ���ͣ��
    {
        if (Longest_White_Column_Right[0] < White_Column[i])//�������һ��
        {
            Longest_White_Column_Right[0] = White_Column[i];//��0���ǰ��г���
            Longest_White_Column_Right[1] = i;              //��1�����±꣬��j��
        }

    }
    Center = (Longest_White_Column_Left[1]+Longest_White_Column_Right[1]) / 2;
}


//
//@breif ���߽�ָ�������߽�ָ�Ϊ
//      LorR 1---L 0---R
//
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

//
//Ѱ�������������
//
point_t Vision_FindCornerFP(int16 *broder,int x1,int x2)
{
    int max = Tool_CmpMax(x1,x2);
    int min = Tool_CmpMin(x1,x2);


}

//
//Ѱ�һ���������
//
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

        //��⵽cosС��ʱ���������ֵ
        final_x = (cosvalue>Vector_AngleGet((point_t){low_x,broder[low_x]},
                                            (point_t){i,broder[i]},
                                            (point_t){high_x,broder[high_x]}))?
                                            i:final_x;
    }
    return (point_t){final_x,broder[final_x]};
}

//
// Ѱ�ұ߽�������
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
            if(target_seg[2].type != NULL_segment){
                //�߽���ж�
                if(fabs(broder[target_seg[2].begin] - lose_value) >= 15){
                    target_FP[1].x = target_seg[2].begin;
                    target_FP[1].y = broder[target_FP[1].x];
                    (*target_n)++;
                }
                //ͨ���Ƕ�Ѱ�ҽǵ�
                else{
                    target_FP[1] = Vision_FindArcFP(broder,target_seg[0].begin,target_seg[0].end);
                    (*target_n)++;
                }
            }
        }
        //���߽�㲻�� ��ͨ���Ƕ���Ѱ�Ҽ�ϵ�
        else{
            target_FP[0] = Vision_FindArcFP(broder,target_seg[0].begin,target_seg[0].end);
            (*target_n)++;
            //Ѱ��Զ���ļ�ϵ�
            if(target_seg[2].type != NULL_segment){
                //�߽���ж�
                if(fabs(broder[target_seg[2].begin] - lose_value) >= 15){
                    target_FP[1].x = target_seg[2].begin;
                    target_FP[1].y = broder[target_FP[1].x];
                    (*target_n)++;
                }
                //ͨ���Ƕ�Ѱ�ҽǵ�
                else{
                    target_FP[0] = Vision_FindArcFP(broder,target_seg[0].begin,target_seg[0].end);
                    (*target_n)++;
                }
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

//
//��ӡ�߽�����
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
//@brief ����״�� ��ӡ��Ϣ
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
//@brief ���·��״��
//
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

#define Cross_State_1 1
#define Cross_State_2 2
#define Leave_Cross   3
//
//@breif ʮ���������
//
void Vision_CrossHandle()
{
    static int state;
    //һ���ǵ����������
    //�����ǵ�����
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

//
//@breif �������ʽ
//
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

//
// @brief Բ������ʽ
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
//@brief ���ݵ�·Ԫ��
//
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

//
//@brief ���ߺ���������������������±�����Ӧ�߽�������ֵ����Ϊֱ��
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


