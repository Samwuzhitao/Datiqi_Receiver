/**
  ******************************************************************************
  * @file   	rc500_handle_layer.c
  * @author  	Tian erjun
  * @version 	V1.0.0.0
  * @date   	2015.11.05
  * @brief
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#define RC500_DATIQI_FUNCHTION	(1)

extern uint8_t serial_cmd_status ;
extern WhiteList_Typedef wl;

uint8_t ForceReadCCfile (void);
uint8_t FindICCard(void);
uint8_t SelectApplication(void);
uint8_t SendInterrupt(void);
uint8_t ReadCCfile(uint8_t *CCfileData, uint8_t *CCfileLen);
uint8_t ReadSystemfile(uint8_t *Systemfile_Data, uint8_t *Systemfile_len);
uint8_t ReadNDEFfile(uint8_t *NDEFfile_Data, uint16_t *NDEFfile_len);
uint8_t WriteNDEFfile(uint8_t *pDataToWrite);
uint8_t Deselect(void);

/*********************************************************************************
* ��	�ܣ�void AnswerErr(int faultcode)
* ��  ��:answerdata = Ӧ������
*        answerlen  = ���ݳ���
* ��	�أ�NULL
* ��	ע:��ȷִ������λ��ָ��з�������
*********************************************************************************/
void AnswerOk(uint8_t *answerdata, unsigned int answerlen)
{
	if(serial_cmd_status == 0)
	{

	}
}

/*********************************************************************************
* ��	�ܣ�void AnswerErr(int faultcode)
* ��  ��:
* ��	�أ�NULL
* ��	ע����λ��ָ��ִ�г���
*********************************************************************************/
void AnswerErr(int faultcode)
{
	if(serial_cmd_status == 0)
	{

	}
}

/*********************************************************************************
* ��	�ܣ�void ComSelectApplication(void)
* ��    ��: NULL
* ��	�أ�MI_OK : the function is succesful.
* ��	ע��
*********************************************************************************/
void ComSelectApplication (void)
{
	uint8_t len = 0;
	uint8_t status = 0;
	memset(respon, 0, BUF_LEN);
	if( status = PcdRATS(respon, &len), ( (MI_OK == status) && (respon[0] == 0x05) && (respon[1] == 0x78) && (respon[4] == 0x02) ) )
	{
		memset(respon, 0, BUF_LEN);
		if( status = PcdPPS(respon, &len), ( (MI_OK == status) && (respon[0] == 0xD0) ) )
		{
			memset(respon, 0, BUF_LEN);
			if( status = PcdSelectApplication(respon, &len), ( (MI_OK == status) && (respon[1] == 0x90) && (respon[2] == 0x00) ) )
			{
				AnswerOk(&respon[1], len - 1);
			}
			else
			{	AnswerErr(11); 		}
		}
		else
		{	AnswerErr(11); 	}
	}
	else
	{    AnswerErr(11);   }
}

/*********************************************************************************
* ��	�ܣ�void ComSendInterrupt (void)
* ��    ��: NULL
* ��	�أ�MI_OK : the function is succesful.
* ��	ע��
*********************************************************************************/
void ComSendInterrupt (void)
{
	uint8_t len = 0;
	uint8_t status = 0;
	memset(respon, 0, BUF_LEN);
	if( status = ForceReadCCfile(), (MI_OK == status) )
	{
		if( status = PcdSelectSystemfile(respon, &len), ( (MI_OK == status) && (respon[1] == 0x90) && (respon[2] == 0x00) ) )
		{
			memset(respon, 0, BUF_LEN);
			if( status = PcdReadSystemfile(0x0004, 0x01, respon, &len), ( (MI_OK == status) && ((respon[1] & 0x40 ) == 0x40) ) )
			{
				memset(respon, 0, BUF_LEN);
				if( status = PcdSendInterrupt(respon, &len), ( (MI_OK == status) && (respon[1] == 0x90) && (respon[2] == 0x00) ) )
				{
					AnswerOk(&respon[1], len - 1);
				}
				else
				{
					AnswerErr(11);
				}
			}
			else
			{
				AnswerErr(11);
			}
		}
		else
		{
			AnswerErr(11);
		}
	}
	else
	{
		AnswerErr(11);
	}
}

