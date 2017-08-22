#ifndef	__NRF_H_
#define	__NRF_H_

#include <stdint.h>
#include "stm32f10x_spi.h"
#include "board.h"

/* nrf configuration define---------------------------------------------------*/
#define NRF_MAX_NUMBER_OF_RETRANSMITS		(3)				//最大重发次数
#define	NRF_RETRANSMIT_DELAY			      (280)				//重发延时 ms
#define	NRF_ACK_PROTOCOL_LEN			      (10)				//自定义的软件模拟ACK协议数据长度
#define	NRF_TOTAL_DATA_LEN				      (250)				//2.4G数据总长度
#define	NRF_USEFUL_DATA_LEN	            (NRF_TOTAL_DATA_LEN - NRF_ACK_PROTOCOL_LEN)		//有效数据长度
#define	NRF_DATA_IS_USEFUL				      (0)
#define NRF_DATA_IS_ACK				          (1)
#define NRF_DATA_IS_PRE				          (2)

#define BUFFER_SIZE_MAX							    (255)

/* STATUS register bit define-------------------------------------------------*/
#define RX_DR                           6     /**< STATUS register bit 6 */
#define TX_DS                           5     /**< STATUS register bit 5 */
#define MAX_RT                          4     /**< STATUS register bit 4 */
#define TX_FULL                         0     /**< STATUS register bit 0 */

/* Private functions ---------------------------------------------------------*/

void nrf51822_spi_init(void);
void nrf1_rst_deinit(void);
void nrf2_rst_deinit(void);
void nrf1_rst_init(void);
void nrf2_rst_init(void);

#endif //__NRF_H_
