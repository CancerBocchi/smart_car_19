#include "zf_common_headfile.h"
#include "camera.h"

RoadLine_t Image_S;//����ͼ��߽�

//�ҵĻҶ�ͼ��
uint8 my_image[imgRow][imgCol];
//��ֵ�����ͼ��
uint8 my_image_BW[imgRow][imgCol];


int16 Threshold = 250;

void Vision_Handle()
{

    //ͼ��Ԥ����
    Camera_PreProcess();

    Camera_FindMidLine();   //����ɨ��
    // Camera_LongestWight();  //��Զ��Ѳ��

    Vision_SymbolJudge();   //Ԫ���жϣ����ǻ�������
    //Vision_RSHandle();      //Ԫ���жϵĽ����ʽ

   //��ȡ����
   for(int i=imgRow-1;i>=0;i--)
   {
      Image_S.MID_Table[i]=(int16)((Image_S.rightBroder[i]+Image_S.leftBroder[i])/2);
   }


//    //ͼ��debug
    float mid_offset=1.65;
    uint8_t row_begin = 20;
    tft180_show_gray_image(START_X, START_Y, (const uint8 *)my_image, imgCol, imgRow, 158, 70, 0);
    for(int i=imgRow-1;i>=row_begin;i--)
    {
        tft180_draw_point(Image_S.MID_Table[i], 78-(imgRow-1)+i, RGB565_RED);
        tft180_draw_point(Image_S.leftBroder[i], 78-(imgRow-1)+i, RGB565_BLUE);
        tft180_draw_point(Image_S.rightBroder[i], 78-(imgRow-1)+i, RGB565_BROWN);
        //����
        // tft180_draw_point((int)(160/1.65), 78-(imgRow-1)+i, RGB565_GREEN);
    }

    //Vision_DrawFP();
        //����߷�
    // tft180_draw_line(Longest_White_Column_Right[1],78-(imgRow-1),Longest_White_Column_Right[1],78-(imgRow-1)+Longest_White_Column_Right[0],RGB565_RED);
    // tft180_draw_line(Longest_White_Column_Left[1],78-(imgRow-1),Longest_White_Column_Left[1],78-(imgRow-1)+Longest_White_Column_Left[0],RGB565_RED);
    // tft180_draw_line(Center,78-(imgRow-1),Center,78-(imgRow-1)+Longest_White_Column_Left[0],RGB565_RED);

}

/**
 * @brief ͼ��Ԥ����
 * 
 */
void Camera_PreProcess()  
{

    Threshold = Camera_My_Adapt_Threshold(mt9v03x_image[0],IMAGE_COL,IMAGE_ROW);

    for(int i=0;i<imgRow;i++){
        for(int j=0;j<imgCol;j++)
        {
            my_image_BW[i][j] = mt9v03x_image[i][j]>Threshold? 255:0;
            my_image[i][j] = mt9v03x_image[i][j];
        }
    }

    
//          Pixle_Filter(imgRow-1,imgCol-1);
}

/**
 * @brief ���
 * 
 * @param image  ��άͼ��ָ��
 * @param width  ͼ����    
 * @param height ͼ��߶�
 * @return int   ���յõ�����ֵ
 */
int Camera_My_Adapt_Threshold(uint8*image,uint16 width, uint16 height)   //����㷨��ע�������ֵ��һ��Ҫ��ԭͼ��
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

/**
 * @brief ������ɨ��
 * 
 */
