/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stdio.h"
#include "app_send_data_process.h"
#include "app_show_message_process.h"

#define CLICKER_SNED_DATA_STATUS_TYPE     10
#define CLICKER_PRE_DATA_STATUS_TYPE      11

extern uint8_t spi_data_buffer[4][256];
extern uint8_t spi_data_write_index, spi_data_read_index, spi_data_count;
extern uint8_t spi_status_buffer[10][18];
extern uint8_t spi_status_write_index, spi_status_read_index, spi_status_count;

uint8_t clicker_send_data_status = 0;
static uint8_t pre_status = 0;
static uint8_t sum_clicker_count = 0;
uint32_t send_data_timer,retransmit_data_timer;

extern nrf_communication_t nrf_communication;
extern uint16_t list_tcb_table[10][8];

extern Uart_MessageTypeDef backup_massage;
extern uint8_t sign_buffer[4];
extern uint8_t sum_clicker_count;
/* ͳ�����ط�������ʹ�ñ��� */
// ����״̬����
#define LOST_INDEX        0
#define OK_INDEX          1

uint8_t rf_online_index[2];

// �ϱ����ط�
#define PRE_SUM_TABLE     0
#define PRE_ACK_TABLE     1
#define CUR_SUM_TABLE     2
#define CUR_ACK_TABLE     3

static uint8_t result_check_tables[2];
static uint8_t after_result_status;
static uint8_t retransmit_check_tables[4];;
static uint8_t after_retransmit_status;

static message_tcb_tydef    message_tcb ;
static retransmit_tcb_tydef retransmit_tcb;
static Uart_MessageTypeDef  revice_lost_massage,revice_ok_massage;
extern WhiteList_Typedef    wl;
extern Revicer_Typedef      revicer;

void retransmit_env_init( void );
/******************************************************************************
  Function:change_clicker_send_data_status
  Description:
		�޸�clicker_send_data_status��״̬
  Input :
		clicker_send_data_status: clicker_send_data_status״̬
  Output:
  Return:
  Others:None
******************************************************************************/
void rf_retransmit_set_status(uint8_t new_status)
{
	retransmit_tcb.status = new_status;
	//printf("rf_retransmit_set_status = %d \r\n",rf_retransmit_status);
}

/******************************************************************************
  Function:change_clicker_send_data_status
  Description:
		�޸�clicker_send_data_status��״̬
  Input :
		clicker_send_data_status: clicker_send_data_status״̬
  Output:
  Return:
  Others:None
******************************************************************************/
uint8_t get_rf_retransmit_status(void)
{
	return retransmit_tcb.status;
}


/******************************************************************************
  Function:change_clicker_send_data_status
  Description:
		�޸�clicker_send_data_status��״̬
  Input :
		clicker_send_data_status: clicker_send_data_status״̬
  Output:
  Return:
  Others:None
******************************************************************************/
void change_clicker_send_data_status( uint8_t newstatus )
{
	clicker_send_data_status = newstatus;
	spi_status_buffer[spi_status_write_index][0] = 0x61;
	memset(spi_status_buffer[spi_status_write_index]+1,0,10);
	spi_status_buffer[spi_status_write_index][11] = CLICKER_SNED_DATA_STATUS_TYPE;
	memset(spi_status_buffer[spi_status_write_index]+12,0,3);
	spi_status_buffer[spi_status_write_index][15] = CLICKER_SNED_DATA_STATUS_TYPE;//xor
	spi_status_buffer[spi_status_write_index][16] = 0x21;
	spi_status_buffer[spi_status_write_index][17] = newstatus;
	{
		#ifdef OPEN_SEND_STATUS_MESSAGE_SHOW
		uint8_t *str,status;
		status = spi_status_buffer[spi_status_write_index][spi_status_buffer[spi_status_write_index][14]+17];
		switch( status )
		{
			case SEND_IDLE_STATUS:            str = "IDLE_STATUS";            break;
			case SEND_DATA1_STATUS:           str = "DATA1_STATUS";           break;
			case SEND_DATA1_UPDATE_STATUS:    str = "DATA1_UPDATE_STATUS";    break;
			case SEND_DATA2_STATUS:           str = "DATA2_STATUS";           break;
			case SEND_DATA2_SEND_OVER_STATUS: str = "DATA2_SEND_OVER_STATUS"; break;
			case SEND_DATA2_UPDATE_STATUS:    str = "DATA2_UPDATE_STATUS";    break;
			case SEND_DATA3_STATUS:           str = "DATA3_STATUS";           break;
			case SEND_DATA3_SEND_OVER_STATUS: str = "DATA3_SEND_OVER_STATUS"; break;
			case SEND_DATA3_UPDATE_STATUS:    str = "DATA3_UPDATE_STATUS";    break;
			case SEND_DATA4_STATUS:           str = "DATA4_STATUS";           break;
			case SEND_DATA4_UPDATE_STATUS:    str = "DATA4_UPDATE_STATUS";    break;
			default:break;
		}
		b_print("send_status = %s\r\n",str);
		{
			int i;
			b_print("%4d %2d write: ", buffer_get_buffer_status(SPI_REVICE_BUFFER),spi_status_count+1);
			for(i=0;i<17;i++)
			{
				b_print("%2x ",spi_status_buffer[spi_status_write_index][i]);
			}
			b_print("%2x \r\n",status);
		}
		#endif
	}
	spi_status_write_index = (spi_status_write_index + 1)%10;
	spi_status_count++;
}

