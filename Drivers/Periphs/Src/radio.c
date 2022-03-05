#include "radio.h"
#include <string.h>
//for making the fifo 

// need to first make the fifo - this is the Xqueue 
static QueueHandle_t *xQUEUE; // information will be put on this and all you do is trasmite the date that you receive. 

//need to figure out this part 
// need to create a init function for the Ethernet - this is where ill work with threads 
int Ethernet_Init(int *lSocket){

 // MX_LWIP_Init(); initialize all the things up here - first one is LWIP
 // xQueueCreateCountingSemaphore() - what do i put in the paranthesis need to find tht out 
  struct sockaddr_in sLocalAddr;
    *xQUEUE= xQueueCreate(ETHERNET_SIZE,sizeof(ethernet_Fifo)); // creates the xQUEUE with the size of the fifo 
    // what do i do if there is no space for xqueue - returns null 
    *lSocket = lwip_socket(AF_INET, SOCK_STREAM, 0);
    if(lSocket < 0) return 0;
  
  memset((char*)&sLocalAddr, 0, sizeof(sLocalAddr));
  sLocalAddr.sin_family = AF_INET;
  sLocalAddr.sin_len    = sizeof(sLocalAddr);
  sLocalAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  sLocalAddr.sin_port   = htons(23);

  if(lwip_bind(*lSocket, (struct sockaddr*)&sLocalAddr, sizeof(sLocalAddr)) < 0) {
    lwip_close(*lSocket);
    return 0;
  }
  if(lwip_listen(*lSocket, 20) != 0) {
    lwip_close(*lSocket);
    return 0;
  }
    int clientfd;
    struct sockaddr_in client_addr;
    int addrlen = sizeof(client_addr);
    int nbytes;
    char buffer[1024];
    //this will establish a connection between the two sockets
    //changed this part of clientfd - if something is wrong here its prob here 
    while(1){
      clientfd = lwip_accept(*lSocket, (struct sockaddr*)&client_addr, (socklen_t*)&addrlen);
      if(clientfd>0){
        break;
      }
    } 
     while(1){
      nbytes = lwip_recv(clientfd, buffer, sizeof(buffer), 0);
      if(nbytes>0){
        break;
      }
     }
      //if the conneciton has a value greater than 0 then it means that a connection was established
   return clientfd;

}
void sendMessage(int client){ 
  ethernet_Fifo *temp;
  ethernet_Fifo temp2;
  temp = &temp2;
  // if(xQueueReceive(xQUEUE,)) do i need to check if there is something actually in the queue 
  while(xQueueReceive(*xQUEUE,temp,(TickType_t)0) != pdPASS){}
  lwip_send(client, temp, sizeof(temp), 0);
}
void endConnection(int lsocket){
  lwip_close(lsocket);
}