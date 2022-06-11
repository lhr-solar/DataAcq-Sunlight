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

#define MOUNTCYCLES 500

void DataLoggingTask(void* argument){
    int cntr = 0;

    while (1){
        printf("b");

        if (SDCard_Sort_Write_Data() == FR_OK) cntr++; //increment counter if data was written
        else debugprintf("SDC write error\n\r");
    }
}
