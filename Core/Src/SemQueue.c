/**
 * @file SemQueue.c
 * @brief Queue interface for use with single writer / single reader scenarios.
 *        Uses 'Direct-to-task Notifications' as counting semaphores. 
 *        https://www.freertos.org/RTOS-task-notifications.html
 */

#include "SemQueue.h"
#include <stdint.h>

BaseType_t SemQueueSendToBack(SemaphoreQueue_t *q, void *msg) {
    BaseType_t success = xQueueSendToBack(q->handle, msg, (TickType_t)0);
    if (success != errQUEUE_FULL) {
        xTaskNotifyGive(q->owner);
    }
    return success;
}

BaseType_t SemQueueRecieve(SemaphoreQueue_t *q, void *buf, bool nonblocking) {
    ulTaskNotifyTake(pdFALSE, 
                     nonblocking ? (TickType_t)0 : portMAX_DELAY);
    // if nonblocking is not set, xQueueReceive() should never return pdFALSE.
    return xQueueReceive(q->handle, buf, (TickType_t)0);
}
