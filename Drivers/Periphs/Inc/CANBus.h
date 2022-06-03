#ifndef CAN_BUS_H
#define CAN_BUS_H

// CAN Drivers

/**
 * Initialize and configure a singular CANBus
 * Currently set up for CAN1
 * 
 * Recieved CAN messages are placed in an external software FIFO.
 * "queue.h" has information on FIFO operations
 */

#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "queue.h"
#include <stdbool.h>


/**
 * Filter and HAL RX_FIFO configurations
 * Set FILTER_MASK_ID_LOW/HIGH to 0 to recieve all can messages
 */
#define CAN_RX_FIFO_NUMBER      CAN_RX_FIFO0    /* CAN_RX_FIFO0 or CAN_RX_FIFO1 */
#define FILTER_BANK             0          /* 0 - 13 for CAN1, 14 - 27 for CAN2 */
#define FILTER_ID_LOW           0x0000
#define FILTER_ID_HIGH          0x0000
#define FILTER_MASK_ID_LOW      0x0000
#define FILTER_MASK_ID_HIGH     0x0000

/**
 * CAN Message structure copied from BPS
 */

// Enum for ID's of all messages that can be sent across CAN bus
// IDs must be listed in strictly increasing order!

typedef enum {
    // System Critical
    DASH_KILL_SWITCH	                        = 0x001,    // 1B
    TRIP    	                                = 0x002,    // 1B
    ANY_SYSTEM_FAILURES	                        = 0x003,    // 1B
    IGNITION	                                = 0x004,    // 1B
    ANY_SYSTEM_SHUTOFF	                        = 0x005,    // 1B

    // BPS
    ALL_CLEAR	                                = 0x101,    // 1B
    CONTACTOR_STATE	                            = 0x102,    // 1B
    CURRENT_DATA	                            = 0x103,    // 4B
    VOLT_DATA	                                = 0x104,    // 5B   - 1B idx + 4B word
    TEMP_DATA	                                = 0x105,    // 5B   - 1B idx + 4B word
    SOC_DATA	                                = 0x106,    // 4B
    WDOG_TRIGGERED	                            = 0x107,    // 1B
    CAN_ERROR	                                = 0x108,    // 1B
    BPS_COMMAND_MSG	                            = 0x109,    // 8B   - Message contents not guaranteed
    SUPPLEMENTAL_VOLTAGE	                    = 0x10B,    // 2B
    CHARGE_ENABLE   	                        = 0x10C,    // 1B

    // Controls
    CAR_STATE	                                = 0x580,    // 1B
    MOTOR_CONTROLLER_BUS	                    = 0x242,    // 8B   - 4B word + 4B word
    VELOCITY	                                = 0x243,    // 8B   - 4B word + 4B word
    MOTOR_CONTROLLER_PHASE_CURRENT	            = 0x244,    // 8B   - 4B word + 4B word
    MOTOR_VOLTAGE_VECTOR	                    = 0x245,    // 8B   - 4B word + 4B word
    MOTOR_CURRENT_VECTOR	                    = 0x246,    // 8B   - 4B word + 4B word
    MOTOR_BACKEMF	                            = 0x247,    // 8B   - 4B word + 4B word
    MOTOR_TEMPERATURE	                        = 0x24B,    // 8B   - 4B word + 4B word
    ODOMETER_BUS_AMP_HOURS  	                = 0x24E,    // 8B   - 4B word + 4B word
    ARRAY_CONTACTOR_STATE_CHANGE	            = 0x24F,    // 1B

    // Array
    SUNSCATTER_A_MPPT1_ARRAY_VOLTAGE_SETPOINT   = 0x600,    // 4B   - float
    SUNSCATTER_A_ARRAY_VOLTAGE_MEASUREMENT	    = 0x601,    // 4B   - float
    SUNSCATTER_A_ARRAY_CURRENT_MEASUREMENT	    = 0x602,    // 4B   - float
    SUNSCATTER_A_BATTERY_VOLTAGE_MEASUREMENT    = 0x603,    // 4B   - float
    SUNSCATTER_A_BATTERY_CURRENT_MEASUREMENT    = 0x604,    // 4B   - float
    SUNSCATTER_A_OVERRIDE_EN_COMMAND	        = 0x605,    // 1B
    SUNSCATTER_A_FAULT	                        = 0x606,    // 1B
    SUNSCATTER_B_MPPT2_ARRAY_VOLTAGE_SETPOINT   = 0x610,    // 4B   - float
    SUNSCATTER_B_ARRAY_VOLTAGE_MEASUREMENT	    = 0x611,    // 4B   - float
    SUNSCATTER_B_ARRAY_CURRENT_MEASUREMENT	    = 0x612,    // 4B   - float
    SUNSCATTER_B_BATTERY_VOLTAGE_MEASUREMENT    = 0x613,    // 4B   - float
    SUNSCATTER_B_BATTERY_CURRENT_MEASUREMENT    = 0x614,    // 4B   - float
    SUNSCATTER_B_OVERRIDE_EN_COMMAND	        = 0x615,    // 1B
    SUNSCATTER_B_FAULT	                        = 0x616,    // 1B
    BLACKBODY_RTD_SENSOR_MEASUREMENT	        = 0x620,    // 5B   - 1B id + 4B float (handle as 5B raw data)
    BLACKBODY_IRRADIANCE_SENSOR_1_MEASUREMENT   = 0x630,    // 4B   - float
    BLACKBODY_IRRADIANCE_SENSOR_2_MEASUREMENT   = 0x631,    // 4B   - float
    BLACKBODY_IRRADIANCE_RTD_BOARD_EN_COMMAND   = 0x632,    // 1B
    BLACKBODY_IRRADIANCE_RTD_BOARD_FAULT	    = 0x633,    // 1B
    PV_CURVE_TRACER_PROFILE	                    = 0x640,    // 5B

    LARGEST_CAN_ID                                          // For the lookup table size
} CANId_t;

