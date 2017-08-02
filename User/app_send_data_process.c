/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "main.h"
#include "cJSON.h"
#include "app_send_data_process.h"
#include "app_show_message_process.h"

task_tcb_typedef send_data_task;
volatile send_data_process_tcb_tydef send_data_process_tcb;
static uint8_t send_data_status = 0;

extern nrf_communication_t nrf_data;
extern uint16_t list_tcb_table[UID_LIST_TABLE_SUM][WHITE_TABLE_LEN];

extern wl_typedef      wl;
extern revicer_typedef revicer;

void retransmit_env_init( void );

void set_send_data_status( uint8_t new_status )
{
	send_data_status = new_status;
//printf("send_data_status = %d\r\n",send_data_status);
}

uint8_t get_send_data_status( void )
{
	return send_data_status;
}
/******************************************************************************
  Function:whitelist_checktable_or
  Description:
		白名单在线状态表或函数,累加在线状态
  Input :
		table1：之前状态表1
		table2：之前状态表2
  Output:
		table3：累加之后的状态表
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
  Function:whitelist_checktable_and
  Description:
		白名单在线状态表与函数,剔除在线状态
  Input :
		table1：之前状态表1
		table2：之前状态表2
  Output:
		table：与操作之后的表格
  Return:
  Others:None
******************************************************************************/
void whitelist_checktable_and(uint8_t table1, uint8_t table2, uint8_t table)
{
	uint8_t i = 0;
	for(i=0;i<8;i++)
	{
		list_tcb_table[table][i] = list_tcb_table[table1][i] & (~list_tcb_table[table2][i]);
	}
}

/******************************************************************************
  Function:spi_process_revice_data
  Description:
		RF SPI buffer 处理函数
  Input :
  Return:
  Others:None
******************************************************************************/
void spi_process_revice_data( void )
{
	uint8_t  spi_message[255];
	uint8_t  spi_message_type = 0;
	uint8_t  Is_whitelist_uid = OPERATION_ERR;
	uint16_t uidpos = 0xFFFF;

	if(buffer_get_buffer_status(SPI_RBUF) == BUF_EMPTY)
		return;

	memset(spi_message,0,255);
	spi_read_data_from_buffer( SPI_RBUF, spi_message );
	spi_message_type = spi_message[13];

	#ifdef OPEN_BUFFER_DATA_SHOW
	{
		int i;
		DEBUG_BUFFER_ACK_LOG("%4d ", buffer_get_buffer_status(SPI_RBUF));
		DEBUG_BUFFER_ACK_LOG("Buffer Read :");
		for(i=5;i<9;i++)
			DEBUG_BUFFER_ACK_LOG("%02x",spi_message[i]);
	}
	#endif

	/* 检索白名单 */
	Is_whitelist_uid = search_uid_in_white_list(spi_message+5,&uidpos);

	if(Is_whitelist_uid != OPERATION_SUCCESS)
		return;

	/* 收到的是Data */
	if(spi_message_type == NRF_DATA_IS_USEFUL) 
	{
		DEBUG_BUFFER_DTATA_LOG("[DATA] uid:%02x%02x%02x%02x, ",\
			*(spi_message+5),*(spi_message+6),*(spi_message+7),*(spi_message+8));
		DEBUG_BUFFER_DTATA_LOG("seq:%2x, pac:%2x\r\n",(uint8_t)*(spi_message+11),\
			(uint8_t)*(spi_message+12));

		if( wl.start == ON )
		{
			/* 将数据送入PC的发送缓冲区 */
			if(BUF_FULL != buffer_get_buffer_status(SPI_RBUF))
			{
				nrf_transmit_parameter_t transmit_config;

				/* 回复ACK */
				memcpy(transmit_config.dist,spi_message+5, 4 );
				transmit_config.package_type   = NRF_DATA_IS_ACK;
				transmit_config.transmit_count = 2;
				transmit_config.delay100us     = 20;
				transmit_config.is_pac_add     = PACKAGE_NUM_SAM;
				transmit_config.data_buf       = NULL;
				transmit_config.data_len       = 0;
				nrf_transmit_start( &transmit_config );
				spi_write_data_to_buffer(UART_RBUF,spi_message);
			}
		}
	}
	/* 收到的是Ack */
	if(spi_message_type == NRF_DATA_IS_ACK)
	{
		uint8_t Is_reviceed_uid = get_index_of_white_list_pos_status(SEND_DATA_ACK_TABLE,uidpos);
		if( Is_reviceed_uid == 0)
		{
			DEBUG_BUFFER_DTATA_LOG("[NEW_ACK] upos[%03d] uid:%02x%02x%02x%02x, ",uidpos,\
				*(spi_message+5),*(spi_message+6),*(spi_message+7),*(spi_message+8));
			DEBUG_BUFFER_DTATA_LOG("seq:%2x, pac:%2x \r\n",(uint8_t)*(spi_message+11), \
				(uint8_t)*(spi_message+12));
		}
		else
		{
			DEBUG_BUFFER_DTATA_LOG("[OLD_ACK] upos[%03d] uid:%02x%02x%02x%02x, ",uidpos,\
				*(spi_message+5),*(spi_message+6),*(spi_message+7),*(spi_message+8));
			DEBUG_BUFFER_DTATA_LOG("seq:%2x, pac:%2x \r\n",(uint8_t)*(spi_message+11), \
				(uint8_t)*(spi_message+12));
		}

		if(( get_send_data_status() == 0 ) || ( Is_reviceed_uid != 0 ))
			return;

		clear_index_of_white_list_pos(SEND_PRE_TABLE,uidpos);
		set_index_of_white_list_pos(SEND_DATA_ACK_TABLE,uidpos);
	}
}

