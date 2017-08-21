/**
  ******************************************************************************
  * @file   	spi_link_protocol.h
  * @author  	Sam.wu
  * @version 	V1.0.0.0
  * @date   	2017.08.14
  * @brief   	platform init functions
  ******************************************************************************
  */
#ifndef _SPI_PROTOCOL_V0200_H_
#define _SPI_PROTOCOL_V0200_H_
#include "stm32f10x.h"

#define ENABLE_SPI_DATA_DEBUG
//#define ENABLE_SPI_STATUS_DEBUG
#define ENABLE_SPI_RF_DEBUG


#ifdef ENABLE_SPI_DATA_DEBUG
#define SPI_DATA_DEBUG  b_print
#else
#define SPI_DATA_DEBUG(...)
#endif

#ifdef ENABLE_SPI_RF_DEBUG
#define SPI_RF_DEBUG  b_print
#else
#define SPI_RF_DEBUG(...)
#endif

#ifdef ENABLE_SPI_STATUS_DEBUG
#define SPI_STATUS_DEBUG  b_print
#else
#define SPI_STATUS_DEBUG(...)
#endif



#define BUFFER_SIZE_MAX							    (255)
#define SPI_PACK_SOF                    (0x86)
#define SPI_PACK_EOF                    (0x76)
#define RF_ADDR_CH                      (0)
#define RF_ADDR_TX_POWER                (1)	

#define RF_DATA_WITH_PRE                (1)
#define RF_DATA_NO_PRE                  (2)
#define RF_ACK                          (3)

/* 发送端默认配置 ************************************************/
#define RF_TX_CH_DEFAULT_CONF           {.len    = 2,             \
                                         .t_buf  = { 0x02, 0x05 } }
/* 接收端默认配置 ************************************************/
#define RF_RX_CH_DEFAULT_CONF           {.len    = 1,             \
                                         .t_buf  = { 0x04 }       }
/* 绑定信道默认配置 **********************************************/
#define RF_BIND_RX_CH_CONF              {.len    = 1,             \
                                         .t_buf  = { 0x0A }       }
/* SPI发送端默认数据包 *******************************************/
#define SPI_DATA_DEFAULT                {.header = 86,            \
                                         .dev_t  = DEVICE_TX,     \
	                                       .cmd    = 0x10,          \
	                                       .length = 0,             \
		                                     .data   = {0},           \
		                                     .xor    = 0,             \
		                                     .end    = 76             }
/* SPI 设备类型 **************************************************/
typedef enum {
	DEVICE_RX = 0x01, 
	DEVICE_TX = 0x02
}spi_dev_t;

/* SPI 信道 发送功率 枚举定义 ************************************/
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

/* SPI 数据包结构体 **********************************************/
typedef struct
{
	uint8_t   header;
	spi_dev_t dev_t;
	uint8_t   cmd;
	uint8_t		length;
	uint8_t 	data[BUFFER_SIZE_MAX];
	uint8_t 	xor;
	uint8_t 	end;
} spi_cmd_t;

typedef struct
{
	uint8_t   cmd;
	spi_dev_t dev;
}spi_cmd_ctl_t;

/* Private functions ---------------------------------------------------------*/
void spi_pro_init_pack_set( spi_cmd_t *spi_scmd, spi_dev_t dev_t );
void spi_pro_init_pack_rf( spi_cmd_t *spi_scmd, uint8_t data_t, uint8_t tx_ch );
void    spi_pro_pack_update_crc( spi_cmd_t *spi_scmd );
void    rf_data_to_spi_data    ( spi_cmd_t *spi_data, rf_pack_t *rf_data );
uint8_t bsp_spi_tx_data        ( spi_cmd_t *spi_scmd );
uint8_t bsp_spi_rx_data        ( uint32_t irq, spi_cmd_t *spi_rcmd );
uint8_t spi_cmd_ack_check( spi_cmd_ctl_t *r_cmd_ctl );
#endif
