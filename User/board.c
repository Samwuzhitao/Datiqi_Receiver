/**
  ******************************************************************************
  * @file   	init.c
  * @author  	Tian erjun
  * @version 	V1.0.0.0
  * @date   	2015.11.05
  * @brief   	platform init functions
  ******************************************************************************
  */

#include "main.h"
#include "app_timer.h"
#include "app_send_data_process.h"
#include "app_systick_package_process.h"
#include "app_spi_send_data_process.h"
#include "app_card_process.h"

/* Private variables ---------------------------------------------------------*/
spi_cmd_type_t 			spi_cmd;
nrf_communication_t	nrf_data;
uint8_t             jsq_uid[8];
bool 						    gbf_hse_setup_fail = FALSE;
RF_TypeDef 				  rf_var;
/* Private variables ---------------------------------------------------------*/
#ifdef ZL_RP551_MAIN_E
const uint8_t hardware[30] = {
		0x5a,0x4c,0x2d,0x52,0x50,0x35,0x35,0x31,0x2d,0x4d,
		0x41,0x49,0x4e,0x2d,0x45 
};
#define MAIN_VERSION          0x00
#define SUB_VERSION_1         0x01
#define SUB_VERSION_2         0x01
#endif

#ifdef ZL_RP551_MAIN_F
const uint8_t hardware[30] = {
		0x5a,0x4c,0x2d,0x52,0x50,0x35,0x35,0x31,0x2d,0x4d,
		0x41,0x49,0x4e,0x2d,0x46 
};
#define MAIN_VERSION          0x00
#define SUB_VERSION_1         0x01
#define SUB_VERSION_2         0x03
#endif

#ifdef ZL_RP551_MAIN_H
const uint8_t hardware[30] = {
		0x5a,0x4c,0x2d,0x52,0x50,0x35,0x35,0x32,0x2d,0x4d,
		0x41,0x49,0x4e,0x2d,0x41
};
#define MAIN_VERSION          0x00
#define SUB_VERSION_1         0x03
#define SUB_VERSION_2         0x00
#endif

const uint8_t company[16] = {
		0xd,0x1,0xb,0x6,0xc,0x1,0xa,0xa,
		0xb,0x9,0xc,0x9,0xb,0x7,0xd,0xd
};

const uint8_t software[3] = {
	MAIN_VERSION,
	SUB_VERSION_1,
	SUB_VERSION_2
};

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

	/* Configure SPI_CSN Pin */								//CSN ÅäÖÃ
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

	/* NRF1_SPIÏà¹Ø²ÎÊýÅäÖÃ */
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	/* ¿ÕÏÐÎªµÍ */
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	/* µÚÒ»¸öµçÆ½¶ÁÈ¡ÐÅºÅ  Ä£Ê½0 */
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	/* ²»³¬¹ý2M */
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

	/* NRF1_SPIÖÐ¶ÏÅäÖÃ */
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

	/* Configure SPI_CSN Pin */								//CSN ÅäÖÃ
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

	/* NRF2_SPIÏà¹Ø²ÎÊýÅäÖÃ */
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	/* ¿ÕÏÐÎªµÍ */
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	/* µÚÒ»¸öµçÆ½¶ÁÈ¡ÐÅºÅ  Ä£Ê½0 */
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	/* ²»³¬¹ý2M */
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
	GPIO_InitStructure.GPIO_Pin   = NRF1_RST_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_Init(NRF1_RST_PORT, &GPIO_InitStructure);
}

void nrf2_rst_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	/* Configure RST Pin */								//RST ??
	GPIO_InitStructure.GPIO_Pin   = NRF2_RST_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_Init(NRF2_RST_PORT, &GPIO_InitStructure);
}

void nrf1_rst_deinit()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	/* Configure RST Pin */								//RST ??
	GPIO_InitStructure.GPIO_Pin   = NRF1_RST_PIN;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
	GPIO_Init(NRF1_RST_PORT, &GPIO_InitStructure);
}

