/*-------------------------------- Include File -----------------------------------------*/
#include "timer.h"
/*-------------------------------- Macro definition -------------------------------------*/
//#define TIMER_DEBUG
#ifdef TIMER_DEBUG
#define timer_debug  debug_printf   
#else  
#define timer_debug(...)                    
#endif 

//��ʱ���궨��
#define APP_TIMER_PRESCALER     0
#define APP_TIMER_OP_QUEUE_SIZE 14

/* ͨ�Ų㶨ʱ�� */
#define TEMP_TIMEOUT_INTERVAL     					APP_TIMER_TICKS(2000,APP_TIMER_PRESCALER)
#define RETURN_ACK_TIMEOUT_INTERVAL     			APP_TIMER_TICKS(500,APP_TIMER_PRESCALER)
#define RX_WINDOW_ON_TIMEOUT_INTERVAL     			APP_TIMER_TICKS(6,APP_TIMER_PRESCALER)
#define RX_WINDOW_OFF_TIMEOUT_INTERVAL     			APP_TIMER_TICKS(50,APP_TIMER_PRESCALER)
#define TX_OVERTIME_TIMEOUT_INTERVAL     			APP_TIMER_TICKS(15,APP_TIMER_PRESCALER) 
#define LCD_UPDATE_TIMEOUT_INTERVAL     			APP_TIMER_TICKS(200,APP_TIMER_PRESCALER) 
#define RETRANSMIT_TIMEOUT_INTERVAL     			APP_TIMER_TICKS(50,APP_TIMER_PRESCALER) 
#define TRANSMIT_RESULT_TIMEOUT_INTERVAL     		APP_TIMER_TICKS(1000,APP_TIMER_PRESCALER) 	
#define POWERDOWN_TIMEOUT_INTERVAL     				APP_TIMER_TICKS(10000,APP_TIMER_PRESCALER) 	
#define BUTTON_TIMEOUT_INTERVAL     				APP_TIMER_TICKS(50,APP_TIMER_PRESCALER) 	
#define REQUEST_DATA_TIMEOUT_INTERVAL     			APP_TIMER_TICKS(500,APP_TIMER_PRESCALER) 	
#define SYSTEM_OFF_TIMEOUT_INTERVAL     			APP_TIMER_TICKS(1000,APP_TIMER_PRESCALER) 	
#define WAIT_DATA_TIMEOUT_INTERVAL     				APP_TIMER_TICKS(1000,APP_TIMER_PRESCALER) 	
#define POWER_ON_TIMEOUT_INTERVAL     				APP_TIMER_TICKS(2000,APP_TIMER_PRESCALER) 	


/*-------------------------------- Local Variable ----------------------------------------*/
APP_TIMER_DEF(temp_timer_id);					/* ������ */
APP_TIMER_DEF(return_ack_timer_id); 			/* �����ʱ��ACK */
APP_TIMER_DEF(rx_window_timer_id);				/* RXģʽʱ�䴰���� */
APP_TIMER_DEF(tx_overtime_timer_id);			/* ���ͳ�ʱ */
APP_TIMER_DEF(lcd_update_timer_id);				/* LCD��ʱˢ�� */
APP_TIMER_DEF(retransmit_timer_id);				/* �ط���ʱ */
APP_TIMER_DEF(transmit_result_timer_id);		/* LCD��ʾ���ͽ��ʱ�� */
APP_TIMER_DEF(powerdown_timer_id);				/* ����״̬ */
APP_TIMER_DEF(button_timer_id);					/* �������� */
APP_TIMER_DEF(request_data_timer_id);			/* ����/���Ѻ��������� */
APP_TIMER_DEF(system_off_timer_id);			    /* 45min�ػ���ʱ�� */
APP_TIMER_DEF(wait_data_timer_id);			    /* �����յ���ǰ��֡����ţ�ȷ��RX����ʱ�� */
APP_TIMER_DEF(power_on_timer_id);			    /* ����������ʱ�� */


timer_struct_t				timer_struct;
									   
/*-------------------------------- Gocal Variable ----------------------------------------*/
timer_struct_t				*TIMER = &timer_struct;
					   
/*-------------------------------- Function Declaration ----------------------------------*/
static void temp_timer_start(void);
static void temp_timer_stop(void);
static void temp_timer_handler(void * p_context);

static void return_ack_timer_start(void);
static void return_ack_timer_stop(void);
static void return_ack_timer_handler(void * p_context);
static void m_return_ack_timer_handler(void);

