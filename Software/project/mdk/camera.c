#include "zf_common_headfile.h"
#include "camera.h"
#include <stdbool.h>

RoadLine_t Image_S;//保存图像边界

//我的灰度图像
uint8 my_image[imgRow][imgCol];
//二值化后的图像
uint8 my_image_BW[imgRow][imgCol];

int cir_line[IMAGE_COL];

int16 Threshold = 250;

void Vision_Handle()
{
	static int handle = 0;
    //图像预处理
    Camera_PreProcess();

    Camera_FindMidLine();   //常规扫线
    // Camera_LongestWight();  //最远线巡线    
    Vision_SymbolJudge();   //元素判断，但是会有问题	
    Vision_RSHandle();      //元素判断的解决方式

    Vision_Draw();
   //获取中线
   for(int i=imgRow-1;i>=0;i--)
   {
      Image_S.MID_Table[i]=(int16)((Image_S.rightBroder[i]+Image_S.leftBroder[i])/2);
   } 
   int16 MID_Table[imgRow];
   int16 leftBroder[imgRow];//左边边界
   int16 rightBroder[imgRow];//右边边界
   for(int i=imgRow-1;i>=0;i--){
       MID_Table[i] = Image_S.MID_Table[i];
       leftBroder[i] = Image_S.leftBroder[i];
       rightBroder[i] = Image_S.rightBroder[i];
   }

   for(int i=imgRow-1;i>0;i--)
   {
       if(MID_Table[i]>=188)
           MID_Table[i] = 187;
        MID_Table[i] = MID_Table[i]<0? 0:MID_Table[i];

       if(leftBroder[i]>=188)
           leftBroder[i] = 187;
        leftBroder[i] = leftBroder[i]<0? 0:leftBroder[i];

       if(rightBroder[i]>=188)
           rightBroder[i] = 187;
        rightBroder[i] = rightBroder[i]<0? 0:rightBroder[i]; 
				
       ips200_draw_point(MID_Table[i], i, RGB565_RED);
       ips200_draw_point(leftBroder[i], i, RGB565_BLUE);
       ips200_draw_point(rightBroder[i], i, RGB565_BROWN);
       //中线
       ips200_draw_point((int)(94), i, RGB565_GREEN);
   }

    
    
    Vision_DrawFP();
        //最长白线法
    // ips200_draw_line(Longest_White_Column_Right[1],78-(imgRow-1),Longest_White_Column_Right[1],78-(imgRow-1)+Longest_White_Column_Right[0],RGB565_RED);
    // ips200_draw_line(Longest_White_Column_Left[1],78-(imgRow-1),Longest_White_Column_Left[1],78-(imgRow-1)+Longest_White_Column_Left[0],RGB565_RED);
    // ips200_draw_line(Center,78-(imgRow-1),Center,78-(imgRow-1)+Longest_White_Column_Left[0],RGB565_RED);

}

void Vision_Draw(){
    ips200_show_gray_image(0, 0, (const uint8 *)my_image, imgCol, imgRow, 188, 70, 0);

}

/**
 * @brief 图像预处理
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
 * @brief 大津法
 * 
 * @param image  二维图像指针
 * @param width  图像宽度    
 * @param height 图像高度
 * @return int   最终得到的阈值
 */
int Camera_My_Adapt_Threshold(uint8*image,uint16 width, uint16 height)   //大津算法，注意计算阈值的一定要是原图像
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

/**
 * @brief 八领域扫线
 * 
 */
#define range_line 10 //扫线范围
void Camera_FindMidLine()
{
    int16 Target_point = 0;
    int16 Limit_Broder[3];
    Limit_Broder[0]=0;Limit_Broder[1]=0;Limit_Broder[2]=0;
    int16 Limit_Broder_subtract=0;
    int16 Limit_Broder_add=0;

    int16 i=0,j=0,w=0,p=0,z=0;

    static int8 left_add_flag[imgRow],right_add_flag[imgRow]; //补线标志位

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
                if(my_image[i][MID_COL-1]>=Threshold)  //中间为白色像素点
                {
                    //左侧
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
                    //右侧
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
                else  //第一行中间黑点
                {
                    //向右扫
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
                    //向左扫
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
            else //往上扫
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
                            left_add_flag[i]=0;//左侧
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
                            left_add_flag[i]=0;//左侧
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
                            left_add_flag[i]=0;//左侧
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
                            right_add_flag[i]=0;//右侧
                            break;
                        }
                    }
                    if(j==imgCol-1)
                    {
                        Image_S.rightBroder[i] = imgCol-1;
                        right_add_flag[i]=1;
                        // if(Limit_Broder_add==imgCol-1) {Unilateral_Line_l[0]=i;Unilateral_Line_l[1]=j;}          //为边线断点，可以加新条件，记为补线可能不合适
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
                            right_add_flag[i]=0;//右侧
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
                            right_add_flag[i]=0;//右侧
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
 * @brief 双边最长白边界法
 *          巡线策略1 弯道以外均用该巡线策略
 * 
*/
int Longest_White_Column_Left[2];
int Longest_White_Column_Right[2];
int White_Column[imgCol];//每列白列长度
int Center;
void Camera_LongestWight(){

    int start_column = 0;
    int end_column = imgCol - 1;
    //从左到右，从下往上，遍历全图记录范围内的每一列白点数量
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

/**
 * @brief 上交大的自适应二值化算法
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

void Camera_and_Screen_Init(){
	//初始化屏幕
	ips200_set_dir(IPS200_CROSSWISE);          
                                     // 需要先横屏 不然显示不下
	ips200_init(IPS200_TYPE_SPI);
	ips200_show_string(0, 0, "mt9v03x init.");
	
	//初始化摄像头
	while(1)
	{
			if(mt9v03x_init())
			{
					ips200_show_string(0, 16, "mt9v03x reinit.");
					rt_kprintf("mt9v03x failed try to reinit\n");
			}
			else
			{
					rt_kprintf("mt9v03x init successfully\n");
					break;
			}
			rt_thread_delay(1000);                                                  // 闪灯表示异常
	}
	ips200_show_string(0, 16, "init success.");
	ips200_clear();
}

/**
 * @brief 圆环寻找边界函数
 * 
 * @param image 
 * @param line 
 */
#define BLOCK 7
#define CLIP_VAL 10
//单点二值化
uint8_t VadaptiveTH(uint8_t src[imgRow][imgCol], uint8_t x, uint8_t y)
{
    int half_block = BLOCK / 2;
        int thres = 0;
    if (y < half_block || y >= imgRow - half_block || x < half_block || x >= imgCol - half_block)
        return 1;
    for (int dy = -half_block; dy <= half_block; dy++)
        for (int dx = -half_block; dx <= half_block; dx++)
            thres += src[y + dy][x + dx];
    thres = thres / (BLOCK * BLOCK) - CLIP_VAL;

    return src[y][x] > thres ? 255 : 0;
}

/**
 * @brief 圆环巡线
 * 
 */
#define Judge_Thre 50
void Camera_CirFindLine(uint8_t src[imgRow][imgCol]) {
    for (int i = imgCol - 1; i >= 0; i--) {
        for (int j = imgRow - 1; j >= 0; j--) {
            if (abs(src[j + 1][i] - src[j][i]) > Judge_Thre) {
                if (!VadaptiveTH(src, i, j)) {
                    cir_line[i] = j;
                    break;
                }
            } else
                cir_line[i] = 0;
        }
    }
}