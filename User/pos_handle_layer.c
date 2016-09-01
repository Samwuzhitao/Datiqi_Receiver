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

/* Private variables ---------------------------------------------------------*/

extern Uart_TxDataTypeDef  uart232_var;
       uint8_t             flag_App_or_Ctr = APP_CTR_IDLE;		 
	     uint8_t             sign_buffer[4];

/* Private functions ---------------------------------------------------------*/
static void send_to_pos(void);
static void receive_from_pos(void);
//static void serial_transmission_to_nrf51822(void);

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
//static void serial_transmission_to_nrf51822(void)
//{
//	Buf_CtrPosToApp[0] = uart232_var.HEADER;
//	Buf_CtrPosToApp[1] = uart232_var.TYPE;
//	
//	Buf_CtrPosToApp[2] = uart232_var.SIGN[0];
//	Buf_CtrPosToApp[3] = uart232_var.SIGN[1];
//	Buf_CtrPosToApp[4] = uart232_var.SIGN[2];
//	Buf_CtrPosToApp[5] = uart232_var.SIGN[3];
//	
//	Buf_CtrPosToApp[6] = uart232_var.LEN;
//	
//	if(uart232_var.flag_uart_rx_xor_err)
//	{
//		uart232_var.flag_uart_rx_xor_err = false;
//		flag_App_or_Ctr = APP_CTR_UPATE_PAIR_DATA;												//xor����
//		hal_uart_clr_rx();
//		return;
//	}
//	
//	if(uart232_var.flag_uart_rx_length_err)
//	{
//		uart232_var.flag_uart_rx_length_err = false;
//		flag_App_or_Ctr = APP_CTR_DATALEN_ERR;												//���ȳ���
//		hal_uart_clr_rx();
//		return;
//	}
//	
//	if((uart232_var.flag_uart_rx_ok) && ( flag_App_or_Ctr == 0))				  //���ڽ��յ�posָ�������ϵͳ����
//	{	
//		Length_CtrPosToApp = uart232_var.LEN+7;							                //��ȡ���ݳ���
//		memcpy(Buf_CtrPosToApp+7, uart232_var.DATA, uart232_var.LEN);		    //��ȡ��������
//		flag_App_or_Ctr = 0x01;	
//		hal_uart_clr_rx();														                      //�����������
//		return ;
//	}
//	
//}

