/**
  ******************************************************************************
  * @file   	mfrc500.c
  * @author  	Tian erjun
  * @version 	V1.0.0.0
  * @date   	2015.11.05
  * @brief   	hal function for nrf moulde
  ******************************************************************************
  */
 
#include "main.h"

bool g_bIblock = false;

/********************************************************************
*    ���� MFRC500 DATA Port Ϊ����ģʽ
*********************************************************************/
void MFRC500_DATA_Port_config_input()
{
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = MFRC500_DATA_Pin;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(MFRC500_DATA_Port, &GPIO_InitStructure);
}

/*********************************************************************
*    ���� MFRC500 DATA Port Ϊ���ģʽ
**********************************************************************/
void MFRC500_DATA_Port_config_output()
{
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = MFRC500_DATA_Pin;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(MFRC500_DATA_Port, &GPIO_InitStructure);
}
/*******************************************************************************
  * @brief  ��RC500�Ĵ���
  * @param  Address=�Ĵ�����ַ
  * @retval ������ֵ
  * @note 	None		  
*******************************************************************************/
uint8_t ReadRC(uint8_t Address)
{
	u8 value;
	u16 temp;
	
	ALE(Bit_RESET);		   //ALE =0	
	CS(Bit_SET);		   //CS = 1
    RD(Bit_SET);		   //RD = 1
	WR(Bit_SET);		   //WR = 1
	temp = GPIO_ReadOutputData(MFRC500_DATA_Port) & 0xff; //ȡ��8λ
	WR_DATA((Address << 8)|temp);		   //PB[8:15] = DATA

	ALE(Bit_SET);		   //ALE = 1	
	Delay10us(3);	
	ALE(Bit_RESET);		   //ALE = 0
	MFRC500_DATA_Port_config_input();	 

	CS(Bit_RESET);		   //CS = 0
	RD(Bit_RESET);		   //RD = 0
	Delay3us();
	value = RD_DATA();	   //value = PB[8:15] 	
	Delay3us();
	RD(Bit_SET);		   //RD = 1	
	CS(Bit_SET);		   //CS = 1	
	MFRC500_DATA_Port_config_output();
	
	return value;
}
/*******************************************************************************
  * @brief  дRC500�Ĵ���
  * @param  Address=�Ĵ�����ַ
			value=Ҫд���ֵ
  * @retval None
  * @note 	None		  
*******************************************************************************/
void WriteRC(uint16_t Address,uint16_t value)
{
  	u16 temp;
	ALE(Bit_SET);  			//ALE = 1
	temp = GPIO_ReadOutputData(MFRC500_DATA_Port) & 0xff;
	WR_DATA((Address << 8)|temp); 		//PB[8:15] = DATA 
	Delay10us(2);
	ALE(Bit_RESET);			//ALE = 0
	Delay10us(2);
	CS(Bit_RESET);			//CS = 0
	temp = GPIO_ReadOutputData(MFRC500_DATA_Port) & 0xff;
	WR_DATA((value << 8)|temp);   		//PB[8:15] = DATA
	WR(Bit_RESET);      	//WR = 0
	Delay10us(2);
	WR(Bit_SET);		   //WR = 1
	CS(Bit_SET);		   //CS = 1
}
/*******************************************************************************
  * @brief  ��RC500�Ĵ���λ
  * @param  reg=�Ĵ�����ַ
  * @param  mask=��λֵ
  * @retval None
  * @note 	None	  
*******************************************************************************/
static void SetBitMask(uint8_t reg,uint8_t mask)
{
   uint8_t tmp=0x0;
   tmp=ReadRC(reg);
   WriteRC(reg,tmp|mask);
}

/*******************************************************************************
  * @brief  ��RC500�Ĵ���λ
  * @param  reg=�Ĵ�����ַ
  * @param  mask=��λֵ
  * @retval None
  * @note 	None	  
*******************************************************************************/
static void ClearBitMask(uint8_t reg,uint8_t mask)
{
   uint8_t tmp=0x0;
   tmp = ReadRC(reg);
   WriteRC(reg,tmp & ~mask);
}

/*******************************************************************************
  * @brief  ����RC500��ʱ
  * @param  tmolength=����ֵ
  * @retval None
  * @note 	None	  
*******************************************************************************/
static void PcdSetTmo(uint8_t tmoLength)
{
   switch(tmoLength)
   {  
      case 1:                             // short timeout (1,0 ms)
         WriteRC(RegTimerClock,0x07);     // TAutoRestart=0,TPrescale=128
         WriteRC(RegTimerReload,0x6a);    // TReloadVal = 'h6a =106(dec)
         break;
      case 2:                             // medium timeout (1,5 ms)
         WriteRC(RegTimerClock,0x07);     // TAutoRestart=0,TPrescale=128
         WriteRC(RegTimerReload,0xa0);    // TReloadVal = 'ha0 =160(dec)
         break;
      case 3:                              // long timeout (6 ms)
         WriteRC(RegTimerClock,0x09);      // TAutoRestart=0,TPrescale=4*128
         WriteRC(RegTimerReload,0xa0);     // TReloadVal = 'ha0 =160(dec)
         break;
      case 4:                              // long timeout (9.6 ms)
         WriteRC(RegTimerClock,0x09);      // TAutoRestart=0,TPrescale=4*128
         WriteRC(RegTimerReload,0xff);     // TReloadVal = 'ff =255(dec)
         break;
      default:                             // short timeout (1,0 ms)   time��1ms = (2^7 *106)/13560000
         WriteRC(RegTimerClock,0x07);      // TAutoRestart=0,TPrescale=128
         WriteRC(RegTimerReload,tmoLength);// TReloadVal = tmoLength
         break;
   }
   WriteRC(RegTimerControl,0x06);
}

/*******************************************************************************
  * @brief  ��RC500ͨѶ
  * @param  pi->MfCommand=RC500������
  * @param  pi->MfLength=���͵����ݳ���
  *	@param	pi->MfData[]=��������
  * @retval pi->MfLength=���յ����ݳ���(λ)
  *	@retval	pi->MfData[]=���յ�������
  * @retval len = ���յ����ݳ��ȣ��ֽڣ�
  * @retval status=����״̬
  * @note 	None	  
*******************************************************************************/
static uint8_t PcdComTransceive(void *p, uint8_t *len)
{  
	struct TranSciveBuffer{	uint8_t MfCommand;
							uint8_t MfLength;
							uint8_t MfData[BUF_LEN];};
	struct TranSciveBuffer *pi = (struct TranSciveBuffer *)p;
   uint8_t recebyte=0;
   uint8_t status;
   uint8_t irqEn=0x00;
   uint8_t waitFor=0x00;
   uint8_t lastBits;
   uint8_t n;
   uint32_t i;
   switch(pi->MfCommand)
   {
      case PCD_IDLE:
         irqEn = 0x00;
         waitFor = 0x00;
         break;
      case PCD_WRITEE2:
         irqEn = 0x11;
         waitFor = 0x10;
         break;
      case PCD_READE2:
         irqEn = 0x07;
         waitFor = 0x04;
         recebyte=1;
         break;
      case PCD_LOADCONFIG:
      case PCD_LOADKEYE2:
      case PCD_AUTHENT1:
         irqEn = 0x05;
         waitFor = 0x04;
         break;
      case PCD_CALCCRC:
         irqEn = 0x11;
         waitFor = 0x10;
         break;
      case PCD_AUTHENT2:
         irqEn = 0x04;
         waitFor = 0x04;
         break;
      case PCD_RECEIVE:
         irqEn = 0x06;
         waitFor = 0x04;
         recebyte=1;
         break;
      case PCD_LOADKEY:
         irqEn = 0x05;
         waitFor = 0x04;
         break;
      case PCD_TRANSMIT:
         irqEn = 0x05;
         waitFor = 0x04;
         break;
      case PCD_TRANSCEIVE:
         irqEn = 0x3D;
         waitFor = 0x04;
         recebyte=1;
         break;
      default:
         pi->MfCommand=MI_UNKNOWN_COMMAND;
         break;
   }
   if(pi->MfCommand!=MI_UNKNOWN_COMMAND)
   {
      WriteRC(RegPage,0x00);
      WriteRC(RegInterruptEn,0x7F);
      WriteRC(RegInterruptRq,0x7F);
      WriteRC(RegCommand,PCD_IDLE);
      SetBitMask(RegControl,0x01);										//Flush FIFO
      WriteRC(RegInterruptEn,irqEn|0x80);
      for(i=0;i<pi->MfLength;i++)
      {
         WriteRC(RegFIFOData,pi->MfData[i]);
      }
      WriteRC(RegCommand,pi->MfCommand);
      i=0x2000;
      do
      {
         n=ReadRC(RegInterruptRq);
         i--;
      }
      while((i!=0)&&!(n&irqEn&0x20)&&!(n&waitFor));
      status=MI_COM_ERR;
      if((i!=0)&&!(n&irqEn&0x20))
      {
         if(!(ReadRC(RegErrorFlag)&0x17))
         {
            status=MI_OK;
            if(recebyte)
            {
              	n=ReadRC(RegFIFOLength);
              	lastBits=ReadRC(RegSecondaryStatus)&0x07;
                if(lastBits)
                {
                   pi->MfLength=(n-1)*8+lastBits;
                }
                else
                {
                   pi->MfLength=n*8;
                }
				
                if(n==0)
                {
                   n=1;
                }
                for(i=0;i<n;i++)
                {
                      pi->MfData[i]=ReadRC(RegFIFOData);
                }
				memcpy(p, pi, sizeof(struct TranSciveBuffer));
				*len = n;
            }
         }
      }
      else if(n&irqEn&0x20)
      {
          status=MI_NOTAGERR;
      }
      else
      {
          status=MI_COM_ERR;
      }
      WriteRC(RegInterruptEn,0x7F);
      WriteRC(RegInterruptRq,0x7F);
   }
   return status;
}
/*******************************************************************************
  * @brief  �ر�RC500���߷����RC500ͨѶ
  * @param  None
  * @retval None
  * @note 	None	  
*******************************************************************************/
void PcdAntennaOff(void)
{
    ClearBitMask(RegTxControl,0x03);
}

