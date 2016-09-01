/**
  ******************************************************************************
  * @file    GPIO/IOToggle/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.1.2
  * @date    09/28/2009
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and peripherals
  *          interrupt service routine.
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2009 STMicroelectronics</center></h2>
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "delay.h"
#include "nrf.h"
extern nrf_communication_t			nrf_communication;
extern uint8_t 					dtq_to_jsq_sequence;
extern uint8_t 					jsq_to_dtq_sequence;
/** @addtogroup STM32F10x_StdPeriph_Examples
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/
#if defined(__CC_ARM) /*------------------RealView Compiler -----------------*/
__asm void GenerateSystemReset(void)
{
//	MOV R0, #1         	//; 
//	MSR FAULTMASK, R0  	//; ���FAULTMASK ��ֹһ���жϲ���
//	LDR R0, =0xE000ED0C //;
//	LDR R1, =0x05Fa0004 //; 
//	STR R1, [R0]       	//; ϵͳ�����λ   
//deadloop
//    B deadloop        //; ��ѭ��ʹ�������в�������Ĵ���
}
#elif (defined(__ICCARM__)) /*------------------ ICC Compiler -------------------*/
//#pragma diag_suppress=Pe940
void GenerateSystemReset(void)
{
	__ASM("MOV R0, #1");
	__ASM("MSR FAULTMASK, R0");
	SCB->AIRCR = 0x05FA0004;
	for (;;);
}
#endif

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
	while(1)
	{
		GenerateSystemReset();
	}
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
		GenerateSystemReset();
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
		GenerateSystemReset();
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
		GenerateSystemReset();
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSV_Handler exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
	TimingDelay_Decrement();
	
	if(uart232_var.flag_uart_rxing)												//���ڽ��ճ�ʱ������
	{
		uart232_var.uart_rx_timeout++;
		if(uart232_var.uart_rx_timeout>5)										//5msc��ʱ�����¿�ʼ����
		{
			hal_uart_clr_rx();
			uart232_var.uart_status = UartHEADER;
		}
	}
	
	if(timer_1ms++ > 1000)
	{
#ifdef ENABLE_WATCHDOG		
		IWDG_ReloadCounter();													//��ʱι��
#endif //ENABLE_WATCHDOG		
		timer_1ms = 0;
		time.second++;
		if(time.second >= 60)
		{
			time.second = 0;
			time.minute++;
			if(time.minute >= 60)
			{
				time.minute = 0;
				time.hour++;
				if(time.hour >= 24)
				{
					time.hour = 0;
				}
			}
		}
//		printf("[RTC]: RTC time - %02d:%02d:%02d\r\n", time.hour, time.minute, time.second);
//		SPI_Init_NRF2();
//		nrf24InitConf_NRF2();
//		ledToggle(LGREEN);
//		printf("nrf2 reset\r\n");
	}
	
	if(time_for_buzzer_on > 1)									//������������ʱʱ��
	{
		time_for_buzzer_on--;
	}
	
	if((time_for_buzzer_on == 0)&&(time_for_buzzer_off > 0))	//����������ʱ
	{
		time_for_buzzer_off--;
	}

	if(delay_nms)
	{
		delay_nms --;
	}
}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles External lines 15 to 10 interrupt request.
  * @param  None
  * @retval None
  */
