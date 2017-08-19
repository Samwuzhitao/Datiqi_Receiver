#ifndef __JSON_FUN_H_
#define __JSON_FUN_H_

void exchange_json_format( char *out, char old_format, char new_format );
char *parse_json_item(char *pdata_str, char *key_str, char *value_str);

#endif
