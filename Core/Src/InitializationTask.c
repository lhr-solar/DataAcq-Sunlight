#include "Tasks.h"
#include "FreeRTOS.h"
#include "task.h"
#include "config.h"
#include "IMU.h"
#include "SDCard.h"
#include "radio.h"
#include "CANBus.h"
#include "LED.h"

#if CAN_LOOPBACK
    #define CURR_CAN_MODE       CAN_MODE_LOOPBACK
#else
    #define CURR_CAN_MODE       CAN_MODE_NORMAL
#endif


TaskHandle_t DataLoggingTaskHandle = NULL;
TaskHandle_t DataReadingTaskHandle = NULL;
TaskHandle_t BroadcastingTaskHandle = NULL;

void InitializationTask(void *argument) {

    // stop other tasks from executing while initialization is occurring
    vTaskSuspendAll();

    BaseType_t task_success __attribute__((unused));
    task_success = xTaskCreate(DataLoggingTask,
                               "Data Logging Task",
                               DATA_LOGGING_TASK_STACK_SIZE,
                               NULL,
                               DATA_LOGGING_TASK_PRIORITY,
                               &DataLoggingTaskHandle
                               );
    task_success = xTaskCreate(DataReadingTask,
                               "Data Reading Task",
                               DATA_READING_TASK_STACK_SIZE,
                               NULL,
                               DATA_READING_TASK_PRIORITY,
                               &DataReadingTaskHandle
                               );
    task_success = xTaskCreate(BroadcastingTask,
                               "Broadcasting Task",
                               BROADCASTING_TASK_STACK_SIZE,
                               NULL,
                               BROADCASTING_TASK_PRIORITY,
                               &BroadcastingTaskHandle
                               );
    // TODO: add error checking

    bool sd_init_success = true;

    // Initialize peripherals
    vTaskDelay(1000);

    Ethernet_QueueInit(BroadcastingTaskHandle);
    debugprintf("Ethernet Queue Initialized\n\r");   

    if (SDCard_Init(DataLoggingTaskHandle) != FR_OK) {
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

    // Remove SD Card / Data Logging Task if SD Card fails to initialize
    if (!sd_init_success) {
        debugprintf("Deleting Data Logging Task\n\r");
        vTaskDelete(DataLoggingTaskHandle);
    }

    // Resume other tasks and exit
    xTaskResumeAll();
    vTaskDelete(NULL);
}

