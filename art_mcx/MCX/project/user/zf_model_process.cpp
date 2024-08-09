/*********************************************************************************************************************
* MCX Vision Opensourec Library ����MCX Vision ��Դ�⣩��һ�����ڹٷ� SDK �ӿڵĵ�������Դ��
* Copyright (c) 2024 SEEKFREE ��ɿƼ�
* 
* ���ļ��� MCX Vision ��Դ���һ����
* 
* MCX Vision ��Դ�� ��������
* �����Ը��������������ᷢ���� GPL��GNU General Public License���� GNUͨ�ù������֤��������
* �� GPL �ĵ�3�棨�� GPL3.0������ѡ��ģ��κκ����İ汾�����·�����/���޸���
* 
* ����Դ��ķ�����ϣ�����ܷ������ã�����δ�������κεı�֤
* ����û�������������Ի��ʺ��ض���;�ı�֤
* ����ϸ����μ� GPL
* 
* ��Ӧ�����յ�����Դ���ͬʱ�յ�һ�� GPL �ĸ���
* ���û�У������<https://www.gnu.org/licenses/>
* 
* ����ע����
* ����Դ��ʹ�� GPL3.0 ��Դ���֤Э�� �����������Ϊ���İ汾
* �������Ӣ�İ��� libraries/doc �ļ����µ� GPL3_permission_statement.txt �ļ���
* ��ӭ��λʹ�ò����������� ���޸�����ʱ���뱣����ɿƼ��İ�Ȩ����������������
* 
* �ļ�����          zf_model_process
* ��˾����          �ɶ���ɿƼ����޹�˾
* �汾��Ϣ          �鿴 libraries/doc �ļ����� version �ļ� �汾˵��
* ��������          MDK 5.38a
* ����ƽ̨          MCX Vision
* ��������          https://seekfree.taobao.com/
* 
* �޸ļ�¼
* ����              ����                ��ע
* 2024-04-21        ZSY            first version
********************************************************************************************************************/
#include "zf_model_process.h"

#include "model.h"
#include "yolo_post_processing.h"

#define MODEL_IN_W	                (160 )
#define MODEL_IN_H                  (128 )
#define MODEL_IN_C	                (3   )
#define MAX_OD_BOX_CNT              (12  )
#define SWAPBYTE(h)                 ((((uint16_t)h << 8)&0xFF00) | ((uint16_t)h >> 8))

typedef struct tagodresult_t
{
    union
    {
        int16_t xyxy[4];
        struct
        {
            int16_t x1;
            int16_t y1;
            int16_t x2;
            int16_t y2;
        };
    };
    float score;
    int label;
} odresult_t;


int start_flag = 0;
extern int start_flag;
int locate_flag = 1;			// 0
extern int locate_flag;
int goto_flag = 0;
extern int goto_flag;
int mode = 2;				// 1
extern int mode;


tensor_dims_t inputDims;
tensor_type_t inputType;
uint8_t* inputData;
tensor_dims_t outputDims;
tensor_type_t outputType;
uint8_t* outputData;
size_t arenaSize;
uint32_t out_size;
yolo::object_detection::PostProcessParams postProcessParams;
TfLiteTensor* outputTensor[3];
float* anchors;
std::vector<yolo::object_detection::DetectionResult> results;

int s_odretcnt = 0;
__attribute__((section(".model_input_buffer"))) volatile uint8_t __attribute((aligned(8))) model_input_buf[MODEL_IN_W * MODEL_IN_H * MODEL_IN_C] = {0};
odresult_t s_odrets[MAX_OD_BOX_CNT];