// Union of data that can be sent across CAN bus. Only one field must be filled out
typedef union {
	uint8_t b;
	uint16_t h;
	uint32_t w;
	float f;
    uint8_t bytes[8];   // use when data matches no normal datatype
} CANData_t;

/**
 * @note    idx is only used when an array of data needs to be sent.
 * @note    data is a union so only one of the fields should be filled out or bad stuff will happen.
 */
typedef struct {
	uint8_t idx;
	CANData_t data;
} CANPayload_t;

// This data type is used to push messages onto the queue
typedef struct {
    CANId_t id;
    CANPayload_t payload;
} CANMSG_t;

// Used to format the fields in the CAN metadata lookup table

struct CanLUTEntry {uint8_t idx_used : 1; uint8_t len : 7;};

/** CAN Config
 * @brief Initialize CAN, configure CAN filters/interrupts, and start CAN
 * 
 * @param mode CAN_MODE_NORMAL or CAN_MODE_LOOPBACK for operation mode
 * @return HAL_StatusTypeDef - Status of CAN configuration
 */
HAL_StatusTypeDef CAN_Init(uint32_t mode);

/** CAN Fetch Message
 * @brief Fetch a CAN message from the queue
 * @note This is a wrapper for xQueueReceive and should not be called from an ISR
 * 
 * @param message Fetched message will be put here
 * @return BaseType_t pdTRUE if CAN message was successfully fetched from queue,
 *                    pdFALSE if queue is empty
 */
BaseType_t CAN_FetchMessage(CANMSG_t *message);

/** CAN Transmit Message
 * @brief Transmit message over CAN
 * @note This is really basic and does not check for a full transmit Mailbox
 * 
 * @param StdId Message ID (Standard)
 * @param TxData Data to transmit
 * @param len Length of data (Bytes) to transmit (MAX 8B)
 * @return HAL_StatusTypeDef - Status of CAN configuration
 */
HAL_StatusTypeDef CAN_TransmitMessage(uint32_t StdId, uint8_t *TxData, uint8_t len);

/**
 * @brief Fetch metadata associated with an id
 * @return True if valid entry, False if invalid
 */
bool CAN_FetchMetadata(CANId_t id, struct CanLUTEntry *entry);

#endif /* CAN_BUS_H */
