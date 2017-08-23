#include "main.h"
#include "answer_fun.h"
#include "json_decode.h"
#include <stdio.h>
#include <stdlib.h>
#include "app_spi_send_data_process.h"

hand_att_cmd_t s_hand_att_cmd = RF_CMD_HAND_ATT_DEFAULT;
rf_pack_t      rf_data;

static uint8_t  answer_quenum     = 1;
static uint8_t  s_is_last_data_full = 0;

static void json_decode_time        ( uint8_t *pdada, char *v_str );
static void json_decode_answer_type ( uint8_t *pdada, char *v_str );
static void json_decode_answer_id   ( uint8_t *pdada, char *v_str );
static void json_decode_answer_range( uint8_t *pdada, char *v_str );
static void json_decode_att         ( uint8_t *pdada, char *v_str );
static void json_decode_hand        ( uint8_t *pdada, char *v_str );

const json_item_t answer_list[] = {
{"fun",        sizeof("fun"),        NULL                    },
{"time",       sizeof("time"),       json_decode_time        },
{"questions",  sizeof("questions"),  NULL                    },
{"type",       sizeof("type"),       json_decode_answer_type },
{"id",         sizeof("id"),         json_decode_answer_id   },
{"range",      sizeof("range"),      json_decode_answer_range},
{"raise_hand", sizeof("raise_hand"), json_decode_hand        },
{"attendance", sizeof("attendance"), json_decode_att         }
};

static void json_decode_hand( uint8_t *pdada, char *v_str )
{
    hand_att_cmd_t *cmd = (hand_att_cmd_t *)pdada;
    uint32_t temp = atoi( v_str );
    if( temp <= 1 )
    {
        if( temp == 0 )
            cmd->hand_att.bits.hand = 0;
        else
            cmd->hand_att.bits.hand = 1;
    }
}

static void json_decode_att( uint8_t *pdada, char *v_str )
{
    hand_att_cmd_t *cmd = (hand_att_cmd_t *)pdada;
    uint32_t temp = atoi( v_str );
    if( temp <= 1 )
    {
        if( temp == 0 )
            cmd->hand_att.bits.att = 0;
        else
            cmd->hand_att.bits.att = 1;
    }
}


void pack_init_answer( answer_cmd_t *cmd )
{
    s_is_last_data_full = 0;
    cmd->cmd  = 0x10;
    cmd->len  = 1;
		cmd->buf[0] = answer_quenum;
}

void answer_pack_quenum_add( answer_cmd_t *cmd )
{
    cmd->buf[0] = answer_quenum ;
    answer_quenum = (answer_quenum + 1) % 16;
    if(answer_quenum == 0)
        answer_quenum = 1;
}

static void json_decode_time( uint8_t *pdada, char *str )
{
    char str1[10];
    /*system_rtc_timer:year*/
    memset(str1,0,10);
    memcpy(str1,str,4);
    system_rtc_timer.year = atoi( str1 );
    /*system_rtc_timer:mon*/
    memset(str1,0,10);
    memcpy(str1,str+5,2);
    system_rtc_timer.mon = atoi( str1 );
    /*system_rtc_timer:date*/
    memset(str1,0,10);
    memcpy(str1,str+8,2);
    system_rtc_timer.date = atoi( str1 );
    /*system_rtc_timer:hour*/
    memset(str1,0,10);
    memcpy(str1,str+11,2);
    system_rtc_timer.hour = atoi( str1 );
    /*system_rtc_timer:min*/
    memset(str1,0,10);
    memcpy(str1,str+14,2);
    system_rtc_timer.min = atoi( str1 );
    /*system_rtc_timer:sec*/
    memset(str1,0,10);
    memcpy(str1,str+17,2);
    system_rtc_timer.sec = atoi( str1 );
}


static void json_decode_answer_type( uint8_t *pdada, char *v_str )
{
    q_info_t *p_answer = (q_info_t *)pdada;
    switch( v_str[0] )
    {
        case 's': p_answer->type = 0; break;
        case 'm': p_answer->type = 1; break;
        case 'j': p_answer->type = 2; break;
        case 'd': p_answer->type = 3; break;
        case 'g': p_answer->type = 4; break;
        default: break;
    }
}

