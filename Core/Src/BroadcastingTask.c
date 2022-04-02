#include "Tasks.h"
#include "radio.h"

void BroadcastingTask(void *argument){
    Ethernet_Init();
    while (1){
        BaseType_t killClient = Ethernet_SendMessage();
        if(killClient == pdFALSE){
            Ethernet_WaitForClient();
        }
    }
}
