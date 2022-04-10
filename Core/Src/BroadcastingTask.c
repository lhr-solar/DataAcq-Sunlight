#include "Tasks.h"
#include "radio.h"

void BroadcastingTask(void *argument){
<<<<<<< HEAD
    int lsocket;
    Ethernet_Init(&lsocket); //NOTE: need to handle returned error later
=======
    Ethernet_Init();
>>>>>>> main
    while (1){
        Ethernet_SendMessage();
    }
}