static void json_decode_answer_id( uint8_t *pdada, char *v_str )
{
    q_info_t *p_answer = (q_info_t *)pdada;
    p_answer->id = atoi( v_str );
}

static void json_decode_answer_range( uint8_t *pdada, char *v_str )
{
    q_info_t *p_answer = (q_info_t *)pdada;
    char range_end;
    if( p_answer->type == 2 )
        p_answer->range = 0x03;
    else if( p_answer->type == 4 )
        p_answer->range = 0xFF;
    else
    {
        range_end  = v_str[2];
        if(( range_end >= 'A') && ( range_end <= 'G'))
        {
            uint8_t j;
            for(j=0;j<=range_end-'A';j++)
                p_answer->range |= 1<<j;
        }
        if(( range_end >= '0') && ( range_end <= '9'))
                p_answer->range = range_end - '0';
    }
}

void q_encode_s( q_info_t * q_tmp, char *q_r, char * q_t )
{
    uint8_t answer = (q_tmp->range)&0x3F;
    uint8_t *pdata = (uint8_t *)q_r;
    switch(answer)
    {
        case 0x01: *pdata = 'A'; break;
        case 0x02: *pdata = 'B'; break;
        case 0x04: *pdata = 'C'; break;
        case 0x08: *pdata = 'D'; break;
        case 0x10: *pdata = 'E'; break;
        case 0x20: *pdata = 'F'; break;
        default: break;
    }
    memcpy(q_t,"s",sizeof("s"));
}

void q_encode_m( q_info_t * q_tmp, char *q_r, char * q_t )
{
    uint8_t i;
    uint8_t answer = (q_tmp->range)&0x3F;
    uint8_t *pdata = (uint8_t *)q_r;

    for( i=0; i<='F'-'A'; i++ )
    {
        uint8_t mask_bit = 1 << i;
        if( (answer & mask_bit) == mask_bit )
        {
            *pdata = 'A'+i;
            pdata = pdata + 1;
        }
    }
    memcpy(q_t,"m",sizeof("m"));
}

void q_encode_j( q_info_t * q_tmp, char *q_r, char * q_t )
{
    uint8_t answer = (q_tmp->range)&0x3F;
    switch(answer)
    {
        case 0x01: memcpy(q_r,"true",sizeof("true"));  break;
        case 0x02: memcpy(q_r,"false",sizeof("false"));break;
        default: break;
    }
    memcpy(q_t,"j",sizeof("j"));
}

void q_encode_d( q_info_t * q_tmp, char *q_r, char * q_t )
{
    if(q_tmp->range < 100)
        sprintf((char *)q_r, "%d" , q_tmp->range);
    memcpy(q_t,"d",sizeof("d"));
}

void q_encode_g( q_info_t * q_tmp, char *q_r, char * q_t )
{
    uint8_t answer = 0;
    {
        uint8_t i;
        uint8_t *pdata = (uint8_t *)q_r;
        answer = (q_tmp->range)&0x3F;
        for( i=0; i<='F'-'A'; i++ )
        {
            uint8_t mask_bit = 1 << i;
            if( (answer & mask_bit) == mask_bit )
            {
                *pdata = 'A'+i;
                pdata = pdata + 1;
            }
        }
    }
    answer = (q_tmp->range)&0xC0;
    switch(answer)
    {
        case 0x40: // true
            memcpy(q_r,"true",sizeof("true"));
        break;
        case 0x80: // false
            memcpy(q_r,"false",sizeof("false"));
        break;
        default: break;
    }
    memcpy(q_t,"g",sizeof("g"));
}

