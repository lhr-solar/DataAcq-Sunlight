#include "Tasks.h"
#include "radio.h"

void BroadcastingTask(void *argument){
    // this one is the while loop one - you put all the while loop things in here
    int lsocket;
    Ethernet_Init(&lsocket); //NOTE: need to handle returned error later
    while (1){
        Ethernet_SendMessage();
    }
}
