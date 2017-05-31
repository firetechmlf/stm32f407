#include "bsp_key.h"


static KEY_T s_tBtn[KEY_COUNT];
static KEY_FIFO_T s_tKey;


static void bsp_InitKeyVar(void);

static uint8_t IsKeyDown0(void) {if((KEY0_GPIO_Port->IDR &KEY0_Pin) == 0)return 1;else return 0;}
static uint8_t IsKeyDown1(void) {if((KEY1_GPIO_Port->IDR &KEY1_Pin) == 0)return 1;else return 0;}





/*
	��ʼ������Ӳ����·
*/
void bsp_InitKey(void)
{
	bsp_InitKeyVar();
}

/*
* ��һ����ֵѹ�밴��FIFO��������������ģ��һ������
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
	�Ӱ���FIFO�ж�ȡһ����ֵ
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
* ��ʼ����������
*/
static void bsp_InitKeyVar(void)
{
	uint8_t i;
	for(i=0;i<KEY_COUNT;i++)
	{
		s_tBtn[i].LongTime = KEY_LONG_TIME;			//����ʱ��0��ʾ����ⳤ�����¼�
		s_tBtn[i].Count = KEY_FILTER_TIME/2;		//����������Ϊ�˲���ʱ���һ��
		s_tBtn[i].State = 0;										//������ʼ״̬ 0��ʾδ����
		s_tBtn[i].RepeatCount = 0;							//����������
		s_tBtn[i].RepeatSpeed = 0;							//�����������ٶȣ�0��ʾ��֧������
	}
	/*
	*�����Ҫ��������ĳ�������Ĳ����������ڴ����¸�ֵ
	*��������Ϊ������0 ���³���1S���Զ��ط���ͬ��ֵ
	*/
//	s_tBtn[KID_K0].LongTime = 100;
//	s_tBtn[KID_K0].RepeatSpeed = 5;
	
	/*
	*�жϰ������µĺ���
	*/
	s_tBtn[0].IsKeyDownFunc = IsKeyDown0;
	s_tBtn[1].IsKeyDownFunc = IsKeyDown1;
}
/*
* ��ȡ����״̬
*/
uint8_t bsp_GetKeyState(KEY_ID_E _ucKEYID)
{
	return s_tBtn[_ucKEYID].State;
}

/*
	*���ð�������
�β� �� -ucKeyID :����ID����0��ʼ
_LongTime:�����¼�ʱ��
_RepeatSpeed:�����ٶ�
*/
void bsp_SetKeyParam(uint8_t _ucKeyID, uint16_t _LongTime, uint8_t  _RepeatSpeed)
{
	s_tBtn[_ucKeyID].LongTime = _LongTime;			// ����ʱ�� 0��ʾ����ⳤ��ʱ��
	s_tBtn[_ucKeyID].RepeatSpeed = _RepeatSpeed;			 //���������ٶȣ�0��ʾ��֧������
	s_tBtn[_ucKeyID].RepeatCount = 0;				   //����������
}

/*
	��հ���FIFO
*/
void bsp_ClearKey(void)
{
	s_tKey.Read = s_tKey.Write;
}



/*
* ��ⰴ���������������������Ե���
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