void dtq_encode_answer( q_info_t *q_tmp, uint8_t *sbuf, uint8_t *sbuf_len )
{
    uint8_t s_cnt = *sbuf_len;

    if(s_is_last_data_full == 0)
    {
        *(sbuf+(s_cnt++)) = ((q_tmp->type) & 0x0F ) |
                            ((q_tmp->id & 0x0F) << 4);
        *(sbuf+(s_cnt++)) = ((q_tmp->id & 0xF0)>>4) |
                            ((q_tmp->range & 0x0F) << 4);
        *(sbuf+(s_cnt))   = (q_tmp->range & 0xF0)>>4;
        s_is_last_data_full = 1;
    }
    else
    {
        *(sbuf+(s_cnt))   = *(sbuf+(s_cnt)) | ((q_tmp->type & 0x0F) << 4);
        s_cnt++;
        *(sbuf+(s_cnt++)) = q_tmp->id ;
        *(sbuf+(s_cnt++)) = q_tmp->range ;
        s_is_last_data_full = 0;
    }
#ifdef ENABLE_ANSWER_ENCODE_DEBUG
    {
        uint8_t i;
        b_print("ANSWER_ENCMDE:");
        b_print("  ");
        for( i=0; i<*sbuf_len+21; i++ )
            b_print("   ");
        for( i=0; i<s_cnt-*sbuf_len+s_is_last_data_full; i++ )
            b_print(" %02x", *(sbuf+*sbuf_len+i));
        b_print("\r\n");
    }
#endif
    *sbuf_len = s_cnt;
}

void dtq_decode_answer( q_info_t * q_tmp, char *q_r, char * q_t )
{
    switch( q_tmp->type )
    {
        case 0: q_encode_s( q_tmp, q_r, q_t); break;
        case 1: q_encode_m( q_tmp, q_r, q_t); break;
        case 2: q_encode_j( q_tmp, q_r, q_t); break;
        case 3: q_encode_d( q_tmp, q_r, q_t); break;
        case 4: q_encode_g( q_tmp, q_r, q_t); break;
        default: break;
    }
}

void serial_cmd_answer_start( char *json_str )
{
    /* prase data control */
		uint8_t  set_raise_hand_att_flg = 0;
    char     *p_end,*p_next = json_str;
    uint16_t str_len = strlen(json_str);
    uint8_t  q_num   = 0;
    int8_t   err     = 0;
    /* send data control */
    q_info_t  q_tmp = { 0, 0, 0 };

    answer_cmd_t   *as_cmd = (answer_cmd_t *)rf_data.data;

		rf_pro_init_pack( &rf_data );
		rf_pro_pack_num_add( &rf_data );

		pack_init_answer( as_cmd );
		answer_pack_quenum_add( as_cmd );

		do
		{
				uint8_t item_t = 0;
				char item_str[25],key_str[20];
				memset( item_str, 0x00, 25 );
				memset( key_str,  0x00, 20 );
				p_end  = parse_json_item( p_next, key_str, item_str );
				p_next = p_end;

				for( item_t = 0; item_t < ANSWER_ITEM_NUM; item_t++ )
				{
						if( strncmp( key_str, answer_list[item_t].key,
										answer_list[item_t].str_len ) == 0 )
						{
								if( answer_list[item_t].j_fun != NULL )
								switch( item_t )
								{
										case AS_TIME:
												json_decode_time( NULL, item_str );
										break;
										case AS_QUESTION_T:
												answer_list[item_t].j_fun( (uint8_t *)(&q_tmp), item_str );
										break;
										case AS_QUESTION_I:
												json_decode_answer_id( (uint8_t *)(&q_tmp), item_str );
										break;
										case AS_QUESTION_R:
										{
												json_decode_answer_range( (uint8_t *)(&q_tmp), item_str );
												dtq_encode_answer( &q_tmp, as_cmd->buf, &(as_cmd->len));
												q_num++;
										}
										break;
										case AS_HAND:
										{
												json_decode_hand( (uint8_t *)&s_hand_att_cmd, item_str );
												set_raise_hand_att_flg = 1;
										}
										break;
										case AS_SIGN:
										{
												json_decode_att( (uint8_t *)&s_hand_att_cmd, item_str );
												set_raise_hand_att_flg = 1;
										}
										break;
										default: break;
								}
								break;
						}
				}

				if( q_num > 80 )
				{
						err = -2;
						break;
				}
		}while( ( p_end - json_str ) < str_len-3 );
		as_cmd->len = as_cmd->len + s_is_last_data_full;
		rf_data.pack_len = as_cmd->len + 2;
		if( set_raise_hand_att_flg == 1)
			rf_pack_add_raise_hand_att_cmd( &s_hand_att_cmd );
		err =	rf_send_data_start();

    b_print("{\r\n");
    b_print("  \"fun\": \"answer_start\",\r\n");
    b_print("  \"result\": \"%d\"\r\n",err);
    b_print("}\r\n");
}

