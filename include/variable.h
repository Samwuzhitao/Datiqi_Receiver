/**
  ******************************************************************************
  * @file   	variable.h
  * @author  	Tian erjun
  * @version 	V1.0.0.0
  * @date   	2015.11.05
  * @brief   	all variables called by other functions
  ******************************************************************************
  */
#ifndef _VARIABLE_H_
#define _VARIABLE_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "define.h"
#include "hal_nrf.h"

/**
 * @enum nrf_esb_base_address_length_t
 * @brief Enumerator used for selecting the base address length.
 */
typedef enum 
{
    NRF_ESB_BASE_ADDRESS_LENGTH_2B,   ///< 2 byte address length
    NRF_ESB_BASE_ADDRESS_LENGTH_3B,   ///< 3 byte address length
    NRF_ESB_BASE_ADDRESS_LENGTH_4B    ///< 4 byte address length
} nrf_esb_base_address_length_t;


/**
 * @enum nrf_esb_output_power_t
 * @brief Enumerator used for selecting the TX output power.
 */
typedef enum 
{
    NRF_ESB_OUTPUT_POWER_4_DBM,          ///<  4 dBm output power.
    NRF_ESB_OUTPUT_POWER_0_DBM,          ///<  0 dBm output power.
    NRF_ESB_OUTPUT_POWER_N4_DBM,         ///< -4 dBm output power.
    NRF_ESB_OUTPUT_POWER_N8_DBM,         ///< -8 dBm output power.
    NRF_ESB_OUTPUT_POWER_N12_DBM,        ///< -12 dBm output power.
    NRF_ESB_OUTPUT_POWER_N16_DBM,        ///< -16 dBm output power.
    NRF_ESB_OUTPUT_POWER_N20_DBM         ///< -20 dBm output power.
} nrf_esb_output_power_t;


/**
 * @enum nrf_esb_datarate_t
 * @brief Enumerator used for selecting the radio data rate.
 */
typedef enum 
{
    NRF_ESB_DATARATE_250_KBPS,            ///< 250 Kbps datarate
    NRF_ESB_DATARATE_1_MBPS,              ///< 1 Mbps datarate
    NRF_ESB_DATARATE_2_MBPS,              ///< 1 Mbps datarate
} nrf_esb_datarate_t;


/**
 * @enum nrf_esb_crc_length_t
 * @brief Enumerator used for selecting the CRC length.
 */
typedef enum 
{
    NRF_ESB_CRC_OFF,            ///< CRC check disabled
    NRF_ESB_CRC_LENGTH_1_BYTE,  ///< CRC check set to 8-bit
    NRF_ESB_CRC_LENGTH_2_BYTE   ///< CRC check set to 16-bit    
} nrf_esb_crc_length_t;


typedef enum {SINGLE_CHOICE = 0x01, MULTIPLE_CHOICE = 0x02, TRUE_OR_FALSE = 0x03} QuestionType;
typedef enum {A = 0x01, B = 0x02, C = 0x04, D = 0x08, E = 0x10, F = 0x20, G = 0x40, H = 0x80} ChoiceAnswer;
typedef enum {LETTER = 0, NUMBER = !LETTER} InputMode;
typedef enum {LED1 	= 0,LED2 	= 1,LGREEN 	= 2,LBLUE 	= 3,} Led_TypeDef;
typedef enum {OFF 	= 0,ON 	= 1} Switch_State;

