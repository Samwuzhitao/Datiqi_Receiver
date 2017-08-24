#include "main.h"
#include "bind_fun.h"
#include "json_decode.h"

#include <stdio.h>
#include <stdlib.h>
#include "app_spi_send_data_process.h"
#include "app_card_process.h"

extern revicer_typedef     revicer;

uint32_t  wireless_uid_list[200];
rf_pack_t rf_bind   = RF_CMD_INIT_DEFAULT ;
uint8_t   bind_mode = 0, upos = 0 , wireless_upos;

Timer_typedef rf_bind_300ms_timer, rf_bind_500ms_timer;

static uint8_t send_cmd_s = 0;
static uint8_t scan_tx_ch_index = 0;
static uint8_t scan_tx_ch[5] = { 4, 3, 7, 8, 11 };

static void json_decode_mode( uint8_t *pdada, char *v_str );
static void json_decode_uid( uint8_t *pdada, char *v_str );

const json_item_t bind_list[] = {
    {"fun",  sizeof("fun"),  NULL             },
    {"mode", sizeof("mode"), json_decode_mode },
    {"list", sizeof("list"), NULL             },
    {"uid",  sizeof("uid"),  json_decode_uid  }
};

static void json_decode_mode( uint8_t *pdada, char *v_str )
{
    uint32_t tmp   = atoi( v_str );
    if( tmp <= 2 )
        bind_mode = tmp;
}

static void json_decode_uid( uint8_t *pdada, char *v_str )
{
    uint32_t tmp = atoi( v_str );
    wireless_uid_list[upos] = tmp;
    upos = (upos + 1) % 200;
}

int8_t serial_cmd_wireless_bind_decode( char *json_str, uint8_t *mode )
{
  /* prase data control */
  char     *p_end,*p_next = json_str;
  uint16_t str_len = strlen(json_str);
  int8_t   err = 0;

	do
	{
		uint8_t item_t = 0;
		char item_str[25],key_str[20];
		memset( item_str, 0x00, 25 );
		memset( key_str,  0x00, 20 );
		p_end  = parse_json_item( p_next, key_str, item_str );
		p_next = p_end;

		for( item_t = 0; item_t < BIND_ITEM_NUM; item_t++ )
		{
			if( strncmp( key_str, bind_list[item_t].key,
									 bind_list[item_t].str_len ) == 0 )
			{
				if( bind_list[item_t].j_fun != NULL )
				switch( item_t )
				{
					case BI_MODE:
					{
						bind_mode = 0;
						json_decode_mode( NULL, item_str );
						*mode = bind_mode;
					}
					break;
					case BI_UID:
						json_decode_uid( NULL, item_str );
					break;
					default: break;
				}
				break;
			}
		}
	}while( ( p_end - json_str ) < str_len-3 );

	return err;
}

void bind_send_ack( answer_cmd_t *answer_cmd )
{
	uint8_t        i = 0;
	rf_pack_t      rf_bind_ack;
	spi_cmd_t      *rf_ack_data = spi_malloc_buf();
	answer_cmd_t   *ack_cmd = (answer_cmd_t *)rf_bind_ack.data;
	uint8_t *daq_uid = (uint8_t *)((uint8_t *)answer_cmd + sizeof(wireless_mode1_cmd_t));
  wireless_mode1_cmd_t *cmd = (wireless_mode1_cmd_t *)answer_cmd;
	spi_pro_init_pack_rf( rf_ack_data, RF_DATA_NO_PRE, cmd->rx_ch );
	rf_pro_init_pack( &rf_bind_ack );
	memcpy( rf_bind_ack.ctl.dst_uid, daq_uid, 4);
	ack_cmd->cmd      = 0x41;
	ack_cmd->len      = 0x0B;
	ack_cmd->buf[i++] = 0x01;
	ack_cmd->buf[i++] = 0x00;
	ack_cmd->buf[i++] = wireless_upos;
	memcpy( ack_cmd->buf+i, revicer.uid, 4 );
	i = i + 4;
	memcpy( ack_cmd->buf+i, cmd->pin   , 2 );
	i = i + 2;
	ack_cmd->buf[i++] = clicker_set.N_CH_TX;
	ack_cmd->buf[i++] = clicker_set.N_CH_RX;
	rf_bind_ack.pack_len = ack_cmd->len + 2;
	rf_data_to_spi_data( rf_ack_data, &rf_bind_ack );
}

void bind_wireless_upos_add( void )
{
	wireless_upos++;
}

int8_t rf_send_bind_start( void )
{
	send_cmd_s = 1;
	return 0;
}

int8_t rf_send_bind_stop( void )
{
	send_cmd_s = 0;
	return 0;
}

void rf_bind_process( void )
{
if( send_cmd_s == 1 )
	{
		nor_buf_t t_conf = RF_TX_CH_DEFAULT_CONF;
		t_conf.t_buf[RF_ADDR_CH] = scan_tx_ch[scan_tx_ch_index];
		scan_tx_ch_index = (scan_tx_ch_index + 1) % 5;
		spi_write_cmd_to_tx( t_conf.t_buf, t_conf.len );
		spi_cmd_t *s_data = spi_malloc_buf();
		spi_pro_init_pack_rf( s_data, RF_DATA_WITH_PRE, clicker_set.N_CH_RX );
		rf_data_to_spi_data( s_data, &rf_bind );
		if(rf_bind.pack_len > 0)
			send_cmd_s = 2;
		else
			send_cmd_s = 0;
		return;
	}

	if( send_cmd_s == 4 )
	{
  	send_cmd_s = 5;
		return;
	}
}

static void r_send_2_cb( void )
{
		  send_cmd_s = 1;
      scan_tx_ch_index = 0;
}

static void r_send_1_cb( void )
{
    static uint8_t send_count = 0;
    send_count++;
    send_cmd_s = 1;
    if( send_count == 5 )
    {
        send_cmd_s = 4;
        send_count = 0;
    }
}

void rf_send_bind_process_timer_init( void )
{
  sw_create_timer( &rf_bind_300ms_timer, 100, 2, 3, &(send_cmd_s), r_send_1_cb );
  sw_create_timer( &rf_bind_500ms_timer,2000, 5, 6, &(send_cmd_s), r_send_2_cb );
}
