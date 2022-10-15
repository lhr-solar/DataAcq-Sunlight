/**
 * @file CANSendingTask.c
 * 
 * Task in charge of sending CAN messages to the rest of the car.
 * Data is added to the sending queue by SOMETHING!!!.  
 * @note Assumes that all lower level functions are initialized in main.c     
 * 
 * @copyright Copyright (c) 2022 UT Longhorn Racing Solar
 * 
 * TODO: error check
 */

#include "Tasks.h"
#include "CANBus.h"
#include "LED.h"
#include "config.h"
#include <stdio.h>

void CANSendingTask(void *argument){
    while (1){
        if (CAN_TransmitMessage() != HAL_OK) {
            taskYIELD();
        };
    }
}
