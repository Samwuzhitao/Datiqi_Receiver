/**
  ******************************************************************************
  * @file   	spi_link_protocol.c
  * @author  	Sam.wu
  * @version 	V1.0.0.0
  * @date   	2017.08.14
  * @brief   	platform init functions
  ******************************************************************************
  */
#include "main.h"
/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static uint8_t hal_nrf_rw(SPI_TypeDef* SPIx, uint8_t value)
{
	while(SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_TXE) == RESET);
	SPI_I2S_SendData(SPIx, value);
	while(SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_RXNE) == RESET);
	return(SPI_I2S_ReceiveData(SPIx));
}

static void spi_tx_put_char( uint8_t s_char )
{
#ifdef ZL_RP551_MAIN_F
		hal_nrf_rw( SPI2, s_char );
#endif
#ifdef ZL_RP551_MAIN_H
		hal_nrf_rw( NRF_TX_SPI, s_char );
#endif	
	SPI_DATA_DEBUG(" %02x",s_char);
}

static void spi_rx_put_char( uint8_t s_char )
{
	hal_nrf_rw( SPI1, s_char );
	SPI_DATA_DEBUG(" %02x",s_char);
}

static uint8_t spi_tx_get_char( void )
{
#ifdef ZL_RP551_MAIN_F
	uint8_t r_char = hal_nrf_rw( SPI2, 0xFF );
#endif
#ifdef ZL_RP551_MAIN_H
	uint8_t r_char = hal_nrf_rw( NRF_TX_SPI, 0xFF );
#endif	
	SPI_DATA_DEBUG(" %02x",r_char);
	return r_char;
}

static uint8_t spi_rx_get_char( void )
{
	uint8_t r_char = hal_nrf_rw( SPI1, 0xFF );
	SPI_DATA_DEBUG(" %02x",r_char);
	return r_char;
}

void spi_pro_init_pack( spi_cmd_t *spi_scmd, spi_dev_t dev_t, uint8_t cmd )
{
	spi_scmd->header = SPI_PACK_SOF;
	spi_scmd->end    = SPI_PACK_EOF;
	spi_scmd->length = 0;
	spi_scmd->dev_t  = dev_t;
	spi_scmd->cmd    = cmd;
}

void spi_pro_pack_update_crc( spi_cmd_t *spi_scmd )
{
	spi_scmd->xor = XOR_Cal( (uint8_t *)&(spi_scmd->dev_t),
		spi_scmd->length + 3 );
}

void rf_data_to_spi_data( spi_cmd_t *spi_data, rf_pack_t *rf_pack )
{
	uint8_t i;
	uint8_t *pdata = spi_data->data;
	rf_pro_pack_update_crc( rf_pack );

	SPI_RF_DEBUG("\r\n[RF]s :");
	SPI_RF_DEBUG(" %14x", rf_pack->head);
	*(pdata++) = rf_pack->head;
	SPI_DECODE_DEBUG("\r\nSEC_UID:\t");
	SPI_RF_DEBUG(" %02x %02x %02x %02x", rf_pack->ctl.src_uid[0],
					rf_pack->ctl.src_uid[1], rf_pack->ctl.src_uid[2],
					rf_pack->ctl.src_uid[3]);
	memcpy(pdata,rf_pack->ctl.src_uid,4);
	pdata = pdata + 4;
	SPI_DECODE_DEBUG("\r\nDST_UID:\t");
	SPI_RF_DEBUG(" %02x %02x %02x %02x", rf_pack->ctl.dst_uid[0],
					rf_pack->ctl.dst_uid[1], rf_pack->ctl.dst_uid[2],
					rf_pack->ctl.dst_uid[3]);
	memcpy(pdata,rf_pack->ctl.dst_uid,4);
	pdata = pdata + 4;
	SPI_DECODE_DEBUG("\r\nDEV_ID:\t");
	SPI_RF_DEBUG(" %02x", rf_pack->ctl.dev_id);
	*(pdata++) = rf_pack->ctl.dev_id;
	SPI_DECODE_DEBUG("\r\nVER_NUM:\t");
	SPI_RF_DEBUG(" %02x", rf_pack->ctl.ver_num.byte);
	*(pdata++) = rf_pack->ctl.ver_num.byte;
	SPI_DECODE_DEBUG("\r\nSEQ_NUM:\t");
	SPI_RF_DEBUG(" %02x", rf_pack->ctl.seq_num);
	*(pdata++) = rf_pack->ctl.seq_num;
	SPI_DECODE_DEBUG("\r\nPAC_NUM:\t");
	SPI_RF_DEBUG(" %02x", rf_pack->ctl.pac_num);
	*(pdata++) = rf_pack->ctl.pac_num;
	SPI_DECODE_DEBUG("\r\nREV_LEN:\t");
	SPI_RF_DEBUG(" %02x", rf_pack->rev_len);
	*(pdata++) = rf_pack->rev_len;
	SPI_DECODE_DEBUG("\r\nREV_DATA:\t");
	for(i=0;i<rf_pack->rev_len;i++)
		SPI_RF_DEBUG(" %02x", rf_pack->rev_data[i]);
	memcpy(pdata,rf_pack->rev_data,rf_pack->rev_len);
	pdata = pdata + rf_pack->rev_len;
	SPI_DECODE_DEBUG("\r\nPAC_LEN:\t");
	SPI_RF_DEBUG(" %02x", rf_pack->pack_len);
	*(pdata++) = rf_pack->pack_len;
	SPI_DECODE_DEBUG("\r\nPAC_DATA:\t");
	for(i=0;i<rf_pack->pack_len;i++)
		SPI_RF_DEBUG(" %02x", rf_pack->data[i]);
	memcpy(pdata,rf_pack->data,rf_pack->pack_len);
	pdata = pdata + rf_pack->pack_len;
	SPI_DECODE_DEBUG("\r\nCRC_XOR:\t");
	SPI_RF_DEBUG(" %02x", rf_pack->crc_xor);
	*(pdata++) = rf_pack->crc_xor;
	SPI_DECODE_DEBUG("\r\nEND:\t");
	SPI_RF_DEBUG(" %02x\r\n", rf_pack->end);
	*(pdata++) = rf_pack->end;

	spi_data->length = rf_pack->pack_len + \
		rf_pack->rev_len + sizeof(rf_pack_ctl_t) + 2 + 3;

	spi_pro_pack_update_crc( spi_data );
}