/******************************************************************************
  Function:get_clicker_send_data_status
  Description:
		��ȡclicker_send_data״̬
  Input :
		clicker_send_data: clicker_send_data����״̬
  Output:
  Return:
  Others:None
******************************************************************************/
uint8_t get_clicker_send_data_status( void )
{
	return clicker_send_data_status;
}

/******************************************************************************
  Function:get_clicker_send_data_status
  Description:
		��ȡclicker_send_data״̬
  Input :
		clicker_send_data: clicker_send_data����״̬
  Output:
  Return:
  Others:None
******************************************************************************/
uint8_t spi_buffer_status_check(uint8_t status)
{
	uint8_t current_status = status;

	if( current_status == 0 )
	{
		if(buffer_get_buffer_status(SPI_REVICE_BUFFER) == BUFFEREMPTY)
		{
			if((clicker_send_data_status != pre_status) && (clicker_send_data_status != 0))
			{
				pre_status = clicker_send_data_status;
			}
		}
		switch( pre_status )
		{
			case SEND_DATA1_STATUS          : current_status = SEND_DATA1_STATUS;          break;
			case SEND_DATA2_STATUS          : current_status = SEND_DATA2_STATUS;          break;
			case SEND_DATA2_SEND_OVER_STATUS: current_status = SEND_DATA2_SEND_OVER_STATUS;break;
			case SEND_DATA3_STATUS          : current_status = SEND_DATA3_STATUS;          break;
			case SEND_DATA3_SEND_OVER_STATUS: current_status = SEND_DATA3_SEND_OVER_STATUS;break;
			case SEND_DATA4_STATUS          : current_status = SEND_DATA4_STATUS;          break;
			default:  break;
		}
	}

	pre_status = current_status;

	return current_status;
}

/******************************************************************************
  Function:clear_uid_check_table
  Description:
		�������״̬������
  Input :
		clicker_num������������
  Output:
  Return:
  Others:None
******************************************************************************/
void clear_uid_check_table( void )
{
	clear_white_list_table(2);
	clear_white_list_table(3);
	clear_white_list_table(4);
	clear_white_list_table(5);
	clear_white_list_table(6);
	clear_white_list_table(7);
	clear_white_list_table(8);
	clear_white_list_table(9);
	DEBUG_STATISTICS_LOG("\r\nSum count:%d\r\n",sum_clicker_count);
	sum_clicker_count = 0;
}

/******************************************************************************
  Function:whitelist_checktable_or
  Description:
		����������״̬�����,�ۼ�����״̬
  Input :
		table1��֮ǰ״̬��1
		table2��֮ǰ״̬��2
  Output:
		table3���ۼ�֮���״̬��
  Return:
  Others:None
******************************************************************************/
void whitelist_checktable_or(uint8_t table1, uint8_t table2)
{
	uint8_t i = 0;
	for(i=0;i<8;i++)
	{
		list_tcb_table[table2][i] = list_tcb_table[table1][i] |
			list_tcb_table[table2][i];
	}
}

/******************************************************************************
  Function:clicker_send_data_statistics
  Description:
		App RF ͳ�ƺ���
  Input :
  Return:
  Others:None
******************************************************************************/
void clicker_send_data_statistics( uint8_t send_data_status, uint8_t uidpos )
{
	switch(send_data_status)
	{
		case SEND_DATA1_STATUS :          set_index_of_white_list_pos(SEND_DATA1_ACK_TABLE,uidpos); break;
		case SEND_DATA2_STATUS :
		case SEND_DATA2_SEND_OVER_STATUS: set_index_of_white_list_pos(SEND_DATA2_ACK_TABLE,uidpos); break;
		case SEND_DATA3_STATUS :
		case SEND_DATA3_SEND_OVER_STATUS: set_index_of_white_list_pos(SEND_DATA3_ACK_TABLE,uidpos); break;
		case SEND_DATA4_STATUS :          set_index_of_white_list_pos(SEND_DATA4_ACK_TABLE,uidpos); break;
		default:break;
	}
}