/*********************************************************************************
* ��	�ܣ�uint8_t ForceReadCCfile (void)
* ��    ��: NULL
* ��	�أ�MI_OK : the function is succesful.
* ��	ע��
*********************************************************************************/
uint8_t ForceReadCCfile (void)
{
	uint8_t len = 0;
	uint16_t NDEF_DataRead = 0;
	uint8_t status = 0;
	memset(respon, 0, BUF_LEN);
	if( status = PcdSelectCCfile(respon, &len), ( (MI_OK == status) && (respon[1] == 0x90) && (respon[2] == 0x00) ) )
	{
		memset(respon, 0, BUF_LEN);
		if( status = PcdReadCCfileLength(respon, &len), (MI_OK == status) )
		{
			NDEF_DataRead = (uint16_t)( (respon[1] << 8) | (respon[2]) );
			memset(respon, 0, BUF_LEN);
			if( status = PcdReadCCfile(0x0000, NDEF_DataRead, respon, &len), (MI_OK == status) )
			{
				return (MI_OK);
			}
			else
			{
				return (1);
			}
		}
		else
		{
			return (1);
		}
	}
	else
	{
		return (1);
	}
}

/*********************************************************************************
* ��	�ܣ�void ComReadCCfile (void)
* ��    ��: NULL
* ��	�أ�MI_OK : the function is succesful.
* ��	ע��
*********************************************************************************/
void ComReadCCfile (void)
{
	uint8_t len = 0;
	uint16_t NDEF_DataRead = 0;
	uint8_t status = 0;
	memset(respon, 0, BUF_LEN);
	if( status = PcdSelectCCfile(respon, &len), ( (MI_OK == status) && (respon[1] == 0x90) && (respon[2] == 0x00) ) )
	{
		memset(respon, 0, BUF_LEN);
		if( status = PcdReadCCfileLength(respon, &len), (MI_OK == status) )
		{
			NDEF_DataRead = (uint16_t)( (respon[1] << 8) | (respon[2]) );
			memset(respon, 0, BUF_LEN);
			if( status = PcdReadCCfile(0x0000, NDEF_DataRead, respon, &len), (MI_OK == status) )
			{
				AnswerOk(&respon[1], len - 1);
			}
			else
			{
				AnswerErr(11);
			}
		}
		else
		{
			AnswerErr(11);
		}
	}
	else
	{
		AnswerErr(11);
	}
}

/*********************************************************************************
* ��	�ܣ�void ComReadSystemfile (void)
* ��    ��: NULL
* ��	�أ�MI_OK : the function is succesful.
* ��	ע��
*********************************************************************************/
void ComReadSystemfile (void)
{
	uint8_t len = 0;
	uint16_t NDEF_DataRead = 0;
	uint8_t status = 0;
	if( status = ForceReadCCfile(), (MI_OK == status) )
	{
		memset(respon, 0, BUF_LEN);
		if( status = PcdSelectSystemfile(respon, &len), ( (MI_OK == status) && (respon[1] == 0x90) && (respon[2] == 0x00) ) )
		{
			memset(respon, 0, BUF_LEN);
			if( status = PcdReadSystemfileLength(respon, &len), (MI_OK == status) )
			{
				NDEF_DataRead = (uint16_t)( (respon[1] << 8) | (respon[2]) );
				memset(respon, 0, BUF_LEN);
				if( status = PcdReadSystemfile(0x0000, NDEF_DataRead, respon, &len), (MI_OK == status) )
				{
					AnswerOk(&respon[1], len - 1);
				}
				else
				{
					AnswerErr(11);
				}
			}
			else
			{
				AnswerErr(11);
			}
		}
		else
		{
			AnswerErr(11);
		}
	}
	else
	{
		AnswerErr(11);
	}
}

/*********************************************************************************
* ��	�ܣ�void ComReadNDEFfile (void)
* ��    ��: NULL
* ��	�أ�MI_OK : the function is succesful.
* ��	ע��
*********************************************************************************/
void ComReadNDEFfile (void)
{
	uint8_t len = 0;
	uint16_t NDEF_DataRead_Len = 0;
	uint8_t status = 0;
	if( status = ForceReadCCfile(), (MI_OK == status) )
	{
		memset(respon, 0, BUF_LEN);
		if( status = PcdSelectNDEFfile(respon, &len), ( (MI_OK == status) && (respon[1] == 0x90) && (respon[2] == 0x00) ) )
		{
			memset(respon, 0, BUF_LEN);
			if( status = PcdReadNDEFfileLength(respon, &len), (MI_OK == status) )
			{
				NDEF_DataRead_Len = ((uint16_t)respon[1] << 8) | respon[2] ;
				memset(respon, 0, BUF_LEN);
				if( status = PcdReadNDEFfile(0x0000, NDEF_DataRead_Len + 2, respon, &len), (MI_OK == status) )
				{
					AnswerOk(&respon[1], len - 1);
				}
				else
				{
					AnswerErr(11);
				}
			}
			else
			{
				AnswerErr(11);
			}
		}
		else
		{
			AnswerErr(11);
		}
	}
	else
	{
		AnswerErr(11);
	}
}

