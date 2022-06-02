// CAN Drivers

#include "CANBus.h"
#include "main.h"
#include <string.h>
#include <stdio.h>

#define CAN_QUEUESIZE  32
/**
 * @brief Data structures needed for HAL CAN operation
 */
static CAN_RxHeaderTypeDef RxHeader;
static uint8_t RxData[8];
static uint32_t TxMailbox;
static QueueHandle_t RxQueue;
uint32_t DroppedMessages = 0;   // for debugging purposes


/** CAN Recieve
 * @brief Convert a raw CAN message to CANMSG_t and add to the RxFifo
 * 
 * @param header RxHeader from CAN message
 * @param data RxData from CAN message
 * @return HAL_StatusTypeDef - HAL_OK if message was parsed correctly
 * @return HAL_StatusTypeDef - HAL_ERROR if message ID does not match known IDs
 */
static HAL_StatusTypeDef CAN_Recieve(CAN_RxHeaderTypeDef *rx_header, uint8_t *rx_data) {
    CANMSG_t canmessage;
    canmessage.id = rx_header->StdId;

    switch (canmessage.id) {
    // Handle messages with one byte of data
    case TRIP:
    case ALL_CLEAR:
    case CONTACTOR_STATE:
    case WDOG_TRIGGERED:
    case CAN_ERROR:
    case CHARGE_ENABLE:
        memcpy(
            &(canmessage.payload.data.b),
            rx_data,
            sizeof(canmessage.payload.data.b));
        break;

    // Handle messages with 4 byte data
    case CURRENT_DATA:
    case SOC_DATA:
        memcpy(
            &(canmessage.payload.data.w),
            rx_data,
            sizeof(canmessage.payload.data.w));
        break;

    // Handle messages with idx + 4 byte data
    case VOLT_DATA:
    case TEMP_DATA:
        canmessage.payload.idx = rx_data[0];
        memcpy(
            &(canmessage.payload.data.w),
            &(rx_data[1]),
            sizeof(canmessage.payload.data.w));
        break;

    // Handle invalid messages
    default:
        return HAL_ERROR;	// Do nothing if invalid
    }

    // Add message to FIFO
    if (xQueueSendToBackFromISR(RxQueue, &canmessage, NULL) == errQUEUE_FULL) {
        DroppedMessages++;
        return HAL_ERROR;
    }
    return HAL_OK;
}

/** MX CAN1 Init
  * @brief CAN1 Initialization Function
  * @note Generated by STMCube
  * 
  * @param mode CAN_MODE_NORMAL or CAN_MODE_LOOPBACK for operation mode
  * @return HAL_StatusTypeDef - Status of CAN initialization
  */
static HAL_StatusTypeDef MX_CAN1_Init(uint32_t mode) {
    hcan1.Instance = CAN1;
    hcan1.Init.Prescaler = 45;
    hcan1.Init.Mode = mode;
    hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
    hcan1.Init.TimeSeg1 = CAN_BS1_3TQ;
    hcan1.Init.TimeSeg2 = CAN_BS2_4TQ;
    hcan1.Init.TimeTriggeredMode = DISABLE;
    hcan1.Init.AutoBusOff = DISABLE;
    hcan1.Init.AutoWakeUp = DISABLE;
    hcan1.Init.AutoRetransmission = DISABLE;
    hcan1.Init.ReceiveFifoLocked = DISABLE;
    hcan1.Init.TransmitFifoPriority = DISABLE;

    return HAL_CAN_Init(&hcan1);
}

/** CAN1 Initialization
 * @brief Initialize CAN1 queue, configure CAN filters/interrupts, and start CAN
 * 
 * @param mode CAN_MODE_NORMAL or CAN_MODE_LOOPBACK for operation mode
 * @return HAL_StatusTypeDef - Status of CAN configuration
 */
HAL_StatusTypeDef CAN_Init(uint32_t mode) {
    RxQueue = xQueueCreate(CAN_QUEUESIZE, sizeof(CANMSG_t)); // creates the xQUEUE with the size of the fifo
    HAL_StatusTypeDef configstatus = MX_CAN1_Init(mode);
    if (configstatus != HAL_OK) return configstatus;

    CAN_FilterTypeDef filterconfig;
    filterconfig.FilterBank = FILTER_BANK;
    filterconfig.FilterMode = CAN_FILTERMODE_IDMASK;
    filterconfig.FilterScale = CAN_FILTERSCALE_32BIT;
    filterconfig.FilterIdLow = FILTER_ID_LOW;
    filterconfig.FilterIdHigh = FILTER_ID_HIGH;
    filterconfig.FilterMaskIdLow = FILTER_MASK_ID_LOW;
    filterconfig.FilterMaskIdHigh = FILTER_MASK_ID_HIGH;
    filterconfig.FilterFIFOAssignment = CAN_RX_FIFO_NUMBER;
    filterconfig.FilterActivation = CAN_FILTER_ENABLE;
    filterconfig.SlaveStartFilterBank = 14;

    // Setup filter
    configstatus = HAL_CAN_ConfigFilter(&hcan1, &filterconfig);
    if (configstatus != HAL_OK) return configstatus;

    // Start actual CAN
    configstatus = HAL_CAN_Start(&hcan1);
    if (configstatus != HAL_OK) return configstatus;

    // Enable interrupt for pending rx message
    #if CAN_RX_FIFO_NUMBER == CAN_RX_FIFO0
    configstatus =
        HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);
    #else   // CAN_RX_FIFO1
    configstatus =
        HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO1_MSG_PENDING);
    #endif

    return configstatus;
}

/** CAN Fetch Message
 * @brief Fetch a CAN message from the queue;
 *        will immediately return if queue is empty (non-blocking).
 * @note This is a wrapper for xQueueReceive and should not be called from an ISR
 * 
 * @param message Fetched message will be put here
 * @return BaseType_t pdTRUE if CAN message was successfully fetched from queue,
 *                    pdFALSE if queue is empty
 */
BaseType_t CAN_FetchMessage(CANMSG_t *message) {
    return xQueueReceive(RxQueue, message, (TickType_t)0);
}


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
        uint8_t len) {

    CAN_TxHeaderTypeDef txheader;

    // ExtID and extended mode are unused/not configured
    txheader.StdId = StdId;
    txheader.RTR = CAN_RTR_DATA;
    txheader.IDE = CAN_ID_STD;
    txheader.DLC = len;
    txheader.TransmitGlobalTime = DISABLE;

    return HAL_CAN_AddTxMessage(&hcan1, &txheader, TxData, &TxMailbox);
}

/**
 * CAN RxFifo Callbacks 
 * Only one should be used.
 * Set the Rx Fifo by changing the CAN_RX_FIFO_NUMBER in CANBus.h
 */

#if CAN_RX_FIFO_NUMBER == CAN_RX_FIFO0
/**
 * @brief Function executed by interrupt when there is a pending message
 *        on RxFifo0
 */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef* hcan) {
    HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, RxData);
    CAN_Recieve(&RxHeader, RxData);
}

#else   // CAN_RX_FIFO1
/**
 * @brief Function executed by interrupt when there is a pending message
 *        on RxFifo1
 */
void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef* hcan) {
    HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO1, &RxHeader, RxData);
    CAN_Recieve(&RxHeader, RxData);
}
#endif