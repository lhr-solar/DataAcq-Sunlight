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
//Enum for ID's of all messages that can be sent across CAN bus
typedef enum {
    TRIP = 0x02,
    ALL_CLEAR = 0x101,
    CONTACTOR_STATE = 0x102,
    CURRENT_DATA = 0x103,
    VOLT_DATA = 0x104,
    TEMP_DATA = 0x105,
    SOC_DATA = 0x106,
    WDOG_TRIGGERED = 0x107,
    CAN_ERROR = 0x108,
    MOTOR_DISABLE = 0x10A,
    CHARGE_ENABLE = 0x10C
} CANId_t;

//Union of data that can be sent across CAN bus. Only one field must be filled out
typedef union {
	uint8_t b;
	uint16_t h;
	uint32_t w;
	float f;
} CANData_t;

/**
 * @note    idx is only used when an array of data needs to be sent.
 * @note    data is a union so only one of the fields should be filled out or bad stuff will happen.
 */
typedef struct {
	uint8_t idx : 8;
	CANData_t data;
} CANPayload_t;

/*This data type is used to push messages onto the queue*/
typedef struct {
    CANId_t id;
    CANPayload_t payload;
}CANMSG_t;

//THESE ARE IN HERE FOR NOW BUT SHOULD BE IN CANbus.h !!!!!!!!!!!!!!!!!!!!!!!!

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