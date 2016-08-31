/**
  ******************************************************************************
  * @file   	pos_handle_layer.c
  * @author  	Tian erjun
  * @version 	V1.0.0.0
  * @date   	2015.11.05
  * @brief
  * @Changelog :
  *    [1].Date   : 2016_8-26
	*        Author : sam.wu
	*        brief  : ���Է������ݣ����ʹ������϶�
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
/* Private define ------------------------------------------------------------*/
/* Set parameter of budffer */
#define APP_CTR_IDLE                      0x00 // 0x00: idle
#define APP_CTR_SENT_DATA_VIA_2_4G        0x01 // 0x01: ͨ��2.4G�·����ݰ�����
#define APP_CTR_GET_DATA_VIA_2_4G         0x02 // 0x02: ͨ��2.4G���չ��������ݰ�����
#define APP_CTR_ADD_WHITE_LIST            0x11 // 0x11: ��Ӱ�����
#define APP_CTR_DEL_WHITE_LIST            0x12 // 0x12: ɾ��������
#define APP_CTR_INIT_WHITE_LIST           0x13 // 0x13: ��ʼ��������
#define APP_CTR_OPEN_WHITE_LIST           0x14 // 0x14: ����������
#define APP_CTR_CLOSE_WHITE_LIST          0x15 // 0x15: �رհ�����
#define APP_CTR_OPEN_ATTENDANCE           0x16 // 0x16: ��������
#define APP_CTR_CLOSE_ATTENDANCE          0x17 // 0x17: �رտ���
#define APP_CTR_UPDATE_ATTENDANCE_DATA    0x18 // 0x18: �ϴ�ˢ������
#define APP_CTR_OPEN_PAIR                 0x19 // 0x19: �������
#define APP_CTR_CLOSE_PAIR                0x0C // 0x0c: �ر����
#define APP_CTR_UPATE_PAIR_DATA           0x0D // 0x0d: �ϴ��������
#define APP_CTR_DATALEN_ERR               0xFE // 0xfe: ֡���Ȳ��Ϸ�
#define APP_CTR_UNKNOWN                   0xFF // 0xff: δ��ʶ���֡

/* Private variables ---------------------------------------------------------*/
uint8_t      flag_App_or_Ctr = APP_CTR_IDLE;
Uart_TypeDef uart232_var;					// ���ڽ��շ��ͻ�����

/* Private functions ---------------------------------------------------------*/
static void send_to_pos(void);
static void receive_from_pos(void);
static void serial_transmission_to_nrf51822(void);

void pos_handle_layer(void)
{
	send_to_pos();
	receive_from_pos();
	//serial_transmission_to_nrf51822();
}

static void send_to_pos(void)
{	
	if(App_to_CtrPosReq)
	{
#ifdef ENABLE_DEBUG_LOG
		app_debuglog_dump(Buf_AppToCtrPos, Length_AppToCtrPos);
#else		
		CtrUartSendTask(Buf_AppToCtrPos, Length_AppToCtrPos);
#endif //ENABLE_DEBUG_LOG	
		App_to_CtrPosReq = false;
	}
}

//	uint8_t 				HEADER;						  //�жϴ��ڽ���֡ͷ
//	uint8_t 				TYPE;								//�жϴ��ڽ��հ�����
//	uint8_t         SIGN[4];            //�жϴ��ڽ��ջ��ʶ
//	uint8_t 				LEN;								//�жϴ��ڽ������ݳ���
//	uint8_t 				DATA[UART_NBUF];		//�жϴ��ڽ�������
//	uint8_t 				XOR;								//�жϴ��ڽ������
//	uint8_t 				END;								//�жϴ��ڽ���֡β
static void serial_transmission_to_nrf51822(void)
{
	Buf_CtrPosToApp[0] = uart232_var.HEADER;
	Buf_CtrPosToApp[1] = uart232_var.TYPE;
	
	Buf_CtrPosToApp[2] = uart232_var.SIGN[0];
	Buf_CtrPosToApp[3] = uart232_var.SIGN[1];
	Buf_CtrPosToApp[4] = uart232_var.SIGN[2];
	Buf_CtrPosToApp[5] = uart232_var.SIGN[3];
	
	Buf_CtrPosToApp[6] = uart232_var.LEN;
	
	if(uart232_var.flag_uart_rx_xor_err)
	{
		uart232_var.flag_uart_rx_xor_err = false;
		flag_App_or_Ctr = APP_CTR_UPATE_PAIR_DATA;												//xor����
		hal_uart_clr_rx();
		return;
	}
	
	if(uart232_var.flag_uart_rx_length_err)
	{
		uart232_var.flag_uart_rx_length_err = false;
		flag_App_or_Ctr = APP_CTR_DATALEN_ERR;												//���ȳ���
		hal_uart_clr_rx();
		return;
	}
	
	if((uart232_var.flag_uart_rx_ok) && ( flag_App_or_Ctr == 0))				  //���ڽ��յ�posָ�������ϵͳ����
	{	
		Length_CtrPosToApp = uart232_var.LEN+7;							                //��ȡ���ݳ���
		memcpy(Buf_CtrPosToApp+7, uart232_var.DATA, uart232_var.LEN);		    //��ȡ��������
		flag_App_or_Ctr = 0x01;	
		hal_uart_clr_rx();														                      //�����������
		return ;
	}
	
}

