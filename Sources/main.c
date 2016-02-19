/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* USER CODE BEGIN Includes */
#include "led_driver.h"
#include "lcd_driver.h"
#include "temp_functions.h"
#include "constants.h"
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef g_AdcHandle;
/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
void ConfigureADC(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

int main(void)
{

  /* USER CODE BEGIN 1 */
  int alarm = 0;
	float CPU_temp = 0.0f;
  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init(); //Init hal drivers

  /* Configure the system clock */
  SystemClock_Config(); //Configure clocks

  /* Initialize all configured peripherals */
  MX_GPIO_Init(); //Init GPIO pins and ports
  ConfigureADC(); //Conigure and init ADC and ADC channels to be used
  HAL_ADC_Start(&g_AdcHandle); //Start adc
  /* USER CODE BEGIN 2 */
	LCD_init(); //Init LCD display
	HAL_Delay(5); //Delay to allow LCD do init properly

	k_filter_init(0.01f, 0.1f, 0.0f, 0.1f, 0.0f); //Init kalman filter

	//Initialize the timing struct values
	tick_counts.display = 0;
	tick_counts.lcd = 0;
	tick_counts.temp = 0;
	tick_counts.alarm = 0;
	tick_counts.test = 0;

  /* USER CODE END 2 */


  while (1)
  {
      /**
       * The while loop uses the tick_counts struct for all timing
       * Every sub system of this project has its own delay value in the constants.h file
       * Every iteration of the while loop, for every subsytems, the current Hal_systick value is compared with
       * the value of the last time the subsytem was updated (stored in the tick_counts struct) if the difference is larger
       * than the delay value in the constants.h file, this system's update subroutine will run.
       */


        //LED display refresh
		if (HAL_GetTick() - tick_counts.display > DISPLAY_REFRESH_DELAY) {
			LED_display();
			tick_counts.display = HAL_GetTick();
		}

        //LCD display update
		if (HAL_GetTick() - tick_counts.lcd > LCD_UPDATE_DELAY) {
			LCD_update();
			tick_counts.lcd = HAL_GetTick();
		}

        //Temperature polling
		if (HAL_GetTick() - tick_counts.temp > TEMP_POLL_DELAY) {
            //Get temp voltage from adc, convert and filter it
			CPU_temp = (k_filter_value(convertVtoC(HAL_ADC_GetValue(&g_AdcHandle))));

            //Set the values for the two displays
			LED_set_value(CPU_temp);
			LCD_set_temp(CPU_temp);

            //If temp above alarm temperature, set alarm on
			if (CPU_temp > ALARM_THRESHOLD) {
				alarm = 1;
			} else {
				alarm = 0;
			}
			tick_counts.temp = HAL_GetTick();
		}

        //If alarm is on and alarm delay for one led is up, swtch to next alarm led
        if (alarm) {
            if (HAL_GetTick() - tick_counts.alarm > ALARM_SWITCH_DELAY) {
                ALARM_switch();
                tick_counts.alarm = HAL_GetTick();
            }
        } else {
            ALARM_off();
        }

  /* USER CODE BEGIN 3 */

  }
  /* USER CODE END 3 */

}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

  __PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_PCLK1
                              |RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);

	//1ms per systick
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

}

void ConfigureADC()
{
		ADC_ChannelConfTypeDef adcChannel;

    __ADC1_CLK_ENABLE();

    // HAL_NVIC_SetPriority(ADC_IRQn, 0, 0);
    // HAL_NVIC_EnableIRQ(ADC_IRQn);

    g_AdcHandle.Instance = ADC1;

    // g_AdcHandle.Init.ClockPrescaler = ADC_CLOCKPRESCALER_PCLK_DIV2;
    g_AdcHandle.Init.Resolution = ADC_RESOLUTION_12B;
    g_AdcHandle.Init.ScanConvMode = DISABLE;
    g_AdcHandle.Init.ContinuousConvMode = ENABLE;
    // g_AdcHandle.Init.DiscontinuousConvMode = DISABLE;
    // g_AdcHandle.Init.NbrOfDiscConversion = 0;
    g_AdcHandle.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    g_AdcHandle.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T1_CC1;
    g_AdcHandle.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    g_AdcHandle.Init.NbrOfConversion = 1;
    // g_AdcHandle.Init.DMAContinuousRequests = ENABLE;
    // g_AdcHandle.Init.EOCSelection = DISABLE;

    HAL_ADC_Init(&g_AdcHandle);
    // HAL_ADC_Start(&g_AdcHandle);

    adcChannel.Channel = ADC_CHANNEL_TEMPSENSOR;
    adcChannel.Rank = 1;
    adcChannel.SamplingTime = ADC_SAMPLETIME_144CYCLES;
    adcChannel.Offset = 0;

    HAL_ADC_ConfigChannel(&g_AdcHandle, &adcChannel);
}


void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __GPIOE_CLK_ENABLE();
  __GPIOD_CLK_ENABLE();
  __GPIOC_CLK_ENABLE();
  //__GPIOH_CLK_ENABLE();
  //__GPIOA_CLK_ENABLE();
  //__GPIOB_CLK_ENABLE();

  /*Configure GPIOE pins */
	///7 segment display + Alarm leds
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIOD pins */
	///LCD data pins
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIOC pins */
	///LCD control pins
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */

/**
  * @}
*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
