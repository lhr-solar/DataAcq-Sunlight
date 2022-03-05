#ifndef RADIO_H
#define RADIO_H

#include "FreeRTOS.h"
#include "sockets.h"
#include "stm32f4xx.h"
#include "queue.h"
#include <stdint.h>

#define ETHERNET_SIZE 256
int Ethernet_Init(int *lsocket);
void sendMessage();
void endConnection(int lsocket);
typedef enum{
    // this is where you have different types for the different messages that you might have 
    // for instance one for can 
    // one for gps one for imu one for rtc 
    IMU = 0x1,
    GPS = 0x2,
    CAN = 0x3
} ethernetID;
typedef union {
    // one need to use on of these - depends on the length of the data 
	uint8_t b;
	uint16_t h;
	uint32_t w;
	float f;
} ethernetData;
typedef struct{
    // this contains the length of the data 
    uint8_t length;
	ethernetData data; // based on the length you choose how big the ethernetData is 
} ethernetPacket;
typedef struct{
    ethernetID id; // contains the ID of the ethernet - where the message is from 
    ethernetPacket info; // contains the actual information of the data 
}ethernet_Fifo;


// need to figure out what i should store in the 
// includes CAN axel and gps 



#endif