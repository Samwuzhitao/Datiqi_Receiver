/**
  ******************************************************************************
  * @file   	clock.c
  * @author  	Tian erjun
  * @version 	V1.0.0.0
  * @date   	2015.11.05
  * @brief   	clock config functions and delay functions
  ******************************************************************************
  */
  
#include "main.h"
white_list_t				white_list[MAX_WHITE_LEN];		// �������б�
uint8_t						white_len;						// ����������
Switch_State				white_on_off;					// ���������� 
Switch_State				attendance_on_off;				// ���ڿ��� 
Switch_State				match_on_off;					// ��Կ��� 
uint16_t					match_number = 1;					// ������

uint8_t 					NDEF_DataWrite[30] = {0x00, 0x00, 0x0f, 0x0e, 0x0d, 0x0c, 0x0b, 0x0a, 0x09, 0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x00};
uint8_t 					NDEF_DataRead[0xFF] = {0x00};

bool 						gbf_hse_setup_fail = FALSE;		// �ⲿ16M���������־
uint8_t 					time_for_buzzer_on = 0;			// �೤ʱ��֮��ʼ��
uint16_t 					time_for_buzzer_off = 0;		// ���˶೤ʱ��֮���

nrf_parameter_t				nrf_parameter;					// 2.4G��ʼ������

uint8_t                     sign_buffer[4];
uint8_t						uart_tx_i = 0;					// ���ڷ��ͻ�������
Uart_TypeDef 				uart232_var;					// ���ڽ��շ��ͻ�����
RF_TypeDef 					rf_var;							// 2.4G���ݰ�����

uint16_t					delay_nms = 0;					// �ж���ʱ����
uint32_t 					timer_1ms = 0;					// ι����ʱ��
time_t						time;							// RTC�ṹ��

uint8_t						uid_len = 0;					// M1�����кų���
uint8_t 					g_cSNR[10];						// M1�����к�

bool						flag_upload_uid_once = false;	// �Ƿ񵥴��ϴ����ű�־
uint8_t						ReadNDEF_Step = 0;				// ��ȡNDEF�ļ��Ĳ���
uint8_t						FindCard_Step = 0;				// Ѱ���Ĳ���

	
uint16_t					Length_CtrRC500ToApp;			// RC500�������ݳ���
uint16_t 					Length_AppToCtrRC500;			// Ӧ�ò��·�RC500ָ���

uint16_t					Length_CtrPosToApp;				// pos�·�ָ���
uint16_t					Length_AppToCtrPos;				// Ӧ�ò��ϱ�ָ���
	
uint8_t						Buf_CtrRC500ToApp[UART_NBUF];	// RC500�������ݻ�����
uint8_t						Buf_AppToCtrRC500[UART_NBUF];	// Ӧ�ò��·�RC500ָ�����
	
uint8_t						Buf_CtrPosToApp[UART_NBUF];		// pos�·�ָ���
uint8_t						Buf_AppToCtrPos[UART_NBUF];		// Ӧ�ò��ϱ�ָ�����

uint8_t						Buf_CtrRC500return[UART_NBUF];	// RC500�������ݻ�����������

bool 						App_to_CtrPosReq = false;		// posָ������
bool 						App_to_CtrRC500Req = false;		// RC500ָ������

uint8_t 					g_cardType[40] = {0x00};		// ���ؿ�����
uint8_t 					respon[BUF_LEN + 20] = {0x00};
uint8_t 					g_cCid;							

uint8_t 					flag_App_or_Ctr = 0;			// 0x00: idle
															// 0x01: ͨ��2.4G�·����ݰ�����
															// 0x02: ͨ��2.4G���չ��������ݰ�����
															// 0x11: ��Ӱ�����
															// 0x12: ɾ��������
															// 0x13: ��ʼ��������
															// 0x14: ����������
															// 0x15: �رհ�����
															// 0x16: ��������
															// 0x17: �رտ���
															// 0x18: �ϴ�ˢ������
															// 0x19: �������
															// 0x0c: �ر����
															// 0x0d: �ϴ��������
															
															// 0xfe: ֡���Ȳ��Ϸ�
															// 0xff: δ��ʶ���֡ 


uint32_t                     MCUID[4];
uint8_t                      jsq_uid[8];
uint8_t                      software[3]={0x01,0x00,0x03};
uint8_t 					 hardware[30] = {0x5,0xa,0x4,0xc,0x2,0xd,0x5,0x2,0x5,0x0,0x3,0x5,0x3,0x5,0x3,0x1,0x2,0xd,0x4,0xd,0x4,0x1,0x4,0x9,0x4,0xe,0x2,0xd,0x4,0x4 };
uint8_t 					 company[16] = {0xd,0x1,0xb,0x6,0xc,0x1,0xa,0xa,0xb,0x9,0xc,0x9,0xb,0x7,0xd,0xd};
/**************************************END OF FILE****************************/