/*******************************************************************************
  * @brief  ����RC500���߷���
  * @param  None
  * @retval None
  * @note 	ÿ��������ر����շ���֮��Ӧ������1ms�ļ��,
  *         ִ�����ʼ��RC500������������������RC500ͨѶ	  
*******************************************************************************/
void PcdAntennaOn(void)
{
    SetBitMask(RegTxControl,0x03);
}

/*******************************************************************************
  * @brief  ��λ����ʼ��RC500
  * @param  None
  * @retval status=����״̬
  * @note 	ע��:RC500�ϵ��Ӧ��ʱ500ms���ܿɿ���ʼ��  
*******************************************************************************/
uint8_t PcdReset(void)
{
	uint8_t status=MI_OK;
	uint16_t  i=0x2000;

	PD(Bit_RESET);
	Delay10us(2500);
	PD(Bit_SET);
	Delay10us(1000);
	PD(Bit_RESET);
	Delay10us(300);

	while ((ReadRC(RegCommand) & 0x3F) && i--)
	{
	}
    if(!i)
    {
        status = MI_NOTAGERR;
    }
    if (status == MI_OK)
    {
        WriteRC(RegPage,0x80);
        if (ReadRC(RegCommand) != 0x00)
        {                           
            status = MI_NOTAGERR;
        }
		else
		{	
			WriteRC(RegPage,0x00);
		}
    }

	if(status==MI_OK)
	{
		WriteRC(RegClockQControl,0x0);
		WriteRC(RegClockQControl,0x40);
		Delay10us(10);
		ClearBitMask(RegClockQControl,0x40);
		WriteRC(RegBitPhase,0xad);
		WriteRC(RegRxThreshold,0xff);
		WriteRC(RegRxControl2,0x41);
		WriteRC(RegFIFOLevel,0x1a);
		WriteRC(RegTimerControl,0x02);
		WriteRC(RegIRqPinConfig,0x02);
		WriteRC(RegTxControl,0x5b);		   //������
		WriteRC(RegCwConductance,0x3f) ;
		PcdAntennaOff();
	}
	return status;
}
/*******************************************************************************
  * @brief  Ѱ��
  * @param  req_code:Ѱ����ʽ
  *         	//0x52 = Ѱ��Ӧ�������з���14443A��׼�Ŀ�
  *             0x26 = Ѱδ��������״̬�Ŀ�
  * @retval pTagType=��Ƭ���ʹ���
  *             0x4400 = Mifare_UltraLight
  *             0x0400 = Mifare_One(S50)
  * @retval status=����״̬
  * @note 	None	  
*******************************************************************************/
uint8_t PcdRequest(uint8_t req_code,uint8_t *pTagType)
{
	uint8_t status;
	uint8_t ret_len = 0;
	struct TranSciveBuffer {uint8_t MfCommand;
						   uint8_t MfLength;
						   uint8_t MfData[2];
						   }MfComData;
	struct TranSciveBuffer *pi;

	pi=&MfComData;
	PcdSetTmo(106);                                     //����RC500��ʱ
	WriteRC(RegChannelRedundancy,0x03);					//����ÿ���ֽں���У��
	ClearBitMask(RegControl,0x08);						//�ر�Crypto1 ����
	WriteRC(RegBitFraming,0x07);						//���һ���ֽڷ���7λ
	MfComData.MfCommand=PCD_TRANSCEIVE;
	MfComData.MfLength=1;
	MfComData.MfData[0]=req_code;
	status = ReadRC(RegChannelRedundancy);	
	status = ReadRC(RegBitFraming);						   
						   
	status=PcdComTransceive(pi, &ret_len);

	if ((status == MI_OK) && (MfComData.MfLength == 0x10))
	{    
		*pTagType     = MfComData.MfData[0];
		*(pTagType+1) = MfComData.MfData[1];
	}
	else
	{
		status = MI_BITCOUNTERR; 
	}
	return status;
}

/*******************************************************************************
  * @brief  ����ײ
  * @param  Snr[OUT]:�õ��Ŀ�Ƭ���кţ�4�ֽ�
  * @retval status=MI_OK:�ɹ�
  * @note 	Ѱ���ɹ���ͨ���˺������������ڿ�Ƭ���ͷ���ײ����������������м��ſ�
  *         �˺���ֻ�õ�һ�ſ�Ƭ�����кţ�����Pcdselect()����ѡ�����ſ��������к���
  *         ������Դ˿���������Ϻ���PcdHalt()����˿���������״̬����Ѱδ��������
  *         ״̬�Ŀ����ɽ���������Ƭ�Ĳ���None	  
*******************************************************************************/
uint8_t PcdAnticoll(uint8_t antiFlag, uint8_t *snr)
{
    uint8_t i;
	uint8_t ret_len = 0;
    uint8_t snr_check=0;
    uint8_t status=MI_OK;
    struct TranSciveBuffer{		uint8_t MfCommand;
                                uint8_t MfLength;
                                uint8_t MfData[5];
                                }MfComData;
    struct TranSciveBuffer *pi;

    pi=&MfComData;
    PcdSetTmo(106);
    WriteRC(RegDecoderControl,0x28);
    ClearBitMask(RegControl,0x08);
    WriteRC(RegChannelRedundancy,0x03);

    MfComData.MfCommand=PCD_TRANSCEIVE;
    MfComData.MfLength=2;
								
	if(PICC_ANTICOLL1 == antiFlag)
		MfComData.MfData[0]=PICC_ANTICOLL1;
	else if(PICC_ANTICOLL2 == antiFlag)
		MfComData.MfData[0]=PICC_ANTICOLL2;
	
    MfComData.MfData[1]=0x20;
    status=PcdComTransceive(pi, &ret_len);
    if(!status)
    {
    	 for(i=0;i<4;i++)
         {
             snr_check^=MfComData.MfData[i];
         }
         if(snr_check!=MfComData.MfData[i])
         {
             status=MI_SERNRERR;
         }
         else
         {
             for(i=0;i<4;i++)
             {
             	*(snr+i)=MfComData.MfData[i];
             }
         }

    }
    ClearBitMask(RegDecoderControl,0x20);
    return status;
}
/*******************************************************************************
  * @brief  ѡ��һ�ſ�
  * @param  snr=������к�(4byte)���ڴ浥Ԫ�׵�ַ,��Ƭ���кţ�4�ֽ�
  * @retval status=MI_OK:�ɹ�
  * @note 	None	  
*******************************************************************************/
uint8_t PcdSelect(uint8_t *snr)
{
    uint8_t i;
    uint8_t status;
	uint8_t ret_len = 0;
    uint8_t snr_check=0;
    struct TranSciveBuffer{uint8_t MfCommand;
                                 uint8_t MfLength;
                                 uint8_t MfData[7];
                                }MfComData;
    struct TranSciveBuffer *pi;

    pi=&MfComData;
    PcdSetTmo(106);
    WriteRC(RegChannelRedundancy,0x0F);
    ClearBitMask(RegControl,0x08);

    MfComData.MfCommand=PCD_TRANSCEIVE;
    MfComData.MfLength=7;
    MfComData.MfData[0]=PICC_ANTICOLL1;
    MfComData.MfData[1]=0x70;
    for(i=0;i<4;i++)
    {
    	snr_check^=*(snr+i);
    	MfComData.MfData[i+2]=*(snr+i);
    }
    MfComData.MfData[6]=snr_check;
    status=PcdComTransceive(pi, &ret_len);
	if(status==MI_OK)
	{    
		if(MfComData.MfLength!=0x8)
		{
			status = MI_BITCOUNTERR;
		}
	}
   return status;
}

/*******************************************************************************
  * @brief  ѡ����Ƭ
  * @param  pSnr[IN]:��Ƭ���кţ�4�ֽ�
  * @retval status=����״̬,�ɹ�����MI_OK
  * @note 	None	  
*******************************************************************************/
uint8_t PcdSelect1(uint8_t *snr, uint8_t *res, uint8_t *len)
{
    uint8_t i;
    uint8_t status;
    uint8_t snr_check=0;
    struct TranSciveBuffer{uint8_t MfCommand;
                                 uint8_t MfLength;
                                 uint8_t MfData[7];
                                }MfComData;
    struct TranSciveBuffer *pi;

    pi=&MfComData;
    PcdSetTmo(106);
    WriteRC(RegChannelRedundancy,0x0F);
    ClearBitMask(RegControl,0x08);

    MfComData.MfCommand=PCD_TRANSCEIVE;
    MfComData.MfLength=7;
    MfComData.MfData[0]=PICC_ANTICOLL1;
    MfComData.MfData[1]=0x70;
    for(i=0;i<4;i++)
    {
    	snr_check^=*(snr+i);
    	MfComData.MfData[i+2]=*(snr+i);
    }
    MfComData.MfData[6]=snr_check;
    status=PcdComTransceive(pi, len);
	
    if(status==MI_OK)
    {   
		memcpy(res, pi->MfData, *len); 		
		if(MfComData.MfLength!=0x8)
        {
			status = MI_BITCOUNTERR;
        }
   }
   return status;
}

