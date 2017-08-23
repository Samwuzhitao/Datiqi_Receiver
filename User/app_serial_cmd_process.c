/**
  ******************************************************************************
  * @file   	app_send_data_process.c
  * @author  	sam.wu
  * @version 	V1.0.0.0
  * @date   	2015.11.05
  * @brief
  * @Changelog :
  *    [1].Date   : 2016_8-26
	*        Author : sam.wu
	*        brief  : 尝试分离数据，降低代码的耦合度
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "main.h"
#include "cJSON.h"
#include "app_spi_send_data_process.h"
#include "app_card_process.h"
#include "b_print.h"
#include "json_decode.h"
#include "answer_fun.h"
#include "bind_fun.h"

typedef  void (*pFunction)(void);

/* Private variables ---------------------------------------------------------*/
extern uint16_t list_tcb_table[UID_LIST_TABLE_SUM][WHITE_TABLE_LEN];

/* 暂存题目信息，以备重发使用 */
static uint8_t rjson_index = 0;
static uint8_t uart_send_s = 0;
uint8_t dtq_self_inspection_flg = 0;
//uint8_t logic_pac_add = 0;

extern wl_typedef       wl;
extern revicer_typedef  revicer;
extern task_tcb_typedef card_task;
/* Private functions ---------------------------------------------------------*/
const static serial_cmd_typedef cmd_list[] = {
{"clear_wl",           sizeof("clear_wl"),           serial_cmd_clear_uid_list        },
{"answer_stop",        sizeof("answer_stop"),        serial_cmd_answer_stop           },
{"bind_stop",          sizeof("bind_stop"),          serial_cmd_bind_stop             },
{"get_device_info",    sizeof("get_device_info"),    serial_cmd_get_device_no         },
{"set_channel",        sizeof("set_channel"),        serial_cmd_set_channel           },
{"set_tx_power",       sizeof("set_tx_power"),       serial_cmd_set_tx_power          },
{"check_config",       sizeof("check_config"),       serial_cmd_check_config          },
{"set_student_id",     sizeof("set_student_id"),     serial_cmd_set_student_id        },
{"one_key_off",        sizeof("one_key_off"),        serial_cmd_one_key_off           },
{"bootloader",         sizeof("bootloader"),         serial_cmd_bootloader            },
{"attendance_24g",     sizeof("attendance_24g"),     serial_cmd_attendance_24g        },
{"set_raise_hand",     sizeof("set_raise_hand"),     serial_cmd_raise_hand_sign_in_set},
{"set_sign_in",        sizeof("set_sign_in"),        serial_cmd_raise_hand_sign_in_set},
{"dtq_debug_set",      sizeof("dtq_debug_set"),      serial_cmd_dtq_debug_set         },
{"dtq_self_inspection",sizeof("dtq_self_inspection"),serial_cmd_self_inspection       },
{"NO_USE",             sizeof("NO_USE"),             NULL                             }
};

static void uart_update_answers(void);
static void uart_cmd_decode(void);

/******************************************************************************
  Function:App_seirial_cmd_process
  Description:
		串口进程处理函数
  Input :None
  Return:None
  Others:None
******************************************************************************/
void uart_cmd_decode(void)
{
	/* 提取中断缓存数据到 */
	uint8_t temp_rbuf[30];
	int8_t  r_count = 0;
	r_count = irq_buf_read( temp_rbuf );
	if ( r_count > 0 )
	{
		uint8_t i = 0;
		for( i=0; i<r_count; i++ )
			uart_json_decode( temp_rbuf[i] );
	}

	if( rjson_cnt > 0 )
	{
		char header[30];
		char *pdata = (char *)rbuf[rjson_index];
		/* 增加对'的支持 */
		exchange_json_format( pdata, '\'', '\"' );
		memcpy(header,pdata+8,14);
		if( strncmp( header, "answer_start", sizeof("answer_start")-1)== 0 )
		{
			serial_cmd_answer_start( pdata );
		}
		else if( strncmp( header, "bind_start", sizeof("bind_start")-1)== 0 )
		{
			serial_cmd_wireless_bind_start( pdata );
		}
		else
		{
			cJSON *json;
			json = cJSON_Parse((char *)rbuf[rjson_index]);
			if (!json)
			{
				  b_print("{\r\n");
	        b_print("  \"fun\": \"Error\",\r\n");
					b_print("  \"description\": \"json syntax error!\"\r\n");
					b_print("}\r\n");
			}
			else
			{
				uint8_t i = 0, is_know_cmd = 0;
				if( cJSON_HasObjectItem(json, "fun") )
				{
					char *p_cmd_str = cJSON_GetObjectItem(json, "fun")->valuestring;

					while(cmd_list[i].cmd_fun != NULL)
					{
						if(strncmp(p_cmd_str, cmd_list[i].cmd_str,
							 cmd_list[i].cmd_len)== 0)
						{
							cmd_list[i].cmd_fun(json);
							is_know_cmd = 1;
						}
						i++;
					}

					if(is_know_cmd == 0)
					{
						printf("{'fun':'Error','description':'unknow cmd!'}");
					}
				}
			}
			cJSON_Delete(json);
		}

		rjson_cnt--;
		memset( pdata, 0, JSON_BUFFER_LEN );
		rjson_index = (rjson_index + 1) % JSON_ITEM_MAX;
	}
}