static void receive_from_pos(void)
{
	Uart_MessageTypeDef CurrentProcessMessage;
	
	if(BUFFEREMPTY == buffer_get_buffer_status())
		return;
	else
		serial_ringbuffer_read_data(&CurrentProcessMessage);
	
	memcpy(sign_buffer,CurrentProcessMessage.SIGN,4);
	
	Buf_CtrPosToApp[0] = CurrentProcessMessage.HEADER;
	Buf_CtrPosToApp[1] = CurrentProcessMessage.TYPE;
	
  /* ���ڽ��յ�posָ�������ϵͳ���� */
	if( flag_App_or_Ctr == 0 )				    
	{	
		switch(CurrentProcessMessage.TYPE)
		{
			/* �·��������� */
			case 0x10:
				{ 
					/* ��ȡ���ݳ��� */
					Length_CtrPosToApp = CurrentProcessMessage.LEN;
					
					/* ��ȡ�������� */ 
					memcpy(Buf_CtrPosToApp, CurrentProcessMessage.DATA, CurrentProcessMessage.LEN);		 
					flag_App_or_Ctr = 0x01;
				}				
				break;
			
			/* ֹͣ�·����� */	
			case 0x12:   
				if(CurrentProcessMessage.LEN != 0)
				{
					flag_App_or_Ctr = 0xfe;	
					break;
				}
				flag_App_or_Ctr = 0x04;										
				break;
				
			case 0x20:		//��Ӱ�����
				if(CurrentProcessMessage.LEN != 4*CurrentProcessMessage.DATA[0] + 1)
				{
					flag_App_or_Ctr = 0xfe;	
					break;
				}
				Length_CtrPosToApp = CurrentProcessMessage.LEN;					//��ȡ���ݳ���
				memcpy(Buf_CtrPosToApp, CurrentProcessMessage.DATA, Length_CtrPosToApp);//��ȡ��������
				flag_App_or_Ctr = 0x11;											//͸������������ָ��
				break;
				
			case 0x21:		//ɾ��������
				if(CurrentProcessMessage.LEN != 4*CurrentProcessMessage.DATA[0] + 1)
				{
					flag_App_or_Ctr = 0xfe;	
					break;
				}
				Length_CtrPosToApp = CurrentProcessMessage.LEN;					//��ȡ���ݳ���
				memcpy(Buf_CtrPosToApp, CurrentProcessMessage.DATA, Length_CtrPosToApp);//��ȡ��������
				flag_App_or_Ctr = 0x12;											//͸������������ָ��	
				break;
				
			case 0x22:		//��ʼ��������
				if(CurrentProcessMessage.LEN != 0)
				{
					flag_App_or_Ctr = 0xfe;	
					break;
				}
				flag_App_or_Ctr = 0x13;											//͸������������ָ��	
				break;	
			case 0x23:		//����������
				if(CurrentProcessMessage.LEN != 0)
				{
					flag_App_or_Ctr = 0xfe;	
					break;
				}
				flag_App_or_Ctr = 0x14;											//͸������������ָ��	
				break;	
			case 0x24:		//�رհ�����
				if(CurrentProcessMessage.LEN != 0)
				{
					flag_App_or_Ctr = 0xfe;	
					break;
				}
				flag_App_or_Ctr = 0x15;											//͸������������ָ��	
				break;	
			case 0x25:		//��ʼ����
				if(CurrentProcessMessage.LEN != 0)
				{
					flag_App_or_Ctr = 0xfe;	
					break;
				}
				flag_App_or_Ctr = 0x16;											//͸������������ָ��	
				break;	
			case 0x27:		//ֹͣ����
				if(CurrentProcessMessage.LEN != 0)
				{
					flag_App_or_Ctr = 0xfe;	
					break;
				}
				flag_App_or_Ctr = 0x17;											//͸������������ָ��	
				break;	
			case 0x28:		//��ʼ���
				if(CurrentProcessMessage.LEN != 0)
				{
					flag_App_or_Ctr = 0xfe;	
					break;
				}
				flag_App_or_Ctr = 0x18;											//͸������������ָ��	
				break;	
			case 0x2A:		//ֹͣ���
				if(CurrentProcessMessage.LEN != 0)
				{
					flag_App_or_Ctr = 0xfe;	
					break;
				}
				flag_App_or_Ctr = 0x19;											//͸������������ָ��	
				break;	
			case 0x2B:		//��ӡ��ǰ������
				if(CurrentProcessMessage.LEN != 0)
				{
					flag_App_or_Ctr = 0xfe;	
					break;
				}
				flag_App_or_Ctr = 0x1a;											//͸������������ָ��	
				break;	
			case 0x2C:		//��ȡ�豸��Ϣ
				if(CurrentProcessMessage.LEN != 0)
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
	}	
}

/*********************************************************************************
* ��	�ܣ�void UartSendBuffferInit(void)
* ��  ��: NULL
* ��	�أ�NULL
* ��	ע��NULL
*********************************************************************************/
void UartSendBuffferInit(void)
{
	uart232_var.flag_tx_ok[0] = true;
	uart232_var.flag_tx_ok[1] = true;
}

/*********************************************************************************
* ��	�ܣ�void CtrUartSendTask(uint8_t *ptr,uint8_t len)
* ��    ��: ptr		���ͻ�����
* 			len		���ͳ���
* ��	�أ�NULL
* ��	ע��NULL
*********************************************************************************/
void CtrUartSendTask(uint8_t *ptr,uint8_t len)
{
	uint8_t temp_state = 0;
	if(len > 0)
	{
		if(uart232_var.flag_tx_ok[0] && uart232_var.flag_tx_ok[1])
		{
			temp_state = 1;					//��ǰϵͳ����
		}
		
		if(uart232_var.flag_tx_ok[uart_tx_i])		//�����ǰ���������У������ݷ��뵱ǰ�����������򣬷�������һ��
		{
			memcpy(uart232_var.uart_tx_buf[uart_tx_i], ptr, len);
			uart232_var.uart_tx_length[uart_tx_i] = len;
			uart232_var.flag_txing[uart_tx_i] = true;
			uart232_var.flag_tx_ok[uart_tx_i] = false;
		}
		else										//����ڶ������������ݣ�ֱ�Ӹ���
		{
			uart232_var.flag_txing[1 - uart_tx_i] = false;
			memcpy(uart232_var.uart_tx_buf[1 - uart_tx_i], ptr, len);
			uart232_var.uart_tx_length[1 - uart_tx_i] = len;
			uart232_var.flag_txing[1 - uart_tx_i] = true;
			uart232_var.flag_tx_ok[1 - uart_tx_i] = false;
		}
		
		if(temp_state)
		{
			uart232_var.uart_tx_cnt = 0;
			//Write one byte to the transmit data register
			USART_SendData(USART1pos, uart232_var.uart_tx_buf[uart_tx_i][uart232_var.uart_tx_cnt++]);
			uart232_var.uart_tx_length[uart_tx_i]--;
			USART_ITConfig(USART1pos, USART_IT_TXE, ENABLE);
		}
	}
	
//	for(temp_state=0;temp_state<4;temp_state++)
//  {
//		sign_buffer[temp_state]=0;
//  }	 
}



void app_debuglog_dump(uint8_t * p_buffer, uint32_t len)
{
	uint32_t index = 0;
	
    for (index = 0; index <  len; index++)
    {
        DebugLog("%02X ", p_buffer[index]);
    }
    DebugLog("\r\n");
}

void app_debuglog_dump_no_space(uint8_t * p_buffer, uint32_t len)
{
	uint32_t index = 0;
	
    for (index = 0; index <  len; index++)
    {
        DebugLog("%02X", p_buffer[index]);
    }
    DebugLog("\r\n");
}
/**
  * @}
  */
/**************************************END OF FILE****************************/

