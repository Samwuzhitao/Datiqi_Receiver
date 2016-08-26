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
static void sendtoRC500(void);
void ComPcdFindCard(void);
void ComPcdAntenna(void);  
uint8_t ComRequestA(void);
uint8_t ComAnticoll(void);
uint8_t ComAnticol2(void);
void ComSelect1(void);
void ComSelect2(void);
void ComHlta(void);
void ComAuthentication(void);
void ComM1Read(void);
void ComM1Write(void);
void ComM1Initval(void);
void ComM1Readval(void);
void ComM1Decrement(void);
void ComM1Increment(void);
void AnswerErr(int faultcode); 
void AnswerOk(uint8_t *answerdata,unsigned int answernum);
void AnswerCommandOk(void);
void DoubleModeCtrRC500(void);
void ComM1BakValue(void);
void ComUL_PcdAnticoll(void);
void ComUL_PcdWrite(void);
void ComTypeARst(void);
void ComTCLCOS(void);
void ComCL_Deselect(void);
void ComSelectApplication(void);
void ComSendInterrupt (void);
uint8_t ForceReadCCfile (void);
void ComReadCCfile (void);
void ComReadSystemfile (void);
void ComReadNDEFfile (void);
void ComWriteNDEFfile (uint16_t NbByteToWrite, uint8_t *pDataToWrite);
void ComDeselect (void);
void RC500_FindCard(void);

uint8_t FindICCard(void);
uint8_t SelectApplication(void);
uint8_t SendInterrupt(void);
uint8_t ReadCCfile(uint8_t *CCfileData, uint8_t *CCfileLen);
uint8_t ReadSystemfile(uint8_t *Systemfile_Data, uint8_t *Systemfile_len);
uint8_t ReadNDEFfile(uint8_t *NDEFfile_Data, uint16_t *NDEFfile_len);
uint8_t WriteNDEFfile(uint8_t *pDataToWrite);
uint8_t Deselect(void);

void rc500_handle_layer(void)
{
	
	sendtoRC500();

}
void write_RF_config(void)
{
//	uint8_t NDEF_Data[BUF_LEN] = {0};
//	uint16_t NDEF_Len;
	
	if( FindICCard() == MI_OK)		//Ѱ���ɹ�
	{
		DebugLog("[FindICCard]:UID is ");
		app_debuglog_dump(g_cSNR, 8);

		if(SelectApplication() == MI_OK)		//ѡ��Ӧ��
		{
			DebugLog("[WriteNDEFfile]:nrf_parameter is ");
//			app_debuglog_dump((uint8_t *)&answer_setting,sizeof(answer_setting));
//			if(WriteNDEFfile((uint8_t *)&answer_setting) == MI_OK)		//д��NDEF�ļ�
//			{
//				time_for_buzzer_on = 10;
//				time_for_buzzer_off = 300;
//				if(ReadNDEFfile(NDEF_Data, &NDEF_Len) == MI_OK)			//������֤
//				{
//					DebugLog("[ReadNDEFfile]:NDEF_Data is ");
//					app_debuglog_dump(NDEF_Data, NDEF_Len);
//				}
//			}
			Deselect();	//ȥ��ѡ��
			PcdHalt();
		}
	}
}