void nrf2_rst_deinit()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	/* Configure RST Pin */								//RST ??
	GPIO_InitStructure.GPIO_Pin   = NRF2_RST_PIN;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
	GPIO_Init(NRF2_RST_PORT, &GPIO_InitStructure);
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

	/* Configure SPI_CSN Pin */								//CSN ??
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

	/* NRF1_SPI?????? */
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
  GPIO_EXTILineConfig(NRF1_RFIRQ_PortSource, GPIO_PinSource5);
	
	EXTI_InitStructure.EXTI_Line    = NRF1_EXTI_LINE_RFIRQ;
	EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	/* NRF1_SPI???? */
	NVIC_PriorityGroupConfig(SYSTEM_MVIC_GROUP_SET);
	NVIC_InitStructure.NVIC_IRQChannel = NRF1_RFIRQ_EXTI_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = NRF1_PREEMPTION_PRIORITY;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = NRF_SUB_PRIORITY;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	SPI_Cmd(SPI1, ENABLE);
	NRF1_CSN_HIGH();
	nrf1_rst_init();
	NRF1_RST_HIGH();	
}

void nrf2_spi_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;

	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | \
	                       RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);      
	
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

	/* Configure SPI_CSN Pin */								//CSN ??
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

	/* NRF2_SPI?????? */
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

	SPI_Cmd(SPI3, ENABLE);
	NRF2_CSN_HIGH();
	nrf2_rst_init();
	NRF2_RST_HIGH();
}
#endif

void nrf51822_spi_init(void)
{
#ifdef ZL_RP551_MAIN_E
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

	/* Configure SPI_CSN Pin */								//CSN_1 ÅäÖÃ
	GPIO_InitStructure.GPIO_Pin   = SPI_CSN_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_Init(SPI_CSN_PORT, &GPIO_InitStructure);

	/* Configure SPI_CSN Pin */								//CSN_2 ÅäÖÃ
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

	/* SPIÏà¹Ø²ÎÊýÅäÖÃ */
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;			//¿ÕÏÐÎªµÍ
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;		//µÚÒ»¸öµçÆ½¶ÁÈ¡ÐÅºÅ  Ä£Ê½0
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;		//²»³¬¹ý2M
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;

	SPI_Init(SPI1, &SPI_InitStructure);

	/* SPIÖÐ¶ÏÅäÖÃ */
	NVIC_PriorityGroupConfig(SYSTEM_MVIC_GROUP_SET);
	NVIC_InitStructure.NVIC_IRQChannel = RFIRQ_EXTI_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = NRF1_PREEMPTION_PRIORITY;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = NRF_SUB_PRIORITY;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	GPIO_EXTILineConfig(RFIRQ_PortSource, RFIRQ_PinSource);
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_Line = NRF1_EXTI_LINE_RFIRQ;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	SPI_Cmd(SPI1, ENABLE);
	SPI_CSN_HIGH();	
	SPI_CSN_HIGH_2();	

	GPIOInit_SE2431L();
#endif

#ifdef ZL_RP551_MAIN_F
	nrf1_spi_init();
	nrf2_spi_init();
#endif

#ifdef ZL_RP551_MAIN_H
	nrf1_spi_init();
	nrf2_spi_init();
#endif
}

