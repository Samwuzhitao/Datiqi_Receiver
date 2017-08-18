/**
  ******************************************************************************
  * @file   	rf_link_protocol.c
  * @author  	Sam.wu
  * @version 	V1.0.0.0
  * @date   	2017.08.14
  * @brief   	platform init functions
  ******************************************************************************
  */

#include "main.h"
										
static void rf_pro_pack_num_add( rf_pack_t *rf_pack )
{
	rf_pack->pack_num = (rf_pack->pack_num + 1) % 255;
	if( rf_pack->pack_num == 0 )
		rf_pack->pack_num = 1;
}

static int8_t rf_pro_set_version( rf_pack_t *rf_pack, uint8_t major, uint8_t minor )
{
	if(( major > 32 ) || ( minor > 32 ))
		return -1;
	else
	{
		rf_pack->ver_num.bits.major = major;
		rf_pack->ver_num.bits.minor = minor;
	}
	return 0;
}

void rf_pro_init_pack( rf_pack_t *rf_pack )
{
	rf_pack->head = RF_PACK_SOF;
	rf_pack->end  = RF_PACK_EOF;
	rf_pro_set_version( rf_pack, VERSION_MAJOR, VERSION_MINOR);
}

void rf_pro_pack_update_crc( rf_pack_t *rf_pack )
{
	rf_pack->crc_xor = XOR_Cal(rf_pack->dst_uid,
		rf_pack->pack_len + rf_pack->rev_len + 5 +8 );
}


