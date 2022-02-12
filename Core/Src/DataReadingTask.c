#include "Tasks.h"
#include "cmsis_os.h"
#include <stdio.h>

void DataReadingTask(void* argument){
    while (1) {
        printf("data reading task\n");
        osDelay(1000);
    }
}
