#include "zf_common_headfile.h"
#include "camera.h"
#include <stdbool.h>

RoadLine_t Image_S;//����ͼ��߽�

//�ҵĻҶ�ͼ��
uint8 my_image[imgRow][imgCol];
//��ֵ�����ͼ��
uint8 my_image_BW[imgRow][imgCol];

int cir_line[IMAGE_COL];

int16 Threshold = 250;

void Vision_Handle()
{
	static int handle = 0;
    //ͼ��Ԥ����
    Camera_PreProcess();

    // Camera_FindMidLine();   //����ɨ��
    // Camera_LongestWight();  //��Զ��Ѳ��    
    // Vision_SymbolJudge();   //Ԫ���жϣ����ǻ�������	
    // Vision_RSHandle();      //Ԫ���жϵĽ����ʽ

   //��ȡ����
//    for(int i=imgRow-1;i>=0;i--)
//    {
//       Image_S.MID_Table[i]=(int16)((Image_S.rightBroder[i]+Image_S.leftBroder[i])/2);
//    }

//    //ͼ��debug
    // float mid_offset=1.65;
    // uint8_t row_begin = 20;
    //ips200_show_gray_image(START_X, START_Y, (const uint8 *)my_image, imgCol, imgRow, 158, 70, 0);
    ips200_show_gray_image(0, 0, (const uint8 *)my_image, imgCol, imgRow, 188, 70, 0);
    //ips200_show_gray_image(START_X, START_Y, (const uint8 *)mt9v03x_image, MT9V03X_W, MT9V03X_H, 158, 70, 0);
//	 
//    int16 MID_Table[imgRow];
//    int16 leftBroder[imgRow];//��߽߱�
//    int16 rightBroder[imgRow];//�ұ߽߱�
//    for(int i=imgRow-1;i>=0;i--){
//        MID_Table[i] = Image_S.MID_Table[i];
//        leftBroder[i] = Image_S.leftBroder[i];
//        rightBroder[i] = Image_S.rightBroder[i];
//    }

//    for(int i=imgRow-1;i>0;i--)
//    {
//        if(MID_Table[i]>=188)
//            MID_Table[i] = 187;
//        if(leftBroder[i]>=188)
//            leftBroder[i] = 187;
//        if(rightBroder[i]>=188)
//            rightBroder[i] = 187;
//				
//        ips200_draw_point(MID_Table[i], i, RGB565_RED);
//        ips200_draw_point(leftBroder[i], i, RGB565_BLUE);
//        ips200_draw_point(rightBroder[i], i, RGB565_BROWN);
//        //����
//        ips200_draw_point((int)(94), i, RGB565_GREEN);
//    }

    
    
    Vision_DrawFP();
        //����߷�
    // ips200_draw_line(Longest_White_Column_Right[1],78-(imgRow-1),Longest_White_Column_Right[1],78-(imgRow-1)+Longest_White_Column_Right[0],RGB565_RED);
    // ips200_draw_line(Longest_White_Column_Left[1],78-(imgRow-1),Longest_White_Column_Left[1],78-(imgRow-1)+Longest_White_Column_Left[0],RGB565_RED);
    // ips200_draw_line(Center,78-(imgRow-1),Center,78-(imgRow-1)+Longest_White_Column_Left[0],RGB565_RED);

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
int White_Column[imgCol];//ÿ�а��г���
int Center;
void Camera_LongestWight(){

    int start_column = 0;
    int end_column = imgCol - 1;
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

void Camera_and_Screen_Init(){
	//��ʼ����Ļ
	ips200_set_dir(IPS200_CROSSWISE);          
                                     // ��Ҫ�Ⱥ��� ��Ȼ��ʾ����
	ips200_init(IPS200_TYPE_SPI);
	ips200_show_string(0, 0, "mt9v03x init.");
	
	//��ʼ������ͷ
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
			rt_thread_delay(1000);                                                  // ���Ʊ�ʾ�쳣
	}
	ips200_show_string(0, 16, "init success.");
	ips200_clear();
}

/**
 * @brief Բ��Ѱ�ұ߽纯��
 * 
 * @param image 
 * @param line 
 */
#define WIDTH IMAGE_COL  // 
#define HEIGHT IMAGE_ROW // 
#define INTENSITY_THRESHOLD 200 // 
#define GRADIENT_THRESHOLD 50   // 

void compute_gradient(uint8_t image[HEIGHT][WIDTH], int16_t gradient[HEIGHT][WIDTH]);
bool is_boundary(uint8_t image[HEIGHT][WIDTH], int16_t gradient[HEIGHT][WIDTH], int x, int y);
int find_initial_boundary(uint8_t image[HEIGHT][WIDTH], int16_t gradient[HEIGHT][WIDTH], int center_x);
void crawl_boundary(uint8_t image[HEIGHT][WIDTH], int16_t gradient[HEIGHT][WIDTH], int *col_line, int start_x, int start_y, int direction);

void Camera_CirculeFindLine(uint8_t image[IMAGE_ROW][IMAGE_COL], int *col_line) {
    int16_t gradient[HEIGHT][WIDTH];

    // 
    compute_gradient(image, gradient);

    int center_x = WIDTH / 2;
    int initial_y = find_initial_boundary(image, gradient, center_x);

    if (initial_y != -1) {
        //
        for (int i = 0; i < WIDTH; i++) {
            col_line[i] = -1; // -1 
        }
        col_line[center_x] = initial_y;

        // 
        crawl_boundary(image, gradient, col_line, center_x, initial_y, -1); // 向左
        crawl_boundary(image, gradient, col_line, center_x, initial_y, 1);  // 向右
    }
}


void compute_gradient(uint8_t image[HEIGHT][WIDTH], int16_t gradient[HEIGHT][WIDTH]) {
    for (int y = 1; y < HEIGHT - 1; y++) {
        for (int x = 1; x < WIDTH - 1; x++) {
            int gx = image[y][x + 1] - image[y][x - 1];
            int gy = image[y + 1][x] - image[y - 1][x];
            gradient[y][x] = abs(gx) + abs(gy); // 
        }
    }
}

bool is_boundary(uint8_t image[HEIGHT][WIDTH], int16_t gradient[HEIGHT][WIDTH], int x, int y) {
    if (image[y][x] >= INTENSITY_THRESHOLD) {
        for (int dx = -1; dx <= 1; dx++) {
            for (int dy = -1; dy <= 1; dy++) {
                if (dx == 0 && dy == 0) continue;
                int nx = x + dx;
                int ny = y + dy;
                if (nx >= 0 && nx < WIDTH && ny >= 0 && ny < HEIGHT) {
                    if (image[ny][nx] < INTENSITY_THRESHOLD || gradient[ny][nx] > GRADIENT_THRESHOLD) {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

int find_initial_boundary(uint8_t image[HEIGHT][WIDTH], int16_t gradient[HEIGHT][WIDTH], int center_x) {
    for (int y = HEIGHT - 1; y >= 0; y--) {
        for (int x = center_x - 10; x <= center_x + 10; x++) {
            if (is_boundary(image, gradient, x, y)) {
                return y;
            }
        }
    }
    return -1;
}

void crawl_boundary(uint8_t image[HEIGHT][WIDTH], int16_t gradient[HEIGHT][WIDTH], int *col_line, int start_x, int start_y, int direction) {
    int x = start_x;
    int y = start_y;
    while (true) {
        bool found = false;
        for (int dx = -1; dx <= 1; dx++) {
            for (int dy = -1; dy <= 1; dy++) {
                if (dx == 0 && dy == 0) continue;
                int nx = x + dx * direction;
                int ny = y + dy;
                if (nx >= 0 && nx < WIDTH && ny >= 0 && ny < HEIGHT) {
                    if (is_boundary(image, gradient, nx, ny)) {
                        if (ny > col_line[nx]) {
                            col_line[nx] = ny;
                        }
                        x = nx;
                        y = ny;
                        found = true;
                        break;
                    }
                }
            }
            if (found) break;
        }
        if (!found) break;
    }
}

