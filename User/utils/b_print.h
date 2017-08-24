#ifndef	__APP_SHOW_MESSAGE_PROCESS_H_
#define	__APP_SHOW_MESSAGE_PROCESS_H_
#include "stm32f10x.h"

//#define FILTER_NOUSE_CHAR

void b_print(const char *fmt, ...) ;
uint8_t b_strncmp( uint8_t *data1, uint8_t *data2, uint8_t n );

#endif