void USART1pos_IRQHandler(void)
{
	if(USART_GetITStatus(USART1pos, USART_IT_PE) != RESET)
	{
		// Enable SC_USART RXNE Interrupt (until receiving the corrupted byte) 
		USART_ITConfig(USART1pos, USART_IT_RXNE, ENABLE);
		// Flush the SC_USART DR register 
		USART_ReceiveData(USART1pos);
	}
  
	if(USART_GetITStatus(USART1pos, USART_IT_RXNE) != RESET)
	{
	  uart232_var.uart_temp = USART_ReceiveData(USART1pos);

		switch(uart232_var.uart_status)
		{
			case UartOK:									//��ʼ����
				if(uart232_var.flag_uart_rx_ok || uart232_var.flag_txing[uart_tx_i])
				{	break;	}	 							//����ǰ����û��ȡ�߻�û�з�����ɣ���������ȡ����
			case UartHEADER:
				if(UART_SOF == uart232_var.uart_temp)		//�������ͷΪ0x5C��ʼ���գ����򲻽���							
				{
					uart232_var.flag_uart_rxing = 1;
					uart232_var.HEADER = uart232_var.uart_temp;
					uart232_var.uart_rx_buf[uart232_var.uart_rx_cnt++] = uart232_var.uart_temp;
					uart232_var.uart_rx_timeout = 0;			
					uart232_var.uart_status =  UartTYPE;
				}
				break;
			case UartTYPE:
				uart232_var.TYPE = uart232_var.uart_temp;
				uart232_var.uart_rx_buf[uart232_var.uart_rx_cnt++] = uart232_var.uart_temp;
				uart232_var.uart_rx_timeout = 0;
				uart232_var.uart_status = UartSIGN; 
				break;	
			case UartSIGN:
				uart232_var.SIGN[uart232_var.temp_sign_len++] = uart232_var.uart_temp;
				uart232_var.uart_rx_buf[uart232_var.uart_rx_cnt++] = uart232_var.uart_temp;
			    uart232_var.uart_rx_timeout = 0;
			    if(uart232_var.temp_sign_len==4)
				 { 
				   uart232_var.uart_status = UartLEN;
				 }
				 memcpy(&sign_buffer[0],&uart232_var.SIGN[0] , 4);
				break;
			case UartLEN:
				uart232_var.LEN = uart232_var.uart_temp;
				uart232_var.uart_rx_buf[uart232_var.uart_rx_cnt++] = uart232_var.uart_temp;
				if(uart232_var.LEN > UART_NBUF)  					//  �����ݳ��ȴ���32
				{
					uart232_var.flag_uart_rx_length_err = 1;		//  �������ݳ��Ȳ���
					uart232_var.uart_status =  UartHEADER;
				}
				else if(uart232_var.LEN > 0)		   				//  DATA��Ϊ��
				{
					uart232_var.uart_status = UartDATA;
				}
				else												//  DATAΪ��
				{
					uart232_var.uart_status = UartXOR;
				}
				uart232_var.uart_rx_timeout = 0;
				break;
			case UartDATA:
				uart232_var.DATA[uart232_var.temp_data_len++] = uart232_var.uart_temp;
				uart232_var.uart_rx_buf[uart232_var.uart_rx_cnt++] = uart232_var.uart_temp;
				uart232_var.uart_rx_timeout = 0;
				if(uart232_var.temp_data_len == uart232_var.LEN)							//���ݽ������
					uart232_var.uart_status = UartXOR;
				break;
			case UartXOR:
				uart232_var.XOR = uart232_var.uart_temp;
				uart232_var.uart_rx_buf[uart232_var.uart_rx_cnt++] = uart232_var.uart_temp;
				uart232_var.uart_rx_timeout = 0;
				uart232_var.uart_status = UartEND;
				break;
			case UartEND:
				if(UART_EOF == uart232_var.uart_temp)
				{
					uart232_var.END = uart232_var.uart_temp;
					uart232_var.uart_rx_buf[uart232_var.uart_rx_cnt++] = uart232_var.uart_temp;
					if((uart232_var.uart_rx_cnt == (uart232_var.LEN + 9))||(uart232_var.uart_rx_cnt == (uart232_var.LEN + 14)))	//���������ݵ�������ճ��ȣ������ʣ�
					{
						if( uart232_var.XOR != XOR_Cal(&uart232_var.uart_rx_buf[1], uart232_var.uart_rx_cnt - 3))//  xorУ��δͨ��
						{
							uart232_var.flag_uart_rx_xor_err = 1;
							uart232_var.uart_status = UartOK;
						}
						else														    //��У��ͨ�������������OK����
						{
							uart232_var.flag_uart_rx_ok = 1;  //�жϴ��ڽ�����ɱ�־
							uart232_var.uart_status = UartOK;                       
						}
					}
					else															    //���������ݲ���������ճ��ȣ����֮ǰ����buf	
					{
						uart232_var.flag_uart_rx_length_err = 1;
						uart232_var.uart_status = UartOK;
					}
				}
				else
				{
					uart232_var.uart_status = UartHEADER;
					hal_uart_clr_rx();                                               //������н�����صı���
				}
				uart232_var.flag_uart_rxing = 0;                                     //�жϴ������ڽ��ձ�־
				uart232_var.uart_rx_timeout = 0;	                                 //�жϴ��ڽ��ճ�ʱ������
				break;
			default:
				break;
		}
	}
   
	if(USART_GetITStatus(USART1pos, USART_IT_TXE) != RESET)
  	{   
	    // Write one byte to the transmit data register 	    
	    if(uart232_var.uart_tx_length[uart_tx_i])			//���Ȳ�Ϊ0��������
	    {
			USART_SendData(USART1pos, uart232_var.uart_tx_buf[uart_tx_i][uart232_var.uart_tx_cnt++]);
			uart232_var.uart_tx_length[uart_tx_i]--;
	    }
	    else
		{
			uart232_var.flag_tx_ok[uart_tx_i] = true;		//�������
			hal_uart_clr_tx(uart_tx_i);						//��յ�ǰ���ͻ�����
			
			if(uart232_var.flag_txing[0])					//�����һ�黺������Ҫ����
			{
				uart_tx_i = 0;
				USART_SendData(USART1pos, uart232_var.uart_tx_buf[uart_tx_i][uart232_var.uart_tx_cnt++]);
				uart232_var.uart_tx_length[uart_tx_i]--;
			}
			else if(uart232_var.flag_txing[1])				//����ڶ��黺������Ҫ����
			{
				uart_tx_i = 1;
				USART_SendData(USART1pos, uart232_var.uart_tx_buf[uart_tx_i][uart232_var.uart_tx_cnt++]);
				uart232_var.uart_tx_length[uart_tx_i]--;
			}
			else											//������Ҫ����
			{	
				USART_ITConfig(USART1pos,USART_IT_TXE,DISABLE);
			}
		}
	}
}

