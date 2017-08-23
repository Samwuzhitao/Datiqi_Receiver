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

int8_t serial_cmd_wireless_bind_decode( char *json_str, uint8_t *mode )
{
  /* prase data control */
  char     *p_end,*p_next = json_str;
  uint16_t str_len = strlen(json_str);
  int8_t   err;

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