/*******************************************************************************
  * @brief  ѡ����Ƭ
  * @param  pSnr[IN]:��Ƭ���кţ�4�ֽ�
  * @retval status=����״̬,�ɹ�����MI_OK
  * @note 	None	  
*******************************************************************************/
uint8_t PcdSelect2(uint8_t *snr, uint8_t *res, uint8_t *len)
{
    uint8_t i;
    uint8_t status;
    uint8_t snr_check=0;
    struct TranSciveBuffer{uint8_t MfCommand;
                                 uint8_t MfLength;
                                 uint8_t MfData[7];
                                }MfComData;
    struct TranSciveBuffer *pi;

    pi=&MfComData;
    PcdSetTmo(106);
    WriteRC(RegChannelRedundancy,0x0F);
    ClearBitMask(RegControl,0x08);

    MfComData.MfCommand=PCD_TRANSCEIVE;
    MfComData.MfLength=7;
    MfComData.MfData[0]=PICC_ANTICOLL2;
    MfComData.MfData[1]=0x70;
    for(i=0;i<4;i++)
    {
    	snr_check^=*(snr+i);
    	MfComData.MfData[i+2]=*(snr+i);
    }
    MfComData.MfData[6]=snr_check;
    status=PcdComTransceive(pi, len);

    if(status==MI_OK)
    {  
		memcpy(res, pi->MfData, *len); 
		if(MfComData.MfLength!=0x8)
        {
			status = MI_BITCOUNTERR;
        }
   }
   return status;
}

/*******************************************************************************
  * @brief  �����������״̬
  * @param  None
  * @retval status=����״̬,�ɹ�����MI_OK
  * @note 	None	  
*******************************************************************************/
uint8_t PcdHalt(void)
{
    uint8_t status=MI_OK;
	uint8_t ret_len = 0;
    struct TranSciveBuffer{uint8_t MfCommand;
                                 uint8_t MfLength;
                                 uint8_t MfData[2];
                                }MfComData;
    struct TranSciveBuffer *pi;
    pi=&MfComData;

    PcdSetTmo(106);
    MfComData.MfCommand=PCD_TRANSCEIVE;
    MfComData.MfLength=2;
    MfComData.MfData[0]=PICC_HALT;
    MfComData.MfData[1]=0;

    status=PcdComTransceive(pi, &ret_len);
    if (status)
    {
        if(status==MI_NOTAGERR||status==MI_ACCESSTIMEOUT)
        status = MI_OK;
    }
    WriteRC(RegCommand,PCD_IDLE);
    return status;
}

/*******************************************************************************
  * @brief  ����Կת��ΪRC500���ո�ʽ,��Կ��ʽ��048020.PDF��67ҳ
  * @param  uncoded=6�ֽ�δת������Կ�׵�ַ
  * @param  coded=12�ֽ�ת�������Կ����׵�ַ,ת�������Կ����ָ����Ԫ 
  * @retval status=����״̬,�ɹ�����MI_OK
  * @note 	��Mifare_One����Կת��ΪRC500���ո�ʽ	  
*******************************************************************************/
uint8_t ChangeCodeKey(uint8_t *uncoded,uint8_t *coded)
{
   uint8_t cnt=0;
   uint8_t ln=0;
   uint8_t hn=0;

   for(cnt=0;cnt<6;cnt++)
   {
      ln=uncoded[cnt]&0x0F;
      hn=uncoded[cnt]>>4;
      coded[cnt*2+1]=(~ln<<4)|ln;
      coded[cnt*2]=(~hn<<4)|hn;
   }
   return MI_OK;
}

/*******************************************************************************
  * @brief  ����ת����ʽ�����Կ�͵�RC500��FIFO��
  * @param  keys=12�ֽ���Կ����׵�ַ
  * @retval status=����״̬,�ɹ�����MI_OK
  * @note 	None	  
*******************************************************************************/
uint8_t PcdAuthKey(uint8_t *keys)
{
    uint8_t status;
	uint8_t ret_len = 0;
    uint8_t i;
    struct TranSciveBuffer{uint8_t MfCommand;
                                 uint8_t MfLength;
                                 uint8_t MfData[12];
                                }MfComData;
    struct TranSciveBuffer *pi;
    pi=&MfComData;
    PcdSetTmo(106);
    MfComData.MfCommand=PCD_LOADKEY;
    MfComData.MfLength=12;
    for(i=0;i<12;i++)
    {
        MfComData.MfData[i]=*(keys+i);
    }
    status=PcdComTransceive(pi, &ret_len);
    return status;
}
/*******************************************************************************
  * @brief  �ô��RC500��FIFO�е���Կ�Ϳ��ϵ���Կ������֤
  * @param  auth_mode=��֤��ʽ,0x60:��֤A��Կ,0x61:��֤B��Կ
  * @param  block=Ҫ��֤�ľ��Կ��ַ
  * @param  snr=���к��׵�ַ,��Ƭ���кţ�4�ֽ�
  * @retval status=����״̬,�ɹ�����MI_OK
  * @note 	None	  
*******************************************************************************/
uint8_t PcdAuthState(uint8_t auth_mode,uint8_t block,uint8_t *snr)
{
    uint8_t status=MI_OK;
	uint8_t ret_len = 0;
    uint8_t i;
    struct TranSciveBuffer{uint8_t MfCommand;
                                 uint8_t MfLength;
                                 uint8_t MfData[6];
                                }MfComData;
    struct TranSciveBuffer *pi;
    pi=&MfComData;

    WriteRC(RegChannelRedundancy,0x07);
    if(status==MI_OK)
    {
        PcdSetTmo(106);
        MfComData.MfCommand=PCD_AUTHENT1;
        MfComData.MfLength=6;
        MfComData.MfData[0]=auth_mode;
        MfComData.MfData[1]=block;
        for(i=0;i<4;i++)
        {
	      MfComData.MfData[i+2]=*(snr+i);
        }
        if((status=PcdComTransceive(pi, &ret_len))==MI_OK)
        {
            if (ReadRC(RegSecondaryStatus)&0x07) 
            {
                status = MI_BITCOUNTERR;
            }
            else
            {
                MfComData.MfCommand=PCD_AUTHENT2;
                 MfComData.MfLength=0;
                if((status=PcdComTransceive(pi, &ret_len))==MI_OK)
                {
                    if(ReadRC(RegControl)&0x08)
                        status=MI_OK;
                    else
                        status=MI_AUTHERR;
                }
             }
         }
   }
   return status;
}
/*******************************************************************************
  * @brief  ������һ��(block)����(16�ֽ�)
  * @param  adde=Ҫ���ľ��Կ��ַ
  * @param  readdata=���������ݴ���׵�ַ,���������ݣ�16�ֽ�
  * @retval status=����״̬,�ɹ�����MI_OK
  * @note 	���������ݴ���readdataָ��ĵ�Ԫ 
*******************************************************************************/
uint8_t PcdRead(uint8_t addr,uint8_t *readdata)
{
    uint8_t status;
	uint8_t ret_len = 0;
    uint8_t i;
    struct TranSciveBuffer
	{
		uint8_t MfCommand;
        uint8_t MfLength;
        uint8_t MfData[16];
    }MfComData;
    struct TranSciveBuffer *pi;

    pi=&MfComData;
    PcdSetTmo(4);
    WriteRC(RegChannelRedundancy,0x0F);
    MfComData.MfCommand=PCD_TRANSCEIVE;
    MfComData.MfLength=2;
    MfComData.MfData[0]=PICC_READ;
    MfComData.MfData[1]=addr;

    status=PcdComTransceive(pi, &ret_len);
    if(status==MI_OK)
    {
        if(MfComData.MfLength!=0x80)
        {
            status = MI_BITCOUNTERR;
        }
        else
        {
            for(i=0;i<16;i++)
            {
                *(readdata+i)=MfComData.MfData[i];
            }
        }
    }
    return status;
}
/*******************************************************************************
  * @brief  д���ݵ����ϵ�һ��
  * @param  adde=Ҫд�ľ��Կ��
  * @param  writedata=д�������׵�ַ,д������ݣ�16�ֽ�
  * @retval status=����״̬,�ɹ�����MI_OK
  * @note 	None	  
*******************************************************************************/
uint8_t PcdWrite(uint8_t addr,uint8_t *writedata)
{
    uint8_t status;
	uint8_t ret_len = 0;
    uint8_t i;
    struct TranSciveBuffer{uint8_t MfCommand;
                                 uint8_t MfLength;
                                 uint8_t MfData[16];
                                }MfComData;
    struct TranSciveBuffer *pi;

    pi=&MfComData;
    PcdSetTmo(4);
  //  WriteRC(RegChannelRedundancy,0x0F); 
    WriteRC(RegChannelRedundancy,0x07); 
    MfComData.MfCommand=PCD_TRANSCEIVE;
    MfComData.MfLength=2;
    MfComData.MfData[0]=PICC_WRITE;
    MfComData.MfData[1]=addr;

    status=PcdComTransceive(pi, &ret_len);
    if(status!=MI_NOTAGERR)
    {
        if(MfComData.MfLength!=4)
        {
           status=MI_BITCOUNTERR;
        }
        else
        {
           MfComData.MfData[0]&=0x0f;
           switch(MfComData.MfData[0])
           {
              case 0x00:
                 status=MI_NOTAUTHERR;
                 break;
              case 0x0a:
                 status=MI_OK;
                 break;
              default:
                 status=MI_CODEERR;
                 break;
           }
        }
     }
     if(status==MI_OK)
     {
        PcdSetTmo(3);
        MfComData.MfCommand=PCD_TRANSCEIVE;
        MfComData.MfLength=16;
        for(i=0;i<16;i++)
        {
            MfComData.MfData[i]=*(writedata+i);
        }
        status=PcdComTransceive(pi, &ret_len);
        if(status!=MI_NOTAGERR)
        {
            MfComData.MfData[0]&=0x0f;
            switch(MfComData.MfData[0])
            {
               case 0x00:
                  status=MI_WRITEERR;
                  break;
               case 0x0a:
                  status=MI_OK;
                  break;
               default:
                  status=MI_CODEERR;
                  break;
           }
        }
     }
  return status;
}

