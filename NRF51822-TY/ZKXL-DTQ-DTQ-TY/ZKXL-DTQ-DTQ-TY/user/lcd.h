/** @file lcd.h
 *
 * @author 
 *
 * @compiler This program has been tested with Keil MDK V4.73.
 *
 * @copyright
 * Copyright (c) 2016 ZKXL. All Rights Reserved.
 *
 * $Date: 		2016.05.19
 * $Revision: 	V1000
 *
 */

#ifndef _LCD_H_
#define _LCD_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "define.h"


//lcdָʾ
#define LCD_DATA          		(8u)
#define LCD_WRB          		(9u)
#define LCD_CSB          		(10u)
#define LCD_POWER				(18u)

//---------------------LCD��������---------------------------------
#define SYSDIS 				0x00 //�ر�ϵͳ����
#define SYSEN 				0x01 //��ϵͳ����
#define LCDOFF 				0x02 //�ر�LCFƫѹ
#define LCDON 				0x03 //��LCFƫѹ
#define RC256 				0x18 //ʹ���ڲ�ʱ��
#define BIAS_1_3			0x29 //LCD1/3ƫѹ����
#define	nop()				nrf_delay_us(1)
#define LCD_DRIVE_DELAY()	do{ __nop();__nop(); }while(0)


//------------------LCD��ʾö������--------------------------------
typedef enum 
{
	SEND_RESULT_CLEAR		= 0X00,
	SEND_RESULT_OK			= 0X01,
	SEND_RESULT_FAIL		= 0X02
}lcd_send_result_enum;		

typedef enum 
{
	JUDGE_CLEAR		= 0X00,
	JUDGE_TRUE		= 0X01,
	JUDGE_FALSE		= 0X02
}lcd_judge_enum;		

typedef enum 
{
	BATTERY_LEVEL_0 = 0X00,
	BATTERY_LEVEL_1 = 0X01,
	BATTERY_LEVEL_2 = 0X02,
	BATTERY_LEVEL_3 = 0X03
}lcd_battery_level_enum;				

typedef enum 
{
	RSSI_0 = 0X00,
	RSSI_1 = 0X01,
	RSSI_2 = 0X02,
	RSSI_3 = 0X03,
	RSSI_4 = 0X04
}lcd_signal_level_enum;				



typedef struct 
{
	bool				update_flg;						//ˢ�±�־
	
	void				(*on)(void);
	void				(*off)(void);
	void				(*sleep)(void);				
	void				(*wakeup)(void);
	
	void				(*timer_start)(void);
	void				(*timer_stop)(void);				
	void				(*timer_handler)(void * p_context);
	
	void                (*update)(void);				//LCDˢ�º���
	
	void				(*battery)(uint8_t tmp);		//��ص���
	void				(*signal)(uint8_t tmp);			//�ź�ǿ��
	void				(*send_result)(uint8_t tmp);	//���ͽ��
	void				(*judge)(uint8_t tmp);			//�ж����
	void				(*choice)(uint8_t tmp);			//ѡ�����
	void				(*student_id)(uint16_t tmp);	//ѧ��
	void				(*question_num)(uint8_t tmp);	//���
	void				(*grade_value)(uint8_t tmp);	//����ֵ
	void                (*clear_segment)(void);	
}lcd_struct_t;


extern lcd_struct_t				*LCD;

void LCD_init(void);
void LCD_display_updata( void );
void LCD_diplay_test(void);

void LCD_diplay_dot( void );
void LCD_clear_dot( void );
void LCD_diplay_segment(uint8_t num, uint8_t data);
void LCD_clear_segment(void);

#endif	//_LCD_H_
 




