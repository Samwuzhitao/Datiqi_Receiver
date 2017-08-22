/**
  ******************************************************************************
  * @file   	board.c
  * @author  	Samwu
  * @version 	V1.0.0.0
  * @date   	2017.01.12
  * @brief   	board init
  ******************************************************************************
  */
#include "main.h"
#include "stm32f10x.h"
#include "board.h"
#include "nrf.h"
#include "sw_timer.h"
#include "app_spi_send_data_process.h"
#include "app_card_process.h"

/* Private variables ---------------------------------------------------------*/
bool 						        gbf_hse_setup_fail = FALSE;		// ????????
RF_TypeDef 				      rf_var;
extern wl_typedef       wl;
extern uint8_t dtq_self_inspection_flg;

void systick_timer_init( void );
/*******************************************************************************
  * @brief  ???????
  * @param  None
  * @retval None
  * @note 	None
*******************************************************************************/
void board_init(void)
{
	uint8_t status = 0;

	/* disable all IRQ */
	DISABLE_ALL_IRQ();

	/* initialize system clock */
	SysClockInit();
	/* get mcu uuid */
	get_mcu_uid();

	/* initialize gpio port */
	bsp_board_init();
	bsp_uart_init();

	/* initialize the spi interface with nrf51822 */
	nrf51822_spi_init();

	/* eeprom init and white_list init*/
	Fee_Init(FEE_INIT_POWERUP);
	get_white_list_from_flash();

	/* init software timer */
	sw_timer_init();
	system_timer_init();
	rf_send_data_process_timer_init();
	spi_timer_init();
	card_timer_init();

	status = mfrc500_init();
	printf("[ INIT ] MF1702 INIT: %s !\r\n", (status == 0) ? "OK" : "FAIL");

	/* enable all IRQ */
	ENABLE_ALL_IRQ();

#ifndef OPEN_SILENT_MODE
	BEEP_EN();
#endif
	ledOn(LRED);
	ledOn(LBLUE);
	DelayMs(200);
	BEEP_DISEN();
	ledOff(LBLUE);

	status = clicker_config_default_set();
	printf("[ INIT ] RF INIT: %s !\r\n", (status == 0) ? "OK" : "FAIL");;
	IWDG_Configuration();
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



/****************************************************************************
* ?    ?:void bsp_uart_init(void)
* ?    ?:??1?????
* ????:?
* ????:?
* ?    ?:
* ????:?
****************************************************************************/
void bsp_uart_init(void)
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

	//????..Only IDLE Interrupt..
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);

	/* Enable the USART1 */
	USART_Cmd(USART1pos, ENABLE);
}

/**************************************END OF FILE****************************/
