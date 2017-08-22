/**
  ******************************************************************************
  * @file   	ringbuffer.c
  * @author  	sam.wu
  * @version 	V1.0.0.0
  * @date   	2016.8.26
  * @brief   	ringbuffer
  ******************************************************************************
  */
#include "ring_buf.h"
#include "string.h"

/* Private variables ---------------------------------------------------------*/
static uint8_t  spi_rbuf[SPI_RBUF_SIZE];
static uint8_t  ptint_buf[PRINT_RBUF_SIZE];
static uint8_t  uart_data_buf[SPI_RBUF_SIZE];
const uint32_t  buf_size[BUF_NUM]        = { PRINT_RBUF_SIZE, SPI_RBUF_SIZE, SPI_RBUF_SIZE };
static uint8_t  *pbuf[BUF_NUM]           = { ptint_buf, spi_rbuf, uart_data_buf };
static volatile uint16_t top[BUF_NUM]    = { 0, 0, 0 };
static volatile uint16_t bottom[BUF_NUM] = { 0, 0, 0 };
static volatile int32_t  Size[BUF_NUM]   = { 0, 0, 0 };
static volatile uint8_t  status[BUF_NUM] = { BUF_EMPTY, BUF_EMPTY, BUF_EMPTY };

/* Private functions ---------------------------------------------------------*/
static void    update_read_status( uint8_t sel) ;
static void    update_write_status( uint8_t sel) ;
static void    update_top( uint8_t sel, uint16_t Len );
static void    update_bottom( uint8_t sel, uint16_t Len );
static uint8_t get(uint8_t sel, uint16_t index);
static void    set(uint8_t sel, uint16_t index,uint8_t data);

/******************************************************************************
  Function:buffer_get_buffer_status
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
uint8_t buffer_get_buffer_status( uint8_t sel )
{
	return status[sel];
}

/******************************************************************************
  Function:get
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
uint8_t get( uint8_t sel, uint16_t index )
{
	uint8_t data = pbuf[sel][index % buf_size[sel]];
	pbuf[sel][index % buf_size[sel]] = 0;
	return  data;
}

/******************************************************************************
  Function:set
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
void set( uint8_t sel, uint16_t index, uint8_t data)
{
	pbuf[sel][index % buf_size[sel]] = data;
}

/******************************************************************************
  Function:update_read_status
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
static void update_read_status( uint8_t sel)
{
	uint8_t bufferstatus = buffer_get_buffer_status(sel);

	switch(bufferstatus)
	{
		case BUF_EMPTY:
			break;
		case BUF_USEING:
			{
				if(Size[sel] > 0)
					status[sel] = BUF_USEING;
				else if(Size[sel] == 0)
					status[sel] = BUF_EMPTY;
				else
				{
					status[sel] = BUF_EMPTY;
					top[sel]    = 0;
					bottom[sel] = 0;
				}
			}
			break;
		case BUF_FULL:
			status[sel] = BUF_USEING;
			break;

		default:
			break;
	}
}

/******************************************************************************
  Function:update_write_status
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
static void update_write_status( uint8_t sel)
{
	uint8_t bufferstatus = buffer_get_buffer_status(sel);

	switch(bufferstatus)
	{
		case BUF_EMPTY:
			status[sel] = BUF_USEING;
			break;

		case BUF_USEING:
			{
				if( Size[sel] > buf_size[sel]-PACKETSIZE )
					status[sel] = BUF_FULL;
				else
					status[sel] = BUF_USEING;
			}
			break;

		default:
			break;
	}
}
/******************************************************************************
  Function:update_top
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
static void update_top( uint8_t sel, uint16_t Len )
{
	CLOSEIRQ();
	Size[sel] += Len;
	top[sel] = (top[sel] + Len) % buf_size[sel];
	OPENIRQ();
}

/******************************************************************************
  Function:update_bottom
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
static void update_bottom( uint8_t sel, uint16_t Len )
{
	CLOSEIRQ();
	Size[sel] -= Len;
	bottom[sel] = (bottom[sel] + Len) % buf_size[sel];
	OPENIRQ();
}

/******************************************************************************
  Function:App_rf_check_process
  Description:
		spi RF 消息缓存处理函数
  Input :
  Return:
  Others:None
******************************************************************************/
void rf_write_data_to_buffer( uint8_t sel, uint8_t *buf, uint8_t len )
{
	uint8_t i;
	for(i=0;i<len;i++)
		set(sel,top[sel]+i,*(buf+i));
	update_top( sel, len);
	update_write_status(sel);
}

