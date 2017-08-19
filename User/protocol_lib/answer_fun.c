#include "main.h"
#include "answer_fun.h"
#include <stdio.h>
#include <stdlib.h>

static uint8_t answer_quenum     = 1;
static uint8_t is_last_data_full = 0;

const json_item_t answer_list[] = {
{"fun",        sizeof("fun"),        NULL              },
{"time",       sizeof("time"),       answer_time       },
{"raise_hand", sizeof("raise_hand"), answer_raise_hand },
{"questions",  sizeof("questions"),  NULL              },
{"type",       sizeof("type"),       answer_data_type  },
{"id",         sizeof("id"),         answer_data_id    },
{"range",      sizeof("range"),      answer_data_range }
};

void answer_pack_init( answer_cmd_t *answer_pack )
{
	is_last_data_full = 0;
	answer_pack->cmd  = 0x10;
	answer_pack->len  = 0;
}

void answer_pack_quenum_add( answer_cmd_t *answer_pack )
{
	answer_pack->que_num.bits.num = answer_quenum;
	answer_quenum = (answer_quenum + 1) % 16;
	if(answer_quenum == 0)
		answer_quenum = 1;
}

void answer_time( uint8_t *pdada, char *str )
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

void answer_raise_hand( uint8_t *pdada, char *v_str )
{
	
}

void answer_data_type( uint8_t *pdada, char *v_str )
{
	answer_info_typedef *p_answer = (answer_info_typedef *)pdada;
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

void answer_data_id( uint8_t *pdada, char *v_str )
{
	answer_info_typedef *p_answer = (answer_info_typedef *)pdada;
	p_answer->id = atoi( v_str );
}

void answer_data_range( uint8_t *pdada, char *v_str )
{
	answer_info_typedef *p_answer = (answer_info_typedef *)pdada;
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

void r_answer_dtq_encode( uint8_t type, uint8_t *len, uint8_t *rpbuf, uint8_t *spbuf )
{
	uint8_t s_cnt = *len;
	answer_info_typedef *q_tmp = (answer_info_typedef *)rpbuf;
	
	if( type == ANSWER_STATUS_DATA_RANGE )
	{
		if(is_last_data_full == 0)
		{
			*(spbuf+(s_cnt++)) = ((q_tmp->type) & 0x0F ) | 
													 ((q_tmp->id & 0x0F) << 4);
			*(spbuf+(s_cnt++)) = ((q_tmp->id & 0xF0)>>4) | 
													 ((q_tmp->range & 0x0F) << 4);
			*(spbuf+(s_cnt))   = (q_tmp->range & 0xF0)>>4;
			is_last_data_full = 1;
		}
		else
		{
			*(spbuf+(s_cnt))   = *(spbuf+(s_cnt)) | 
														((q_tmp->type & 0x0F) << 4);
			s_cnt++;
			*(spbuf+(s_cnt++)) = q_tmp->id ;
			*(spbuf+(s_cnt++)) = q_tmp->range ;
			is_last_data_full = 0;
		}
#ifdef ENABLE_ANSWER_ENCODE_DEBUG
		{
			uint8_t i;
			printf("ANSWER_ENCMDE:");
			printf("  ");
			for( i=0; i<*len+19; i++ )
				printf("   ");
			for( i=0; i<s_cnt-*len; i++ )
				printf(" %02x", *(spbuf+*len+i));
			printf("\r\n");
		}
#endif
	}
	*len = s_cnt;
}

void r_answer_dtq_decode( answer_info_typedef * answer_temp, 
	char *answer_range, char * answer_type )
{
	memset(answer_range,0x00,7);
	memset(answer_type, 0x00,2);
	switch( answer_temp->type )
	{
		case 0: 
		{
			uint8_t answer = (answer_temp->range)&0x3F;
			uint8_t *pdata = (uint8_t *)answer_range;
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
			memcpy(answer_type,"s",sizeof("s"));
		}
		break;

		case 1: 
		{
			uint8_t i;
			uint8_t answer = (answer_temp->range)&0x3F;
			uint8_t *pdata = (uint8_t *)answer_range;
			
			for( i=0; i<='F'-'A'; i++ )
			{
				uint8_t mask_bit = 1 << i;
				if( (answer & mask_bit) == mask_bit )
				{
					*pdata = 'A'+i;
					pdata = pdata + 1;
				}
			}
			memcpy(answer_type,"m",sizeof("m"));
		}
		break;

		case 2: 
		{
			uint8_t answer = (answer_temp->range)&0x3F;
			switch(answer)
			{
				case 0x01: memcpy(answer_range,"true",sizeof("true"));  break;
				case 0x02: memcpy(answer_range,"false",sizeof("false"));break;
				default: break;
			}
			memcpy(answer_type,"j",sizeof("j"));
		}
		break;

		case 3: 
		{
			if(answer_temp->range < 100)
				sprintf((char *)answer_range, "%d" , answer_temp->range);
			memcpy(answer_type,"d",sizeof("d"));
		}
		break;
		
		case 4: 
		{
			uint8_t answer = 0;
			{
				uint8_t i;
				uint8_t *pdata = (uint8_t *)answer_range;
				answer = (answer_temp->range)&0x3F;
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
			answer = (answer_temp->range)&0xC0;
			switch(answer)
			{
				case 0x40: // true
					memcpy(answer_range,"true",sizeof("true"));
				break;
				case 0x80: // false
					memcpy(answer_range,"false",sizeof("false"));
				break;
				default: break;
			}
			memcpy(answer_type,"g",sizeof("g"));
		}
		break;
		default: break;
	}
}


