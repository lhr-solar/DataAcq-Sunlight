/* Copyright (c) 2020 UT Longhorn Racing Solar */

#ifndef __CANBUS_H
#define __CANBUS_H
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "main.h"
#include "stm32f4xx_hal_can.h"

int RxFlag;
uint8_t RxData[8];
CAN_RxHeaderTypeDef pRxHeader;

typedef enum {
    TRIP = 0x02,
    ALL_CLEAR = 0x101,
    CONTACTOR_STATE = 0x102,
    CURRENT_DATA = 0x103,
    VOLT_DATA = 0x104,
    TEMP_DATA = 0x105,
    SOC_DATA = 0x106,
    WDOG_TRIGGERED = 0x107,
    CAN_ERROR = 0x108
} CANId_t;

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

/**
 * @brief   Initializes the CAN system
 * @param   hcan1 pointer to initialized CAN struct
 * @return  None
 */
void CANbus_Init(CAN_HandleTypeDef *hcan1);

/**
 * @brief   Transmits data onto the CANbus
 * @param   id : CAN id of the message
 * @param   payload : the data that will be sent.
 */
int CANbus_Send(CANId_t id, CANPayload_t payload);

#endif