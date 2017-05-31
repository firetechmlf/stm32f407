#ifndef __BSP_KEY_H__
#define __BSP_KEY_H__

#include "stm32f4xx_hal.h"

/*按键个数*/
#define KEY_COUNT			2

/*按键ID,主要用于bsp_KeyState()函数的入口参数*/
typedef enum
{
	KID_K0=0,
	KID_K1
}KEY_ID_E;

/*
	按键滤波时间是50ms,单位10ms
	只有连续检测到50ms状态不变才认为有效，包括弹起和按下两种事件
*/
#define KEY_FILTER_TIME			5
#define KEY_LONG_TIME				100   //单位10ms,持续1秒，认为长按事件


typedef struct
{
	uint8_t (*IsKeyDownFunc)(void);			//按键按下的判断函数，1表示按下
	uint8_t Count;											//滤波器计数
	uint16_t LongCount;									//长按计数器
	uint16_t LongTime;									//按键按下持续时间，0表示不检测长按
	uint8_t State;											//按键当前状态
	uint8_t RepeatSpeed;								//持续按键周期
	uint8_t RepeatCount;								//连续按键计数器
}KEY_T;



typedef enum
{
	KEY_NONE = 0,				//0表示按键事件
	KEY_0_DOWN,					//
	KEY_0_UP,
	KEY_0_LONG,
	KEY_1_DOWN,					//
	KEY_1_UP,
	KEY_1_LONG,
}KEY_ENUM;

//按键FIFO用到的变量
#define KEY_FIFO_SIZE 		10

typedef struct 
{
	uint8_t Buf[KEY_FIFO_SIZE];		//键值缓冲区
	uint8_t Read;									//缓冲区读指针1
	uint8_t Write;								//缓冲区写指针1
	uint8_t Read2;								//缓冲区读指针2
}KEY_FIFO_T;


/*供外部调用函数*/

void bsp_InitKey(void);
void bsp_KeyScan(void);
void bsp_PutKey(uint8_t _KeyCode);
uint8_t bsp_GetKey(void);
//uint8_t bsp_GetKey2(void);
uint8_t bsp_GetKeyState(KEY_ID_E _ucKeyID);
void bsp_SetKeyParam(uint8_t _ucKeyID, uint16_t _LongTime, uint8_t  _RepeatSpeed);
void bsp_ClearKey(void);

#endif