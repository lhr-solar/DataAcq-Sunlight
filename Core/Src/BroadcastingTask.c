#include "Tasks.h"
#include "radio.h"

void BroadcastingTask(void *argument){
<<<<<<< HEAD
    // this one is the while loop one - you put all the while loop things in here
    Ethernet_Init(); //NOTE: need to handle returned error later
    Ethernet_WaitForClient();
=======
    int lsocket;
    Ethernet_Init(&lsocket); //NOTE: need to handle returned error later
>>>>>>> 1f5db8790442b943f58beae4ba34b3f9081c5c9b
    while (1){
        BaseType_t killClient = Ethernet_SendMessage();
        if(killClient == pdFALSE){
            Ethernet_WaitForClient();
        }
    }
}
