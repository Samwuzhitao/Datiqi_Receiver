/**
  ******************************************************************************
  * @file   	pos_handle_layer.c
  * @author  	Tian erjun
  * @version 	V1.0.0.0
  * @date   	2015.11.05
  * @brief   	
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

static void send_to_pos(void);
static void receive_from_pos(void);

void pos_handle_layer(void)
{
	send_to_pos();
	receive_from_pos();
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

static void receive_from_pos(void)
{
	if(uart232_var.flag_uart_rx_xor_err)
	{
		uart232_var.flag_uart_rx_xor_err = false;
		Buf_CtrPosToApp[0] = uart232_var.HEADER;
		Buf_CtrPosToApp[1] = uart232_var.TYPE;
		flag_App_or_Ctr = 0x0d;												//xor����
		hal_uart_clr_rx();
		return;
	}
	
	if(uart232_var.flag_uart_rx_length_err)
	{
		uart232_var.flag_uart_rx_length_err = false;
		Buf_CtrPosToApp[0] = uart232_var.HEADER;
		Buf_CtrPosToApp[1] = uart232_var.TYPE;
		flag_App_or_Ctr = 0xfe;												//���ȳ���
		hal_uart_clr_rx();
		return;
	}
	
	if((uart232_var.flag_uart_rx_ok) && ( flag_App_or_Ctr == 0))				//���ڽ��յ�posָ�������ϵͳ����
	{	
		switch(uart232_var.TYPE)
		{
			case 0x10:
				if(uart232_var.uart_rx_buf[6] != uart232_var.uart_rx_buf[14] + 10)//�·�ָ��ȳ�������Ӧ�ò㴦��
				{
					Buf_CtrPosToApp[0] = uart232_var.HEADER;
					Buf_CtrPosToApp[1] = uart232_var.TYPE;
					flag_App_or_Ctr = 0xfe;	
					break;
				}
				Length_CtrPosToApp = uart232_var.LEN;							//��ȡ���ݳ���
				memcpy(Buf_CtrPosToApp, uart232_var.DATA, uart232_var.LEN);		//��ȡ��������
				flag_App_or_Ctr = 0x01;											//�·���������
				break;
			case 0x12:                                                          //ֹͣ�·�����
				if(uart232_var.uart_rx_buf[6] != 0)
				{
					Buf_CtrPosToApp[0] = uart232_var.HEADER;
					Buf_CtrPosToApp[1] = uart232_var.TYPE;
					flag_App_or_Ctr = 0xfe;	
					break;
				}
				flag_App_or_Ctr = 0x04;											//�·���������
				break;
			case 0x20:		//��Ӱ�����
				if(uart232_var.LEN != 4*uart232_var.uart_rx_buf[7] + 1)
				{
					Buf_CtrPosToApp[0] = uart232_var.HEADER;
					Buf_CtrPosToApp[1] = uart232_var.TYPE;
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
					Buf_CtrPosToApp[0] = uart232_var.HEADER;
					Buf_CtrPosToApp[1] = uart232_var.TYPE;
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
					Buf_CtrPosToApp[0] = uart232_var.HEADER;
					Buf_CtrPosToApp[1] = uart232_var.TYPE;
					flag_App_or_Ctr = 0xfe;	
					break;
				}
				flag_App_or_Ctr = 0x13;											//͸������������ָ��	
				break;	
			case 0x23:		//����������
				if(uart232_var.uart_rx_buf[6] != 0)
				{
					Buf_CtrPosToApp[0] = uart232_var.HEADER;
					Buf_CtrPosToApp[1] = uart232_var.TYPE;
					flag_App_or_Ctr = 0xfe;	
					break;
				}
				flag_App_or_Ctr = 0x14;											//͸������������ָ��	
				break;	
			case 0x24:		//�رհ�����
				if(uart232_var.uart_rx_buf[6] != 0)
				{
					Buf_CtrPosToApp[0] = uart232_var.HEADER;
					Buf_CtrPosToApp[1] = uart232_var.TYPE;
					flag_App_or_Ctr = 0xfe;	
					break;
				}
				flag_App_or_Ctr = 0x15;											//͸������������ָ��	
				break;	
			case 0x25:		//��ʼ����
				if(uart232_var.uart_rx_buf[6] != 0)
				{
					Buf_CtrPosToApp[0] = uart232_var.HEADER;
					Buf_CtrPosToApp[1] = uart232_var.TYPE;
					flag_App_or_Ctr = 0xfe;	
					break;
				}
				flag_App_or_Ctr = 0x16;											//͸������������ָ��	
				break;	
			case 0x27:		//ֹͣ����
				if(uart232_var.uart_rx_buf[6] != 0)
				{
					Buf_CtrPosToApp[0] = uart232_var.HEADER;
					Buf_CtrPosToApp[1] = uart232_var.TYPE;
					flag_App_or_Ctr = 0xfe;	
					break;
				}
				flag_App_or_Ctr = 0x17;											//͸������������ָ��	
				break;	
			case 0x28:		//��ʼ���
				if(uart232_var.uart_rx_buf[6] != 0)
				{
					Buf_CtrPosToApp[0] = uart232_var.HEADER;
					Buf_CtrPosToApp[1] = uart232_var.TYPE;
					flag_App_or_Ctr = 0xfe;	
					break;
				}
				flag_App_or_Ctr = 0x18;											//͸������������ָ��	
				break;	
			case 0x2A:		//ֹͣ���
				if(uart232_var.uart_rx_buf[6] != 0)
				{
					Buf_CtrPosToApp[0] = uart232_var.HEADER;
					Buf_CtrPosToApp[1] = uart232_var.TYPE;
					flag_App_or_Ctr = 0xfe;	
					break;
				}
				flag_App_or_Ctr = 0x19;											//͸������������ָ��	
				break;	
			case 0x2B:		//��ӡ��ǰ������
				if(uart232_var.uart_rx_buf[6] != 0)
				{
					Buf_CtrPosToApp[0] = uart232_var.HEADER;
					Buf_CtrPosToApp[1] = uart232_var.TYPE;
					flag_App_or_Ctr = 0xfe;	
					break;
				}
				flag_App_or_Ctr = 0x1a;											//͸������������ָ��	
				break;	
			case 0x2C:		//��ȡ�豸��Ϣ
				if(uart232_var.uart_rx_buf[6] != 0)
				{
					Buf_CtrPosToApp[0] = uart232_var.HEADER;
					Buf_CtrPosToApp[1] = uart232_var.TYPE;
					flag_App_or_Ctr = 0xfe;	
					break;
				}
				flag_App_or_Ctr = 0x1b;											//͸������������ָ��	
				break;					
			default:		//�޷�ʶ���ָ��
				flag_App_or_Ctr = 0xff;	
				Buf_CtrPosToApp[0] = uart232_var.HEADER;
				Buf_CtrPosToApp[1] = uart232_var.TYPE;
				break;
		}			
		hal_uart_clr_rx();														//�����������
	}	
}


/**
  * @}
  */
/**************************************END OF FILE****************************/

