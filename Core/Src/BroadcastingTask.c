#include "Tasks.h"
#include "LWIP/App/lwip.h"
#include "Drivers/Periphs/Inc/radio.h"

UART_HandleTypeDef huart3;

void BroadcastingTask(void* argument){
  // this one is the while loop one - you put all the while loop things in here 
  while(1){
    int clientFD;
    struct sockaddr_in client_addr;
    int addrlen = sizeof(client_addr);
    char buffer[1024];
    int nbytes;
    HAL_UART_Transmit(&huart3, data, strlen(data), 50);
    clientfd = lwip_accept(lSocket, (struct sockaddr*)&client_addr, (socklen_t*)&addrlen);
    if(clientfd > 0) {
      do {
        nbytes = lwip_recv(clientfd, buffer, sizeof(buffer), 0);
        if(nbytes > 0)  {
          lwip_send(clientfd, buffer, nbytes, 0);
        }
      } while(nbytes > 0);
    }


  }
}