#define range_line 10 //ɨ�߷�Χ
void Camera_FindMidLine()
{
    int16 Target_point = 0;
    int16 Limit_Broder[3];
    Limit_Broder[0]=0;Limit_Broder[1]=0;Limit_Broder[2]=0;
    int16 Limit_Broder_subtract=0;
    int16 Limit_Broder_add=0;

    int16 i=0,j=0,w=0,p=0,z=0;

    static int8 left_add_flag[imgRow],right_add_flag[imgRow]; //���߱�־λ

    for(i=0;i<imgRow;i++)
    {
        left_add_flag[i]=0;
        right_add_flag[i]=0;
        Image_S.leftBroder[i]=0;
        Image_S.rightBroder[i]=imgCol-1;
        Image_S.MID_Table[i]=80;
    }

    for(i=imgRow-1;i>=0;i--)
    {
        if(Limit_Broder[0]==0 && Limit_Broder[1]==0 && Limit_Broder[2]==0)
        {
            if(i==imgRow-1)
            {
                if(my_image[i][MID_COL-1]>=Threshold)  //�м�Ϊ��ɫ���ص�
                {
                    //���
                    for(j=MID_COL-1;j>0;j--)
                    {
                        if(my_image[i][j]>Threshold && my_image[i][j-1]<Threshold)
                        {
                            Image_S.leftBroder[i]=(int16)j-1;
                            left_add_flag[i]=0;
                            break;
                        }
                    }
                    if(j==0)
                    {
                        Image_S.leftBroder[i]=0;
                        left_add_flag[i]=1;
                    }
                    //�Ҳ�
                    for(j=MID_COL-1;j<imgCol;j++)
                    {
                        if(my_image[i][j]<Threshold && my_image[i][j-1]>Threshold)
                        {
                            Image_S.rightBroder[i]=(int16)j;
                            right_add_flag[i]=0;
                            break;
                        }
                    }
                    if(j==imgCol)
                    {
                        Image_S.rightBroder[i]=imgCol-1;
                        right_add_flag[i]=1;
                    }
                }
                else  //��һ���м�ڵ�
                {
                    //����ɨ
                    for(j=MID_COL-1;j<imgCol;j++)
                    {
                        if(my_image[i][j]>Threshold && my_image[i][j-1]<Threshold)
                        {
                            Image_S.leftBroder[i]=(int16)j-1;
                            break;
                        }
                    }
                    // if(j==imgCol) {Image_S.leftBroder[i]=0;left_add_flag[i]=1;}
                    // else
                    if(j!=imgCol)
                    {
                        for(w=Image_S.leftBroder[i]+1;w<imgCol;w++)
                        {
                            if(my_image[i][w]<Threshold && my_image[i][w-1]>Threshold)
                            {
                                Image_S.rightBroder[i]=(int16)w;
                                break;
                            }
                        }
                        if(w==imgCol)  {Image_S.rightBroder[i]=imgCol-1;right_add_flag[i]=1;}
                    }
                    //����ɨ
//                    if(Image_S.leftBroder[i]==0)
//                    {
                        for(z=MID_COL-1;z>0;z--)
                        {
                            if(my_image[i][z-1]>Threshold && my_image[i][z]<Threshold)
                            {
                                Image_S.rightBroder[i]=(int16)z;
                                break;
                            }
                        }
                        // if(z==0)   {Image_S.rightBroder[i]=imgCol-1;right_add_flag[i]=1;}
                        // else
                        if(z!=0)
                        {
                            for(w=Image_S.rightBroder[i]-1;w>0;w--)
                            {
                                if(my_image[i][w]>Threshold && my_image[i][w-1]<Threshold)
                                {
                                    Image_S.leftBroder[i]=(int16)w;
                                    break;
                                }
                            }
                            if(w==0)  {Image_S.leftBroder[i]=1;left_add_flag[i]=1;}
                        }
//                    }
                        if(Image_S.leftBroder[i]==0) left_add_flag[i]=1;
                        if(Image_S.rightBroder[i]==0) right_add_flag[i]=1;

                }
            }
            else //����ɨ
            {
                if(left_add_flag[i+1]==0)  //left
                {
                    Target_point = Image_S.leftBroder[i+1];

                    Limit_Broder_add=Target_point + range_line;
                    Limit_Broder_subtract=Target_point - range_line ;
                    if(Limit_Broder_add>imgCol-1)  Limit_Broder_add=imgCol-1;
                    if(Limit_Broder_subtract<0) Limit_Broder_subtract=0;

                    for(j=Limit_Broder_add ; j>0; j--)
                    {
                        if(my_image[i][j]>Threshold && my_image[i][j-1]<Threshold)
                        {
                            Image_S.leftBroder[i]=(int16)j-1;
                            left_add_flag[i]=0;//���
                            break;
                        }
                        if(j<=3) break;
                    }
                    if(j==3)
                    {
                        Image_S.leftBroder[i] = 0;
                        left_add_flag[i] = 1;
                        // if(Limit_Broder_subtract==0 && Unilateral_Line_l[0] == 0)  {Unilateral_Line_l[0]=i;Unilateral_Line_l[1]=j;}
                        // if(Limit_Broder_subtract==0)
                        // else left_add_flag[i]=2;
                    }
                }
                else if(right_add_flag[i+1]==0)
                {
                    for(j=Image_S.rightBroder[i+1];j>0;j--)
                    {
                        if(my_image[i][j] > Threshold && my_image[i][j-1] < Threshold)
                        {
                            Image_S.leftBroder[i]=(int16)j-1;
                            left_add_flag[i]=0;//���
                            break;
                        }
                    }
                    if(j==0)
                    {
                        Image_S.leftBroder[i]=0;
                        left_add_flag[i]=1;
                    }
                }
                else
                {
                    for(j=MID_COL-1;j>0;j--)
                    {
                        if(my_image[i][j] > Threshold && my_image[i][j-1] < Threshold)
                        {
                            Image_S.leftBroder[i]=(int16)j-1;
                            left_add_flag[i]=0;//���
                            break;
                        }
                        if(j<=3) break;
                    }
                    if(j==3)
                    {
                        Image_S.leftBroder[i]=0;
                        left_add_flag[i]=1;
                    }
                }

                if(right_add_flag[i+1]==0)  //right
                {
                    Target_point = Image_S.rightBroder[i+1];

                    Limit_Broder_add=Target_point + range_line;
                    Limit_Broder_subtract=Target_point - range_line ;
                    if(Limit_Broder_add>=imgCol-1)  Limit_Broder_add=imgCol-1;
                    if(Limit_Broder_subtract<=1) Limit_Broder_subtract=1;

                    for(j = Limit_Broder_subtract ; j < imgCol-1 ; j++)
                    {
                        if(my_image[i][j-1]>Threshold && my_image[i][j]<Threshold)
                        {
                            Image_S.rightBroder[i]=(int16)j;
                            right_add_flag[i]=0;//�Ҳ�
                            break;
                        }
                    }
                    if(j==imgCol-1)
                    {
                        Image_S.rightBroder[i] = imgCol-1;
                        right_add_flag[i]=1;
                        // if(Limit_Broder_add==imgCol-1) {Unilateral_Line_l[0]=i;Unilateral_Line_l[1]=j;}          //Ϊ���߶ϵ㣬���Լ�����������Ϊ���߿��ܲ�����
                        // if(Limit_Broder_add==imgCol-1)
                        // else right_add_flag[i]=2;
                    }
                }
                else if(left_add_flag[i+1]==0)
                {
                    for(j=Image_S.leftBroder[i+1];j<imgCol;j++)
                    {
                        if(my_image[i][j] < Threshold && my_image[i][j-1] > Threshold)
                        {
                            Image_S.rightBroder[i]=(int16)j;
                            right_add_flag[i]=0;//�Ҳ�
                            break;
                        }
                    }
                    if(j==imgCol)
                    {
                        Image_S.rightBroder[i]=imgCol-1;
                        right_add_flag[i]=1;
                    }
                }
                else
                {
                    for(j=MID_COL-1;j<imgCol;j++)
                    {
                        if(my_image[i][j-1] > Threshold && my_image[i][j] < Threshold)
                        {
                            Image_S.rightBroder[i]=(int16)j;
                            right_add_flag[i]=0;//�Ҳ�
                            break;
                        }
                    }
                    if(j==imgCol)
                    {
                        Image_S.rightBroder[i]=imgCol-1;
                        right_add_flag[i]=1;
                    }
                }
            }

            if(Image_S.leftBroder[i]<=Image_S.rightBroder[i] && Image_S.rightBroder[i]<=10 && Limit_Broder[0]==0)      Limit_Broder[0]=i;
            else if(Image_S.rightBroder[i]>=Image_S.leftBroder[i] && Image_S.leftBroder[i]>=150 && Limit_Broder[1]==0) Limit_Broder[1]=i;
            else if((left_add_flag[i] == 1 && right_add_flag[i] == 1 && my_image[i][25] == 0 && my_image[i][135] == 0) || (Image_S.leftBroder[i] > Image_S.rightBroder[i]-6))  Limit_Broder[2]=i;
        }
        else
        {
            for(i=Limit_Broder[0];i>=0;i--)
            {
                Image_S.leftBroder[i]=0;
                Image_S.rightBroder[i]=imgCol-1;
                // left_add_flag[i]=1;
                // right_add_flag[i]=1;
            }
            for(i=Limit_Broder[1];i>=0;i--)
            {
                Image_S.leftBroder[i]=0;
                Image_S.rightBroder[i]=imgCol-1;
                // left_add_flag[i]=1;
                // right_add_flag[i]=1;
            }
            for(i=Limit_Broder[2];i>=0;i--)
            {
                Image_S.leftBroder[i]=0;
                Image_S.rightBroder[i]=imgCol-1;
                // left_add_flag[i]=1;
                // right_add_flag[i]=1;
            }
        }
    }
}

