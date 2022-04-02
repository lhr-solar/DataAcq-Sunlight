#ifndef RADIO_H
#define RADIO_H

#include "FreeRTOS.h"
#include "sockets.h"
#include "stm32f4xx.h"
#include "queue.h"
#include <stdint.h>
#include <string.h>
#include "CANBus.h"
#include "IMU.h"
#include "GPS.h"

#define ETHERNET_QUEUESIZE 256

typedef enum{
    // this is where you have different types for the different messages that you might have 
    // for instance one for can 
    // one for gps one for imu one for rtc 
    IMU = 0x1,
    GPS = 0x2,
    CAN = 0x3
} EthernetID_t;
typedef union {
    // one need to use on of these - depends on the length of the data 
	CANMSG_t CANData;
    IMUData_t IMUData;
    GPSData_t GPSData;
} EthernetData_t;
typedef struct{
    // this contains the length of the data 
    EthernetID_t id;
    uint8_t length;
	EthernetData_t data; // based on the length you choose how big the ethernetData is 
} EthernetMSG_t;

//NOTE: This driver is created under the assumption only one socket will be open at a time

/** Ethernet Initialize
 * @brief Initialize Ethernet, create queue to hold messages and allocate
 *        socket when connection has been established
 * 
 * @return ErrorStatus ERROR if socket could not be binded to local address
 *                     ERROR if socket did not receive connection request
 *                     SUCCESS if socket was created successfully
 */
ErrorStatus Ethernet_Init(void);

/** Ethernet waitForClient
 * @brief Waits until a client is established - blocking funciton that waits until a client is established
 * 
 */

void Ethernet_WaitForClient();

/** Ethernet Put in queue
 * @brief Put data in Ethernet Queue
 * 
 * @param msg Data to place in queue
 * @return BaseType_t - pdTrue if placed, pdFalse if full
 */
BaseType_t Ethernet_PutInQueue(EthernetMSG_t* msg);

/** Ethernet Send Message
 * @brief Send data from Ethernet Fifo across ethernet.
 */
int Ethernet_SendMessage(void);

/** Ethernet End Connection
 * @brief Close ethernet connection
 * 
 * @param lsocket socket to close connection
 */
void Ethernet_EndConnection(void);

#endif
