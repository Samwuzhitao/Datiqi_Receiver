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

#define AS_FUN                  0
#define AS_TIME                 1
#define AS_QUESTION             2
#define AS_QUESTION_T           3
#define AS_QUESTION_I           4
#define AS_QUESTION_R           5
#define AS_HAND                 6
#define AS_SIGN                 7

#define ANSWER_ITEM_NUM         8

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

typedef union
{
	uint8_t byte;
	struct
	{
		uint8_t hand:1;   /*!< bit:  0..1 hand */
		uint8_t att:1;    /*!< bit:  1..2 att*/
		uint8_t rev:6;    /*!< bit:  2..7 REV */
	}bits;
}hand_att_t;

typedef struct
{
	uint8_t cmd;
	uint8_t len;
	uint8_t buf[250];
}answer_cmd_t;

typedef struct
{
	uint8_t    cmd;
	uint8_t    len;
	hand_att_t hand_att;
}hand_att_cmd_t;

typedef struct
{
	uint8_t cmd;
	uint8_t len;
	uint8_t clear_screen;
}clear_screen_cmd_t;

/* 绑定信道默认配置 **********************************************/
#define RF_CMD_HAND_ATT_DEFAULT         {.cmd      = 0x12,        \
                                         .len      = 0x01,        \
                                         .hand_att = 0x00         }

#define RF_CMD_CLEAR_SCREEN_DEFAULT     {.cmd          = 0x11,    \
                                         .len          = 0x01,    \
                                         .clear_screen = 0x01     }

#define RF_CMD_INIT_DEFAULT       {.head        = RF_PACK_SOF,    \
                                   .end         = RF_PACK_EOF,    \
                                   .ctl.dev_id  = DEVICE_JSQ,     \
                                   .ctl.seq_num = 1,              \
                                   .ctl.pac_num = 1,              \
	                       .ctl.ver_num.bits.major = VERSION_MAJOR, \
	                       .ctl.ver_num.bits.minor = VERSION_MINOR, \
                                   .rev_len     = 0,              \
                                   .pack_len    = 0               }

extern rf_pack_t rf_data;
extern hand_att_cmd_t s_hand_att_cmd;
																	 
void pack_init_answer( answer_cmd_t *cmd );
void answer_pack_quenum_add( answer_cmd_t *cmd );

void dtq_decode_answer( q_info_t *q_tmp, char *q_r, char * q_t );
void dtq_encode_answer( q_info_t *q_tmp, uint8_t *sbuf, uint8_t *sbuf_len );
int8_t serial_cmd_answer_decode( char *json_str, answer_cmd_t *as_cmd );

void rf_pack_del_cmd_no_crc( uint8_t del_cmd );
int8_t rf_check_has_cmd( uint8_t check_cmd, answer_cmd_t *r_cmd );

int8_t rf_pack_add_answer_stop_cmd( void );
int8_t rf_pack_add_raise_hand_att_cmd( hand_att_cmd_t *cmd );
int8_t rf_pack_add_answer_start_cmd( answer_cmd_t *cmd );

#endif
