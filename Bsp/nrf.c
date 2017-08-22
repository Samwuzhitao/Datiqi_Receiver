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
#include "app_spi_send_data_process.h"

#ifdef NRF_DEBUG
#define nrf_debug  printf
#else
#define nrf_debug(...)
#endif

extern uint16_t            list_tcb_table[UID_LIST_TABLE_SUM][WHITE_TABLE_LEN];
extern wl_typedef          wl;
extern revicer_typedef     revicer;

#ifdef ZL_RP551_MAIN_F
void nrf1_spi_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);  
	
	/* Configure SPI_MISO Pin */
	GPIO_InitStructure.GPIO_Pin   = NRF1_SPI_MISO_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_Init(NRF1_SPI_MISO_PORT, &GPIO_InitStructure);

	/* Configure SPI_MOSI Pin */
	GPIO_InitStructure.GPIO_Pin   = NRF1_SPI_MOSI_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_Init(NRF1_SPI_MOSI_PORT, &GPIO_InitStructure);

	/* Configure SPI_SCK Pin */
	GPIO_InitStructure.GPIO_Pin   = NRF1_SPI_SCK_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_Init(NRF1_SPI_SCK_PORT, &GPIO_InitStructure);

	/* Configure SPI_CSN Pin */								//CSN 配置
	GPIO_InitStructure.GPIO_Pin   = NRF1_SPI_CSN_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_Init(NRF1_SPI_CSN_PORT, &GPIO_InitStructure);

	/* Configure SPI_CE Pin */
	GPIO_InitStructure.GPIO_Pin   = NRF1_SPI_CE_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_Init(NRF1_SPI_CE_PORT, &GPIO_InitStructure);

	/* Configure SPI_IRQ Pin */
	GPIO_InitStructure.GPIO_Pin   = NRF1_SPI_IRQ_PIN;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
	GPIO_Init(NRF1_SPI_IRQ_PORT, &GPIO_InitStructure);

	/* NRF1_SPI相关参数配置 */
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	/* 空闲为低 */
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	/* 第一个电平读取信号  模式0 */
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	/* 不超过2M */
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;

	SPI_Init(SPI1, &SPI_InitStructure);

  /* Connect EXTI5 Line to PC.05 pin */
  GPIO_EXTILineConfig(NRF1_RFIRQ_PortSource, GPIO_PinSource5);
	
	EXTI_InitStructure.EXTI_Line    = NRF1_EXTI_LINE_RFIRQ;
	EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	/* NRF1_SPI中断配置 */
	NVIC_PriorityGroupConfig(SYSTEM_MVIC_GROUP_SET);
	NVIC_InitStructure.NVIC_IRQChannel = NRF1_RFIRQ_EXTI_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = NRF1_PREEMPTION_PRIORITY;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = NRF_SUB_PRIORITY;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	SPI_Cmd(SPI1, ENABLE);
	NRF1_CSN_HIGH();		
}

void nrf2_spi_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;


	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);      
	
	/* Configure SPI_MISO Pin */
	GPIO_InitStructure.GPIO_Pin   = NRF2_SPI_MISO_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_Init(NRF2_SPI_MISO_PORT, &GPIO_InitStructure);

	/* Configure SPI_MOSI Pin */
	GPIO_InitStructure.GPIO_Pin   = NRF2_SPI_MOSI_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_Init(NRF2_SPI_MOSI_PORT, &GPIO_InitStructure);

	/* Configure SPI_SCK Pin */
	GPIO_InitStructure.GPIO_Pin   = NRF2_SPI_SCK_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_Init(NRF2_SPI_SCK_PORT, &GPIO_InitStructure);

	/* Configure SPI_CSN Pin */								//CSN 配置
	GPIO_InitStructure.GPIO_Pin   = NRF2_SPI_CSN_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_Init(NRF2_SPI_CSN_PORT, &GPIO_InitStructure);

	/* Configure SPI_CE Pin */
	GPIO_InitStructure.GPIO_Pin   = NRF2_SPI_CE_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_Init(NRF2_SPI_CE_PORT, &GPIO_InitStructure);

	/* Configure SPI_IRQ Pin */
	GPIO_InitStructure.GPIO_Pin   = NRF2_SPI_IRQ_PIN;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
	GPIO_Init(NRF2_SPI_IRQ_PORT, &GPIO_InitStructure);

	/* NRF2_SPI相关参数配置 */
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	/* 空闲为低 */
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	/* 第一个电平读取信号  模式0 */
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	/* 不超过2M */
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;

	SPI_Init(SPI2, &SPI_InitStructure);

	SPI_Cmd(SPI2, ENABLE);
	NRF2_CSN_HIGH();		
}
#endif

