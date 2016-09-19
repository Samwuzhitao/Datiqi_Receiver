/**
  ******************************************************************************
  * @file   	Main.c
  * @author  	Tian erjun
  * @version 	V1.0.0.0
  * @date   	2015.11.05
  * @brief   	main function for STM32F103RB
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stdlib.h"

extern void app_handle_layer(void);
extern void rc500_handle_layer(void);

/* Private functions ---------------------------------------------------------*/
static void Fee_Test(void);
static void nrf51822_spi_send_test(void);
static void nrf51822_spi_revice_test(void);
static void Whitelist_test( void );
static void Ringbuffer_test(void);
/******************************************************************************
  Function:main
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
int main(void)
{
	/* System initialize -------------------------------------------------------*/
	Platform_Init();

	/* System function test start-----------------------------------------------*/
	// Fee_Test();
	// nrf51822_spi_send_test();
	// nrf51822_spi_revice_test();
	// Whitelist_test();
	// Ringbuffer_test();
	/* System function test end ------------------------------------------------*/
	
	while(1)
	{	
		app_handle_layer();
		
//	printf("Uart revice Buffer status = %d uasge rate = %d \r\n",
//			buffer_get_buffer_status(0),serial_ringbuffer_get_usage_rate(0));
//	printf("Uart send   Buffer status = %d uasge rate = %d \r\n",
//			buffer_get_buffer_status(1),serial_ringbuffer_get_usage_rate(1));	
	}	
}