/******************************************************************************
  Function:spi_process_revice_data
  Description:
		RF SPI buffer ������
  Input :
  Return:
  Others:None
******************************************************************************/
uint8_t spi_process_revice_data( void )
{
	uint8_t spi_message[255];
	uint8_t spi_message_type = 0;
	bool    Is_whitelist_uid = OPERATION_ERR;
	uint8_t uidpos = 0;
	uint8_t clicker_send_data_status = 0;

	if(buffer_get_buffer_status(SPI_REVICE_BUFFER) != BUFFEREMPTY)
	{
		memset(spi_message,0,255);
		spi_read_data_from_buffer( SPI_REVICE_BUFFER, spi_message );
		clicker_send_data_status = spi_message[spi_message[14]+17];
		spi_message_type = spi_message[11];

		#ifdef OPEN_BUFFER_DATA_SHOW
		{
			int i;
			printf("%4d ", buffer_get_buffer_status(SPI_REVICE_BUFFER));
			printf("Buffer Read :");
			for(i=0;i<spi_message[14]+18;i++)
				printf("%2x ",spi_message[i]);
		}
		#endif

		/* �ж��Ƿ�Ϊ״̬֡ */
		if(spi_message_type != 0x0A)
		{
			/* ���������� */
			Is_whitelist_uid = search_uid_in_white_list(spi_message+5,&uidpos);

			if(wl.uids[uidpos].use == 0)
			{
				wl.uids[uidpos].use = 1;
				wl.uids[uidpos].firstrev = 1;
			}
			else
			{
				wl.uids[uidpos].firstrev = 0;
			}

			/* ����ǰ����� */
			if(Is_whitelist_uid == OPERATION_SUCCESS)
			{
				uint8_t systick_current_status = 0;

				/* ��ȡ������״̬ */
				systick_current_status = rf_get_systick_status();

				/* ��������� */
				if(systick_current_status == 1)
				{
					set_index_of_white_list_pos(1,uidpos);
				}

				/* ͳ�Ʒ���״̬ */
				clicker_send_data_statistics( clicker_send_data_status, uidpos );
			}

			if(1 == get_rf_retransmit_status())
			{
//				if(spi_message[5] == retransmit_tcb.uid[0] &&
//					 spi_message[6] == retransmit_tcb.uid[1]
//					)
//				{
//					rf_retransmit_set_status(2);
//				}
			}

			/* ����������״̬ */
			if(wl.switch_status == OFF)
			{
				/* �رհ������ǲ����˰����� */
				Is_whitelist_uid = OPERATION_SUCCESS;
			}

			/* ����ͨ�� */
			if(Is_whitelist_uid == OPERATION_SUCCESS)
			{
				/* get uid */
				memcpy(sign_buffer   ,spi_message+5 ,4);

				/* �յ�����Data */
				if(spi_message[11] == NRF_DATA_IS_USEFUL)
				{
					/* ����ACK�İ��ź��ϴη��͵��Ƿ���ͬ */
					if(spi_message[10] != revicer.sen_num)//�յ�������Ч����
					{
						uint8_t temp;
						DEBUG_BUFFER_DTATA_LOG("[DATA] uid:%02x%02x%02x%02x, ",
							*(nrf_communication.receive_buf+5),*(nrf_communication.receive_buf+6),
							*(nrf_communication.receive_buf+7),*(nrf_communication.receive_buf+8));
						DEBUG_BUFFER_DTATA_LOG("seq:%2x, pac:%2x\r\n",(uint8_t)*(nrf_communication.receive_buf+9),
							(uint8_t)*(nrf_communication.receive_buf+10));

						/* ��Ч���ݸ��Ƶ����� */
						//rf_move_data_to_buffer(&nrf_communication);
						/* ���½�������֡������� */
						wl.uids[uidpos].rev_seq = spi_message[9];
						wl.uids[uidpos].rev_num = spi_message[10];
						/* �ظ�ACK */
						nrf_transmit_start(&temp,0,NRF_DATA_IS_ACK, 1, 0, SEND_DATA_ACK_TABLE);
						/* �û����յ����ݴ����� */
						my_nrf_receive_success_handler();
					}
				}
				/* �յ�����Ack */
				else if(spi_message[11] == NRF_DATA_IS_ACK)
				{
					DEBUG_BUFFER_DTATA_LOG("[ACK] uid:%02x%02x%02x%02x, ",
						*(nrf_communication.receive_buf+5),*(nrf_communication.receive_buf+6),
						*(nrf_communication.receive_buf+7),*(nrf_communication.receive_buf+8));
					DEBUG_BUFFER_DTATA_LOG("seq:%2x, pac:%2x\r\n",(uint8_t)*(nrf_communication.receive_buf+9),
						(uint8_t)*(nrf_communication.receive_buf+10));
					/* �ظ����յ����ݣ����ذ��ź��ϴ�һ����*/
					if(wl.uids[uidpos].rev_num != spi_message[10])
					{
						/* ͳ�ƶ��� */
						if( wl.uids[uidpos].use == 1 )
						{
							if(wl.uids[uidpos].firstrev == 0)
							{
								if( spi_message[10] > wl.uids[uidpos].rev_num)
									wl.uids[uidpos].lost_package_num += spi_message[10] - wl.uids[uidpos].rev_num -1 ;

								if( spi_message[10] < wl.uids[uidpos].rev_num )
									wl.uids[uidpos].lost_package_num += spi_message[10] + 255 - wl.uids[uidpos].rev_num ;
							}
							else
							{
								wl.uids[uidpos].lost_package_num = 0;
							}

//							/* ͳ���յ����� */
//						clickers[uidpos].revice_package_num++;
//						b_print("clickers : %02x%02x%02x%02x, pre:%2x, cur:%2x revice = %08x, lost = %08x, \r\n",
//						clickers[uidpos].uid[0],clickers[uidpos].uid[1],clickers[uidpos].uid[2],
//						clickers[uidpos].uid[3],
//						clickers[uidpos].prepacknum,
//						nrf_communication.receive_buf[10],
//						clickers[uidpos].revice_package_num,clickers[uidpos].lost_package_num
//						);
							wl.uids[uidpos].rev_num = spi_message[10];
						}
					}
				}
				else
				{
					DEBUG_BUFFER_DTATA_LOG("[STATUS]:%d \r\n",spi_message[spi_message[15]+18]);
				}
			}
		}
		else
		{
			#ifdef OPEN_SEND_STATUS_SHOW
			int i;
			b_print("%4d    read2: ", buffer_get_buffer_status(SPI_REVICE_BUFFER));
			for(i=0;i<17;i++)
			{
				b_print("%2x ",spi_message[i]);
			}
			b_print("%2x \r\n",spi_message[17]);
			#endif
		}
	}
	else
	{
		// ok to update to pc
	}
	return (clicker_send_data_status);
}

