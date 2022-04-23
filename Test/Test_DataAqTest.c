/* Copyright (c) 2020 UT Longhorn Racing Solar */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include "FreeRTOS.h"
#include "radio.h"
#include "IMU.h"
#include "GPS.h"


/******************************************************************************
 * CANBus Drivers Test
 * * This test utilizes two RTOS threads to transmit and recieve CAN messages.
 * * The main focus is to verify driver functionality, so the RTOS part is not
 *   very in-depth.
 * 
 * * Test procedure:
 * 1. Serial output should read:
 *        "CAN message (word): 0ABCDEF0"
 *        (note: you may want to add carriage returns to the print statements)
 * 2. The char tx[4] array can be changed to different content to observe
 *    matching outputs over serial.
 * 3. Transmit IDs (defined when calling CAN_TransmitMessage) can be set to
 *    messages with different formats, but the tx array and output printf()
 *    will need to be modified for the different message formats.
 *
 *****************************************************************************/

CAN_HandleTypeDef hcan1;
UART_HandleTypeDef huart3;

/* Definitions for threads */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 128 * 4
};
osThreadId_t TransmitTaskHandle;
osThreadAttr_t TransmitTask_attributes = {
  .name = "Transmit Task",
  .priority = (osPriority_t) osPriorityHigh, //Will determine priorities later
  .stack_size = 1024 //arbitrary value might need to make it larger or smaller
};

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART3_UART_Init(void);
void StartDefaultTask(void *argument);


/* Test Threads --------------------------------------------------------------*/
void TransmitTask(void* argument) {
    printf("initializing ethernet...\n");
    ErrorStatus initstatus = Ethernet_Init();


    if (initstatus != SUCCESS) {
      printf("Initialization Error\n");
      Error_Handler();
    }


     EthernetMSG_t Message;

     //error = IMU_GetMeasurements(IMU_Data);

    //  //IMU data
    //  Message.id= IMU;
    //  Message.length = sizeof(IMU_Data);  // TODO: Ensure this is correct
    //  Message.data.IMUData = IMU_Data; // TODO: Make sure 

    //  //GPS Data
    //  Message.id= GPS;
    //  Message.length = sizeof(GPS_Data);  // TODO: Ensure this is correct
    //  Message.data.GPSData = GPS_Data;

    //  //CAN Data
    //  Message.id= CAN;
    //  Message.length = sizeof(CAN_Data);  // TODO: Ensure this is correct
    //  Message.data.GPSData = CAN_Data;

    IMUData_t IMU_Data ={1, 2, 3, 4, 5, 6, 7, 8, 9};
    // GPSData_t GPS_Data = { {'0', '3'}, {'2', '3'}, {'1', '2'},{'2', '2', '2'}
    //                , {'2', '0'}, {'4', '5', '6', '6', '1', '4'}, '6'
    //                , {'6', '1', '4'}, {'6', '3', '7', '4', '3', '7'}, 'N'
    //                , {'3', '4', '5', '6'}, {'7', '8'},{'9', '8'}
    //                , {'1', '2', '3', '4'}, {'5', '6', '7', '8'}, 'N'};
    GPSData_t GPS_Data;
    GPS_Data.hr[2] = {'0','3'}; // Will not use these parameters unless we have to
    GPS_Data.min[] = '12'; // ^^
    GPS_Data.sec[] = '67'; // ^^
    GPS_Data.ms = '678'; // ^^
    GPS_Data.latitude_Deg = '12';
    GPS_Data.latitude_Min = '123.56';
    GPS_Data.NorthSouth ='N' ;
    GPS_Data.longitude_Deg = '456';
    GPS_Data.longitude_Min = '123456';
    GPS_Data.EastWest ='W' ;
    GPS_Data.speedInKnots ='4567' ;
    GPS_Data.day = '34'; // Will not use these parameters unless we have to
    GPS_Data.month = '45'; // ^^
    GPS_Data.year = '2022'; // ^^
    GPS_Data.magneticVariation_Deg = '45.5';
    GPS_Data.magneticVariation_EastWest ="W" ;

    char gpsData[] = "032312222204566146614637437N3456789812345678N";
    memcpy(&Message.data.GPSData, gpsData, sizeof(gpsData));
 
    CANMSG_t CAN_Data;
    CANData_t data;
    data.w = 0xFFFFFFFF;
    CANPayload_t payload = {0xE, data};
 
    CAN_Data.id = 0x02;
    CAN_Data.payload = payload;


     
    BaseType_t status;
    
    

    //memset(&testmessage, 0, sizeof(testmessageGPS));
    //testmessageGPS.id = GPS;
    //testmessageGPS.length = sizeof(testmessageGPS.data.GPSData);
    //char teststring[] = "zyxwvutsrqponmlkjihgfedcba\n";
    //memcpy(&testmessage.data.GPSData, teststring, sizeof(teststring));
    int8_t x=1;

    while (1){
        printf("Beginning of while loop\n");

        //IMU data
    
        if(x%3==1){
        Message.id= IMU;
        Message.length = sizeof(IMU_Data);
        //memcpy(&Message.data.IMUData, teststring, sizeof(teststring));
        Message.data.IMUData = IMU_Data;
        x++;
        }

    
        else if(x%3==2){
        //GPS Data
        Message.id= GPS;
        memcpy(&Message.data.GPSData, gpsData, sizeof(gpsData));
        Message.length = sizeof(gpsData);  
        x++;
        }

        else if(x%3==0){
        //CAN Data
        Message.id= CAN;
        Message.length = sizeof(CAN_Data); 
        Message.data.CANData = CAN_Data;
        x++;
        }

        status = Ethernet_PutInQueue(&Message);

        if (status != pdTRUE) printf("PutInQueue error\n");
    
        printf("Sending message now\n");

        if (Ethernet_SendMessage()) printf("Send message error");

        if(x==3)
        {
            x=1;
        }
        osDelay(1000);
    }

}

/* End Test Threads -----------------------------------------------------------*/




/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART3_UART_Init();

  /* Init scheduler */
  printf("initializing os");
  osKernelInitialize();
  printf("...\n");

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);
  /* USER CODE BEGIN RTOS_THREADS */
  TransmitTaskHandle = osThreadNew(TransmitTask, NULL, &TransmitTask_attributes);
  /* USER CODE END RTOS_THREADS */

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  while (1)
  {

  }
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 180;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}


/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10
                          |GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);

  /*Configure GPIO pin : PF10 */
  GPIO_InitStruct.Pin = GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  /*Configure GPIO pins : PE7 PE8 PE9 PE10
                           PE11 PE12 PE13 */
  GPIO_InitStruct.Pin = GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10
                          |GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pin : PB12 */
  GPIO_InitStruct.Pin = GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PA8 PA11 */
  GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PB4 */
  GPIO_InitStruct.Pin = GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PB5 */
  GPIO_InitStruct.Pin = GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END 5 */
}

 /**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM1 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM1) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
    // TODO: Create a proper Error Handler
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