void draw_rect_on_slice_buffer(uint16_t* pcam, int srcw, int cury, int stride, odresult_t* podret, int retcnt, int slice_height)
{
    int i = 0;
    for(i = 0; i < retcnt; i++, podret++)
    {
        uint32_t color = 0xffffffff;
        uint16_t* phorline = 0;
        int stripey1 = podret->y1 - cury;
        if(stripey1 >= 0 && stripey1 < slice_height)
        {
            for(int j = 0; j < 2 && stripey1 + j < slice_height; j++)
            {
                phorline = pcam + srcw * (j + stripey1) + podret->x1;
                memset(phorline, color, (podret->x2 - podret->x1) * 2);
            }
        }

        int stripey2 = podret->y2 - cury;
        if(stripey2 >= 0 && stripey2 < slice_height)
        {
            for(int j = 0; j < 2 && stripey2 + j < slice_height; j++)
            {
                phorline = pcam + srcw * (j + stripey2) + podret->x1;
                memset(phorline, color, (podret->x2 - podret->x1) * 2);
            }
        }

        uint16_t* pvtclinel = pcam + podret->x1;
        uint16_t* pvtcliner = pcam + podret->x2 - 2;

        for(int y = cury; y < cury + slice_height; y++)
        {
            if(y > podret->y1 && y < podret->y2)
            {
                memset(pvtclinel, color, 4);
                memset(pvtcliner, color, 4);
            }
            pvtclinel += srcw;
            pvtcliner += srcw;
        }

    }
}

void rgb565stridedtorgb888(const uint16_t* pin, int srcw, int wndw, int wndh, int wndx0, int wndy0,
                           uint8_t* p888, int stride, uint8_t issub128)
{
    const uint16_t* psrc = pin;
    uint32_t datin, datout, datouts[3];
    uint8_t* p888out = p888;

    for(int y = wndy0, y1 = (wndh - wndy0) / stride - wndy0; y < wndh; y += stride, y1--)
    {
        psrc = pin + srcw * y + wndx0;
        for(int x = 0; x < wndw; x += stride * 4)
        {
            datin = SWAPBYTE(psrc[0]);
            psrc += stride;
            datouts[0] = (datin & 31) << 19 | (datin & 63 << 5) << 5 | ((datin >> 8) & 0xf8);

            datin = SWAPBYTE(psrc[0]);
            psrc += stride;
            datout = (datin & 31) << 19 | (datin & 63 << 5) << 5 | ((datin >> 8) & 0xf8);
            datouts[0] |= datout << 24;
            datouts[1] = datout >> 8;

            datin = SWAPBYTE(psrc[0]);
            psrc += stride;
            datout = (datin & 31) << 19 | (datin & 63 << 5) << 5 | ((datin >> 8) & 0xf8);
            datouts[1] |= (datout << 16) & 0xffff0000;
            datouts[2] = datout >> 16;

            datin = SWAPBYTE(psrc[0]);
            psrc += stride;
            datout = (datin & 31) << 19 | (datin & 63 << 5) << 5 | ((datin >> 8) & 0xf8);

            datouts[2] |= datout << 8;

            if(issub128)
            {
                datouts[0] ^= 0x80808080;
                datouts[1] ^= 0x80808080;
                datouts[2] ^= 0x80808080;
            }
            memcpy(p888out, datouts, 3 * 4);
            p888out += 3 * 4;
        }
    }
}

void ezh_copy_slice_to_model_input(uint32_t idx, uint32_t cam_slice_buffer, uint32_t cam_slice_width, uint32_t cam_slice_height, uint32_t max_idx)
{
    static uint8_t* pCurDat;
    uint32_t curY;
    uint32_t s_imgStride = cam_slice_width / MODEL_IN_W;

    if(idx > max_idx)
    {
        return;
    }
    uint32_t ndx = idx;
    curY = ndx * cam_slice_height;
    int wndY = (s_imgStride - (curY - 0) % s_imgStride) % s_imgStride;

    pCurDat = (uint8*)model_input_buf + 3 * ((cam_slice_height * ndx + wndY) * cam_slice_width / s_imgStride / s_imgStride);

    if(curY + cam_slice_height >= 0)
    {
        rgb565stridedtorgb888((uint16_t*)cam_slice_buffer, cam_slice_width, cam_slice_width, cam_slice_height, 0, wndY, pCurDat, s_imgStride, 1);
    }
}
typedef struct
{
    uint16 res_x1;
    uint16 res_y1;
    uint16 res_x2;
    uint16 res_y2;
}od_result_t;
volatile od_result_t od_result;