static void sendtoRC500(void)
{
	uint8_t  temp_cmd;

	uint8_t len = 0;
	uint16_t tmp_read_len = 0;
		
	if(App_to_CtrRC500Req)
	{ 
		temp_cmd = Buf_AppToCtrRC500[3];

		switch(temp_cmd)  
		{
			case 0x35:
				ComRequestA();
			break;
			case 0x36:		// ���ߴ� or �ر�
				ComPcdAntenna();
			break;             
			case 0x37:		// 13.56MѰ������
				ComPcdFindCard();
			break;  
			case 0x38:		// A������ײ����
				ComAnticoll();
			break;        
			case 0x39:		// A������
				ComSelect1();
			break;
			case 0x30:		// A������ײ����
				ComAnticol2();
			break;
			case 0x31:		// A����������
				ComSelect2();
			break;
			case 0x29:		// A����������
				ComHlta();
			break;
			case 0x4A:		// A����֤��Կ
				ComAuthentication();
			break; 
			case 0x4B:		// ��M1��
				ComM1Read(); 
			break;
			case 0x4C:		// дM1��
				ComM1Write();
			break;
			case 0x4D:		// ��ʼ��Ǯ��
				ComM1Initval();
			break; 
			case 0x4E:		// ��Ǯ��
				ComM1Readval();
			break;
			case 0x4F:		// �ۿ�
				ComM1Decrement();
			break;
			case 0x50:		// ��ֵ
				ComM1Increment();
			break;
			case 0x51:		// ����Ǯ��
				ComM1BakValue();
			break;
			case 0x52:		// UltraLght������ײ
				ComUL_PcdAnticoll();
			break;      
			case 0x53:		// дUltraLight��
				ComUL_PcdWrite();
			break;  
			case 0x54:		// Mifare_Pro(X)���߼���λ
				ComTypeARst();
			break; 
			case 0x55:		// T=CL��COS����
				ComTCLCOS();
			break; 
			case 0x56:		// T=CL���˳�����״̬
				ComCL_Deselect();
			break; 
			case 0x57:		// ѡ��Ӧ��
				ComSelectApplication();
			break;
			case 0x58:		// �����ж�
				ComSendInterrupt();
			break;
			case 0x59:		// ��NDEF�ļ�
				ComReadNDEFfile();			
			break;
			case 0x5a:		// дNDEF�ļ�
				ComWriteNDEFfile( (((uint16_t)Buf_AppToCtrRC500[4] << 8) | Buf_AppToCtrRC500[5]), &Buf_AppToCtrRC500[6]);			
			break;
			case 0x5b:		// �� CC �ļ�
				ComReadCCfile();
			break;
			case 0x5c:		// ��ϵͳ�ļ�
				ComReadSystemfile();
			break;
			case 0x5d:		// ȥ��ѡ��
				ComDeselect();
			break;
					 
////////////////////////////////////   NFC�������  ////////////////////////////////////////////////					 
			case 0x70:		// Request for Answer To Select, Type A
				memset(g_cardType, 0, BUF_LEN);
				PcdRATS(respon, &len);
				memcpy(g_cardType, respon, len);
				flag_App_or_Ctr = 0xee;	
			break;

			case 0x71:		// Protocol and Parameter Selection
				memset(g_cardType, 0, BUF_LEN);
				PcdPPS(respon, &len);
				memcpy(g_cardType, respon, len);
				flag_App_or_Ctr = 0xee;	
			break;

			case 0x72:		// ѡ��M24SRӦ��
				memset(g_cardType, 0, BUF_LEN);
				PcdSelectApplication(respon, &len);
				memcpy(g_cardType, respon, len);
				flag_App_or_Ctr = 0xee;
			break;	

			case 0x73:		// ѡ��CC�ļ�
				memset(g_cardType, 0, BUF_LEN);
				PcdSelectCCfile(respon, &len);
				memcpy(g_cardType, respon, len);
				flag_App_or_Ctr = 0xee;	
			break;

			case 0x74:		// ��ȡCC�ļ�����
				memset(g_cardType, 0, BUF_LEN);
				PcdReadCCfileLength(respon, &len);
				memcpy(g_cardType, respon, len);
				flag_App_or_Ctr = 0xee;	
			break;

			case 0x75:		// ��ȡָ������CC�ļ�
				memset(g_cardType, 0, BUF_LEN);
				PcdReadCCfile(0x0002, 0x0d, respon, &len);
				memcpy(g_cardType, respon, len);
				flag_App_or_Ctr = 0xee;	
			break;

			case 0x76:		// ѡ��NDEF�ļ�
				memset(g_cardType, 0, BUF_LEN);
				PcdSelectNDEFfile(respon, &len);
				memcpy(g_cardType, respon, len);
				flag_App_or_Ctr = 0xee;	
			break;

			case 0x77:		// ��ȡNDEF�ļ�����
				memset(g_cardType, 0, BUF_LEN);
				PcdReadNDEFfileLength(respon, &len);
				memcpy(g_cardType, respon, len);
				flag_App_or_Ctr = 0xee;	
			break;

			case 0x78:		// ��ȡָ������NDEF�ļ�
				memset(g_cardType, 0, BUF_LEN);
				PcdReadNDEFfileLength(respon, &len);
				tmp_read_len = ((uint16_t)respon[1] << 8) | (respon[2]);
				PcdReadNDEFfile(0x0000, tmp_read_len + 2, respon, &len);
				memcpy(g_cardType, respon, len);
				flag_App_or_Ctr = 0xee;	
			break;

			case 0x79:		// ѡ��ϵͳ�ļ�
				memset(g_cardType, 0, BUF_LEN);
				PcdSelectSystemfile(respon, &len);
				memcpy(g_cardType, respon, len);
				flag_App_or_Ctr = 0xee;	
			break;

			case 0x7a:		// ��ȡϵͳ�ļ�����
				memset(g_cardType, 0, BUF_LEN);
				PcdReadSystemfileLength(respon, &len);
				memcpy(g_cardType, respon, len);
				flag_App_or_Ctr = 0xee;	
			break;
	
			case 0x7b:		// ��ȡָ������ϵͳ�ļ�
				memset(g_cardType, 0, BUF_LEN);
				PcdReadSystemfile(0x0000, 10, respon, &len);
				memcpy(g_cardType, respon, len);
				flag_App_or_Ctr = 0xee;	
			break;

			case 0x7c:		//  �����������ж�
				memset(g_cardType, 0, BUF_LEN);
				PcdSendInterrupt(respon, &len);
				memcpy(g_cardType, respon, len);
				flag_App_or_Ctr = 0xee;	
			break;					 

			case 0x7d:		// д���ݵ�NDEF�ļ�
				memset(g_cardType, 0, BUF_LEN);
				PcdWriteNDEFfile(0x0000, 0x02, NDEF_DataWrite, respon, &len);
				memcpy(g_cardType, respon, len);
				flag_App_or_Ctr = 0xee;	
			break;
			
			case 0x7e:		//ȥ��ѡ��
				memset(g_cardType, 0, BUF_LEN);
				PcdDeselect(respon, &len);
				memcpy(g_cardType, respon, len);
				flag_App_or_Ctr = 0xee;	
			break;
////////////////////////////////////   NFC�������  ////////////////////////////////////////////////					 
			default:
				AnswerErr( 11 );         
			break;
		}
		App_to_CtrRC500Req = false;
	}
}

