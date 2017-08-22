/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "ring_buf.h"
#include "b_print.h"
#include "stdio.h"
#include "stdarg.h"

/******************************************************************************
  Function:b_print
  Description:
		打印信息输出到Buffer中
  Input :
  Return:
  Others:None
******************************************************************************/
void b_print(const char *fmt, ...)                                       
{
	uint8_t r_index = 0;
	char str[256];
	va_list args;

	memset(str,0,256);

	va_start(args, fmt);
	vsprintf(str,fmt,args);
	va_end(args);

	/* JSON 剔除格式化输出字符 */
	
#ifdef FILTER_NOUSE_CHAR
	{
		static uint8_t skip_flg = 0x00; 
	  char *pdata = str;

		while( *pdata != '\0' )
		{
			if( *pdata ==  '\"')
			{
				skip_flg = skip_flg ^ 0x01;
				*pdata    = '\'';
			}

			if((skip_flg == 1) || (*pdata > 32))
			{
				if(*pdata != str[r_index])
					str[r_index] = *pdata;
				pdata++;
				r_index++;
			}
			else
			{
				pdata++;
			}
		}
		if( *pdata == '\0' )
			str[r_index] = '\0';
	}
#else
	r_index = strlen(str);
#endif
	print_write_data_to_buffer( str, r_index );
}
