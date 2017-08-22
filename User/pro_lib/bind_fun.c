#include "main.h"
#include "bind_fun.h"
#include "json_decode.h"
#include <stdio.h>
#include <stdlib.h>
#include "app_spi_send_data_process.h"
#include "app_card_process.h"

uint32_t wireless_uid_list[200];
uint8_t  bind_mode = 0, upos = 0;

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

void serial_cmd_wireless_bind_start( char *json_str )
{
  /* prase data control */
  char     *p_end,*p_next = json_str;
  uint16_t str_len = strlen(json_str);
  int8_t   err;
  rf_pack_t rf_data;

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
							bind_mode = 0;
							json_decode_mode( NULL, item_str );
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

	switch( bind_mode )
	{
		case 0: // 刷卡绑定
		{
			uint8_t card_status = rf_get_card_status();
			if( card_status == 0 )
			{
				err = 0;
				wl.match_status = ON;
				rf_set_card_status(1);
			}
			else
			  err = -1;
		}
		break;
		case 1: // 无线手动绑定
		{

		}
		break;
		case 2: // 无线自动
		{
			spi_cmd_t *s_data = spi_malloc_buf();
			rf_pro_init_pack( &rf_data );
			rf_pro_pack_num_add( &rf_data );
			err = initialize_white_list();
		}
		break;
		default: break;
	}

	b_print("{\r\n");
	b_print("  \"fun\": \"bind_start\",\r\n");
	b_print("  \"result\": \"%d\"\r\n",err);
	b_print("}\r\n");
}