/////////////////////////////////////////////////////////////////////
//д������������
/////////////////////////////////////////////////////////////////////
/*
void write_RF_config(void)
{
	uint8_t len;
	uint8_t temp[15] = {0};
	uint16_t NDEF_DataRead_Len;
	uint8_t Step = 1;
	do{
		switch(Step)
		{
			case 0x01:	// ������
				PcdAntennaOn();												//��13.56M����
				Step = 0x02;
				break;
			case 0x02:	//����reqAָ��
				memset(g_cardType, 0, 40);
				if(PcdRequest(PICC_REQIDL,g_cardType) == MI_OK)				//����A�������ؿ����ͣ���ͬ���Ϳ���Ӧ��ͬ��UID����
				{	
					if( (g_cardType[0] & 0x40) == 0x40)
					{	uid_len = 8;	}
					else
					{	uid_len = 4;	}
					Step = 0x03;
				}
				break;
			case 0x03:	//����ײ1
				if(PcdAnticoll(PICC_ANTICOLL1, g_cSNR) == MI_OK )
					Step = 0x04;
				else
					Step = 0x02;
			break;
			case 0x04:	//ѡ��1	
				memset(respon, 0, BUF_LEN);
				if (MI_OK == PcdSelect1(g_cSNR, respon, &len))				
				{
					if((uid_len == 8) && ((respon[0] & 0x04) == 0x04))
						Step = 0x05;
					else
						Step = 0x07;
				}
				else
					Step = 0x02;
			break;
			case 0x05:	//����ײ2
				memset(respon, 0, BUF_LEN);
				if(MI_OK == PcdAnticoll(PICC_ANTICOLL2, &g_cSNR[4]))		
				{
					Step = 0x06;
				}
				else
					Step = 0x02;		
			break;
			case 0x06:	//ѡ��2
				if((MI_OK == PcdSelect2(&g_cSNR[4], respon, &len))&&((respon[0] & 0x20) == 0x20))
				{
					Step = 0x08;
				}
				else
					Step = 0x02;		
			break;
			case 0x07:	//4λ�������
				DebugLog("[RC500_FindCard]:UID is ");
				sprintf((char *)temp, "%010u [����]", (g_cSNR[0] << 24) | (g_cSNR[1] << 16) | (g_cSNR[2] << 8) | (g_cSNR[3] << 0));
				DebugLog("%s or ", temp);
				sprintf((char *)temp, "%010u [����]", (g_cSNR[3] << 24) | (g_cSNR[2] << 16) | (g_cSNR[1] << 8) | (g_cSNR[0] << 0));
				DebugLog("%s\r\n", temp);	
				time_for_buzzer_switch = 10;
				Step = 0x09;
			break;
			case 0x08:	//8λ�������
				DebugLog("[RC500_FindCard]:UID is ");
				app_debuglog_dump_no_space(g_cSNR, 8);
				time_for_buzzer_switch = 10;
				Step = 0x09;
			break;
			case 0x09:	//������
				PcdHalt();												//ʹ�������ŵĿ���������
				Step = 0x00;
			break;
			
			case 0x10:	//�������ɽ�������Ϊ48byte
				memset(respon, 0, BUF_LEN);
				if((MI_OK == PcdRATS(respon, &len)) && (respon[0] == 0x05) && (respon[1] == 0x78) && (respon[4] == 0x02))
					Step = 0x11;
				else
					Step = 0x02;	
			break;
			case 0x11:	//���ô�������
				memset(respon, 0, BUF_LEN);
				if((MI_OK == PcdPPS(respon, &len)) && (respon[0] == 0xD0))
					Step = 0x12;
				else
					Step = 0x02;	
			break;
			case 0x12:	//ѡ��ϵͳӦ��
				memset(respon, 0, BUF_LEN);
				if((MI_OK == PcdSelectApplication(respon, &len)) && (respon[1] == 0x90) && (respon[2] == 0x00))
					Step = 0x08;
				else
					Step = 0x02;	
			break;
			
			case 0x20:	//ǿ�ƶ�ȡCC file
				if(ForceReadCCfile() == MI_OK)
					Step = 0x21;
				else
					Step = 0x02;
			break;
			case 0x21:	//ѡ��NDEF�ļ�
				memset(respon, 0, BUF_LEN);
				if((MI_OK == PcdSelectNDEFfile(respon, &len)) && (respon[1] == 0x90) && (respon[2] == 0x00)) 
					Step = 0x22;
				else
					Step = 0x02;
			break;
			case 0x22:	//��ȡNDEF�ļ����ȼ�����
				memset(respon, 0, BUF_LEN);
				if(PcdReadNDEFfileLength(respon, &len) == MI_OK)
				{
					NDEF_DataRead_Len = ((uint16_t)respon[1] << 8) | respon[2] ;
					memset(respon, 0, BUF_LEN);
					if(PcdReadNDEFfile(0x0000, NDEF_DataRead_Len + 2, respon, &len) == MI_OK)
						Step = 0x23;
					else
						Step = 0x02;
				}
				else
					Step = 0x02;
			break;
			case 24:
				memset(respon, 0, BUF_LEN);
				if( status = PcdWriteNDEFfile(0x0000, 0x02, EraseLen, respon, &len), ( (MI_OK == status) && (respon[1] == 0x90) && (respon[2] == 0x00) ) )
					Step = 0x22;
				else
					Step = 0x02;	
			
			case 0x23:	//дNDEF�ļ�
				ComWriteNDEFfile( (((uint16_t)Buf_AppToCtrRC500[4] << 8) | Buf_AppToCtrRC500[5]), &Buf_AppToCtrRC500[6]);
			break;
			case 0x30:	//ȥ��ѡ��
				ComDeselect();
			break;
			case 0xFF:
				PcdAntennaOff();											//�ر�13.56M����
				Step = 0x00;
			break;
			default	:
				Step = 0x00;
		}
	}while(step);
}

*/

