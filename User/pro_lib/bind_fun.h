#ifndef __BIND_FUN_H_
#define __BIND_FUN_H_
#include "answer_fun.h"

#define BI_FUN                  0
#define BI_MODE                 1
#define BI_LIST                 2
#define BI_UID                  3

#define BIND_ITEM_NUM           4

typedef struct
{
    uint8_t cmd;
    uint8_t len;
    uint8_t jsq_uid[4];
    uint8_t pin[2];
    uint8_t tx_ch;
    uint8_t rx_ch;
    uint8_t stdid[10];
}wireless_mode1_cmd_t;

#define RF_CMD_WIRELESS_BIND_MODE1_CONF {.cmd          = 0x40,    \
                                         .len          = 0x12,    \
                                         .jsq_uid      = { 0 },   \
                                         .pin          = { 0 },   \
                                         .tx_ch        = clicker_set.N_CH_TX,   \
                                         .rx_ch        = clicker_set.N_CH_RX,   \
                                         .stdid        = { 0 }    }

extern rf_pack_t rf_bind;
extern Timer_typedef rf_bind_300ms_timer, rf_bind_500ms_timer;

int8_t rf_send_bind_stop( void );
int8_t rf_send_bind_start( void );
int8_t serial_cmd_wireless_bind_decode( char *json_str, uint8_t *mode );
void rf_send_bind_process_timer_init( void );
void rf_bind_process( void );

#endif
