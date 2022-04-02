
#include "radio.h"
#include "sockets.h"
#include "queue.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "lwip.h"
#include "config.h"

static QueueHandle_t EthernetQ; // information will be put on this and all you do is trasmit the date that you receive.
static struct sockaddr_in sLocalAddr;
static int servsocket;

/** Ethernet ConnectToServer
 * @brief Waits until server connection is established - blocking
 */
static void Ethernet_ConnectToServer() {
    if (servsocket < 0) {
        do {
            servsocket = lwip_socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        } while (servsocket < 0);
        printf("servsocket %d\n", servsocket);
        while (lwip_connect(servsocket, (struct sockaddr *)&sLocalAddr, sizeof(sLocalAddr)) < 0);
        printf("done\n");
    }
}

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
    EthernetQ = xQueueCreate(ETHERNET_QUEUESIZE, sizeof(EthernetMSG_t)); // creates the xQUEUE with the size of the fifo
    servsocket = -1;

    memset((char *)&sLocalAddr, 0, sizeof(sLocalAddr));
    sLocalAddr.sin_family = AF_INET;
    sLocalAddr.sin_len = sizeof(sLocalAddr);
    sLocalAddr.sin_addr.s_addr = htonl(lwip_makeu32_func(IP4_SERVER_ADDRESS));
    sLocalAddr.sin_port = htons(SERVER_PORT);

    Ethernet_ConnectToServer();

    return SUCCESS;
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
 * @return BaseType_t - pdFalse if Ethernet Queue is empty, pdTrue if Ethernet Queue is not full
 */
BaseType_t Ethernet_SendMessage() {
    EthernetMSG_t eth_rx;

    int bytes_sent = 0;
    if (servsocket >= 0) {
        // pull message from queue to send over ethernet
        if (xQueueReceive(EthernetQ, &eth_rx, (TickType_t)0) != pdTRUE) return pdFALSE;

        bytes_sent = lwip_send(servsocket, &eth_rx, sizeof(eth_rx), 0);
        if (bytes_sent < 0) {   // send failed
            bytes_sent = 0;     // reset bytes_sent to 0 to signify error
            Ethernet_ConnectToServer(); // reconnect to server if send failed
        }
    }
    return pdTRUE;
}

/** Ethernet End Connection
 * @brief Close ethernet connection
 */
void Ethernet_EndConnection(){
    if (servsocket >= 0) lwip_close(servsocket);
}
