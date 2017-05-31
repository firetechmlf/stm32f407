#ifndef __BSP_KEY_H__
#define __BSP_KEY_H__

#include "stm32f4xx_hal.h"

/*��������*/
#define KEY_COUNT			2

/*����ID,��Ҫ����bsp_KeyState()��������ڲ���*/
typedef enum
{
	KID_K0=0,
	KID_K1
}KEY_ID_E;

/*
	�����˲�ʱ����50ms,��λ10ms
	ֻ��������⵽50ms״̬�������Ϊ��Ч����������Ͱ��������¼�
*/
#define KEY_FILTER_TIME			5
#define KEY_LONG_TIME				100   //��λ10ms,����1�룬��Ϊ�����¼�


typedef struct
{
	uint8_t (*IsKeyDownFunc)(void);			//�������µ��жϺ�����1��ʾ����
	uint8_t Count;											//�˲�������
	uint16_t LongCount;									//����������
	uint16_t LongTime;									//�������³���ʱ�䣬0��ʾ����ⳤ��
	uint8_t State;											//������ǰ״̬
	uint8_t RepeatSpeed;								//������������
	uint8_t RepeatCount;								//��������������
}KEY_T;



typedef enum
{
	KEY_NONE = 0,				//0��ʾ�����¼�
	KEY_0_DOWN,					//
	KEY_0_UP,
	KEY_0_LONG,
	KEY_1_DOWN,					//
	KEY_1_UP,
	KEY_1_LONG,
}KEY_ENUM;

//����FIFO�õ��ı���
#define KEY_FIFO_SIZE 		10

typedef struct 
{
	uint8_t Buf[KEY_FIFO_SIZE];		//��ֵ������
	uint8_t Read;									//��������ָ��1
	uint8_t Write;								//������дָ��1
	uint8_t Read2;								//��������ָ��2
}KEY_FIFO_T;


/*���ⲿ���ú���*/

void bsp_InitKey(void);
void bsp_KeyScan(void);
void bsp_PutKey(uint8_t _KeyCode);
uint8_t bsp_GetKey(void);
//uint8_t bsp_GetKey2(void);
uint8_t bsp_GetKeyState(KEY_ID_E _ucKeyID);
void bsp_SetKeyParam(uint8_t _ucKeyID, uint16_t _LongTime, uint8_t  _RepeatSpeed);
void bsp_ClearKey(void);

#endif