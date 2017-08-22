#ifndef __APP_SPI_SEND_DATA_PROCESS_H_
#define __APP_SPI_SEND_DATA_PROCESS_H_

#include "main.h"

//#define ENABLE_SPI_DEBUG_LOG

#ifdef 	ENABLE_SPI_DEBUG_LOG
#define DEBUG_SPI_LOG							     DEBUG_LOG
#else
#define DEBUG_SPI_LOG(...)
#endif

#define SPI_SEND_DATA_PRE_DELAY        10
#define SPI_SEND_DATA_BUFFER_COUNT_MAX 4

extern spi_cmd_t  irq_rbuf[SPI_SEND_DATA_BUFFER_COUNT_MAX];
extern uint16_t   irq_rbuf_cnt_w, irq_rbuf_cnt_r, irq_rbuf_cnt;

int8_t spi_write_cmd_to_rx( u8 *buf, u8 len );
int8_t spi_write_cmd_to_tx( u8 *buf, u8 len );

spi_cmd_t *spi_malloc_buf( void );

void spi_timer_init( void );
void spi_s_cmd_process( void );

#endif
