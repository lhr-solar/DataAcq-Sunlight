/**
 * @file radio.h
 * @brief Ethernet API
 * 
 * @copyright Copyright (c) 2022 UT Longhorn Racing Solar
 * 
 */

#ifndef RADIO_H
#define RADIO_H

#include "FreeRTOS.h"
#include "queue.h"
#include "CANBus.h"
#include "IMU.h"
#include "GPS.h"
#include <stdint.h>

#define ETHERNET_QUEUESIZE 16

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
    uint8_t id; // should be of type EthernetID_t
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

/** Ethernet Queue Initialize
 * @brief Initialize just the ethernet queue. 
 *        Must be called before Ethernet_PutInQueue()
 */
void Ethernet_QueueInit(void);

/** Ethernet Put in queue
 * @brief Put data in Ethernet Queue
 * 
 * @param msg Data to place in queue
 * @return BaseType_t - pdTrue if placed, pdFalse if full
 */
BaseType_t Ethernet_PutInQueue(EthernetMSG_t* msg);

/** Ethernet Send Message
 * @brief Send data from Ethernet Fifo across ethernet.
 * @return BaseType_t - pdFalse if Ethernet Queue is empty, pdTrue if Ethernet Queue is not full
 */
BaseType_t Ethernet_SendMessage(void);

/** Ethernet End Connection
 * @brief Close ethernet connection
 */
void Ethernet_EndConnection(void);

/**
 * @brief Fetch number of dropped Ethernet messages due to queue overfilling.
 *        Included for debug purposes
 * @return Number of dropped messages
 */
uint32_t Ethernet_FetchDroppedMsgCnt();

#endif