/******************************************************************************
  Function:App_seirial_cmd_process
  Description:
		串口进程处理函数
  Input :None
  Return:None
  Others:None
******************************************************************************/
void App_seirial_cmd_process(void)
{
	/* send process data to pc */
	uart_update_answers();
	/* serial cmd process */
	uart_cmd_decode();

	/* enable interrupt Start send data*/
	if( BUF_EMPTY != buffer_get_buffer_status(PRINT_BUF) )
		USART_ITConfig( USART1pos, USART_IT_TXE, ENABLE );
}

/******************************************************************************
  Function:uart_update_answers
  Description:
		串口发送指令函数
  Input :None
  Return:None
  Others:None
******************************************************************************/
static void uart_update_answers(void)
{
	static rssi_rf_pack_t rssi_pack;
	static uint8_t r_index = 0,r_is_last_data_full = 0;
	static answer_cmd_t *answer_cmd;

	if( uart_send_s == 0 )
	{
		if(buffer_get_buffer_status( SPI_RBUF ) == BUF_EMPTY)
			return;
		else
		{
			rf_read_data_from_buffer( SPI_RBUF, &rssi_pack );	
			b_print("{\r\n");
			b_print("  \"fun\": \"update_answer_list\",\r\n");
			b_print("  \"card_id\": \"%08x\",\r\n", *(uint32_t *)(rssi_pack.rf_pack.ctl.src_uid) );
			b_print("  \"rssi\": \"-%d\",\r\n", rssi_pack.rssi );
			b_print("  \"update_time\": \"%04d-%02d-%02d %02d:%02d:%02d:%03d\",\r\n",
					system_rtc_timer.year, system_rtc_timer.mon, system_rtc_timer.date, 
					system_rtc_timer.hour, system_rtc_timer.min, system_rtc_timer.sec, 
					system_rtc_timer.ms);
			b_print("  \"answers\": [\r\n");
		  uart_send_s = 1;
		  r_is_last_data_full = 0;
			r_index = 0;
		}
		return;
	}

	if( uart_send_s == 1 )
	{
		do
		{
			answer_cmd = (answer_cmd_t *)(rssi_pack.rf_pack.data + r_index);
			r_index = r_index + answer_cmd->len + 2;
			if( answer_cmd->cmd == 0x10 )
			{
				uint8_t  i = 0, *prdata = answer_cmd->buf + 1;
				q_info_t q_tmp = { 0, 0, 0 };
				char q_t_str[2];
		    char q_r_str[7];
				do
				{
					if(r_is_last_data_full == 0)
					{
						q_tmp.type  = prdata[i] & 0x0F;
						q_tmp.id    = ((prdata[i]   & 0xF0) >> 4)| 
													((prdata[i+1] & 0x0F) << 4);
						q_tmp.range = ((prdata[i+1] & 0xF0) >> 4)| 
													((prdata[i+2] & 0x0F) << 4);
						i = i + 2;
						r_is_last_data_full = 1;
					}
					else
					{
						q_tmp.type  = (prdata[i] & 0xF0) >> 4;
						q_tmp.id    = prdata[i+1];
						q_tmp.range = prdata[i+2];
						i = i + 3;
						r_is_last_data_full = 0;
					}
					dtq_decode_answer( &q_tmp, q_r_str, q_t_str );
					b_print("    {");
					b_print("\"type\": \"%s\", ",q_t_str);
					b_print("\"id\": \"%d\", ", q_tmp.id);
					b_print("\"answer\": \"%s\" ",q_r_str);
				  b_print("},\r\n");
				}while( i < answer_cmd->len );
				b_print("}\r\n");
				b_print("  ]\r\n");
				b_print("}\r\n");
			}
		}while( r_index < rf_data.pack_len );
		uart_send_s = 0;
		return;
	}

//	if( uart_send_s == 2 )
//	{
//		if(( Cmdtype == 0x10 ) && ( wl.start == ON ))
//		{
//			uint8_t  raise_sign = 0;

//			raise_sign  = *(spi_message+14+spi_message[14]+2+1);
//			b_print("{\r\n");
//			b_print("  \"fun\": \"update_answer_list\",\r\n");
//			b_print("  \"card_id\": \"%010u\",\r\n", *(uint32_t *)( wl.uids[uidpos].uid) );
//			b_print("  \"rssi\": \"-%d\",\r\n", wl.uids[uidpos].rssi );
//			b_print("  \"update_time\": \"%04d-%02d-%02d %02d:%02d:%02d:%03d\",\r\n",
//				system_rtc_timer.year, system_rtc_timer.mon, system_rtc_timer.date, 
//				system_rtc_timer.hour, system_rtc_timer.min, system_rtc_timer.sec, 
//				system_rtc_timer.ms);
//			b_print("  \"raise_hand\": \"%d\",\r\n", (raise_sign & 0x01) ? 1: 0 );
//			b_print("  \"attendance\": \"%d\",\r\n", (raise_sign & 0x02) ? 1: 0 );
//			b_print("  \"answers\": [\r\n");
//			uart_send_s = 3;
//		}
//		else
//		{
//			memset(spi_message,0,255);
//			uart_send_s = 0;
//			Cmdtype     = 0;
//		}
//		return;
//	}
//	
//	if( uart_send_s == 3 )
//	{
//		char q_t_str[2];
//		char q_r_str[7];
//		q_info_t q_tmp = {0,0,0};
//		
//		uint8_t  *prdata;
//		prdata   = spi_message+14+spi_message[14]+2+2;
//		
//		if( r_index < DataLen-3 )
//		{
//			if(r_is_last_data_full == 0)
//			{
//				q_tmp.type  = prdata[r_index] & 0x0F;
//				q_tmp.id    = ((prdata[r_index]   & 0xF0) >> 4)| 
//											((prdata[r_index+1] & 0x0F) << 4);
//				q_tmp.range = ((prdata[r_index+1] & 0xF0) >> 4)| 
//											((prdata[r_index+2] & 0x0F) << 4);
//				r_index = r_index + 2;
//				r_is_last_data_full = 1;
//			}
//			else
//			{
//				q_tmp.type  = (prdata[r_index] & 0xF0) >> 4;
//				q_tmp.id    = prdata[r_index+1];
//				q_tmp.range = prdata[r_index+2];
//				r_index = r_index + 3;
//				r_is_last_data_full = 0;
//			}
//			memset( q_r_str, 0x00, 7 );
//			memset( q_t_str, 0x00, 2 );
//			dtq_decode_answer( &q_tmp, q_r_str, q_t_str );

//			b_print("    {");
//			b_print("\"type\": \"%s\", ",q_t_str);
//			b_print("\"id\": \"%d\", ", q_tmp.id);
//			b_print("\"answer\": \"%s\" ",q_r_str);
//			if( r_index < DataLen-2 )
//				b_print("},\r\n");
//			else
//			{
//				b_print("}\r\n");
//				b_print("  ]\r\n");
//				b_print("}\r\n");
//				uart_send_s = 4;
//				r_index     = 0;
//				r_is_last_data_full = 0;
//			}
//			return;
//		}
//	}
//	
//	if(uart_send_s == 4)
//	{
//		uint8_t is_retuen_ack = 0;
//		if(is_retuen_ack == 1)
//		{
//			uart_send_s = 2;
//		}
//		else
//		{
//			memset(spi_message,0,255);
//			uart_send_s = 0;
//			Cmdtype     = 0;
//		}
//	}
}