typedef struct {
    double x;   // ����
    double P;   // �������Э����
    double Q;   // ��������Э����
    double R;   // ��������Э����
    double K;   // ����������
} KalmanFilter;

// ��ʼ���������˲���
void kalman_init(KalmanFilter* kf, double Q, double R) {
    kf->x = 0.0;
    kf->P = 1.0;
    kf->Q = Q;
    kf->R = R;
    kf->K = 0.0;
}

// �������˲�������
double kalman_update(KalmanFilter* kf, double measurement) {
    // Ԥ��
    kf->P += kf->Q;

    // ���㿨��������
    kf->K = kf->P / (kf->P + kf->R);

    // ���¹���ֵ
    kf->x += kf->K * (measurement - kf->x);

    // �������Э����
    kf->P *= (1 - kf->K);

    return kf->x;
}

KalmanFilter kf_x, kf_y;
//-------------------------------------------------------------------------------------------------------------------
// �������     ģ�͵�Ŀ����Ϣ���
// ����˵��     odres           Ŀ��ṹ��
// ����˵��     retcnt          Ŀ������
// ���ز���     void
// ʹ��ʾ��     zf_model_odresult_out(s_odrets, s_odretcnt);
// ��ע��Ϣ     
//-------------------------------------------------------------------------------------------------------------------
void zf_model_odresult_out(const odresult_t* odres, int retcnt)
{
    for(int i = 0; i < retcnt; i++, odres++)
    {
        zf_debug_printf("sc:%d,x1:%d,y1:%d,x2:%d,y2:%d\r\n",
                        (int)(odres->score*100), odres->x1, odres->y1, odres->x2, odres->y2);
        
        // ͨ���û�����-����5�������ݣ�ͨ��������Ƭ������
        od_result.res_x1 = odres->x1;
        od_result.res_y1 = odres->y1;
        od_result.res_x2 = odres->x2;
        od_result.res_y2 = odres->y2;
        user_uart_putchar(0xAA);
        user_uart_putchar((uint8)i);
        user_uart_write_buffer((const uint8*)(&od_result), sizeof(od_result));
        user_uart_putchar(0xFF);
    }
}

//-------------------------------------------------------------------------------------------------------------------
// �������     ģ�ͳ�ʼ��
// ���ز���     void
// ʹ��ʾ��     zf_model_init();
// ��ע��Ϣ     
//-------------------------------------------------------------------------------------------------------------------
void zf_model_init(void)
{
    // ��ʼ���������˲�����Q��R��Ҫ����ʵ���������
    kalman_init(&kf_x, 0.015, 1.0);
    kalman_init(&kf_y, 0.015, 1.0);
	
	
    if(MODEL_Init() != kStatus_Success)
    {
        zf_debug_printf("Failed initializing model");
        while(1);
    }
    size_t usedSize = MODEL_GetArenaUsedBytes(&arenaSize);
    zf_debug_printf("\r\n%d/%d kB (%0.2f%%) tensor arena used\r\n", usedSize / 1024, arenaSize / 1024, 100.0 * usedSize / arenaSize);

    inputData = MODEL_GetInputTensorData(&inputDims, &inputType);
    outputData = MODEL_GetOutputTensorData(&outputDims, &outputType);
    out_size = MODEL_GetOutputSize();

    postProcessParams.inputImgRows = (int)inputDims.data[1];
    postProcessParams.inputImgCols =	(int)inputDims.data[2];
    postProcessParams.output_size = (int)out_size;
    postProcessParams.originalImageWidth = SCC8660_W;
    postProcessParams.originalImageHeight = SCC8660_H;
    postProcessParams.threshold = 0.45;
    postProcessParams.nms = 0.2;
    postProcessParams.numClasses = 1;
    postProcessParams.topN = 0;
    
    anchors = MODEL_GetAnchors();
    for(int i = 0; i < out_size; i ++)
    {
        outputTensor[i] = MODEL_GetOutputTensor(i);
        postProcessParams.anchors[i][0] = *(anchors + 6 * (out_size - 1 - i));
        postProcessParams.anchors[i][1] = *(anchors + 6 * (out_size - 1 - i) + 1);
        postProcessParams.anchors[i][2] = *(anchors + 6 * (out_size - 1 - i) + 2);
        postProcessParams.anchors[i][3] = *(anchors + 6 * (out_size - 1 - i) + 3);
        postProcessParams.anchors[i][4] = *(anchors + 6 * (out_size - 1 - i) + 4);
        postProcessParams.anchors[i][5] = *(anchors + 6 * (out_size - 1 - i) + 5);
    }
}