/////////////////////////////////////////////////////////////////////
//��ӦӦ�ò��Ѱ������
/////////////////////////////////////////////////////////////////////
void ComPcdFindCard(void)
{
	uint8_t len,findIC_flow = 0x01;
	
	while(findIC_flow)
	{
		switch(findIC_flow)
		{
			case 0x01:	// ������
				PcdAntennaOn();												//��13.56M����
				findIC_flow = 0x02;
				break;
			
			case 0x02:	//����reqAָ��
				memset(g_cardType, 0, 40);
				if(PcdRequest(PICC_REQIDL,g_cardType) == MI_OK)				//����A�������ؿ����ͣ���ͬ���Ϳ���Ӧ��ͬ��UID����
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
				{
					findIC_flow = 0x06;
				}
				else
					findIC_flow = 0x02;		
				break;
				
			case 0x06:	//ѡ��2
				if((MI_OK == PcdSelect2(&g_cSNR[4], respon, &len))&&((respon[0] & 0x20) == 0x20))
				{
					findIC_flow = 0x07;
				}
				else
					findIC_flow = 0x02;		
				break;
				
			case 0x07:	//Ѱ���ɹ�
				flag_App_or_Ctr = 0x09;							//Ѱ���ɹ�
				if(flag_upload_uid_once)						//�����λ�����ϴ����ű�־�����ÿ����ߣ�����ֻ�ϴ�һ��
				{
					flag_upload_uid_once = false;
					findIC_flow = 0x08;
				}
				else
					findIC_flow = 0x00;				
				break;
				
			case 0x08:	//������
				PcdHalt();												//ʹ�������ŵĿ���������
				findIC_flow = 0x00;
				break;
			
			default	:
				findIC_flow = 0x00;
		}
	}
}
/////////////////////////////////////////////////////////////////////
//��Ӧ��λ�����͵���������
/////////////////////////////////////////////////////////////////////
void ComPcdAntenna(void)
{
    if (!Buf_AppToCtrRC500[4])
    {   PcdAntennaOff();   }
    else
    {   PcdAntennaOn();    }
    AnswerCommandOk();  
}

