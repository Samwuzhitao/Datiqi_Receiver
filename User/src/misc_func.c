/**
  ******************************************************************************
  * @file   	misc.c
  * @author  	Tian erjun
  * @version 	V1.0.0.0
  * @date   	2015.11.05
  * @brief   	hal function for misc functions
  ******************************************************************************
  */
 
#include "main.h"

/*********************************************************************************
**��	�ܣ������㺯��
**��    ��: data	����������ݿ�
**		  : length	���ݿ鳤��
**��	�أ���������
**��	ע��NULL
*********************************************************************************/
uint8_t XOR_Cal(uint8_t *data, uint16_t length)
{
	uint8_t temp_xor;
	uint16_t i;

	temp_xor = *data;
	for(i = 1;i < length; i++)
	{
		temp_xor = temp_xor ^ *(data+i);
	}
	return temp_xor;
}

/*********************************************************************************
**��	�ܣ�CRC16���㺯��
**��    ��: pucY	����������ݿ�
**		  : ucX	���ݿ鳤��
**��	�أ�CRC16������
**��	ע��NULL
*********************************************************************************/
#define PRESET_VALUE				0xFFFF 
#define POLYNOMIAL					0x8408 
uint16_t Crc16Cal(uint8_t const * dest, uint8_t length)
{
	uint8_t ucI,ucJ;
	uint16_t uiCrcValue = PRESET_VALUE;
	
	for(ucI = 0; ucI < length; ucI++) 
	{
		uiCrcValue = uiCrcValue ^ *(dest + ucI);
		for(ucJ = 0; ucJ < 8; ucJ++)
		{
			if(uiCrcValue & 0x0001)
			{ 
				uiCrcValue = (uiCrcValue >> 1) ^ POLYNOMIAL;
			}
			else
			{
				uiCrcValue = (uiCrcValue >> 1);
			}
		}
	}
	return uiCrcValue;
}


/**
  * @}
  */
/**************************************END OF FILE****************************/