/******************************************************************************
  Function:checkout_outline_uid
  Description:
		��ȡ������״̬�Ĵ�����UID
  Input :
  Return:
  Others:None
******************************************************************************/
bool checkout_online_uids(uint8_t src_table, uint8_t check_table,
	                        uint8_t mode, uint8_t *buffer,uint8_t *len)
{
	uint8_t i;
	uint8_t is_use_pos = 0,is_online_pos = 0;
#ifdef SEND_DATA_DETAIL_MESSAGE_SHOW
	uint8_t index = 0;
#endif
	for(i=rf_online_index[mode];(i<120)&&(*len<240);i++)
	{
		is_use_pos = get_index_of_white_list_pos_status(src_table,i);
		if(is_use_pos == 1)
		{
			is_online_pos = get_index_of_white_list_pos_status(check_table,i);
			if(is_online_pos == mode)
			{
				get_index_of_uid(i,buffer);
#ifdef SEND_DATA_DETAIL_MESSAGE_SHOW
				{
					DEBUG_UID_LOG("[%3d]:%02x%02x%02x%02x ",i,*buffer, *(buffer+1),*(buffer+2), *(buffer+3));
					if(((index++)+1) % 5 == 0)
					{
						DEBUG_UID_LOG("\n");
					}
				}
#endif
				buffer = buffer+4;
				*len = *len + 4;
			}
		}
	}

	if(i==120)
	{
		rf_online_index[mode] = 0;
		return 0;
	}
	else
	{
		rf_online_index[mode] = i;
		return 1;
	}
}