static void rx_window_timer_start(void);
static void rx_window_timer_stop(void);
static void rx_window_timer_add(uint32_t time_ms);
static void rx_window_timer_handler(void * p_context);

static void tx_overtime_timer_start(void);
static void tx_overtime_timer_stop(void);
static void tx_overtime_timer_handler(void * p_context);

static void lcd_update_timer_start(void);
static void lcd_update_timer_stop(void);
static void lcd_update_timer_handler(void * p_context);

static void retransmit_timer_start(void);
static void retransmit_timer_stop(void);
static void retransmit_timer_handler(void * p_context);
static void m_retransmit_timer_handler(void);

static void transmit_result_timer_start(void);
static void transmit_result_timer_stop(void);
static void transmit_result_timer_handler(void * p_context);

static void powerdown_timer_start(void);
static void powerdown_timer_stop(void);
static void powerdown_timer_handler(void * p_context);

static void button_timer_start(void);
static void button_timer_stop(void);
static void button_timer_handler(void * p_context);

static void request_data_timer_start(void);
static void request_data_timer_stop(void);
static void request_data_timer_handler(void * p_context);

static void system_off_timer_start(void);
static void system_off_timer_stop(void);
static void system_off_timer_handler(void * p_context);

static void wait_data_timer_start(uint8_t time_ms);
static void wait_data_timer_stop(void);
static void wait_data_timer_handler(void * p_context);

static void power_on_timer_start(void);
static void power_on_timer_stop(void);
static void power_on_timer_handler(void * p_context);


static void m_timer_event_handler(void);
/*-------------------------------- Revision record ---------------------------------------*/

void TIMER_init(void)
{
	uint32_t err_code;
	
	/* ��ʱ������ */
	APP_TIMER_INIT(APP_TIMER_PRESCALER, APP_TIMER_OP_QUEUE_SIZE, false);
	err_code = app_timer_create(&temp_timer_id,APP_TIMER_MODE_REPEATED,temp_timer_handler);
	APP_ERROR_CHECK(err_code);
	err_code = app_timer_create(&return_ack_timer_id,APP_TIMER_MODE_REPEATED,return_ack_timer_handler);
	APP_ERROR_CHECK(err_code);
	err_code = app_timer_create(&rx_window_timer_id,APP_TIMER_MODE_REPEATED,rx_window_timer_handler);
	APP_ERROR_CHECK(err_code);
	err_code = app_timer_create(&tx_overtime_timer_id,APP_TIMER_MODE_REPEATED,tx_overtime_timer_handler);
	APP_ERROR_CHECK(err_code);
	err_code = app_timer_create(&lcd_update_timer_id,APP_TIMER_MODE_REPEATED,lcd_update_timer_handler);
	APP_ERROR_CHECK(err_code);
	err_code = app_timer_create(&retransmit_timer_id,APP_TIMER_MODE_REPEATED,retransmit_timer_handler);
	APP_ERROR_CHECK(err_code);
	err_code = app_timer_create(&transmit_result_timer_id,APP_TIMER_MODE_REPEATED,transmit_result_timer_handler);
	APP_ERROR_CHECK(err_code);
	err_code = app_timer_create(&powerdown_timer_id,APP_TIMER_MODE_REPEATED,powerdown_timer_handler);
	APP_ERROR_CHECK(err_code);
	err_code = app_timer_create(&button_timer_id,APP_TIMER_MODE_REPEATED,button_timer_handler);
	APP_ERROR_CHECK(err_code);
	err_code = app_timer_create(&request_data_timer_id,APP_TIMER_MODE_REPEATED,request_data_timer_handler);
	APP_ERROR_CHECK(err_code);
	err_code = app_timer_create(&system_off_timer_id,APP_TIMER_MODE_REPEATED,system_off_timer_handler);
	APP_ERROR_CHECK(err_code);
	err_code = app_timer_create(&wait_data_timer_id,APP_TIMER_MODE_REPEATED,wait_data_timer_handler);
	APP_ERROR_CHECK(err_code);
	err_code = app_timer_create(&power_on_timer_id,APP_TIMER_MODE_SINGLE_SHOT,power_on_timer_handler);	//��һ��
	APP_ERROR_CHECK(err_code);
	
	
	/* TIMER struct init */
	TIMER->event_flg 			= 0x00;
	TIMER->event_handler		= m_timer_event_handler;
	
	TIMER->tmp_start			= temp_timer_start;
	TIMER->tmp_stop				= temp_timer_stop;
	
	TIMER->return_ack_start		= return_ack_timer_start;
	TIMER->return_ack_stop		= return_ack_timer_stop;
	TIMER->rx_window_start		= rx_window_timer_start;
	TIMER->rx_window_stop		= rx_window_timer_stop;
	TIMER->rx_window_add		= rx_window_timer_add;
	
	TIMER->tx_overtime_start	= tx_overtime_timer_start;
	TIMER->tx_overtime_stop		= tx_overtime_timer_stop;
	
	TIMER->lcd_update_start		= lcd_update_timer_start;
	TIMER->lcd_update_stop		= lcd_update_timer_stop;
	TIMER->lcd_update_handler	= lcd_update_timer_handler;
	
	TIMER->retransmit_start		= retransmit_timer_start;
	TIMER->retransmit_stop		= retransmit_timer_stop;
	TIMER->transmit_result_start= transmit_result_timer_start;
	TIMER->transmit_result_stop	= transmit_result_timer_stop;
	
	TIMER->powerdown_start		= powerdown_timer_start;
	TIMER->powerdown_stop		= powerdown_timer_stop;
	
	TIMER->button_start			= button_timer_start;
	TIMER->button_stop			= button_timer_stop;
	
	TIMER->request_data_start	= request_data_timer_start;
	TIMER->request_data_stop	= request_data_timer_stop;
	
	TIMER->system_off_start		= system_off_timer_start;
	TIMER->system_off_stop		= system_off_timer_stop;
	
	TIMER->wait_data_start		= wait_data_timer_start;
	TIMER->wait_data_stop		= wait_data_timer_stop;
	
	TIMER->power_on_start		= power_on_timer_start;
	TIMER->power_on_stop		= power_on_timer_stop;
}



