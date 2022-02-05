#include "Tasks.h"
#include "cmsis_os.h"
#include <stdio.h>

void DataLoggingTask(void* argument){
    while (1) {
        printf("data logging task\n\r");
        osDelay(10);
    }
}