void get_send_data_table_message(uint8_t status)
{
	switch( status )
	{
		case SEND_DATA1_UPDATE_STATUS:
			{
				DEBUG_STATISTICS_LOG("Statistic : %d\r\n",revicer.data_statistic_count++);
				DEBUG_STATISTICS_LOG("First Statistic:");
				result_check_tables[PRE_SUM_TABLE] = SEND_DATA1_SUM_TABLE;
				result_check_tables[PRE_ACK_TABLE] = SEND_DATA1_ACK_TABLE;
				after_result_status = SEND_DATA2_STATUS;
			}
			break;

		case SEND_DATA2_UPDATE_STATUS:
			{
				DEBUG_STATISTICS_LOG("\r\nSecond Statistic:");
				result_check_tables[PRE_SUM_TABLE] = SEND_DATA2_SUM_TABLE;
				result_check_tables[PRE_ACK_TABLE] = SEND_DATA2_ACK_TABLE;
				after_result_status = SEND_DATA3_STATUS;
			}
			break;
		case SEND_DATA3_UPDATE_STATUS:
			{
				DEBUG_STATISTICS_LOG("\r\nThird Statistic:");
				result_check_tables[PRE_SUM_TABLE] = SEND_DATA3_SUM_TABLE;
				result_check_tables[PRE_ACK_TABLE] = SEND_DATA3_ACK_TABLE;
				after_result_status = SEND_DATA4_STATUS;
			}
			break;
		case SEND_DATA4_UPDATE_STATUS:
			{
				DEBUG_STATISTICS_LOG("\r\nFourth Statistic:");
				result_check_tables[PRE_SUM_TABLE] = SEND_DATA4_SUM_TABLE;
				result_check_tables[PRE_ACK_TABLE] = SEND_DATA4_ACK_TABLE;
				after_result_status = SEND_IDLE_STATUS;
			}
			break;
		default:
			{
				result_check_tables[PRE_SUM_TABLE] = 0;
				result_check_tables[PRE_ACK_TABLE] = 0;
				after_result_status = 0;
			}
			break;
	}
}

void get_retransmit_messsage( uint8_t status )
{
	switch( status )
	{
		case SEND_DATA2_STATUS:
			{
				DEBUG_DATA_DETAIL_LOG("\r\n\r\n[1].retransmit:\r\n");
				retransmit_check_tables[PRE_SUM_TABLE] = SEND_DATA1_SUM_TABLE;
				retransmit_check_tables[PRE_ACK_TABLE] = SEND_DATA1_ACK_TABLE;
				retransmit_check_tables[CUR_SUM_TABLE] = SEND_DATA2_SUM_TABLE;
				retransmit_check_tables[CUR_ACK_TABLE] = SEND_DATA2_ACK_TABLE;
				after_retransmit_status                = SEND_DATA2_SEND_OVER_STATUS;
			}
			break;

		case SEND_DATA3_STATUS:
			{
				DEBUG_DATA_DETAIL_LOG("\r\n\r\n[2].retransmit:\r\n");
				retransmit_check_tables[PRE_SUM_TABLE] = SEND_DATA2_SUM_TABLE;
				retransmit_check_tables[PRE_ACK_TABLE] = SEND_DATA2_ACK_TABLE;
				retransmit_check_tables[CUR_SUM_TABLE] = SEND_DATA3_SUM_TABLE;
				retransmit_check_tables[CUR_ACK_TABLE] = SEND_DATA3_ACK_TABLE;
				after_retransmit_status                = SEND_DATA3_SEND_OVER_STATUS;
			}
			break;

		default:
			{
				retransmit_check_tables[PRE_SUM_TABLE] = 0;
				retransmit_check_tables[PRE_ACK_TABLE] = 0;
				retransmit_check_tables[CUR_SUM_TABLE] = 0;
				retransmit_check_tables[CUR_ACK_TABLE] = 0;
				after_retransmit_status                = 0;
			}
			break;
	}
}

/******************************************************************************
  Function:checkout_retransmit_clickers
  Description:
		�����������ط�
  Input :
		presumtable  ���ܵ�������
		preacktable  ���Ѿ����ߣ������ط���������
		cursumtable  ���´�ͳ�Ƶ�������
  Return:
  Others:None
******************************************************************************/
uint8_t checkout_retransmit_clickers(uint8_t presumtable, uint8_t preacktable, uint8_t cursumtable)
{
	uint8_t i;
	uint8_t is_use_pos = 0,is_online_pos = 0;
	uint8_t puid[4];
	uint8_t clickernum = 0;
#ifdef SEND_DATA_DETAIL_MESSAGE_SHOW
	uint8_t index = 0;
#endif
	for(i=0;i<120;i++)
	{
		is_use_pos = get_index_of_white_list_pos_status(presumtable,i);
		if(is_use_pos == 1)
		{
			is_online_pos = get_index_of_white_list_pos_status(preacktable,i);
			if(is_online_pos == 0)
			{
				get_index_of_uid(i,puid);
				set_index_of_white_list_pos(cursumtable,i);
				clickernum++;
#ifdef SEND_DATA_DETAIL_MESSAGE_SHOW
				{
					DEBUG_UID_LOG("[%3d]:%02x%02x%02x%02x ",i,puid[0],puid[1],puid[2],puid[3]);
					if(((index++)+1) % 5 == 0)
						DEBUG_UID_LOG("\n");
				}
#endif
			}
		}
	}
#ifdef SEND_DATA_DETAIL_MESSAGE_SHOW
	DEBUG_UID_LOG("\n");
#endif
	return clickernum;
}