typedef struct
{

	bool 					flag_uart_rxing;		  			//�жϴ������ڽ��ձ�־
	bool 					flag_uart_rx_ok;		  			//�жϴ��ڽ�����ɱ�־
	
	bool 					flag_uart_rx_xor_err;				//�жϴ��ڽ���XOR�����־
	bool 					flag_uart_rx_length_err;			//�жϴ��ڽ��ճ��ȳ����־
	
	uint8_t 				uart_status;						//�жϽ������ݲ������
	uint8_t 				uart_temp;							//�жϽ���������ʱ����
	uint8_t 				temp_uid_len;						//�жϽ��������������	
	uint8_t					temp_data_len;						//�жϽ���DATA������ʱ���ȱ���	
	uint8_t                 temp_sign_len;                      //�жϽ���SIGN������ʱ���ȱ���
	
	uint8_t 				uart_rx_timeout;					//�жϴ��ڽ��ճ�ʱ������

	
	uint8_t 				HEADER;								//�жϴ��ڽ���֡ͷ
	uint8_t 				TYPE;								//�жϴ��ڽ��հ�����
	uint8_t                 SIGN[4];                            //�жϴ��ڽ��ջ��ʶ
	uint8_t 				LEN;								//�жϴ��ڽ������ݳ���
	uint8_t 				DATA[UART_NBUF];					//�жϴ��ڽ�������
	uint8_t 				XOR;								//�жϴ��ڽ������
	uint8_t 				END;								//�жϴ��ڽ���֡β
	
	uint8_t	 				uart_rx_cnt;			  			//�жϴ��ڽ��ռ�����
	uint8_t					uart_rx_buf[UART_NBUF + 5];			//�жϴ��ڽ��ջ���	
	
	bool 					flag_tx_ok[2];		  				//�жϴ��ڽ�����ɱ�־
	bool 					flag_txing[2];		   				//�жϴ������ڷ��ͱ�־
	uint8_t 				uart_tx_length[2];					//�жϴ��ڷ��ͳ���	
	uint8_t 				uart_tx_cnt;			  			//�жϴ��ڷ��ͼ�����
	uint8_t					uart_tx_buf[2][UART_NBUF + 5];		//�жϴ��ڷ��ͻ��� ,���黺�������������� 

}Uart_TypeDef;

typedef struct 
{	
	bool 					flag_txing;		   					//2.4G�·����ݱ�־
	bool 					flag_rx_ok;		  					//2.4G���յ������ݰ�
	bool 					flag_tx_ok;		  					//2.4G���յ������ݰ�
	
	uint8_t 				tx_len;			  					//2.4G���ͳ���
	uint8_t	 				rx_len;			  					//2.4G���ճ���
	uint8_t					tx_buf[RF_NBUF];					//2.4G���ͻ���  
	uint8_t					rx_buf[RF_NBUF];					//2.4G���ջ���
}RF_TypeDef;

typedef struct 
{	
	uint8_t 						packet_len;					//������
	uint8_t 						receive_flag;				//���ձ�ʶ
	uint8_t							data[32];					//ʵ������
}recv_data_packet_t;



typedef struct
{
	uint8_t 						hour;
	uint8_t 						minute;
	uint8_t 						second;
}time_t;

typedef struct
{
	bool 							state;						//uid��Ч��־
	bool	 						tx_flag;					//���·���־	
	uint8_t 						count;						//�յ��հ�����
	uint8_t							number;						//������
	uint8_t 						uid[4];						//UID
}white_list_t;

typedef union
{
	uint8_t 						true_or_false;				//�ж����Ӧ��
	uint8_t							choice_answer;				//ѡ�����Ӧ��
}answer_t;

typedef struct 
{	
	uint8_t							question_amount;			//��Ŀ��������Ŀ�𰸵���Ч��������ΧΪ1-11
	uint8_t 						question_number;			//��ţ��������ݵĵ�ǰ��ż�����һ�⿪ʼ��
	uint8_t							question_type[4];			//��Ŀ���ͣ�ÿ2λ��ʾһ����Ŀ���ͣ�1Ϊ��ѡ��2Ϊ��ѡ��3Ϊ�ж�
	answer_t						answer[16];					//��Ŀ�𰸣�ÿ����Ŀռ1bytes
}answer_packet_t;

typedef struct 
{
	uint8_t							header;						//�̶�0x5A
	uint8_t							uid[4];						//uid
	uint8_t							pack_num;					//���ݰ��İ���
	uint8_t							pack_type;					//���ݰ�������
	uint8_t							payload_len;				//������ĳ���
	uint8_t							payload[22];				//�����������
	uint8_t							xor_value;					//���У��
	uint8_t							end;						//�̶�Ϊ0xCA
}datiqi_type_t;

typedef struct 
{
	hal_nrf_output_power_t			power;						//�������
	hal_nrf_datarate_t	 			datarate;					//����
	hal_nrf_crc_mode_t	 			crc_length;					//CRCУ�鳤��
	hal_nrf_address_width_t			base_address_length;		//����ַ����
	uint8_t							channel;					//Ƶ��
	uint8_t							pipe;						//����ͨ��	
	uint8_t							attempts;					//�ط�����
	uint8_t							prefix_address;				//ǰ׺��ַ
	uint16_t						base_address_low;			//����ַ��16λ
	uint16_t						base_address_high;			//����ַ��16λ
	uint16_t						delay_us;					//�ط��ӳ�ʱ��
}nrf_parameter_t;