/**
 * @brief ˫����ױ߽編
 *          Ѳ�߲���1 ���������ø�Ѳ�߲���
 * 
*/
int Longest_White_Column_Left[2];
int Longest_White_Column_Right[2];
int White_Column[IMAGE_COL];//ÿ�а��г���
int Center;
void Camera_LongestWight(){

    int start_column = 0;
    int end_column = IMAGE_COL - 1;
    //�����ң��������ϣ�����ȫͼ��¼��Χ�ڵ�ÿһ�а׵�����
    for(int j =start_column; j<=end_column; j++ ){
            White_Column[j] = 0;
    }
	
    for (int j =start_column; j<=end_column; j++)
    {
        for (int i = IMAGE_ROW - 1; i >= 0; i--)
        {
            if(my_image_BW[i][j] == 0){
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

/**
 * @brief �Ͻ��������Ӧ��ֵ���㷨
 * 
 * @param img_data 
 * @param output_data 
 * @param width 
 * @param height 
 * @param block 
 * @param clip_value 
 */
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
 * @brief ����������1�7�1�7��,����ǰ�ɫ����Ϊ255�������ÁA1�1�770
 *
 * @param src raw image
 * @param x x
 * @param y y
 * @return uint8_t
 */
uint8_t VadaptiveTH(uint8_t src[VIMGH][VIMGW], uint8_t x, uint8_t y)
{
    int half_block = BLOCK / 2;
    int thres = 0;
    if (y < half_block || y >= VIMGH - half_block || x < half_block || x >= VIMGW - half_block)
        return 0;
    for (int dy = -half_block; dy <= half_block; dy++)
        for (int dx = -half_block; dx <= half_block; dx++)
            thres += src[y + dy][x + dx];
    thres = thres / (BLOCK * BLOCK) - CLIP_VAL;

    return src[y][x] > thres ? 255 : 0;
}

/* TAG there is the logic below*/

/**
 * @brief ֱ��ʱ��ɨ�߲���
 *
 */
void VscanStraight(void)
{
    for (uint8_t i = VIMGW / 2 - 20; i >= BLOCK / 2 - 1; i--)
    {
        if (!VadaptiveTH(SRCIMG, i, VIMGH - BLOCK / 2 - 1))
        {
            Veight_neighborhood(SRCIMG, F.Lp, (point_t){i, VIMGH - BLOCK / 2 - 1}, 0);
            break;
        }
    }
    for (uint8_t i = VIMGW / 2 + 20; i < VIMGW - BLOCK / 2 + 1; i++)
    {
        if (!VadaptiveTH(SRCIMG, i, VIMGH - BLOCK / 2 - 1))
        {
            Veight_neighborhood(SRCIMG, F.Rp, (point_t){i, VIMGH - BLOCK / 2 - 1}, 1);
            break;
        }
    }
}

/**
 * @brief
 *
 * @param p ������ĳ���ĵ�ַ,pֻ����ΪF.Lp��1�7�1�7F.Rp
 * @param seed seed
 * @param LorR 0��ʾ��ʱ�룬1��ʾ˳ʱ�1�1�77
 */
void Veight_neighborhood(uint8_t src[VIMGH][VIMGW], point_t *p, point_t seed, uint8_t LorR)
{
    point_t **pp;
    if (p == F.Lp)
        pp = &F.Lpp;
    else if (p == F.Rp)
        pp = &F.Rpp;
    else
        while (1)
            ;

    uint8_t dirindex = 0;
    uint8_t arrayindex;
    uint8_t breakflag = 0;
    point_t *orip = p;
    point_t *stackbottom = NULL;
    uint8_t stacksize = 0;
    point_t newp;

    while (!breakflag)
    {
        uint8_t i = 0;
        for (i = 0; i < 8; i++)
        {
            arrayindex = dirindex + i;
            if (arrayindex > 7)
                arrayindex -= 8;
            newp = (point_t){seed.x + Pointdirection[LorR][arrayindex].x, seed.y + Pointdirection[LorR][arrayindex].y};
            if (!VadaptiveTH(src, newp.x, newp.y)) //�ҵ����¶��1�7����
            {
                if (!checkPexists(orip, newp))
                {
                    if (arrayindex >= 2)
                        dirindex = arrayindex - 2;
                    else
                        dirindex = arrayindex + 6;

                    if (LorR == SCANFORLEFT && newp.x > VIMGW - 10)
                    {
                        breakflag = 1;
                        break;
                    }
                    if (LorR == SCANFORRIGHT && newp.x < 10)
                    {
                        breakflag = 1;
                        break;
                    }
                    if ((arrayindex == 7 || arrayindex == 6) && stackbottom == NULL) //ջ�׳�ʼ��1�1�77
                        stackbottom = *pp;
                    if (stackbottom != NULL)
                        stacksize++;
                    if (stacksize == 16) //��ջ�����|�1�7ʼ����ջ�����1�1�77
                    {
                        int dx = 0, dy = 0;
                        for (point_t *sp = *pp - 1; sp >= stackbottom; sp--)
                        {
                            dx += (sp + 1)->x - sp->x;
                            dy += (sp + 1)->y - sp->y;
                        }
                        if (LorR == 0 && dx >= 0 && dy > dx / 2) //��߮q1�1�77
                        {
                            *pp = stackbottom;
                            breakflag = 1;
                            break;
                        }
                        else if (LorR == 1 && dx <= 0 && dy >= -dx / 2) //�ұ߮q1�1�77
                        {
                            *pp = stackbottom;
                            breakflag = 1;
                            break;
                        }
                        else //���������1�7�1�7�1�7��������ջ��ʼ��1�1�77
                        {
                            stacksize = 0;
                            stackbottom = NULL;
                        }
                    }

                    **pp = newp;
                    seed = newp;

                    (*pp)++;
                    if (*pp - p >= VIMGH * 5)
                    {
                        breakflag = 1;
                        break;
                    }
                    break;
                }
                else
                {
                    breakflag = 1;
                    // break;
                }
            }
        }
        if (i == 8)
            breakflag = 1;
    }
}

void Camera_and_Screen_Init(){
	//��ʼ����Ļ
	tft180_set_dir(TFT180_CROSSWISE);                                           // ��Ҫ�Ⱥ��� ��Ȼ��ʾ����
	tft180_init();
	tft180_show_string(0, 0, "mt9v03x init.");
	
	//��ʼ������ͷ
	while(1)
	{
			if(mt9v03x_init())
			{
					tft180_show_string(0, 16, "mt9v03x reinit.");
					rt_kprintf("mt9v03x failed try to reinit\n");
			}
			else
			{
					rt_kprintf("mt9v03x init successfully\n");
					break;
			}
			system_delay_ms(1000);                                                  // ���Ʊ�ʾ�쳣
	}
	tft180_show_string(0, 16, "init success.");
	tft180_clear();
}