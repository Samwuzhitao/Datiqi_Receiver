/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "main.h"
#include "cJSON.h"
#include "app_rf_send_data_process.h"
#include "app_show_message_process.h"

task_tcb_typedef send_data_task;
static uint8_t send_data_status = 0;

void set_send_data_status( uint8_t new_status )
{
    send_data_status = new_status;
//printf("send_data_status = %d\r\n",send_data_status);
}

uint8_t get_send_data_status( void )
{
    return send_data_status;
}

/******************************************************************************
  Function:spi_process_revice_data
  Description:
        RF SPI buffer 处理函数
  Input :
  Return:
  Others:None
******************************************************************************/
void spi_process_revice_data( void )
{
    uint8_t  spi_message[255];
    uint8_t  spi_message_type = 0;
    uint8_t  Is_whitelist_uid = OPERATION_ERR;
    uint16_t uidpos = 0xFFFF;

    if(buffer_get_buffer_status(SPI_RBUF) == BUF_EMPTY)
        return;

    memset(spi_message,0,255);
    spi_read_data_from_buffer( SPI_RBUF, spi_message );
    spi_message_type = spi_message[13];

    #ifdef OPEN_BUFFER_DATA_SHOW
    {
        int i;
        DEBUG_BUFFER_ACK_LOG("%4d ", buffer_get_buffer_status(SPI_RBUF));
        DEBUG_BUFFER_ACK_LOG("Buffer Read :");
        for(i=5;i<9;i++)
            DEBUG_BUFFER_ACK_LOG("%02x",spi_message[i]);
    }
    #endif

    /* 检索白名单 */
    Is_whitelist_uid = search_uid_in_white_list(spi_message+5,&uidpos);

    if(Is_whitelist_uid != OPERATION_SUCCESS)
        return;

    /* 收到的是Data */
    if(spi_message_type == NRF_DATA_IS_USEFUL)
    {
        DEBUG_BUFFER_DTATA_LOG("[DATA] uid:%02x%02x%02x%02x, ",\
            *(spi_message+5),*(spi_message+6),*(spi_message+7),*(spi_message+8));
        DEBUG_BUFFER_DTATA_LOG("seq:%2x, pac:%2x\r\n",(uint8_t)*(spi_message+11),\
            (uint8_t)*(spi_message+12));

        if( wl.start == ON )
        {
            /* 将数据送入PC的发送缓冲区 */
            if(BUF_FULL != buffer_get_buffer_status(SPI_RBUF))
            {
                nrf_transmit_parameter_t transmit_config;

                /* 回复ACK */
                memcpy(transmit_config.dist,spi_message+5, 4 );
                transmit_config.package_type   = NRF_DATA_IS_ACK;
                transmit_config.transmit_count = 2;
                transmit_config.delay100us     = 20;
                transmit_config.is_pac_add     = PACKAGE_NUM_SAM;
                transmit_config.data_buf       = NULL;
                transmit_config.data_len       = 0;
                nrf_transmit_start( &transmit_config );
                spi_write_data_to_buffer(UART_RBUF,spi_message);
            }
        }
    }
    /* 收到的是Ack */
    if(spi_message_type == NRF_DATA_IS_ACK)
    {
        uint8_t Is_reviceed_uid = get_index_of_white_list_pos_status(SEND_DATA_ACK_TABLE,uidpos);
        if( Is_reviceed_uid == 0)
        {
            DEBUG_BUFFER_DTATA_LOG("[NEW_ACK] upos[%03d] uid:%02x%02x%02x%02x, ",uidpos,\
                *(spi_message+5),*(spi_message+6),*(spi_message+7),*(spi_message+8));
            DEBUG_BUFFER_DTATA_LOG("seq:%2x, pac:%2x \r\n",(uint8_t)*(spi_message+11), \
                (uint8_t)*(spi_message+12));
        }
        else
        {
            DEBUG_BUFFER_DTATA_LOG("[OLD_ACK] upos[%03d] uid:%02x%02x%02x%02x, ",uidpos,\
                *(spi_message+5),*(spi_message+6),*(spi_message+7),*(spi_message+8));
            DEBUG_BUFFER_DTATA_LOG("seq:%2x, pac:%2x \r\n",(uint8_t)*(spi_message+11), \
                (uint8_t)*(spi_message+12));
        }

        if(( get_send_data_status() == 0 ) || ( Is_reviceed_uid != 0 ))
            return;

        clear_index_of_white_list_pos(SEND_PRE_TABLE,uidpos);
        set_index_of_white_list_pos(SEND_DATA_ACK_TABLE,uidpos);
    }
}

/******************************************************************************
  Function:App_clickers_send_data_process
  Description:
        答题器发送处理逻辑函数
  Input :
  Return:
  Others:None
******************************************************************************/
void App_clickers_send_data_process( void )
{
//    spi_process_revice_data();

////    if(( send_data_status == SEND_500MS_DATA_STATUS )||
//         ( send_data_status == SEND_2S_DATA_STATUS    ))
//    {
//        if( send_data_status == SEND_2S_DATA_STATUS    )
//        {
//            uint8_t status = 0;
//            static uint8_t err_count = 0;

//            if( status != 0 )
//            {
//                nrf1_rst_init();
//                nrf2_rst_init();
//                NRF_RX_RST_LOW();
//                NRF_TX_RST_LOW();
//                DelayMs(50);
//                NRF_RX_RST_HIGH();
//                NRF_TX_RST_HIGH();
//                nrf1_rst_deinit();
//                nrf2_rst_deinit();
//                err_count++;
//                if( err_count >= 10)
//                {
//                    err_count = 0;
//                    send_data_status = 0;
//                }
//            }
//            else
//                err_count = 0;
//        }

//        send_data_status = send_data_status + 1;
//    }
}




