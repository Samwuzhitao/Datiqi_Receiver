#ifndef __POWER_H
#define	__POWER_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "define.h"
#include "nrf_delay.h"


typedef struct 
{
	uint8_t				sys_state;	
	bool				state_change_flg;
	bool				request_data_flg;		//����/���Ѻ��������ݱ�־
	uint8_t				request_data_num;		//����/���Ѻ��������ݴ���������Լ�����Զ��ػ�
	uint8_t 			request_state;			//��������״̬�����������ǻ�������
	uint16_t			sys_off_count;			//ϵͳ�ػ�������1��+1��45min��ػ�
	
	void				(*off)(void);
	void				(*on)(void);
	void				(*sleep)(void);
	void				(*wakeup)(void);
	
	void				(*send_state)(uint8_t sys_state);

}power_struct_t;


extern power_struct_t				*POWER;

void POWER_init(void);

#endif 

