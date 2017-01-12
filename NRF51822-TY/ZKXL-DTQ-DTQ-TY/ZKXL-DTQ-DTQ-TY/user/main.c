#include "define.h"

static void debug_fun(void);

int main (void)
{
//	nrf_delay_ms(10);						//�ϵ���ʱ	
	GPIO_default();
								
	CLOCK_init();
//	UART_init();							//UART����ʼ���ܽ�����
	debug_logic_init();
	TIMER_init();
	LCD_init();  
	NFC_init();								//NRF�����KEY_init()ǰ�棬��Ϊ�����жϽ���KEY_init()��������
	KEY_init();
	RTC_Init();
	ADC_init();
	RADIO_init();
	POWER_init();
	PARAMETER_init();
	APP_init();								// APP_initҪ����NFC��,��ΪNFC��ȡUIDҪ��APP_init��ʹ�� 		
		
	POWER->on();							//����������Ҫ����APP_init��
	
	TIMER->rx_window_start();
	LCD->timer_start();
//	LCD->battery(BATTERY_LEVEL_3);						//������ʾ�������
	POWER->sys_state = SYS_ON;
	POWER->request_state = SYS_ON;
	POWER->send_state(SYS_ON);				
	TIMER->request_data_start();			//������������			
	TIMER->powerdown_start();				//30S�ް�����������״̬
	
	LCD->student_id(STUDENT_ID);			//��ʾ������������ţ�����ʹ��
	while(true)
	{
//		debug_fun();       					//�����ú���
		switch (POWER->sys_state)
        {
        	case SYS_ON:  
				if(POWER->state_change_flg)
				{
					POWER->state_change_flg = false;
					CLOCK->HFCLK_start();
					RADIO->wakeup();
					LCD->wakeup();							//LCD->wakeup������(����)�������ݺ���ã���������һ��ָ����һ���ػ��������LCD��һС�������������Ӿ�
					POWER->request_state = SYS_SLEEP;
					POWER->send_state(SYS_SLEEP);			//���ͻ�������	
					POWER->request_data_flg = false;	
					POWER->request_data_num = 0;			//�������ݼ�������Ϊ0
					TIMER->request_data_start();			//��������ʱ��
					TIMER->powerdown_start();				//30S�ް�����������״̬
					TIMER->system_off_stop();				//������ʱ�Զ��ػ���ʱ���ر�
				}
				TIMER->event_handler();				
				RADIO->rx_data();				
				APP->command_handler();			
				APP->button_handler();
//				RTC->update();							//δ���ڴ���״̬�Ż���ʾRTC			
				ADC->update();
				LCD->update();	
				break;
        	case SYS_SLEEP: 
				if(POWER->state_change_flg)
				{
					POWER->state_change_flg = false;
					CLOCK->HFCLK_stop();			
					LCD->sleep();			
					RADIO->sleep();
					POWER->sys_off_count = 0;
					TIMER->system_off_start();		//������ʱ�Զ��ػ���ʱ������
				}
				TIMER->event_handler();
				APP->button_handler();
        		break;
        	default:
        		break;
        }
		__WFE();							//POWER DOWN��wait for EVENT	.
		__WFI();
	}
}

static void debug_fun(void)
{
	logic_high();
	nrf_delay_ms(20);
	logic_low();
	nrf_delay_ms(20);
}










