#ifndef GPS_H
#define GPS_H

#include <stdlib.h>
#include <stdint.h>
#include "FreeRTOS.h"
#include "queue.h"
#include "main.h"
#include <string.h>

#define GPS_RX_QUEUE_SIZE 8

#define GPS_BUFSIZE     100
extern char GPSRxDataBuf[GPS_BUFSIZE];

typedef struct{
    char time[9]; //2 hr, 2 min, 2 sec, 3 ms
    char status; //Data Valid (A) or Not Valid (V)
    char latitude[8]; //2 degrees, 6 minutes
    char NorthSouth;
    char longitude[9]; //3 degrees, 6 minutes
    char EastWest;
    char speedInKnots[4]; //x.xx
    char courseInDegrees[6]; //xxx.xx
    char date[6]; //may not use this (ddmmyy)
    char magneticVariation[7]; // "x.xx y" y is E or W
} GPSData_t;

/** GPSInit
 * 
 * @brief Initialize GPS, configure GPS
 * @return ERROR or SUCCESS if transmit worked
 */
ErrorStatus GPS_Init();

/** GPS Read Data
 * Wrapper function for XQueueReceive and should not be called from an ISR
 * 
 * @param Data
 * @return pdTRUE if GPS message was successfully fetched from queue, pdFALSE if queue is empty
 */
BaseType_t GPS_ReadData(GPSData_t *Data);

#endif