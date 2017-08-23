#ifndef __BIND_FUN_H_
#define __BIND_FUN_H_
#include "answer_fun.h"

#define BI_FUN                  0
#define BI_MODE                 1
#define BI_LIST                 2
#define BI_UID                  3

#define BIND_ITEM_NUM           4

int8_t serial_cmd_wireless_bind_decode( char *json_str, uint8_t *mode );

#endif