void serial_cmd_clear_uid_list(const cJSON *object)
{
	uint8_t err = initialize_white_list();
	EE_WriteVariable(CPU_ADDR_CLONE_FLAG,0);
	get_mcu_uid();

	b_print("{\r\n");
	b_print("  \"fun\": \"clear_wl\",\r\n");
	b_print("  \"result\": \"%d\"\r\n",(err==OPERATION_SUCCESS)?0:-1);
	b_print("}\r\n");
}

void serial_cmd_bind_stop(const cJSON *object)
{
	uint8_t card_status = rf_get_card_status();
	if( card_status == 0 )
	{
		wl.match_status = OFF;
		rf_set_card_status(0);
		PcdHalt();
		PcdAntennaOff();
		b_print("{\r\n");
		b_print("  \"fun\": \"bind_stop\",\r\n");
		b_print("  \"result\": \"0\"\r\n");
		b_print("}\r\n");
	}
}

void serial_cmd_get_device_no(const cJSON *object)
{
	int8_t tx_power = 0,attend_tx_ch = 0;
	char str[20];
	uint8_t i,is_pos_use = 0;
	uint8_t count = 0;

	b_print("{\r\n");
	b_print("  \"fun\": \"get_device_info\",\r\n");
	memset(str,0,20);
	sprintf(str, "%010u" , *(uint32_t *)(revicer.uid));
	b_print("  \"device_id\": \"%s\",\r\n",str);
	b_print("  \"software_version\": \"v%d.%d.%d\",\r\n",software[0],software[1],software[2]);
	b_print("  \"hardware_version\": \"%s\",\r\n",hardware);
	b_print("  \"company\": \"zkxltech\",\r\n");
	memset(str,0,10);
	sprintf(str, "%d" , clicker_set.N_CH_TX);
	b_print("  \"tx_ch\": \"%s\",\r\n",str);
	memset(str,0,10);
	sprintf(str, "%d" , clicker_set.N_CH_RX);
	b_print("  \"rx_ch\": \"%s\",\r\n",str);
	memset(str,0,10);
	tx_power = clicker_set.N_TX_POWER;
	sprintf(str, "%d" , tx_power);
	b_print("  \"tx_power\": \"%s\",\r\n",str);
	if( clicker_set.N_24G_ATTEND & 0x80 )
	{
		attend_tx_ch = clicker_set.N_24G_ATTEND & 0x7F;
		b_print("  \"pro_index\": \"%d\",\r\n",attend_tx_ch);
	}
	else
		b_print("  \"pro_index\": \"\",\r\n");
	b_print("  \"list\": [\r\n");

	for(i=0; i < MAX_WHITE_LEN; i++)
	{
		is_pos_use = get_index_of_white_list_pos_status(0,i);
		if( is_pos_use == 1 )
		{
			count++;
			b_print("    {");;
			b_print("  \"upos\": \"%d\",", i );
			b_print("  \"uid\": \"%010u\"", *(uint32_t *)( wl.uids[i].uid) );
			if( count < wl.len )
				b_print(" },\r\n");
			else
				b_print(" }\r\n");
		}
	}
	b_print("  ]\r\n");
	b_print(" }\r\n");
}
void serial_cmd_one_key_off(const cJSON *object)
{
	uint8_t sdata_index = 0;
	uint8_t *pSdata;

	/* 准备发送数据 */
	pSdata = (uint8_t *)rf_var.tx_buf;
	*(pSdata+(sdata_index++)) = 0x01;
	rf_var.cmd = 0x25;
	rf_var.tx_len = sdata_index+1 ;

	/* 发送数据 */
	{
//		nrf_transmit_parameter_t transmit_config;

		/* 准备发送数据管理块 */
//		memset(list_tcb_table[SEND_DATA_ACK_TABLE],0,16);
		
//		memset(nrf_data.dtq_uid,    0x00, 4);
//		memcpy(nrf_data.jsq_uid,    revicer.uid, 4);
//		memset(transmit_config.dist,0x00, 4);
		//send_data_process_tcb.is_pack_add   = PACKAGE_NUM_ADD;
		//send_data_process_tcb.logic_pac_add = PACKAGE_NUM_SAM;

		/* 启动发送数据状态机 */
		//set_send_data_status( SEND_500MS_DATA_STATUS );
	}

	b_print("{\r\n");
	b_print("  \"fun\": \"one_key_off\",\r\n");
	b_print("  \"result\": \"0\"\r\n");
	b_print("}\r\n");
}

