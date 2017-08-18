#include "main.h"
#include "answer_fun.h"
#include <stdio.h>
#include <stdlib.h>

const json_item_typedef answer_item_list[] = {
{"fun",            sizeof("fun"),            ANSWER_STATUS_FUN},
{"time",           sizeof("time"),           ANSWER_STATUS_TIME},
{"raise_hand",     sizeof("raise_hand"),     ANSWER_STATUS_HAND},
{"questions",      sizeof("questions"),      ANSWER_STATUS_QUESTION},
{"type",           sizeof("type"),           ANSWER_STATUS_DATA_TYPE},
{"id",             sizeof("id"),             ANSWER_STATUS_DATA_ID},
{"range",          sizeof("range"),          ANSWER_STATUS_DATA_RANGE},
{"over",           sizeof("over"),           0xFF}
};


void exchange_json_format( char *out, char old_format, char new_format )
{
	char *pdata = out;

	while(*pdata != '\0')
	{
		if(*pdata == old_format)
		{
			*pdata = new_format;
		}
		pdata++;
	}
}

void parse_str_to_time( char *str )
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


