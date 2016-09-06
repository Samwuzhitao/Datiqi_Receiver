#include "whitelist.h"

/* Private variables ---------------------------------------------------------*/
uint8_t						white_len;						        // ����������
Switch_State			white_on_off;					        // ���������� 
Switch_State			attendance_on_off;		        // ���ڿ��� 
Switch_State			match_on_off;					        // ��Կ��� 
uint16_t					match_number = 1;			  	    // ������
uint8_t           uid_p;
uint8_t		        uid_len = 0;					        // M1�����кų���
uint8_t 	        g_cSNR[10];						        // M1�����к�

/******************************************************************************
  Function:initialize_white_list
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
bool initialize_white_list( void )
{
	uint16_t FlashStatus;
	
	white_len = get_len_of_white_list();
	
	FlashStatus = Fee_Init(FEE_INIT_CLEAR);
	if (FlashStatus != FLASH_COMPLETE)
	{
		return false;
	}
	
	white_on_off = OFF;
	store_switch_status_to_fee(white_on_off);
	
	return true;
}

/******************************************************************************
  Function:uidcmp
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
bool uidcmp(uint8_t *uid1, uint8_t *uid2)
{
	if((uid1[0] == uid2[0])&&(uid1[1] == uid2[1])&&(uid1[2] == uid2[2])&&(uid1[3] == uid2[3]))
		return true;
	else
		return false;
}

/******************************************************************************
  Function:get_index_of_uid
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
void get_index_of_uid( uint8_t index, uint8_t  uid[4] )
{
	uint16_t tmpuid[4];
	uint16_t viraddr = index * 4;
	uint8_t  i;
	
	for(i=0;i<4;i++)
	{
		EE_ReadVariable(viraddr+i,tmpuid+i);
		uid[i] = (uint8_t)(tmpuid[i]&0xFF);
	}
}

/******************************************************************************
  Function:clear_index_of_uid
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
void clear_index_of_uid(uint8_t index)
{
	uint16_t viraddr = index * 4;
	uint8_t  i;
	
	for(i=0;i<4;i++)
	{
		EE_WriteVariable(viraddr+i,0xFF);
	}
}

/******************************************************************************
  Function:add_index_of_uid
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
void add_index_of_uid( uint8_t index, uint8_t  uid[4] )
{
	uint16_t viraddr = index * 4;
	uint8_t  i;
	
	for(i=0;i<4;i++)
	{
		EE_WriteVariable(viraddr+i,uid[i]);
		DelayMs(20);
	}
}

/******************************************************************************
  Function:search_uid_in_white_list
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
bool search_uid_in_white_list(uint8_t *g_uid , uint8_t *position)
{
	uint8_t i;
	uint8_t temuid[4];
	
	/* ������Ϊ�գ�ֱ�ӷ���ʧ�� */
	if(white_len == 0)
		return false;				
	
	for(i=0; i < white_len; i++)
	{
		get_index_of_uid( i, temuid);
		
		/* ������������Ѿ����ڸ�UID */
		if(uidcmp(temuid,g_uid))	
		{  
			*position = i;
			return true;
		}
	}
	return false;
}

/******************************************************************************
  Function:delete_uid_from_white_list
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
bool delete_uid_from_white_list(uint8_t *g_uid)
{
	uint8_t j;
	uint8_t temuid[4];
	uint8_t pos;
	uint8_t status = true;
	
	/* delete the uid form white list*/
	status = search_uid_in_white_list( g_uid, &pos );
	if(status == false)
		return false;
	
	/* delete the uid form white list*/
  clear_index_of_uid( pos );
	
	/* move uid */
	for(j=pos;j<white_len;j++)
	{
		get_index_of_uid( j+1, temuid );
		add_index_of_uid( j,   temuid );
	}
	
	white_len --;
	store_len_to_fee(white_len);
	
	return true;
}

/******************************************************************************
  Function:get_len_of_white_list
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
uint8_t get_len_of_white_list(void)
{
	uint16_t len = 0;
	
	EE_ReadVariable(WHITE_LIST_LEN_POS_OF_FEE,&len);
	
	return (uint8_t)(len&0xFF);
}

/******************************************************************************
  Function:store_len_to_fee
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
void store_len_to_fee(uint8_t len)
{
	EE_WriteVariable(WHITE_LIST_LEN_POS_OF_FEE,len);
}

/******************************************************************************
  Function:store_switch_status_to_fee
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
void store_switch_status_to_fee(uint8_t switch_status)
{
	EE_WriteVariable(WHITE_LIST_SW_POS_OF_FEE,switch_status);
}

/******************************************************************************
  Function:get_switch_status_of_white_list
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
uint8_t get_switch_status_of_white_list(void)
{
	uint16_t switch_status = 0;
	
	EE_ReadVariable(WHITE_LIST_SW_POS_OF_FEE,&switch_status);
	
	return (uint8_t)(switch_status&0xFF);
}

/******************************************************************************
  Function:add_uid_to_white_list
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
bool add_uid_to_white_list(uint8_t *g_uid, uint8_t *position)
{
	uint8_t i;
	uint8_t temuid[4];
	
	/* ���������������ֱ�ӷ���ʧ�� */
	if(white_len == MAX_WHITE_LEN)						
		return false;
	
	/* ������������Ѿ����ڸ�UID */
	for(i=0; i < MAX_WHITE_LEN; i++)
	{
		get_index_of_uid( i, temuid);
		
		if(uidcmp(temuid, g_uid))	              
			return false;
	}
	
	/* ����������Ѿ��ﵽ��ȣ��޷������uid */
	if(white_len >= MAX_WHITE_LEN)						
	{
		white_len = MAX_WHITE_LEN;
		return false;
	}
	else //UID�ڰ�������������ţ�ֱ������ĩβ����µ�uid
	{
		//printf("UID = %2x%2x%2x%2x \r\n",g_uid[0],g_uid[1],g_uid[2],g_uid[3]);
		add_index_of_uid(white_len,g_uid);
		*position = white_len;							 
	}
	
	white_len ++;
	store_len_to_fee(white_len);
	
	return true;
}