void serial_cmd_set_channel(const cJSON *object)
{
	int8_t status;
	nor_buf_t t_conf = RF_TX_CH_DEFAULT_CONF;
	nor_buf_t r_conf = RF_RX_CH_DEFAULT_CONF;

	t_conf.t_buf[RF_ADDR_CH] = atoi(cJSON_GetObjectItem(object, "tx_ch")->valuestring);
	r_conf.t_buf[RF_ADDR_CH] = atoi(cJSON_GetObjectItem(object, "rx_ch")->valuestring);
	if((( t_conf.t_buf[RF_ADDR_CH] >= 1) && ( t_conf.t_buf[RF_ADDR_CH] <= 11)) && 
		 (( r_conf.t_buf[RF_ADDR_CH] >= 1) && ( r_conf.t_buf[RF_ADDR_CH] <= 11)) &&
	    ( t_conf.t_buf[RF_ADDR_CH] != r_conf.t_buf[RF_ADDR_CH] ))
	{
		/* 设置接收的信道 */
		clicker_set.N_CH_TX = t_conf.t_buf[RF_ADDR_CH];
		clicker_set.N_CH_RX = r_conf.t_buf[RF_ADDR_CH];
		status |= spi_write_cmd_to_rx( r_conf.t_buf, r_conf.len );
		status |= spi_write_cmd_to_tx( t_conf.t_buf, t_conf.len );
	}
	else
		status = -1;
	/* 打印返回 */
	b_print("{\r\n");
	b_print("  \"fun\": \"set_channel\",\r\n");
	b_print("  \"result\": \"%d\"\r\n",(int8_t)(status));
	b_print("}\r\n");
}

