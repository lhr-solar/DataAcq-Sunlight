/**
 * @file DataLoggingTask.c
 * 
 * Task in charge of logging data to the SD card as an offline backup 
 * to the data sent over ethernet to Data Acquisition.
 * Data is added to the logging queue by the Data Reading Task.       
 * 
 * @copyright Copyright (c) 2022 UT Longhorn Racing Solar
 * 
 * TODO: error check
 */

#include "Tasks.h"
#include "SDCard.h"
#include "config.h"
#include <stdio.h>

#define MOUNTCYCLES 500

void DataLoggingTask(void* argument){
    int cntr = 0;
    if (SDCard_Init() != FR_OK);

    #if DEBUGGINGMODE
    printf("Data Logging Task done initializing...\n\r");
    #endif

    xSemaphoreTake(InitSem, 0);
    while(uxSemaphoreGetCount(InitSem) != 0);

    while (1){
        if (SDCard_Sort_Write_Data() == FR_OK) cntr++; //increment counter if data was written
        if (cntr > MOUNTCYCLES) {
            if (SDCard_CloseFileSystem() != FR_OK);
            cntr = 0;
            if (SDCard_OpenFileSystem() != FR_OK);
        }
    }
}
