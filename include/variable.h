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
#include "gpio.h"
//#include "nrf_config_and_hander.h"
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

/* An enum describing the radio's CRC mode ---------------------------------- */
typedef enum
{
	HAL_NRF_CRC_OFF,    /**< CRC check disabled */
	HAL_NRF_CRC_8BIT,   /**< CRC check set to 8-bit */
	HAL_NRF_CRC_16BIT   /**< CRC check set to 16-bit */
} hal_nrf_crc_mode_t;
/* An enum describing the radio's address width ----------------------------- */
typedef enum
{
	HAL_NRF_AW_3BYTES = 3,      /**< Set address width to 3 bytes */
	HAL_NRF_AW_4BYTES,          /**< Set address width to 4 bytes */
	HAL_NRF_AW_5BYTES           /**< Set address width to 5 bytes */
}hal_nrf_address_width_t;


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

/* An enum describing the radio's on-air datarate --------------------------- */
typedef enum
{
	HAL_NRF_1MBPS,          /**< Datarate set to 1 Mbps  */
	HAL_NRF_2MBPS,          /**< Datarate set to 2 Mbps  */
	HAL_NRF_250KBPS         /**< Datarate set to 250 kbps*/
}hal_nrf_datarate_t;


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

/* An enum describing the radio's output power mode's. ---------------------- */
typedef enum
{
	HAL_NRF_18DBM,          /**< Output power set to -18dBm */
	HAL_NRF_12DBM,          /**< Output power set to -12dBm */
	HAL_NRF_6DBM,           /**< Output power set to -6dBm  */
	HAL_NRF_0DBM            /**< Output power set to 0dBm   */
}hal_nrf_output_power_t;

typedef struct 
{	
	bool 					flag_txing;		   					//2.4G�·����ݱ�־
	bool 					flag_tx_ok;		  					//2.4G���յ������ݰ�
	
	uint8_t 			tx_len;			  					//2.4G���ͳ���
	uint8_t				tx_buf[RF_NBUF];					//2.4G���ͻ���  
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



extern uint8_t 						NDEF_DataWrite[];
extern uint8_t 						NDEF_DataRead[];
extern uint16_t						NDEF_Len;

extern bool 						  gbf_hse_setup_fail;				//�ⲿ16M���������־
extern uint8_t 						time_for_buzzer_on;				//�೤ʱ��֮��ʼ��
extern uint16_t 					time_for_buzzer_off;			//���˶೤ʱ��֮���

extern answer_setting_parameter_t	answer_setting;					// �����������ò���
extern nrf_parameter_t				nrf_parameter;

extern RF_TypeDef 				rf_var;							// 2.4G���ݰ�����
extern uint16_t						delay_nms;						// �ж���ʱ����
extern uint32_t 					timer_1ms;
extern time_t						time;							//���ֵ�ǰʱ��

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

typedef struct
{
	uint8_t  uid[4];
	uint8_t  pos;
	uint8_t  use;
	uint8_t  rev_num;
	uint8_t  rev_seq;
	uint8_t  sen_num;
	uint8_t  sen_seq;
	uint8_t  firstrev;
	uint32_t lost_package_num;
	uint32_t recv_package_num;
}Clicker_Typedef;

typedef struct
{
	uint8_t  uid[4];
	uint8_t  sen_num;
	uint8_t  sen_seq;
	uint8_t  pre_seq;
	uint32_t data_statistic_count;
}Revicer_Typedef;

typedef struct
{
	Clicker_Typedef uids[120];
	uint8_t    len;
	uint8_t    switch_status;
	uint8_t    attendance_sttaus;
	uint8_t    match_status;
}WhiteList_Typedef;

typedef struct
{
	uint8_t  status;
	uint32_t cnt;
}Timer_Typedef;

typedef struct 
{
	uint8_t hour;
	uint8_t min;
	uint8_t s;
	uint16_t ms;
}timer_t;

#endif //_VARIABLE_H_
/**
  * @}
  */
/**************************************END OF FILE****************************/

