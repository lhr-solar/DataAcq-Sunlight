#include "Tasks.h"
#include "radio.h"

void BroadcastingTask(void *argument){
    Ethernet_Init();
    while (1){
        BaseType_t status = Ethernet_SendMessage();
        if(status == pdFALSE){
            Ethernet_WaitForClient();
        }
    }
}
