/**
 * @file BroadcastingTask.c
 * 
 * Task in charge of broadcasting data over ethernet/radio to Data Acquisition.
 * Data is added to the broadcasting queue by the Data Reading Task.       
 * 
 * @copyright Copyright (c) 2022 UT Longhorn Racing Solar
 * 
 * TODO: error check
 */

#include "Tasks.h"
#include "radio.h"
#include "LED.h"
#include <stdio.h>

void BroadcastingTask(void *argument){
    Ethernet_QueueInit();

    #if DEBUGGINGMODE
    printf("Broadcasting Task done initializing...\n\r");
    #endif

    xSemaphoreTake(InitSem, 0);
    while(uxSemaphoreGetCount(InitSem) != 0);
    // this can take awhile/not work 
    // so it's run after everything else is initialized
    Ethernet_Init();

    while (1){
        Ethernet_SendMessage();
    }
}
