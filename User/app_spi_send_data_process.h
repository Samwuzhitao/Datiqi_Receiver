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

void change_spi_send_data_process_status( uint8_t new_status );
uint8_t get_spi_send_data_process_status( void );
void App_spi_send_data_process(void);
void spi_send_data_process_timer_init( void );
void spi_write_data_to_buf( u8 *buf, u8 len, u8 cnt, u8 us, u8 ms );
uint8_t get_spi_status( void );

int8_t spi_write_cmd_to_rx( u8 *buf, u8 len );
int8_t spi_write_cmd_to_tx( u8 *buf, u8 len );
void spi_timer_init( void );
spi_cmd_t *spi_malloc_buf( void );
#endif