void App_retransmit_data( uint8_t is_new_pack, uint8_t is_new_logic_pack)
{
	nrf_transmit_parameter_t transmit_config;
	memset(transmit_config.dist,0, 4);
	transmit_config.package_type   = NRF_DATA_IS_PRE;
	transmit_config.transmit_count = SEND_PRE_COUNT;
	transmit_config.delay100us     = SEND_PRE_DELAY100US;
	transmit_config.is_pac_add     = is_new_pack;
	transmit_config.logic_pac_add  = is_new_logic_pack;
	transmit_config.data_buf       = NULL;
	transmit_config.data_len       = 0;
	transmit_config.sel_table      = SEND_PRE_TABLE;
	nrf_transmit_start( &transmit_config );
	
	/* 发送数据帧 */
	memset(transmit_config.dist,0, 4);
	transmit_config.package_type   = NRF_DATA_IS_USEFUL;
	transmit_config.transmit_count = SEND_DATA_COUNT;
	transmit_config.delay100us     = SEND_DATA_DELAY100US;
	transmit_config.is_pac_add     = PACKAGE_NUM_SAM;
	transmit_config.data_buf       = rf_var.tx_buf; 
	transmit_config.data_len       = rf_var.tx_len;
	transmit_config.sel_table      = SEND_DATA_ACK_TABLE;
	nrf_transmit_start( &transmit_config );
}
/******************************************************************************
  Function:App_clickers_send_data_process
  Description:
		答题器发送处理逻辑函数
  Input :
  Return:
  Others:None
******************************************************************************/
void App_clickers_send_data_process( void )
{
	spi_process_revice_data();
	
	if(( send_data_status == SEND_500MS_DATA_STATUS )|| 
		 ( send_data_status == SEND_2S_DATA_STATUS    ))
	{
		if( send_data_status == SEND_2S_DATA_STATUS    )
		{
			uint8_t status;
			static uint8_t err_count = 0;

			if(clicker_set.N_CH_RX == clicker_set.N_CH_TX )
				clicker_set.N_CH_RX = (clicker_set.N_CH_TX + 2) % 11;
			status  = spi_set_cpu_tx_signal_ch(clicker_set.N_CH_RX);
			status |= spi_set_cpu_rx_signal_ch(clicker_set.N_CH_TX);

			if( status != 0 )
			{
				nrf1_rst_init();
				nrf2_rst_init();
				NRF1_RST_LOW();	
				NRF2_RST_LOW();	
				DelayMs(50);
				NRF1_RST_HIGH();	
				NRF2_RST_HIGH();
				nrf1_rst_deinit();
				nrf2_rst_deinit();
				err_count++;
				if( err_count >= 10)
				{
					err_count = 0;
					send_data_status = 0;
					printf("{'fun':'Error','description':'rf fails, please restart your receiver!'}");
				}
			}
			else
				err_count = 0;
		}

		/* 发送前导帧 */
		whitelist_checktable_and( 0, SEND_DATA_ACK_TABLE, SEND_PRE_TABLE );

		App_retransmit_data(send_data_process_tcb.is_pack_add,
												send_data_process_tcb.logic_pac_add);
		if(send_data_process_tcb.is_pack_add == PACKAGE_NUM_ADD)
			send_data_process_tcb.is_pack_add = PACKAGE_NUM_SAM;
		if(send_data_process_tcb.logic_pac_add == PACKAGE_NUM_ADD)
			send_data_process_tcb.logic_pac_add = PACKAGE_NUM_SAM;

		send_data_status = send_data_status + 1;
	}
}

void retransmit_2s_timer_callback( void )
{
	send_data_status = SEND_2S_DATA_STATUS;
}

void retransmit_500ms_timer_callback( void )
{
	static uint8_t send_count = 0;

	send_count++;
	send_data_status = SEND_500MS_DATA_STATUS;

	if( send_count == 8 )
	{
		send_data_status = SEND_2S_DATA_STATUS;
		send_count = 0;
	}
}

/******************************************************************************
  Function:send_data_process_timer_init
  Description:
		发送过程中的定时器初始化
  Input :
  Return:
  Others:None
******************************************************************************/
void send_data_process_timer_init( void )
{
	/* initialize send_data_process_tcb */
	{
		send_data_process_tcb.pre_data_count      = 110;
		send_data_process_tcb.pre_data_delay100us = 10;
		send_data_process_tcb.data_count          = 2;
		send_data_process_tcb.data_delay100us     = 50;
		send_data_process_tcb.rand_delayms        = 500;
		send_data_process_tcb.retransmit_count    = 5;
	}

	/* create send data timer */
	sw_create_timer(&retransmit_500ms_timer ,        // timer
	                500,                             // timeout (ms)
	                SEND_500MS_TIMER_START_STATUS,   // start status
	                SEND_500MS_TIMER_TIMEOUT_STATUS, // timeout status
	                &(send_data_status),             // status mechine
	                retransmit_500ms_timer_callback);// timerout callback 
	
	sw_create_timer(&retransmit_2s_timer    ,
	                2000, 
	                SEND_2S_TIMER_START_STATUS, 
	                SEND_2S_TIMER_TIMEOUT_STATUS,
	                &(send_data_status), 
	                retransmit_2s_timer_callback);

	/* 接收器包号初始为一个非0的值，这个0用作答题器开机的标志 */
	revicer.sen_num = 0x55;
}
