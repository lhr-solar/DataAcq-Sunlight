#ifndef TASKS_H
#define TASKS_H

#include "FreeRTOS.h"
#include "semphr.h"

void InitializationTask(void *argument);

void DataReadingTask(void* argument);

void DataLoggingTask(void* argument);

void BroadcastingTask(void* argument);

void CANSendingTask(void* argument);

#endif