#ifdef ZL_RP551_MAIN_H
void nrf1_rst_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	/* Configure RST Pin */								//RST ??
	GPIO_InitStructure.GPIO_Pin   = NRF_RX_RST_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_Init(NRF_RX_RST_PORT, &GPIO_InitStructure);
}

void nrf2_rst_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	/* Configure RST Pin */								//RST ??
	GPIO_InitStructure.GPIO_Pin   = NRF_TX_RST_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_Init(NRF_TX_RST_PORT, &GPIO_InitStructure);
}

void nrf1_rst_deinit()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	/* Configure RST Pin */								//RST ??
	GPIO_InitStructure.GPIO_Pin   = NRF_RX_RST_PIN;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
	GPIO_Init(NRF_RX_RST_PORT, &GPIO_InitStructure);
}

void nrf2_rst_deinit()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	/* Configure RST Pin */								//RST ??
	GPIO_InitStructure.GPIO_Pin   = NRF_TX_RST_PIN;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
	GPIO_Init(NRF_TX_RST_PORT, &GPIO_InitStructure);
}

void nrf1_spi_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);  
	
	/* Configure SPI_MISO Pin */
	GPIO_InitStructure.GPIO_Pin   = NRF_RX_SPI_MISO_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_Init(NRF_RX_SPI_MISO_PORT, &GPIO_InitStructure);

	/* Configure SPI_MOSI Pin */
	GPIO_InitStructure.GPIO_Pin   = NRF_RX_SPI_MOSI_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_Init(NRF_RX_SPI_MOSI_PORT, &GPIO_InitStructure);

	/* Configure SPI_SCK Pin */
	GPIO_InitStructure.GPIO_Pin   = NRF_RX_SPI_SCK_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_Init(NRF_RX_SPI_SCK_PORT, &GPIO_InitStructure);

	/* Configure SPI_CSN Pin */								//CSN ??
	GPIO_InitStructure.GPIO_Pin   = NRF_RX_SPI_CSN_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_Init(NRF_RX_SPI_CSN_PORT, &GPIO_InitStructure);

	/* Configure SPI_CE Pin */
	GPIO_InitStructure.GPIO_Pin   = NRF_RX_SPI_CE_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_Init(NRF_RX_SPI_CE_PORT, &GPIO_InitStructure);

	/* Configure SPI_IRQ Pin */
	GPIO_InitStructure.GPIO_Pin   = NRF_RX_SPI_IRQ_PIN;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
	GPIO_Init(NRF_RX_SPI_IRQ_PORT, &GPIO_InitStructure);

	/* NRF_RX_SPI?????? */
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	/* ???? */
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	/* ?????????  ??0 */
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	/* ???2M */
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;

	SPI_Init(SPI1, &SPI_InitStructure);

  /* Connect EXTI5 Line to PC.05 pin */
  GPIO_EXTILineConfig(NRF_RX_RFIRQ_PortSource, GPIO_PinSource5);
	
	EXTI_InitStructure.EXTI_Line    = NRF_RX_EXTI_LINE_RFIRQ;
	EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	/* NRF_RX_SPI???? */
	NVIC_PriorityGroupConfig(SYSTEM_MVIC_GROUP_SET);
	NVIC_InitStructure.NVIC_IRQChannel = NRF_RX_RFIRQ_EXTI_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = NRF_RX_PREEMPTION_PRIORITY;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = NRF_SUB_PRIORITY;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	SPI_Cmd(SPI1, ENABLE);
	NRF_RX_CSN_HIGH();
	nrf1_rst_init();
	NRF_RX_RST_HIGH();	
}

