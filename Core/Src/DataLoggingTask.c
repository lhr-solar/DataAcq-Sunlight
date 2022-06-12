/**
 * @file DataLoggingTask.c
 * 
 * Task in charge of logging data to the SD card as an offline backup 
 * to the data sent over ethernet to Data Acquisition.
 * Data is added to the logging queue by the Data Reading Task.    
 * @note Assumes that all lower level functions are initialized in main.c     
 * 
 * @copyright Copyright (c) 2022 UT Longhorn Racing Solar
 * 
 * TODO: error check
 */

#include "Tasks.h"
#include "SDCard.h"
#include "config.h"
#include "LED.h"
#include <stdio.h>

void DataLoggingTask(void* argument){
    FRESULT success;
    TickType_t ticks = xTaskGetTickCount();

    while (1){
        success = SDCard_Sort_Write_Data();
        if (success == FR_OK) {
            
        }
        else if (success == SDC_QUEUE_EMPTY) {
            taskYIELD();
        }
        else {
            debugprintf("SDC write error\n\r");
        }

        if ((xTaskGetTickCount() - (ticks / portTICK_PERIOD_MS)) >= SDCARD_SYNC_PERIOD) {
            SDCard_SyncLogFiles();
            ticks = xTaskGetTickCount();
        }
    }
}
