/**
  ******************************************************************************
  * @file   	delay.c
  * @author  	sam.wu
  * @version 	V1.0.0.0
  * @date   	2016.8.27
  * @brief   	delay function
  ******************************************************************************
  */
  
#include "delay.h"

/* Private variables ---------------------------------------------------------*/
static __IO uint32_t TimingDelay;

/* Private function prototypes -----------------------------------------------*/
void Delay(__IO uint32_t nTime);

/*******************************************************************************
  * @brief  Init system clock
  * @param  None
  * @retval None
*******************************************************************************/
void SysClockInit(void)
{
	SystemInit();
	SystemCoreClockUpdate();
	
  if(SysTick_Config(SystemCoreClock / 1000))
  { 
    /* Capture error */ 
    while (1);
  }		
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	RCC_ClockSecuritySystemCmd(ENABLE);
	
	
}

/*******************************************************************************
  * @brief  Inserts a delay time.
  * @param  nTime: specifies the delay time length, in milliseconds.
  * @retval None
*******************************************************************************/
void DelayMs(__IO uint32_t nTime)
{ 
  TimingDelay = nTime;

  while(TimingDelay != 0);
}

/*******************************************************************************
  * @brief  Decrements the TimingDelay variable.
  * @param  None
  * @retval None
*******************************************************************************/
void TimingDelay_Decrement(void)
{
  if (TimingDelay != 0x00)
  { 
    TimingDelay--;
  }
}


#ifdef ENABLE_WATCHDOG	
/*******************************************************************************
  * @brief  watchdog init.
  * @param  None
  * @retval None
*******************************************************************************/
void IWDG_Configuration(void)
{
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);	/* д��0x5555,�����������Ĵ���д�빦�� */
	IWDG_SetPrescaler(IWDG_Prescaler_256);				/* ����ʱ�ӷ�Ƶ,40K/256=156HZ(6.4ms)*/
	IWDG_SetReload(312);													/* ι��ʱ�� 2s/6.4MS=312 .ע�ⲻ�ܴ���0xfff*/
	IWDG_ReloadCounter();													/* ι��*/
	IWDG_Enable();																/* ʹ��*/
}
#endif //ENABLE_WATCHDOG

/*******************************************************************************
  * @brief  delay 2us 
  * @param  None
  * @retval None
*******************************************************************************/
void Delay2us(uint32_t times)
{
	uint16_t i,j;
	for(i = 0; i < times; i++)
	{
		for(j = 0;j < 12;j++)
		{
			__nop();
		}	
	}
}

/*******************************************************************************
  * @brief  delay 3us 
  * @param  None
  * @retval None
*******************************************************************************/
void Delay3us(void)
{
	uint16_t i;
	for(i = 0;i < 17;i++)
	{
		__nop();
	}
}

/*******************************************************************************
  * @brief  delay 10us 
  * @param  None
  * @retval None
*******************************************************************************/
void Delay10us(uint16_t times)
{
	uint16_t i,j;
	for(i = 0; i < times; i++)
	{
		for(j = 0;j < 70;j++)
		{
			__nop();
		}		
	}	
}

/**************************************END OF FILE****************************/