/******************************************************************************
  Function:Ringbuffer_test
  Description:
		���������Ժ���
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
void Ringbuffer_test(void)
{
	Uart_MessageTypeDef uart_test_message;
	uint16_t i = 0;
	
	uart_test_message.HEADER = 0x5C;
	uart_test_message.TYPE = 0X25;
	memset(uart_test_message.SIGN,0,4);
	uart_test_message.LEN = 0x00;
	uart_test_message.XOR = 0x5C;
	uart_test_message.END = 0xCA;
	
	while(1)
	{
		for(i=0;i<20;i++)
		{
			if(BUFFERFULL == buffer_get_buffer_status(REVICE_RINGBUFFER))
			{
				printf("Serial revice Buffer is full! \r\n");
			}
			else
			{
				serial_ringbuffer_write_data(REVICE_RINGBUFFER,&uart_test_message);
			}	
			printf("Uart revice Buffer status = %d uasge rate = %d \r\n",buffer_get_buffer_status(0),serial_ringbuffer_get_usage_rate(0));
		}
		
		for(i=0;i<10;i++)
		{
			if(BUFFEREMPTY == buffer_get_buffer_status(REVICE_RINGBUFFER))
			{
				printf("Serial revice Buffer is empty! \r\n");
			}
			else
			{
				serial_ringbuffer_read_data(REVICE_RINGBUFFER,&uart_test_message);
			}	
			printf("Uart revice Buffer status = %d uasge rate = %d \r\n",buffer_get_buffer_status(0),serial_ringbuffer_get_usage_rate(0));
		}
	}
}

/******************************************************************************
  Function:Whitelist_test
  Description:
		�������ݵ����������Ժ���
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
static void Whitelist_test( void )
{
	uint8_t i,j;
	uint8_t writeuid[4];
	uint8_t readuid[4];
	
	uint8_t uidpos = 0;
	uint8_t status =0;
	static uint32_t storetestcntOk = 0, storetestcntErr = 0,storetestcnt = 0;
	static uint32_t deletetestcntOk = 0,deletetestcntErr = 0,deletetestcnt =0 ;
	
	initialize_white_list();
	
	while(1)
	{
		uint8_t status = 0;
		uint8_t pos = 0;
		uint8_t uidvalue = 0;
		
		for(i=0;i<120;i++)
		{ 
			uidvalue = rand()/100;
			writeuid[0] = uidvalue + 0;
			writeuid[1] = uidvalue + 1;
			writeuid[2] = uidvalue + 2;
			writeuid[3] = uidvalue + 3;
			
			if(i == 64)
				printf(" uidpos = 64");
			
			status = add_uid_to_white_list(writeuid,&uidpos);
			DelayMs(20);
			if(status == OPERATION_SUCCESS)
			{
				printf(" uidpos = %d Write UID = %2X%2X%2X%2X \r\n",uidpos,writeuid[0],writeuid[1],writeuid[2],writeuid[3]);
				status = get_index_of_uid(uidpos,readuid);
				if(status == OPERATION_SUCCESS)
				{
					printf(" uidpos = %d Read  UID = %2X%2X%2X%2X \r\n",uidpos,readuid[0],readuid[1],readuid[2],readuid[3]);
		
					status = uidcmp(writeuid,readuid);
					if(status == OPERATION_SUCCESS)
					{
						storetestcntOk++;
					}
					else
					{
						storetestcntErr++;
					}
				}
				
				delete_uid_from_white_list(writeuid);
				printf("the len of white list is %d \r\n",get_len_of_white_list());
			}
			storetestcnt++;
			if(storetestcnt%20 == 0)
					printf(" Store OK cnt = %d  Err cnt = %d\r\n",storetestcntOk,storetestcntErr);
		}
		
	}
}

/******************************************************************************
  Function:nrf51822_spi_test
  Description:
		�������ݵ����������Ժ���
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
static void nrf51822_spi_send_test(void)
{
    static uint32_t i = 0;
    uint8_t j = 0;
	  uint8_t TestBuffer[10] = {0xAA,0x55,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88};
	
    while(1)
		{
				my_nrf_transmit_start(TestBuffer,10,NRF_DATA_IS_USEFUL);
				
				for(j=0;j<10;j++)
				{
					printf(" %2x ",TestBuffer[j]);
				}
				printf("\r\n");

				printf("Sent Data test num = %d! \r\n",i++);
				
				DelayMs(500);
		}
}
/******************************************************************************
  Function:Fee_Test
  Description:
		Flash ģ�� EEPROM ���Ժ���
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
static void Fee_Test(void)
{
/* Define --------------------------------------------------------------------*/
#define  TEST_END_NUM   480
#define  TEST_START_NUM 0
	
	uint16_t ReadData;
	uint32_t TestNum = 0;
	uint32_t TestOkNum = 0;
	uint32_t TestErrNum = 0;
	uint32_t WriteNum = 0;
	uint32_t WriteFlag = 0;
	uint16_t VarAddr = 0;

	/* EEPROM Init */
	Fee_Init(FEE_INIT_POWERUP);	
	
	while(1)
	{
	  /* --- Store many values in the EEPROM ---*/
		for (VarAddr = TEST_START_NUM; VarAddr <= TEST_END_NUM; VarAddr++)
		{
			if(VarAddr == TEST_END_NUM)
			{
				if(WriteFlag == 0)
					WriteFlag = 1;
				else
					WriteFlag = 0;
			}
			
			if(WriteFlag == 0)
				WriteNum = VarAddr;
			else
				WriteNum = 480-VarAddr;
			
			EE_WriteVariable(VarAddr, WriteNum);
			DelayMs(100);

			EE_ReadVariable(VarAddr, &ReadData);
			
			TestNum++;
			if(ReadData == WriteNum)
			{
				TestOkNum++;
			}
			else
			{
				TestErrNum++;
			}

				
			if(TestNum%100 == 0 )
			{
				printf("FEE Test Num = %5d write and read data Ok = %4d Error = %4d \r\n\r\n",
				        TestNum,TestOkNum,TestErrNum);
			}
		}
	}	
}

