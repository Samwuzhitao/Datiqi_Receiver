/**
  ******************************************************************************
  * @file   	hal_uart.c
  * @author  	Tian erjun
  * @version 	V1.0.0.0
  * @date   	2015.11.05
  * @brief   	hal function for uart
  ******************************************************************************
  */
 
#include "main.h"

/*********************************************************************************
* ��	�ܣ�void hal_uart_clr_tx(uint8_t tx_index)
* ��    ��: NULL
* ��	�أ�NULL
* ��	ע�����㷢����ر���
*********************************************************************************/
void hal_uart_clr_tx(uint8_t tx_index)
{	
	uart232_var.flag_txing[tx_index] = false;		//���ͽ���
	memset(uart232_var.uart_tx_buf[tx_index], 0x00,  uart232_var.uart_tx_cnt);
	uart232_var.uart_tx_cnt = 0x00;
}

/*********************************************************************************
* ��	�ܣ�void hal_uart_clr_rx(void)
* ��    ��: NULL
* ��	�أ�NULL
* ��	ע�����������ر���
*********************************************************************************/
void hal_uart_clr_rx(void)
{
    uint8_t i;
	uart232_var.temp_uid_len = 0;
	uart232_var.temp_data_len = 0;
	uart232_var.temp_sign_len = 0;
	uart232_var.HEADER = 0x00;
	uart232_var.TYPE = 0x00;
	for(i = 0; i < uart232_var.LEN; i++)
	{
		uart232_var.DATA[i] = 0x00;
	}
	uart232_var.LEN = 0x00;
	uart232_var.END = 0x00;
	uart232_var.XOR = 0x00;
	uart232_var.flag_uart_rxing = 0;
	uart232_var.flag_uart_rx_ok = 0;
	uart232_var.uart_rx_timeout = 0;
	for(i = 0; i < 4; i++)
	{
		uart232_var.SIGN[i] = 0;
	}
	
	for(i = 0; i < uart232_var.uart_rx_cnt; i++)
	{
		uart232_var.uart_rx_buf[i] = 0;
	}
	uart232_var.uart_rx_cnt = 0;
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
for(temp_state=0;temp_state<4;temp_state++)
 {
    sign_buffer[temp_state]=0;
 }	 
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


