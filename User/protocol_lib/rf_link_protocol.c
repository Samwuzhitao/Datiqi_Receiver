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
extern revicer_typedef   revicer;

static uint8_t seq_num = 1, pac_num = 1;

void rf_pro_pack_num_add( rf_pack_t *rf_pack )
{
	pac_num = (pac_num + 1) % 255;
	if( pac_num == 0 )
		pac_num = 1;
}

static void rf_pro_seq_num_add( rf_pack_t *rf_pack )
{
	seq_num = (seq_num + 1) % 255;
	if( seq_num == 0 )
		seq_num = 1;
}

static int8_t rf_pro_set_version( rf_pack_t *rf_pack, uint8_t major, uint8_t minor )
{
	if(( major > 32 ) || ( minor > 32 ))
		return -1;
	else
	{
		rf_pack->ctl.ver_num.bits.major = major;
		rf_pack->ctl.ver_num.bits.minor = minor;
	}
	return 0;
}

void rf_pro_init_pack( rf_pack_t *rf_pack )
{
	rf_pack->head        = RF_PACK_SOF;
	rf_pack->end         = RF_PACK_EOF;
	rf_pack->ctl.dev_id  = DEVICE_JSQ;
	rf_pack->ctl.seq_num = seq_num;
	rf_pack->ctl.pac_num = pac_num;
	rf_pack->rev_len     = 0;
	rf_pack->pack_len    = 0;
	rf_pro_seq_num_add( rf_pack );
	memcpy( rf_pack->ctl.src_uid, revicer.uid, 4 );
	rf_pro_set_version( rf_pack, VERSION_MAJOR, VERSION_MINOR);
}

void rf_pro_pack_update_crc( rf_pack_t *rf_pack )
{
	uint8_t array_crc[5];
	array_crc[0] = XOR_Cal((uint8_t *)&(rf_pack->ctl), sizeof(rf_pack_ctl_t));
	array_crc[1] = XOR_Cal( rf_pack->data, rf_pack->pack_len );
	array_crc[2] = XOR_Cal( rf_pack->rev_data, rf_pack->rev_len );
	array_crc[3] = rf_pack->rev_len;
	array_crc[4] = rf_pack->pack_len;
	rf_pack->crc_xor = XOR_Cal( array_crc, 5);
}

void rf_pack_add_data( rf_pack_t *pack_a, uint8_t *buf, uint8_t len )
{
	memcpy( pack_a->data+pack_a->pack_len, buf, len);
	pack_a->pack_len = pack_a->pack_len + len;
}