/******************************************************************************
  @����:temp_timer_id
  @����:
  @����:
  @���:
  @����:
******************************************************************************/
void temp_timer_start(void)
{
	uint32_t err_code;
	
	err_code = app_timer_start(temp_timer_id,TEMP_TIMEOUT_INTERVAL,NULL);
//	err_code = app_timer_start(temp_timer_id,6554,NULL);	//�ֶ������200ms��ʱ����������
	APP_ERROR_CHECK(err_code);
}


void temp_timer_stop(void)
{
	uint32_t err_code;
	err_code = app_timer_stop(temp_timer_id);
	APP_ERROR_CHECK(err_code);
}


void temp_timer_handler(void * p_context)
{
	TIMER->tmp_stop();
}



/******************************************************************************
  @����:return_ack_timer_id
  @����:
  @����:
  @���:
  @����:
******************************************************************************/
void return_ack_timer_start(void)
{
	uint32_t err_code;
	uint8_t  random_num1,random_num2,random_num3,random_num4,random_num5;
	uint8_t  random_num6,random_num7,random_num8;
	uint32_t random_delay = 0;
	uint32_t random_value = 0;
	
	random_num1 = get_random_number();			//�����ʱ600ms
	random_num2 = get_random_number();
	random_num3 = get_random_number();
	random_num4 = get_random_number();
	random_num5 = get_random_number();
	random_num6 = get_random_number();
	random_num7 = get_random_number();
	random_num8 = get_random_number();
	
	random_value = random_num1 + random_num2 + random_num3 + random_num4 + random_num5 + random_num6 
			+ random_num7 + random_num8;
	random_delay = random_value * 5;	
	
	//systic 1tick = 30.517us 
	//�ٷ�Ҫ��tick��СֵΪ5
	
	err_code = app_timer_start(return_ack_timer_id,random_delay,NULL);

	APP_ERROR_CHECK(err_code);
}


void return_ack_timer_stop(void)
{
	uint32_t err_code;
	err_code = app_timer_stop(return_ack_timer_id);
	APP_ERROR_CHECK(err_code);
}


void return_ack_timer_handler(void * p_context)
{
	TIMER->event_flg |= RETURN_ACK_TIMEER_EVENT;
	TIMER->return_ack_stop();
}

