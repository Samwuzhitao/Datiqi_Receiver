/**
  ******************************************************************************
  * @file   	main.h
  * @author  	Tian erjun
  * @version 	V1.0.0.0
  * @date   	2015.11.05
  * @brief   	include all other header files
  ******************************************************************************
  */
#ifndef _MAIN_H_
#define _MAIN_H_
#include "stm32f10x.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "board.h"

#include "stm32f10x_it.h"
#include "mfrc500.h"
#include "m24sr_cmd.h"
#include "eeprom.h"
#include "gpio.h"

#include "app_timer.h"
#include "whitelist.h"
#include "ringbuffer.h"

#include "rf_link_protocol.h"
#include "spi_link_protocol.h"

typedef struct 
{
	uint8_t 			tx_len;			  					//2.4G发送长度
	uint8_t				tx_buf[RF_NBUF];				//2.4G发送缓存  
}RF_TypeDef;

extern bool 						  gbf_hse_setup_fail;	// 外部16M晶振起振标志
extern RF_TypeDef 				rf_var;							// 2.4G数据包缓冲
extern uint8_t 						g_cardType[];				// 返回卡类型
extern uint8_t 						respon[];

typedef struct
{
	uint8_t  uid[4];
	uint8_t  pos;
	uint8_t  rev_num;
	uint8_t  rev_seq;
	uint8_t  sen_num;
	uint8_t  sen_seq;
}dtq_ctl_typedef;

typedef struct
{
	uint8_t  uid[4];
	uint8_t  sen_num;
	uint8_t  sen_seq;
	uint8_t  pre_seq;
	uint32_t data_statistic_count;
}jsq_ctl_t;

typedef struct
{
	dtq_ctl_typedef uids[120];
	uint8_t    len;
	uint8_t    switch_status;
	uint8_t    start;
	uint8_t    attendance_sttaus;
	uint8_t    match_status;
}uid_list_ctl_t;

extern nrf_communication_t   nrf_data;
extern jsq_ctl_t             revicer;
extern uid_list_ctl_t        wl;
extern       uint8_t         jsq_uid[8];
extern const uint8_t         software[3];
extern const uint8_t 		     hardware[30];
extern const uint8_t 			   company[16];
extern       uint16_t        list_tcb_table[16][8];

#endif //_MAIN_H_
/**
  * @}
  */
/**************************************END OF FILE****************************/

