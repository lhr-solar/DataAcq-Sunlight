#ifndef CAN_BUS_H
#define CAN_BUS_H

// CAN Drivers

/**
 * Initialize and configure a singular CANBus
 * Currently set up for CAN2
 */

#include "stm32f4xx.h"
#include <stdbool.h>

/**
 * CAN Message structure copied from BPS
 */

// Enum for ID's of all messages that can be sent across CAN bus
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
    CHARGE_ENABLE = 0x10C
} CANId_t;

// Union of data that can be sent across CAN bus. Only one field must be filled out
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

// This data type is used to push messages onto the queue
typedef struct {
    CANId_t id;
    CANPayload_t payload;
} CANMSG_t;

/**
 * Filter and HAL RX_FIFO configurations
 * Set FILTER_MASK_ID_LOW/HIGH to 0 to recieve all can messages
 */
#define CAN_RX_FIFO_NUMBER      CAN_RX_FIFO0
#define FILTER_BANK             14          /* 0 - 13 for CAN1, 14 - 27 for CAN2 */
#define FILTER_ID_LOW           0x0000
#define FILTER_ID_HIGH          0x0000
#define FILTER_MASK_ID_LOW      0x0000
#define FILTER_MASK_ID_HIGH     0x0000

/** CAN Config
 * @brief Initialize CAN, configure CAN filters/interrupts, and start CAN
 * 
 * @param mode CAN_MODE_NORMAL or CAN_MODE_LOOPBACK for operation mode
 * @return HAL_StatusTypeDef - Status of CAN configuration
 */
HAL_StatusTypeDef CAN_Config(CAN_HandleTypeDef *hcan, uint32_t mode);

/** CAN Transmit Message
 * @brief Transmit message over CAN
 * @note This is really basic and does not check for a full transmit Mailbox
 * 
 * @param StdId Message ID (Standard)
 * @param TxData Data to transmit
 * @param len Length of data (Bytes) to transmit (MAX 8B)
 * @return HAL_StatusTypeDef - Status of CAN configuration
 */
HAL_StatusTypeDef CAN_TransmitMessage(
        uint32_t StdId,
        uint8_t *TxData,
        uint8_t len);

/** CAN Retrieve Message
 * @brief Retrieve a message from the CAN Rx software FIFO
 * 
 * @param canmessage CANMSG_t to put message contents in
 * @return HAL_StatusTypeDef - HAL_OK if message was retrieved successfully
 * @return HAL_StatusTypeDef - HAL_ERROR if no messages are present
 */
HAL_StatusTypeDef CAN_RetrieveMessage(CANMSG_t *canmessage);

/** CAN Is Rx Fifo Empty
 * @brief Check if CAN Rx software FIFO is empty
 * 
 * @return true - FIFO is empty
 * @return false - FIFO is not empty
 */
bool CAN_IsRxFifoEmpty();

#endif /* CAN_BUS_H */