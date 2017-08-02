#include "main.h"
#include "app_spi_send_data_process.h"

static uint8_t  sbuf_s = 0;
static uint8_t  sbuf[SPI_SEND_DATA_BUFFER_COUNT_MAX][260];
static uint8_t  sbuf_len[4];
static uint16_t sbuf_cnt_w, sbuf_cnt_r, sbuf_cnt;
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
  Function:App_card_process
  Description:
  Input :
  Return:
  Others:None
******************************************************************************/
void App_spi_send_data_process(void)
{
	if( sbuf_s == 0 )
	{
		if( sbuf_cnt > 0 )
		{
			uint8_t delayms;
			spi_write_tx_payload( sbuf[sbuf_cnt_r], sbuf_len[sbuf_cnt_r],
				              sbuf[sbuf_cnt_r][sbuf_len[sbuf_cnt_r]] ,
				              sbuf[sbuf_cnt_r][sbuf_len[sbuf_cnt_r]+1]);

			delayms  = sbuf[sbuf_cnt_r][sbuf_len[sbuf_cnt_r]+2];
			DEBUG_SPI_LOG("[%3d] [%3d] delayms = %d\r\n", sbuf_cnt, sbuf_cnt_r, delayms );
			sbuf_cnt_r = (sbuf_cnt_r + 1) % SPI_SEND_DATA_BUFFER_COUNT_MAX;
			sbuf_cnt--;

			if( delayms > 0 )
			{
				sw_create_timer( &spi_send_data_timer , delayms, 1, 2,&(sbuf_s), NULL );
				set_spi_status(1);
			}
			else
				set_spi_status(0);
		}
	}
	
	if( sbuf_s == 2 )
		set_spi_status(0);
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
	memcpy( sbuf[sbuf_cnt_w], buf, len );
	sbuf_len[sbuf_cnt_w]    = len;
	sbuf[sbuf_cnt_w][len]   = cnt;
	sbuf[sbuf_cnt_w][len+1] = us;
	sbuf[sbuf_cnt_w][len+2] = ms;
	DEBUG_SPI_LOG("[%3d] spi_delayms = %d\r\n",sbuf_cnt_w,ms);
	sbuf_cnt_w = (sbuf_cnt_w + 1) % SPI_SEND_DATA_BUFFER_COUNT_MAX;
	sbuf_cnt++;
}
