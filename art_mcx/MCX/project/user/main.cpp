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
* �ļ�����          main
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
#if defined(__cplusplus)
extern "C" // mian�ļ���C++�ļ��������Ҫ����C���Ե�ͷ�ļ�������Ҫʹ��extern "C"
{
#endif /* __cplusplus */ 
#include "zf_common_headfile.h"

// ���µĹ��̻��߹����ƶ���λ�����ִ�����²���
// ��һ�� �ر��������д򿪵��ļ�
// �ڶ��� project->clean  �ȴ��·�����������

// ���İ������������Ҫ�ֶ���λ������
// ���İ������������Ҫ�ֶ���λ������
// ���İ������������Ҫ�ֶ���λ������.
    
// ����������к�ģ���ϵ�Core����˸��˵�����������HardFault������Ҫ����Ƿ�������Խ�磬����û�г�ʼ��������ʱ��û�����õ����
    
// *************************** ���̲���˵�� ***************************
// 1.ʹ��DAP����������ģ��,��IPS2.0��Ļ�嵽ģ���ϣ�ע�����ŷ���
// 
// 2.MCX Vision���ر����̣�
// 
// 3.��λ���İ壨��Ҫ�ֶ���λ�� 
// 
// 4.��Ļ����ʾʵʱͼ������ģ�Ϳ��Ŀ��
//
// 5.DEBUG���û����ڻ����Ŀ�����Ϣ��������zf_model_process.cpp�ļ���zf_model_odresult_out�����鿴
//   ������û���������1064�Ĵ���4��ʹ���ṩ��1064���̣����Խ��ղ�����Ŀ���������

// *************************** ����ע������ ***************************
// 1.Ĭ�ϵķֱ�����QVGA������ͷ�ķֱ��ʿ�����zf_device_scc8660.h�ļ����޸�FRAME_SIZE�궨��
//
// 2.���������Ļ��ʾ��QVGA��QQVGA��֡��Լ30֡��������Ļ��ʾ��QVGAԼ20֡��QQVGAԼ30֡
//
// 3.ע�⣬QVGAͼ�������ģ��ռ�û���ռ������RAM���޷�����������㷨�������Ҫд�����㷨������ȥ��ģ�ͻ���ʹ��QQVGA����
//
// 4.ģ�͵�ѵ���ο�MCX Vision˵����
//
// **************************** �������� ****************************
#include "param_adjust.h"
int main(void)
{
    // ʱ�Ӻ͵��Դ���-����4��ʼ��
    zf_board_init();
    // �û�����-����5��ʼ��
    user_uart_init();
    // ��ʱ300ms
    system_delay_ms(300);
    // ʹ��C++�����޷�ʹ��printf������ʹ��zf_debug_printf��zf_user_printf���
    zf_debug_printf("debug_uart_init_finish\r\n");  // ʹ�õ��Դ���-����4��������
    zf_user_printf("user_uart_init_finish\r\n");    // ʹ���û�����-����5��������

    // ��Ļ��ʼ��
    ips200_init();
    // ����ͷ��ʼ��
    scc8660_init();
    // ģ�ͳ�ʼ����ģ�͵������ֵ�޸ĺ����е�postProcessParams.threshold��������Χ��0 - 1��Ĭ��Ϊ0.4������ԽС��Խ����ʶ��Ŀ��
    zf_model_init();
    while (1)
    {
        if(scc8660_finish)
        {
            scc8660_finish = 0;
            // ģ�����в���ʾͼ��
            zf_model_run();
        }
    }
}


extern int start_flag;
extern int locate_flag;
extern int goto_flag;
extern int mode;

void UART5_IRQHandler(uint8 data) {
	//zf_user_printf("%c", data);
//	if (data == 'E') {
//		start_flag = 0;
//	}
	static uint8_t brightness_flag = 0;
	if (data == 'R') {
		start_flag = 0;
		locate_flag = 0;
		goto_flag = 0;
		mode = 0;
	}
	if (data == 'D') {
		start_flag = 1;
		locate_flag = 0;
		goto_flag = 0;
		mode = 1;
	}
	if (data == 'L') {
		locate_flag = 1;
		start_flag = 0;
		goto_flag = 0;
		mode = 2;
	}
	if (data == 'F') {		// ��Ѱ���ڷſ�Ƭ��������ĸ��Ƭ
		locate_flag = 0;
		start_flag = 1;
		goto_flag = 0;
		mode = 3;
	}
//	if (data == 'N') {
//		locate_flag = 0;
//	}
	if (data == 'G') {
		locate_flag = 0;
		start_flag = 0;
		goto_flag = 1;
	}
	if (data == 'c')		// Configue ��ѯ�����б�
	{
		zf_user_printf("brightness:%d\n", brightness);	// ����ͷ�ع� Ĭ��ֵ200
	}
	if (data == 'b')		// Brightness �����ع�
	{
		brightness_flag = 1;
	}
	if (data == 'e')
	{
		brightness_flag = 0;
		zf_user_printf("exit!\n");
	}
	if (brightness_flag == 1) {
		brightness = data * 2;
		if(scc8660_set_brightness(data*2) == 0)		// ��������Ϊdata��uint8����
		{
			zf_user_printf("success with brightness: %d\n", brightness);
		} else
		{
			zf_user_printf("failed!\n");
		}
	}
//	if (data == 'B') {
//		goto_flag = 0;
//	}
}

// **************************** �������� ****************************
#if defined(__cplusplus)
}
#endif /* __cplusplus */