/*******************************************************************************
  * @brief  �ۿ�ͳ�ֵ
  * @param  dd_mode=������,0xc0:�ۿ�,0xc1:��ֵ
  * @param  addr=Ǯ���ľ��Կ��
  * @param  value=4�ֽ���(��)ֵ�׵�ַ,16������,��λ��ǰ
  * @retval status=����״̬,�ɹ�����MI_OK
  * @note 	None	  
*******************************************************************************/
uint8_t PcdValue(uint8_t dd_mode,uint8_t addr,uint8_t *value)
{
    uint8_t status;
	uint8_t ret_len = 0;
    uint8_t i;
    struct TranSciveBuffer{uint8_t MfCommand;
                                 uint8_t MfLength;
                                 uint8_t MfData[4];
                                }MfComData;
    struct TranSciveBuffer *pi;
    pi=&MfComData;

    PcdSetTmo(106);
    MfComData.MfCommand=PCD_TRANSCEIVE;
    MfComData.MfLength=2;
    MfComData.MfData[0]=dd_mode;
    MfComData.MfData[1]=addr;

    status=PcdComTransceive(pi, &ret_len);
    if(status!=MI_NOTAGERR)
    {
        if(MfComData.MfLength!=4)
        {
           status=MI_BITCOUNTERR;
        }
        else
        {
           MfComData.MfData[0]&=0x0f;
           switch(MfComData.MfData[0])
           {
              case 0x00:
                 status=MI_NOTAUTHERR;
                 break;
              case 0x0a:
                 status=MI_OK;
                 break;
              case 0x01:
                 status=MI_VALERR;
                 break;
              default:
                 status=MI_CODEERR;
                 break;
           }
        }
     }
     if(status==MI_OK)
     {
        PcdSetTmo(4);
        MfComData.MfCommand=PCD_TRANSCEIVE;
        MfComData.MfLength=4;
        pi=&MfComData;
        for(i=0;i<4;i++)
        {
            MfComData.MfData[i]=*(value+i);
        }
        status=PcdComTransceive(pi, &ret_len);
        if(status==MI_OK||status==MI_NOTAGERR)
        {
           if(MfComData.MfLength!=4)
           {
              status = MI_BITCOUNTERR;
           }
           else
           {
              status=MI_OK;
            }
         }
         else
         {
              status=MI_COM_ERR;
          }
     }
     if(status==MI_OK)
     {

        MfComData.MfCommand=PCD_TRANSCEIVE;
        MfComData.MfLength=2;
        MfComData.MfData[0]=PICC_TRANSFER;
        MfComData.MfData[1]=addr;
        status=PcdComTransceive(pi, &ret_len);
        if(status!=MI_NOTAGERR)
        {
            if(MfComData.MfLength!=4)
            {
               status=MI_BITCOUNTERR;
            }
            else
            {
               MfComData.MfData[0]&=0x0f;
               switch(MfComData.MfData[0])
               {
                  case 0x00:
                     status=MI_NOTAUTHERR;
                     break;
                  case 0x0a:
                     status=MI_OK;
                     break;
                  case 0x01:
                     status=MI_VALERR;
                     break;
                  default:
                     status=MI_CODEERR;
                     break;
               }
            }
        }
     }
   return status;
}
/*******************************************************************************
  * @brief  ��Ǯ��ֵ���뻺����
  * @param  addr=Ǯ���ľ��Կ��
  * @retval status=����״̬,�ɹ�����MI_OK
  * @note 	None	  
*******************************************************************************/
uint8_t PcdRestore(uint8_t addr)
{
    uint8_t status;
	uint8_t ret_len = 0;
    struct TranSciveBuffer{uint8_t MfCommand;
                                 uint8_t MfLength;
                                 uint8_t MfData[4];
                                }MfComData;
    struct TranSciveBuffer *pi;
    pi=&MfComData;

    PcdSetTmo(4);
    MfComData.MfCommand=PCD_TRANSCEIVE;
    MfComData.MfLength=2;
    MfComData.MfData[0]=PICC_RESTORE;
    MfComData.MfData[1]=addr;

    status=PcdComTransceive(pi, &ret_len);
    if(status!=MI_NOTAGERR)
    {
        if(MfComData.MfLength!=4)
        {
           status=MI_BITCOUNTERR;
        }
        else
        {
           MfComData.MfData[0]&=0x0f;
           switch(MfComData.MfData[0])
           {
              case 0x00:
                 status=MI_NOTAUTHERR;
                 break;
              case 0x0a:
                 status=MI_OK;
                 break;
              case 0x01:
                 status=MI_VALERR;
                 break;
              default:
                 status=MI_CODEERR;
                 break;
           }
        }
     }
     if(status==MI_OK)
     {
        PcdSetTmo(4);
        MfComData.MfCommand=PCD_TRANSCEIVE;
        MfComData.MfLength=4;
        MfComData.MfData[0]=0;
        MfComData.MfData[1]=0;
        MfComData.MfData[2]=0;
        MfComData.MfData[3]=0;
        status=PcdComTransceive(pi, &ret_len);
        if(status==MI_NOTAGERR)
        {
            status=MI_OK;
        }
     }
     return status;
}
/*******************************************************************************
  * @brief  ���滺����ֵ�е�Ǯ��ֵ
  * @param  addr=Ǯ���ľ��Կ��
  * @retval status=����״̬,�ɹ�����MI_OK
  * @note 	None	  
*******************************************************************************/
uint8_t PcdTransfer(uint8_t addr)
{
    uint8_t status;
	uint8_t ret_len = 0;
    struct TranSciveBuffer{uint8_t MfCommand;
                                 uint8_t MfLength;
                                 uint8_t MfData[2];
                                }MfComData;
    struct TranSciveBuffer *pi;
    pi=&MfComData;

    PcdSetTmo(4);
    MfComData.MfCommand=PCD_TRANSCEIVE;
    MfComData.MfLength=2;
    MfComData.MfData[0]=PICC_TRANSFER;
    MfComData.MfData[1]=addr;

    status=PcdComTransceive(pi, &ret_len);
    if (status!=MI_NOTAGERR)
    {
        if(MfComData.MfLength!=4)
        {
           status=MI_BITCOUNTERR;
        }
        else
        {
           MfComData.MfData[0]&=0x0f;
           switch(MfComData.MfData[0])
           {
              case 0x00:
                 status=MI_NOTAUTHERR;
                 break;
              case 0x0a:
                 status=MI_OK;
                 break;
              case 0x01:
                 status=MI_VALERR;
                 break;
              default:
                 status=MI_CODEERR;
                 break;
           }
        }
     }
   return status;
}
/*******************************************************************************
  * @brief  Mifare ultralight anticollision and select
  * @param  pSnr[OUT]:7Byte card SNR
  * @retval status=����״̬,�ɹ�����MI_OK
  * @note 	None	  
*******************************************************************************/
uint8_t UL_PcdAnticoll(uint8_t *pSnr)
{
	uint8_t status;
	uint8_t ret_len = 0;
    uint8_t i;
	uint8_t snr_check=0;
    struct TranSciveBuffer{uint8_t MfCommand;
                                 uint8_t MfLength;
                                 uint8_t MfData[7];
                                }MfComData;
    struct TranSciveBuffer *pi;
    pi=&MfComData;

    WriteRC(RegDecoderControl,0x28);
    ClearBitMask(RegControl,0x08);
    WriteRC(RegChannelRedundancy,0x03);
    PcdSetTmo(4);

    MfComData.MfCommand=PCD_TRANSCEIVE;
	MfComData.MfLength = 2;
    MfComData.MfData[0] = PICC_ANTICOLL1;
    MfComData.MfData[1] = 0x20;

    //status = PcdComISO14443(PCD_TRANSCEIVE,ucComBuf,2,ucComBuf,&unLen);
	status=PcdComTransceive(pi, &ret_len);
    
    if (status == MI_OK)
    {
    	 for (i=0; i<4; i++)
         {   
             *(pSnr+i)  = MfComData.MfData[i];
             snr_check ^= MfComData.MfData[i];
         }
         if (snr_check != MfComData.MfData[i])
         {   status = MI_COM_ERR;    }
         else
         {   memcpy(pSnr, &MfComData.MfData[1], 3);   }
    }
    
    ClearBitMask(RegDecoderControl,0x20);

    snr_check = 0x88;
    WriteRC(RegChannelRedundancy,0x0F);
    ClearBitMask(RegControl,0x08);
    PcdSetTmo(4);

	MfComData.MfCommand=PCD_TRANSCEIVE;
    MfComData.MfLength = 7;
    MfComData.MfData[0] = PICC_ANTICOLL1;
    MfComData.MfData[1] = 0x70;
    MfComData.MfData[2] = 0x88;
    for (i=0; i<3; i++)
    {
    	snr_check ^= *(pSnr+i);
    	MfComData.MfData[i+3] = *(pSnr+i);
    }
    MfComData.MfData[6] = snr_check;
    
    //status = PcdComISO14443(PCD_TRANSCEIVE,ucComBuf,7,ucComBuf,&unLen);
	status=PcdComTransceive(pi, &ret_len);
    if (status == MI_OK)
    {    
    	if (MfComData.MfLength != 0x08)
        {   status = MI_COM_ERR;     }
    }
    
    if (status == MI_OK)
    {
       snr_check = 0;
       WriteRC(RegDecoderControl,0x28);          
       ClearBitMask(RegControl,0x08);
       WriteRC(RegChannelRedundancy,0x03);
       PcdSetTmo(4);

       MfComData.MfCommand=PCD_TRANSCEIVE;
	   MfComData.MfLength = 2;
       MfComData.MfData[0] = PICC_ANTICOLL2;
       MfComData.MfData[1] = 0x20;
   
       //status = PcdComISO14443(PCD_TRANSCEIVE,ucComBuf,2,ucComBuf,&unLen);
	   status=PcdComTransceive(pi, &ret_len);
       if (!status)
       {
    	  for (i=0; i<4; i++)
          {  snr_check ^= MfComData.MfData[i];   }
          if (snr_check != MfComData.MfData[i])
          {  status = MI_COM_ERR;   }
          else
          {   memcpy(pSnr+3, &MfComData.MfData[0], 4);    }
       }
       ClearBitMask(RegDecoderControl,0x20);
    
       snr_check = 0;
       WriteRC(RegChannelRedundancy,0x0F);
       ClearBitMask(RegControl,0x08);
       PcdSetTmo(4);

       MfComData.MfCommand=PCD_TRANSCEIVE;
	   MfComData.MfLength = 7;
       MfComData.MfData[0] = PICC_ANTICOLL2;
       MfComData.MfData[1] = 0x70;
       for (i=0; i<4; i++)
       {
    	   snr_check ^= *(pSnr+3+i);
    	   MfComData.MfData[i+2] = *(pSnr+3+i);
       }
       MfComData.MfData[6] = snr_check;
       
       //status = PcdComISO14443(PCD_TRANSCEIVE,ucComBuf,7,ucComBuf,&unLen);
	   status=PcdComTransceive(pi, &ret_len);
       if (status == MI_OK)
       {    
       	   if (MfComData.MfLength != 0x08)
           {   status = MI_COM_ERR;   }
       }   	
    }
    return status;
}
/*******************************************************************************
  * @brief  Mifare ultralight write
  * @param  addr[IN]:page addr
  * @param  pData[IN]:4byte data
  * @retval status=����״̬,�ɹ�����MI_OK
  * @note 	None	  
*******************************************************************************/
uint8_t UL_PcdWrite(uint8_t addr,uint8_t *pData)
{
//    uint8_t status;
//    unsigned int  unLen;
//    unsigned uint8_t idata ucComBuf[FSD]; 
    uint8_t status;
	uint8_t ret_len = 0;
//   uint8_t i;
//	uint8_t snr_check=0;
    struct TranSciveBuffer{uint8_t MfCommand;
                                 uint8_t MfLength;
                                 uint8_t MfData[6];
                                }MfComData;
    struct TranSciveBuffer *pi;
    pi=&MfComData;

    PcdSetTmo(4);
    WriteRC(RegChannelRedundancy,0x07); 

    MfComData.MfCommand = PCD_TRANSCEIVE;
	MfComData.MfLength = 3;
    MfComData.MfData[0] = 0xA2;
    MfComData.MfData[1] = addr;
    memcpy(&MfComData.MfData[2], pData, 4);
	status=PcdComTransceive(pi, &ret_len);
    //status = PcdComISO14443(PCD_TRANSCEIVE,ucComBuf,6,ucComBuf,&unLen);
    
    if ((status != MI_OK) || (MfComData.MfLength != 4) || ((MfComData.MfData[0] & 0x0F) != 0x0A))
    {   status = MI_COM_ERR;   }
    
    PcdSetTmo(4);
    return status;
}
/*******************************************************************************
  * @brief  ��λMifare_Pro(X)
  * @param  param[IN]:FSDI+CID��ISO14443-4
  * @param  pLen[OUT]:��λ��Ϣ�ֽڳ���
  * @param  pData[OUT]:��λ��Ϣ
  * @retval status=����״̬,�ɹ�����MI_OK
  * @note 	None	  
*******************************************************************************/
uint8_t MifareProRst(uint8_t param,uint8_t *pLen,uint8_t *pData)
{
//    uint8_t status;
//    unsigned int  unLen;
//    unsigned uint8_t idata ucComBuf[FSD];
    uint8_t status;
	uint8_t ret_len = 0;
    struct TranSciveBuffer{uint8_t MfCommand;
                                 uint8_t MfLength;
                                 uint8_t MfData[2];
                                }MfComData;
    struct TranSciveBuffer *pi;
    pi=&MfComData;
    
    PcdSetTmo(15);
  
	MfComData.MfCommand = PCD_TRANSCEIVE;
	MfComData.MfLength = 2;
    MfComData.MfData[0] = PICC_RESET;
    MfComData.MfData[1] = param;
    g_bIblock = 0;

    //status = PcdComISO14443(PCD_TRANSCEIVE,ucComBuf,2,ucComBuf,&unLen);
	status=PcdComTransceive(pi, &ret_len);
    
    if ((*pLen = MfComData.MfLength/8) <=1 )
    {   memcpy(pData, MfComData.MfData, *pLen);   }    
    else
    {   status = MI_COM_ERR;   }
    return status;
}
/*******************************************************************************
  * @brief  ��T=CLЭ��Ŀ�����COSָ��
  * @param  CID[IN]:     �߰��ֽ�ָ���Ƿ����CID,�Ͱ��ֽڣ�CID
  * @param  pCommand[IN]:COSָ��
  * @param  ComLg[IN]:   COSָ���
  * @param  pAnswer[OUT]:��ƬӦ������
  * @param  pAnsLg[OUT]: Ӧ�����ݳ���
  * @retval status=����״̬,�ɹ�����MI_OK
  * @note 	None	  
*******************************************************************************/

