/**
  ******************************************************************************
  * @file   	board.h
  * @author  	Samwu
  * @version 	V1.0.0.0
  * @date   	2017.01.12
  * @brief   	board infomation
  ******************************************************************************
  */

#ifndef _BOARD_H_
#define _BOARD_H_
#include "stm32f10x.h"
/* Defines ------------------------------------------------------------------*/
//#define OPEN_SILENT_MODE
#define ENABLE_WATCHDOG					(1)
//#define ENABLE_OUTPUT_MODE_NORMOL		
//#define ENABLE_DEBUG_LOG			
//#define ENABLE_RF_DATA_SHOW   


#define DISABLE_ALL_IRQ()  			__set_PRIMASK(1);
#define ENABLE_ALL_IRQ()   			__set_PRIMASK(0);

#define UART_BAUD								1152000

/* Uart Message configuration */
#define UART_NBUF								(240)
#define RF_NBUF									(UART_NBUF)

//#define BRANCH_WHTY
//#define BRANCH_WHNH

/* board name defines --------------------------------------------------------*/
/* NVIC defines ---------------------------------------------------------------*/
/* set system nvic group */
#define SYSTEM_MVIC_GROUP_SET         NVIC_PriorityGroup_2

/* Peripheral interrupt preemption set */
#define UART1_PREEMPTION_PRIORITY     1
#define UART2_PREEMPTION_PRIORITY     1
#define NRF1_PREEMPTION_PRIORITY      0
#define NRF2_PREEMPTION_PRIORITY      1
#define TIM3_PREEMPTION_PRIORITY      0

/* Peripheral interrupt response set */
#define UART1_SUB_PRIORITY            1
#define UART2_SUB_PRIORITY            1
#define NRF_SUB_PRIORITY              0 
#define TIM3_SUB_PRIORITY             1

/* USART defines -------------------------------------------------------------*/
#define USART1pos                     USART1
#define USART1pos_GPIO                GPIOA
#define USART1pos_CLK                 RCC_APB2Periph_USART1
#define USART1pos_GPIO_CLK            RCC_APB2Periph_GPIOA
#define USART1pos_RxPin               GPIO_Pin_10
#define USART1pos_TxPin               GPIO_Pin_9
#define USART1pos_IRQn                USART1_IRQn
#define USART1pos_IRQHandler          USART1_IRQHandler

#define USART2pos                     USART2
#define USART2pos_GPIO                GPIOA
#define USART2pos_CLK                 RCC_APB1Periph_USART2
#define USART2pos_GPIO_CLK            RCC_APB2Periph_GPIOA
#define USART2pos_RxPin               GPIO_Pin_3
#define USART2pos_TxPin               GPIO_Pin_2
#define USART2pos_IRQn                USART2_IRQn
#define USART2pos_IRQHandler          USART2_IRQHandler

/* nrf configuration define---------------------------------------------------*/
#define	NRF_TOTAL_DATA_LEN				      (250)				//2.4G数据总长度
#define	NRF_DATA_IS_USEFUL				      (0)
#define NRF_DATA_IS_ACK				          (1)
#define NRF_DATA_IS_PRE				          (2)
#define BUFFER_SIZE_MAX							    (255)

/* STATUS register bit define-------------------------------------------------*/
#define RX_DR                           6     /**< STATUS register bit 6 */
#define TX_DS                           5     /**< STATUS register bit 5 */
#define MAX_RT                          4     /**< STATUS register bit 4 */
#define TX_FULL                         0     /**< STATUS register bit 0 */

typedef enum {
  UESB_WRITE_PARAM 				    = 0x80,
  UESB_FLUSH_TX 					    = 0x81,
  UESB_FLUSH_RX 					    = 0x82,
	UESB_WRITE_TX_PAYLOAD 			= 0x83,
	UESB_WRITE_TX_PAYLOAD_NOACK = 0x84,
	UESB_READ_RX_PAYLOAD 			  = 0x85,
	UESB_READ_RF_INT_STATUS 		= 0x86,
	UESB_CLEAR_RF_INT_STATUS 		= 0x87,
	UESB_SWITCH_TX_RX	 			    = 0x88,
} spi_cmd_t;

typedef struct
{
	uint8_t	 				dtq_uid[4];				//此答题器UID
	uint8_t					jsq_uid[4];				//与之配对的接收器UID
	uint8_t					rlen;
	uint8_t					tlen;
	uint8_t					rbuf[NRF_TOTAL_DATA_LEN];		//接收数据反冲区
	uint8_t					tbuf[NRF_TOTAL_DATA_LEN];		//发送数据反冲区
}nrf_communication_t;

typedef struct
{
	uint8_t 				spi_cmd;
	uint8_t					data_len;
	uint8_t         count;
	uint8_t         delay100us;
	uint8_t 				data[BUFFER_SIZE_MAX];
	uint8_t 				xor;														//为所有数据异或结果
} spi_cmd_type_t;

//#define ZL_RP551_MAIN_E
//#define ZL_RP551_MAIN_F

#if !defined (ZL_RP551_MAIN_E) && !defined (ZL_RP551_MAIN_F) && !defined (ZL_RP551_MAIN_H)
 #error "Please select board used in your application (in board.h file)"
#endif

#if defined (ZL_RP551_MAIN_E) && defined (ZL_RP551_MAIN_F) && defined (ZL_RP551_MAIN_H)
 #error "Please select only one board used in your application (in board.h file)"
#endif

#ifdef ZL_RP551_MAIN_E
#include "zl_rp551_main_e.h"
#endif

#ifdef ZL_RP551_MAIN_F
#include "zl_rp551_main_f.h"
#endif

#ifdef ZL_RP551_MAIN_H
#include "zl_rp551_main_h.h"
#endif

/* Private define ------------------------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
void bsp_device_id_init(void);



void bsp_uart1_init(void);
void NVIC_Configuration_USART1(void);
void Usart2_Init(void);
void NVIC_Configuration_USART2(void);

#ifdef ZL_RP551_MAIN_E
void GPIOInit_SE2431L(void);
void SE2431L_LNA(void);
void SE2431L_Bypass(void);
void SE2431L_TX(void);
#endif

void GPIOInit_MFRC500(void);
//void uart_send_char( uint8_t ch );
void bsp_board_init(void);
/* platform misc functions's declaritions */
uint8_t XOR_Cal(uint8_t *data, uint16_t length);

void bsp_device_id_init( void );
uint8_t bsp_rf_rx_data ( SPI_TypeDef* SPIx, uint8_t *buf_len, uint8_t *rbuf );
uint8_t bsp_rf_tx_data ( uint8_t *tbuf, uint8_t len, uint8_t cnt, uint8_t us );
void nrf_transmit_start( uint8_t *tbuf, uint8_t buf_len, uint8_t buf_t,
												 uint8_t cnt, uint8_t us, uint8_t sel_table, uint8_t pack_t);
#endif //_BOARD_H_

/**************************************END OF FILE****************************/
