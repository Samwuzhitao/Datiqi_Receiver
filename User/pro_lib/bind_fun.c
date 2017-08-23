#include "main.h"
#include "bind_fun.h"
#include "json_decode.h"

#include <stdio.h>
#include <stdlib.h>
#include "app_spi_send_data_process.h"
#include "app_card_process.h"

uint32_t  wireless_uid_list[200];
rf_pack_t rf_bind   = RF_CMD_INIT_DEFAULT ;
uint8_t   bind_mode = 0, upos = 0;

Timer_typedef rf_bind_300ms_timer, rf_bind_500ms_timer;

static uint8_t send_cmd_s = 0;
static uint8_t scan_tx_ch_index = 0;
static uint8_t scan_tx_ch[5] = { 0x02, 0x01, 0x05, 0x06, 0x09 };

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
		spi_cmd_t *s_data = spi_malloc_buf();
		spi_pro_init_pack_rf( s_data, RF_DATA_WITH_PRE, clicker_set.N_CH_RX );
		rf_data_to_spi_data( s_data, &rf_bind );
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
    if( send_count == 5 )
    {
        send_cmd_s = 4;
        send_count = 0;
    }
}

void rf_send_bind_process_timer_init( void )
{
  sw_create_timer( &rf_bind_300ms_timer, 150, 2, 3, &(send_cmd_s), r_send_1_cb );
  sw_create_timer( &rf_bind_500ms_timer,1000, 5, 6, &(send_cmd_s), r_send_2_cb );
}
