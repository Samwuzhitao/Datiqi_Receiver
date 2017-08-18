/**
  ******************************************************************************
  * @file   	answer_fun.h
  * @author  	Sam.wu
  * @version 	V1.0.0.0
  * @date   	2017.08.18
  * @brief   	platform init functions
  ******************************************************************************
  */
#include "main.h"

#define ANSWER_STATUS_FUN                  1
#define ANSWER_STATUS_TIME                 2
#define ANSWER_STATUS_QUESTION             3
#define ANSWER_STATUS_DATA_TYPE            4
#define ANSWER_STATUS_DATA_ID              5
#define ANSWER_STATUS_DATA_RANGE           6
#define ANSWER_STATUS_HAND                 7
#define ANSWER_STATUS_SIGN                 8

/* Private functions ---------------------------------------------------------*/
typedef struct
{
	uint8_t type;
	uint8_t id;
	uint8_t range;
}answer_info_typedef;

extern const json_item_typedef answer_item_list[];

void parse_str_to_time( char *str );
void exchange_json_format( char *out, char old_format, char new_format );
void r_answer_dtq_decode( answer_info_typedef * answer_temp, 
	char *answer_range, char * answer_type );
