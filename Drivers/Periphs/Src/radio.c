/**
 * @file radio.c
 * @brief Ethernet API
 * 
 * @copyright Copyright (c) 2022 UT Longhorn Racing Solar
 * 
 */

#include "radio.h"
#include "sockets.h"
#include "lwip.h"
#include "main.h"
#include "config.h"
#include "LED.h"
#include <string.h>

static QueueHandle_t EthernetQ; // information will be put on this and all you do is trasmit the date that you receive.
static struct sockaddr_in sLocalAddr;
static struct linger soLinger = {.l_onoff = true, .l_linger = 0};
static int servsocket;
static uint32_t EthDroppedMessages = 0;    // for debugging purposes
extern int errno;

/** Ethernet ConnectToServer
 * @brief Waits until server connection is established - blocking
 */
static void Ethernet_ConnectToServer() {
    while (servsocket < 0) {
        do {
            servsocket = lwip_socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        } while (servsocket < 0);

        debugprintf("servsocket %d\n\r", servsocket);

        // set linger to 0 - this makes sure closed sockets are freed immediately
        lwip_setsockopt(servsocket, SOL_SOCKET, SO_LINGER, &soLinger, sizeof(soLinger));

        if (lwip_connect(servsocket, (struct sockaddr *)&sLocalAddr, sizeof(sLocalAddr)) < 0) {
            Ethernet_EndConnection();
        }
    }
    debugprintf("Ethernet connected\n\r");
    LED_On(ETH_CONNECT);
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
    servsocket = -1;

    memset((char *)&sLocalAddr, 0, sizeof(sLocalAddr));
    sLocalAddr.sin_family = AF_INET;
    sLocalAddr.sin_len = sizeof(sLocalAddr);
    sLocalAddr.sin_addr.s_addr = htonl(lwip_makeu32_func(IP4_SERVER_ADDRESS));
    sLocalAddr.sin_port = htons(SERVER_PORT);

    Ethernet_ConnectToServer();

    return SUCCESS;
}

/** Ethernet Queue Initialize
 * @brief Initialize just the ethernet queue. 
 *        Must be called before Ethernet_PutInQueue()
 */
void Ethernet_QueueInit() {
    EthernetQ = xQueueCreate(ETHERNET_QUEUESIZE, sizeof(EthernetMSG_t)); // creates the xQUEUE with the size of the fifo
}

/** Ethernet PutInQueue
 * @brief Put data in Ethernet Queue
 * 
 * @param msg Data to place in queue
 * @return BaseType_t - pdTrue if placed, errQUEUE_FULL if full
 */
BaseType_t Ethernet_PutInQueue(EthernetMSG_t* msg) {
    BaseType_t success = xQueueSendToBack(EthernetQ, msg, (TickType_t)0);
    if (success == errQUEUE_FULL) {
        EthDroppedMessages++;
    }
    return success;
}

/** Ethernet Send Message
 * @brief Send data from Ethernet Fifo across ethernet. Blocking: This will
 *        wait until there is a valid connection to the server
 * 
 * @return BaseType_t - pdFalse if Ethernet Queue is empty, pdTrue if Ethernet Queue is not full
 */
BaseType_t Ethernet_SendMessage() {
    EthernetMSG_t eth_rx;
    uint8_t raw_ethmsg[sizeof(EthernetMSG_t)];

    int bytes_sent = 0;
    if (servsocket >= 0) {
        if (errno != 0) {
            Ethernet_EndConnection();
        }

        // pull message from queue to send over ethernet
        if (xQueueReceive(EthernetQ, &eth_rx, (TickType_t)0) != pdTRUE) return pdFALSE;
        raw_ethmsg[0] = eth_rx.id;
        raw_ethmsg[1] = eth_rx.length;
        // copy data from dataptr into raw ethernet message array
        // the struct word-aligns the first two bytes (id and length), so this is necessary
        memcpy(&raw_ethmsg[2],
               &eth_rx.data, 
               eth_rx.length);

        bytes_sent = lwip_send(servsocket, &raw_ethmsg, eth_rx.length + 2, MSG_DONTWAIT);
        if (bytes_sent < 0 || errno != 0) {   // send failed
            Ethernet_EndConnection();
        }
    }
    else {
        Ethernet_ConnectToServer(); // reconnect to server if send previously failed
    }
    return pdTRUE;
}

/** Ethernet End Connection
 * @brief Close ethernet connection
 */
void Ethernet_EndConnection() {
    if (servsocket >= 0) {
        lwip_close(servsocket);
        servsocket = -1;
        debugprintf("Ethernet Disconnected\n\r");
        LED_Off(ETH_CONNECT);
    }
}

/**
 * @brief Fetch number of dropped Ethernet messages due to queue overfilling.
 *        Included for debug purposes
 * @return Number of dropped messages
 */
uint32_t Ethernet_FetchDroppedMsgCnt() {
    return EthDroppedMessages;
}
