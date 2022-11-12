#ifndef TASKS_H
#define TASKS_H

#define STACK_SIZE_DEFAULT                  1024    // stack size in words
enum TaskPriority {
    TASK_PRIORITY_VERY_HIGH = 50,
    TASK_PRIORITY_HIGH = 40,
    TASK_PRIORITY_NORMAL = 30,
    TASK_PRIORITY_LOW = 20,
    TASK_PRIORITY_VERY_LOW = 10
};

// Initialization Task
#define INITIALIZATION_TASK_STACK_SIZE      STACK_SIZE_DEFAULT  // stack size in words
#define INITIALIZATION_TASK_PRIORITY        TASK_PRIORITY_VERY_HIGH
void InitializationTask(void *argument);    

// Data Reading Task
#define DATA_READING_TASK_STACK_SIZE        STACK_SIZE_DEFAULT  // stack size in words
#define DATA_READING_TASK_PRIORITY          TASK_PRIORITY_NORMAL
void DataReadingTask(void* argument);

// Data Logging Task
#define DATA_LOGGING_TASK_STACK_SIZE        STACK_SIZE_DEFAULT  // stack size in words
#define DATA_LOGGING_TASK_PRIORITY          TASK_PRIORITY_NORMAL
void DataLoggingTask(void* argument);

// Broadcasting Task
#define BROADCASTING_TASK_STACK_SIZE        STACK_SIZE_DEFAULT  // stack size in words
#define BROADCASTING_TASK_PRIORITY          TASK_PRIORITY_NORMAL
void BroadcastingTask(void* argument);

#endif
