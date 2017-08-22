/**
  ******************************************************************************
  * @file   	sw_crc.h
  * @author  	sam.wu
  * @version 	V1.0.0.0
  * @date   	2018.08.22
  * @brief   	include all other header files
  ******************************************************************************
  */
#ifndef _SW_CRC_
#define _SW_CRC_

#include "stm32f10x.h"

uint8_t crc_xor( uint8_t *pdata, uint16_t len );

#endif