uint8_t ISO14443CLCom(uint8_t CID, 
                   uint8_t *pCommand, 
                   uint8_t ComLg,
                   uint8_t *pAnswer,
                   uint8_t *pAnsLg)
{
//    uint8_t status;
//    unsigned int  unLen;
//    unsigned uint8_t idata ucComBuf[FSD];
//    
    uint8_t i,CidLg,PCB_I,PCB_R,a,b,c,d,e;
//    unsigned uint8_t idata sendlgok,sendlgnow,sendlgsum,recelgnow,recelgsum;
    uint8_t status;
	uint8_t ret_len = 0;
    struct TranSciveBuffer{uint8_t MfCommand;
                                 uint8_t MfLength;
                                 uint8_t MfData[3];
                                }MfComData;
    struct TranSciveBuffer *pi;
    pi=&MfComData;
    
//    recelgnow = 0x00;
//    recelgsum = 0x00;
//    sendlgnow = 0x00;
//    sendlgok  = 0x00;
//    sendlgsum = ComLg;
    a = 0x00;
	b = 0x00;
	c = 0x00;
	d = 0x00;
	e = ComLg;
    PCB_R  = 0xA2;
    
    PcdSetTmo(11);

    if (CID & 0xF0)
    {    CidLg = 1;    }
    else
    {    CidLg = 0;    }
    
    //if (e > FSD-1-CidLg)
	if (e > 3-1-CidLg)
    {
       // a  = FSD-1-CidLg;
	    a  = 3-1-CidLg;
        e -= c;
        PCB_I = 0x12 ^ g_bIblock;
    }    
    else
    {   
        c  = e;
        PCB_I = 0x02 ^ g_bIblock;
    }
    
    g_bIblock = !g_bIblock;

	MfComData.MfCommand = PCD_TRANSCEIVE;
	MfComData.MfLength	= c+CidLg+1	;
    MfComData.MfData[0] = (CidLg<<3) | PCB_I; 
    if (CidLg)
    {   MfComData.MfData[1] = CID & 0x0F;    }
    for (i=0; i<CidLg; i++)
    {  MfComData.MfData[i+1] = 0;  } 
    memcpy(&MfComData.MfData[CidLg+1], pCommand, c);   
    
    d += c;
    
   // status = PcdComISO14443(PCD_TRANSCEIVE,ucComBuf,sendlgnow+CidLg+1,ucComBuf,&unLen);
   status=PcdComTransceive(pi, &ret_len);
//////////////////////////////////////////////
    while (status == MI_OK)
    {
       // if (MfComData.MfLength/8-1-CidLg > FSD)
	    if (MfComData.MfLength/8-1-CidLg > 3)
        {   
            status = MI_COM_ERR;
            break;
        }
        if ((MfComData.MfData[0] & 0xF0) == 0x00) 
        //����ͨѶ����
        {
            a = MfComData.MfLength/8 - 1 - CidLg;
            memcpy(pAnswer+b, &MfComData.MfData[CidLg+1], a);    
            b += a;
            *pAnsLg = b;
            break;
        }  
        if ((MfComData.MfData[0] & 0xF0) == 0xA0)
        //���ͺ�������
        {
            for(i=0; i<CidLg; i++)
            {   MfComData.MfData[i+1] = 0;  }               
            PCB_I ^= 1;
           // if(e > FSD-1-CidLg)
		    if(e > 3-1-CidLg)
            {  
               // c  = FSD-1-CidLg; 
			    c  = 3-1-CidLg;
                e -= c; 
                PCB_I |= 0x10^ g_bIblock;
                g_bIblock = !g_bIblock;
            }    
            else
            {
            	c = e;
                PCB_I &= 0xEF;
                PCB_I |= g_bIblock;
                g_bIblock = !g_bIblock;
            }
			MfComData.MfCommand = PCD_TRANSCEIVE;
			MfComData.MfLength  = c+CidLg+1	;
            MfComData.MfData[0] = 0x02 | (CID<<3) | PCB_I;
            memcpy(&MfComData.MfData[CidLg+1], pCommand+d, c); 
            d += c;

//            status = PcdComISO14443(PCD_TRANSCEIVE,ucComBuf,sendlgnow+CidLg+1,ucComBuf,&unLen);
			status=PcdComTransceive(pi, &ret_len);
            continue;
        }
        if ((MfComData.MfData[0] & 0xF0) == 0x10)
        //���պ�������
        {
            a = MfComData.MfLength/8 - 1 - CidLg;
            memcpy(pCommand+b, &MfComData.MfData[CidLg+1], a);  
            b += a;
                          
            if(MfComData.MfData[0] & 0x01) 
            {  PCB_R &= 0xFE;  }
            else 
            {  PCB_R |= 0x01;  }
			MfComData.MfCommand = PCD_TRANSCEIVE;
			MfComData.MfLength  = CidLg+1	;
            MfComData.MfData[0] = PCB_R | (CID<<3) & 0xFB;
            for(i=0; i<CidLg; i++)
            {  MfComData.MfData[i+1]=0;  } 
            
//            status = PcdComISO14443(PCD_TRANSCEIVE,ucComBuf,CidLg+1,ucComBuf,&unLen);
            status=PcdComTransceive(pi, &ret_len); 
            continue;    
        }
        status = MI_COM_ERR;
        break;
    }     
    return status;
}
/*******************************************************************************
  * @brief  ISO14443-4 DESELECT
  * @param  CID[IN]:     �߰��ֽ�ָ���Ƿ����CID,�Ͱ��ֽڣ�CID
  * @retval status=����״̬,�ɹ�����MI_OK
  * @note 	None	  
*******************************************************************************/
uint8_t CL_Deselect(uint8_t CID)
{
//    uint8_t status;
//    unsigned int  unLen;
//    unsigned uint8_t idata ucComBuf[FSD];
    uint8_t status;
	uint8_t ret_len = 0;
    struct TranSciveBuffer{uint8_t MfCommand;
                                 uint8_t MfLength;
                                 uint8_t MfData[2];
                                }MfComData;
    struct TranSciveBuffer *pi;
    pi=&MfComData;
    
    PcdSetTmo(4);
	MfComData.MfCommand = PCD_TRANSCEIVE;
	MfComData.MfLength  = 2;
    MfComData.MfData[0] = 0xCA;
    MfComData.MfData[1] = CID;
    
    //status = PcdComISO14443(PCD_TRANSCEIVE,ucComBuf,2,ucComBuf,&unLen);
    status=PcdComTransceive(pi, &ret_len);
    return status;
}

