/**
  ******************************************************************************
  * @file   	app_handle_layer.c
  * @author  	Tian erjun
  * @version 	V1.0.0.0
  * @date   	2015.11.05
  * @brief   	
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
uint8_t FindICCard(void);
bool initialize_white_list( void );
bool delete_uid_from_white_list(uint8_t *g_uid);
bool insert_uid_to_white_list(uint8_t *g_uid, uint8_t *position);
bool search_uid_in_white_list(uint8_t *g_uid, uint8_t *position);
void clear_white_list_tx_flag(void);
bool uidcmp(uint8_t *uid1, uint8_t *uid2);
static void Buzze_Control(void);
void App_returnInsertState(uint8_t sw1, uint8_t sw2);
void App_returnDeleteState(uint8_t sw1, uint8_t sw2);
void App_returnInitializeState(void);
void App_returnWhiteListSwitchState(Switch_State SWS);
void App_returnAttendanceSwitchState(Switch_State SWS);
void App_returnMatchSwitchState(Switch_State SWS);
void App_returnErr(uint8_t cmd_type, uint8_t err_type);
void app_handle_layer(void)
{
	uint8_t temp_count = 0, uid_p,i;
	
	if(rf_var.flag_tx_ok)
	{
		flag_App_or_Ctr = 0x03;
		rf_var.flag_tx_ok = false;
	}
	else if(rf_var.flag_rx_ok)
	{
		flag_App_or_Ctr = 0x02;
		rf_var.flag_rx_ok = false;
	}
	
	switch(flag_App_or_Ctr)
	{		
		case 0x01:		//�·�����������ָ��
			memcpy(rf_var.tx_buf, Buf_CtrPosToApp, Length_CtrPosToApp);
			rf_var.tx_len = Length_CtrPosToApp;
			rf_var.flag_txing = true;
			clear_white_list_tx_flag();
			
			Length_AppToCtrPos = 0x00;
			Buf_AppToCtrPos[Length_AppToCtrPos++] = 0x5C;
			Buf_AppToCtrPos[Length_AppToCtrPos++] = 0x10;
		    Buf_AppToCtrPos[Length_AppToCtrPos++] = sign_buffer[0];
		    Buf_AppToCtrPos[Length_AppToCtrPos++] = sign_buffer[1];
		    Buf_AppToCtrPos[Length_AppToCtrPos++] = sign_buffer[2];
		    Buf_AppToCtrPos[Length_AppToCtrPos++] = sign_buffer[3];
			Buf_AppToCtrPos[Length_AppToCtrPos++] = 0x03;
			Buf_AppToCtrPos[Length_AppToCtrPos++] = 0x00;
			Buf_AppToCtrPos[Length_AppToCtrPos++] = white_on_off;
			Buf_AppToCtrPos[Length_AppToCtrPos++] = white_len;
			Buf_AppToCtrPos[Length_AppToCtrPos++] = XOR_Cal(&Buf_AppToCtrPos[1], 9);
			Buf_AppToCtrPos[Length_AppToCtrPos++] = 0xCA;
			App_to_CtrPosReq =true;
			flag_App_or_Ctr = 0x00;
			break;

		case 0x02:		
//			memcpy(Buf_AppToCtrPos, rf_var.rx_buf, rf_var.rx_len);	//�յ�������������ֱ���ϴ�	    
//		    Length_AppToCtrPos = rf_var.rx_len ;
//			memset(rf_var.rx_buf, 0x00, rf_var.rx_len);
//			rf_var.rx_len = 0x00;
//			flag_App_or_Ctr = 0x00;
//			App_to_CtrPosReq =true;

		    Length_AppToCtrPos = rf_var.rx_len+0x09;  //���յ��𰸴���ϴ�
		    Buf_AppToCtrPos[0] = 0x5C;
			Buf_AppToCtrPos[1] = 0x10;
		    Buf_AppToCtrPos[2] = sign_buffer[0];
		    Buf_AppToCtrPos[3] = sign_buffer[1];
		    Buf_AppToCtrPos[4] = sign_buffer[2];
		    Buf_AppToCtrPos[5] = sign_buffer[3];
		    Buf_AppToCtrPos[6] =rf_var.rx_len+0x00;
		    for (temp_count=0;temp_count<rf_var.rx_len+1;temp_count++)
		    {
		    Buf_AppToCtrPos[temp_count+7]=rf_var.rx_buf[temp_count];		
		    }
	        Buf_AppToCtrPos[rf_var.rx_len+7] = XOR_Cal(&Buf_AppToCtrPos[1], rf_var.rx_len+7);		
		    Buf_AppToCtrPos[rf_var.rx_len+8] = 0xCA;
			memset(rf_var.rx_buf, 0x00, rf_var.rx_len);
			rf_var.rx_len = 0x00;
			flag_App_or_Ctr = 0x00;
			App_to_CtrPosReq =true;				
			break;
		case 0x03:		//�ϴ��·��ɹ���Ϣ
			Length_AppToCtrPos = 0x00;
			Buf_AppToCtrPos[Length_AppToCtrPos++] = 0x5C;
			Buf_AppToCtrPos[Length_AppToCtrPos++] = 0x11;
		    Buf_AppToCtrPos[Length_AppToCtrPos++] = sign_buffer[0];
		    Buf_AppToCtrPos[Length_AppToCtrPos++] = sign_buffer[1];
		    Buf_AppToCtrPos[Length_AppToCtrPos++] = sign_buffer[2];
		    Buf_AppToCtrPos[Length_AppToCtrPos++] = sign_buffer[3];
			Buf_AppToCtrPos[Length_AppToCtrPos++] = 0x04;
			memcpy(&Buf_AppToCtrPos[7], &rf_var.rx_buf[1], 4);
			Buf_AppToCtrPos[11] = XOR_Cal(&Buf_AppToCtrPos[1], 10);
			Buf_AppToCtrPos[12] = 0xCA;
			Length_AppToCtrPos = 13;
			App_to_CtrPosReq =true;
			flag_App_or_Ctr = 0x00;
			break;	
		case 0x04:		//ֹͣ�·�ָ��
			rf_var.flag_txing = false;
			memset(rf_var.tx_buf, 0x00, rf_var.tx_len);
			rf_var.tx_len = 0x00;
			clear_white_list_tx_flag();
		
			Length_AppToCtrPos = 0x00;
			Buf_AppToCtrPos[Length_AppToCtrPos++] = 0x5C;
			Buf_AppToCtrPos[Length_AppToCtrPos++] = 0x12;
		    Buf_AppToCtrPos[Length_AppToCtrPos++] = sign_buffer[0];
		    Buf_AppToCtrPos[Length_AppToCtrPos++] = sign_buffer[1];
		    Buf_AppToCtrPos[Length_AppToCtrPos++] = sign_buffer[2];
		    Buf_AppToCtrPos[Length_AppToCtrPos++] = sign_buffer[3];
			Buf_AppToCtrPos[Length_AppToCtrPos++] = 0x03;
			Buf_AppToCtrPos[Length_AppToCtrPos++] = 0x00;
			Buf_AppToCtrPos[Length_AppToCtrPos++] = white_on_off;
			Buf_AppToCtrPos[Length_AppToCtrPos++] = white_len;
			Buf_AppToCtrPos[Length_AppToCtrPos++] = XOR_Cal(&Buf_AppToCtrPos[1], 9);
			Buf_AppToCtrPos[Length_AppToCtrPos++] = 0xCA;
			App_to_CtrPosReq =true;
		
			flag_App_or_Ctr = 0x00;
			break;	
		case 0x11:		//��Ӱ�����
			while(temp_count != Buf_CtrPosToApp[0])
			{
				if(insert_uid_to_white_list(&Buf_CtrPosToApp[4*temp_count + 1], &uid_p))
				{
					temp_count++;
					continue;
				}
				else
					break;
			}
			
			if(temp_count == Buf_CtrPosToApp[0])
				App_returnInsertState(0x00,white_len);
			else
				App_returnInsertState(temp_count + 1,white_len);
			
			flag_App_or_Ctr = 0x00;
			break;
		
		case 0x12:		//ɾ��������
			while(temp_count != Buf_CtrPosToApp[0])
			{
				if(delete_uid_from_white_list(&Buf_CtrPosToApp[4*temp_count + 1]))
				{	
					temp_count++;
					continue;
				}
				else
					break;
			}
			
			if(temp_count == Buf_CtrPosToApp[0])
				App_returnDeleteState(0x00,white_len);
			else
				App_returnDeleteState(temp_count + 1,white_len);
			
			flag_App_or_Ctr = 0x00;
			break;
		case 0x13:		//��ʼ��������
			initialize_white_list();
			App_returnInitializeState();
			flag_App_or_Ctr = 0x00;	
			break;
		
		case 0x14:		//����������
			white_on_off = ON;
			App_returnWhiteListSwitchState(ON);
			flag_App_or_Ctr = 0x00;	
			break;
		
		case 0x15:		//�رհ�����
			white_on_off = OFF;
			App_returnWhiteListSwitchState(OFF);
			flag_App_or_Ctr = 0x00;	
			break;
			
		case 0x16:		//��ʼ����
			attendance_on_off = ON;
			App_returnAttendanceSwitchState(ON);
			flag_App_or_Ctr = 0x00;	
			break;
			
		case 0x17:		//ֹͣ����
			attendance_on_off = OFF;
			App_returnAttendanceSwitchState(OFF);
			flag_App_or_Ctr = 0x00;
			break;
			
		case 0x18:		//��ʼ���
			match_on_off = ON;
			match_number = 1;
			initialize_white_list();
			App_returnMatchSwitchState(ON);
			flag_App_or_Ctr = 0x00;
			break;
		
		case 0x19:		//ֹͣ���
			match_on_off = OFF;
			App_returnMatchSwitchState(OFF);
			flag_App_or_Ctr = 0x00;
			break;
		case 0x1a:      //��ӡ��ǰ������			
		    Length_AppToCtrPos = 4*white_len+10;  
		    Buf_AppToCtrPos[0] = 0x5C;
			Buf_AppToCtrPos[1] = 0x2B;
		    Buf_AppToCtrPos[2] = sign_buffer[0];
		    Buf_AppToCtrPos[3] = sign_buffer[1];
		    Buf_AppToCtrPos[4] = sign_buffer[2];
		    Buf_AppToCtrPos[5] = sign_buffer[3];
		    Buf_AppToCtrPos[6] = 4*white_len+1;
            Buf_AppToCtrPos[7] = white_len;
		    for(temp_count=7,uid_p=0;temp_count<4*white_len,uid_p<white_len;temp_count=temp_count+4,uid_p++)
            {
			 Buf_AppToCtrPos[temp_count+1]=white_list[uid_p].uid[0];
             Buf_AppToCtrPos[temp_count+2]=white_list[uid_p].uid[1];
             Buf_AppToCtrPos[temp_count+3]=white_list[uid_p].uid[2];
             Buf_AppToCtrPos[temp_count+4]=white_list[uid_p].uid[3]; 				
			}
            Buf_AppToCtrPos[4*white_len+8] = XOR_Cal(&Buf_AppToCtrPos[1], white_len+3);
            Buf_AppToCtrPos[4*white_len+9] = 0xCA;
		    flag_App_or_Ctr = 0x00;
		    App_to_CtrPosReq =true;
			break;
		case 0x1b:      //��ӡ�豸��Ϣ			
		    Length_AppToCtrPos = 0x27;  
		    Buf_AppToCtrPos[0] = 0x5C;
		    Buf_AppToCtrPos[1] = 0x2C;
		    Buf_AppToCtrPos[2] = sign_buffer[0];
		    Buf_AppToCtrPos[3] = sign_buffer[1];
		    Buf_AppToCtrPos[4] = sign_buffer[2];
		    Buf_AppToCtrPos[5] = sign_buffer[3];	
		    Buf_AppToCtrPos[6] = 0x34;
		    for(temp_count=0,i=0;temp_count<4,i<8;temp_count++,i=i+2)
            {
			 Buf_AppToCtrPos[temp_count+7]=(jsq_uid[i]<<4|jsq_uid[i+1]);	
			}
			
		    for(temp_count=0;temp_count<3;temp_count++)
			{
			 Buf_AppToCtrPos[temp_count+11]=software[temp_count];
			}
			
			for(temp_count=0,i=0;temp_count<15,i<30;temp_count++,i=i+2)
			{
			 Buf_AppToCtrPos[temp_count+14]=(hardware[i]<<4)|(hardware[i+1]);
			 
			}

			for(temp_count=0,i=0;temp_count<8,i<16;temp_count++,i=i+2)
			{
			 Buf_AppToCtrPos[temp_count+29]=(company[i]<<4)|(company[i+1]);
			}			
            Buf_AppToCtrPos[37] = XOR_Cal(&Buf_AppToCtrPos[1],36);
            Buf_AppToCtrPos[38] = 0xCA;
		    flag_App_or_Ctr = 0x00;
		    App_to_CtrPosReq =true;
			break;
				
		case 0x0d:		// ָ�����������	
			if(!App_to_CtrPosReq)
			{
				App_returnErr(Buf_CtrPosToApp[1], 0xE0);
				flag_App_or_Ctr = 0x00;
			}
			break;
		
		case 0xfe:		// ����Ȳ��Ϸ�
			if(!App_to_CtrPosReq)
			{
				App_returnErr(Buf_CtrPosToApp[1],0xE1);			// ���س��ȴ���
				App_to_CtrPosReq = true;											// posָ������
				flag_App_or_Ctr = 0x00;												// Ӧ�ò����
			}
			break;
			
		case 0xff:		// δ���������
			if(!App_to_CtrPosReq)
			{
				App_returnErr(Buf_CtrPosToApp[1],0xE2);
				App_to_CtrPosReq = true;
				flag_App_or_Ctr = 0x00;
			}		
			break;
		
		default:		// Ĭ�ϣ�����
			flag_App_or_Ctr = 0x00;
			break;			
	}
	
	if((delay_nms == 0)&&((attendance_on_off == ON) || match_on_off == ON))
	{
		delay_nms = 200;							//ÿ��Ѱ��5��
		if(FindICCard() == MI_OK)
		{
			if(match_on_off)						//�������Կ���
			{
				if(insert_uid_to_white_list(&g_cSNR[4], &uid_p))
				{
					white_list[uid_p].number = match_number++;
					Buf_AppToCtrPos[0] = 0x5C;
					Buf_AppToCtrPos[1] = 0x29;
				    Buf_AppToCtrPos[2] = sign_buffer[0];
				    Buf_AppToCtrPos[3] = sign_buffer[1];
				    Buf_AppToCtrPos[4] = sign_buffer[2];
				    Buf_AppToCtrPos[5] = sign_buffer[3];
					Buf_AppToCtrPos[6] = 0x05;
					memcpy(&Buf_AppToCtrPos[7], &g_cSNR[4],4);
					Buf_AppToCtrPos[11] = white_list[uid_p].number;
					Buf_AppToCtrPos[12] = XOR_Cal(&Buf_AppToCtrPos[1],11);
					Buf_AppToCtrPos[13] = 0xCA;
					Length_AppToCtrPos = 14;
				}
				else if(search_uid_in_white_list(&g_cSNR[4], &uid_p))
				{
					Buf_AppToCtrPos[0] = 0x5C;
					Buf_AppToCtrPos[1] = 0x29;
				    Buf_AppToCtrPos[2] = sign_buffer[0];
				    Buf_AppToCtrPos[3] = sign_buffer[1];
				    Buf_AppToCtrPos[4] = sign_buffer[2];
				    Buf_AppToCtrPos[5] = sign_buffer[3];					
					Buf_AppToCtrPos[6] = 0x05;
					memcpy(&Buf_AppToCtrPos[7], &g_cSNR[4],4);
					Buf_AppToCtrPos[11] = white_list[uid_p].number;
					Buf_AppToCtrPos[12] = XOR_Cal(&Buf_AppToCtrPos[1],11);
					Buf_AppToCtrPos[13] = 0xCA;
					Length_AppToCtrPos = 14;
				}
				else
				{
					App_returnErr(0x29,0xE3);
				}
				App_to_CtrPosReq = true;				
			}
			else									//����ǿ��ڿ���
			{
				//���ڷ���UID
				Buf_AppToCtrPos[0] = 0x5C;
				Buf_AppToCtrPos[1] = 0x26;
				Buf_AppToCtrPos[2] = sign_buffer[0];
				Buf_AppToCtrPos[3] = sign_buffer[1];
				Buf_AppToCtrPos[4] = sign_buffer[2];
				Buf_AppToCtrPos[5] = sign_buffer[3];
				Buf_AppToCtrPos[6] = 0x04;
				memcpy(&Buf_AppToCtrPos[7], &g_cSNR[4],4);
				Buf_AppToCtrPos[11] = XOR_Cal(&Buf_AppToCtrPos[1],10);
				Buf_AppToCtrPos[12] = 0xCA;
				Length_AppToCtrPos = 0x10;
				App_to_CtrPosReq = true;
			}
			
			//��������300ms
			time_for_buzzer_on = 10;
			time_for_buzzer_off = 300;
			
			//���ظ�Ѱ��
			PcdHalt();
		}
	}
	Buzze_Control();	// �ȴ��������ر�
}

bool uidcmp(uint8_t *uid1, uint8_t *uid2)
{
	if((uid1[0] == uid2[0])&&(uid1[1] == uid2[1])&&(uid1[2] == uid2[2])&&(uid1[3] == uid2[3]))
		return true;
	else
		return false;
}

bool insert_uid_to_white_list(uint8_t *g_uid, uint8_t *position)
{
	uint8_t i;
	if(white_len == MAX_WHITE_LEN)						//���������������ֱ�ӷ���ʧ��
		return false;
	
	for(i=0; i < MAX_WHITE_LEN; i++)
	{
		if((white_list[i].state == 1)&&uidcmp(white_list[i].uid, g_uid))	//������������Ѿ����ڸ�UID
			return false;
	}
	
	if(white_len >= MAX_WHITE_LEN)						//����������Ѿ��ﵽ��ȣ��޷������uid
	{
		white_len = MAX_WHITE_LEN;
		return false;
	}
	else if(white_list[white_len].state == 0)			//���UID�ڰ�������������ţ���ֱ������ĩβ����µ�uid
	{
		memcpy(white_list[white_len].uid, g_uid, 4);	// ����UID
		white_list[white_len].state = 0x01;				// ״̬��1
		*position = white_len;							// ��ȡλ������
	}
	else												//�����������ţ�����ҿ�λ��Ȼ����
	{
		for(i=0; i < MAX_WHITE_LEN; i++)
		{
			if(white_list[i].state == 0)				//����ҵ�һ����λ�����Ž�ȥ
			{
				memcpy(white_list[i].uid, g_uid, 4);	// ����UID
				white_list[i].state = 0x01;				// ״̬��1
				*position = i;							// ��ȡλ������
			}
		}
	}
	white_len ++;
	return true;
}

bool search_uid_in_white_list(uint8_t *g_uid , uint8_t *position)
{
	uint8_t i;
	if(white_len == 0)
		return false;				//������Ϊ�գ�ֱ�ӷ���ʧ��
	for(i=0; i < MAX_WHITE_LEN; i++)
	{
		if((white_list[i].state == 1)&&uidcmp(white_list[i].uid, g_uid))	//������������Ѿ����ڸ�UID
		{
			white_list[i].count ++;  
			*position = i;
			return true;
		}
	}
	return false;
}

void clear_white_list_tx_flag(void)
{
	uint8_t i;
	for(i=0; i < MAX_WHITE_LEN; i++)
	{
		white_list[i].tx_flag = false;
	}
}

bool delete_uid_from_white_list(uint8_t *g_uid)
{
	uint8_t i;
	if(white_len == 0)
		return false;				//������Ϊ�գ�ֱ�ӷ���ʧ��
	for(i=0; i < MAX_WHITE_LEN; i++)
	{
		if((white_list[i].state == 1)&&uidcmp(white_list[i].uid, g_uid))	//������������Ѿ����ڸ�UID
		{
			memset(white_list[i].uid, 0, 4);
			white_list[i].state = 0x00;
			white_len --;
			return true;
		}
	}
	return false;
}

bool initialize_white_list( void )
{
	memset(white_list, 0x00, MAX_WHITE_LEN*sizeof(white_list_t));
	white_len = 0x00;
	white_on_off = OFF;
	return true;
}

void App_returnInsertState(uint8_t sw1, uint8_t sw2)
{
	Length_AppToCtrPos = 0x00;
	Buf_AppToCtrPos[Length_AppToCtrPos ++] = 0x5C;
	Buf_AppToCtrPos[Length_AppToCtrPos ++] = 0x20;
	Buf_AppToCtrPos[Length_AppToCtrPos++] = sign_buffer[0];
	Buf_AppToCtrPos[Length_AppToCtrPos++] = sign_buffer[1];
	Buf_AppToCtrPos[Length_AppToCtrPos++] = sign_buffer[2];
	Buf_AppToCtrPos[Length_AppToCtrPos++] = sign_buffer[3];
	Buf_AppToCtrPos[Length_AppToCtrPos ++] = 0x03;
	Buf_AppToCtrPos[Length_AppToCtrPos ++] = Buf_CtrPosToApp[0];
	Buf_AppToCtrPos[Length_AppToCtrPos ++] = sw1;
	Buf_AppToCtrPos[Length_AppToCtrPos ++] = sw2;
	Buf_AppToCtrPos[Length_AppToCtrPos ++] = XOR_Cal(&Buf_AppToCtrPos[1],9);
	Buf_AppToCtrPos[Length_AppToCtrPos ++] = 0xCA;
	App_to_CtrPosReq = true;
}

void App_returnDeleteState(uint8_t sw1, uint8_t sw2)
{
	Length_AppToCtrPos = 0x00;
	Buf_AppToCtrPos[Length_AppToCtrPos ++] = 0x5C;
	Buf_AppToCtrPos[Length_AppToCtrPos ++] = 0x21;
	Buf_AppToCtrPos[Length_AppToCtrPos++] = sign_buffer[0];
	Buf_AppToCtrPos[Length_AppToCtrPos++] = sign_buffer[1];
	Buf_AppToCtrPos[Length_AppToCtrPos++] = sign_buffer[2];
	Buf_AppToCtrPos[Length_AppToCtrPos++] = sign_buffer[3];
	Buf_AppToCtrPos[Length_AppToCtrPos ++] = 0x03;
	Buf_AppToCtrPos[Length_AppToCtrPos ++] = Buf_CtrPosToApp[0];
	Buf_AppToCtrPos[Length_AppToCtrPos ++] = sw1;
	Buf_AppToCtrPos[Length_AppToCtrPos ++] = sw2;
	Buf_AppToCtrPos[Length_AppToCtrPos ++] = XOR_Cal(&Buf_AppToCtrPos[1],9);
	Buf_AppToCtrPos[Length_AppToCtrPos ++] = 0xCA;
	App_to_CtrPosReq = true;
}

void App_returnInitializeState(void)
{
	Length_AppToCtrPos = 0x00;
	Buf_AppToCtrPos[Length_AppToCtrPos ++] = 0x5C;
	Buf_AppToCtrPos[Length_AppToCtrPos ++] = 0x22;
	Buf_AppToCtrPos[Length_AppToCtrPos++] = sign_buffer[0];
	Buf_AppToCtrPos[Length_AppToCtrPos++] = sign_buffer[1];
	Buf_AppToCtrPos[Length_AppToCtrPos++] = sign_buffer[2];
	Buf_AppToCtrPos[Length_AppToCtrPos++] = sign_buffer[3];
	Buf_AppToCtrPos[Length_AppToCtrPos ++] = 0x01;
	Buf_AppToCtrPos[Length_AppToCtrPos ++] = 0x00;
	Buf_AppToCtrPos[Length_AppToCtrPos ++] = XOR_Cal(&Buf_AppToCtrPos[1],7);	//�����
	Buf_AppToCtrPos[Length_AppToCtrPos ++] = 0xCA;
	App_to_CtrPosReq = true;
}

void App_returnWhiteListSwitchState(Switch_State SWS)
{
	Length_AppToCtrPos = 0x00;
	Buf_AppToCtrPos[Length_AppToCtrPos ++] = 0x5C;
	if(SWS == ON)
		Buf_AppToCtrPos[Length_AppToCtrPos ++] = 0x23;
	else
		Buf_AppToCtrPos[Length_AppToCtrPos ++] = 0x24;
	Buf_AppToCtrPos[Length_AppToCtrPos++] = sign_buffer[0];
	Buf_AppToCtrPos[Length_AppToCtrPos++] = sign_buffer[1];
	Buf_AppToCtrPos[Length_AppToCtrPos++] = sign_buffer[2];
	Buf_AppToCtrPos[Length_AppToCtrPos++] = sign_buffer[3];	
	Buf_AppToCtrPos[Length_AppToCtrPos ++] = 0x01;
	Buf_AppToCtrPos[Length_AppToCtrPos ++] = 0x00;
	Buf_AppToCtrPos[Length_AppToCtrPos ++] = XOR_Cal(&Buf_AppToCtrPos[1],7);
	Buf_AppToCtrPos[Length_AppToCtrPos ++] = 0xCA;
	App_to_CtrPosReq = true;
}


void App_returnAttendanceSwitchState(Switch_State SWS)
{
	Length_AppToCtrPos = 0x00;
	Buf_AppToCtrPos[Length_AppToCtrPos ++] = 0x5C;
	if(SWS == ON)
		Buf_AppToCtrPos[Length_AppToCtrPos ++] = 0x25;
	else
		Buf_AppToCtrPos[Length_AppToCtrPos ++] = 0x27;
	Buf_AppToCtrPos[Length_AppToCtrPos++] = sign_buffer[0];
	Buf_AppToCtrPos[Length_AppToCtrPos++] = sign_buffer[1];
	Buf_AppToCtrPos[Length_AppToCtrPos++] = sign_buffer[2];
	Buf_AppToCtrPos[Length_AppToCtrPos++] = sign_buffer[3];
	Buf_AppToCtrPos[Length_AppToCtrPos ++] = 0x01;
	Buf_AppToCtrPos[Length_AppToCtrPos ++] = 0x00;
	Buf_AppToCtrPos[Length_AppToCtrPos ++] = XOR_Cal(&Buf_AppToCtrPos[1],7);
	Buf_AppToCtrPos[Length_AppToCtrPos ++] = 0xCA;
	App_to_CtrPosReq = true;
}

void App_returnMatchSwitchState(Switch_State SWS)
{
	Length_AppToCtrPos = 0x00;
	Buf_AppToCtrPos[Length_AppToCtrPos ++] = 0x5C;
	if(SWS == ON)
		Buf_AppToCtrPos[Length_AppToCtrPos ++] = 0x28;
	else
		Buf_AppToCtrPos[Length_AppToCtrPos ++] = 0x2A;
	Buf_AppToCtrPos[Length_AppToCtrPos++] = sign_buffer[0];
	Buf_AppToCtrPos[Length_AppToCtrPos++] = sign_buffer[1];
	Buf_AppToCtrPos[Length_AppToCtrPos++] = sign_buffer[2];
	Buf_AppToCtrPos[Length_AppToCtrPos++] = sign_buffer[3];
	Buf_AppToCtrPos[Length_AppToCtrPos ++] = 0x01;
	Buf_AppToCtrPos[Length_AppToCtrPos ++] = 0x00;
	Buf_AppToCtrPos[Length_AppToCtrPos ++] = XOR_Cal(&Buf_AppToCtrPos[1],7);
	Buf_AppToCtrPos[Length_AppToCtrPos ++] = 0xCA;
	App_to_CtrPosReq = true;
}

void App_returnErr(uint8_t cmd_type, uint8_t err_type)
{
	Length_AppToCtrPos = 0x00;
	Buf_AppToCtrPos[Length_AppToCtrPos ++] = 0x5C;
	Buf_AppToCtrPos[Length_AppToCtrPos ++] = cmd_type;
	Buf_AppToCtrPos[Length_AppToCtrPos ++] = 0x01;
	Buf_AppToCtrPos[Length_AppToCtrPos ++] = err_type;
	Buf_AppToCtrPos[Length_AppToCtrPos ++] = XOR_Cal(&Buf_AppToCtrPos[1],3);
	Buf_AppToCtrPos[Length_AppToCtrPos ++] = 0xCA;
	App_to_CtrPosReq = true;
}

void Buzze_Control(void)
{
	if(time_for_buzzer_on == 1)//����������
	{
		BEEP_EN();
		time_for_buzzer_on = 0;
	}
	if(time_for_buzzer_off == 0)
	{
		BEEP_DISEN();
	}
}
/**************************************END OF FILE****************************/
