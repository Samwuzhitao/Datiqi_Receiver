#ifndef __JSON_FUN_H_
#define __JSON_FUN_H_
#include "main.h"

typedef struct
{
	char    *key;
	uint8_t str_len;
	void    (*j_fun)( uint8_t *pdada, char *v_str );
}json_item_t;

void exchange_json_format( char *out, char old_format, char new_format );
char *parse_json_item(char *pdata_str, char *key_str, char *value_str);

#endif
