
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "app_error.h"
#include "nrf_delay.h"
#include "simple_uart.h"
#include "my_uart.h"
#include "my_misc_fun.h"
#include "my_radio.h"
#include "my_timer.h"
#include "ringbuf2.h"

//#define MAIN_DEBUG

#ifdef MAIN_DEBUG
#define main_debug  printf   
#else  
#define main_debug(...)                    
#endif 

static void spi_rx_data_handler(void);

int main (void)
{
    ret_code_t err_code;
	
	nrf_delay_ms(500);			//����ʹ�ã��ϵ綶����ʱ
	
	nrf_gpio_cfg_output(RX_PIN_NUMBER_1);
	
	clocks_start();
//	debug_uart_init();			//�����������ͽ��������ڽŲ�һ��
	timers_init();
	spi_gpio_init();
	my_spi_slave_init();

    err_code = my_tx_esb_init();
    APP_ERROR_CHECK(err_code);

	temp_timeout_start();			//RTCУ׼��ʱ��
	tx_payload.noack  = true;
	tx_payload.pipe   = NRF_PIPE;
	tx_ing_flag = false;
	nrf_tx_flg = false;
	
	while(true)
	{
		if(true == tx_ing_flag)					//����ϵͳ����
		{			
			if(true == nrf_tx_flg)			//������ʱ��ʱ����ʱ
			{
				if(nrf_tx_num > 0)
				{
					nrf_esb_flush_tx();
					sequence_re_tx_num = 1;
					nrf_esb_write_payload(&tx_payload);
					nrf_tx_flg = false;	
					nrf_tx_num--;
					
					if(0x02 == tx_payload.data[11])		//�����͵���ǰ��֡�����·������
					{
						preamble_num++;					
						tx_payload.data[9] = preamble_num;
						tx_payload.data[31] = XOR_Cal(&tx_payload.data[1],30);	//����У��
					}		
					
					if(0 == nrf_tx_num)
					{
						tx_ing_flag = false;
						nrf_transmit_timeout_stop();
					}
				}
			}
		}
		else if(false == tx_ing_flag)
		{
			if((get_ringbuf_status() != BUFF_EMPTY))
			{
				spi_rx_data_handler();
			}
		}
//		nrf_delay_ms(2);
//		printf("~~~~~~~~~~ \r\n");
	}
}


void spi_rx_data_handler(void)
{
	uint8_t i;
	uint8_t tmp_ringbuf_len = 0;
	uint8_t tmp_ringbuf_buf[250];

	ringbuf_read_data(tmp_ringbuf_buf,&tmp_ringbuf_len);
	
	memcpy(tx_payload.data,tmp_ringbuf_buf+2,tmp_ringbuf_len-2);
	tx_payload.length = tmp_ringbuf_len-2;
	nrf_tx_num = tmp_ringbuf_buf[0];						//�����ܴ���	
	nrf_tx_delay = tmp_ringbuf_buf[1];						//�ط���ʱ,100us����	
	
	main_debug("len:%d \r\n",tx_payload.length);
	
	/* ������Ҫ���ͣ��ñ�־λ��������ʱ��ʱ�� */
	if(nrf_tx_num > 0)
	{
		tx_ing_flag = true;
		nrf_tx_flg = false;
		if(0x02 == tx_payload.data[11])		//�����͵���ǰ��֡
		{
			preamble_num = 0;				//ǰ��֡��Ŵ�0��ʼ
			tx_payload.data[9] = preamble_num;
			tx_payload.data[31] = XOR_Cal(&tx_payload.data[1],30);	//����У��
		}
		nrf_transmit_timeout_start(nrf_tx_delay/10);
	}

	
	//�ط���������������ж�ԭ����delay����2.4G�ж����ȼ��ߣ��ᵼ��SPI�޷�����
//	for(i = 0;i < nrf_tx_num;i++)
//	{
//		nrf_delay_100us(nrf_tx_delay);
////		nrf_gpio_pin_toggle(RX_PIN_NUMBER_1);
//		nrf_esb_write_payload(&tx_payload);
//	}
}
