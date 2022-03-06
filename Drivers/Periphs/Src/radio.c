#include "radio.h"

static QueueHandle_t *EthernetQ; // information will be put on this and all you do is trasmit the date that you receive.
static int clientfd;

/** Ethernet Initialize
 * @brief Initialize Ethernet, create queue to hold messages and allocate
 *        socket when connection has been established
 * 
 * @param lsocket pointer to socket to allocated in ethernet
 * @return ErrorStatus ERROR if socket could not be binded to local address
 *                     ERROR if socket did not receive connection request
 *                     SUCCESS if socket was created successfully
 */
ErrorStatus Ethernet_Init(int *lSocket){

    // MX_LWIP_Init(); initialize all the things up here - first one is LWIP
    struct sockaddr_in sLocalAddr;
    *EthernetQ = xQueueCreate(ETHERNET_SIZE, sizeof(EthernetMSG_t)); // creates the xQUEUE with the size of the fifo
    *lSocket = lwip_socket(AF_INET, SOCK_STREAM, 0);
    if (lSocket < 0)
        return 0;

    memset((char *)&sLocalAddr, 0, sizeof(sLocalAddr));
    sLocalAddr.sin_family = AF_INET;
    sLocalAddr.sin_len = sizeof(sLocalAddr);
    sLocalAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    sLocalAddr.sin_port = htons(23);

    if (lwip_bind(*lSocket, (struct sockaddr *)&sLocalAddr, sizeof(sLocalAddr)) < 0){
        lwip_close(*lSocket);
        return ERROR;
    }
    if (lwip_listen(*lSocket, 20) != 0){
        lwip_close(*lSocket);
        return ERROR;
    }
    struct sockaddr_in client_addr;
    int addrlen = sizeof(client_addr);
    int nbytes;
    char buffer[1024];
    // this will establish a connection between the two sockets
    // changed this part of clientfd - if something is wrong here its prob here
    while (1){
        clientfd = lwip_accept(*lSocket, (struct sockaddr *)&client_addr, (socklen_t *)&addrlen);
        if (clientfd > 0)
            break;
    }
    while (1){
        nbytes = lwip_recv(clientfd, buffer, sizeof(buffer), 0);
        if (nbytes > 0)
            break;
    }
    return SUCCESS;
}

/** Ethernet CollectMessage
 * @brief Put data in Ethernet Queue
 * 
 * @param msg Data to place in queue
 * @return BaseType_t - pdTrue if placed, pdFalse if full
 */
BaseType_t Ethernet_CollectMessage(EthernetMSG_t* msg){
    return xQueueSend(*EthernetQ, msg, 0);
}

/** Ethernet Send Message
 * @brief Send data from Ethernet Fifo across ethernet. Blocking: This will
 *        wait until there is data in the queue to send it across
 */
void Ethernet_SendMessage(void){
    EthernetMSG_t* temp = 0;
    while (xQueueReceive(*EthernetQ, temp, (TickType_t)0) != pdPASS);
    lwip_send(clientfd, temp, sizeof(temp), 0);
}

/** Ethernet End Connection
 * @brief Close ethernet connection
 * 
 * @param lsocket socket to close connection
 */
void Ethernet_EndConnection(int lsocket){
    lwip_close(lsocket);
}