void m_return_ack_timer_handler(void)
{
//	CLOCK->HFCLK_start();
	TIMER->rx_window_stop();	 //��������ʱ��RX���ڶ�ʱ���ر�
	TIMER->tx_overtime_start();  //���ͳ�ʱ��ʱ��
	my_esb_mode_change(NRF_ESB_MODE_PTX);
	TRANSPORT.sequence_re_tx_num = 1;

	memcpy(tx_payload.data,TRANSPORT.ack,NRF_ACK_PROTOCOL_LEN);
	tx_payload.length = NRF_ACK_PROTOCOL_LEN;
	tx_payload.pipe = NRF_PIPE;
	tx_payload.noack = true;
	nrf_esb_flush_tx();
	nrf_esb_write_payload(&tx_payload);	
}


/******************************************************************************
  @����:rx_window_timer_id
  @����:
  @����:
  @���:
  @����:
******************************************************************************/
void rx_window_timer_start(void)
{
	uint32_t err_code;
	
	if(RADIO->rx_window_on_flg)	//��ǰRX�򿪣���ر�
	{
//		CLOCK->HFCLK_stop();   //RX�ر�ʱ���ص��ⲿʱ�ӣ�������
		nrf_esb_stop_rx();	
//		err_code = app_timer_start(rx_window_timer_id,RX_WINDOW_OFF_TIMEOUT_INTERVAL,NULL);
		err_code = app_timer_start(rx_window_timer_id,APP_TIMER_TICKS(RADIO->rx_window_off,APP_TIMER_PRESCALER),NULL);
		RADIO->rx_window_on_flg = false;	
		RADIO->rx_window_add_flg = false;			
	}
	else
	{
//		CLOCK->HFCLK_start();   //RX��ʱ�����ⲿʱ�ӣ�����radio���費����
		my_esb_mode_change(NRF_ESB_MODE_PRX);
		nrf_esb_start_rx();	
//		err_code = app_timer_start(rx_window_timer_id,RX_WINDOW_ON_TIMEOUT_INTERVAL,NULL);
		err_code = app_timer_start(rx_window_timer_id,APP_TIMER_TICKS(RADIO->rx_window_on,APP_TIMER_PRESCALER),NULL);
		RADIO->rx_window_on_flg = true;
		RADIO->rx_window_add_flg = false;
	}
	APP_ERROR_CHECK(err_code);
}

//����ʱ�䴰
void rx_window_timer_add(uint32_t time_ms)
{
	uint32_t err_code;
	
//	CLOCK->HFCLK_start();   	//RX��ʱ�����ⲿʱ�ӣ�����radio���費����
	
	my_esb_mode_change(NRF_ESB_MODE_PRX);
	nrf_esb_start_rx();	
	
	TIMER->rx_window_stop();
	err_code = app_timer_start(rx_window_timer_id,APP_TIMER_TICKS(time_ms,APP_TIMER_PRESCALER),NULL);
	APP_ERROR_CHECK(err_code);
	
	RADIO->rx_window_on_flg = true;
	RADIO->rx_window_add_flg = true;
}


void rx_window_timer_stop(void)
{

	uint32_t err_code;
	err_code = app_timer_stop(rx_window_timer_id);
	APP_ERROR_CHECK(err_code);
}

void rx_window_timer_handler(void * p_context)
{
	RADIO->rx_window_wait_data = false;
	RADIO->rx_window_add_flg = false;
	TIMER->rx_window_stop();
	TIMER->rx_window_start();
}


/******************************************************************************
  @����:tx_overtime_timer_id
  @����:
  @����:
  @���:
  @����:��/�ⲿ
******************************************************************************/
void tx_overtime_timer_start(void)
{
	uint32_t err_code;
	err_code = app_timer_start(tx_overtime_timer_id,TX_OVERTIME_TIMEOUT_INTERVAL,NULL);
	APP_ERROR_CHECK(err_code);
}


void tx_overtime_timer_stop(void)
{
	uint32_t err_code;
	err_code = app_timer_stop(tx_overtime_timer_id);
	APP_ERROR_CHECK(err_code);
}

void tx_overtime_timer_handler(void * p_context)
{	
	TIMER->tx_overtime_stop();
	
	/* ������ͳ�ʱ��ת��RXģʽ�������¿���RX��ʱ�� */
	if(m_config_local.mode != NRF_ESB_MODE_PRX)
	{

		RADIO->rx_window_on_flg = false;
		RADIO->rx_window_add_flg = false;
		TIMER->rx_window_start();
	}
}