/******************************************************************************
  Function:nrf_transmit_start
  Description:
  Input:	data_buff£º	   Òª·¢ËÍµÄÊý×é
			data_buff_len£ºÒª·¢ËÍµÄÊý×é³¤¶È
			nrf_data_type£º·¢ËÍÊý¾ÝÀàÐÍ£¬ÓÐÐ§Êý¾Ý:NRF_DATA_IS_USEFUL
										ACK		:NRF_DATA_IS_ACK
  Output:
  Return:
  Others:×¢Òâ£ºÍ¨ÐÅ·½Ê½ÏÞÖÆ£¬ÈôÏòÍ¬Ò»UID´ðÌâÆ÷ÏÂ·¢Êý¾Ý£¬Ê±¼äÒª¼ä¸ô3SÒÔÉÏ
******************************************************************************/
void nrf_transmit_start(uint8_t *tbuf, uint8_t buf_len, uint8_t buf_t,
												uint8_t cnt, uint8_t us, uint8_t sel_table, uint8_t pack_t)
{
	if( pack_t == 1 )
		revicer.sen_num++;

	if( buf_t == NRF_DATA_IS_USEFUL )		//ÓÐÐ§Êý¾Ý°ü£¬·¢ËÍnrf_data.tbufÄÚÈÝ
	{
		/* data header */
		nrf_data.tbuf[0]  = 0x61;
		memcpy((nrf_data.tbuf + 1), nrf_data.dtq_uid, 4);
		memcpy((nrf_data.tbuf + 5), nrf_data.jsq_uid, 4);
		nrf_data.tbuf[9]  = revicer.sen_seq++;
		nrf_data.tbuf[10] = revicer.sen_num;
		nrf_data.tbuf[11] = NRF_DATA_IS_USEFUL;
		nrf_data.tbuf[12] = 0xFF;
		nrf_data.tbuf[13] = 0xFF;

		/* len */
		nrf_data.tbuf[14] = buf_len;

		/* get data */
		memcpy((nrf_data.tbuf + 15), tbuf, buf_len);	//ÓÐÐ§Êý¾Ý´ÓµÚ10Î»¿ªÊ¼·Å

		/* ¼ì²âÊÇ·ñÎª¶¨ÏòÖØ·¢Ö¡£¬Èç¹ûÊÇÔò¼ÓÈë×´Ì¬Ë÷Òý±í */
		memcpy(nrf_data.tbuf+15 + buf_len, list_tcb_table[sel_table], 16);
#ifdef OPEN_ACT_TABLE_SHOW
		{
			int i = 0;
			printf("Seq:%2x Pac:%2x ",revicer.pre_seq-1,revicer.sen_num);
			printf("ACK TABLE[%2d]:",sel_table);
			for(i=0;i<8;i++)
			{
				printf("%04x ",list_tcb_table[sel_table][i]);
			}
			printf("\r\n");
		}
#endif

		/* xor data */
		nrf_data.tbuf[15+16 + buf_len] = XOR_Cal(nrf_data.tbuf+1,14+buf_len+16);
		nrf_data.tbuf[16+16 + buf_len] = 0x21;

		nrf_data.tlen = buf_len + 17+16;

		/* ¿ªÊ¼Í¨Ñ¶Ö®Ç°ÏÈ·¢2´Î£¬Ö®ºó¿ªÆô¶¨Ê±ÅÐ¶ÏÖØ·¢»úÖÆ */
		spi_write_data_to_buf(nrf_data.tbuf,nrf_data.tlen,cnt,us,1);
	}
	else if( buf_t == NRF_DATA_IS_ACK )	//ACKÊý¾Ý°ü£¬·¢ËÍnrf_data.tbuf ÄÚÈÝ
	{
		uint8_t uidpos;
		search_uid_in_white_list(nrf_data.dtq_uid,&uidpos);
		nrf_data.tbuf[0] = 0x61;
		memcpy((nrf_data.tbuf + 1), wl.uids[uidpos].uid, 4);
		memcpy((nrf_data.tbuf + 5), nrf_data.jsq_uid, 4);
		nrf_data.tbuf[9]  = wl.uids[uidpos].rev_num;
		nrf_data.tbuf[10] = wl.uids[uidpos].rev_seq;
		nrf_data.tbuf[11] = NRF_DATA_IS_ACK;
		nrf_data.tbuf[12] = 0xFF;
		nrf_data.tbuf[13] = 0xFF;
		nrf_data.tbuf[14] = 0;
		nrf_data.tbuf[15] = XOR_Cal(nrf_data.tbuf+1,14);
		nrf_data.tbuf[16] = 0x21;

		nrf_data.tlen = 17;
	  spi_write_data_to_buf(nrf_data.tbuf,nrf_data.tlen,cnt,us,1);
	}
	else if( buf_t == NRF_DATA_IS_PRE )
	{
		nrf_data.tbuf[0] = 0x61;
		memcpy((nrf_data.tbuf + 1), nrf_data.dtq_uid, 4);
		memcpy((nrf_data.tbuf + 5), nrf_data.jsq_uid, 4);
		nrf_data.tbuf[9]  = revicer.pre_seq++;
		nrf_data.tbuf[10] = revicer.sen_num;
		nrf_data.tbuf[11] = NRF_DATA_IS_PRE;
		nrf_data.tbuf[12] = 0xFF;
		nrf_data.tbuf[13] = 0xFF;
		/* len */
		nrf_data.tbuf[14] = 0;
		/* get data */
		memcpy(nrf_data.tbuf+15 + buf_len, list_tcb_table[sel_table], 16);
#ifdef OPEN_ACT_TABLE_SHOW
		{
			int i = 0;
			printf("Seq:%2x Pac:%2x ",revicer.pre_seq-1,revicer.sen_num);
			printf("SUM TABLE[%2d]:",sel_table);
			for(i=0;i<8;i++)
			{
				printf("%04x ",list_tcb_table[sel_table][i]);
			}
			printf("\r\n");
		}
#endif

		nrf_data.tbuf[15+16 + buf_len] = XOR_Cal(nrf_data.tbuf+1,14+buf_len+16);
		nrf_data.tbuf[16+16 + buf_len] = 0x21;

		nrf_data.tlen = 17+16;
		spi_write_data_to_buf(nrf_data.tbuf,nrf_data.tlen, cnt, us, 10);
	}
}

