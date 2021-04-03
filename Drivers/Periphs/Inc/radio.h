#ifndef RADIO_H
#define RADIO_H

#include "ethernetif.h"
#include "tcpip.h"
#include "pbuf.h"
#include "netif.h"
#include "dhcp.h"
#include "stm32f4xx.h"
#include <string.h>
//Data types sent across Ethernet will be similar to CAN data structure so it is easier to understand

//Initialize Ethernet port and buffers for transmitting and receiving data
void radio_Init(void);
//Allow us to receieve data from ethernet (could be commands)
/*
 * @param data: pointer to memory address that will receive data. Once data format is finalized
 *              the void should change to something else so we know the size of the data.
 * @return ErrorStatus: Success or Error
 */
ErrorStatus radio_RX(void* data);

/*
 * @param data: Data to be sent across ethernet. Using same format as CAN messages since they
 *              are familiar
 * @return ErrorStatus: Success or Error
 */
ErrorStatus radio_TX(CANMSG_t data);
#endif