/*********************************************************************************
* ��	�ܣ�void ComWriteNDEFfile (uint16_t NbByteToWrite, uint8_t *pDataToWrite)
* ��    ��: NULL
* ��	�أ�MI_OK : the function is succesful.
* ��	ע��
*********************************************************************************/
void ComWriteNDEFfile (uint16_t NbByteToWrite, uint8_t *pDataToWrite)
{
	uint8_t len = 0, FWTbyte = 0;
	uint8_t EraseLen[2] = {0x00, 0x00};
	uint8_t status = 0;
	if( status = ForceReadCCfile(), (MI_OK == status) )
	{
		memset(respon, 0, BUF_LEN);
		if( status = PcdSelectNDEFfile(respon, &len), ( (MI_OK == status) && (respon[1] == 0x90) && (respon[2] == 0x00) ) )
		{
			memset(respon, 0, BUF_LEN);
			if( status = PcdWriteNDEFfile(0x0000, 0x02, EraseLen, respon, &len), ( (MI_OK == status) && (respon[1] == 0x90) && (respon[2] == 0x00) ) )
			{
				memset(respon, 0, BUF_LEN);
				status = PcdWriteNDEFfile(0x0002, NbByteToWrite, pDataToWrite, respon, &len);			//??????

				if( (MI_OK == status) && (respon[1] == 0x90) && (respon[2] == 0x00) )					//????????????????
				{
					memset(respon, 0, BUF_LEN);
					pDataToWrite[0] = (uint8_t)( NbByteToWrite >> 8 );
					pDataToWrite[1] = (uint8_t)( NbByteToWrite & 0xFF );

					if( status = PcdWriteNDEFfile(0x0000, 0x02, pDataToWrite, respon, &len), ( (MI_OK == status) && (respon[1] == 0x90) && (respon[2] == 0x00) ))
					{
						AnswerOk(&respon[1], len - 1);
					}
					else
					{
						AnswerErr(11);
					}
				}
				else if((MI_OK == status) && ((respon[0] & 0xC0) == 0xC0) )		//modify by tianerjun, before is respon[0] == 0xF2							//????????????WTX
				{
					FWTbyte = respon[1];
					memset(respon, 0, BUF_LEN);
					if( status = PcdFWTExtension(FWTbyte, respon, &len), ( (MI_OK == status) && (respon[1] == 0x90) && (respon[2] == 0x00)) )
					{
						pDataToWrite[0] = (uint8_t)( NbByteToWrite >> 8 );
						pDataToWrite[1] = (uint8_t)( NbByteToWrite & 0xFF );

						if( status = PcdWriteNDEFfile(0x0000, 0x02, pDataToWrite, respon, &len), ( (MI_OK == status) && (respon[1] == 0x90) && (respon[2] == 0x00) ))
						{
							AnswerOk(&respon[1], len - 1);
						}
						else
						{
							AnswerErr(11);
						}
					}
					else
					{
						AnswerErr(11);
					}
				}
				else
				{
					AnswerErr(11);
				}
			}
			else
			{
				AnswerErr(11);
			}
		}
		else
		{
			AnswerErr(11);
		}
	}
	else
	{
		AnswerErr(11);
	}
}

/*********************************************************************************
* ��	�ܣ�void ComDeselect (void)
* ��    ��: NULL
* ��	�أ�MI_OK : the function is succesful.
* ��	ע��
*********************************************************************************/
void ComDeselect (void)
{
	uint8_t len = 0;
	uint8_t status = 0;
	memset(respon, 0, BUF_LEN);
	if( status = PcdDeselect(respon, &len), ( (MI_OK == status) && (respon[0] == 0xC2) && (respon[1] == 0xE0) && (respon[2] == 0xB4)) )
	{
		AnswerOk(&respon[0], len);
	}
	else
	{
		AnswerErr(11);
	}
}