/////////////////////////////////////////////////////////////////////
//��Ӧ��λ�����͵�ѰA������
/////////////////////////////////////////////////////////////////////
uint8_t ComRequestA(void)
{
    uint8_t atq[2];
    uint8_t status;
	
	memset(g_cardType, 0, 40);
	status = PcdRequest(Buf_AppToCtrRC500[4],atq);
	g_cardType[0] = atq[0];
	g_cardType[1] = atq[1];
	if( (g_cardType[0] & 0x40) == 0x40)
	{
		uid_len = 8;
	}
	else
	{
		uid_len = 4;
	}
	
//	memcpy(&g_cSNR[8], g_cardType, 2);
	
	return (status); 	
}  

/////////////////////////////////////////////////////////////////////
//��Ӧ��λ�����͵�A������ײ����
/////////////////////////////////////////////////////////////////////
uint8_t ComAnticoll(void)
{ 
	uint8_t status;
	status = PcdAnticoll(PICC_ANTICOLL1, g_cSNR);
//	memcpy(&g_cardType[2], g_cSNR, 4);
    return(status) ;	
}

/////////////////////////////////////////////////////////////////////
//��Ӧ��λ�����͵�A������ײ����
/////////////////////////////////////////////////////////////////////
uint8_t ComAnticol2(void)
{ 
	uint8_t status;
	status = PcdAnticoll(PICC_ANTICOLL2, &g_cSNR[4]);
//	memset(g_cardType, 0, 20);
//	memcpy(&g_cardType[6], &g_cSNR[4], 4);
    return(status) ;	
}

/////////////////////////////////////////////////////////////////////
//��Ӧ��λ�����͵�A����������
/////////////////////////////////////////////////////////////////////
void ComSelect1(void)
{
	uint8_t len = 0;
    if (MI_OK == PcdSelect1(g_cSNR, respon, &len))
    {    
//		memcpy(&g_cSNR[10], respon, 1);
//		memcpy(&g_cardType[10], respon, 1);
        AnswerOk(respon, len);   
    }
    else
    {    
		AnswerErr(11);   
	}    	
}  

/////////////////////////////////////////////////////////////////////
//��Ӧ��λ�����͵�A����������
/////////////////////////////////////////////////////////////////////
void ComSelect2(void)
{
	uint8_t len = 0;
    if (MI_OK == PcdSelect2(&g_cSNR[4], respon, &len))
    {    
//		memcpy(&g_cSNR[15], respon, 1);
//		memcpy(&g_cardType[15], respon, 1);
        AnswerOk(respon, len);   
    }
    else
    {    
		AnswerErr(11);     	
	}
}  


/////////////////////////////////////////////////////////////////////
//��Ӧ��λ�����͵�A����������
/////////////////////////////////////////////////////////////////////
void ComHlta(void)
{
    if (MI_OK == PcdHalt())
    {   
		AnswerCommandOk();  
    }
    else
    {    AnswerErr(11);   }    	
}

/////////////////////////////////////////////////////////////////////
//��Ӧ��λ�����͵�A����֤��Կ����
//Buf_AppToCtrRC500[4]:������֤��ʽ
//Buf_AppToCtrRC500[5]:Ҫ��֤�Ŀ��
//Buf_AppToCtrRC500[6]6�ֽ���Կ
/////////////////////////////////////////////////////////////////////
void ComAuthentication(void)
{	
    uint8_t status;
    if (MI_OK == ChangeCodeKey(&Buf_AppToCtrRC500[6],&Buf_AppToCtrRC500[12]))                       //ת����Կ��ʽ
    {    
    	if (MI_OK == PcdAuthKey(&Buf_AppToCtrRC500[12]))                                     //������Կ��RC500FIFO
        {
             status = PcdAuthState(Buf_AppToCtrRC500[4], Buf_AppToCtrRC500[5], g_cSNR);     //��֤��Կ
        }
    }
    if (status == MI_OK)
    {   
	   AnswerCommandOk();  
    }
    else
    {    
	    AnswerErr(11); 
    }
}    