void serial_cmd_set_tx_power(const cJSON *object)
{
	char str[3];
	int8_t tx_power = 0x04;
	int8_t status;

	tx_power = atoi(cJSON_GetObjectItem(object, "tx_power")->valuestring);
	
	if(( tx_power >= 1) && ( tx_power <= 5))
	{
		clicker_set.N_TX_POWER = tx_power;
		EE_WriteVariable( CPU_TX_POWER_POS_OF_FEE , clicker_set.N_TX_POWER );
		status = 0;
	}
	else
	{
		status = -1;
	}

	/* 打印返回 */
	b_print("{\r\n");
	b_print("  \"fun\": \"set_tx_power\",\r\n");
	sprintf(str, "%d" , (int8_t)(status));
	b_print("  \"result\": \"%s\"\r\n",str);
	b_print("}\r\n");
}

void serial_cmd_set_student_id(const cJSON *object)
{
	cJSON *root;
	char *out;
	int8_t status;
	uint8_t i = 0;
	char str[3];
	uint8_t card_status = rf_get_card_status();

	char    *prdata = cJSON_GetObjectItem(object,"student_id")->valuestring;
	uint8_t len = strlen( prdata );

	uint8_t *pwdata = card_task.stdid;

	if((len > 0) && (len <= 20))
	{
		uint8_t wdata_index = 0,rdata_index = 0;
		
		memset(pwdata,0,10);
		
		while( rdata_index < len )
		{
			*pwdata = (((prdata[rdata_index  ]-'0') << 4) & 0xF0) | 
			           ((prdata[rdata_index+1]-'0') & 0x0F);
			pwdata++;
			wdata_index++;
			rdata_index = rdata_index + 2;
		}
		
		if( len % 2 )
			*(pwdata-1) |= 0x0F; 

		for(i=wdata_index; i<10; i++)
		{
			*pwdata = 0xFF;
			pwdata++;
		}
		if(card_status == 0)
		{
			wl.match_status = ON;
			wl.weite_std_id_status = ON;
			rf_set_card_status(1);
			status = 0;
		}
		else
		{
			status = -1;
		}
	}
	else
	{
		status = -2;
	}

	/* 打印返回 */
	root = cJSON_CreateObject();
	cJSON_AddStringToObject(root, "fun", "set_student_id" );
	sprintf(str, "%d" , (int8_t)(status));
	cJSON_AddStringToObject(root, "result", str );
	out = cJSON_Print(root);
	exchange_json_format( out, '\"', '\"' );
	b_print("%s", out);
	cJSON_Delete(root);
	free(out);
}