/******************************************************************************
  @����:lcd_update_timer_id
  @����:
  @����:
  @���:
  @����:��/�ⲿ
******************************************************************************/
void lcd_update_timer_start(void)
{
	uint32_t err_code;
	err_code = app_timer_start(lcd_update_timer_id,LCD_UPDATE_TIMEOUT_INTERVAL,NULL);
	APP_ERROR_CHECK(err_code);
}

void lcd_update_timer_stop(void)
{
	uint32_t err_code;
	err_code = app_timer_stop(lcd_update_timer_id);
	APP_ERROR_CHECK(err_code);
}

void lcd_update_timer_handler(void * p_context)
{
	static uint8_t rtc_timer = 0;		//RTC��LCD���ö�ʱ��
	static uint16_t adc_timer = 0;
	LCD->update_flg  = true;
	if(++rtc_timer == 10)
	{
		RTC->update_flg = true;
		rtc_timer = 0;
	}
	
	//��һ�βɼ�ʱ���ӿ�ɼ��ٶ�
	if(ADC->sample_count)
	{
		if(++adc_timer >= 10)
		{
			ADC->update_flg = true;
			adc_timer = 0;
		}
	}
	else
	{
		ADC->update_flg = true;
	}

}

/******************************************************************************
  @����:retransmit_timer_id
  @����:
  @����:
  @���:
  @����:��/�ⲿ
******************************************************************************/
void retransmit_timer_start(void)
{
	uint32_t err_code;
	err_code = app_timer_start(retransmit_timer_id,RETRANSMIT_TIMEOUT_INTERVAL,NULL);
	APP_ERROR_CHECK(err_code);
}

void retransmit_timer_stop(void)
{
	uint32_t err_code;
	err_code = app_timer_stop(retransmit_timer_id);
	APP_ERROR_CHECK(err_code);
}

void retransmit_timer_handler(void * p_context)
{	
	TIMER->event_flg |= RETRANSMIT_TIMEER_EVENT;
}

void m_retransmit_timer_handler(void)
{
	TRANSPORT.retransmit_num++;

	if( true == TRANSPORT.tx_ok_flag ) 					//�յ���ЧACK,���ͳɹ�
	{
		TRANSPORT.tx_ing_flag = false;
		TRANSPORT.retransmit_num = 0;					
		TIMER->retransmit_stop();						
		RADIO->tx_success();					
		timer_debug("[timer]transmit succeed \r\n");
	}
	else if(TRANSPORT.retransmit_num > NRF_MAX_NUMBER_OF_RETRANSMITS )	//�ﵽ����ط�����������ʧ��
	{	
		TRANSPORT.tx_ing_flag = false;
		TRANSPORT.retransmit_num = 0;		
		TIMER->retransmit_stop();
		RADIO->tx_failed();
		timer_debug("[timer]transmit failure \r\n");
	}
	else
	{
		
		TRANSPORT.data[9]++;		//�ط����Ų��䣬֡��+1
		TRANSPORT.data[NRF_ACK_PROTOCOL_LEN + rf_var.tx_len - 2] = XOR_Cal(TRANSPORT.data+1,TRANSPORT.data_len-3);
		nrf_start_transmit();
	}
}

/******************************************************************************
  @����:transmit_result_timer_id
  @����:
  @����:
  @���:
  @����:��/�ⲿ
******************************************************************************/
void transmit_result_timer_start(void)
{
	uint32_t err_code;
	
	err_code = app_timer_start(transmit_result_timer_id,TRANSMIT_RESULT_TIMEOUT_INTERVAL,NULL);
	APP_ERROR_CHECK(err_code);
}


void transmit_result_timer_stop(void)
{
	uint32_t err_code;
	err_code = app_timer_stop(transmit_result_timer_id);
	APP_ERROR_CHECK(err_code);
}

void transmit_result_timer_handler(void * p_context)
{
	transmit_result_timer_stop();
	LCD->send_result(SEND_RESULT_CLEAR);
	APP->key_send_allow_flg = true;
}


/******************************************************************************
  @����:powerdown_timer_start
  @����:
  @����:
  @���:
  @����:��/�ⲿ
******************************************************************************/
void powerdown_timer_start(void)
{
	uint32_t err_code;
	
	err_code = app_timer_start(powerdown_timer_id,POWERDOWN_TIMEOUT_INTERVAL,NULL);
	APP_ERROR_CHECK(err_code);
}


void powerdown_timer_stop(void)
{
	uint32_t err_code;
	err_code = app_timer_stop(powerdown_timer_id);
	APP_ERROR_CHECK(err_code);
}


