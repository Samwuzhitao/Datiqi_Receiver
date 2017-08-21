#include "main.h"
#include "answer_fun.h"
#include <stdio.h>
#include <stdlib.h>

void exchange_json_format( char *out, char old_format, char new_format )
{
	char *pdata = out;
	while(*pdata != '\0')
	{
		if(*pdata == old_format)
		{
			*pdata = new_format;
		}
		pdata++;
	}
}

char *parse_json_item( char *pdata_str, char *key_str, char *value_str )
{
	uint8_t i = 0;
	char *pdata = pdata_str;
	while(*pdata != ':')
	{
		if(*pdata != '{')
		{
			if((*pdata != '"') && (*pdata != ','))
			{
			  //printf("%c",*pdata);
				key_str[i++] = *pdata;
			}
			if(*pdata == ',')
				i = 0;
		}
		pdata++;
	}
	key_str[i] = '\0';

	i = 0;
	pdata++;
	while((*pdata != ',') && (*pdata != '[') && (*pdata != '}'))
	{
		if(*pdata != '"')
		{
		  //printf("%c",*pdata);
			value_str[i++] = *pdata;
		}
		pdata++;
	}
	pdata++;
	value_str[i] = '\0';
  printf("KEY:%15s  VALUE:%s \r\n",key_str,value_str);
	return (pdata);
}
