/**
  ******************************************************************************
  * @file   	app_card_process.c
  * @author  	sam.wu
  * @version 	V1.0.0.0
  * @date   	2015.11.05
  * @brief
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "app_serial_cmd_process.h"
#include "app_card_process.h"

//#define SHOW_CARD_PROCESS_TIME
extern uint8_t g_cSNR[10];	
extern WhiteList_Typedef wl;
extern Revicer_Typedef   revicer;
extern rf_config_typedef clicker_set;
task_tcb_typedef card_task;

#ifdef SHOW_CARD_PROCESS_TIME
extern __IO uint32_t     PowerOnTime;
uint32_t StartTime,EndTime;
#endif

static Uart_MessageTypeDef card_message;
static uint8_t card_process_status = 0;
static uint8_t is_white_list_uid = 0;
/* ���ؿ����� */
uint8_t g_cardType[40];	
uint8_t respon[BUF_LEN + 20];	
static rf_id_typedf rID,wID;

uint16_t NDEF_Len               = 0;
static uint8_t g_uid_len        = 0;
static uint8_t wtrte_flash_ok   = 0;
static uint8_t read_uid_pos     = 0xFF;
static uint8_t write_uid_pos    = 0xFF;
static uint8_t card_message_err = 0;
static uint8_t find_card_ok     = 0;
/******************************************************************************
  Function:rf_set_card_status
  Description:
		�޸�systick��״̬
  Input :
		rf_status: systick����״̬
  Output:
  Return:
  Others:None
******************************************************************************/
void rf_set_card_status(uint8_t new_status)
{
	card_process_status = new_status;
}

/******************************************************************************
  Function:rf_get_card_status
  Description:
		��ȡsystick��״̬
  Input :
  Output:systick����״̬
  Return:
  Others:None
******************************************************************************/
uint8_t rf_get_card_status(void)
{
	return card_process_status ;
}