/*********************************************************************************
* ��	�ܣ�void FindICCard(void)
* ��    ��: NULL
* ��	�أ�MI_OK : the function is succesful.
* ��	ע��
*********************************************************************************/
uint8_t FindICCard(void)
{
	uint8_t len,findIC_flow = 0x01;
	uint8_t status = 0x11;

	while(findIC_flow)
	{
		switch(findIC_flow)
		{
			case 0x01:
				{
					/* ��13.56M���� */
					PcdAntennaOn();
					findIC_flow = 0x02;
				}
				break;

			case 0x02:
				memset(g_cardType, 0, 40);
			  /* reqAָ�� :����A�������ؿ����ͣ���ͬ���Ϳ���Ӧ��ͬ��UID���� */
				if(PcdRequest(PICC_REQIDL,g_cardType) == MI_OK)
				{
					if( (g_cardType[0] & 0x40) == 0x40)
					{	uid_len = 8;	}
					else
					{	uid_len = 4;	}
					findIC_flow = 0x03;
				}
				else
					findIC_flow = 0x00;
				break;

			case 0x03:	//����ײ1
				if(PcdAnticoll(PICC_ANTICOLL1, g_cSNR) == MI_OK )
					findIC_flow = 0x04;
				else
					findIC_flow = 0x02;
				break;

			case 0x04:	//ѡ��1
				memset(respon, 0, BUF_LEN);
				if (MI_OK == PcdSelect1(g_cSNR, respon, &len))
				{
					if((uid_len == 8) && ((respon[0] & 0x04) == 0x04))
						findIC_flow = 0x05;
					else if(uid_len == 4)
						findIC_flow = 0x07;
				}
				else
					findIC_flow = 0x02;
				break;

			case 0x05:	//����ײ2
				memset(respon, 0, BUF_LEN);
				if(MI_OK == PcdAnticoll(PICC_ANTICOLL2, &g_cSNR[4]))
					findIC_flow = 0x06;
				else
					findIC_flow = 0x02;
				break;

			case 0x06:	//ѡ��2
				if((MI_OK == PcdSelect2(&g_cSNR[4], respon, &len))&&((respon[0] & 0x20) == 0x20))
				{
					if(wl.match_status == ON)
						findIC_flow = 0x09;
					else
						findIC_flow = 0x07;
				}
				else
					findIC_flow = 0x02;
				break;

			case 0x07:	//Ѱ���ɹ�
				if(SelectApplication() == MI_OK)		//ѡ��Ӧ��
					findIC_flow = 0x08;
				else
					findIC_flow = 0x02;
				break;

			case 0x08:	//Ѱ���ɹ�
				if(ReadNDEFfile(NDEF_DataWrite, &NDEF_Len) == MI_OK)
					findIC_flow = 0x09;
				else
					findIC_flow = 0x02;
				break;

			case 0x09:	//Ѱ���ɹ�
					findIC_flow = 0x00;
					status = MI_OK;
				break;
		}
	}
	return status;
}
/*********************************************************************************
* ��	�ܣ�void ComSelectApplication(void)
* ��    ��: NULL
* ��	�أ�MI_OK : the function is succesful.
* ��	ע��
*********************************************************************************/
uint8_t SelectApplication (void)
{
	uint8_t len = 0;
	uint8_t status = 0;
	memset(respon, 0, BUF_LEN);
	if( status = PcdRATS(respon, &len), ( (MI_OK == status) && (respon[0] == 0x05) && (respon[1] == 0x78) && (respon[4] == 0x02) ) )
	{
		memset(respon, 0, BUF_LEN);
		if( status = PcdPPS(respon, &len), ( (MI_OK == status) && (respon[0] == 0xD0) ) )
		{
			memset(respon, 0, BUF_LEN);
			if( status = PcdSelectApplication(respon, &len), ( (MI_OK == status) && (respon[1] == 0x90) && (respon[2] == 0x00) ) )
			{	return status;		}
			else
			{	return 0x11; 		}
		}
		else
		{	return 0x11; 	}
	}
	else
	{    return 0x11;   }
}

