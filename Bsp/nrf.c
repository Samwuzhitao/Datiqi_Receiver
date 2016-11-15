/**
  ******************************************************************************
  * @file   	hal_nrf.c
  * @author  	Tian erjun
  * @version 	V1.0.0.0
  * @date   	2015.11.05
  * @brief   	hal function for nrf moulde
  ******************************************************************************
  */

#include "main.h"
#include "nrf.h"
#include "app_send_data_process.h"


#ifdef NRF_DEBUG
#define nrf_debug  printf
#else
#define nrf_debug(...)
#endif

extern nrf_communication_t nrf_communication;
extern uint16_t            list_tcb_table[10][8];
extern WhiteList_Typedef wl;
extern Revicer_Typedef   revicer;
/******************************************************************************
  Function:my_nrf_transmit_start
  Description:
  Input:
	    data_buff��	   Ҫ���͵�����
			data_buff_len��Ҫ���͵����鳤��
			nrf_data_type�������������ͣ���Ч����:NRF_DATA_IS_USEFUL
			ACK		:        NRF_DATA_IS_ACK
  Output:
  Return:
  Others:ע�⣺ͨ�ŷ�ʽ���ƣ�����ͬһUID�������·����ݣ�ʱ��Ҫ���3S����
******************************************************************************/
void nrf51822_spi_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;

	/* Configure SPI_MISO Pin */
	GPIO_InitStructure.GPIO_Pin   = SPI_MISO_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_Init(SPI_MISO_PORT, &GPIO_InitStructure);

	/* Configure SPI_MOSI Pin */
	GPIO_InitStructure.GPIO_Pin   = SPI_MOSI_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_Init(SPI_MOSI_PORT, &GPIO_InitStructure);

	/* Configure SPI_SCK Pin */
	GPIO_InitStructure.GPIO_Pin   = SPI_SCK_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_Init(SPI_SCK_PORT, &GPIO_InitStructure);

	/* Configure SPI_CSN Pin */								//CSN_1 ����
	GPIO_InitStructure.GPIO_Pin   = SPI_CSN_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_Init(SPI_CSN_PORT, &GPIO_InitStructure);

	/* Configure SPI_CSN Pin */								//CSN_2 ����
	GPIO_InitStructure.GPIO_Pin   = SPI_CSN_PIN_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_Init(SPI_CSN_PORT_2, &GPIO_InitStructure);

	/* Configure SPI_CE Pin */
	GPIO_InitStructure.GPIO_Pin   = SPI_CE_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_Init(SPI_CE_PORT, &GPIO_InitStructure);

	/* Configure SPI_IRQ Pin */
	GPIO_InitStructure.GPIO_Pin   = SPI_IRQ_PIN;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
	GPIO_Init(SPI_IRQ_PORT, &GPIO_InitStructure);

	/* SPI��ز������� */
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;			//����Ϊ��
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;		//��һ����ƽ��ȡ�ź�  ģʽ0
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;		//������2M
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;

	SPI_Init(SPI1, &SPI_InitStructure);

	/* SPI�ж����� */
	NVIC_PriorityGroupConfig(SYSTEM_MVIC_GROUP_SET);
	NVIC_InitStructure.NVIC_IRQChannel = RFIRQ_EXTI_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = NRF_PREEMPTION_PRIORITY;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = NRF_SUB_PRIORITY;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	GPIO_EXTILineConfig(RFIRQ_PortSource, RFIRQ_PinSource);
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_Line = EXTI_LINE_RFIRQ;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	SPI_Cmd(SPI1, ENABLE);
	SPI_CSN_HIGH();		//Ƭѡ���ߣ���ֹSPI
	SPI_CSN_HIGH_2();	//�������˷��Ұ���ʱ��
}



void TIM3_Int_Init(u16 arr,u16 psc)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //ʱ��ʹ��

	//��ʱ��TIM3��ʼ��
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ

	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM3�ж�,��������ж�

	//�ж����ȼ�NVIC����
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = TIM3_PREEMPTION_PRIORITY;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = TIM3_SUB_PRIORITY;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���

}

