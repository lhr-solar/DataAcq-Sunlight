#include "radio.h"
#include "sockets.h"
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
<<<<<<< HEAD

    Ethernet_ConnectToServer();

<<<<<<< HEAD
=======
    if (lwip_bind(lsocket, (struct sockaddr *)&sLocalAddr, sizeof(sLocalAddr)) < 0){
        lwip_close(lsocket);
        return ERROR;
    }
    if (lwip_listen(lsocket, 20) != 0){
        lwip_close(lsocket);
<<<<<<< HEAD
=======
        return ERROR;
    }
=======
>>>>>>> sunlight as a client

    Ethernet_ConnectToServer();

    return SUCCESS;
}

/** Ethernet PutInQueue
 * @brief Put data in Ethernet Queue
 * 
 * @param msg Data to place in queue
 * @return BaseType_t - pdTrue if placed, errQUEUE_FULL if full
>>>>>>> added the waiting function for ethernet
 */
BaseType_t Ethernet_PutInQueue(EthernetMSG_t* msg) {
    return xQueueSendToBack(EthernetQ, msg, (TickType_t)0);
}

/** Ethernet Send Message
 * @brief Send data from Ethernet Fifo across ethernet. Blocking: This will
 *        wait until there is data in the queue to send it across
 * 
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
 * @return BaseType_t - pdFalse if Ethernet Queue is empty, pdTrue if Ethernet Queue is not full
 */
BaseType_t Ethernet_SendMessage() {
    EthernetMSG_t eth_rx;
    uint8_t raw_ethmsg[sizeof(EthernetMSG_t)];

    int bytes_sent = 0;
    if (servsocket >= 0) {
        // pull message from queue to send over ethernet
        if (xQueueReceive(EthernetQ, &eth_rx, (TickType_t)0) != pdTRUE) return pdFALSE;
        raw_ethmsg[0] = eth_rx.id;
        raw_ethmsg[1] = eth_rx.length;

        // for eliminating leading undefined bytes in union
        memcpy(&raw_ethmsg[2],
               (char *)&eth_rx.data + sizeof(eth_rx.data) - eth_rx.length, 
               eth_rx.length);

        bytes_sent = lwip_send(servsocket, &raw_ethmsg, eth_rx.length + 2, 0);
        if (bytes_sent < 0) {   // send failed
            bytes_sent = 0;     // reset bytes_sent to 0 to signify error
            Ethernet_ConnectToServer(); // reconnect to server if send failed
        }
    }
=======
 * @return BaseType_t - pdTrue if successful, pdFalse if no message in queue to send
=======
 * @return int - Bytes sent, -1 if Ethernet Queue is empty, 0 if Send failed
>>>>>>> ethernet fixes
=======
 * @return BaseType_t - pdFalse if Ethernet Queue is empty, pdTrue if Ethernet Queue is not full
>>>>>>> Changed Broadcasting task - implemented some changes in radio.h and radio.c because of that
 */
BaseType_t Ethernet_SendMessage() {
    EthernetMSG_t eth_rx;
    uint8_t raw_ethmsg[sizeof(EthernetMSG_t)];

    int bytes_sent = 0;
    if (servsocket >= 0) {
        // pull message from queue to send over ethernet
        if (xQueueReceive(EthernetQ, &eth_rx, (TickType_t)0) != pdTRUE) return pdFALSE;
        raw_ethmsg[0] = eth_rx.id;
        raw_ethmsg[1] = eth_rx.length;

        // for eliminating leading undefined bytes in union
        memcpy(&raw_ethmsg[2],
               (char *)&eth_rx.data + sizeof(eth_rx.data) - eth_rx.length, 
               eth_rx.length);

        bytes_sent = lwip_send(servsocket, &raw_ethmsg, eth_rx.length + 2, 0);
        if (bytes_sent < 0) {   // send failed
            bytes_sent = 0;     // reset bytes_sent to 0 to signify error
            Ethernet_ConnectToServer(); // reconnect to server if send failed
        }
    }
<<<<<<< HEAD
<<<<<<< HEAD
    else return pdFALSE;
>>>>>>> Ethernet working
    return pdTRUE;
=======
    return bytes_sent;
>>>>>>> ethernet fixes
=======
    return pdTRUE;
>>>>>>> Changed Broadcasting task - implemented some changes in radio.h and radio.c because of that
}

/** Ethernet End Connection
 * @brief Close ethernet connection
 */
void Ethernet_EndConnection(){
<<<<<<< HEAD
<<<<<<< HEAD
    if (servsocket >= 0) lwip_close(servsocket);
=======
    lwip_close(lsocket);
>>>>>>> ethernet fixes
=======
    if (servsocket >= 0) lwip_close(servsocket);
>>>>>>> sunlight as a client
}