void serial_cmd_raise_hand_sign_in_set(const cJSON *object)
{
	uint8_t raise_hand,sign_in;
	char    *p_sign_in_data;
	char    *p_raise_hand_data;
	char *p_cmd_str = cJSON_GetObjectItem(object, "fun")->valuestring;

	b_print("{\r\n");
	if(strncmp(p_cmd_str, "set_raise_hand", sizeof("set_raise_hand")-1)== 0 )
	{
		p_raise_hand_data = cJSON_GetObjectItem(object,"raise_hand")->valuestring;
		raise_hand = atoi(p_raise_hand_data);
		b_print("  \"fun\": \"set_raise_hand\",\r\n");
	}

	if(strncmp(p_cmd_str, "set_sign_in", sizeof("set_sign_in")-1)== 0 )
	{
		p_sign_in_data = cJSON_GetObjectItem(object,"attendance")->valuestring;
		sign_in = atoi(p_sign_in_data);
		b_print("  \"fun\": \"set_sign_in\",\r\n");
	}

	/* 准备发送数据 */
	if( raise_hand <= 1 )
	{
		if( raise_hand == 0 )
			rf_var.tx_buf[0] &= 0xFE;
		else
			rf_var.tx_buf[0] |= 0x01;
	}

	if( sign_in <= 1 )
	{
		if( sign_in == 0 )
			rf_var.tx_buf[0] &= 0xFD;
		else
			rf_var.tx_buf[0] |= 0x02;
	}

	if(rf_var.tx_len == 0)
	{
		rf_var.tx_len = 1;
		rf_var.cmd = 0x10;
	}

	b_print("  \"result\": \"0\"\r\n");
	b_print("}\r\n");
}

void serial_cmd_check_config(const cJSON *object)
{
	int8_t tx_power = 0;
	int8_t attend_tx_ch = 0;
	char str[20];
	uint8_t i,is_pos_use = 0;
	uint8_t count = 0;

	b_print("{\r\n");
	b_print("  \"fun\": \"check_config\",\r\n");
	memset(str,0,20);
	sprintf(str, "%010u" , *(uint32_t *)(revicer.uid));
	b_print("  \"addr\": \"%s\",\r\n",str);
	memset(str,0,10);
	sprintf(str, "%d" , clicker_set.N_CH_TX);
	b_print("  \"tx_ch\": \"%s\",\r\n",str);
	memset(str,0,10);
	sprintf(str, "%d" , clicker_set.N_CH_RX);
	b_print("  \"rx_ch\": \"%s\",\r\n",str);
	memset(str,0,10);
	tx_power = clicker_set.N_TX_POWER;
	sprintf(str, "%d" , tx_power);
	b_print("  \"tx_power\": \"%s\",\r\n",str);

	if( clicker_set.N_24G_ATTEND & 0x80 )
	{
		attend_tx_ch = clicker_set.N_24G_ATTEND & 0x7F;
		b_print("  \"pro_index\": \"%d\",\r\n",attend_tx_ch);
	}
	else
		b_print("  \"pro_index\": \"\",\r\n");

	b_print("  \"list\": [\r\n");

	for(i=0; i < MAX_WHITE_LEN; i++)
	{
		is_pos_use = get_index_of_white_list_pos_status(0,i);
		if( is_pos_use == 1 )
		{
			count++;
			b_print("    {");
			memset(str,0,20);
			sprintf(str, "%d" , i);
			b_print("  \"upos\": \"%s\",", str );
			memset(str,0,20);
			sprintf(str, "%010u" , *(uint32_t *)( wl.uids[i].uid));
			b_print("  \"uid\": \"%s\"", str );
			if( count < wl.len )
				b_print(" },\r\n");
			else
				b_print(" }\r\n");
		}
	}
	b_print("  ]\r\n");
	b_print("}\r\n");
}

void serial_cmd_attendance_24g(const cJSON *object)
{
	char str[3];
	uint8_t attend = 0x00;
	int8_t  tx_ch = 81;
	int8_t status;

	attend = atoi(cJSON_GetObjectItem(object, "is_open")->valuestring);
	tx_ch  = atoi(cJSON_GetObjectItem(object, "pro_index")->valuestring);
	
	if(( attend <= 1) && (( tx_ch >= 0) && ( tx_ch <= 12)))
	{
		if (attend == 1)
			clicker_set.N_24G_ATTEND = (uint8_t)tx_ch | 0x80;
		else
			clicker_set.N_24G_ATTEND = (uint8_t)tx_ch & 0x7F;

		EE_WriteVariable( CPU_24G_ATTENDANCE_OF_FEE , clicker_set.N_24G_ATTEND );
		status = 0;
	}
	else
	{
		status = -1;
	}

	/* 打印返回 */
	b_print("{\r\n");
	b_print("  \"fun\": \"attendance_24g\",\r\n");
	sprintf(str, "%d" , (int8_t)(status));
	b_print("  \"result\": \"%s\"\r\n",str);
	b_print("}\r\n");
}