/*******************************************************************************
  * @brief  ��RC500-EEPROM������
  * @param  startaddr=��ȡEEPROM����ʼ��ַ
  * @param  length=���ֽ���(<=16)
  * @param  readdata = ���������� 
  * @retval status=����״̬,�ɹ�����MI_OK
  * @note 	���������ݴ���readdataָ��ĵ�Ԫ 
*******************************************************************************/
uint8_t PcdReadE2(uint32_t startaddr,uint8_t length,uint8_t *readdata)
{
    uint8_t status;
	uint8_t ret_len = 0;
    uint8_t i;
    struct TranSciveBuffer{uint8_t MfCommand;
                                 uint8_t MfLength;
                                 uint8_t MfData[16];
                                }MfComData;
    struct TranSciveBuffer *pi;
    pi=&MfComData;

    MfComData.MfCommand=PCD_READE2;
    MfComData.MfLength=3;
    MfComData.MfData[0]=startaddr&0xff;
    MfComData.MfData[1]=(startaddr>>8)&0xff;
    MfComData.MfData[2]=length;
    status=PcdComTransceive(pi, &ret_len);

    if(status==MI_OK)
    {
        for(i=0;i<length;i++)
        {
            *(readdata+i)=MfComData.MfData[i];
        }
    }
    return status;
}
/*******************************************************************************
  * @brief  д���ݵ�RC500-EEPROM
  * @param  startaddr=д��EEPROM����ʼ��ַ
  * @param  length=���ֽ���(<=16)
  * @param  writedata=Ҫд���������ڴ��е��׵�ַ
  * @retval status=����״̬,�ɹ�����MI_OK
  * @note 	None
*******************************************************************************/
uint8_t PcdWriteE2(uint32_t startaddr,uint8_t length,uint8_t *writedata)
{
    uint8_t status;
	uint8_t ret_len = 0;
    uint8_t i;
    struct TranSciveBuffer{uint8_t MfCommand;
                                 uint8_t MfLength;
                                 uint8_t MfData[19];
                                }MfComData;
    struct TranSciveBuffer *pi;
    pi=&MfComData;

    MfComData.MfCommand=PCD_WRITEE2;
    MfComData.MfLength=length+2;
    MfComData.MfData[0]=startaddr&0xff;
    MfComData.MfData[1]=(startaddr>>8)&0xff;
    for(i=0;i<length;i++)
    {
        MfComData.MfData[i+2]=*(writedata+i);
    }
    status=PcdComTransceive(pi, &ret_len);
    return status;
}
/*******************************************************************************
  * @brief  ���Զ����RC500����
  * @param  startaddr=��ȡEEPROM����ʼ��ַ
  * @retval status=����״̬,�ɹ�����MI_OK
  * @note 	���ɽ��Զ���RC500�����ô�����EEPROM��10H��7FH��Ԫ��,
  *         ͨ���˺�������RC500�ļĴ���
*******************************************************************************/
uint8_t PcdLoadConfig(uint32_t startaddr)
{
    uint8_t status;
	uint8_t ret_len = 0;
    struct TranSciveBuffer{uint8_t MfCommand;
                                 uint8_t MfLength;
                                 uint8_t MfData[2];
                                }MfComData;
    struct TranSciveBuffer *pi;
    pi=&MfComData;

    MfComData.MfCommand=PCD_LOADCONFIG;
    MfComData.MfLength=2;
    MfComData.MfData[0]=startaddr&0xff;
    MfComData.MfData[1]=(startaddr>>8)&0xff;
    status=PcdComTransceive(pi, &ret_len);
    return status;
}
/*******************************************************************************
  * @brief  �ָ�RC500�ĳ�������
  * @param  None
  * @retval status=����״̬,�ɹ�����MI_OK
  * @note 	�����������RC500EEPROM�е�ַ10H��2FH������,���ô˺����ָ���
  *         �ٴγ�ʼ��RC500��������Ч
*******************************************************************************/
uint8_t PcdConfigRestore()
{
    uint8_t rc500config[32]=
    {
	 0x00,0x58,0x3f,0x3f,0x19,0x13,0x00,0x00,0x00,0x73,0x08,0xad,0xff,0x00,0x41,0x00,
     0x00,0x06,0x03,0x63,0x63,0x00,0x00,0x00,0x00,0x08,0x07,0x06,0x0a,0x02,0x00,0x00
	};
    uint8_t status;
    uint8_t i;
    uint8_t congfigdata[16];
    uint8_t *pa;
    pa=&congfigdata[0];
    for(i=0;i<16;i++)
    {
    	*(pa+i)=rc500config[i];
    }
    status=PcdWriteE2(0x0010,0x10,pa);
    if(status==MI_OK)
    {
        for(i=0;i<16;i++)
        {
    	    *(pa+i)=rc500config[i+16];
        }
    }
    status=PcdWriteE2(0x0020,0x10,pa);
    return status;
}
/*******************************************************************************
  * @brief  ������RC500��EEPROM�е���Կ�׵���RC500��FIFO
  * @param  startaddr=��ȡEEPROM����ʼ��ַ
  * @retval status=����״̬,�ɹ�����MI_OK
  * @note 	None
*******************************************************************************/
uint8_t PcdLoadKeyE2(uint32_t startaddr)
{
    uint8_t status;
	uint8_t ret_len = 0;
    struct TranSciveBuffer{uint8_t MfCommand;
                                 uint8_t MfLength;
                                 uint8_t MfData[2];
                                }MfComData;
    struct TranSciveBuffer *pi;
    pi=&MfComData;

    MfComData.MfCommand=PCD_LOADKEYE2;
    MfComData.MfLength=2;
    MfComData.MfData[0]=startaddr&0xff;
    MfComData.MfData[1]=(startaddr>>8)&0xff;
    status=PcdComTransceive(pi, &ret_len);
    return status;
}

