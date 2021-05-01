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
  int addr, nbytes;
  char buffer[1024];
  struct sockaddr_in client_addr;
  socklen_t size = sizeof(client_addr);
  int error = lwip_connect(lSocket, (struct sockaddr*)&radioSocket, sizeof(radioSocket)); //make connection
  if (error < 0){return ERROR;/*DO SOMETHING HERE*/} //This means it couldn't connect
  addr = lwip_accept(lSocket, (struct sockaddr*)&client_addr, &size);
  if(addr > 0) { //We will probably be receiving commands from pit crew, so input buffer will be char
    do {
      nbytes = lwip_recv(addr, buffer, sizeof(buffer), 0);
    } while(nbytes > 0);
  }
  if (addr < 0) {return ERROR;/*DO SOMETHING HERE*/} //I think this means nothing sent an acception request
  data = buffer;
  lwip_close(lSocket);
  return SUCCESS;
}

ErrorStatus Radio_TX(CANPayload_t data){
  struct sockaddr_in client_addr;
  socklen_t size = sizeof(client_addr);
  int error = lwip_connect(lSocket, (struct sockaddr*)&radioSocket, sizeof(radioSocket)); //make connection
  if (error < 0){return ERROR;/*DO SOMETHING HERE*/} //This means it couldn't connect
  int addr = lwip_accept(lSocket, (struct sockaddr*)&client_addr, &size);
  error = lwip_send(addr, &data, sizeof(CANPayload_t), 0);
  if (error < 0){return ERROR;/*DO SOMETHING HERE*/} //This means it didn't send
  lwip_close(lSocket);
  return SUCCESS;
}