//-------------------------------------------------------------------------------------------------------------------
// �������     ģ������
// ���ز���     void
// ʹ��ʾ��     zf_model_run();
// ��ע��Ϣ     
//-------------------------------------------------------------------------------------------------------------------
uint8 buffer[6] = {0};
uint8_t count_cards;
float dist2car;
typedef struct card{
	uint8_t x;
	uint8_t y;
	float score;
}detected_card;
detected_card d_cards[4];


#define ALPHA 0.1
void low_pass_filter(double x, double y, double *x_filtered, double *y_filtered) {
    static double x_prev = 0.0;
    static double y_prev = 0.0;
    static int first_run = 1;

    // ��ʼ����£��˲����ֵ��Ϊ����ֵ
    if (first_run) {
        *x_filtered = x;
        *y_filtered = y;
        x_prev = x;
        y_prev = y;
        first_run = 0;
    } else {
        // ��ͨ�˲���ʽ
        *x_filtered = ALPHA * x + (1 - ALPHA) * x_prev;
        *y_filtered = ALPHA * y + (1 - ALPHA) * y_prev;

        // ����ǰһ�ε�ֵ
        x_prev = *x_filtered;
        y_prev = *y_filtered;
    }
}

void zf_model_run(void)
{
//		uint8 buffer[6] = {0};
		// ͨ�Ÿ�ʽ��"@{����}{x}{y}{����}$"
		buffer[0] = (uint8)0X0A;
		buffer[5] = (uint8)0X0D;	
		static uint16_t score;
		uint16_t near_x, near_y;
		count_cards = 0;
		uint8_t dist_to_midline = 160;
		uint8_t dist_to_car = 255;
//		uint8_t car_dist_thresh = 120;
		uint8_t midline_thresh = 50;
		float ratio_wh_thresh_max = 10;
		float ratio_wh_thresh_min = 0.1;
		float ratio_wh = 1;
		float scale = 1.5;
		// ********************************************************************************
    uint8_t* buf = 0;
    memset(inputData, 0, inputDims.data[1]*inputDims.data[2]*inputDims.data[3]);
    buf = inputData + (inputDims.data[1] - MODEL_IN_H) / 2 * MODEL_IN_W * MODEL_IN_C;
    memcpy(buf, (uint8*)model_input_buf, MODEL_IN_W * MODEL_IN_H * MODEL_IN_C);

    results.clear();
    MODEL_RunInference();

    s_odretcnt = 0;
    if(!(yolo::DetectorPostProcess((const TfLiteTensor**)outputTensor,results, postProcessParams).DoPostProcess()))
    {
        s_odretcnt = 0;
    }
    for(const auto & result : results)
    {
        if(result.m_normalisedVal > postProcessParams.threshold)
        {
						// ������ڣ�7/12��Բ����Ŀ��������ĸ��˼·����һ��findģʽ������Ƭ�����������λ������ȡ����м��λ�÷���ȥ
            s_odrets[s_odretcnt].x1 = result.m_x0;
            s_odrets[s_odretcnt].x2 = result.m_x0 + result.m_w;
            s_odrets[s_odretcnt].y1 = result.m_y0;
            s_odrets[s_odretcnt].y2 = result.m_y0 + result.m_h;
            s_odrets[s_odretcnt].score = result.m_normalisedVal;
						// ***********************************************************************
						ratio_wh = result.m_w * 1.0 / result.m_h;
						#define WH_RATIO_CORRECT ((ratio_wh_thresh_min <= ratio_wh) && (ratio_wh <= ratio_wh_thresh_max))
						#define SQUARE_THRESH (110*90)
						#define WH_SQUARE_CORRECT (result.m_w * result.m_h <= SQUARE_THRESH)
						#define CIRCLE_R 180*180		// 185*185
						#define CARD_IN_CIRCLE (((result.m_x0 - 160) * (result.m_x0 - 160) + (result.m_y0 - 240) * (result.m_y0 - 240)) <= CIRCLE_R)
						
//						float x_filtered = kalman_update(&kf_x, result.m_x0);
//						float y_filtered = kalman_update(&kf_y, result.m_y0);
						double x_filtered = result.m_x0;
						double y_filtered = result.m_y0;
					
						dist2car =  sqrt((x_filtered - 160) * (x_filtered - 160) + (240 - y_filtered) * (240 - y_filtered));
					
						if(mode == 0) {
							buffer[2] = 0;
							buffer[3] = 0;
							buffer[4] = 0;
						}
						if (((CARD_IN_CIRCLE && (start_flag == 1) && (mode == 1))
								|| (locate_flag == 1 && CARD_IN_CIRCLE)			// locating					
								|| (goto_flag == 1 && result.m_normalisedVal >= 0.4)
								|| (mode == 3)) && WH_RATIO_CORRECT && WH_SQUARE_CORRECT)
						{
//							s_odrets[s_odretcnt].x1 = result.m_x0;
//							s_odrets[s_odretcnt].x2 = result.m_x0 + result.m_w;
//							s_odrets[s_odretcnt].y1 = result.m_y0;
//							s_odrets[s_odretcnt].y2 = result.m_y0 + result.m_h;
//							s_odrets[s_odretcnt].score = result.m_normalisedVal;
							
							zf_debug_printf("%d,%d\r\n",(int)x_filtered, (int)y_filtered);
							if (start_flag && mode == 1) {					// ��Detectionģʽ��
								// ������ڣ�7/18 ���Ŷȸ�Ϊ���ұ�־λ����0��1��
//								x_filtered = kalman_update(&kf_x, result.m_x0);
//								y_filtered = kalman_update(&kf_y, result.m_y0);
//								low_pass_filter(result.m_x0, result.m_y0, &x_filtered, &y_filtered);		// ��ͨ�˲���
								//buffer[1] = 0x01;	//���ұ�־
								if (x_filtered >= 150) {
									buffer[1] = 1;
								} else if (x_filtered < 150) {
									buffer[1] = 0;
								}
//								buffer[2] = (int)x_filtered / scale;
//								buffer[3] = (int)y_filtered / scale;
								dist2car =  sqrt((x_filtered - 160) * (x_filtered - 160) + (240 - y_filtered) * (240 - y_filtered));
								// 7/16 Ѱ������㷨������
								//7/16 ���ҵ��ķ���Ҫ��Ŀ�Ƭ�浽�����Ȼ����������ȡ�복�������score��ߵĿ�Ƭ
								if(dist2car < dist_to_car) {			// �����еĿ�Ƭ�����������복�������
										dist_to_car = dist2car;	// ��������Ŀ�Ƭ��Ϣ
										near_x = x_filtered;
										near_y = y_filtered;
										
								 } else {
										
								 }
								count_cards++;
								score = 0;
							} else if (locate_flag) {
//								x_filtered = kalman_update(&kf_x, result.m_x0);
//								y_filtered = kalman_update(&kf_y, result.m_y0);
//								buffer[1] = 0x01;
//								buffer[2] = (int)x_filtered / scale;
//								buffer[3] = (int)y_filtered / scale;
//								low_pass_filter(result.m_x0, result.m_y0, &x_filtered, &y_filtered);
								dist2car =  sqrt((x_filtered - 160) * (x_filtered - 160) + (240 - y_filtered) * (240 - y_filtered));
								if(dist2car < dist_to_car) {			// �����еĿ�Ƭ�����������복�������
										dist_to_car = dist2car;	// ��������Ŀ�Ƭ��Ϣ
										near_x = x_filtered;
										near_y = y_filtered;
								 }
								count_cards++;
							} else if (mode == 3) {				// ��Ѱ������ĸ��Ƭģʽ
								 if(fabs(x_filtered - 160) < dist_to_midline) {			// �����еĿ�Ƭ��������������Ļ���������
										dist_to_midline = fabs(x_filtered - 160);	// ��������Ŀ�Ƭ��Ϣ
										near_x = x_filtered;
										near_y = y_filtered;
								 }
								 count_cards++;
							}
						}
//						if (((result.m_y0 < 120 || result.m_y0 > 20) && start_flag == 1)) {	// slow down
//							buffer[1] = 0x02;
//							buffer[2] = (int)result.m_x0;
//							buffer[3] = (int)result.m_y0;
//						}
						s_odretcnt ++;
        }
    }
		// *****
		switch (mode) {
			case 0:ips200_show_string(180,10,"RESET");break;
			case 1:ips200_show_string(180,10,"Detecting");break;
			case 2:ips200_show_string(180,10,"Locating");break;
			case 3:ips200_show_string(180,10,"Find Nums/abc");break;
			default:ips200_show_string(180,10,"default");break;
		}
		buffer[4] = count_cards;
		if (mode == 3 || mode == 2 || mode == 1) {		// ����������ĸ��Ƭģʽ�� ��λģʽ��
			buffer[2] = (int)near_x / scale;
			buffer[3] = (int)near_y / scale;
		}
		if (mode != 0){
			user_uart_write_buffer(buffer, sizeof(buffer));
		}
		//count_cards = 0;	// �ǵù���
		// **********************************
#if IS_UART_OUTPUT_ODRESULT
    if(s_odretcnt > 0)
    {
        zf_model_odresult_out(s_odrets, s_odretcnt);
    }
#endif
#if IS_SHOW_SCC8660
    if(s_odretcnt)
    {
      draw_rect_on_slice_buffer((scc8660_image), SCC8660_W,  0, 1, s_odrets, s_odretcnt, SCC8660_H);
//			for (int i = 0;i <=s_odretcnt;i++)
//			{
//				s_odrets[i].x1 = 0;
//				s_odrets[i].x2 = 0;
//				s_odrets[i].y1 = 0;
//				s_odrets[i].y2 = 0;
//				s_odrets[i].score = 0;
//			}
		}
			ips200_show_scc8660(scc8660_image);
#endif

}