/******************************************************************************
 *    ��������
*******************************************************************************/
extern C_APDU 							Command;
extern uint8_t 							uM24SRbuffer[];
/*******************************************************************************
  * @brief  
  * @param  
  * @param  
  * @retval status=����״̬,�ɹ�����MI_OK
  * @note 	
*******************************************************************************/
uint8_t PcdRATS(uint8_t *res, uint8_t *len)
{
    uint8_t status;
    struct TranSciveBuffer{uint8_t MfCommand;
                                 uint8_t MfLength;
                                 uint8_t MfData[15];
                                }MfComData;
    struct TranSciveBuffer *pi;

    pi=&MfComData;
    PcdSetTmo(106);
    WriteRC(RegChannelRedundancy,0x03);
    ClearBitMask(RegControl,0x08);
								
    MfComData.MfCommand=PCD_TRANSCEIVE;
    MfComData.MfLength= 4;
    MfComData.MfData[0] = 0xE0;
	MfComData.MfData[1] = 0x40;
	MfComData.MfData[2] = GETLSB( M24SR_ComputeCrc(MfComData.MfData, 2) );							
    MfComData.MfData[3] = GETMSB( M24SR_ComputeCrc(MfComData.MfData, 2) );
								
    status=PcdComTransceive(pi, len);
	if(MI_OK == status)
	{
		memcpy(res, pi->MfData, *len); 
		return MI_OK;
	}

   return status;
}
/*******************************************************************************
  * @brief  
  * @param  
  * @param  
  * @retval status=����״̬,�ɹ�����MI_OK
  * @note 	
*******************************************************************************/
uint8_t PcdPPS(uint8_t *res, uint8_t *len)
{
    uint8_t status;
    struct TranSciveBuffer{uint8_t MfCommand;
                                 uint8_t MfLength;
                                 uint8_t MfData[15];
                                }MfComData;
    struct TranSciveBuffer *pi;

    pi=&MfComData;
    PcdSetTmo(106);
    WriteRC(RegChannelRedundancy,0x03);
    ClearBitMask(RegControl,0x08);
								
    MfComData.MfCommand=PCD_TRANSCEIVE;
    MfComData.MfLength= 5;
    MfComData.MfData[0] = 0xD0;
	MfComData.MfData[1] = 0x11;
	MfComData.MfData[2] = 0x00;
	MfComData.MfData[3] = GETLSB( M24SR_ComputeCrc(MfComData.MfData, 3) );							
    MfComData.MfData[4] = GETMSB( M24SR_ComputeCrc(MfComData.MfData, 3) );
   
    status=PcdComTransceive(pi,len);
	if(MI_OK == status)
	{
		memcpy(res, pi->MfData, *len); 
		return MI_OK;
	} 

   return status;
}
/*******************************************************************************
  * @brief  
  * @param  
  * @param  
  * @retval status=����״̬,�ɹ�����MI_OK
  * @note 	
*******************************************************************************/
uint8_t PcdSelectApplication(uint8_t *res, uint8_t *len)
{
    uint8_t status;
	uint8_t ret_len = 0;
    struct TranSciveBuffer{uint8_t MfCommand;
                                 uint8_t MfLength;
                                 uint8_t MfData[30];
                                }MfComData;
    struct TranSciveBuffer *pi;

    pi=&MfComData;
    PcdSetTmo(106);
    WriteRC(RegChannelRedundancy,0x07);
    ClearBitMask(RegControl,0x08);

	M24SR_InitStructure();
	M24SR_SelectApplication(&ret_len);
								
    MfComData.MfCommand=PCD_TRANSCEIVE;
    MfComData.MfLength= ret_len;
    memcpy(MfComData.MfData, uM24SRbuffer, ret_len);
   
    status=PcdComTransceive(pi, len);
    if(MI_OK == status)
	{
		memcpy(res, pi->MfData, *len); 
		return MI_OK;
	}

   return status;
}
/*******************************************************************************
  * @brief  
  * @param  
  * @param  
  * @retval status=����״̬,�ɹ�����MI_OK
  * @note 	
*******************************************************************************/
uint8_t PcdSelectCCfile(uint8_t *res, uint8_t *len)
{
	uint8_t ret_len = 0;
    uint8_t status;
    struct TranSciveBuffer{uint8_t MfCommand;
                                 uint8_t MfLength;
                                 uint8_t MfData[30];
                                }MfComData;
    struct TranSciveBuffer *pi;

    pi=&MfComData;
    PcdSetTmo(106);
    WriteRC(RegChannelRedundancy,0x07);
    ClearBitMask(RegControl,0x08);

	M24SR_InitStructure();
	M24SR_SelectCCfile(&ret_len);
								
    MfComData.MfCommand=PCD_TRANSCEIVE;
    MfComData.MfLength= ret_len;
    memcpy(MfComData.MfData, uM24SRbuffer, ret_len);
   
    status=PcdComTransceive(pi, len);
    if(MI_OK == status)
	{
		memcpy(res, pi->MfData, *len); 
		return MI_OK;
	}
   return status;
}

/*******************************************************************************
  * @brief  
  * @param  
  * @param  
  * @retval status=����״̬,�ɹ�����MI_OK
  * @note 	
*******************************************************************************/
uint8_t PcdReadCCfileLength(uint8_t *res, uint8_t *len)
{
	uint8_t ret_len = 0;
    uint8_t status;
    struct TranSciveBuffer{uint8_t MfCommand;
                                 uint8_t MfLength;
                                 uint8_t MfData[30];
                                }MfComData;
    struct TranSciveBuffer *pi;

    pi=&MfComData;
    PcdSetTmo(106);
    WriteRC(RegChannelRedundancy,0x07);
    ClearBitMask(RegControl,0x08);

	M24SR_InitStructure();
	M24SR_ReadBinary(0x0000, 0x02, &ret_len);
								
    MfComData.MfCommand=PCD_TRANSCEIVE;
    MfComData.MfLength= ret_len;
    memcpy(MfComData.MfData, uM24SRbuffer, ret_len);
   
    status=PcdComTransceive(pi, len);
	if(MI_OK == status)
	{
		memcpy(res, pi->MfData, *len); 
		return MI_OK;
	}
   return status;
}

/*******************************************************************************
  * @brief  
  * @param  
  * @param  
  * @retval status=����״̬,�ɹ�����MI_OK
  * @note 	
*******************************************************************************/
uint8_t PcdReadCCfile(uint16_t Offset , uint8_t NbByteToRead, uint8_t *res, uint8_t *len)
{
	uint8_t ret_len = 0;
    uint8_t status;
    struct TranSciveBuffer{uint8_t MfCommand;
                                 uint8_t MfLength;
                                 uint8_t MfData[30];
                                }MfComData;
    struct TranSciveBuffer *pi;

    pi=&MfComData;
    PcdSetTmo(106);
    WriteRC(RegChannelRedundancy,0x07);
    ClearBitMask(RegControl,0x08);

	M24SR_InitStructure();
	M24SR_ReadBinary(Offset, NbByteToRead, &ret_len);
								
    MfComData.MfCommand=PCD_TRANSCEIVE;
    MfComData.MfLength= ret_len;
    memcpy(MfComData.MfData, uM24SRbuffer, ret_len);
   
    status=PcdComTransceive(pi, len);
	if(MI_OK == status)
	{
		memcpy(res, pi->MfData, *len); 
		return MI_OK;
	}
   return status;
}

/*******************************************************************************
  * @brief  
  * @param  
  * @param  
  * @retval status=����״̬,�ɹ�����MI_OK
  * @note 	
*******************************************************************************/
uint8_t PcdSelectNDEFfile(uint8_t *res, uint8_t *len)
{
	uint8_t ret_len = 0;
    uint8_t status;
    struct TranSciveBuffer{uint8_t MfCommand;
                                 uint8_t MfLength;
                                 uint8_t MfData[30];
                                }MfComData;
    struct TranSciveBuffer *pi;

    pi=&MfComData;
    PcdSetTmo(106);
    WriteRC(RegChannelRedundancy,0x07);
    ClearBitMask(RegControl,0x08);

	M24SR_InitStructure();
	M24SR_SelectNDEFfile(0x0001, &ret_len);
								
    MfComData.MfCommand=PCD_TRANSCEIVE;
    MfComData.MfLength= ret_len;
    memcpy(MfComData.MfData, uM24SRbuffer, ret_len);
   
    status=PcdComTransceive(pi, len);
	if(MI_OK == status)
	{
		memcpy(res, pi->MfData, *len); 
		return MI_OK;
	}
   return status;
}
/*******************************************************************************
  * @brief  
  * @param  
  * @param  
  * @retval status=����״̬,�ɹ�����MI_OK
  * @note 	
*******************************************************************************/
uint8_t PcdReadNDEFfileLength(uint8_t *res, uint8_t *len)
{
	uint8_t ret_len = 0;
    uint8_t status;
    struct TranSciveBuffer{uint8_t MfCommand;
                                 uint8_t MfLength;
                                 uint8_t MfData[30];
                                }MfComData;
    struct TranSciveBuffer *pi;

    pi=&MfComData;
    PcdSetTmo(106);			//�������Ĳ���106��ʲô���� by Henry 2016.5.18
    WriteRC(RegChannelRedundancy,0x07);
    ClearBitMask(RegControl,0x08);

	M24SR_InitStructure();
	M24SR_ReadBinary(0x0000, 0x02, &ret_len);
								
    MfComData.MfCommand=PCD_TRANSCEIVE;
    MfComData.MfLength= ret_len;
    memcpy(MfComData.MfData, uM24SRbuffer, ret_len);
   
    status=PcdComTransceive(pi, len);
	if(MI_OK == status)
	{
		memcpy(res, pi->MfData, *len); 
		return MI_OK;
	}
   return status;
}

