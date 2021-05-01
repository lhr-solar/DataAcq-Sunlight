#ifndef RTC_H
#define RTC_H


#include "main.h"
#include <stdlib.h>
#include <stdint.h>

#define DEVICE_ADDRESS 0x3400   //shifted left by 7 for HAL library

#define CONTROL_1 0x00
#define CONTROL_2 0x01
#define CONTROL_3 0x02
#define SECONDS 0x03
#define MINUTES 0x04
#define HOURS 0x05
#define DAYS 0x06
#define WEEKDAYS 0x07
#define MONTHS 0x08
#define YEARS 0x09
#define MINUTE_ALARM 0x0A
#define HOUR_ALARM 0x0B
#define DAY_ALARM 0x0C
#define WEEKDAY_ALARM 0x0D
#define OFFSET 0x0E

#define TIME_MODE 0x08; // 0 = 24 hr mode, 1 = 12 hr mode

typedef struct{
    uint8_t sec; // Will not use these parameters unless we have to
    uint8_t min; // ^^
    uint8_t hr; // ^^
    uint8_t days; // ^^
    uint8_t weekdays;
    uint8_t months;
    uint8_t years;
} RTCData_t;

void RTC_Init(RTCData_t *data);

void RTC_UpdateMeasurements();

#endif