typedef struct 
{
	nrf_esb_output_power_t			power;							//�������
	nrf_esb_datarate_t	 			datarate;						//����
	nrf_esb_crc_length_t 			crc_length;						//CRCУ�鳤��
	nrf_esb_base_address_length_t	base_address_length;			//����ַ����
	uint8_t							channel;						//Ƶ��
	uint8_t							pipe;							//����ͨ��
	uint8_t							attempts;						//�ط�����
	uint8_t							prefix_address;					//ǰ׺��ַ
	uint16_t						base_address_low;				//����ַ��16λ
	uint16_t						base_address_high;				//����ַ��16λ
	uint16_t						delay_us;						//�ط��ӳ�ʱ��
}answer_nrf_parameter_t;

typedef struct 
{
	uint8_t							Length_high_bytes;				//���ݳ��ȸ��ֽ�	
	uint8_t							Length_low_bytes;				//���ݳ��ȵ��ֽ�
	answer_nrf_parameter_t			ptx_parameter;					//������Ƶ����
	answer_nrf_parameter_t			prx_parameter;					//������Ƶ����
//	uint16_t						nrfSleepTime;					//�������ʱ��
//	uint8_t							nrfSleepFlag;					//���߱�־
//	uint8_t							nrfWorkMode;					//����ģʽ��־
//	uint8_t							nrfMatchSIM;					//ƥ���־
//	uint8_t							HWVersion[14];					//���Ӳ���汾��ZL-S741-MAIN-X
//	uint16_t						SWVersion;						//�������汾
//	uint8_t							SIM_UID[8];						//SIM�����к�
	uint8_t							InitFlag;						//��ʼ����־
	uint8_t							Xor;							//���ݶ�У��ֵ
}answer_setting_parameter_t;

extern white_list_t					white_list[MAX_WHITE_LEN];		// �������б�
extern uint8_t						white_len;						// ����������
extern Switch_State					white_on_off;					// ����������
extern Switch_State					attendance_on_off;				// ���ڿ��� 
extern Switch_State					match_on_off;					// ��Կ��� 
extern uint16_t						match_number;					// ������

extern uint8_t 						NDEF_DataWrite[];
extern uint8_t 						NDEF_DataRead[];

extern bool 						gbf_hse_setup_fail;				//�ⲿ16M���������־
extern uint8_t 						time_for_buzzer_on;				//�೤ʱ��֮��ʼ��
extern uint16_t 					time_for_buzzer_off;			//���˶೤ʱ��֮���

extern answer_setting_parameter_t	answer_setting;					// �����������ò���
extern nrf_parameter_t				nrf_parameter;


extern uint8_t                      sign_buffer[];
extern uint8_t						uart_tx_i;						//���ڷ��ͻ�������
extern Uart_TypeDef 				uart232_var;
extern RF_TypeDef 					rf_var;							// 2.4G���ݰ�����
extern uint16_t						delay_nms;						// �ж���ʱ����
extern uint32_t 					timer_1ms;
extern time_t						time;							//���ֵ�ǰʱ��

extern uint8_t						uid_len;
extern uint8_t 						g_cSNR[];                    	//M1�����к�

extern bool							flag_upload_uid_once;			//�Ƿ񵥴��ϴ����ű�־
extern uint8_t						ReadNDEF_Step;					// ��ȡNDEF�ļ��Ĳ���
extern uint8_t						FindCard_Step;					// Ѱ���Ĳ���

extern uint8_t 						flag_App_or_Ctr;
extern uint16_t 					Length_CtrRC500ToApp;
extern uint16_t 					Length_AppToCtrRC500;
extern uint16_t 					Length_CtrPosToApp;
extern uint16_t 					Length_AppToCtrPos;
extern uint8_t 						Buf_CtrRC500ToApp[];
extern uint8_t 						Buf_AppToCtrRC500[];
extern uint8_t 						Buf_CtrPosToApp[];
extern uint8_t 						Buf_AppToCtrPos[];
extern uint8_t 						Buf_CtrRC500return[];			 //RC500�������ݻ�����
extern bool 						App_to_CtrPosReq;
extern bool 						App_to_CtrRC500Req;

extern uint8_t 						g_cardType[];					//���ؿ�����
extern uint8_t 						respon[];
extern uint8_t 						g_cCid;

extern uint32_t                     MCUID[4];
extern uint8_t                     jsq_uid[8];
extern uint8_t                     software[3];
extern uint8_t 					   hardware[30];
extern uint8_t 					   company[16];
#endif //_VARIABLE_H_
/**
  * @}
  */
/**************************************END OF FILE****************************/