void serial_cmd_dtq_debug_set(const cJSON *object)
{
	char str[3];
	uint8_t open_debug = 0x00;
	int8_t status;

	open_debug = atoi(cJSON_GetObjectItem(object, "open_debug")->valuestring);
	
	if( open_debug <= 1 )
	{
		if (open_debug == 1)
			clicker_set.N_OPEN_DENUG = (uint8_t)open_debug | 0x01;
		else
			clicker_set.N_OPEN_DENUG = (uint8_t)open_debug & 0xFE;

		EE_WriteVariable( CPU_OPEN_DEBUG_OF_FEE , clicker_set.N_OPEN_DENUG );
		status = 0;
	}
	else
	{
		status = -1;
	}

	/* 打印返回 */
	b_print("{\r\n");
	b_print("  \"fun\": \"dtq_debug_set\",\r\n");
	sprintf(str, "%d" , (int8_t)(status));
	b_print("  \"result\": \"%s\"\r\n",str);
	b_print("}\r\n");
}

void serial_cmd_self_inspection(const cJSON *object)
{
	uint8_t sdata_index = 0;
	uint8_t *pSdata;
	uint8_t rf_status = 0;

	/* 准备发送数据 */
	pSdata = (uint8_t *)rf_var.tx_buf;
	*(pSdata+(sdata_index++)) = 0x00;
	*(pSdata+(sdata_index++)) = 0x00;
	rf_var.cmd = 0xF1;
	rf_var.tx_len = sdata_index+1 ;

	/* 发送数据 */
	{
//		nrf_transmit_parameter_t transmit_config;

		/* 准备发送数据管理块 */
//		memset(list_tcb_table[SEND_DATA_ACK_TABLE],0,16);

//		memset(nrf_data.dtq_uid,    0x00, 4);
//		memset(nrf_data.jsq_uid,    0x00, 4);
//		memset(transmit_config.dist,0x00, 4);
		//send_data_process_tcb.is_pack_add   = PACKAGE_NUM_ADD;
		//send_data_process_tcb.logic_pac_add = PACKAGE_NUM_SAM;

		/* 启动发送数据状态机 */
		//set_send_data_status( SEND_500MS_DATA_STATUS );
		dtq_self_inspection_flg = 1;
	}
	
	rf_status = clicker_config_default_set();

	b_print("{\r\n");
	b_print("  \"fun\": \"dtq_self_inspection\",\r\n");
	switch(rf_status)
	{
		case 0:  b_print("  \"tx_rx_check\": \"OK\",\r\n"); break;
		case 1:  b_print("  \"tx_rx_check\": \"DEVICE ERROR\",\r\n"); break;
		default: b_print("  \"tx_rx_check\": \"FIRMWARE ERROR\",\r\n"); break;
	}
	b_print("  \"result\": \"0\"\r\n");
	b_print("}\r\n");
}

void serial_cmd_answer_stop(const cJSON *object)
{
	uint8_t err = rf_pack_add_answer_stop_cmd();
	err =	rf_send_data_start();
	b_print("{\r\n");
	b_print("  \"fun\": \"answer_stop\",\r\n");
	b_print("  \"result\": \"%d\"\r\n",err);
	b_print("}\r\n");
}

void serial_cmd_bootloader(const cJSON *object)
{
	/* 打印返回 */
	b_print("{\r\n");
	b_print("  \"fun\": \"bootloader\",\r\n");
	b_print("  \"result\": \"0\"\r\n");
	b_print("}\r\n");
	DelayMs(200);
	{
		uint32_t JumpAddress;
		pFunction JumpToBootloader;
		/* Jump to user application */
		JumpAddress = *(__IO uint32_t*) (0x8000000 + 4);
		JumpToBootloader = (pFunction) JumpAddress;
		/* Initialize user application's Stack Pointer */
		__set_MSP(*(__IO uint32_t*) 0x8000000);
		JumpToBootloader();
	}
}
/**************************************END OF FILE****************************/