void nrf2_spi_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | \
	                       RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);      
	
	/* Configure SPI_MISO Pin */
	GPIO_InitStructure.GPIO_Pin   = NRF_TX_SPI_MISO_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_Init(NRF_TX_SPI_MISO_PORT, &GPIO_InitStructure);

	/* Configure SPI_MOSI Pin */
	GPIO_InitStructure.GPIO_Pin   = NRF_TX_SPI_MOSI_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_Init(NRF_TX_SPI_MOSI_PORT, &GPIO_InitStructure);

	/* Configure SPI_SCK Pin */
	GPIO_InitStructure.GPIO_Pin   = NRF_TX_SPI_SCK_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_Init(NRF_TX_SPI_SCK_PORT, &GPIO_InitStructure);

	/* Configure SPI_CSN Pin */								//CSN ??
	GPIO_InitStructure.GPIO_Pin   = NRF_TX_SPI_CSN_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_Init(NRF_TX_SPI_CSN_PORT, &GPIO_InitStructure);

	/* Configure SPI_CE Pin */
	GPIO_InitStructure.GPIO_Pin   = NRF_TX_SPI_CE_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_Init(NRF_TX_SPI_CE_PORT, &GPIO_InitStructure);

	/* Configure SPI_IRQ Pin */
	GPIO_InitStructure.GPIO_Pin   = NRF_TX_SPI_IRQ_PIN;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
	GPIO_Init(NRF_TX_SPI_IRQ_PORT, &GPIO_InitStructure);

	/* NRF_TX_SPI?????? */
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	/* ???? */
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	/* ?????????  ??0 */
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	/* ???2M */
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;

	SPI_Init(SPI3, &SPI_InitStructure);

  /* Connect EXTI2 Line to PD.02 pin */
  GPIO_EXTILineConfig(NRF_TX_RFIRQ_PortSource, GPIO_PinSource2);
	EXTI_InitStructure.EXTI_Line    = NRF_TX_EXTI_LINE_RFIRQ;
	EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	/* NRF_RX_SPI???? */
	NVIC_PriorityGroupConfig(SYSTEM_MVIC_GROUP_SET);
	NVIC_InitStructure.NVIC_IRQChannel = NRF_TX_RFIRQ_EXTI_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = NRF_TX_PREEMPTION_PRIORITY;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = NRF_SUB_PRIORITY;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	SPI_Cmd(SPI3, ENABLE);
	NRF_TX_CSN_HIGH();
	nrf2_rst_init();
	NRF_TX_RST_HIGH();
}
#endif
/******************************************************************************
  Function:my_nrf_transmit_start
  Description:
  Input:
	    data_buff：	   要发送的数组
			data_buff_len：要发送的数组长度
			nrf_data_type：发送数据类型，有效数据:NRF_DATA_IS_USEFUL
			ACK		:        NRF_DATA_IS_ACK
  Output:
  Return:
  Others:注意：通信方式限制，若向同一UID答题器下发数据，时间要间隔3S以上
******************************************************************************/
void nrf51822_spi_init(void)
{
#ifdef ZL_RP551_MAIN_F
	nrf1_spi_init();
	nrf2_spi_init();
#endif

#ifdef ZL_RP551_MAIN_H
	nrf1_spi_init();
	nrf2_spi_init();
#endif
}



void TIM3_Int_Init(u16 arr,u16 psc)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //时钟使能

	//定时器TIM3初始化
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位

	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); //使能指定的TIM3中断,允许更新中断

	//中断优先级NVIC设置
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = TIM3_PREEMPTION_PRIORITY;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = TIM3_SUB_PRIORITY;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器
}

/**************************************END OF FILE****************************/


