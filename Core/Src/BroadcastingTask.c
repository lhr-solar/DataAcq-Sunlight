#include "Tasks.h"
#include "radio.h"

void BroadcastingTask(void *argument){
    Ethernet_Init();
    while (1){
        Ethernet_SendMessage();
    }
}
