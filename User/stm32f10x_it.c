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

/* uart global variables */
// revice part
Uart_MessageTypeDef uart_irq_revice_massage;
static uint32_t uart_rx_timeout = 0;
static uint32_t rf_tx_time_cnt = 0;
static uint32_t rf_tx_timeout_cnt = 0;

static bool     flag_uart_rxing = false;
static uint8_t  uart_status     = UartHEADER;

// send part
Uart_MessageTypeDef uart_irq_send_massage;
uint8_t uart_tx_status      = 0;

/* uart global variables */
extern nrf_communication_t	nrf_communication;
extern uint8_t 					    dtq_to_jsq_sequence;
extern uint8_t 			        jsq_to_dtq_sequence;
extern uint8_t 					    dtq_to_jsq_packnum;
extern uint8_t 			        jsq_to_dtq_packnum;
extern uint8_t              sign_buffer[4];

/* rf systick data */
volatile uint8_t rf_systick_status = 0; // 0 = IDLE
/******************************************************************************
  Function:rf_change_systick_status
  Description:
		�޸�systick��״̬
  Input :
		rf_status: systick����״̬
  Output:
  Return:
  Others:None
******************************************************************************/
void rf_change_systick_status(uint8_t rf_status)
{
	rf_systick_status = rf_status;
	DebugLog("<%s> rf_systick_status = %d \r\n",__func__,rf_systick_status);
}

/******************************************************************************
  Function:rf_get_systick_status
  Description:
		��ȡsystick��״̬
  Input :
  Output:systick����״̬
  Return:
  Others:None
******************************************************************************/
uint8_t rf_get_systick_status(void)
{
	return rf_systick_status ;
}
/******************************************************************************
  Function:uart_clear_message
  Description:
		���Message�е���Ϣ
  Input :
		Message: Э��Message��ָ��
  Output:
  Return:
  Others:None
******************************************************************************/
void uart_clear_message( Uart_MessageTypeDef *Message )
{
	uint8_t i;
	uint8_t *pdata = (uint8_t*)(Message);

	for(i=0;i<PACKETSIZE;i++)
	{
		*pdata = 0;
		pdata++;
	}
}

/******************************************************************************
  Function:uart_revice_data_state_mechine
  Description:
		�������ݽ��պ�������ȡ��Ч���ݴ��뻺��
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
void uart_revice_data_state_mechine( uint8_t data )
{
	static uint8_t	uart_rx_cnt     = 0;
	static uint8_t  temp_sign_len   = 0;

	switch(uart_status)
		{
			case UartHEADER:
				{
					if(UART_SOF == data)		//�������ͷΪ0x5C��ʼ���գ����򲻽���
					{
						uart_irq_revice_massage.HEADER = data;
						uart_status =  UartTYPE;
						flag_uart_rxing = true;
					}
				}
				break;

			case UartTYPE:
				{
					uart_irq_revice_massage.TYPE = data;
					uart_status = UartSIGN;
					temp_sign_len = 0;
				}
				break;

			case UartSIGN:
				{
						uart_irq_revice_massage.SIGN[temp_sign_len++] = data;
						if( temp_sign_len == 4 )
						{
								uart_status = UartLEN;
						}
				}
				break;

			case UartLEN:
				{
					uart_irq_revice_massage.LEN = data;

					/*  �����ݳ��ȴ��� 236 */
					if(uart_irq_revice_massage.LEN > UART_NBUF)
					{
						uart_status =  UartHEADER;
						/* ��� uart_irq_revice_massage ������Ϣ */
						uart_clear_message(&uart_irq_revice_massage);
						flag_uart_rxing = false;
					}
					else if(uart_irq_revice_massage.LEN > 0)	//  DATA��Ϊ��
					{
						uart_status = UartDATA;
						uart_rx_cnt = 0;
					}
					else//  DATAΪ��
					{
						uart_status = UartXOR;
					}
				}
				break;

			case UartDATA:
				{
						uart_irq_revice_massage.DATA[uart_rx_cnt++] = data;
						/* ���ݽ������ */
						if(uart_rx_cnt == uart_irq_revice_massage.LEN)
								uart_status = UartXOR;
				}
				break;

			case UartXOR:
				{
						uart_irq_revice_massage.XOR = data;
						uart_status = UartEND;
				}
				break;

			case UartEND:
				{
					if(UART_EOF == data)
					{
						uint8_t UartMessageXor = XOR_Cal((uint8_t *)(&uart_irq_revice_massage.TYPE),
												uart_irq_revice_massage.LEN + 6 );
						uart_irq_revice_massage.END = data;

						if( uart_irq_revice_massage.XOR == UartMessageXor)
						{   /* ��У��ͨ�������������OK���� */
								if(BUFFERFULL != buffer_get_buffer_status(REVICE_RINGBUFFER))
								{
									serial_ringbuffer_write_data(REVICE_RINGBUFFER,&uart_irq_revice_massage);
								}
							  flag_uart_rxing = false;
								uart_status = UartHEADER;
								uart_clear_message(&uart_irq_revice_massage);
						}
						else
						{
							uart_clear_message(&uart_irq_revice_massage);
						}
					}
					else
					{
						uart_status = UartHEADER;
						uart_clear_message(&uart_irq_revice_massage);
						flag_uart_rxing = false;
					}
				}
				break;

			default:
				break;
		}
}