/*

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdint.h>  // ȷ��ʹ����ȷ������

#define THRESHOLD 50        // ������ֵ
#define STABILITY_FRAMES 10  // �ȶ���֡��

typedef struct {
    int32_t x;
    int32_t y;
    int jump_count;
} FilterState;

void initialize_filter_state(FilterState* state, int32_t x, int32_t y) {
    state->x = x;
    state->y = y;
    state->jump_count = 0;
}

void detect_and_filter_jumps(FilterState* state, int32_t x0, int32_t y0, int32_t threshold, int stability_frames, int32_t* x_filtered, int32_t* y_filtered) {
    int32_t dx = abs(x0 - state->x);
    int32_t dy = abs(y0 - state->y);

    if (dx < threshold && dy < threshold) {
        state->x = x0;
        state->y = y0;
        state->jump_count = 0;  // �������������
    } else {
        state->jump_count++;
        if (state->jump_count >= stability_frames) {
            state->x = x0;  // ��Ϊ�����µ�Ŀ��
            state->y = y0;
            state->jump_count = 0;  // �������������
        }
    }

    *x_filtered = state->x;
    *y_filtered = state->y;
}

void zf_model_run(void)
{
    uint8 buffer[5] = {0};
    // ͨ�Ÿ�ʽ��"@{conf}{x}{y}$"
    buffer[0] = (uint8)0X0A;
    buffer[4] = (uint8)0X0D;
    // ********************************************************************************
    uint8_t* buf = 0;
    memset(inputData, 0, inputDims.data[1]*inputDims.data[2]*inputDims.data[3]);
    buf = inputData + (inputDims.data[1] - MODEL_IN_H) / 2 * MODEL_IN_W * MODEL_IN_C;
    memcpy(buf, (uint8*)model_input_buf, MODEL_IN_W * MODEL_IN_H * MODEL_IN_C);

    results.clear();
    MODEL_RunInference();

    s_odretcnt = 0;
    if(!(yolo::DetectorPostProcess((const TfLiteTensor**)outputTensor,results, postProcessParams).DoPostProcess()))
    {
        s_odretcnt = 0;
    }

    static FilterState filter_state = {0, 0, 0};
    static int filter_initialized = 0;

    for(const auto& result : results)
    {
        if(result.m_normalisedVal > postProcessParams.threshold)
        {
            if (!filter_initialized) {
                initialize_filter_state(&filter_state, (int32_t)result.m_x0, (int32_t)result.m_y0);
                filter_initialized = 1;
            }

            int32_t x_filtered, y_filtered;
            detect_and_filter_jumps(&filter_state, (int32_t)result.m_x0, (int32_t)result.m_y0, THRESHOLD, STABILITY_FRAMES, &x_filtered, &y_filtered);

            s_odrets[s_odretcnt].x1 = x_filtered;
            s_odrets[s_odretcnt].x2 = x_filtered + result.m_w;
            s_odrets[s_odretcnt].y1 = y_filtered;
            s_odrets[s_odretcnt].y2 = y_filtered + result.m_h;
            s_odrets[s_odretcnt].score = result.m_normalisedVal;
            // ***********************************************************************
            if (((y_filtered > 120) && (start_flag == 1))
            || (locate_flag == 1 && y_filtered >= 100)						
            || (goto_flag == 1 && result.m_normalisedVal >= 0.6)) {
                buffer[1] = 0x01;
                buffer[2] = (uint8_t)x_filtered;
                buffer[3] = (uint8_t)y_filtered;
            }
            s_odretcnt ++;
        }
    }
    // *****
    switch (mode) {
        case 0:ips200_show_string(180,10,"RESET");break;
        case 1:ips200_show_string(180,10,"Detecting");break;
        case 2:ips200_show_string(180,10,"Locating");break;
        default:ips200_show_string(180,10,"default");break;
    }
    user_uart_write_buffer(buffer, sizeof(buffer));
    // *****
#if IS_UART_OUTPUT_ODRESULT
    if(s_odretcnt > 0)
    {
        zf_model_odresult_out(s_odrets, s_odretcnt);
    }
#endif
#if IS_SHOW_SCC8660
    if(s_odretcnt)
    {
        draw_rect_on_slice_buffer((scc8660_image), SCC8660_W,  0, 1, s_odrets, s_odretcnt, SCC8660_H);
    }
    ips200_show_scc8660(scc8660_image);
#endif
}
*/
