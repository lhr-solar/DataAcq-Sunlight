/**
 * @file BroadcastingTask.c
 * 
 * Task in charge of broadcasting data over ethernet/radio to Data Acquisition.
 * Data is added to the broadcasting queue by the Data Reading Task.  
 * @note Assumes that all lower level functions are initialized in main.c     
 * 
 * @copyright Copyright (c) 2022 UT Longhorn Racing Solar
 * 
 * TODO: error check
 */

#include "Tasks.h"
#include "radio.h"
#include "LED.h"
#include "config.h"
#include <stdio.h>

void BroadcastingTask(void *argument){
    
    Ethernet_Init();
    debugprintf("Ethernet: connected to server\n\r");

    while (1){
        if (Ethernet_SendMessage() == pdFALSE) {
            taskYIELD();
        };
    }
}