/*********************************************************************************
* ��	�ܣ�void ComSendInterrupt (void)
* ��    ��: NULL
* ��	�أ�MI_OK : the function is succesful.
* ��	ע��
*********************************************************************************/
uint8_t SendInterrupt (void)
{
	uint8_t len = 0;
	uint8_t status = 0;
	memset(respon, 0, BUF_LEN);
	if( status = ForceReadCCfile(), (MI_OK == status) )
	{
		if( status = PcdSelectSystemfile(respon, &len), ( (MI_OK == status) && (respon[1] == 0x90) && (respon[2] == 0x00) ) )
		{
			memset(respon, 0, BUF_LEN);
			if( status = PcdReadSystemfile(0x0004, 0x01, respon, &len), ( (MI_OK == status) && ((respon[1] & 0x40 ) == 0x40) ) )
			{
				memset(respon, 0, BUF_LEN);
				if( status = PcdSendInterrupt(respon, &len), ( (MI_OK == status) && (respon[1] == 0x90) && (respon[2] == 0x00) ) )
				{	return status;		}
				else
				{	return 0x11; 		}
			}
			else
			{	return 0x11; 	}
		}
		else
		{  	return 0x11;   }
	}
	else
	{  	return 0x11;  }
}

/*********************************************************************************
* ��	�ܣ�void ComReadCCfile (void)
* ��    ��: NULL
* ��	�أ�MI_OK : the function is succesful.
* ��	ע��
*********************************************************************************/
uint8_t ReadCCfile (uint8_t *CCfileData, uint8_t *CCfileLen)
{
	uint8_t len = 0;
	uint16_t NDEF_DataRead = 0;
	uint8_t status = 0;
	memset(respon, 0, BUF_LEN);
	if( status = PcdSelectCCfile(respon, &len), ( (MI_OK == status) && (respon[1] == 0x90) && (respon[2] == 0x00) ) )
	{
		memset(respon, 0, BUF_LEN);
		if( status = PcdReadCCfileLength(respon, &len), (MI_OK == status) )
		{
			NDEF_DataRead = (uint16_t)( (respon[1] << 8) | (respon[2]) );
			memset(respon, 0, BUF_LEN);
			if( status = PcdReadCCfile(0x0000, NDEF_DataRead, respon, &len), (MI_OK == status) )
			{
				memcpy(CCfileData, &respon[1], len - 1);
				*CCfileLen = len - 1;
				return status;
			}
			else
			{	return 0x11; 	}
		}
		else
		{	return 0x11; 	}
	}
	else
	{  	return 0x11;   }
}

/*********************************************************************************
* ��	�ܣ�void ComReadSystemfile (void)
* ��    ��: NULL
* ��	�أ�MI_OK : the function is succesful.
* ��	ע��
*********************************************************************************/
uint8_t ReadSystemfile (uint8_t *Systemfile_Data, uint8_t *Systemfile_len)
{
	uint8_t len = 0;
	uint16_t NDEF_DataRead = 0;
	uint8_t status = 0;
	if( status = ForceReadCCfile(), (MI_OK == status) )
	{
		memset(respon, 0, BUF_LEN);
		if( status = PcdSelectSystemfile(respon, &len), ( (MI_OK == status) && (respon[1] == 0x90) && (respon[2] == 0x00) ) )
		{
			memset(respon, 0, BUF_LEN);
			if( status = PcdReadSystemfileLength(respon, &len), (MI_OK == status) )
			{
				NDEF_DataRead = (uint16_t)( (respon[1] << 8) | (respon[2]) );
				memset(respon, 0, BUF_LEN);
				if( status = PcdReadSystemfile(0x0000, NDEF_DataRead, respon, &len), (MI_OK == status) )
				{
					*Systemfile_len = len - 1;
					memcpy(Systemfile_Data, &respon[1], len - 1);
					return status;
				}
				else
				{	return 0x11; 	}
			}
			else
			{	return 0x11; 	}
		}
		else
		{   return 0x11; 	}
	}
	else
	{ 	return 0x11; 	}
}

