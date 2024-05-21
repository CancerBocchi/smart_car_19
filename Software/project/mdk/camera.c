#include "zf_common_headfile.h"
#include "camera.h"

//        tft180_show_float(00,80,threshold,3,1);//横向纵向

/*========================================*/

//int8 Camera_element=-1;
 int8 Camera_element=0;

int8 Threshold_up=65;
int8 Threshold_down=52;

int16 i=0,j=0,w=0,p=0,z=0;
ImageStruct Image_S;

feature_t 	 Feature_Judge;
feature_flag road_Feature;


uint8 my_image[imgRow][imgCol];
int16 Threshold = 250;
int8 left_add_flag[imgRow],right_add_flag[imgRow]; //补线标志位


/*========================================*/

void UseImage()
{
    ReadMyImage();
    FindMidLine(); 

		Vision_SymbolJudge();
		Vision_RSHandle();

    for(i=imgRow-1;i>=0;i--)
    {
        Image_S.MID_Table[i]=(int16)((Image_S.rightBroder[i]+Image_S.leftBroder[i])/2);
    }
}

/*****************************************/


void ReadMyImage()  //图像处理
{
//    Threshold = 160; // OTSU(mt9v03x_image[0],0,0,imgCol,imgRow);

//    if(Threshold<Threshold_down) Threshold=Threshold_down;
//    if(Threshold>Threshold_up) Threshold=Threshold_up;
//     tft180_show_float(00,100,Threshold,3,1);//横向纵向Judge_Round
//         tft180_show_float(50,100,Opening_flag,3,1);//横向纵向

    for(i=0;i<imgRow;i++)
            for(j=0;j<imgCol;j++)
            {
                if(mt9v03x_image[i][j]>Threshold)   my_image[i][j]=255;  //白
                else                                 my_image[i][j]=0;   //黑
            }
//          Pixle_Filter(imgRow-1,imgCol-1);
}

uint16 OTSU(uint8 *image, uint16 col_start, uint16 row_start, uint16 col_end, uint16 row_end)               //大津法
{
        #define GrayScale 256
        uint16 width = col_end;
        uint16 height = row_end;
        int pixelCount[GrayScale];
        float pixelPro[GrayScale];
        uint16 i, j, pixelSum = width * height/4;
        uint16 threshold = 0;
        uint8* data = image;
        uint32 gray_sum = 0;
        for (i = 0; i < GrayScale; i++)
        {
                pixelCount[i] = 0;pixelPro[i] = 0;
        }
        for (i = row_start; i < height; i+=2)
            for (j = col_start; j < width; j+=2)
                {
                    pixelCount[(int)data[i * width + j]]++;
                    gray_sum+=(int)data[i * width + j];
                }
        for (i = 0; i < GrayScale; i++)
        {
            pixelPro[i] = (float)pixelCount[i] / pixelSum;
        }
        float w0, w1, u0tmp, u1tmp, u0, u1, u, deltaTmp, deltaMax = 0;

                w0 = w1 = u0tmp = u1tmp = u0 = u1 = u = deltaTmp = 0;
                for (j = 0; j < GrayScale; j++)
                {
                    w0 += pixelPro[j];u0tmp += j * pixelPro[j];w1=1-w0;u1tmp=gray_sum/pixelSum-u0tmp;u0 = u0tmp / w0;
                    u1 = u1tmp / w1;u = u0tmp + u1tmp;deltaTmp = w0 * (u0 - u) * (u0 - u) + w1 * (u1 - u) * (u1 - u);
                    if (deltaTmp > deltaMax){deltaMax = deltaTmp;threshold = j;}
                    if (deltaTmp < deltaMax){break;}
                 }
        return threshold;
}
#define range_line 10 //扫线范围
void FindMidLine()
{
    int16 Target_point = 0;
    int16 Limit_Broder[3];
    Limit_Broder[0]=0;Limit_Broder[1]=0;Limit_Broder[2]=0;
    int16 Limit_Broder_subtract=0;
    int16 Limit_Broder_add=0;

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




//
//@brief 获取特征点
//
