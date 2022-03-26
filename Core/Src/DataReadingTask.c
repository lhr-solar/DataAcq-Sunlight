#include "Tasks.h"
#include "CANBus.h"
// #include "IMU.h"
// #include "RTC.h"
// #include "GPS.h"
// #include "SD.h"
// #include "radio.h"

CAN_HandleTypeDef hcan1;

void DataReadingTask(void* argument){
    // Initialize Peripherals
    if (CAN_Config(&hcan1, CAN_MODE_LOOPBACK) != HAL_OK) {
        // TODO: error checking
    }

    CANMSG_t message;
    while (1) {
        // Fetch RTC for adding to sd queue

        // Add CAN message to be accessed by Broadcasting and Logging
        if (CAN_FetchMessage(&message) == pdTRUE) {
            // Add to ethernet queue
            // Add to sd card queue
        }

        // Fetch IMU and add to eth/sd queues
        // Fetch GPS and add to eth/sd queues
    }
}