/******************************************************************************
  Function:uart_send_data_state_machine
  Description:
		���������ͺ������ӻ�������ȡ���ݷ��͵���λ��
  Input :
		status: uart tx status
  Output:
  Return:
  Others:None
******************************************************************************/
void uart_send_data_state_machine( void )
{
	static uint8_t uart_tx_cnt  = 0;
	static uint8_t *pdata;

	switch( uart_tx_status )
	{
		case 0:
			{
					if(BUFFEREMPTY == buffer_get_buffer_status(SEND_RINGBUFFER))
					{
						USART_ITConfig(USART1pos,USART_IT_TXE,DISABLE);
						return;
					}
					else
					{
						serial_ringbuffer_read_data(SEND_RINGBUFFER, &uart_irq_send_massage);
						pdata = (uint8_t *)(&uart_irq_send_massage);
						uart_tx_status = 1;
						uart_tx_cnt = *(pdata+6) + 7;
						uart_tx_status = 1;
					}
			}
			break;

		case 1:
			{
				USART_SendData(USART1pos,*pdata);
				uart_tx_cnt--;
				pdata++;
				if( uart_tx_cnt == 0 )
				{
					pdata = &(uart_irq_send_massage.XOR);
					uart_tx_cnt = 2;
					uart_tx_status = 2;
				}
			}
			break;

		case 2:
			{
				USART_SendData(USART1pos,*pdata);
				uart_tx_cnt--;
				pdata++;
				if( uart_tx_cnt == 0 )
				{
					uart_clear_message(&uart_irq_send_massage);
					uart_tx_status = 0;
				}
			}
			break;

		default:
			break;
	}
}

