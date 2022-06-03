#include "Tasks.h"
#include "radio.h"

void BroadcastingTask(void *argument){
    Ethernet_QueueInit();
    xSemaphoreTake(InitSem, 0);
    while(uxSemaphoreGetCount(InitSem) != 0);

    // TODO: cleanup radio.c

    // this can take awhile/not work 
    // so it's run after everything else is initialized
    Ethernet_Init();

    while (1){
        Ethernet_SendMessage();
    }
}
