#ifndef TASKS_H
#define TASKS_H

#include "FreeRTOS.h"
#include "queue.h"

void DataReadingTask(void* argument);

void DataLoggingTask(void* argument);

void BroadcastingTask(void* argument);

#endif