/*******************************************************************************
  * @brief  
  * @param  
  * @param  
  * @retval status=����״̬,�ɹ�����MI_OK
  * @note 	
*******************************************************************************/
uint8_t PcdReadNDEFfile(uint16_t Offset , uint8_t NbByteToRead, uint8_t *res, uint8_t *len)
{
	uint8_t ret_len = 0;
    uint8_t status;
    struct TranSciveBuffer{uint8_t MfCommand;
                                 uint8_t MfLength;
                                 uint8_t MfData[BUF_LEN];
                                }MfComData;
    struct TranSciveBuffer *pi;

    pi=&MfComData;
    PcdSetTmo(106);
    WriteRC(RegChannelRedundancy,0x07);
    ClearBitMask(RegControl,0x08);

	M24SR_InitStructure();
	M24SR_ReadBinary(Offset, NbByteToRead, &ret_len);
								
    MfComData.MfCommand=PCD_TRANSCEIVE;
    MfComData.MfLength= ret_len;
    memcpy(MfComData.MfData, uM24SRbuffer, ret_len);
   
    status=PcdComTransceive(pi, len);
	if(MI_OK == status)
	{
		memcpy(res, pi->MfData, *len); 
		return MI_OK;
	}
   return status;
}

/*******************************************************************************
  * @brief  
  * @param  
  * @param  
  * @retval status=����״̬,�ɹ�����MI_OK
  * @note 	
*******************************************************************************/
uint8_t PcdWriteNDEFfile(uint16_t Offset , uint8_t NbByteToWrite , uint8_t *pDataToWrite, uint8_t *res, uint8_t *len)
{
	uint8_t ret_len = 0;
    uint8_t status;
    struct TranSciveBuffer{uint8_t MfCommand;
                                 uint8_t MfLength;
                                 uint8_t MfData[BUF_LEN];
                                }MfComData;
    struct TranSciveBuffer *pi;

    pi=&MfComData;
    PcdSetTmo(4);
    WriteRC(RegChannelRedundancy,0x07);
    ClearBitMask(RegControl,0x08);

	M24SR_InitStructure();
	M24SR_UpdateBinary(Offset, NbByteToWrite, pDataToWrite, &ret_len);
								
    MfComData.MfCommand=PCD_TRANSCEIVE;
    MfComData.MfLength= ret_len;
    memcpy(MfComData.MfData, uM24SRbuffer, ret_len);
   
    status=PcdComTransceive(pi, len);
	if(MI_OK == status)
	{
		memcpy(res, pi->MfData, *len); 
		return MI_OK;
	}
	memcpy(res, pi->MfData, *len);
	return status;
}

/*******************************************************************************
  * @brief  
  * @param  
  * @param  
  * @retval status=����״̬,�ɹ�����MI_OK
  * @note 	
*******************************************************************************/
uint8_t PcdSelectSystemfile(uint8_t *res, uint8_t *len)
{
	uint8_t ret_len = 0;
    uint8_t status;
    struct TranSciveBuffer{uint8_t MfCommand;
                                 uint8_t MfLength;
                                 uint8_t MfData[30];
                                }MfComData;
    struct TranSciveBuffer *pi;

    pi=&MfComData;
    PcdSetTmo(106);
    WriteRC(RegChannelRedundancy,0x07);
    ClearBitMask(RegControl,0x08);

	M24SR_InitStructure();
	M24SR_SelectSystemfile(&ret_len);
								
    MfComData.MfCommand=PCD_TRANSCEIVE;
    MfComData.MfLength= ret_len;
    memcpy(MfComData.MfData, uM24SRbuffer, ret_len);
   
    status=PcdComTransceive(pi, len);
	if(MI_OK == status)
	{
		memcpy(res, pi->MfData, *len); 
		return MI_OK;
	}
   return status;
}
/*******************************************************************************
  * @brief  
  * @param  
  * @param  
  * @retval status=����״̬,�ɹ�����MI_OK
  * @note 	
*******************************************************************************/
uint8_t PcdReadSystemfileLength(uint8_t *res, uint8_t *len)
{
	uint8_t ret_len = 0;
    uint8_t status;
    struct TranSciveBuffer{uint8_t MfCommand;
                                 uint8_t MfLength;
                                 uint8_t MfData[30];
                                }MfComData;
    struct TranSciveBuffer *pi;

    pi=&MfComData;
    PcdSetTmo(106);
    WriteRC(RegChannelRedundancy,0x07);
    ClearBitMask(RegControl,0x08);

	M24SR_InitStructure();
	M24SR_ReadBinary(0x0000, 0x02, &ret_len);
								
    MfComData.MfCommand=PCD_TRANSCEIVE;
    MfComData.MfLength= ret_len;
    memcpy(MfComData.MfData, uM24SRbuffer, ret_len);
   
    status=PcdComTransceive(pi, len);
	if(MI_OK == status)
	{
		memcpy(res, pi->MfData, *len); 
		return MI_OK;
	}
   return status;
}
/*******************************************************************************
  * @brief  
  * @param  
  * @param  
  * @retval status=����״̬,�ɹ�����MI_OK
  * @note 	
*******************************************************************************/
uint8_t PcdReadSystemfile(uint16_t Offset , uint8_t NbByteToRead, uint8_t *res, uint8_t *len)
{
	uint8_t ret_len = 0;
    uint8_t status;
    struct TranSciveBuffer{uint8_t MfCommand;
                                 uint8_t MfLength;
                                 uint8_t MfData[30];
                                }MfComData;
    struct TranSciveBuffer *pi;

    pi=&MfComData;
    PcdSetTmo(106);
    WriteRC(RegChannelRedundancy,0x07);
    ClearBitMask(RegControl,0x08);

	M24SR_InitStructure();
	M24SR_ReadBinary(Offset, NbByteToRead, &ret_len);
								
    MfComData.MfCommand=PCD_TRANSCEIVE;
    MfComData.MfLength= ret_len;
    memcpy(MfComData.MfData, uM24SRbuffer, ret_len);
   
    status=PcdComTransceive(pi, len);
	if(MI_OK == status)
	{
		memcpy(res, pi->MfData, *len); 
		return MI_OK;
	}
   return status;
}

/*******************************************************************************
  * @brief  
  * @param  
  * @param  
  * @retval status=����״̬,�ɹ�����MI_OK
  * @note 	
*******************************************************************************/
uint8_t PcdSendInterrupt (uint8_t *res, uint8_t *len)
{
	uint8_t ret_len = 0;
    uint8_t status;
    struct TranSciveBuffer{uint8_t MfCommand;
                                 uint8_t MfLength;
                                 uint8_t MfData[30];
                                }MfComData;
    struct TranSciveBuffer *pi;

    pi=&MfComData;
    PcdSetTmo(106);
    WriteRC(RegChannelRedundancy,0x07);
    ClearBitMask(RegControl,0x08);

	M24SR_InitStructure();
	M24SR_SendInterrupt(&ret_len);
								
    MfComData.MfCommand=PCD_TRANSCEIVE;
    MfComData.MfLength= ret_len;
    memcpy(MfComData.MfData, uM24SRbuffer, ret_len);
   
    status=PcdComTransceive(pi, len);
	if(MI_OK == status)
	{
		memcpy(res, pi->MfData, *len); 
		return MI_OK;
	}
   return status;
}

/*******************************************************************************
  * @brief  
  * @param  
  * @param  
  * @retval status=����״̬,�ɹ�����MI_OK
  * @note 	
*******************************************************************************/
uint8_t PcdDeselect (uint8_t *res, uint8_t *len)
{
    uint8_t status;
    struct TranSciveBuffer{uint8_t MfCommand;
                                 uint8_t MfLength;
                                 uint8_t MfData[15];
                                }MfComData;
    struct TranSciveBuffer *pi;

    pi=&MfComData;
    PcdSetTmo(106);
    WriteRC(RegChannelRedundancy,0x03);
    ClearBitMask(RegControl,0x08);
								
    MfComData.MfCommand=PCD_TRANSCEIVE;
    MfComData.MfLength= 3;
    MfComData.MfData[0] = 0xC2;			
	MfComData.MfData[1] = 0xE0;
	MfComData.MfData[2] = 0xB4;
   
    status=PcdComTransceive(pi,len);
	if(MI_OK == status)
	{
		memcpy(res, pi->MfData, *len); 
		return MI_OK;
	} 

   return status;
}

/*******************************************************************************
  * @brief  
  * @param  
  * @param  
  * @retval status=����״̬,�ɹ�����MI_OK
  * @note 	
*******************************************************************************/
uint8_t PcdFWTExtension (uint8_t FWTbyte, uint8_t *res, uint8_t *len)
{
    uint8_t ret_len = 0;
    uint8_t status;
    struct TranSciveBuffer{uint8_t MfCommand;
                                 uint8_t MfLength;
                                 uint8_t MfData[30];
                                }MfComData;
    struct TranSciveBuffer *pi;

    pi=&MfComData;
    PcdSetTmo(4);
    WriteRC(RegChannelRedundancy,0x03);
    ClearBitMask(RegControl,0x08);

	M24SR_FWTExtension(FWTbyte, &ret_len);
								
    MfComData.MfCommand=PCD_TRANSCEIVE;
    MfComData.MfLength= ret_len;
    memcpy(MfComData.MfData, uM24SRbuffer, ret_len);
   
    status=PcdComTransceive(pi, len);
	if(MI_OK == status)
	{
		memcpy(res, pi->MfData, *len); 
		return MI_OK;
	}
   return status;
}
/**************************************END OF FILE****************************/