/* �ط����� */
void retansmit_data( uint8_t status )
{
	if(( status == SEND_DATA2_STATUS ) || ( status == SEND_DATA3_STATUS ))
	{
		get_retransmit_messsage( status );

		checkout_retransmit_clickers( retransmit_check_tables[PRE_SUM_TABLE] ,retransmit_check_tables[PRE_ACK_TABLE],
		                     retransmit_check_tables[CUR_SUM_TABLE] );
		/* ����ǰ��֡ */
		memset(nrf_communication.dtq_uid, 0, 4);
		nrf_transmit_start( nrf_communication.dtq_uid, 0, NRF_DATA_IS_PRE, SEND_PRE_COUNT,
		                    SEND_PRE_DELAY100US, retransmit_check_tables[CUR_SUM_TABLE]);
		/* ��������֡ */
		memset(nrf_communication.dtq_uid, 0, 4);

		whitelist_checktable_or(retransmit_check_tables[PRE_ACK_TABLE],SEND_DATA_ACK_TABLE);

		nrf_transmit_start( rf_var.tx_buf, rf_var.tx_len, NRF_DATA_IS_USEFUL,
		                    SEND_DATA_COUNT, SEND_DATA_DELAY100US, SEND_DATA_ACK_TABLE );

		/* ����״̬����ʼ2��ͳ�� */
		change_clicker_send_data_status( after_retransmit_status );

		if(after_retransmit_status == SEND_DATA3_SEND_OVER_STATUS)
		{
			retransmit_env_init();
		}
	}
}

/* ͳ�ƺ��� */
void send_data_result( uint8_t status )
{
	if(( status == SEND_DATA1_UPDATE_STATUS ) ||
		 ( status == SEND_DATA2_UPDATE_STATUS ) ||
		 ( status == SEND_DATA3_UPDATE_STATUS ) ||
	   ( status == SEND_DATA4_UPDATE_STATUS ))
	{

		get_send_data_table_message(status);

		DEBUG_DATA_DETAIL_LOG("\r\nlost:\r\n");

		/* ����ʧ�ܵ�UID */
		while( message_tcb.Is_lost_over != 0)
		{
			message_tcb.Is_lost_over = checkout_online_uids( result_check_tables[PRE_SUM_TABLE],result_check_tables[PRE_ACK_TABLE], 0,
				revice_lost_massage.DATA,&(revice_lost_massage.LEN));
			message_tcb.lostuidlen += revice_lost_massage.LEN;

#ifdef ENABLE_SEND_DATA_TO_PC
			revice_lost_massage.HEADER = 0x5C;
			memset(revice_lost_massage.SIGN,0,4);
			revice_lost_massage.TYPE = 0x30;
			revice_lost_massage.XOR = XOR_Cal((uint8_t *)(&(revice_lost_massage.TYPE)), revice_lost_massage.LEN+6);
			revice_lost_massage.END = 0xCA;
			if(revice_lost_massage.LEN != 0)
			{
				if(BUFFERFULL != buffer_get_buffer_status(SEND_RINGBUFFER))
				{
					serial_ringbuffer_write_data(SEND_RINGBUFFER,&revice_lost_massage);
				}
			}
#endif
			memset(revice_lost_massage.DATA,0,revice_lost_massage.LEN);
			revice_lost_massage.LEN = 0;
		}
		DEBUG_DATA_DETAIL_LOG("\r\nok:\r\n");
		message_tcb.clicker_count = 0;
		while(message_tcb.Is_ok_over != 0)
		{
			message_tcb.Is_ok_over = checkout_online_uids( result_check_tables[PRE_SUM_TABLE],result_check_tables[PRE_ACK_TABLE], 1,
				revice_ok_massage.DATA,&(revice_ok_massage.LEN));
			revice_ok_massage.XOR =  XOR_Cal((uint8_t *)(&(revice_ok_massage.TYPE)),
			                                 revice_ok_massage.LEN+6);
			revice_ok_massage.END = 0xCA;
			message_tcb.clicker_count += revice_ok_massage.LEN/4;
			message_tcb.okuidlen += revice_ok_massage.LEN;

#ifdef ENABLE_SEND_DATA_TO_PC
			revice_ok_massage.HEADER = 0x5C;
			memset(revice_ok_massage.SIGN,0,4);
			revice_ok_massage.TYPE = 0x31;
			revice_ok_massage.XOR = XOR_Cal((uint8_t *)(&(revice_ok_massage.TYPE)), revice_ok_massage.LEN+6);
			revice_ok_massage.END = 0xCA;
			if( revice_ok_massage.LEN != 0)
			{
				if(BUFFERFULL != buffer_get_buffer_status(SEND_RINGBUFFER))
				{
					serial_ringbuffer_write_data(SEND_RINGBUFFER,&revice_ok_massage);
				}
			}
#endif
			memset(revice_lost_massage.DATA,0,revice_lost_massage.LEN);
			revice_ok_massage.LEN = 0;
		}
		DEBUG_STATISTICS_LOG("\r\ncount:%d\r\n",message_tcb.clicker_count);
		sum_clicker_count += message_tcb.clicker_count;
		message_tcb.clicker_count = 0;

		/* �ϴ�����״̬ */
		if(message_tcb.lostuidlen != 0)
		{
			if( status == SEND_DATA3_UPDATE_STATUS )
			{
				uint8_t retransmit_clickers;
				DEBUG_DATA_DETAIL_LOG("\r\n\r\n[3].retransmit:\r\n");
				retransmit_clickers = checkout_retransmit_clickers(SEND_DATA3_SUM_TABLE,SEND_DATA3_ACK_TABLE,
																			SEND_DATA4_SUM_TABLE);
				if(retransmit_clickers > 0)
				{
					retransmit_tcb.sum = 3;
				}
				else
				{
					retransmit_tcb.sum = 0;
				}
				whitelist_checktable_or(SEND_DATA3_ACK_TABLE,SEND_DATA_ACK_TABLE);
			}

			change_clicker_send_data_status( after_result_status );
			if(after_result_status == SEND_IDLE_STATUS)
				clear_uid_check_table();
		}
		else
		{
			change_clicker_send_data_status(0);
			clear_uid_check_table();
		}
		message_tcb.okuidlen     = 0;
		message_tcb.lostuidlen   = 0;
		message_tcb.Is_lost_over = 1;
		message_tcb.Is_ok_over   = 1;
	}
}

