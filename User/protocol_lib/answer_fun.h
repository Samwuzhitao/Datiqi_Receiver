/**
  ******************************************************************************
  * @file   	answer_fun.h
  * @author  	Sam.wu
  * @version 	V1.0.0.0
  * @date   	2017.08.18
  * @brief   	platform init functions
  ******************************************************************************
  */
#ifndef __ANSWER_FUN_H_
#define __ANSWER_FUN_H_

#include "main.h"

//#define ENABLE_ANSWER_ENCODE_DEBUG  

#define ANSWER_STATUS_FUN                  1
#define ANSWER_STATUS_TIME                 2
#define ANSWER_STATUS_QUESTION             3
#define ANSWER_STATUS_DATA_TYPE            4
#define ANSWER_STATUS_DATA_ID              5
#define ANSWER_STATUS_DATA_RANGE           6
#define ANSWER_STATUS_HAND                 7
#define ANSWER_STATUS_SIGN                 8

#define ANSWER_ITEM_NUM                    7

/* Private functions ---------------------------------------------------------*/
typedef struct
{
	uint8_t type;
	uint8_t id;
	uint8_t range;
}answer_info_typedef;

typedef struct
{
	char    *key;
	uint8_t str_len;
	void    (*j_fun)( uint8_t *pdada, char *v_str );
}json_item_t;

typedef struct
{
	char    *key;
	uint8_t str_len;
	uint8_t status;
}json_item_typedef;

typedef union
{
	uint8_t byte;
	struct
	{
		uint8_t num:4;   /*!< bit:  0..3 NUM */
		uint8_t rev:4;   /*!< bit:  4..7 REV */
	}bits;
}que_num_t;

typedef struct
{
	uint8_t   cmd;
	uint8_t   len;
	que_num_t que_num;
	uint8_t   buf[250];
}answer_cmd_t;

void answer_pack_init( answer_cmd_t *answer_pack );
void answer_pack_quenum_add( answer_cmd_t *answer_pack );

void answer_time      ( uint8_t *pdada, char *v_str );
void answer_raise_hand( uint8_t *pdada, char *v_str );
void answer_data_type ( uint8_t *pdada, char *v_str );
void answer_data_id   ( uint8_t *pdada, char *v_str );
void answer_data_range( uint8_t *pdada, char *v_str );
extern const json_item_t answer_list[];

void parse_str_to_time( char *str );
void r_answer_dtq_decode( answer_info_typedef *answer_temp,
	char *answer_range, char * answer_type );
void r_answer_dtq_encode( uint8_t type, uint8_t *len, uint8_t *rpbuf, uint8_t *spbuf );

#endif