/******************************************************************************
  Function:nrf_transmit_start
  Description:
  Input:	data_buff��	   Ҫ���͵�����
			data_buff_len��Ҫ���͵����鳤��
			nrf_data_type�������������ͣ���Ч����:NRF_DATA_IS_USEFUL
										ACK		:NRF_DATA_IS_ACK
  Output:
  Return:
  Others:ע�⣺ͨ�ŷ�ʽ���ƣ�����ͬһUID�������·����ݣ�ʱ��Ҫ���3S����
******************************************************************************/
void nrf_transmit_start(uint8_t *data_buff, uint8_t data_buff_len,uint8_t nrf_data_type,
												uint8_t count, uint8_t delay100us, uint8_t sel_table)
{
	if(nrf_data_type == NRF_DATA_IS_USEFUL)		//��Ч���ݰ�������nrf_communication.transmit_buf����
	{
		/* data header */
		nrf_communication.transmit_buf[0]  = 0x61;
		memcpy((nrf_communication.transmit_buf + 1), nrf_communication.dtq_uid, 4);
		memcpy((nrf_communication.transmit_buf + 5), nrf_communication.jsq_uid, 4);
		nrf_communication.transmit_buf[9]  = revicer.sen_seq++;
		nrf_communication.transmit_buf[10] = revicer.sen_num;
		nrf_communication.transmit_buf[11] = NRF_DATA_IS_USEFUL;
		nrf_communication.transmit_buf[12] = 0xFF;
		nrf_communication.transmit_buf[13] = 0xFF;

		/* len */
		nrf_communication.transmit_buf[14] = data_buff_len;

		/* get data */
		memcpy((nrf_communication.transmit_buf + 15), data_buff, data_buff_len);	//��Ч���ݴӵ�10λ��ʼ��

		/* ����Ƿ�Ϊ�����ط�֡������������״̬������ */
		memcpy(nrf_communication.transmit_buf+15 + data_buff_len, list_tcb_table[sel_table], 16);
#ifdef OPEN_ACT_TABLE_SHOW
		{
			int i = 0;
			printf("ACK TABLE[%d]:",sel_table);
			for(i=0;i<8;i++)
			{
				printf("%04x ",list_tcb_table[sel_table][i]);
			}
			printf("\r\n");
		}
#endif

		/* xor data */
		nrf_communication.transmit_buf[15+16 + data_buff_len] = XOR_Cal(nrf_communication.transmit_buf+1,14+data_buff_len+16);
		nrf_communication.transmit_buf[16+16 + data_buff_len] = 0x21;

		nrf_communication.transmit_len = data_buff_len + 17+16;

		/* ��ʼͨѶ֮ǰ�ȷ�2�Σ�֮������ʱ�ж��ط����� */
		uesb_nrf_write_tx_payload(nrf_communication.transmit_buf,nrf_communication.transmit_len,count,delay100us);
	}
	else if(nrf_data_type == NRF_DATA_IS_ACK)	//ACK���ݰ�������nrf_communication.transmit_buf ����
	{
		uint8_t uidpos;
		search_uid_in_white_list(&uidpos,nrf_communication.dtq_uid);
		nrf_communication.transmit_buf[0] = 0x61;
		memcpy((nrf_communication.transmit_buf + 1), wl.uids[uidpos].uid, 4);
		memcpy((nrf_communication.transmit_buf + 5), nrf_communication.jsq_uid, 4);
		nrf_communication.transmit_buf[9]  = wl.uids[uidpos].rev_num;
		nrf_communication.transmit_buf[10] = wl.uids[uidpos].rev_seq;
		nrf_communication.transmit_buf[11] = NRF_DATA_IS_ACK;
		nrf_communication.transmit_buf[12] = 0xFF;
		nrf_communication.transmit_buf[13] = 0xFF;
		nrf_communication.transmit_buf[14] = 0;
		nrf_communication.transmit_buf[15] = XOR_Cal(nrf_communication.transmit_buf+1,14);
		nrf_communication.transmit_buf[16] = 0x21;

		nrf_communication.transmit_len = 17;

		uesb_nrf_write_tx_payload(nrf_communication.transmit_buf,nrf_communication.transmit_len,count,delay100us);
	}
	else if( nrf_data_type == NRF_DATA_IS_PRE )
	{
		nrf_communication.transmit_buf[0] = 0x61;
		memcpy((nrf_communication.transmit_buf + 1), nrf_communication.dtq_uid, 4);
		memcpy((nrf_communication.transmit_buf + 5), nrf_communication.jsq_uid, 4);
		nrf_communication.transmit_buf[9]  = revicer.pre_seq++;
		nrf_communication.transmit_buf[10] = revicer.sen_num;
		nrf_communication.transmit_buf[11] = NRF_DATA_IS_PRE;
		nrf_communication.transmit_buf[12] = 0xFF;
		nrf_communication.transmit_buf[13] = 0xFF;
		/* len */
		nrf_communication.transmit_buf[14] = 0;
		/* get data */
		memcpy(nrf_communication.transmit_buf+15 + data_buff_len, list_tcb_table[sel_table], 16);
#ifdef OPEN_ACT_TABLE_SHOW
		{
			int i = 0;
			printf("\r\nSUM TABLE[%d]:",sel_table);
			for(i=0;i<8;i++)
			{
				printf("%04x ",list_tcb_table[sel_table][i]);
			}
			printf("\r\n");
		}
#endif

		nrf_communication.transmit_buf[15+16 + data_buff_len] = XOR_Cal(nrf_communication.transmit_buf+1,14+data_buff_len+16);
		nrf_communication.transmit_buf[16+16 + data_buff_len] = 0x21;

		nrf_communication.transmit_len = 17+16;

		uesb_nrf_write_tx_payload(nrf_communication.transmit_buf,nrf_communication.transmit_len,count,delay100us);
		DelayMs(10);
	}
}

/**************************************END OF FILE****************************/


