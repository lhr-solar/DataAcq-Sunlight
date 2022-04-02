#include "Tasks.h"
#include "radio.h"

void BroadcastingTask(void *argument){
    int lsocket;
    Ethernet_Init(&lsocket); //NOTE: need to handle returned error later
    while (1){
        Ethernet_SendMessage();
    }
}
