#ifndef RADIO_H
#define RADIO_H

#include "lwipopts.h"
#include "lwip.h"
#include "tcpip.h"
#include "pbuf.h"
#include "netif.h"
#include "dhcp.h"
#include "stm32f4xx.h"
#include <string.h>
#include "CANbus.h"
#include "sockets.h"
//Data types sent across Ethernet will be similar to CAN data structure so it is easier to understand

//Initialize Ethernet port and buffers for transmitting and receiving data
void Radio_Init(void);
//Allow us to receieve data from ethernet (could be commands)
/*
 * @param data: pointer to memory address that will receive data. Once data format is finalized
 *              the void should change to something else so we know the size of the data.
 * @return ErrorStatus: Success or Error
 */
ErrorStatus Radio_RX(void* data);

/*
 * @param data: Data to be sent across ethernet. Using same format as CAN messages since they
 *              are familiar
 * @return ErrorStatus: Success or Error
 */
ErrorStatus Radio_TX(CANPayload_t data);
#endif