uint8_t bsp_spi_rx_data( uint32_t irq, spi_cmd_t *spi_rcmd )
{
	uint8_t i    = 0;
	uint8_t rx_s = 0, rx_cnt = 0;
	typedef  uint8_t (*spi_get_char)(void);
	spi_get_char pfun;

	memset(spi_rcmd->data, 0xFF, BUFFER_SIZE_MAX);
	spi_rcmd->length = 0x00;

	if( irq == NRF_RX_EXTI_LINE_RFIRQ )
	{
		SPI_DATA_DEBUG("[RX]r :");
		NRF_RX_CSN_LOW();
		pfun = spi_rx_get_char;
	}
	else
	{
		SPI_DATA_DEBUG("[TX]r :");
		NRF_TX_CSN_LOW();
		pfun = spi_tx_get_char;
	}
	
	for( i=0; i<spi_rcmd->length+6; i++ )
	{
		SPI_STATUS_DEBUG("\r\nrx_s:%d ",rx_s);
		switch(rx_s)
		{
			case 0:
				{
					spi_rcmd->header = pfun();
					if(spi_rcmd->header != 0x86)
						return 1;
					else
						rx_s = 1;
				}
				break;
				
			case 1:
				{
					spi_rcmd->dev_t = pfun();
					if(spi_rcmd->dev_t > 2)
						return 1;
					else
						rx_s = 2;
				}
				break;
		
			case 2:
				{
					spi_rcmd->cmd = pfun();
					rx_s = 3;
				}
				break;

			case 3:
				{
					spi_rcmd->length = pfun();
					rx_s = 4;
					rx_cnt = 0;
				}
				break;	

			case 4:
				{
					spi_rcmd->data[rx_cnt] = pfun();
					rx_cnt++;
					if( rx_cnt >= spi_rcmd->length)
						rx_s = 5;
				}
				break;

			case 5:
				{
					spi_rcmd->xor = pfun();
					if( spi_rcmd->xor != XOR_Cal((uint8_t *)&spi_rcmd->dev_t,spi_rcmd->length + 3))
						return 1;
					else
						rx_s = 6;
				}
				break;

			case 6:
				{
					spi_rcmd->end = pfun();
					if( spi_rcmd->end != 0x76)
						return 1;
					else
						rx_s = 0;
				}
				break;
			default: break;
		}
	}
	if( irq == NRF_RX_EXTI_LINE_RFIRQ )
		NRF_RX_CSN_HIGH();
	else
		NRF_TX_CSN_HIGH();

	SPI_DATA_DEBUG("\r\n");
	return 0;
}

uint8_t bsp_spi_tx_data( spi_cmd_t *spi_scmd )
{
	uint16_t i = 0;
	uint8_t tx_s = 0, tx_cnt = 0;
	typedef  void (*spi_get_char)(uint8_t data);
	spi_get_char pfun;

	if( spi_scmd->dev_t == DEVICE_TX )
	{
		SPI_DATA_DEBUG("[TX]s :");
		pfun = spi_tx_put_char;
		NRF_TX_CSN_LOW();
	}
	if( spi_scmd->dev_t == DEVICE_RX )
	{
		SPI_DATA_DEBUG("[RX]s :");
		pfun = spi_rx_put_char;
		NRF_RX_CSN_LOW();
	}

	for( i=0; i<spi_scmd->length+6; i++ )
	{
		SPI_STATUS_DEBUG("\r\ntx_s:%d ",tx_s);
		switch(tx_s)
		{
			case 0: pfun( spi_scmd->header ); tx_s = 1; break;
			case 1: pfun( spi_scmd->dev_t  ); tx_s = 2; break;
			case 2: pfun( spi_scmd->cmd    ); tx_s = 3; break;
			case 3:
				{
					pfun( spi_scmd->length );
					if( spi_scmd->length == 0)
						tx_s = 5;
					else
						tx_s = 4;
				}
				break;	
			case 4:
				{
					pfun( spi_scmd->data[tx_cnt] );
					tx_cnt++;
					if( tx_cnt == spi_scmd->length)
						tx_s = 5;
				}
				break;
			case 5: pfun( spi_scmd->xor ); tx_s = 6; break;
			case 6: pfun( spi_scmd->end ); tx_s = 0; break;
			default: break;
		}
	}

	if( spi_scmd->dev_t == DEVICE_RX )
		NRF_RX_CSN_HIGH();
	if( spi_scmd->dev_t == DEVICE_TX )
		NRF_TX_CSN_HIGH();

	SPI_DATA_DEBUG("\r\n");
	return 0;
}


