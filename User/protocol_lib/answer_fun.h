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

#define AS_FUN                  1
#define AS_TIME                 2
#define AS_QUESTION             3
#define AS_QUESTION_T           4
#define AS_QUESTION_I           5
#define AS_QUESTION_R           6
#define AS_HAND                 7
#define AS_SIGN                 8

#define ANSWER_ITEM_NUM         7

/* Private functions ---------------------------------------------------------*/
typedef struct
{
	uint8_t type;
	uint8_t id;
	uint8_t range;
}q_info_t;

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

void dtq_decode_answer( q_info_t *q_tmp, char *q_r, char * q_t );
void dtq_encode_answer( q_info_t *q_tmp, uint8_t *sbuf, uint8_t *sbuf_len );
void serial_cmd_answer_start( char *json_str );
#endif
