#include "main.h"
#include "app_spi_send_data_process.h"
#include "answer_fun.h"
#include "ring_buf.h"

static uint8_t    sbuf_s = 0;
static spi_cmd_t  sbuf[SPI_SEND_DATA_BUFFER_COUNT_MAX];
static uint16_t   sbuf_cnt_w, sbuf_cnt_r, sbuf_cnt;
static uint8_t    retry_cnt = 0;
spi_cmd_t         irq_rbuf[SPI_SEND_DATA_BUFFER_COUNT_MAX];
uint16_t          irq_rbuf_cnt_w = 0, irq_rbuf_cnt_r = 0, irq_rbuf_cnt = 0;
spi_cmd_ctl_t     r_cmd_ctl = { 0, DEVICE_TX };
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
		if((irq_rbuf[irq_rbuf_cnt_r].cmd == 0x22) && 
		(irq_rbuf[irq_rbuf_cnt_r].dev_t == DEVICE_RX))
		{
			static uint8_t pack_num = 0,seq_num = 0;
			/* SPI 主动提交的数据 */
			/* 返回 SPI ACK */
			bsp_spi_rx_ack();
			{
				uint8_t i;
				rssi_rf_pack_t *rssi_pack = (rssi_rf_pack_t *)(irq_rbuf[irq_rbuf_cnt_r].data);
				SPI_RF_DEBUG("[RF]r :");
				for(i=0;i<irq_rbuf[irq_rbuf_cnt_r].length;i++)
				{
					SPI_RF_DEBUG(" %02x",irq_rbuf[irq_rbuf_cnt_r].data[i]);
				}
				SPI_RF_DEBUG("\r\n");
				SPI_RF_DEBUG("[RF] DATA: seq_num:%02x pac_num:%02x \r\n",rssi_pack->rf_pack.ctl.seq_num,
						rssi_pack->rf_pack.ctl.pac_num );
			}
			/* 将数据送入应用层处理 */
			{
				if( BUF_FULL != buffer_get_buffer_status(SPI_RBUF) )
				{ 
					/* 返回 RF ACK */
					rf_pack_t      rf_ack;
					rssi_rf_pack_t *rssi_pack = (rssi_rf_pack_t *)(irq_rbuf[irq_rbuf_cnt_r].data);
					spi_cmd_t      *rf_ack_data = spi_malloc_buf();
					answer_cmd_t   *ack_cmd = (answer_cmd_t *)rf_ack.data;
					spi_pro_init_pack_rf( rf_ack_data, RF_ACK, 0x04 );
					rf_pro_init_pack( &rf_ack );
					ack_cmd->cmd    = 0x52;
					ack_cmd->len    = 0x05;
					ack_cmd->buf[0] = 0x01;
					memcpy( ack_cmd->buf+1, rssi_pack->rf_pack.ctl.dst_uid, 4);
					rf_ack.pack_len = ack_cmd->len + 2;
					rf_data_to_spi_data( rf_ack_data, &rf_ack );
					if(( pack_num != rssi_pack->rf_pack.ctl.pac_num ) &&
						 ( seq_num  != rssi_pack->rf_pack.ctl.seq_num ))
					{
						pack_num = rssi_pack->rf_pack.ctl.pac_num;
						seq_num  = rssi_pack->rf_pack.ctl.seq_num;
						rf_write_data_to_buffer( SPI_RBUF, irq_rbuf[irq_rbuf_cnt_r].data,
							irq_rbuf[irq_rbuf_cnt_r].length );
					}
				
				}
			}
		}
		else
		{
			/* SPI 被动应答 */
			r_cmd_ctl.cmd = irq_rbuf[irq_rbuf_cnt_r].cmd;
			r_cmd_ctl.dev = irq_rbuf[irq_rbuf_cnt_r].dev_t;
		}
		irq_rbuf_cnt--;
		irq_rbuf_cnt_r = (irq_rbuf_cnt_r + 1) % SPI_SEND_DATA_BUFFER_COUNT_MAX;
	}
}

void spi_tx_data_process(void)
{
	static uint8_t ack_flg;

	if( sbuf_s == 0 )
	{
		if( sbuf_cnt > 0 )
		{
			bsp_spi_tx_data( &sbuf[sbuf_cnt_r] );
			set_spi_status(1);
		}
		return;
	}

	if( sbuf_s == 1 )
	{
		ack_flg = spi_cmd_ack_check( &r_cmd_ctl );
		if( ack_flg == 1 )
		{
			sw_clear_timer( &spi_send_data1_timer );
			set_spi_status( 2 );
		}
	}

	if( sbuf_s == 2 )
	{
		if( ack_flg == 1 )
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
		return;
	}
}

void spi_s_cmd_process( void )
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
    sw_create_timer( &spi_send_data1_timer , 5, 1, 2,&(sbuf_s), sbuf_s_1_timer_init );
}

/******************************************************************************
  Function: spi_malloc_buf
  Description: store data to spi send data buffer
  Input :
  Return:
  Others:None
******************************************************************************/
spi_cmd_t *spi_malloc_buf( void )
{
	spi_cmd_t *pdata;
	if( sbuf_cnt < SPI_SEND_DATA_BUFFER_COUNT_MAX )
	{
		pdata = &sbuf[sbuf_cnt_w];
		sbuf_cnt_w = (sbuf_cnt_w + 1) % SPI_SEND_DATA_BUFFER_COUNT_MAX;
		sbuf_cnt++;
	}
	else
		pdata = NULL;
	return pdata;
}

int8_t spi_write_cmd_to_tx( u8 *buf, u8 len )
{
	spi_pro_init_pack_set( &sbuf[sbuf_cnt_w], DEVICE_TX );
	sbuf[sbuf_cnt_w].length = len;
	memcpy( sbuf[sbuf_cnt_w].data, buf, len);
	spi_pro_pack_update_crc( &sbuf[sbuf_cnt_w] );
	sbuf_cnt_w = (sbuf_cnt_w + 1) % SPI_SEND_DATA_BUFFER_COUNT_MAX;
	sbuf_cnt++;
	return 0;
}

int8_t spi_write_cmd_to_rx( u8 *buf, u8 len )
{
	spi_pro_init_pack_set( &sbuf[sbuf_cnt_w], DEVICE_RX);
	sbuf[sbuf_cnt_w].length = len;
	memcpy( sbuf[sbuf_cnt_w].data, buf, len);
	spi_pro_pack_update_crc( &sbuf[sbuf_cnt_w] );
	sbuf_cnt_w = (sbuf_cnt_w + 1) % SPI_SEND_DATA_BUFFER_COUNT_MAX;
	sbuf_cnt++;
	return 0;
}
