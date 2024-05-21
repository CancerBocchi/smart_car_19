/*********************************************************************************************************************
* RT1064DVL6A Opensourec Library ����RT1064DVL6A ��Դ�⣩��һ�����ڹٷ� SDK �ӿڵĵ�������Դ��
* Copyright (c) 2022 SEEKFREE ��ɿƼ�
* 
* ���ļ��� RT1064DVL6A ��Դ���һ����
* 
* RT1064DVL6A ��Դ�� ��������
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
* ���֤������ libraries �ļ����� �����ļ����µ� LICENSE �ļ�
* ��ӭ��λʹ�ò����������� ���޸�����ʱ���뱣����ɿƼ��İ�Ȩ����������������
* 
* �ļ�����          zf_device_gps_tau1201
* ��˾����          �ɶ���ɿƼ����޹�˾
* �汾��Ϣ          �鿴 libraries/doc �ļ����� version �ļ� �汾˵��
* ��������          IAR 8.32.4 or MDK 5.33
* ����ƽ̨          RT1064DVL6A
* ��������          https://seekfree.taobao.com/
* 
* �޸ļ�¼
* ����              ����                ��ע
* 2022-09-21        SeekFree            first version
********************************************************************************************************************/
/*********************************************************************************************************************
* ���߶��壺
*                   ------------------------------------
*                   ģ��ܽ�            ��Ƭ���ܽ�
*                   RX                  �鿴 zf_device_gps_tau1201.h �� GPS_TAU1201_RX �궨��
*                   TX                  �鿴 zf_device_gps_tau1201.h �� GPS_TAU1201_TX �궨��
*                   VCC                 3.3V��Դ
*                   GND                 ��Դ��
*                   ------------------------------------
********************************************************************************************************************/

#ifndef _zf_device_gnss_h_
#define _zf_device_gnss_h_

#include "zf_common_typedef.h"

//--------------------------------------------------------------------------------------------------
//��������
//--------------------------------------------------------------------------------------------------
#define GNSS_UART           (UART_4)
#define GNSS_RX             (UART4_TX_C16)                                      // GPS RX�������ӵ���Ƭ����
#define GNSS_TX             (UART4_RX_C17)                                      // GPS TX��������

#define ANGLE_TO_RAD(x)     ( (x) * PI / 180.0 )                                // �Ƕ�ת��Ϊ����
#define RAD_TO_ANGLE(x)     ( (x) * 180.0 / PI )                                // ����ת��Ϊ�Ƕ�
#define PI                  ( 3.1415926535898 )

typedef enum
{
    TAU1201 = 1,                                                                // ��ɿƼ�˫ƵGPSģ��
    GN42A   = 1,                                                                // ��ɿƼ�˫ƵGPSģ�� ��TAU1201��һ����
    GN43RFA = 2,                                                                // ��ɿƼ���ƵRTKģ��
}gps_device_enum;

typedef struct
{
    uint16      year;  
    uint8       month; 
    uint8       day;
    uint8       hour;
    uint8       minute;
    uint8       second;
}gps_time_struct;

typedef struct
{
    gps_time_struct    time;                                                    // ʱ��
    
    uint8       state;                                                          // ��Ч״̬  1����λ��Ч  0����λ��Ч
    
    uint16      latitude_degree;                                                // ��
    uint16      latitude_cent;                                                  // ��
    uint16      latitude_second;                                                // ��    ��������Ǳ��Ŵ���100���ģ���Ҫ�Ǳ���ʹ�ø�����
    uint16      longitude_degree;                                               // ��
    uint16      longitude_cent;                                                 // ��
    uint16      longitude_second;                                               // ��    ��������Ǳ��Ŵ���100���ģ���Ҫ�Ǳ���ʹ�ø�����
    
    double      latitude;                                                       // γ��
    double      longitude;                                                      // ����
    
    int8        ns;                                                             // γ�Ȱ��� N�������򣩻� S���ϰ���
    int8        ew;                                                             // ���Ȱ��� E���������� W��������
    
    float       speed;                                                          // �ٶȣ�����/ÿСʱ��
    float       direction;                                                      // ���溽��000.0~359.9 �ȣ����汱��Ϊ�ο���׼��+
    uint8       antenna_direction_state;                                        // ˫���߲�����Ч״̬ 1��������Ч  0��������Ч����Чʱantenna_direction��������Ч��
    float       antenna_direction;                                              // ������ָ����������汱���ɵļнǣ�000.0~359.9 �ȣ�
    
    // ������������Ϣ��GNGGA����л�ȡ
    uint8       satellite_used;                                                 // ���ڶ�λ����������
    float       height;                                                         // �߶�
}gnss_info_struct;

typedef enum
{
    GPS_STATE_RECEIVING,                                                        // ���ڽ�������
    GPS_STATE_RECEIVED,                                                         // ���ݽ������
    GPS_STATE_PARSING,                                                          // ���ڽ���
}gps_state_enum;

extern gnss_info_struct gnss;
extern uint8            gnss_flag;


double      get_two_points_distance     (double lat1, double lng1, double lat2, double lng2);
double      get_two_points_azimuth      (double lat1, double lon1, double lat2, double lon2);
uint8       gnss_data_parse             (void);
void        gnss_uart_callback          (void);
void        gnss_init                   (gps_device_enum gps_device);

#endif