/*******************************************************************************
  * @brief  硬件平台初始化
  * @param  None
  * @retval None
  * @note 	None
*******************************************************************************/
void bsp_board_init(void)
{
	uint8_t status = 0;
	/* disable all IRQ */
	DISABLE_ALL_IRQ();

	/* initialize system clock */
	SysClockInit();

	/* initialize gpio port */
	GpioInit();

	bsp_uart1_init();

	/* get mcu uuid */
	bsp_device_id_init();

	/* initialize the spi interface with nrf51822 */
	nrf51822_spi_init();

	/* eeprom init and white_list init*/
	Fee_Init(FEE_INIT_POWERUP);
	get_white_list_from_flash();

	/* init software timer */
	sw_timer_init();
	system_timer_init();
	send_data_process_timer_init();
	card_timer_init();

	/* 复位并初始化RC500 */
	status = mfrc500_init();
	printf("[ INIT ] MF1702 INIT: %s !\r\n", (status == 0) ? "OK" : "FAIL");

	/* enable all IRQ */
	ENABLE_ALL_IRQ();

	/* led 、蜂鸣器声音提示初始化完毕 */
#ifndef OPEN_SILENT_MODE
	BEEP_EN();
#endif
	ledOn(LRED);
	ledOn(LBLUE);
	DelayMs(200);
	BEEP_DISEN();
	ledOff(LBLUE);
	clicker_config_default_set();
	IWDG_Configuration();
}

/****************************************************************************
* 名    称：void bsp_uart1_init(void)
* 功    能：串口1初始化函数
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无
****************************************************************************/
void bsp_uart1_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(USART1pos_CLK , ENABLE);

	GPIO_InitStructure.GPIO_Pin = USART1pos_TxPin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(USART1pos_GPIO, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = USART1pos_RxPin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(USART1pos_GPIO, &GPIO_InitStructure);

	USART_InitStructure.USART_BaudRate = UART_BAUD;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	/* Configure USART1 */
	USART_Init(USART1pos, &USART_InitStructure);

	NVIC_PriorityGroupConfig(SYSTEM_MVIC_GROUP_SET);
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = UART1_PREEMPTION_PRIORITY;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = UART1_SUB_PRIORITY;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	//中断配置..Only IDLE Interrupt..
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);

	/* Enable the USART1 */
	USART_Cmd(USART1pos, ENABLE);
}

