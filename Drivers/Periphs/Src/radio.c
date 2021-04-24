#include "radio.h"

#define CMD_SIZE 64

struct sockaddr_in radioSocket;
int lSocket;

void Radio_Init(void){
  MX_LWIP_Init();
  //initialize socket to TCP protocol with byte streams
  lSocket = lwip_socket(AF_INET, SOCK_STREAM, 0);
  if (lSocket < 0) {/*do something here*/}
  //Initialize Radio Socket address to TCP with IP address 0.0.0.0, port 23
  radioSocket.sin_family = AF_INET;
  radioSocket.sin_len = sizeof(radioSocket);
  radioSocket.sin_addr.s_addr = htonl(INADDR_ANY);
  radioSocket.sin_port = htons(23);
  //bind socket to local address
  if(lwip_bind(lSocket, (struct sockaddr*)&radioSocket, sizeof(radioSocket)) < 0) {
    lwip_close(lSocket);
    return;
  }
  //marks socket to accept connection requests (by radio)
  if(lwip_listen(lSocket, 20) != 0) {
    lwip_close(lSocket);
    return;
  }
  
}

ErrorStatus Radio_RX(void* data){
  
}

ErrorStatus Radio_TX(CANMSG_t data){
  
}
