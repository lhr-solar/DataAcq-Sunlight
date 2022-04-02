
#include "radio.h"
#include "sockets.h"
#include "queue.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "lwip.h"

static QueueHandle_t EthernetQ; // information will be put on this and all you do is trasmit the date that you receive.
static int clientfd;
static int lsocket;

/** Ethernet Initialize
 * @brief Initialize Ethernet, create queue to hold messages and allocate
 *        socket when connection has been established
 * 
 * @return ErrorStatus ERROR if socket could not be binded to local address
 *                     ERROR if socket did not receive connection request
 *                     SUCCESS if socket was created successfully
 */
ErrorStatus Ethernet_Init() {
    MX_LWIP_Init(); // initialize all the things up here - first one is LWIP
    struct sockaddr_in sLocalAddr;
    EthernetQ = xQueueCreate(ETHERNET_QUEUESIZE, sizeof(EthernetMSG_t)); // creates the xQUEUE with the size of the fifo
    lsocket = lwip_socket(AF_INET, SOCK_STREAM, 0);
    if (lsocket < 0)
        return 0;

    memset((char *)&sLocalAddr, 0, sizeof(sLocalAddr));
    sLocalAddr.sin_family = AF_INET;
    sLocalAddr.sin_len = sizeof(sLocalAddr);
    sLocalAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    sLocalAddr.sin_port = htons(23);

    if (lwip_bind(lsocket, (struct sockaddr *)&sLocalAddr, sizeof(sLocalAddr)) < 0){
        lwip_close(lsocket);
        return ERROR;
    }
    if (lwip_listen(lsocket, 20) != 0){
        lwip_close(lsocket);
        return ERROR;
    }


    return SUCCESS;
}

/** Ethernet waitForClient
 * @brief Waits until a client is established - blocking funciton that waits until a client is established
 */
void Ethernet_WaitForClient(){
    struct sockaddr_in client_addr;
    int addrlen = sizeof(client_addr);
    while (1) {
        clientfd = lwip_accept(lsocket, (struct sockaddr *)&client_addr, (socklen_t *)&addrlen);
        if (clientfd >= 0) break;
    }
}

/** Ethernet PutInQueue
 * @brief Put data in Ethernet Queue
 * 
 * @param msg Data to place in queue
 * @return BaseType_t - pdTrue if placed, errQUEUE_FULL if full
 */
BaseType_t Ethernet_PutInQueue(EthernetMSG_t* msg) {
    return xQueueSendToBack(EthernetQ, msg, (TickType_t)0);
}

/** Ethernet Send Message
 * @brief Send data from Ethernet Fifo across ethernet. Blocking: This will
 *        wait until there is data in the queue to send it across
 * 
 * @return int - Bytes sent, -1 if Ethernet Queue is empty, 0 if Send failed
 */
int Ethernet_SendMessage() {
    EthernetMSG_t eth_rx;

    // pull message from queue to send over ethernet
    if (xQueueReceive(EthernetQ, &eth_rx, (TickType_t)0) != pdTRUE) return -1;

    int bytes_sent = 0;
    if (clientfd >= 0) {
        bytes_sent = lwip_send(clientfd, &eth_rx, sizeof(eth_rx), 0);
        if (bytes_sent < 0) {   // send failed
            clientfd = -1;
            bytes_sent = 0;     // reset bytes_sent to 0 to signify error
        }
    }
    return bytes_sent;
}

/** Ethernet End Connection
 * @brief Close ethernet connection
 */
void Ethernet_EndConnection(){
    lwip_close(lsocket);
}
