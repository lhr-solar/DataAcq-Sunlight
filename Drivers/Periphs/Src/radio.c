#include "radio.h"
#include <string.h>
//for making the fifo 
#define FIFO_TYPE CANMSG_t
#define FIFO_SIZE 256
#define FIFO_NAME CAN_fifo
#include "fifo.h"
#include "os.h"



// need to first make the fifo - this is the Xqueue 
static QueueHandle_t *xQUEUE; // information will be put on this and all you do is trasmite the date that you receive. 
static ethernet_Fifo info;
static OS_SEM EthernetFifo_Sem4;
static OS_MUTEX EthernetFifo_Mutex;
//static OS_SEM canFifo_Sem4;
//static OS_MUTEX canFifo_Mutex;
//need to figure out this part 
// need to create a init function for the Ethernet - this is where ill work with threads 
int Ethernet_Init(int lsocket){
  MX_LWIP_Init(); // initialize all the things up here - first one is LWIP
 // xQueueCreateCountingSemaphore() - what do i put in the paranthesis need to find tht out 
  struct sockaddr_in sLocalAddr;
	  OS_ERR err;
    CPU_TS ticks;
    xQUEUE= xQueueCreate(sizeEtherFifo, sizeof(ethernet_Fifo)); // creates the xQUEUE with the size of the fifo 
    // create the mutex for ethernet fifo 
    osMutexCreate(&EthernetFifo_Mutex);
    assertOSError(err);
    //make sure that the fifo is created or else it calls an error 
    OSSemCreate(&EthernetFifo_Sem4,
                "Ethernet queue semaphore",
                0,
                &err);
                //create the semaphore for keeping track for ethernet 
              assertOSError(err);
    OSMutexPend(&EthernetFifo_Mutexas, 0, OS_OPT_POST_NONE, &ticks, &err);
    //makes sure that the mutex is avaiable to be usd 
    assertOSError(err);
    CAN_fifo_renew(&canFifo);
    OSMutexPost(&canFifo_Mutex, OS_OPT_POST_NONE, &err);
    assertOSError(err);
    struct sockaddr_in sLocalAddr;
    lSocket = lwip_socket(AF_INET, SOCK_STREAM, 0);
    if(lSocket < 0) return;
  
  memset((char*)&sLocalAddr, 0, sizeof(sLocalAddr));
  sLocalAddr.sin_family = AF_INET;
  sLocalAddr.sin_len    = sizeof(sLocalAddr);
  sLocalAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  sLocalAddr.sin_port   = htons(23);

  if(lwip_bind(lSocket, (struct sockaddr*)&sLocalAddr, sizeof(sLocalAddr)) < 0) {
    lwip_close(lSocket);
    return;
  }
  if(lwip_listen(lSocket, 20) != 0) {
    lwip_close(lSocket);
    return;
  }
    int clientfd;
    struct sockaddr_in client_addr;
    int addrlen = sizeof(client_addr);
    int nbytes;
    //this will establish a connection between the two sockets
    //changed this part of clientfd - if something is wrong here its prob here 
    while(1){
      clientfd = lwip_accept(lSocket, (struct sockaddr*)&client_addr, (socklen_t*)&addrlen);
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
  // if(xQueueReceive(xQUEUE,)) do i need to check if there is something actually in the queue 
  while(1){
    if(xQueueReceive(xQueue,temp,(TickType_t)0)==pdPASS){
      break;
    }
  }
  lwip_send(client, temp, sizeof(temp), 0);
}
void endConnection(int lsocket){
  lwip_close(lsocket);
}