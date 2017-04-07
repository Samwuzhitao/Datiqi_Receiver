

#include "my_misc_fun.h"


/******************************************************************************
  @����:XOR_Cal
  @����:
  @����:* dat �������飬���ڼ�λ��ʼ
		length����������Ҫ���ĳ���
  @���:�������Ľ��
  @����:
******************************************************************************/
uint8_t XOR_Cal(uint8_t * dat,uint16_t length)
{
	uint8_t temp_xor;
	uint16_t i;

	temp_xor = *dat;
	for(i = 1;i < length; i++)
	{
		temp_xor = temp_xor ^ *(dat+i);
	}
	return temp_xor;
}

/******************************************************************************
  @����:gpio_default_init
  @����:51822��gpio����ΪĬ��(ʡ����)״̬
  @����:
  @���:
  @����:
******************************************************************************/
void gpio_default_init(void)
{
    uint32_t i = 0;
    for(i = 0; i< 32 ; ++i ) {
        NRF_GPIO->PIN_CNF[i] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos)
                               | (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)
                               | (GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos)
                               | (GPIO_PIN_CNF_INPUT_Disconnect << GPIO_PIN_CNF_INPUT_Pos)
                               | (GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos);
    }
}

/******************************************************************************
  @����:clocks_start
  @����:�����ⲿ16Mʱ��
  @����:
  @���:
  @����:
******************************************************************************/
void clocks_start(void)
{
    NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;
    NRF_CLOCK->TASKS_HFCLKSTART = 1;

    while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0);
	
	
	 /* Start low frequency crystal oscillator for app_timer(used by bsp)*/
    NRF_CLOCK->LFCLKSRC            = (CLOCK_LFCLKSRC_SRC_RC << CLOCK_LFCLKSRC_SRC_Pos);
    NRF_CLOCK->EVENTS_LFCLKSTARTED = 0;
    NRF_CLOCK->TASKS_LFCLKSTART    = 1;

    while (NRF_CLOCK->EVENTS_LFCLKSTARTED == 0)
    {
        // Do nothing.
    }	
	
    NRF_CLOCK->EVENTS_DONE = 0;					//RTCУ׼
    NRF_CLOCK->TASKS_CAL = 1;
//    while (NRF_CLOCK->EVENTS_DONE == 0)
//    {
//        // Do nothing.
//    }
	
}


/******************************************************************************
  @����:get_random_number
  @����:��ȡ0~255֮��������
  @����:
  @���:
  @����:�˺������յ��������·����ݺ������ʱ0`255ms���ٷ������ACK��
		 ���ٶ������ͬʱ�ظ�ACK���������ݳ�ͻ
******************************************************************************/
uint8_t get_random_number(void)
{
    NRF_RNG->TASKS_START = 1; // start the RNG peripheral.

	// Wait until the value ready event is generated.
	while (NRF_RNG->EVENTS_VALRDY == 0)
	{
		// Do nothing.
	}
	NRF_RNG->EVENTS_VALRDY = 0;		 // Clear the VALRDY EVENT.
	
	return (uint8_t)NRF_RNG->VALUE;
}

void parameters_init(void)
{
}