/////////////////////////////////////////////////////////////////////
//��Ӧ��λ����M1������
/////////////////////////////////////////////////////////////////////
void ComM1Read(void)
{
    if (MI_OK == PcdRead(Buf_AppToCtrRC500[4], &Buf_CtrRC500return[0]))
    {	 AnswerOk(&Buf_CtrRC500return[0], 16);  }
    else
    {    AnswerErr(11);            }
}

/////////////////////////////////////////////////////////////////////
//��Ӧ��λ��дM1������
//Buf_AppToCtrRC500[4]:Ҫд�����ݵĿ��
//Buf_AppToCtrRC500[5]:����16�ֽ�Ҫд�������
/////////////////////////////////////////////////////////////////////
void ComM1Write(void)
{
    if (MI_OK == PcdWrite(Buf_AppToCtrRC500[4], &Buf_AppToCtrRC500[5]))
    {    AnswerCommandOk();    }
    else
    {    AnswerErr(11);   }
}

/////////////////////////////////////////////////////////////////////
//��Ӧ��λ����ʼ��Ǯ������
//g_cReceBuf[4]:���
//g_cReceBuf[5]:4�ֽڳ�ʼֵ�����ֽ���ǰ
//Ǯ����ʽ��4�ֽ�Ǯ��ֵ��4�ֽ�Ǯ��ֵȡ����4�ֽ�Ǯ��ֵ��1�ֽڿ�ţ�1�ֽڿ��ȡ����1�ֽڿ�ţ�1�ֽڿ��ȡ��
/////////////////////////////////////////////////////////////////////
void ComM1Initval(void)
{
//    Buf_AppToCtrRC500[5]=0;
//    Buf_AppToCtrRC500[6]=0;
//    Buf_AppToCtrRC500[7]=0;
//    Buf_AppToCtrRC500[8]=0;

    Buf_AppToCtrRC500[9]=~Buf_AppToCtrRC500[5];
    Buf_AppToCtrRC500[10]=~Buf_AppToCtrRC500[6];
    Buf_AppToCtrRC500[11]=~Buf_AppToCtrRC500[7];
    Buf_AppToCtrRC500[12]=~Buf_AppToCtrRC500[8];
    
    Buf_AppToCtrRC500[13]=Buf_AppToCtrRC500[5];
    Buf_AppToCtrRC500[14]=Buf_AppToCtrRC500[6];
    Buf_AppToCtrRC500[15]=Buf_AppToCtrRC500[7];
    Buf_AppToCtrRC500[16]=Buf_AppToCtrRC500[8];
    
    Buf_AppToCtrRC500[17]=Buf_AppToCtrRC500[4];
    Buf_AppToCtrRC500[18]=~Buf_AppToCtrRC500[4];
    Buf_AppToCtrRC500[19]=Buf_AppToCtrRC500[4];
    Buf_AppToCtrRC500[20]=~Buf_AppToCtrRC500[4];
    
    if (MI_OK == PcdWrite(Buf_AppToCtrRC500[4], &Buf_AppToCtrRC500[5]))
    {    AnswerCommandOk();    }
    else
    {    AnswerErr(11);   }
}

/////////////////////////////////////////////////////////////////////
//��Ӧ��λ����Ǯ������
/////////////////////////////////////////////////////////////////////
void ComM1Readval(void)
{
    if (MI_OK == PcdRead(Buf_AppToCtrRC500[4], &Buf_CtrRC500return[0]))
    {	 AnswerOk(&Buf_CtrRC500return[0], 4);   }
    else
    {    AnswerErr(11);         }
}

/////////////////////////////////////////////////////////////////////
//��Ӧ��λ���ۿ�����
//Buf_AppToCtrRC500[4]:���
//Buf_AppToCtrRC500f[5]:4�ֽڿۿ�ֵ�����ֽ���ǰ
/////////////////////////////////////////////////////////////////////
void ComM1Decrement()
{
    if (MI_OK == PcdValue(PICC_DECREMENT, Buf_AppToCtrRC500[4], &Buf_AppToCtrRC500[5]))
    {    AnswerCommandOk(); 
	}
    else
    {    AnswerErr(11);   }
}

