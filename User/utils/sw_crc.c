#include "sw_crc.h"

/*******************************************************************************
* Function Name   : uint8_t crc_oxr(uint8_t *data, uint16_t length)
* Description     : �����㺯��
* Input           : data	: ����ָ��
*        		    length	�����ݳ���
* Output          : None
*******************************************************************************/
uint8_t crc_xor( uint8_t *pdata, uint16_t len )
{
	uint8_t  crc = *pdata;
	uint16_t i;
	for( i = 1; i < len; i++ )
		crc = crc ^ *(pdata+i);
	return crc;
}
