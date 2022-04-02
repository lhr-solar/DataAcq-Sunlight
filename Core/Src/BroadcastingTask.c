#include "Tasks.h"
#include "radio.h"

void BroadcastingTask(void *argument){
    // this one is the while loop one - you put all the while loop things in here
    Ethernet_Init(); //NOTE: need to handle returned error later
    Ethernet_WaitForClient();
    while (1){
        BaseType_t killClient = Ethernet_SendMessage();
        if(killClient == pdFALSE){
            Ethernet_WaitForClient();
        }
    }
}