/******************************************************************************
  Function:App_card_process
  Description:
		App MI Card ��ѯ������
  Input :
  Return:
  Others:None
******************************************************************************/
void App_card_process(void)
{
	/* ��ȡ��ǰ״̬ */
	uint8_t card_current_status = 0;
	
	card_current_status = rf_get_card_status();

	if( card_current_status == 1 )
	{
		uint8_t status = 0;
		#ifdef SHOW_CARD_PROCESS_TIME
		StartTime = PowerOnTime;
		#endif

		PcdAntennaOn();
	 // MRC500_DEBUG_START("PcdRequest \r\n");
		memset(g_cardType, 0, 40);
		/* reqAָ�� :����A�������ؿ����ͣ���ͬ���Ϳ���Ӧ��ͬ��UID���� */
		status = PcdRequest(PICC_REQIDL,g_cardType);
	  //MRC500_DEBUG_END();
		if( status == MI_OK )
		{
			//DEBUG_CARD_DEBUG_LOG("PcdRequest status = %d\r\n",status);
			if(find_card_ok == 1)
			{
				sw_clear_timer(&card_second_find_timer);
				return;
			}
			if(find_card_ok == 2)
			{
				find_card_ok = 0;
			}
		
			if( (g_cardType[0] & 0x40) == 0x40)
			{	
				g_uid_len = 8;	
			}
			else
			{	
				g_uid_len = 4;
			}
			DEBUG_CARD_DEBUG_LOG("uid len = %d\r\n",g_uid_len);
		}
		else
		{
			//DEBUG_CARD_DEBUG_LOG("PcdRequest status = %d\r\n",status);
			return;
		}
		/* ����ײ1 */
		//MRC500_DEBUG_START("PcdAnticoll \r\n");
		status = PcdAnticoll(PICC_ANTICOLL1, g_cSNR);
		//MRC500_DEBUG_END();
		DEBUG_CARD_DEBUG_LOG("PcdAnticoll status = %d\r\n",status);
		if( status != MI_OK )
		{
			return;
		}

		/* ѡ��1 */
		memset(respon, 0, 10);
		status = PcdSelect1(g_cSNR, respon);
		DEBUG_CARD_DEBUG_LOG("PcdSelect1 status = %d\r\n",status);
		if( status == MI_OK )
		{
			if((g_uid_len == 8) && ((respon[0] & 0x04) == 0x04))
			{
				//MRC500_DEBUG_START("PICC_ANTICOLL2 \r\n");
				status = PcdAnticoll(PICC_ANTICOLL2, &g_cSNR[4]);
				//MRC500_DEBUG_END();
				if( status != MI_OK)
				{
					return;
				}
				status = PcdSelect2(&g_cSNR[4], respon);
				DEBUG_CARD_DEBUG_LOG("PcdSelect2 status = %d\r\n",status);
				if( status == MI_OK)
				{
					rf_set_card_status(2);
				}
				else
				{
					return;
				}
			}
		}
		else
		{
			return;
		}
		#ifdef SHOW_CARD_PROCESS_TIME
		EndTime = PowerOnTime - StartTime;
		printf("UseTime:PcdSelect2 = %d \r\n",EndTime);
		#endif
	}

	if( card_current_status == 2 )
	{
		uint8_t status = 0;
		/*ѡ��Ӧ��*/
		status = SelectApplication();
		DEBUG_CARD_DEBUG_LOG("SelectApplication status = %d\r\n",status);
		if( status != MI_OK )
		{
			mfrc500_init();
			rf_set_card_status(1);
			return;
		}
		#ifdef SHOW_CARD_PROCESS_TIME
		EndTime = PowerOnTime - StartTime;
		printf("UseTime:SelectApplication = %d \r\n",EndTime);
		#endif
		/* ����ָ�� */
		if( wl.attendance_sttaus == ON )
		{
			uint8_t *pdata = (uint8_t *)&rID;
			uint8_t card_data_len = sizeof(rf_id_typedf);
			status = ReadNDEFfile(pdata, &NDEF_Len);
			DEBUG_CARD_DEBUG_LOG("ReadNDEFfile status = %d\r\n",status);
			if( status != MI_OK )
			{
				memset(pdata,00,card_data_len);
				mfrc500_init();
				rf_set_card_status(1);
				return;
			}
			else
			{
				if(rID.data_xor != XOR_Cal(rID.uid,card_data_len-3))
				{
					uint8_t i;
					DEBUG_CARD_DATA_LOG("NDEF_DataRead :");
					pdata = rID.uid;
					for(i=0;i<card_data_len-3;i++)
						DEBUG_CARD_DATA_LOG("%02x ",pdata[i]);
					DEBUG_CARD_DATA_LOG("\r\n");
					memset(pdata,00,card_data_len);
					DEBUG_CARD_DATA_LOG("NDEF_DataRead XOR ERROR!\r\n");
					mfrc500_init();
					rf_set_card_status(1);
					return;
				}
				else
				{
					uint8_t i;
					DEBUG_CARD_DATA_LOG("NDEF_DataRead :");
					for(i=0;i<card_data_len;i++)
						DEBUG_CARD_DATA_LOG("%02x ",pdata[i]);
					DEBUG_CARD_DATA_LOG("\r\n");
					//memset(NDEF_DataRead,00,28);
					DEBUG_CARD_DATA_LOG("NDEF_DataRead Clear!\r\n");
				}
			}

			is_white_list_uid = search_uid_in_white_list(g_cSNR+4,&read_uid_pos);
			if(is_white_list_uid == OPERATION_ERR)
			{
				read_uid_pos = 0xFF;
			}
			card_message_err = 1;
			wtrte_flash_ok = 1;
			#ifdef SHOW_CARD_PROCESS_TIME
			EndTime = PowerOnTime - StartTime;
			printf("UseTime:ReadNDEFfile = %d \r\n",EndTime);
			#endif
			rf_set_card_status(3);
		}

		/* ���ָ�� */
		if( wl.match_status == ON )
		{
			uint8_t *rpdata = (uint8_t *)&rID;
			uint8_t *wpdata = (uint8_t *)&wID;
			is_white_list_uid = add_uid_to_white_list(g_cSNR+4,&write_uid_pos);

			if(is_white_list_uid != OPERATION_ERR)
			{
				uint8_t card_data_len = sizeof(rf_id_typedf);
				card_message_err  = 1;
				wID.len_h  = 0;
				wID.len_l = card_data_len;
				memcpy(wID.uid,revicer.uid,4);
				memcpy(&(wID.rf_conf),&clicker_set,sizeof(rf_config_typedef));
				wID.upos  = write_uid_pos;
				memset(wID.rev,0xFF,1);
				
				/* ����д�����ݼ�� */
				status = ReadNDEFfile(rpdata, &NDEF_Len);
				DEBUG_CARD_DEBUG_LOG("ReadNDEFfile0 status = %d\r\n",status);
				#ifdef SHOW_card_task_TIME
				EndTime = PowerOnTime - StartTime;
				printf("UseTime:ReadNDEFfile0 = %d \r\n",EndTime);
				#endif
				if( status != MI_OK )
				{
					memset(rpdata,0x00,card_data_len);
					memset(wpdata,0x00,card_data_len);
					mfrc500_init();
					rf_set_card_status(1);
					return;
				}
				else
				{
					uint8_t i;
					uint8_t write_flag = 0;
					DEBUG_CARD_DATA_LOG("NDEF_DataRead :");
					for(i=0;i<card_data_len;i++)
						DEBUG_CARD_DATA_LOG("%02x ",rpdata[i]);
					DEBUG_CARD_DATA_LOG("\r\n");

					if( card_task.cmd_type == 0x28 )
					{
						memcpy(wID.stdid,card_task.stdid,10);
					}
					if( card_task.cmd_type == 0x41 )
					{
						memset(wID.stdid, 0x00, 10);
					}
					wID.data_xor = XOR_Cal(wID.uid,card_data_len-3);
					DEBUG_CARD_DATA_LOG("NDEF_DataWrite :");
					for(i=0;i<card_data_len;i++)
						DEBUG_CARD_DATA_LOG("%02x ",wpdata[i]);
					DEBUG_CARD_DATA_LOG("\r\n");
					/* ��ȫ�Ƚϣ�����������У����ɺ�*/
					for(i=0;i<card_data_len;i++)
					{
						if(rpdata[i] != wpdata[i])
							write_flag = 1;
					}

					if(write_flag != 1)
					{
						/* ����д��������,ֱ�ӽ�����һ���� */
						card_message_err = 1;
						wtrte_flash_ok = 1;
						rf_set_card_status(3);
						return;
					}
				}

				#ifdef SHOW_card_task_TIME
				EndTime = PowerOnTime - StartTime;
				printf("UseTime:WriteNDEFfile0 = %d \r\n",EndTime);
				#endif
				status = WriteNDEFfile(wpdata);
				DEBUG_CARD_DEBUG_LOG("WriteNDEFfile1 status = %d\r\n",status);
				#ifdef SHOW_CARD_PROCESS_TIME
				EndTime = PowerOnTime - StartTime;
				printf("UseTime:WriteNDEFfile1 = %d \r\n",EndTime);
				#endif
				if( status != MI_OK )
				{
					memset(wpdata,0x00,card_data_len);
					mfrc500_init();
					rf_set_card_status(1);
					return;
				}
				else
				{
					uint8_t i;
					uint8_t write_flg = 0;
					DEBUG_CARD_DATA_LOG("NDEF_DataWrite:");
					for(i=0;i<card_data_len;i++)
						DEBUG_CARD_DATA_LOG("%02x ",wpdata[i]);
				}

				status = ReadNDEFfile(rpdata, &NDEF_Len);
				DEBUG_CARD_DEBUG_LOG("ReadNDEFfile status = %d\r\n",status);
				#ifdef SHOW_CARD_PROCESS_TIME
				EndTime = PowerOnTime - StartTime;
				printf("UseTime:ReadNDEFfile = %d \r\n",EndTime);
				#endif
				if( status != MI_OK )
				{
					memset(rpdata,0x00,card_data_len);
					memset(wpdata,0x00,card_data_len);
					mfrc500_init();
					rf_set_card_status(1);
					return;
				}
				else
				{
					uint8_t i;
					DEBUG_CARD_DATA_LOG("NDEF_DataRead :");
					for(i=0;i<card_data_len;i++)
						DEBUG_CARD_DATA_LOG("%02x ",rpdata[i]);
					DEBUG_CARD_DATA_LOG("\r\n");
					if(rID.data_xor != XOR_Cal(rID.uid,card_data_len-3))
					{
						memset(rpdata,0x00,card_data_len);
						memset(wpdata,0x00,card_data_len);
						mfrc500_init();
						rf_set_card_status(1);
						return;
					}
				}

				status = SendInterrupt();
				DEBUG_CARD_DEBUG_LOG("SendInterrupt status = %d\r\n",status);
				#ifdef SHOW_CARD_PROCESS_TIME
				EndTime = PowerOnTime - StartTime;
				printf("UseTime:SendInterrupt = %d \r\n",EndTime);
				#endif
				if( status != MI_OK )
				{
					memset(rpdata,0x00,card_data_len);
					memset(wpdata,0x00,card_data_len);
					mfrc500_init();
					rf_set_card_status(1);
					return;
				}
				wtrte_flash_ok = 1;
			}
			else
			{
				card_message_err = 2;
				wtrte_flash_ok = 1;
			}
			rf_set_card_status(3);
		}
	}

	if( card_current_status == 3 )
	{
		if(wtrte_flash_ok == 1)
		{
			if( card_message_err != 2 )
			{
				#ifdef OPEN_SILENT_MODE
				ledOn(LBLUE);
				#else
				BEEP_EN();
				#endif
				Deselect();
				PcdHalt();
				PcdAntennaOff();
			}
		}

		if( card_message_err == 1 )
		{
			card_message.HEADER = 0x5C;
			switch(card_task.cmd_type)
			{
				case 0x25: card_message.TYPE   = 0x26; break;
				case 0x28: card_message.TYPE   = 0x29; break;
				case 0x41: card_message.TYPE   = 0x42; break;
				default:                               break;
			}
			memcpy(card_message.SIGN,card_task.srcid,4);
			card_message.LEN     = 25;
			memset(card_message.DATA,0x00,25);
			if( wl.attendance_sttaus == ON )
			{
				card_message.DATA[0] = read_uid_pos;
			}
			if( wl.match_status == ON )
			{
				card_message.DATA[0] = write_uid_pos;
			}
			memcpy(card_message.DATA+1,g_cSNR+4,4);
			{
				uint8_t j=0;
				uint8_t *pdata = card_message.DATA+5;
				for(j=0;j<10;j++)
				{
					pdata[2*j] = (rID.stdid[j] & 0xF0) >> 4;
					pdata[2*j + 1] = (rID.stdid[j] & 0x0F) ;
				}
			}
			card_message.XOR = XOR_Cal(&card_message.TYPE,31);
			card_message.END  = 0xCA;	
		}
		if( card_message_err == 2 )
		{
			memcpy(card_message.SIGN,card_task.srcid,4);
			App_returnErr(&card_message,card_task.cmd_type,0xFD);
		}

		if(card_message_err != 0)
		{
			if( wtrte_flash_ok == 1 )
			{
				if(BUFFERFULL != buffer_get_buffer_status(SEND_RINGBUFFER))
				{
					#ifndef OPEN_CARD_DATA_SHOW 
					serial_ringbuffer_write_data(SEND_RINGBUFFER,&card_message);
					DEBUG_CARD_DATA_LOG("NDEF_DataRead and NDEF_DataWrite Clear!\r\n");
					#endif
				}
			}
		}
		rf_set_card_status(4);
	}

	if( card_current_status == 5 )
	{
		uint8_t *rpdata = (uint8_t *)&rID;
		uint8_t *wpdata = (uint8_t *)&wID;
		#ifdef OPEN_SILENT_MODE
		ledOff(LBLUE);
		#else
		BEEP_DISEN();
		#endif
		rf_set_card_status(1);
		memset(rpdata,0x00,sizeof(rf_id_typedf));
		memset(wpdata,0x00,sizeof(rf_id_typedf));
		if( card_task.cmd_type == 0x28 )
		{
			memset(wID.stdid,0x00,10);
			wl.match_status = OFF;
			rf_set_card_status(0);
		}
		find_card_ok = 1;
		#ifdef SHOW_card_task_TIME
		EndTime = PowerOnTime - StartTime;
		printf("UseTime:SecondFindStart = %d \r\n",EndTime);
		#endif
	}
}
/******************************************************************************
  Function:systick_timer_init
  Description:
  Input :
  Return:
  Others:None
******************************************************************************/
void card_timer_init( void )
{
	sw_create_timer(&card_buzzer_timer    , 150, 4, 5,&(card_process_status), NULL);
	sw_create_timer(&card_second_find_timer,100, 1, 2,&(find_card_ok), NULL);
}