#ifdef ZL_RP551_MAIN_E
void GPIOInit_SE2431L(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = SE2431L_CTX_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(SE2431L_CTX_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = SE2431L_CPS_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(SE2431L_CPS_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = SE2431L_CSD_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(SE2431L_CSD_PORT, &GPIO_InitStructure);

	SE2431L_Bypass();
}

void SE2431L_LNA(void)
{
	GPIO_SetBits(SE2431L_CPS_PORT, SE2431L_CPS_PIN);	//110 LNA. 111 TX. 010 bypass.
	GPIO_SetBits(SE2431L_CSD_PORT, SE2431L_CSD_PIN);
	GPIO_ResetBits(SE2431L_CTX_PORT, SE2431L_CTX_PIN);
}

void SE2431L_Bypass(void)
{
	GPIO_ResetBits(SE2431L_CPS_PORT, SE2431L_CPS_PIN);	//110 LNA. 111 TX. 010 bypass.
	GPIO_SetBits(SE2431L_CSD_PORT, SE2431L_CSD_PIN);
	GPIO_ResetBits(SE2431L_CTX_PORT, SE2431L_CTX_PIN);
}

void SE2431L_TX(void)
{
	GPIO_SetBits(SE2431L_CPS_PORT, SE2431L_CPS_PIN);	//110 LNA. 111 TX. 010 bypass.
	GPIO_SetBits(SE2431L_CSD_PORT, SE2431L_CSD_PIN);
	GPIO_SetBits(SE2431L_CTX_PORT, SE2431L_CTX_PIN);
}
#endif

/* Private functions ---------------------------------------------------------*/
static uint8_t hal_nrf_rw(SPI_TypeDef* SPIx, uint8_t value)
{
	while(SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_TXE) == RESET);
	SPI_I2S_SendData(SPIx, value);
	while(SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_RXNE) == RESET);
	return(SPI_I2S_ReceiveData(SPIx));
}

uint8_t bsp_rf_rx_data( SPI_TypeDef* SPIx, uint8_t *buf_len, uint8_t *rbuf )
{
	uint8_t data[BUFFER_SIZE_MAX];
	uint8_t i = 0;
	uint8_t nop = 0xFF;

	*buf_len = 0;
	memset(spi_cmd.data, 0xFF, BUFFER_SIZE_MAX);
	spi_cmd.spi_cmd = UESB_READ_RF_INT_STATUS;
	spi_cmd.data_len = 0x02;
	spi_cmd.data[0] = 0xFF;
	spi_cmd.data[1] = 0xFF;

  /* 开始SPI传输 */
	//printf("SPI_RX :");
	NRF1_CSN_LOW();
	memset(data, 0, BUFFER_SIZE_MAX);
	for(i=0; i<spi_cmd.data_len+3; i++)
	{
		data[i] = hal_nrf_rw( SPIx, nop );
		//printf(" %02x",data[i]);
		if( i == 3 && (data[2] & (1<<RX_DR)) && data[3] < BUFFER_SIZE_MAX )
	  {
			*buf_len = data[3];
			spi_cmd.data_len += *buf_len;
			spi_cmd.data[spi_cmd.data_len] = XOR_Cal((uint8_t *)&spi_cmd.data[3],
				spi_cmd.data_len - 3);
		}
	}
	/* 关闭SPI传输 */
	NRF1_CSN_HIGH();
	//printf("\r\n");

	memcpy(rbuf, &data[4],*buf_len);

	if(data[0] != 0 && data[0] != 0xFF)
		return 1;
	else
		return 0;
}

uint8_t bsp_rf_tx_data( uint8_t *tbuf, uint8_t len, uint8_t cnt, uint8_t us )
{
	uint8_t data[BUFFER_SIZE_MAX];
	uint16_t i = 0;
	uint8_t *pdata = NULL;

	spi_cmd.spi_cmd    = UESB_WRITE_TX_PAYLOAD;
	spi_cmd.data_len   = len+2;
	spi_cmd.count      = cnt;
	spi_cmd.delay100us = us;

	memcpy(spi_cmd.data, tbuf, len);
	spi_cmd.data[spi_cmd.data_len-2] = XOR_Cal((uint8_t *)&spi_cmd,
		spi_cmd.data_len+2);
	pdata = (uint8_t *)&spi_cmd;

//	{
//		uint8_t i;
//		uint8_t *mdata = tbuf;
//		printf("TX_DATA:");
//		for(i=0;i<len;i++)
//			printf(" %02x",*(mdata+i));
//		printf("\r\n");
//	}
	
  /* 开始SPI传输 */
	//printf("SPI_TX :");
	NRF2_CSN_LOW();
	memset(data, 0, BUFFER_SIZE_MAX);
	for(i=0; i<spi_cmd.data_len+3; i++)
	{
#ifdef ZL_RP551_MAIN_E
		data[i] = hal_nrf_rw(SPI1, *(pdata+i));
#endif

#ifdef ZL_RP551_MAIN_F
		data[i] = hal_nrf_rw(SPI2, *(pdata+i));
#endif

#ifdef ZL_RP551_MAIN_H
		data[i] = hal_nrf_rw(NRF_TX_SPI, *(pdata+i));
#endif
		//printf(" %02x",*(pdata+i));
	}
	/* 关闭SPI传输 */
	NRF2_CSN_HIGH();
	//printf("\r\n");

  /* 若接收到数据校验正确 */
	if( data[0] != 0 ) 									
		return 1;
	else
		return 0;
}

