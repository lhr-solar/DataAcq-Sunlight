#include <stdio.h>
#include "cmsis_os.h"
#include "stm32f4xx_hal.h"
#include "main.h"
#include "IMU.h"
/* TEST PROCEDURE:
    1: Make proper connections between main board and IMU (Although the INT and RESET pins are connected, they are not
    used because we do not use low power mode)
    2: Follow steps in Pg. 51 of datasheet for calibration of all 3 sensors
    3: It is a bit difficult to test since you would have to be moving your laptop. Since the magnetometer is also 
    initialized, we will test to see if we get readings from that (So get a fridge magnet and wave it around the 
    IMU and check to see if the data changes). According to the datasheet, it uses the magnetometer to read gravity
    as well so there should be some data already being sent.
    4: If that works, then move the IMU around very quickly. This should change the accelerometer and gyro readings.
    5: According to the data collected, determine if we should initialize the IMU in another mode. (NDOF mode is 
    one to consider. Page 25 of datasheet)
    6: If readings are wrong, use calibration registers to make them more accurate. (Registers 0x55-0x6A)
*/
I2C_HandleTypeDef hi2c1;
UART_HandleTypeDef huart3;

CAN_HandleTypeDef hcan1;

I2C_HandleTypeDef hi2c2;

SPI_HandleTypeDef hspi5;

UART_HandleTypeDef huart1;

HAL_StatusTypeDef error;

//These defines are only necessary for debugging
#define CALIB_STAT 0x35 //This register returns 0xFF if fully calibrated
//If the below register returns a 1 anywhere, that means that it will only send values if the accelerometer
//reads a change in data
#define INT_STA 0x37 //We are not using low power mode, this should return all 0's (no interrupts)
#define SYS_TRIGGER 0x3F //Setting this register to 0x01 starts a self test of the IMU
//400 ms after the test starts, the below register will either remain 0(success) or return 3(failure)
#define SYS_ERROR 0x3A 
#define ST_RESULT 0x36 //This register will show a 0 for whichever device failed the test (pg. 50 of datasheet)
#define OPR_MODE 0x3D

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART3_UART_Init(void);
void StartDefaultTask(void *argument);
void IMUTest(void * argument);

osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 128 * 4
};

osThreadId_t IMUTestHandle;
const osThreadAttr_t IMUTest_attributes = {
  .name = "IMUTest",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 128 * 4
};

int main(void)
{
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_USART3_UART_Init();

  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();

  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);
  IMUTestHandle = osThreadNew(IMUTest, NULL, &IMUTest_attributes);

  /* Start scheduler */
  osKernelStart();

  while (1);
}

void IMUTest(void* argument){
    //code taken from main.c in Core folder
    osDelay(2000);
    IMU_Init();
    IMUData_t Data;  
    
    while (1){
      
        ErrorStatus err = IMU_GetMeasurements(&Data);
        if ((err) == ERROR){
            printf("I2C transmitting or receiving failed.\n\r");
            printf("Consider checking I2C init function or changing device address to backup\n\r");
        }
        printf("ACCEL_DATA_X: %d\n\r", Data.accel_x); //The _write function below was taken from BPS retarget.c
        printf("ACCEL_DATA_Y: %d\n\r", Data.accel_y); 
        printf("ACCEL_DATA_Z: %d\n\r", Data.accel_z); 
        printf("MAG_DATA_X: %d\n\r", Data.mag_x); 
        printf("MAG_DATA_Y: %d\n\r", Data.mag_y); 
        printf("MAG_DATA_Z: %d\n\r", Data.mag_z); 
        printf("GYR_DATA_X: %d\n\r", Data.gyr_x); 
        printf("GYR_DATA_Y: %d\n\r", Data.gyr_y); 
        printf("GYR_DATA_Z: %d\n\r", Data.gyr_z); 

      
        osDelay(2000); //This delays by tick count. Included to prevent too many I2C transmit/receive calls
    }
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void){
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
//}  
    /** Activate the Over-Drive mode
  */
    if (HAL_PWREx_EnableOverDrive() != HAL_OK)
    {
        Error_Handler();
       
    }
    /** Initializes the CPU, AHB and APB buses clocks
  */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
    {
        Error_Handler();
       
    }
}

static void MX_I2C1_Init(void){
    hi2c1.Instance = I2C1;
    hi2c1.Init.ClockSpeed = 100000;
    hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
    hi2c1.Init.OwnAddress1 = 0;
    hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.OwnAddress2 = 0;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    if (HAL_I2C_Init(&hi2c1) != HAL_OK)
        Error_Handler();
    /** Configure Analogue filter
  */
    if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK){
        Error_Handler();
    /** Configure Digital filter
  */

    }
    if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
    {
        Error_Handler();
       

    }
}

static void MX_USART3_UART_Init(void){
    huart3.Instance = USART3;
    huart3.Init.BaudRate = 115200;
    huart3.Init.WordLength = UART_WORDLENGTH_8B;
    huart3.Init.StopBits = UART_STOPBITS_1;
    huart3.Init.Parity = UART_PARITY_NONE;
    huart3.Init.Mode = UART_MODE_TX_RX;
    huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart3.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart3) != HAL_OK){
        Error_Handler();
        
    }
}

static void MX_GPIO_Init(void){
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
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13, GPIO_PIN_RESET);

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);

    /*Configure GPIO pin : PF10 */
    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

    /*Configure GPIO pins : PE7 PE8 PE9 PE10
                           PE11 PE12 PE13 */
    GPIO_InitStruct.Pin = GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13;
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
    GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_11;
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

//int _write(int fd, char *buffer, unsigned int len) {
  //  if(buffer != NULL) {
    //    HAL_UART_Transmit(&huart3,(uint8_t*)buffer, len, 1000);
    //}
    //return len;
//}

void StartDefaultTask(void *argument)
{

  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END 5 */
}

void Error_Handler(void) //TAKEN FROM main.c
{
  
  __disable_irq();
  while (1)
  {
    printf("error");
    HAL_Delay(10);
  }
  
}