bool search_uid_in_white_list(uint8_t *g_uid , uint8_t *position);

uint8_t blank_packet[]={0x5A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xCA };			//�յ��������հ�������һ���հ�
uint8_t response[] =   {0x5A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0xCA };		//�յ����������ݰ�����ȷ�ϰ�
uint8_t irq_flag;
uint8_t ack_buff[] = {0xAA,0xBB,0xCC,0xDD,0xEE,0xFF};

void RFIRQ_EXTI_IRQHandler(void)
{
	bool    Is_whitelist_uid = false;
	uint8_t uidpos = 0;
	
	if(EXTI_GetITStatus(EXTI_LINE_RFIRQ) != RESET)
	{
		EXTI_ClearITPendingBit(EXTI_LINE_RFIRQ);
		
		uesb_nrf_get_irq_flags(SPI1, &irq_flag, &nrf_communication.receive_len, nrf_communication.receive_buf);		//��ȡ����
		
		printf("UID = %2x %2x %2x %2x \r\n",
		       *(nrf_communication.receive_buf+1),*(nrf_communication.receive_buf+2),
		       *(nrf_communication.receive_buf+2),*(nrf_communication.receive_buf+3));
		
		//Is_whitelist_uid = search_uid_in_white_list(nrf_communication.receive_buf+1,&uidpos);
		Is_whitelist_uid = true;
		
		if(Is_whitelist_uid)			//������ƥ��
		{		
			if(nrf_communication.receive_buf[5] == NRF_DATA_IS_ACK)						//�յ�����ACK
			{
				if(nrf_communication.receive_buf[0] == jsq_to_dtq_sequence)				//����ACK�İ��ź��ϴη��͵��Ƿ���ͬ
				{
					nrf_communication.transmit_ok_flag = true;
					//irq_debug("irq_debug����ЧACK,������ͬ: %02X  \n",nrf_communication.receive_buf[0]);	
//					for(i = 0; i < nrf_communication.receive_len; i++)
//					{
//						irq_debug("%02X ", nrf_communication.receive_buf[i]);
//					}irq_debug("\r\n");

				}
				else
				{
					//irq_debug("irq_debug����ЧACK,���Ų�ͬ: %02X \n",rf_var.rx_buf[0]);
				}
			}
			else														//�յ�������Ч����
			{
				if(dtq_to_jsq_sequence == nrf_communication.receive_buf[0])	//�ظ����յ����ݣ����ذ��ź��ϴ�һ����ACK
				{
					//irq_debug("irq_debug���ظ��յ�����,����: %02X  \n",nrf_communication.receive_buf[0]);
//					dtq_to_jsq_sequence = nrf_communication.receive_buf[0];			//���½��հ���		
					my_nrf_transmit_start(ack_buff,6,NRF_DATA_IS_ACK);
					
				}
				else													//��Ч���ݣ�����ACK
				{
					//irq_debug("irq_debug���״��յ�����,����: %02X  \n",nrf_communication.receive_buf[0]);
//					for(i = 0; i < nrf_communication.receive_len; i++)
//					{
//						irq_debug("%02X ", nrf_communication.receive_buf[i]);
//					}irq_debug("\r\n");
					rf_var.rx_len = nrf_communication.receive_buf[6];
					memcpy(rf_var.rx_buf, nrf_communication.receive_buf+10, rf_var.rx_len);	//��Ч���ݸ��Ƶ�rf_var.rx_buf
					rf_var.flag_rx_ok = true;
					dtq_to_jsq_sequence = nrf_communication.receive_buf[0];				//���½��հ���
					my_nrf_transmit_start(ack_buff,6,NRF_DATA_IS_ACK);		//�ظ�ACK
					my_nrf_receive_success_handler();						//�û����յ����ݴ�����
				}	
			}
		}
		else														//��������ƥ�䣬�˵�
		{;}
	}
	ledToggle(LBLUE);
}