/////////////////////////////////////////////////////////////////////
//��Ӧ��λ����ֵ����
//Buf_AppToCtrRC500[4]:���
//Buf_AppToCtrRC500[5]:4�ֽڳ�ֵ�����ֽ���ǰ
/////////////////////////////////////////////////////////////////////
void ComM1Increment(void)
{
    if (MI_OK == PcdValue(PICC_INCREMENT, Buf_AppToCtrRC500[4], &Buf_AppToCtrRC500[5]))
    {    AnswerCommandOk(); 
	}
    else
    {    AnswerErr(11);   }
}

/////////////////////////////////////////////////////////////////////
//��Ӧ��λ��M1������Ǯ������
//Buf_AppToCtrRC500[4]):Դ��ַ���
//Buf_AppToCtrRC500[5]):Ŀ���ַ��ţ������Դ��ַ��ͬһ������
/////////////////////////////////////////////////////////////////////
void ComM1BakValue()
{
	uint8_t status;
	if(MI_OK == PcdRestore(Buf_AppToCtrRC500[4]))
	{
		status = PcdTransfer(Buf_AppToCtrRC500[5])	;
	}
	if(status == MI_OK)
	{
		AnswerCommandOk();
	}
	else
	{
		AnswerErr(11); 
	}
}

/////////////////////////////////////////////////////////////////////
//��Ӧ��λ��UltraLght������ײ����
/////////////////////////////////////////////////////////////////////
void ComUL_PcdAnticoll()
{
    if (MI_OK == UL_PcdAnticoll(&Buf_AppToCtrRC500[0]))
    {   AnswerOk(&Buf_AppToCtrRC500[0], 7);  }
    else
    {   AnswerErr(11);    }    
}

/////////////////////////////////////////////////////////////////////
//��Ӧ��λ��дUltraLight������
//Buf_AppToCtrRC500[4]:Ҫд�����ݵ�ҳ��
//Buf_AppToCtrRC500[5]:����4�ֽ�Ҫд�������
/////////////////////////////////////////////////////////////////////
void ComUL_PcdWrite()
{
    if (MI_OK == UL_PcdWrite(Buf_AppToCtrRC500[4], &Buf_AppToCtrRC500[5]))
    {   AnswerCommandOk();   }
    else
    {   AnswerErr(11);  }    
}

/////////////////////////////////////////////////////////////////////
//��Ӧ��λ��Mifare_Pro(X)���߼���λ����
/////////////////////////////////////////////////////////////////////
void ComTypeARst()
{
    uint8_t status;
	uint8_t *response = NULL;
	uint8_t len = 0;

    status = PcdRequest(Buf_AppToCtrRC500[4], &Buf_AppToCtrRC500[0]);
    
    if (status == MI_OK)
    {   status =  PcdAnticoll(PICC_ANTICOLL1, &Buf_AppToCtrRC500[1]);   }
    
    if (status == MI_OK)
    {   status =  PcdSelect1(&Buf_AppToCtrRC500[1], response, &len);   }
    
    if (status == MI_OK)    	
    {   status =  MifareProRst(0x40, &Buf_AppToCtrRC500[0], &Buf_AppToCtrRC500[5]);    }
    
    if (status == MI_OK)
    {	
        Buf_AppToCtrRC500[0] += 4;
        g_cCid     = 0;//(g_cReceBuf[9] & 0x03) << 2;
        AnswerOk(&Buf_AppToCtrRC500[1], Buf_AppToCtrRC500[0]);
    }
    else
    {   AnswerErr(11);    }
}   

/////////////////////////////////////////////////////////////////////
//��Ӧ��λ��T=CL��COS����
/////////////////////////////////////////////////////////////////////
void ComTCLCOS()
{
    uint8_t ucLg;
	ucLg =  Buf_AppToCtrRC500[1] - 3;
    if (MI_OK == ISO14443CLCom(g_cCid, &Buf_AppToCtrRC500[4], ucLg, &Buf_AppToCtrRC500[1], &Buf_AppToCtrRC500[0]))
    {	AnswerOk(&Buf_AppToCtrRC500[1], Buf_AppToCtrRC500[0]);   }
    else
    {   AnswerErr(11);    }
} 