/******************************************************************************
  Function:nrf51822_spi_revice_test
  Description:
		���մ��������ݲ��Ժ���
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
static void nrf51822_spi_revice_test(void)
{
	uint16_t temp_len = 0;
	datiqi_type_t recv_package;
	answer_packet_t ans_package;
	
	while(1)
	{
		if(1 == rf_var.flag_rx_ok)								//���ݽ������
		{	
			recv_package.payload_len = rf_var.rx_buf[7];
			memcpy(&recv_package, rf_var.rx_buf, recv_package.payload_len + 8);
			ledToggle(LED1);
		
			DebugLog("\r\n===========================================================================\r\n");

			DebugLog("recv_raw_data:");
			for(temp_len = 0; temp_len < rf_var.rx_len; temp_len++)
			{
				DebugLog(" %02X ", rf_var.rx_buf[temp_len]);
			}	
		
			DebugLog("\r\nrecv_data_len: %d  \r\n", rf_var.rx_len);

			if(recv_package.header == 0x5A)
			{

				DebugLog("UID:		");
				for(temp_len = 0; temp_len < 4; temp_len++)
				{
					DebugLog("%02X", recv_package.uid[temp_len]);
				}
				DebugLog("\r\n");	
				
				if(recv_package.pack_type == 0x10||recv_package.pack_type == 0x11)
				{
					memcpy(&ans_package, recv_package.payload, recv_package.payload_len);
					for(temp_len = 0; temp_len < ans_package.question_amount; temp_len ++)
					{
						DebugLog("-------------------------------------------------------------------------\r\n");
						DebugLog("question ID:	%d\r\n", ans_package.question_number + temp_len );
						DebugLog("question type:	");
						switch((ans_package.question_type[(15-temp_len)/4]&(0x03 << (temp_len%4))) >> (temp_len%4) )
						{
							case SINGLE_CHOICE:
								DebugLog("single choice\r\n");
								DebugLog("choice answer:	");
								switch(ans_package.answer[temp_len].choice_answer)
								{
									case A:
										DebugLog("A");
										break;
									case B:
										DebugLog("B");
										break;
									case C:
										DebugLog("C");
										break;
									case D:
										DebugLog("D");
										break;
									case E:
										DebugLog("E");
										break;
									case F:
										DebugLog("F");
										break;
									case G:
										DebugLog("G");
										break;
									case H:
										DebugLog("H");
										break;
									default:
										break;
								}
								DebugLog("\r\n");
								break;
							case MULTIPLE_CHOICE:
								DebugLog("multiple choice\r\n");
								DebugLog("choice answer:	");
								if(ans_package.answer[temp_len].choice_answer & A)
								{
									DebugLog("A	");
								}
								if(ans_package.answer[temp_len].choice_answer & B)
								{
									DebugLog("B	");
								}
								if(ans_package.answer[temp_len].choice_answer & C)
								{
									DebugLog("C	");
								}
								if(ans_package.answer[temp_len].choice_answer & D)
								{
									DebugLog("D	");
								}
								if(ans_package.answer[temp_len].choice_answer & E)
								{
									DebugLog("E	");
								}
								if(ans_package.answer[temp_len].choice_answer & F)
								{
									DebugLog("F	");
								}
								if(ans_package.answer[temp_len].choice_answer & G)
								{
									DebugLog("G	");
								}
								if(ans_package.answer[temp_len].choice_answer & H)
								{
									DebugLog("H	");
								}
								DebugLog("\r\n");
								break;
							case TRUE_OR_FALSE:
								DebugLog("true or false\r\n");
								if(ans_package.answer[temp_len].true_or_false == 0x01)
								{
									DebugLog("answer:		%c\r\n", 86);
								}
								else
								{
									DebugLog("answer:		%c\r\n", 88);
								}
								break;
							default:
								break;
						}				
					}				
					
				}
				else if(recv_package.pack_type == 0x12)
				{
					DebugLog("-------------------------------------------------------------------------\r\n");
					DebugLog("score:		%d\r\n", recv_package.payload[0]);
				}
			}
			DebugLog("===========================================================================\r\n");
			
			//hal_uart_clr_rx();
			rf_var.flag_rx_ok = 0;
			memset(rf_var.rx_buf, 0, rf_var.rx_len);
			rf_var.rx_len = 0;
		}
	}
}


/**************************************END OF FILE****************************/
