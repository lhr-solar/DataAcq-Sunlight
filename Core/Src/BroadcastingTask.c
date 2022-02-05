#include "Tasks.h"
#include "cmsis_os.h"
#include <stdio.h>

void BroadcastingTask(void* argument){
    while (1) {
        printf("broadcasting task\n\r");
        osDelay(10);
    }
}