/******************************************************************************
  Function:App_rf_check_process
  Description:
		App RF ��Ϣ���洦����
  Input :
  Return:
  Others:None
******************************************************************************/
void rf_move_data_to_buffer(nrf_communication_t *Message)
{
	Uart_MessageTypeDef rf_message;
	uint8_t i = 0 ;

	rf_message.HEADER = 0x5C;
	rf_message.TYPE = 0x10;

	memcpy(rf_message.SIGN,nrf_communication.receive_buf+5,4);

	/* ��ȡ��Ϣ����Ч���� */
	rf_message.LEN = Message->receive_buf[14];

	for (i=0;i<rf_message.LEN;i++)
	{
		rf_message.DATA[i]=Message->receive_buf[i+15];
	}

	rf_message.XOR =  XOR_Cal((uint8_t *)(&(rf_message.TYPE)), i+6);
	rf_message.END = 0xCA;

	/* ���뻺�� */
	if(BUFFERFULL != buffer_get_buffer_status(SEND_RINGBUFFER))
	{
		serial_ringbuffer_write_data(SEND_RINGBUFFER,&rf_message);
	}
}

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

	if(get_clicker_send_data_status() == 3)
	{
		clicker_send_data_time_set(1);
		if(clicker_send_data_time_set(2) == 300)
		{
			clicker_send_data_time_set(0);
			change_clicker_send_data_status(4);
		}
	}
	
	if(rf_systick_status == 3)
	{
		rf_tx_time_cnt++;

		/* 5S ���������� ͬʱ���������� */
		if(rf_tx_time_cnt >= 5000)
		{
			rf_change_systick_status(4);
			rf_tx_time_cnt = 0;
		}
	}

	if(rf_systick_status == 1)
	{
		rf_tx_timeout_cnt++;
		/* 1S ��������״̬ͳ�ƣ����㳬ʱ������ */
		if(rf_tx_timeout_cnt >= 1000)
		{
			rf_change_systick_status(2);
			change_clicker_send_data_status(2);
			rf_tx_timeout_cnt = 0;
		}
	}

	if(flag_uart_rxing)												//���ڽ��ճ�ʱ������
	{
		uart_rx_timeout++;
		if(uart_rx_timeout>5)										//5msc��ʱ�����¿�ʼ����
		{
			uart_clear_message(&uart_irq_revice_massage);
			flag_uart_rxing = false;
			uart_status = UartHEADER;
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
	uint8_t uart_temp = 0;

	if(USART_GetITStatus(USART1pos, USART_IT_PE) != RESET)
	{
		// Enable SC_USART RXNE Interrupt (until receiving the corrupted byte)
		USART_ITConfig(USART1pos, USART_IT_RXNE, ENABLE);
		// Flush the SC_USART DR register
		USART_ReceiveData(USART1pos);
	}

	if(USART_GetITStatus(USART1pos, USART_IT_RXNE) != RESET)
	{
	  uart_temp = USART_ReceiveData(USART1pos);

		/* store it to uart_irq_revice_massage */
		uart_revice_data_state_mechine( uart_temp );

		uart_rx_timeout = 0;
	}


	if(USART_GetITStatus(USART1pos, USART_IT_TXE) != RESET)
  {
    uart_send_data_state_machine( );
	}
}

bool search_uid_in_white_list(uint8_t *g_uid , uint8_t *position);

uint8_t irq_flag;

void RFIRQ_EXTI_IRQHandler(void)
{
	bool    Is_whitelist_uid = OPERATION_ERR;
	uint8_t uidpos = 0;

	if(EXTI_GetITStatus(EXTI_LINE_RFIRQ) != RESET)
	{
		EXTI_ClearITPendingBit(EXTI_LINE_RFIRQ);

		uesb_nrf_get_irq_flags(SPI1, &irq_flag, &nrf_communication.receive_len, nrf_communication.receive_buf);		//��ȡ����

		/* ����������������Ƿ�Ϊ������������ */
		Is_whitelist_uid = search_uid_in_white_list(nrf_communication.receive_buf+5,&uidpos);

		if(Is_whitelist_uid == OPERATION_SUCCESS)
		{
			uint8_t systick_current_status = 0, clicker_send_data_current_status = 0;

			/* ��ȡ��ǰ��systick��״̬ */
			systick_current_status = rf_get_systick_status();
			clicker_send_data_current_status = get_clicker_send_data_status();
			
			if(systick_current_status == 1)
			{
				set_index_of_white_list_pos(1,uidpos);
			}
			
			if((clicker_send_data_current_status == 2)|(clicker_send_data_current_status == 3))
			{
				set_index_of_white_list_pos(5,uidpos);
			}
		}
		
		/* �������Ƿ�ر� */
		if(white_on_off == OFF)
		{
			/* �������ر�����͸�� */
			Is_whitelist_uid = OPERATION_SUCCESS;
		}

		/* ������ƥ�� */
		if(Is_whitelist_uid == OPERATION_SUCCESS)
		{
			/* get uid */
			memcpy(sign_buffer   ,nrf_communication.receive_buf+5 ,4);
			memcpy(nrf_communication.dtq_uid,nrf_communication.receive_buf+5 ,4);

			/* �յ�����ACK */
			if(nrf_communication.receive_buf[11] == NRF_DATA_IS_ACK)
			{
//			printf("NRF_DATA_IS_ACK\r\n");
//			printf("sequence num = %2x \r\n",(uint8_t)*(nrf_communication.receive_buf+9));
//			printf("package  num = %2x \r\n",(uint8_t)*(nrf_communication.receive_buf+10));
//			printf("UID = %2x%2x%2x%2x",*(nrf_communication.receive_buf+5),*(nrf_communication.receive_buf+6),*(nrf_communication.receive_buf+7),*(nrf_communication.receive_buf+8));
				/* ����ACK�İ��ź��ϴη��͵��Ƿ���ͬ */
				if(nrf_communication.receive_buf[10] == jsq_to_dtq_packnum)
				{
					nrf_communication.transmit_ok_flag = true;
//				jsq_to_dtq_packnum++;
//				printf("irq_debug, same sequence %02X  \n",nrf_communication.receive_buf[10]);
//				for(i = 0; i < nrf_communication.receive_len; i++)
//				{
//					printf("%02X ", nrf_communication.receive_buf[i]);
//				}printf("\r\n");
				}
				else
				{
					//my_nrf_transmit_start(&dtq_to_jsq_sequence,0,NRF_DATA_IS_ACK);
				}
			}
			else//�յ�������Ч����
			{
//				printf("NRF_DATA_NOT_ACK\r\n");
//				printf("sequence num = %2x \r\n",(uint8_t)*(nrf_communication.receive_buf+9));
//				printf("package  num = %2x \r\n",(uint8_t)*(nrf_communication.receive_buf+10));

				/* �ظ����յ����ݣ����ذ��ź��ϴ�һ����ACK */
				if(dtq_to_jsq_packnum == nrf_communication.receive_buf[10])
				{
          /* �ж��Ƿ�Ϊ��ǿ�룬�ظ�ACK */
					if(dtq_to_jsq_sequence != nrf_communication.receive_buf[9])
					{
						dtq_to_jsq_sequence = nrf_communication.receive_buf[9];
						dtq_to_jsq_packnum = nrf_communication.receive_buf[10];
						
						my_nrf_transmit_start(&dtq_to_jsq_sequence,0,NRF_DATA_IS_ACK,1);
					}
				}
				else//��Ч���ݣ�����ACK
				{
//				printf("irq_debug dtq_to_jsq_sequence = %02X  \r\n",nrf_communication.receive_buf[10]);
//				for(i = 0; i < nrf_communication.receive_len; i++)
//				{
//					printf("%02X ", nrf_communication.receive_buf[i]);
//				}
//				printf("\r\n");
					/* ��Ч���ݸ��Ƶ����� */
					rf_move_data_to_buffer(&nrf_communication);
					/* ���½�������֡������� */
					dtq_to_jsq_sequence = nrf_communication.receive_buf[9];
					dtq_to_jsq_packnum = nrf_communication.receive_buf[10];
					/* �ظ�ACK */
					my_nrf_transmit_start(&dtq_to_jsq_sequence,0,NRF_DATA_IS_ACK,1);
					/* �û����յ����ݴ����� */
					my_nrf_receive_success_handler();
				}
			}
		}
		else //��������ƥ�䣬�˵�
		{
//			printf("UID = %2x%2x%2x%2x \r\n",
//		       *(nrf_communication.receive_buf+1),*(nrf_communication.receive_buf+2),
//		       *(nrf_communication.receive_buf+2),*(nrf_communication.receive_buf+3));
//		  printf("Update:The Clickers not register! \r\n ");
		}
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
				jsq_to_dtq_sequence++;
				nrf_communication.transmit_buf[9] = jsq_to_dtq_sequence;
				nrf_communication.transmit_buf[15 + rf_var.tx_len] = XOR_Cal(nrf_communication.transmit_buf+1,14+rf_var.tx_len);
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
