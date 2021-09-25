#include "IMU.h"
#include <stdio.h>
#include "cmsis_os.h"
#include "stm32f4xx_hal.h"
#include "cmsis_gcc.h"
//#include "stm32f413xx.h"

int main(void){
    HAL_Init();
    __disable_irq();

    GPIO_InitTypeDef GPIO_Init;
    GPIO_Init.Pin = GPIO_PIN_7;
    GPIO_Init.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_Init.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOE, &GPIO_Init);
    //SystemClock_Config();

    //MX_GPIO_Init();
    while(1){
        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, GPIO_PIN_SET);
        for (volatile int i = 0; i < 1000000; ++i);
        
        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, GPIO_PIN_RESET);
        for (volatile int i = 0; i < 1000000; ++i);

        
    }
}