/******************************************************************************
  Function:App_rf_check_process
  Description:
		spi RF 消息读取处理函数
  Input :
  Return:
  Others:None
******************************************************************************/
int8_t rf_read_data_from_buffer( uint8_t sel, rssi_rf_pack_t *pack )
{
	uint16_t r_index = 0;
	uint8_t  r_buf_s = 0,*pdata;
	int8_t err = -1,r_cnt = 0, i = 0;
	do
	{
		if( err == 0)
			break;
		
		switch( r_buf_s )
		{
			case 0: 
			{
				pack->rssi = get(sel,bottom[sel]+r_index);
				r_index++; 
				r_buf_s = 1;
			}	
			break;
			case 1: 
			{
				pack->rf_pack.head = get(sel,bottom[sel]+r_index);
				r_index++;
				if( pack->rf_pack.head != 0x61 )
					r_buf_s = 0;
				else
				{
					i = 0;
					r_buf_s = 2;
					r_cnt   = sizeof(rf_pack_ctl_t);
					pdata   = (uint8_t *)&(pack->rf_pack.ctl);
				}
			}
			break;
			case 2:
			{
				*(pdata+i) = get(sel,bottom[sel]+r_index);
				r_index++;
				i++;
				if(i == r_cnt)
					r_buf_s = 3;
			}
			break;
			
			case 3:
			{
				pack->rf_pack.rev_len = get(sel,bottom[sel]+r_index);
				r_index++;
				if( pack->rf_pack.rev_len != 0)
				{
					i = 0;
					r_cnt   = pack->rf_pack.rev_len;
					pdata   = pack->rf_pack.rev_data;
					r_buf_s = 4;
				}
				else
					r_buf_s = 5;
			}
			break;
			case 4:
			{
				*(pdata+i) = get(sel,bottom[sel]+r_index);
				r_index++;
				i++;
				if(i == r_cnt)
					r_buf_s = 5;
			}
			break;
			
			case 5:
			{
				pack->rf_pack.pack_len = get(sel,bottom[sel]+r_index);
				r_index++;
				if( pack->rf_pack.pack_len != 0)
				{
					i = 0;
					r_cnt   = pack->rf_pack.pack_len;
					pdata   = pack->rf_pack.data;
					r_buf_s = 6;
				}
				else
					r_buf_s = 7;
			}
			break;
			
			case 6:
			{
				*(pdata+i) = get(sel,bottom[sel]+r_index);
				r_index++;
				i++;
				if(i == r_cnt)
					r_buf_s = 7;
			}
			break;
			
			case 7:
			{
				uint8_t crc_tmp = get(sel,bottom[sel]+r_index);
				r_index++;
				rf_pro_pack_update_crc( &(pack->rf_pack) );
				if( crc_tmp == pack->rf_pack.crc_xor )
					r_buf_s = 8;
				else
					err = -1;
			}
			break;
			
			case 8:
			{
				pack->rf_pack.end = get(sel,bottom[sel]+r_index);
				r_index++;
				err = 0;
			}
			break;
			
			default: 
				break;
		}

	}while(r_index<Size[sel]);

	update_bottom(sel, r_index);
	update_read_status(sel);

	return err;
}
/******************************************************************************
  Function:ringbuffer_get_usage_rate
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
uint8_t ringbuffer_get_usage_rate(uint8_t sel)
{
	return (Size[sel]*100/buf_size[sel]);
}

/******************************************************************************
  Function:print_write_data_to_buffer
  Description:
		存储打印数据到缓存
  Input :
  Return:
  Others:None
******************************************************************************/
void print_write_data_to_buffer( char *str, uint8_t len )
{
	char *pdata;
  uint8_t	i;

	pdata = str;

	for(i=0;i<len;i++)
	{
		set(PRINT_BUF,top[PRINT_BUF]+i,*pdata);
		pdata++;
	}

	update_top( PRINT_BUF, len);
	update_write_status(PRINT_BUF);
}

/******************************************************************************
  Function:print_write_data_to_buffer
  Description:
		从缓存中读取打印数据
  Input :
  Return:
  Others:None
******************************************************************************/
uint8_t print_read_data_to_buffer( uint8_t *str ,uint8_t size)
{
	uint8_t Len, *pdata, i;

	Len = Size[PRINT_BUF] > size ? size : Size[PRINT_BUF];
	pdata = str;

	for(i=0;i<Len;i++)
	{
		*pdata = get(PRINT_BUF,bottom[PRINT_BUF]+i);
		pdata++;
	}

	update_bottom(PRINT_BUF, Len);
	update_read_status(PRINT_BUF);
	return Len;
}

/**************************************END OF FILE****************************/