/*********************************************************************************
* ��	�ܣ�void ComReadNDEFfile (void)
* ��    ��: NULL
* ��	�أ�MI_OK : the function is succesful.
* ��	ע��
*********************************************************************************/
uint8_t ReadNDEFfile (uint8_t *NDEFfile_Data, uint16_t *NDEFfile_len)
{
	uint8_t len = 0;
	uint16_t NDEF_DataRead_Len = 0;
	uint8_t status = 0;
	if( status = ForceReadCCfile(), (MI_OK == status) )
	{
		memset(respon, 0, BUF_LEN);
		if( status = PcdSelectNDEFfile(respon, &len), ( (MI_OK == status) && (respon[1] == 0x90) && (respon[2] == 0x00) ) )
		{
			memset(respon, 0, BUF_LEN);
			if( status = PcdReadNDEFfileLength(respon, &len), (MI_OK == status) )
			{
				NDEF_DataRead_Len = ((uint16_t)respon[1] << 8) | respon[2] ;
				memset(respon, 0, BUF_LEN);
				if( status = PcdReadNDEFfile(0x0000, NDEF_DataRead_Len + 2, respon, &len), (MI_OK == status) )
				{
					memcpy(NDEFfile_Data, &respon[1], NDEF_DataRead_Len + 2);
					*NDEFfile_len = NDEF_DataRead_Len + 2;
					return status;
				}
				else
				{	return 0x11; }
			}
			else
			{	return 0x11; }
		}
		else
		{  	return 0x11; }
	}
	else
	{ 	return 0x11; }
}

/*********************************************************************************
* ��	�ܣ�void ComWriteNDEFfile (uint16_t NbByteToWrite, uint8_t *pDataToWrite)
* ��    ��: NULL
* ��	�أ�MI_OK : the function is succesful.
* ��	ע��
*********************************************************************************/
uint8_t WriteNDEFfile (uint8_t *pDataToWrite)
{
	uint8_t len = 0, FWTbyte = 0;
	uint8_t EraseLen[2] = {0x00, 0x00};
	uint8_t status = 0;
	uint16_t NbByteToWrite = ((uint16_t)*pDataToWrite << 8) | *(pDataToWrite + 1);
	if( status = ForceReadCCfile(), (MI_OK == status) )
	{
		memset(respon, 0, BUF_LEN);
		if( status = PcdSelectNDEFfile(respon, &len), ( (MI_OK == status) && (respon[1] == 0x90) && (respon[2] == 0x00) ) )
		{
			memset(respon, 0, BUF_LEN);
			if( status = PcdWriteNDEFfile(0x0000, 0x02, EraseLen, respon, &len), ( (MI_OK == status) && (respon[1] == 0x90) && (respon[2] == 0x00) ) )
			{
				memset(respon, 0, BUF_LEN);
				status = PcdWriteNDEFfile(0x0002, NbByteToWrite, (pDataToWrite + 2), respon, &len);
//				status = PcdWriteNDEFfile(0x0000, NbByteToWrite, pDataToWrite, respon, &len);
				app_debuglog_dump(respon, len);

				if( (MI_OK == status) && (respon[1] == 0x90) && (respon[2] == 0x00) )
				{
					memset(respon, 0, BUF_LEN);
					if( status = PcdWriteNDEFfile(0x0000, 0x02, pDataToWrite, respon, &len), ( (MI_OK == status) && (respon[1] == 0x90) && (respon[2] == 0x00) ))
					{	return status;	}
					else
					{	return 0x11; 	}
				}
				else if((MI_OK == status) && ((respon[0] & 0xC0) == 0xC0) )		//modify by tianerjun, before is respon[0] == 0xF2
				{
					FWTbyte = respon[1];
					memset(respon, 0, BUF_LEN);
					if( status = PcdFWTExtension(FWTbyte, respon, &len), ( (MI_OK == status) && (respon[1] == 0x90) && (respon[2] == 0x00)) )
					{
						if( status = PcdWriteNDEFfile(0x0000, 0x02, pDataToWrite, respon, &len), ( (MI_OK == status) && (respon[1] == 0x90) && (respon[2] == 0x00) ))
						{	return status;		}
						else
						{	return 0x11; 		}
					}
					else
					{	return 0x11;	}
				}
				else
				{	return 0x11;	}
			}
			else
			{    return 0x11; 	}
		}
		else
		{  	return 0x11; 	}
	}
	else
	{  	return 0x11;  }
}

/*********************************************************************************
* ��	�ܣ�void ComDeselect (void)
* ��    ��: NULL
* ��	�أ�MI_OK : the function is succesful.
* ��	ע��
*********************************************************************************/
uint8_t Deselect (void)
{
	uint8_t len = 0;
	uint8_t status = 0;
	memset(respon, 0, BUF_LEN);

	if( status = PcdDeselect(respon, &len), ( (MI_OK == status) && (respon[0] == 0xC2) && (respon[1] == 0xE0) && (respon[2] == 0xB4)) )
	{  	return status;	}
	else
	{   return 0x11;  	}
}
/**************************************END OF FILE****************************/