/////////////////////////////////////////////////////////////////////
//��Ӧ��λ��T=CL���˳�����״̬����
/////////////////////////////////////////////////////////////////////
void ComCL_Deselect()
{
    if (MI_OK == CL_Deselect(0))
    {    AnswerCommandOk();    }
    else
    {    AnswerErr(11);   }
}
/////////////////////////////////////////////////////////////////////
//��ȷִ������λ��ָ��޷�������
/////////////////////////////////////////////////////////////////////
void AnswerCommandOk(void)
{
	if(flag_App_or_Ctr == 0)
	{
		Buf_CtrRC500ToApp[0] = 0x00;
		Buf_CtrRC500ToApp[1] = 0x00;
		Buf_CtrRC500ToApp[2] = 0x04;
		Buf_CtrRC500ToApp[3] = Buf_AppToCtrRC500[3];
		Buf_CtrRC500ToApp[4] = 0x00;	 //�ɹ���־λ
		Buf_CtrRC500ToApp[5] = XOR_Cal(&Buf_CtrRC500ToApp[0],5);
		flag_App_or_Ctr = 0x05;	
		Length_CtrRC500ToApp = 6;	
	}	
}

/////////////////////////////////////////////////////////////////////
//��ȷִ������λ��ָ��з�������
//input��answerdata = Ӧ������
//       answerlen  = ���ݳ���
/////////////////////////////////////////////////////////////////////
void AnswerOk(uint8_t *answerdata, unsigned int answerlen)
{
	if(flag_App_or_Ctr == 0)
	{
		Buf_CtrRC500ToApp[0] = 0x00;
		Buf_CtrRC500ToApp[1] = 0x00;
		Buf_CtrRC500ToApp[2] = answerlen + 4;
		Buf_CtrRC500ToApp[3] = Buf_AppToCtrRC500[3];
		Buf_CtrRC500ToApp[4] = 0x00;	 //�ɹ���־λ
		memcpy(&Buf_CtrRC500ToApp[5], answerdata, answerlen);
		Buf_CtrRC500ToApp[Buf_CtrRC500ToApp[2]+1]= XOR_Cal(&Buf_CtrRC500ToApp[0],answerlen+5);
		flag_App_or_Ctr = 0x05;  
		Length_CtrRC500ToApp = answerlen + 6; 	
//		memcpy(&Buf_CtrRC500return[0],answerdata,answernum);	
	}
}

/////////////////////////////////////////////////////////////////////
//��λ��ָ��ִ�г���
/////////////////////////////////////////////////////////////////////
void AnswerErr(int faultcode)
{
	if(flag_App_or_Ctr == 0)
	{
		Buf_CtrRC500ToApp[0] = 0x00;
		Buf_CtrRC500ToApp[1] = 0x00;
		Buf_CtrRC500ToApp[2] = 0x04;
		Buf_CtrRC500ToApp[3] = Buf_AppToCtrRC500[3];
		Buf_CtrRC500ToApp[4] = faultcode  ;	 //�������
		Buf_CtrRC500ToApp[5] = XOR_Cal(&Buf_CtrRC500ToApp[0],5);
		flag_App_or_Ctr = 0x05;	
		Length_CtrRC500ToApp = 6; 		
	}
}


/*
 *********************************************************************************
 *    ��������
 *********************************************************************************
 */
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
/*
 *********************************************************************************
 *    ����ӿں���
 *********************************************************************************
 */
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
			case 0x01:	// ������
				PcdAntennaOn();												//��13.56M����
				findIC_flow = 0x02;
				break;
			
			case 0x02:	//����reqAָ��
				memset(g_cardType, 0, 40);
				if(PcdRequest(PICC_REQIDL,g_cardType) == MI_OK)				//����A�������ؿ����ͣ���ͬ���Ϳ���Ӧ��ͬ��UID����
				{	
					if( (g_cardType[0] & 0x40) == 0x40)
					{	uid_len = 8;	}
					else
					{	uid_len = 4;	}
					findIC_flow = 0x03;
				}
				else
				{
					findIC_flow = 0x00;
				}
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
				{
					findIC_flow = 0x06;
				}
				else
					findIC_flow = 0x02;		
				break;
				
			case 0x06:	//ѡ��2
				if((MI_OK == PcdSelect2(&g_cSNR[4], respon, &len))&&((respon[0] & 0x20) == 0x20))
				{
					findIC_flow = 0x07;
				}
				else
					findIC_flow = 0x02;		
				break;
				
			case 0x07:	//Ѱ���ɹ�
				if(flag_upload_uid_once)						//�����λ�����ϴ����ű�־�����ÿ����ߣ�����ֻ�ϴ�һ��
				{
					flag_upload_uid_once = false;
					findIC_flow = 0x08;
					status = MI_OK;
				}
				else
				{
					findIC_flow = 0x00;
					status = MI_OK;
				}				
				break;
				
			case 0x08:	//������
				PcdHalt();												//ʹ�������ŵĿ���������
				findIC_flow = 0x00;
				break;
			default	:
				findIC_flow = 0x00;
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