static void receive_from_pos(void)
{
	Buf_CtrPosToApp[0] = uart232_var.HEADER;
	Buf_CtrPosToApp[1] = uart232_var.TYPE;

	if(uart232_var.flag_uart_rx_xor_err)
	{
		uart232_var.flag_uart_rx_xor_err = false;
		flag_App_or_Ctr = APP_CTR_UPATE_PAIR_DATA;												//xor����
		hal_uart_clr_rx();
		return;
	}
	
	if(uart232_var.flag_uart_rx_length_err)
	{
		uart232_var.flag_uart_rx_length_err = false;
		flag_App_or_Ctr = APP_CTR_DATALEN_ERR;												//���ȳ���
		hal_uart_clr_rx();
		return;
	}
	
	if((uart232_var.flag_uart_rx_ok) && ( flag_App_or_Ctr == 0))				    //���ڽ��յ�posָ�������ϵͳ����
	{	
		switch(uart232_var.TYPE)
		{
			case 0x10:
				if(uart232_var.uart_rx_buf[6] != uart232_var.uart_rx_buf[14] + 10)//�·�ָ��ȳ�������Ӧ�ò㴦��
				{
					flag_App_or_Ctr = 0xfe;	
					break;
				}
				Length_CtrPosToApp = uart232_var.LEN;							                //��ȡ���ݳ���
				memcpy(Buf_CtrPosToApp, uart232_var.DATA, uart232_var.LEN);		    //��ȡ��������
				flag_App_or_Ctr = 0x01;											                      //�·���������
				break;
			case 0x12:                                                          //ֹͣ�·�����
				if(uart232_var.uart_rx_buf[6] != 0)
				{
					flag_App_or_Ctr = 0xfe;	
					break;
				}
				flag_App_or_Ctr = 0x04;											//�·���������
				break;
			case 0x20:		//��Ӱ�����
				if(uart232_var.LEN != 4*uart232_var.uart_rx_buf[7] + 1)
				{
					flag_App_or_Ctr = 0xfe;	
					break;
				}
				Length_CtrPosToApp = uart232_var.uart_rx_buf[6];					//��ȡ���ݳ���
				memcpy(Buf_CtrPosToApp, &uart232_var.uart_rx_buf[7], Length_CtrPosToApp);//��ȡ��������
				flag_App_or_Ctr = 0x11;											//͸������������ָ��
				break;
			case 0x21:		//ɾ��������
				if(uart232_var.uart_rx_buf[6] != 4*uart232_var.uart_rx_buf[7] + 1)
				{
					flag_App_or_Ctr = 0xfe;	
					break;
				}
				Length_CtrPosToApp = uart232_var.uart_rx_buf[6];					//��ȡ���ݳ���
				memcpy(Buf_CtrPosToApp, &uart232_var.uart_rx_buf[7], Length_CtrPosToApp);//��ȡ��������
				flag_App_or_Ctr = 0x12;											//͸������������ָ��	
				break;
			case 0x22:		//��ʼ��������
				if(uart232_var.uart_rx_buf[6] != 0)
				{
					flag_App_or_Ctr = 0xfe;	
					break;
				}
				flag_App_or_Ctr = 0x13;											//͸������������ָ��	
				break;	
			case 0x23:		//����������
				if(uart232_var.uart_rx_buf[6] != 0)
				{
					flag_App_or_Ctr = 0xfe;	
					break;
				}
				flag_App_or_Ctr = 0x14;											//͸������������ָ��	
				break;	
			case 0x24:		//�رհ�����
				if(uart232_var.uart_rx_buf[6] != 0)
				{
					flag_App_or_Ctr = 0xfe;	
					break;
				}
				flag_App_or_Ctr = 0x15;											//͸������������ָ��	
				break;	
			case 0x25:		//��ʼ����
				if(uart232_var.uart_rx_buf[6] != 0)
				{
					flag_App_or_Ctr = 0xfe;	
					break;
				}
				flag_App_or_Ctr = 0x16;											//͸������������ָ��	
				break;	
			case 0x27:		//ֹͣ����
				if(uart232_var.uart_rx_buf[6] != 0)
				{
					flag_App_or_Ctr = 0xfe;	
					break;
				}
				flag_App_or_Ctr = 0x17;											//͸������������ָ��	
				break;	
			case 0x28:		//��ʼ���
				if(uart232_var.uart_rx_buf[6] != 0)
				{
					flag_App_or_Ctr = 0xfe;	
					break;
				}
				flag_App_or_Ctr = 0x18;											//͸������������ָ��	
				break;	
			case 0x2A:		//ֹͣ���
				if(uart232_var.uart_rx_buf[6] != 0)
				{
					flag_App_or_Ctr = 0xfe;	
					break;
				}
				flag_App_or_Ctr = 0x19;											//͸������������ָ��	
				break;	
			case 0x2B:		//��ӡ��ǰ������
				if(uart232_var.uart_rx_buf[6] != 0)
				{
					flag_App_or_Ctr = 0xfe;	
					break;
				}
				flag_App_or_Ctr = 0x1a;											//͸������������ָ��	
				break;	
			case 0x2C:		//��ȡ�豸��Ϣ
				if(uart232_var.uart_rx_buf[6] != 0)
				{
					flag_App_or_Ctr = 0xfe;	
					break;
				}
				flag_App_or_Ctr = 0x1b;											//͸������������ָ��	
				break;					
			default:		//�޷�ʶ���ָ��
				flag_App_or_Ctr = 0xff;	
				break;
		}
		hal_uart_clr_rx();														//�����������
	}	
}


/**
  * @}
  */
/**************************************END OF FILE****************************/

