#ifndef	__WHITE_LIST_H_
#define	__WHITE_LIST_H_
/* Includes ------------------------------------------------------------------*/
#include "main.h"

#define MAX_WHITE_LEN						   (120)
#define WHITE_LIST_LEN_POS_OF_FEE  (481)

typedef struct
{
	bool 							  state;						//uid��Ч��־
	bool	 						  tx_flag;					//���·���־	
	uint8_t 						count;						//�յ��հ�����
	uint8_t							number;						//������
	uint8_t 						uid[4];						//UID
} white_list_t;

typedef enum 
{
	OFF 	= 0,
	ON 	= 1
} Switch_State;

//extern white_list_t			white_list[MAX_WHITE_LEN];		// �������б�
extern uint8_t						white_len;						// ����������
extern Switch_State				white_on_off;					// ����������
extern Switch_State				attendance_on_off;		// ���ڿ��� 
extern Switch_State				match_on_off;					// ��Կ��� 
extern uint16_t						match_number;					// ������
extern uint8_t            uid_p;
extern uint8_t		        uid_len;					    // M1�����кų���
extern uint8_t 	          g_cSNR[10];						// M1�����к�


void get_index_of_uid( uint8_t index, uint8_t  uid[4] );
void clear_index_of_uid(uint8_t index);
void add_index_of_uid( uint8_t index, uint8_t  uid[4] );

bool initialize_white_list( void );
bool uidcmp(uint8_t *uid1, uint8_t *uid2);
bool insert_uid_to_white_list(uint8_t *g_uid, uint8_t *position);
void clear_white_list_tx_flag(void);
bool delete_uid_from_white_list(uint8_t *g_uid);

void store_len_to_fee(uint8_t len);
uint8_t get_len_of_white_list(void);
#endif // __WHITE_LIST_H_
