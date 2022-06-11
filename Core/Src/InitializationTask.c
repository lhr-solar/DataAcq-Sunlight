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

#if CAN_LOOPBACK
    #define CURR_CAN_MODE       CAN_MODE_LOOPBACK
#else
    #define CURR_CAN_MODE       CAN_MODE_NORMAL
#endif

osThreadId_t DataLoggingTaskHandle;
osThreadAttr_t DataLoggingTask_attributes = {
  .name = "Data Logging Task",
  .priority = (osPriority_t) osPriorityNormal, //Will determine priorities later
  .stack_size = 1024 //arbitrary value might need to make it larger or smaller
};
osThreadId_t DataReadingTaskHandle;
osThreadAttr_t DataReadingTask_attributes = {
  .name = "Data Reading Task",
  .priority = (osPriority_t) osPriorityNormal, //Will determine priorities later
  .stack_size = 1024 //arbitrary value might need to make it larger or smaller
};
osThreadId_t BroadcastingTaskHandle;
osThreadAttr_t BroadcastingTask_attributes = {
  .name = "Broadcasting Task",
  .priority = (osPriority_t) osPriorityBelowNormal, //Will determine priorities later
  .stack_size = 1024 //arbitrary value might need to make it larger or smaller
};

void InitializationTask(void *argument) {
    // Initialize peripherals
    osDelay(1000);

    Ethernet_QueueInit();
    debugprintf("Ethernet Queue Initialized\n\r");   

    if (SDCard_Init() != FR_OK);
    debugprintf("SD Card Initialized\n\r");  

    if (CAN_Init(CURR_CAN_MODE) != HAL_OK);
    debugprintf("CAN Initialized\n\r");  

    if (GPS_Init() == ERROR);
    debugprintf("GPS Initialized\n\r");

    if (IMU_Init() != HAL_OK);
    debugprintf("IMU Initialized\n\r");

    // Create tasks
    DataReadingTaskHandle = osThreadNew(DataReadingTask, NULL, &DataReadingTask_attributes);
    DataLoggingTaskHandle = osThreadNew(DataLoggingTask, NULL, &DataLoggingTask_attributes);
    BroadcastingTaskHandle = osThreadNew(BroadcastingTask, NULL, &BroadcastingTask_attributes);

    // Exit
    osThreadExit();
}

