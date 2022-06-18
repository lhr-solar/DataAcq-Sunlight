/**
 * @file CANBus.c
 * @brief CAN API
 * 
 * @copyright Copyright (c) 2022 UT Longhorn Racing Solar
 * 
 */

#include "CANBus.h"
#include "main.h"
#include "config.h"
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
static uint32_t CANDroppedMessages = 0;   // for debugging purposes

/**
 * @brief Lookup table containing the lengths (in bytes) of corresponding 
 *        to every valid CAN message ID, and if the index is used.
 * @note  Entries are populated at the bottom
 */
static const struct CanLUTEntry CanMetadataLUT[LARGEST_CAN_ID];

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
    struct CanLUTEntry metadata;
    memset(&canmessage, 0, sizeof(canmessage));

    canmessage.id = rx_header->StdId;

    if (!CAN_FetchMetadata(canmessage.id, &metadata)) {
        return HAL_ERROR;   // invalid ID
    }

    if (metadata.idx_used) {
        canmessage.payload.idx = rx_data[0];
        memcpy(canmessage.payload.data.bytes, &rx_data[1], metadata.len);
    }
    else {
        memcpy(canmessage.payload.data.bytes, rx_data, metadata.len);
    }

    // Add message to FIFO
    if (xQueueSendToBackFromISR(RxQueue, &canmessage, NULL) == errQUEUE_FULL) {
        CANDroppedMessages++;
        return HAL_ERROR;
    }
    debugprintf("CANID: %d, CANidx: %d, CAN Payload: %d", canmessage.id, canmessage.payload.idx, canmessage.payload.data);
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
 * @brief Fetch metadata associated with an id
 * @return True if valid entry, False if invalid
 */
bool CAN_FetchMetadata(CANId_t id, struct CanLUTEntry *entry) {
    *entry = CanMetadataLUT[id];
    return (entry->len != 0);
}

/**
 * @brief Fetch number of dropped CAN messages due to queue overfilling.
 *        Included for debug purposes
 * @return Number of dropped messages
 */
uint32_t CAN_FetchDroppedMsgCnt() {
    return CANDroppedMessages;
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




static const struct CanLUTEntry CanMetadataLUT[LARGEST_CAN_ID] = {
    // System Critical
    [DASH_KILL_SWITCH]                          = {.idx_used = 0, .len = 1},
    [TRIP]                                      = {.idx_used = 0, .len = 1},
    [ANY_SYSTEM_FAILURES]                       = {.idx_used = 0, .len = 1},
    [IGNITION]                                  = {.idx_used = 0, .len = 1},
    [ANY_SYSTEM_SHUTOFF]                        = {.idx_used = 0, .len = 1},
    
    // BPS
    [ALL_CLEAR]                                 = {.idx_used = 0, .len = 1},
    [CONTACTOR_STATE]                           = {.idx_used = 0, .len = 1},
    [CURRENT_DATA]                              = {.idx_used = 0, .len = 4},
    [VOLT_DATA]                                 = {.idx_used = 1, .len = 4},
    [TEMP_DATA]                                 = {.idx_used = 1, .len = 4},
    [SOC_DATA]                                  = {.idx_used = 0, .len = 4},
    [WDOG_TRIGGERED]                            = {.idx_used = 0, .len = 1},
    [CAN_ERROR]                                 = {.idx_used = 0, .len = 1},
    [BPS_COMMAND_MSG]                           = {.idx_used = 0, .len = 8},
    [SUPPLEMENTAL_VOLTAGE]                      = {.idx_used = 0, .len = 2},
    [CHARGE_ENABLE]                             = {.idx_used = 0, .len = 1},
    
    // Controls
    [CAR_STATE]                                 = {.idx_used = 0, .len = 1},
    [MOTOR_CONTROLLER_BUS]                      = {.idx_used = 0, .len = 8},
    [VELOCITY]                                  = {.idx_used = 0, .len = 8},
    [MOTOR_CONTROLLER_PHASE_CURRENT]            = {.idx_used = 0, .len = 8},
    [MOTOR_VOLTAGE_VECTOR]                      = {.idx_used = 0, .len = 8},
    [MOTOR_CURRENT_VECTOR]                      = {.idx_used = 0, .len = 8},
    [MOTOR_BACKEMF]                             = {.idx_used = 0, .len = 8},
    [MOTOR_TEMPERATURE]                         = {.idx_used = 0, .len = 8},
    [ODOMETER_BUS_AMP_HOURS]                    = {.idx_used = 0, .len = 8},
    [ARRAY_CONTACTOR_STATE_CHANGE]              = {.idx_used = 0, .len = 1},
    
    // Array
    [SUNSCATTER_A_MPPT1_ARRAY_VOLTAGE_SETPOINT] = {.idx_used = 0, .len = 4},
    [SUNSCATTER_A_ARRAY_VOLTAGE_MEASUREMENT]    = {.idx_used = 0, .len = 4},
    [SUNSCATTER_A_ARRAY_CURRENT_MEASUREMENT]    = {.idx_used = 0, .len = 4},
    [SUNSCATTER_A_BATTERY_VOLTAGE_MEASUREMENT]  = {.idx_used = 0, .len = 4},
    [SUNSCATTER_A_BATTERY_CURRENT_MEASUREMENT]  = {.idx_used = 0, .len = 4},
    [SUNSCATTER_A_OVERRIDE_EN_COMMAND]          = {.idx_used = 0, .len = 1},
    [SUNSCATTER_A_FAULT]                        = {.idx_used = 0, .len = 1},
    [SUNSCATTER_B_MPPT2_ARRAY_VOLTAGE_SETPOINT] = {.idx_used = 0, .len = 4},
    [SUNSCATTER_B_ARRAY_VOLTAGE_MEASUREMENT]    = {.idx_used = 0, .len = 4},
    [SUNSCATTER_B_ARRAY_CURRENT_MEASUREMENT]    = {.idx_used = 0, .len = 4},
    [SUNSCATTER_B_BATTERY_VOLTAGE_MEASUREMENT]  = {.idx_used = 0, .len = 4},
    [SUNSCATTER_B_BATTERY_CURRENT_MEASUREMENT]  = {.idx_used = 0, .len = 4},
    [SUNSCATTER_B_OVERRIDE_EN_COMMAND]          = {.idx_used = 0, .len = 1},
    [SUNSCATTER_B_FAULT]                        = {.idx_used = 0, .len = 1},
    [BLACKBODY_RTD_SENSOR_MEASUREMENT]          = {.idx_used = 0, .len = 5},
    [BLACKBODY_IRRADIANCE_SENSOR_1_MEASUREMENT] = {.idx_used = 0, .len = 4},
    [BLACKBODY_IRRADIANCE_SENSOR_2_MEASUREMENT] = {.idx_used = 0, .len = 4},
    [BLACKBODY_IRRADIANCE_RTD_BOARD_EN_COMMAND] = {.idx_used = 0, .len = 1},
    [BLACKBODY_IRRADIANCE_RTD_BOARD_FAULT]      = {.idx_used = 0, .len = 1},
    [PV_CURVE_TRACER_PROFILE]                   = {.idx_used = 0, .len = 5}
};
