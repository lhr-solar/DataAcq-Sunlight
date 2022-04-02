#include "Tasks.h"
#include "radio.h"

void BroadcastingTask(void *argument){
    Ethernet_Init();
    Ethernet_ConnectToServer();
    while (1){
        BaseType_t status = Ethernet_SendMessage();
        if(status == pdFALSE){
            Ethernet_ConnectToServer();
        }
    }
}
