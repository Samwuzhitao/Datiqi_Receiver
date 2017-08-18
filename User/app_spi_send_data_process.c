#include "main.h"
#include "app_spi_send_data_process.h"
#include "app_send_data_process.h"

static uint8_t    sbuf_s = 0;
static spi_cmd_t  sbuf[SPI_SEND_DATA_BUFFER_COUNT_MAX];
static uint16_t   sbuf_cnt_w, sbuf_cnt_r, sbuf_cnt;
static uint8_t    retry_cnt = 0;
spi_cmd_t  irq_rbuf[SPI_SEND_DATA_BUFFER_COUNT_MAX];
uint16_t   irq_rbuf_cnt_w = 0, irq_rbuf_cnt_r = 0, irq_rbuf_cnt = 0;
/******************************************************************************
  Function:set_spi_status
  Description:
  Input :
  Output:
  Return:
  Others:None
******************************************************************************/
void set_spi_status( uint8_t new_status )
{
	sbuf_s = new_status;
	DEBUG_SPI_LOG("spi_status = %d\r\n",sbuf_s);
}

/******************************************************************************
  Function:get_spi_status
  Description:
  Input :
  Output:
  Return:
  Others:None
******************************************************************************/
uint8_t get_spi_status( void )
{
	return sbuf_s ;
}

/******************************************************************************
  Function:spi_rx_data_process
  Description:
		将零时缓存的数据存入到buffer中
  Input :
  Return:
  Others:None
******************************************************************************/
void spi_rx_data_process( void )
{
	if( irq_rbuf_cnt > 0 )
	{
		uint8_t i, rx_s = 0, rx_cnt = 0;
		printf("\t rx_buf: ");
		printf("%02x %02x %02x %02x",                                      \
			irq_rbuf[irq_rbuf_cnt_r].header, irq_rbuf[irq_rbuf_cnt_r].dev_t, \
			irq_rbuf[irq_rbuf_cnt_r].cmd, irq_rbuf[irq_rbuf_cnt_r].length);
		for( i = 0; i < irq_rbuf[irq_rbuf_cnt_r].length; i++ )
			printf(" %02x", irq_rbuf[irq_rbuf_cnt_r].data[i]);
		printf(" %02x %02x \r\n",
			irq_rbuf[irq_rbuf_cnt_r].xor, irq_rbuf[irq_rbuf_cnt_r].end);
		irq_rbuf_cnt--;
		irq_rbuf_cnt_r = (irq_rbuf_cnt_r + 1) % SPI_SEND_DATA_BUFFER_COUNT_MAX;
	}
}

void spi_tx_data_process(void)
{
	if( sbuf_s == 0 )
	{
		if( sbuf_cnt > 0 )
		{
			bsp_spi_tx_data( &sbuf[sbuf_cnt_r] );
			set_spi_status(1);
		}
	}

	if( sbuf_s == 2 )
	{
		uint8_t spi_ack_flag = 0;

		if( spi_ack_flag == 1 )
		{
			sbuf_cnt--;
			sbuf_cnt_r = (sbuf_cnt_r + 1) % SPI_SEND_DATA_BUFFER_COUNT_MAX;
			retry_cnt = 0;
			set_spi_status(0);
		}
		else
		{
			retry_cnt++;
			if(retry_cnt == 3)
			{
				sbuf_cnt--;
				sbuf_cnt_r = (sbuf_cnt_r + 1) % SPI_SEND_DATA_BUFFER_COUNT_MAX;
				retry_cnt = 0;
			}
			set_spi_status(0);
		}
	}
}

void App_spi_send_data_process( void )
{
	spi_tx_data_process();
	spi_rx_data_process();
}

/******************************************************************************
  Function:App_card_process
  Description:
  Input :
  Return:
  Others:None
******************************************************************************/
void sbuf_s_1_timer_init( void )
{
	DEBUG_SPI_LOG("spi_status = %d\r\n",sbuf_s);
}

void spi_timer_init( void )
{
    sw_create_timer( &spi_send_data1_timer , 3, 1, 2,&(sbuf_s), sbuf_s_1_timer_init );
//sw_create_timer( &spi_send_data2_timer , delay1_ms, 2, 3,&(sbuf_s), sbuf_s_1_timer_init );
}

/******************************************************************************
  Function: spi_write_data_to_buf
  Description: store data to spi send data buffer
  Input :
  Return:
  Others:None
******************************************************************************/
void spi_write_data_to_buf( u8 *buf, u8 len, u8 cnt, u8 us, u8 ms )
{
	spi_pro_init_pack( &sbuf[sbuf_cnt_w] );
	sbuf[sbuf_cnt_w].cmd   = 0x10;
	sbuf[sbuf_cnt_w].dev_t = DEVICE_TX;
	sbuf[sbuf_cnt_w].length = len;
	memcpy( sbuf[sbuf_cnt_w].data, buf, len);
	spi_pro_pack_update_crc( &sbuf[sbuf_cnt_w] );
	sbuf_cnt_w = (sbuf_cnt_w + 1) % SPI_SEND_DATA_BUFFER_COUNT_MAX;
	sbuf_cnt++;
}

int8_t spi_write_cmd_to_tx( u8 *buf, u8 len )
{
	spi_pro_init_pack( &sbuf[sbuf_cnt_w] );
	sbuf[sbuf_cnt_w].cmd   = 0x20;
	sbuf[sbuf_cnt_w].dev_t = DEVICE_TX;
	sbuf[sbuf_cnt_w].length = len;
	memcpy( sbuf[sbuf_cnt_w].data, buf, len);
	spi_pro_pack_update_crc( &sbuf[sbuf_cnt_w] );
	sbuf_cnt_w = (sbuf_cnt_w + 1) % SPI_SEND_DATA_BUFFER_COUNT_MAX;
	sbuf_cnt++;
	return 0;
}

int8_t spi_write_cmd_to_rx( u8 *buf, u8 len )
{
	spi_pro_init_pack( &sbuf[sbuf_cnt_w] );
	sbuf[sbuf_cnt_w].cmd   = 0x20;
	sbuf[sbuf_cnt_w].dev_t = DEVICE_RX;
	sbuf[sbuf_cnt_w].length = len;
	memcpy( sbuf[sbuf_cnt_w].data, buf, len);
	spi_pro_pack_update_crc( &sbuf[sbuf_cnt_w] );
	sbuf_cnt_w = (sbuf_cnt_w + 1) % SPI_SEND_DATA_BUFFER_COUNT_MAX;
	sbuf_cnt++;
	return 0;
}