/*******************************************************************************
  * @brief  Get stm32 MCU.
  * @param  None
  * @retval None
*******************************************************************************/
void bsp_device_id_init(void)
{
	uint32_t MCUID[4];
	
	MCUID[0] = *(volatile uint32_t *)(0x1FFFF7E8);
	MCUID[1] = *(volatile uint32_t *)(0x1FFFF7EC);
	MCUID[2] = *(volatile uint32_t *)(0x1FFFF7F0);
	
	MCUID[3] = MCUID[0] + MCUID[1] + MCUID[2];
	
	jsq_uid[7]=(uint8_t)(MCUID[3]>>0)&0x0f;
	jsq_uid[6]=(uint8_t)(MCUID[3]>>4)&0x0f; 
	jsq_uid[5]=(uint8_t)(MCUID[3]>>8)&0x0f;
	jsq_uid[4]=(uint8_t)(MCUID[3]>>12)&0x0f;
	jsq_uid[3]=(uint8_t)(MCUID[3]>>16)&0x0f;
	jsq_uid[2]=(uint8_t)(MCUID[3]>>20)&0x0f;
	jsq_uid[1]=(uint8_t)(MCUID[3]>>24)&0x0f;
	jsq_uid[0]=(uint8_t)(MCUID[3]>>28)&0x0f;

	nrf_data.jsq_uid[0] = (jsq_uid[1]&0x0F)|((jsq_uid[0]<<4)&0xF0);
	nrf_data.jsq_uid[1] = (jsq_uid[3]&0x0F)|((jsq_uid[2]<<4)&0xF0);
	nrf_data.jsq_uid[2] = (jsq_uid[5]&0x0F)|((jsq_uid[4]<<4)&0xF0);
	nrf_data.jsq_uid[3] = (jsq_uid[7]&0x0F)|((jsq_uid[6]<<4)&0xF0);
	
	memcpy(revicer.uid,nrf_data.jsq_uid,4);
}

/*******************************************************************************
* Function Name   : void uart_send_char( uint8_t ch )
* Description     : Retargets the C library printf function
* Input           : None
* Output          : None
*******************************************************************************/
void uart_send_char( uint8_t ch )
{
	/* Write a character to the USART */
	USART_SendData(USART1pos, (u8) ch);

	/* Loop until the end of transmission */
	while(!(USART_GetFlagStatus(USART1pos, USART_FLAG_TXE) == SET))
	{
	}
}

/*******************************************************************************
* Function Name   : int fputc(int ch, FILE *f)
* Description     : Retargets the C library printf function to the printf
* Input           : None
* Output          : None
*******************************************************************************/
int fputc(int ch, FILE *f)
{
	/* Write a character to the USART */
	USART_SendData(USART1pos, (u8) ch);

	/* Loop until the end of transmission */
	while(!(USART_GetFlagStatus(USART1pos, USART_FLAG_TXE) == SET))
	{
	}

	return ch;
}

/*******************************************************************************
* Function Name   : int fgetc(FILE *f)
* Description     : Retargets the C library printf function to the fgetc
* Input           : None
* Output          : None
*******************************************************************************/
int fgetc(FILE *f)
{
	/* Loop until received a char */
	while(!(USART_GetFlagStatus(USART1pos, USART_FLAG_RXNE) == SET))
	{
	}

	/* Read a character from the USART and RETURN */
	return (USART_ReceiveData(USART1pos));
}

/*******************************************************************************
* Function Name   : uint8_t XOR_Cal(uint8_t *data, uint16_t length)
* Description     : Òì»ò¼ÆËãº¯Êý
* Input           : data	: Êý¾ÝÖ¸Õë
*        		    length	£ºÊý¾Ý³¤¶È
* Output          : None
*******************************************************************************/
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

/**************************************END OF FILE****************************/

