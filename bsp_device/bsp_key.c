#include "bsp_key.h"


static KEY_T s_tBtn[KEY_COUNT];
static KEY_FIFO_T s_tKey;


static void bsp_InitKeyVar(void);

static uint8_t IsKeyDown0(void) {if((KEY0_GPIO_Port->IDR &KEY0_Pin) == 0)return 1;else return 0;}
static uint8_t IsKeyDown1(void) {if((KEY1_GPIO_Port->IDR &KEY1_Pin) == 0)return 1;else return 0;}





/*
	初始化按键硬件电路
*/
void bsp_InitKey(void)
{
	bsp_InitKeyVar();
}

/*
* 将一个键值压入按键FIFO缓冲区，可用于模拟一个按键
*/
void bsp_PutKey(uint8_t _KeyCode)
{
	s_tKey.Buf[s_tKey.Write] = _KeyCode;
	if(++s_tKey.Write>=KEY_FIFO_SIZE)
	{
		s_tKey.Write = 0;
	}
}

/*
	从按键FIFO中读取一个键值
*/
uint8_t bsp_GetKey(void)
{
	uint8_t ret;
	if(s_tKey.Read == s_tKey.Write)
	{
		return KEY_NONE;
	}
	else
	{
		ret = s_tKey.Buf[s_tKey.Read];
		if(++s_tKey.Read>=KEY_FIFO_SIZE)
		{
			s_tKey.Read = 0;
		}
		return ret;
	}
}



/*
* 初始化按键变量
*/
static void bsp_InitKeyVar(void)
{
	uint8_t i;
	for(i=0;i<KEY_COUNT;i++)
	{
		s_tBtn[i].LongTime = KEY_LONG_TIME;			//长按时间0表示不检测长按键事件
		s_tBtn[i].Count = KEY_FILTER_TIME/2;		//计数器设置为滤波器时间的一半
		s_tBtn[i].State = 0;										//按键初始状态 0表示未按下
		s_tBtn[i].RepeatCount = 0;							//连发计数器
		s_tBtn[i].RepeatSpeed = 0;							//按键连发的速度，0表示不支持连发
	}
	/*
	*如果需要单独更改某个按键的参数，可以在此重新赋值
	*下面设置为：按键0 按下超过1S后，自动重发相同键值
	*/
//	s_tBtn[KID_K0].LongTime = 100;
//	s_tBtn[KID_K0].RepeatSpeed = 5;
	
	/*
	*判断按键按下的函数
	*/
	s_tBtn[0].IsKeyDownFunc = IsKeyDown0;
	s_tBtn[1].IsKeyDownFunc = IsKeyDown1;
}
/*
* 读取按键状态
*/
uint8_t bsp_GetKeyState(KEY_ID_E _ucKEYID)
{
	return s_tBtn[_ucKEYID].State;
}

/*
	*设置按键参数
形参 ： -ucKeyID :按键ID，从0开始
_LongTime:长按事件时间
_RepeatSpeed:连发速度
*/
void bsp_SetKeyParam(uint8_t _ucKeyID, uint16_t _LongTime, uint8_t  _RepeatSpeed)
{
	s_tBtn[_ucKeyID].LongTime = _LongTime;			// 长按时间 0表示不检测长按时间
	s_tBtn[_ucKeyID].RepeatSpeed = _RepeatSpeed;			 //按键连发速度，0表示不支持连发
	s_tBtn[_ucKeyID].RepeatCount = 0;				   //连发计数器
}

/*
	清空按键FIFO
*/
void bsp_ClearKey(void)
{
	s_tKey.Read = s_tKey.Write;
}



/*
* 检测按键，非阻塞，必须周期性调用
*/
static void bsp_DetectKey(uint8_t i)
{
	KEY_T *pBtn;
	pBtn = &s_tBtn[i];
	if(pBtn->IsKeyDownFunc())
	{
		if(pBtn->Count<KEY_FILTER_TIME)
		{
			pBtn->Count = KEY_FILTER_TIME;
		}
		else if(pBtn->Count<2*KEY_FILTER_TIME)
		{
			pBtn->Count ++;
		}
		else
		{
			if(pBtn->State == 0)
			{
				pBtn->State = 1;
				bsp_PutKey((uint8_t)(3*i+1));
			}
			if(pBtn->LongTime>0)
			{
				if(pBtn->LongCount<pBtn->LongTime)
				{
					if(++pBtn->LongCount == pBtn->LongTime)
					{
						bsp_PutKey((uint8_t)(3*i+3));
					}
				}
				else
				{
					if(pBtn->RepeatSpeed>0)
					{
						if(++pBtn->RepeatCount>=pBtn->RepeatSpeed)
						{
							pBtn->RepeatCount = 0;
							bsp_PutKey((uint8_t)(3*i+1));
						}
					}
				}
			}
		}
	}
	else
	{
		if(pBtn->Count>KEY_FILTER_TIME)
		{
			pBtn->Count = KEY_FILTER_TIME;
		}
		else if(pBtn->Count!= 0)
		{
			pBtn->Count--;
		}
		else
		{
			if(pBtn->State == 1)
			{
				pBtn->State = 0;
				bsp_PutKey((uint8_t)(3*i+2));
			}
		}
		pBtn->LongCount = 0;
		pBtn->RepeatCount=0;
	}
}

void bsp_KeyScan(void)
{
	uint8_t i;
	for(i = 0;i<KEY_COUNT;i++)
	{
		bsp_DetectKey(i);
	}
}



