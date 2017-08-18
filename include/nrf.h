#ifndef	__NRF_H_
#define	__NRF_H_

#include <stdint.h>
#include "stm32f10x_spi.h"
#include "board.h"

/* nrf configuration define---------------------------------------------------*/
#define NRF_MAX_NUMBER_OF_RETRANSMITS		(3)				//����ط�����
#define	NRF_RETRANSMIT_DELAY			      (280)				//�ط���ʱ ms
#define	NRF_ACK_PROTOCOL_LEN			      (10)				//�Զ�������ģ��ACKЭ�����ݳ���
#define	NRF_TOTAL_DATA_LEN				      (250)				//2.4G�����ܳ���
#define	NRF_USEFUL_DATA_LEN	            (NRF_TOTAL_DATA_LEN - NRF_ACK_PROTOCOL_LEN)		//��Ч���ݳ���
#define	NRF_DATA_IS_USEFUL				      (0)
#define NRF_DATA_IS_ACK				          (1)
#define NRF_DATA_IS_PRE				          (2)

#define BUFFER_SIZE_MAX							    (255)

/* STATUS register bit define-------------------------------------------------*/
#define RX_DR                           6     /**< STATUS register bit 6 */
#define TX_DS                           5     /**< STATUS register bit 5 */
#define MAX_RT                          4     /**< STATUS register bit 4 */
#define TX_FULL                         0     /**< STATUS register bit 0 */


typedef struct
{
	uint8_t	 						      dtq_uid[4];				//�˴�����UID
	uint8_t							      jsq_uid[4];				//��֮��ԵĽ�����UID
	uint8_t							      rlen;
	uint8_t							      tlen;
	uint8_t							      rbuf[NRF_TOTAL_DATA_LEN];		//�������ݷ�����
	uint8_t							      tbuf[NRF_TOTAL_DATA_LEN];		//�������ݷ�����
}nrf_communication_t;

typedef struct
{
	uint8_t 				spi_header;
	uint8_t         cmd;
	uint8_t         count;
	uint8_t         delay100us;
	uint8_t					data_len;
	uint8_t 				data[BUFFER_SIZE_MAX];
	uint8_t 				xor;														//Ϊ�������������
} spi_cmd_type_t;



typedef struct
{
	uint8_t dist[4];
	uint8_t transmit_count;
	uint8_t delay100us;
	uint8_t is_pac_add;
	uint8_t logic_pac_add;
	uint8_t package_type;
	uint8_t *data_buf;
	uint8_t data_len;
	uint8_t sel_table;
} nrf_transmit_parameter_t;

/* Private functions ---------------------------------------------------------*/
uint8_t spi_read_tx_payload(SPI_TypeDef* SPIx, uint8_t *rx_data_len, uint8_t *rx_data);
void    spi_write_tx_payload(const uint8_t *tx_pload, uint8_t length, uint8_t count, uint8_t delay100us);


void nrf_transmit_start( nrf_transmit_parameter_t *t_conf);
void nrf51822_spi_init(void);
void nrf51822_parameters_init(void);
void nrf1_rst_deinit(void);
void nrf2_rst_deinit(void);
void nrf1_rst_init(void);
void nrf2_rst_init(void);

#endif //__NRF_H_
