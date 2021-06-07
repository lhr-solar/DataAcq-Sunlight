/* Copyright (c) 2020 UT Longhorn Racing Solar */

#include "stm32f4xx_hal.h"

#define STDIN_FILENO  0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

extern UART_HandleTypeDef huart3;

int _write(int fd, char *buffer, unsigned int len) {
    if(buffer != NULL) {
        HAL_UART_Transmit(&huart3, (uint8_t*)buffer, len, HAL_MAX_DELAY);
    }
    return len;
}

int _read(int const fd, char *buffer, unsigned const len) {
    if(buffer != NULL) {
        HAL_UART_Receive(&huart3, (uint8_t*)buffer, 1, HAL_MAX_DELAY);
    }

    return 1;
}

int _close(int file)
{
    return -1;
}

int _lseek(int file, int ptr, int dir)
{
    return 0;
}



