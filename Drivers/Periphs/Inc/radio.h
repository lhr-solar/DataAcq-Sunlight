#ifndef RADIO_H
#define RADIO_H

#include "FreeRTOS.h"
<<<<<<< HEAD
#include "sockets.h"
#include "stm32f4xx.h"
#include "queue.h"
<<<<<<< HEAD
<<<<<<< HEAD
#include <stdint.h>
#include <string.h>
=======
>>>>>>> Ethernet working
#include "CANBus.h"
<<<<<<< HEAD
#include "lwip.h"
#include "IMU.h"
#include "GPS.h"

<<<<<<< HEAD
#define ETHERNET_QUEUESIZE 32

=======
#include "os.h"
=======
#include <stdint.h>
>>>>>>> changes to radio.c,radio.h, and braodcastingTask.c

#define ETHERNET_SIZE 256
int Ethernet_Init(int *lsocket);
void sendMessage();
void endConnection(int lsocket);
>>>>>>> finished radio.c radio.h and broadcasting - need to test and debug
=======
#include "IMU.h"
#include "GPS.h"
=======
#define ETHERNET_SIZE 32
>>>>>>> Ethernet working

#define ETHERNET_SIZE 256

>>>>>>> Added function to put data in queue, added comments for documentation, fixed function names
typedef enum{
    // this is where you have different types for the different messages that you might have 
    // for instance one for can 
    // one for gps one for imu one for rtc 
    IMU = 0x1,
    GPS = 0x2,
    CAN = 0x3
<<<<<<< HEAD
<<<<<<< HEAD
} EthernetID_t;
=======
} ethernetID;
>>>>>>> changes to radio.c,radio.h, and braodcastingTask.c
=======
} EthernetID_t;
>>>>>>> Added function to put data in queue, added comments for documentation, fixed function names
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
<<<<<<< HEAD
<<<<<<< HEAD
=======
 * @param lsocket pointer to socket to allocated in ethernet
>>>>>>> Added function to put data in queue, added comments for documentation, fixed function names
=======
>>>>>>> Ethernet working
 * @return ErrorStatus ERROR if socket could not be binded to local address
 *                     ERROR if socket did not receive connection request
 *                     SUCCESS if socket was created successfully
 */
<<<<<<< HEAD
<<<<<<< HEAD
ErrorStatus Ethernet_Init(void);

/** Ethernet Put in queue
=======
ErrorStatus Ethernet_Init(int *lsocket);
=======
ErrorStatus Ethernet_Init();
>>>>>>> Ethernet working

/** Ethernet CollectMessage
>>>>>>> Added function to put data in queue, added comments for documentation, fixed function names
 * @brief Put data in Ethernet Queue
 * 
 * @param msg Data to place in queue
 * @return BaseType_t - pdTrue if placed, pdFalse if full
 */
<<<<<<< HEAD
<<<<<<< HEAD
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
=======
BaseType_t Ethernet_CollectMessage(EthernetMSG_t* msg);
=======
BaseType_t Ethernet_PutInQueue(EthernetMSG_t* msg);
>>>>>>> made a new file for testing ethernet ; made changes to radio.c and radio.h

/** Ethernet Send Message
 * @brief Send data from Ethernet Fifo across ethernet. Blocking: This will
 *        wait until there is data in the queue to send it across
 * 
 * @return BaseType_t - pdTrue if successful, pdFalse if no message in queue to send
 */
BaseType_t Ethernet_SendMessage();

/** Ethernet End Connection
 * @brief Close ethernet connection
 * 
 * @param lsocket socket to close connection
 */
void Ethernet_EndConnection(int lsocket);
>>>>>>> Added function to put data in queue, added comments for documentation, fixed function names

#endif
