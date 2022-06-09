#ifndef TASKS_H
#define TASKS_H

#include "FreeRTOS.h"
#include "semphr.h"

#define NUM_TASKS_WITH_INITS            3

extern SemaphoreHandle_t InitSem;

void InitializationTask(void *argument);

void DataReadingTask(void* argument);

void DataLoggingTask(void* argument);

void BroadcastingTask(void* argument);

#endif