/******************************************************************************
  Function:retransmit_data_to_next_clicker
  Description:
		�������ط�����
  Input :
  Return:
  Others:None
******************************************************************************/
void retransmit_data_to_next_clicker( void )
{
	nrf_transmit_start(rf_var.tx_buf,0,NRF_DATA_IS_PRE,SEND_PRE_COUNT,
	                   SEND_PRE_DELAY100US,SEND_DATA4_SUM_TABLE);

	whitelist_checktable_or(SEND_DATA4_ACK_TABLE,SEND_DATA_ACK_TABLE);

	nrf_transmit_start(rf_var.tx_buf,rf_var.tx_len,NRF_DATA_IS_USEFUL,SEND_DATA_COUNT,
	                   SEND_DATA_DELAY100US,SEND_DATA_ACK_TABLE);

	rf_retransmit_set_status(1);
}

/******************************************************************************
  Function:retransmit_env_init
  Description:
		����ط������еĻ�������
  Input :
  Return:
  Others:None
******************************************************************************/
void retransmit_env_init( void )
{
	retransmit_tcb.count  = 0;
	retransmit_tcb.sum    = 0;
	retransmit_tcb.pos    = 0;
	retransmit_tcb.status = 0;
	memset(retransmit_tcb.uid,0,4);
	clear_current_uid_index();
}

