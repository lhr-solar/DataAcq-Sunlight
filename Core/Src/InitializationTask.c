#include "Tasks.h"
#include "config.h"
#include "cmsis_os.h"
#include "fatfs.h"
#include "lwip.h"
#include "IMU.h"
#include "SDCard.h"
#include "Tasks.h"
#include "radio.h"
#include "CANBus.h"
#include "LED.h"

#if CAN_LOOPBACK
    #define CURR_CAN_MODE       CAN_MODE_LOOPBACK
#else
    #define CURR_CAN_MODE       CAN_MODE_NORMAL
#endif

osThreadId_t DataLoggingTaskHandle;
osThreadAttr_t DataLoggingTask_attributes = {
  .name = "Data Logging Task",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 1024
};
osThreadId_t DataReadingTaskHandle;
osThreadAttr_t DataReadingTask_attributes = {
  .name = "Data Reading Task",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 1024
};
osThreadId_t BroadcastingTaskHandle;
osThreadAttr_t BroadcastingTask_attributes = {
  .name = "Broadcasting Task",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 1024
};
osThreadId_t CANSendingTaskHandle;
osThreadAttr_t CANSendingTask_attributes = {
  .name = "CANSending Task",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 1024
};

void InitializationTask(void *argument) {
    bool sd_init_success = true;

    // Initialize peripherals
    osDelay(1000);

    Ethernet_QueueInit();
    debugprintf("Ethernet Queue Initialized\n\r");   

    if (SDCard_Init() != FR_OK) {
        sd_init_success = false;
    }
    debugprintf("SD Card Initialized\n\r");  

    if (CAN_Init(CURR_CAN_MODE) != HAL_OK) {
        LED_On(INIT_FAIL);
    }
    debugprintf("CAN Initialized\n\r");

    if (GPS_Init() == ERROR) {
        LED_On(INIT_FAIL);
    }
    debugprintf("GPS Initialized\n\r");

    if (IMU_Init() != HAL_OK) {
        LED_On(INIT_FAIL);
    }
    debugprintf("IMU Initialized\n\r");

    // Create tasks
    if (sd_init_success) {
        DataLoggingTaskHandle = osThreadNew(DataLoggingTask, NULL, &DataLoggingTask_attributes);
    }
    DataReadingTaskHandle = osThreadNew(DataReadingTask, NULL, &DataReadingTask_attributes);
    BroadcastingTaskHandle = osThreadNew(BroadcastingTask, NULL, &BroadcastingTask_attributes);
    CANSendingTaskHandle = osThreadNew(CANSendingTask, NULL, &CANSendingTask_attributes);

    // Exit
    osThreadExit();
}

