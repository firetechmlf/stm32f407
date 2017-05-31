/*
freertos 学习基础任务
使用freertos线程任务打印系统当前运行状态
*/

#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include "serial_debug_task.h"
#include "bsp_device.h"

TIM_HandleTypeDef htim6;


volatile uint32_t ulHighFrequencyTimerTicks = 0UL;

void MX_TIM6_Init(void)
{

  TIM_MasterConfigTypeDef sMasterConfig;

  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 0x1387;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 0x68F;
//	htim6.Init.RepetitionCounter
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
	HAL_TIM_Base_Start_IT(&htim6);
}



void TIM6_DAC_IRQHandler(void)
{
  /* USER CODE BEGIN TIM6_DAC_IRQn 0 */
//	if(__HAL_TIM_GET_FLAG(htim6,TIM_FLAG_UPDATE) != RESET)
//	HAL_GPIO_TogglePin(LED1_GPIO_Port,LED1_Pin);
  /* USER CODE END TIM6_DAC_IRQn 0 */
  HAL_TIM_IRQHandler(&htim6);
	
  /* USER CODE BEGIN TIM6_DAC_IRQn 1 */

  /* USER CODE END TIM6_DAC_IRQn 1 */
}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	/*
		定时器6 普通定时器中断
	*/
	if(htim ==  &htim6)
	{
		ulHighFrequencyTimerTicks++;
		HAL_GPIO_TogglePin(LED1_GPIO_Port,LED1_Pin);
	}
}







static void vTaskTaskUserIF(void *pvP)
{
	uint8_t uckeyCode;
	uint8_t pcWriteBuffer[521];
	while(1)
	{
		uckeyCode = bsp_GetKey();
		if(uckeyCode != KEY_NONE)
		{
			switch(uckeyCode)
			{
				case KEY_0_DOWN:
					printf("====================key0 down=================\r\n");
					printf("任务名	任务状态	优先级	剩余堆栈	任务序号\r\n");
					vTaskList((char *)&pcWriteBuffer);
					printf("%s\r\n",pcWriteBuffer);
					break;
				case KEY_1_DOWN:
					printf("====================key1 down=================\r\n");
					printf("任务名	运行计数	使用率\r\n");
					vTaskGetRunTimeStats((char *)&pcWriteBuffer);
					printf("%s\r\n",pcWriteBuffer);
					break;
			}
		}
		vTaskDelay(20);
	}
}


void vTask_Serial_Debug(void)
{
	xTaskCreate(vTaskTaskUserIF,"userIF",1024,NULL,1,NULL);
}

 // osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
//  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);
