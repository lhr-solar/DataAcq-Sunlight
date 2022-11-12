#ifndef SEM_QUEUE_H
#define SEM_QUEUE_H

/**
 * @file SemQueue.h
 * @brief Queue interface for use with single writer / single reader scenarios.
 *        Uses 'Direct-to-task Notifications' as counting semaphores. 
 *        https://www.freertos.org/RTOS-task-notifications.html
 */

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include <stdbool.h>

typedef struct {
    TaskHandle_t owner;
    QueueHandle_t handle;
} SemaphoreQueue_t;

BaseType_t SemQueueSendToBack(SemaphoreQueue_t *q, void *msg);

BaseType_t SemQueueRecieve(SemaphoreQueue_t *q, void *buf, bool nonblocking);

#endif  // SEM_QUEUE_H
