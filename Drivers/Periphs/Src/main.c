#include <stdio.h> 
#include "CANBus.h"
#include "stm32f4xx_hal_can.h"
#include "stm32f4xx_hal_gpio.h"

int main() { 
    HAL_Init(); 
    SystemClock_Config(); 
    MX_GPIO_Init(); 
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin = GPIO_PIN_7;  
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; 
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM; 
    GPIO_InitStruct.Pull = GPIO_PULLDOWN; 
    HAL_GPIO_Init(GPIOE, GPIO_InitStruct); 

    printf("HEllO WORLD!"); 

    while (1) { 
        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, GPIO_PIN_SET); 
        HAL_Delay(1000); 
        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, GPIO_PIN_RESET); 
        HAL_Delay(1000); 
    }

    int correct_array[4] = {1, 2, 3, 4}; 
    int empty_array[4] = {}; 
    floatTo4Bytes(1.234, empty_array);  
    for (int i = 0; i < 4; i++) { 
        if (correct_array[i] != empty_array[i]) { 
            //turn on an LED or something idk 
            printf("These don't match (float to 4 bytes)"); 
        }
    }

    for (int j = 0; j < 4; j++) { 
        correct_array[j] = 0; 
        empty_array[j] = 0; 
    }
    floatTo4Bytes(0, empty_array); 
    for (int i = 0; i < 4; i++) { 
        if (correct_array[i] != empty_array[i]) { 
            //turn on an LED or something idk 
            printf("These don't match (float to 4 bytes)"); 
        }
    }
    //these first two will fail, will work once array/code is fixed 

    for (int j = 0; j < 4; j++) { 
        if (j == 0) {  
            correct_array[j] = 1; 
        }
        empty_array[j] = 0; 
    }
    floatTo4Bytes(1, empty_array); //expecting an int
    for (int i = 0; i < 4; i++) { 
        if (correct_array[i] != empty_array[i]) { 
            //turn on an LED or something idk 
            printf("These don't match (float to 4 bytes)"); 
        }
    }
    //this one should work though. 


    //first test CANBUS_INIT with CANbus_Send
    CAN_HandleTypeDef hcan_test; 
    CAN_TypeDef Instance; 
    hcan_test->Instance = Instance; 
    hcan_test->Init = ENABLE; 
    hcan_test->State= HAL_CAN_STATE_READY; //starting state should be ready
    hcan_test->ErrorCode = 5; 
    CANbus_Init(&hcan_test); 

    int *data = 255; 
    CANbus_Read(5, data); 
    
    while (1) { 
        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, GPIO_PIN_SET); 
        HAL_Delay(100); 

        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, GPIO_PIN_RESET); 
        HAL_Delay(100); 
    }

    return 0; 
}