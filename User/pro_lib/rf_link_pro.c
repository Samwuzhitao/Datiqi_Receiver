/**
  ******************************************************************************
  * @file   	rf_link_protocol.c
  * @author  	Sam.wu
  * @version 	V1.0.0.0
  * @date   	2017.08.14
  * @brief   	platform init functions
  ******************************************************************************
  */

#include "main.h"
#include "answer_fun.h"
#include "json_decode.h"
#include "app_spi_send_data_process.h"

extern revicer_typedef revicer;
Timer_typedef  spi_send_2_timer,spi_send_1_timer;
static uint8_t send_cmd_s = 0;
	
int8_t rf_send_data_start( void )
{
	if((send_cmd_s == 0) || (send_cmd_s >= 4))
	{
		if(send_cmd_s == 0)
		{
			memcpy( rf_data.ctl.src_uid, revicer.uid, 4 );
			memset( rf_data.ctl.dst_uid, 0x00, 4 );
			rf_pro_pack_update_crc( &rf_data );
		}
		send_cmd_s = 1;
		return 0;
	}
	else
		return -1;
}

int8_t rf_send_data_stop( void )
{
	send_cmd_s = 0;
	return 0;
}

void rf_pro_pack_num_add( rf_pack_t *rf_pack )
{
	rf_pack->ctl.pac_num = (rf_pack->ctl.pac_num + 1) % 255;
	if( rf_pack->ctl.pac_num == 0 )
		rf_pack->ctl.pac_num = 1;
}

void rf_pro_seq_num_add( rf_pack_t *rf_pack )
{
	rf_pack->ctl.seq_num = (rf_pack->ctl.seq_num + 1) % 255;
	if( rf_pack->ctl.seq_num == 0 )
		rf_pack->ctl.seq_num = 1;
}

static int8_t rf_pro_set_version( rf_pack_t *rf_pack, uint8_t major, uint8_t minor )
{
	if(( major > 32 ) || ( minor > 32 ))
		return -1;
	else
	{
		rf_pack->ctl.ver_num.bits.major = major;
		rf_pack->ctl.ver_num.bits.minor = minor;
	}
	return 0;
}

void rf_pro_init_pack( rf_pack_t *rf_pack )
{
	rf_pack->head        = RF_PACK_SOF;
	rf_pack->end         = RF_PACK_EOF;
	rf_pack->ctl.dev_id  = DEVICE_JSQ;
	rf_pack->rev_len     = 0;
	rf_pack->pack_len    = 0;
	rf_pro_seq_num_add( rf_pack );
	memcpy( rf_pack->ctl.src_uid, revicer.uid, 4 );
	memset( rf_pack->ctl.dst_uid, 0x00, 4 );
	rf_pro_set_version( rf_pack, VERSION_MAJOR, VERSION_MINOR);
}

void rf_pro_pack_update_crc( rf_pack_t *rf_pack )
{
	uint8_t array_crc[5];
	array_crc[0] = crc_xor((uint8_t *)&(rf_pack->ctl), sizeof(rf_pack_ctl_t));
	array_crc[1] = crc_xor( rf_pack->data, rf_pack->pack_len );
	array_crc[2] = crc_xor( rf_pack->rev_data, rf_pack->rev_len );
	array_crc[3] = rf_pack->rev_len;
	array_crc[4] = rf_pack->pack_len;
	rf_pack->crc_xor = crc_xor( array_crc, 5);
}

void rf_pack_add_data( rf_pack_t *pack_a, uint8_t *buf, uint8_t len )
{
	memcpy( pack_a->data+pack_a->pack_len, buf, len);
	pack_a->pack_len = pack_a->pack_len + len;
}

void rf_s_cmd_process( void )
{
	if( send_cmd_s == 1 )
	{
		spi_cmd_t *s_data = spi_malloc_buf();
		spi_pro_init_pack_rf( s_data, RF_DATA_WITH_PRE, clicker_set.N_CH_RX );
		rf_data_to_spi_data( s_data, &rf_data );
		if(rf_data.pack_len > 0)
			send_cmd_s = 2;
		else
			send_cmd_s = 0;
		return;
	}
	
	if( send_cmd_s == 4 )
	{
		spi_cmd_t *s_data = spi_malloc_buf();
		spi_pro_init_pack_rf( s_data, RF_DATA_WITH_PRE, clicker_set.N_CH_RX );
		rf_data_to_spi_data( s_data, &rf_data );
		send_cmd_s = 5;
		return;
	}
}

static void r_send_2_cb( void )
{
    send_cmd_s = 4;
}

static void r_send_1_cb( void )
{
    static uint8_t send_count = 0;
    send_count++;
    send_cmd_s = 1;
    if( send_count == 4 )
    {
        send_cmd_s = 4;
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
void rf_send_data_process_timer_init( void )
{
  sw_create_timer( &spi_send_1_timer, 300, 2, 3, &(send_cmd_s), r_send_1_cb );
  sw_create_timer( &spi_send_2_timer,1000, 5, 6, &(send_cmd_s), r_send_2_cb );
}