/******************************************************************************
  Function:spi_write_temp_buffer_to_buffer
  Description:
		����ʱ��������ݴ��뵽buffer��
  Input :
  Return:
  Others:None
******************************************************************************/
void spi_write_temp_buffer_to_buffer()
{
	if(BUFFERFULL != buffer_get_buffer_status(SPI_REVICE_BUFFER))
	{
		if(spi_data_count > 0)
		{
			spi_write_data_to_buffer(SPI_REVICE_BUFFER,spi_data_buffer[spi_data_read_index],
			    spi_data_buffer[spi_data_read_index][spi_data_buffer[spi_data_read_index][14]+17]);
			spi_data_read_index = (spi_data_read_index + 1) % 4;
			spi_data_count--;
		}
	}

	if(BUFFERFULL != buffer_get_buffer_status(SPI_REVICE_BUFFER))
	{
		if((spi_status_count > 0) && (spi_data_count == 0))
		{
			spi_write_data_to_buffer(SPI_REVICE_BUFFER,spi_status_buffer[spi_status_read_index],
			    spi_status_buffer[spi_status_read_index][spi_status_buffer[spi_status_read_index][14]+17]);
			{
				#ifdef OPEN_SEND_STATUS_SHOW
				uint8_t *str,status;
				status = spi_status_buffer[spi_status_read_index][spi_status_buffer[spi_status_read_index][14]+17];
				switch( status )
				{
					case SEND_IDLE_STATUS:            str = "IDLE_STATUS";            break;
					case SEND_DATA1_STATUS:           str = "DATA1_STATUS";           break;
					case SEND_DATA1_UPDATE_STATUS:    str = "DATA1_UPDATE_STATUS";    break;
					case SEND_DATA2_STATUS:           str = "DATA2_STATUS";           break;
					case SEND_DATA2_SEND_OVER_STATUS: str = "DATA2_SEND_OVER_STATUS"; break;
					case SEND_DATA2_UPDATE_STATUS:    str = "DATA2_UPDATE_STATUS";    break;
					case SEND_DATA3_STATUS:           str = "DATA3_STATUS";           break;
					case SEND_DATA3_SEND_OVER_STATUS: str = "DATA3_SEND_OVER_STATUS"; break;
					case SEND_DATA3_UPDATE_STATUS:    str = "DATA3_UPDATE_STATUS";    break;
					case SEND_DATA4_STATUS:           str = "DATA4_STATUS";           break;
					case SEND_DATA4_UPDATE_STATUS:    str = "DATA4_UPDATE_STATUS";    break;
					default:break;
				}
				DEBUG_BUFFER_DTATA_LOG("send_status = %s\r\n",str);
				#endif
				#ifdef OPEN_SEND_STATUS_MESSAGE_SHOW
				{
					int i;
					DEBUG_BUFFER_DTATA_LOG("%4d %2d read1: ", buffer_get_buffer_status(SPI_REVICE_BUFFER),spi_status_count);
					for(i=0;i<17;i++)
					{
						DEBUG_BUFFER_DTATA_LOG("%2x ",spi_status_buffer[spi_status_read_index][i]);
					}
					DEBUG_BUFFER_DTATA_LOG("%2x \r\n",status);
				}
				#endif
			}
			spi_status_read_index = (spi_status_read_index + 1) % 10;
			spi_status_count--;
		}
	}
}
/******************************************************************************
  Function:App_clickers_send_data_process
  Description:
		���������ʹ����߼�����
  Input :
  Return:
  Others:None
******************************************************************************/
void App_clickers_send_data_process( void )
{
	uint8_t spi_buffer_status = 0;
	uint8_t current_status = 0;

	/* ��ȡspi����д�뵽 Buffer */
	spi_write_temp_buffer_to_buffer();

	/* ��ȡ����״̬ */
	spi_buffer_status = spi_process_revice_data();

	/* ������ǰ״̬ */
	current_status = spi_buffer_status_check(spi_buffer_status);

	/* �������ϱ�֮�����µ������� */
	if( current_status == SEND_DATA4_STATUS ) // 10
	{
		uint8_t rf_retransmit_status = 0;

		rf_retransmit_status = get_rf_retransmit_status();

		if(retransmit_tcb.sum != 0)
		{
			if(rf_retransmit_status == 0)
			{
				retransmit_data_to_next_clicker();
			}
		}

		if(rf_retransmit_status == 2)
		{
			retransmit_tcb.count++;
			DEBUG_DATA_DETAIL_LOG("retransmit_count = %d\r\n",retransmit_tcb.count );
			if(retransmit_tcb.count == retransmit_tcb.sum)
			{
				retransmit_tcb.count = 0;
				change_clicker_send_data_status( SEND_DATA4_UPDATE_STATUS ); // 11
				retransmit_env_init();
			}
			else
			{
				retransmit_data_to_next_clicker();
			}
		}
	}

	/* ��ӡͳ�ƽ�� */
	send_data_result( current_status );

	/* �ϱ�֮�����¹㲥���� */
  retansmit_data( current_status );
}

/******************************************************************************
  Function:send_data_env_init
  Description:
		���������ʹ����߼�����
  Input :
  Return:
  Others:None
******************************************************************************/
void send_data_env_init(void)
{
	message_tcb.okuidlen     = 0;
	message_tcb.lostuidlen   = 0;
	message_tcb.Is_lost_over = 1;
	message_tcb.Is_ok_over   = 1;

	memset(result_check_tables,0,2);
	after_result_status = 0;
	memset(retransmit_check_tables,0,4);
	after_retransmit_status = 0;

	memset(rf_online_index,0,2);

	/* clear retransmit task control block*/
	memset(&retransmit_tcb,0,9);

	/* clear online check table */
	memset(list_tcb_table[2],0,16*8);

	/* clear count of clicker */
	sum_clicker_count = 0;

	/* clear send data status */
	clicker_send_data_status = 0;

	/* clear last status of send status */
	pre_status = 0;
}