void rf_pack_del_answer_cmd_no_crc( void )
{
	uint8_t r_index = 0, w_index = 0;
	uint8_t err = 0;
	answer_cmd_t *pcmd;
	/* 剔除答题指令 */
	do
	{
		pcmd = (answer_cmd_t *)(rf_data.data + r_index);
		r_index = r_index + pcmd->len + 2;

		if( pcmd->cmd != 0x10 )
		{
#ifdef ENABLE_ANSWER_ENCODE_DEBUG
			{
				uint8_t i;
				uint8_t *pdata = (uint8_t *)pcmd;
				b_print("RF DATA:");
				for( i =0; i < pcmd->len + 2;i++)
					b_print(" %02x",*(pdata+i));
				b_print("\r\n");
			}
#endif
			memcpy( rf_data.data+w_index, (uint8_t *)pcmd, pcmd->len + 2);
			w_index = w_index + pcmd->len + 2;
		}
	}while( r_index < rf_data.pack_len );
	rf_data.pack_len = w_index;
}

int8_t rf_check_has_cmd( uint8_t check_cmd, answer_cmd_t *r_cmd )
{
	uint8_t r_index = 0;
	uint8_t err = 0, is_has_cmd = 0;
	answer_cmd_t *pcmd;
	/* 检查是否有举手指令，有指令直接修改指令 */
	do
	{
		pcmd = (answer_cmd_t *)(rf_data.data + r_index);
		r_index = r_index + pcmd->len + 2;

		if( pcmd->cmd == check_cmd )
		{
			r_cmd = pcmd;
#ifdef ENABLE_ANSWER_ENCODE_DEBUG
			{
				uint8_t i;
				uint8_t *pdata = (uint8_t *)pcmd;
				b_print("[RF] RAISE_HAND_ATT:");
				for( i =0; i < pcmd->len + 2;i++)
					b_print(" %02x",*(pdata+i));
				b_print("\r\n");
			}
#endif
			is_has_cmd = 1;
		}
	}while( r_index < rf_data.pack_len );
	
	return is_has_cmd;
}

int8_t rf_pack_add_answer_stop_cmd( void )
{
	uint8_t err = 0, is_has_cmd = 0;
	answer_cmd_t *pcmd;
	clear_screen_cmd_t cear_screen_cmd = RF_CMD_CLEAR_SCREEN_DEFAULT;
	/* 剔除就有的答题指令 */
	rf_pack_del_answer_cmd_no_crc();
	/* 检查指令是否已经在运输帧中 */
	is_has_cmd = rf_check_has_cmd( RF_CMD_CLERA_SCREEN, pcmd );
  /* 添加指令到运输帧 */
	if(is_has_cmd == 1)
		memcpy( (uint8_t *)pcmd, (uint8_t *)&cear_screen_cmd, sizeof(clear_screen_cmd_t));
	else
		rf_pack_add_data( &rf_data, (uint8_t *)&cear_screen_cmd, sizeof(clear_screen_cmd_t));
	/* 更新 包号 帧号和CRC */
	rf_pro_seq_num_add( &rf_data );
	rf_pro_pack_num_add( &rf_data );
	rf_pro_pack_update_crc( &rf_data );
	return err;
}

int8_t rf_pack_add_raise_hand_att_cmd( hand_att_cmd_t *cmd )
{
	uint8_t err = 0, is_has_cmd = 0;
	answer_cmd_t *pcmd;
	/* 检查指令是否已经在运输帧中 */
  is_has_cmd = rf_check_has_cmd( RF_CMD_RAISE_HAND_ATT, pcmd );
	/* 添加指令到运输帧 */
	if( is_has_cmd == 1 )
		memcpy( (uint8_t *)pcmd, (uint8_t *)cmd, sizeof(clear_screen_cmd_t));
	else
		rf_pack_add_data( &rf_data, (uint8_t *)cmd, sizeof(clear_screen_cmd_t));
	/* 更新 包号 帧号和CRC */
	rf_pro_seq_num_add( &rf_data );
	rf_pro_pack_num_add( &rf_data );
	rf_pro_pack_update_crc( &rf_data );
	return err;
}