void powerdown_timer_handler(void * p_context)
{
//	tmp_debug("powerdown_timer_handler \r\n");
	powerdown_timer_stop();
	POWER->sleep();
}



/******************************************************************************
  @����:temp_timer_id
  @����:
  @����:
  @���:
  @����:
******************************************************************************/
void button_timer_start(void)
{
	uint32_t err_code;
	
	err_code = app_timer_start(button_timer_id,BUTTON_TIMEOUT_INTERVAL,NULL);
	APP_ERROR_CHECK(err_code);
}


void button_timer_stop(void)
{
	uint32_t err_code;
	err_code = app_timer_stop(button_timer_id);
	APP_ERROR_CHECK(err_code);
}


void button_timer_handler(void * p_context)
{
	KEY->run_flg = true;
	//button_timer_stop();
}



void request_data_timer_start(void)
{
	uint32_t err_code;
	
	err_code = app_timer_start(request_data_timer_id,REQUEST_DATA_TIMEOUT_INTERVAL,NULL);
	APP_ERROR_CHECK(err_code);
}


void request_data_timer_stop(void)
{
	uint32_t err_code;
	err_code = app_timer_stop(request_data_timer_id);
	APP_ERROR_CHECK(err_code);
}

//��������ʱ�����������������������
void request_data_timer_handler(void * p_context)
{
	if(POWER->request_data_flg)		//����������ظ���������Ϣ��ֹͣ��������ʱ��
	{
		TIMER->request_data_stop();
//		LCD->wakeup();	
	}
	else
	{
		POWER->request_data_num++;
		if(5 == POWER->request_data_num)	//Ϊ��ʡ���ģ�����5���������ղ�������������
		{
			TIMER->request_data_stop();
//			LCD->wakeup();	
		}
		else
		{
			if(SYS_ON == POWER->request_state)
				POWER->send_state(SYS_ON);			
			else if(SYS_SLEEP == POWER->request_state)
				POWER->send_state(SYS_SLEEP);
		}
	}
}


void system_off_timer_start(void)
{
	uint32_t err_code;
	
	err_code = app_timer_start(system_off_timer_id,SYSTEM_OFF_TIMEOUT_INTERVAL,NULL);
	APP_ERROR_CHECK(err_code);
}


void system_off_timer_stop(void)
{
	uint32_t err_code;
	err_code = app_timer_stop(system_off_timer_id);
	APP_ERROR_CHECK(err_code);
}


void system_off_timer_handler(void * p_context)
{
	if(POWER->sys_off_count >= 2700)	//60*45min
		POWER->off();					//�ػ�
	else
		POWER->sys_off_count++;
}


void wait_data_timer_start(uint8_t time_ms)
{
	uint32_t err_code;
	
	err_code = app_timer_start(wait_data_timer_id,APP_TIMER_TICKS(time_ms,APP_TIMER_PRESCALER),NULL);
	APP_ERROR_CHECK(err_code);
}


void wait_data_timer_stop(void)
{
	uint32_t err_code;
	err_code = app_timer_stop(wait_data_timer_id);
	APP_ERROR_CHECK(err_code);
}


void wait_data_timer_handler(void * p_context)
{
//	tmp_debug("wait_data_timer_handler \r\n");
	wait_data_timer_stop();							//ֹͣ��ʱ��
	TIMER->rx_window_add(RADIO->rx_window_add);		//����RX���ڣ��ȴ���Ч����
}

void power_on_timer_start(void)
{
	uint32_t err_code;
	
	err_code = app_timer_start(power_on_timer_id,POWER_ON_TIMEOUT_INTERVAL,NULL);
	APP_ERROR_CHECK(err_code);
}


void power_on_timer_stop(void)
{
	uint32_t err_code;
	err_code = app_timer_stop(power_on_timer_id);
	APP_ERROR_CHECK(err_code);
}


void power_on_timer_handler(void * p_context)
{
	
}


void m_timer_event_handler(void)
{
	if(TIMER->event_flg)
	{
		if(TIMER->event_flg & RETURN_ACK_TIMEER_EVENT)
		{
			TIMER->event_flg &= ~RETURN_ACK_TIMEER_EVENT;
			m_return_ack_timer_handler();
		}
		if(TIMER->event_flg & RETRANSMIT_TIMEER_EVENT)
		{
			TIMER->event_flg &= ~RETRANSMIT_TIMEER_EVENT;
			m_retransmit_timer_handler();
		}
	}
}