//���ģ��ACKͨ�ţ�����ʱ��
void TIM3_IRQHandler(void)   //TIM3�ж�
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)  //���TIM3�����жϷ������
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //���TIMx�����жϱ�־ 
		//irq_debug("��ʱ��3�ж�  ");
		
		nrf_communication.number_of_retransmits++;
		if( true == nrf_communication.transmit_ing_flag )		//���ڴ���
		{
			if( true == nrf_communication.transmit_ok_flag ) 	//�յ���ЧACK,���ͳɹ�
			{
				my_nrf_transmit_tx_success_handler();			//�û����ͳɹ�������		
				nrf_communication.transmit_ing_flag = false;	
				nrf_communication.transmit_ok_flag = true;			
				nrf_communication.number_of_retransmits = 0;	
				TIM_Cmd(TIM3, DISABLE); 						//ֹͣ��ʱ��
				//irq_debug("irq_debug:transmit succeed,sequence:	%02X \r\n",jsq_to_dtq_sequence);
			}
			else if( nrf_communication.number_of_retransmits > NRF_MAX_NUMBER_OF_RETRANSMITS )	//�ﵽ����ط�����������ʧ��
			{
				my_nrf_transmit_tx_failed_handler();			//�û�����ʧ�ܴ�����
				nrf_communication.transmit_ing_flag = false;	
				nrf_communication.transmit_ok_flag = false;			
				nrf_communication.number_of_retransmits = 0;	
				TIM_Cmd(TIM3, DISABLE); 						
				//irq_debug("irq_debug:transmit  failure,sequence: %02X \r\n",jsq_to_dtq_sequence);
			}
			else
			{
				uesb_nrf_write_tx_payload(nrf_communication.transmit_buf,nrf_communication.transmit_len);
			}
		}
		else	//��ʱ����һ�η�������							
		{
			nrf_communication.transmit_ing_flag = true;	
			nrf_communication.transmit_ok_flag = false;		
			uesb_nrf_write_tx_payload(nrf_communication.transmit_buf,nrf_communication.transmit_len);
		}
	}
}
/**
  * @}
  */

/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
