#include "Tasks.h"
//#include "Drivers/Periphs/Inc/radio.h"
#include "radio.h"

UART_HandleTypeDef huart3;

void BroadcastingTask(void* argument){
  // this one is the while loop one - you put all the while loop things in here
   int lsocket; 
   int client = Ethernet_Init(&lsocket);
   while(1){
    sendMessage(client);
  }
  
}
