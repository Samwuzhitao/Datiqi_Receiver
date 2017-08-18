/**
  ******************************************************************************
  * @file   	spi_link_protocol.h
  * @author  	Sam.wu
  * @version 	V1.0.0.0
  * @date   	2017.08.14
  * @brief   	platform init functions
  ******************************************************************************
  */
#include "stm32f10x.h"

#define ENABLE_SPI_DATA_DEBUG
//#define ENABLE_SPI_STATUS_DEBUG

#ifdef ENABLE_SPI_DATA_DEBUG
#define SPI_DATA_DEBUG  printf
#else
#define SPI_DATA_DEBUG(...)
#endif

#ifdef ENABLE_SPI_STATUS_DEBUG
#define SPI_STATUS_DEBUG  printf
#else
#define SPI_STATUS_DEBUG(...)
#endif

#define BUFFER_SIZE_MAX							    (255)
#define SPI_PACK_SOF                    (0x86)
#define SPI_PACK_EOF                    (0x76)

#define RF_TX_CH_DEFAULT_CONF           {.len       = 2,               \
                                         .t_buf     = { 0x02, 0x05 }   }
#define RF_RX_CH_DEFAULT_CONF           {.len       = 1,               \
                                         .t_buf     = { 0x04 }   }
#define RF_BIND_RX_CH_CONF              {.len       = 1,               \
                                         .t_buf     = { 0x0A }         }

#define RF_ADDR_CH                      (0)
#define RF_ADDR_TX_POWER                (1)		
																				 
typedef enum {
	DEVICE_RX = 0x01, 
	DEVICE_TX = 0x02
}spi_dev_t;

typedef struct
{
	uint8_t 				header;
	spi_dev_t       dev_t;
	uint8_t         cmd;
	uint8_t					length;
	uint8_t 				data[BUFFER_SIZE_MAX];
	uint8_t 				xor;
	uint8_t 				end;
} spi_cmd_t;
																	
typedef enum {
	RF_TX_CH1 = 0x02, 
	RF_TX_CH2 = 0x01,
	RF_TX_CH3 = 0x05,
	RF_TX_CH4 = 0x06,
	RF_TX_CH5 = 0x09
}rf_tx_ch_t;

typedef enum {
	RF_RX_CH1 = 0x04, 
	RF_RX_CH2 = 0x03,
	RF_RX_CH3 = 0x07,
	RF_RX_CH4 = 0x08,
	RF_RX_CH5 = 0x0B
}rf_rx_ch_t;

typedef enum {
	RF_TX_POWER_CH1 = 0x01, 
	RF_TX_POWER_CH2 = 0x02,
	RF_TX_POWER_CH3 = 0x03,
	RF_TX_POWER_CH4 = 0x04,
	RF_TX_POWER_CH5 = 0x05
}rf_tx_power_t;


/* Private functions ---------------------------------------------------------*/
void spi_pro_init_pack( spi_cmd_t *spi_scmd );
void spi_pro_pack_update_crc( spi_cmd_t *spi_scmd );

uint8_t bsp_spi_rx_data( uint32_t irq, spi_cmd_t *spi_rcmd );
uint8_t bsp_spi_tx_data( spi_cmd_t *spi_scmd );



