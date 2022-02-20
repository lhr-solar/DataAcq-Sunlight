#ifndef GPS_H
#define GPS_H

#include <stdlib.h>
#include <stdint.h>
#include "FreeRTOS.h"
#include "queue.h"
#include "main.h"
#include <string.h>

#define GPS_RX_QUEUE_SIZE 1

typedef struct{
    char hr[2]; // Will not use these parameters unless we have to
    char min[2]; // ^^
    char sec[2]; // ^^
    char ms[3]; // ^^
    char latitude_Deg[2];
    char latitude_Min[6];
    char NorthSouth;
    char longitude_Deg[3];
    char longitude_Min[6];
    char EastWest;
    char speedInKnots[4];
    char day[2]; // Will not use these parameters unless we have to
    char month[2]; // ^^
    char year[4]; // ^^
    char magneticVariation_Deg[4];
    char magneticVariation_EastWest;
} GPSData_t;

/** GPSInit
 * @brief Initialize GPS, configure GPS
 * @return ERROR or SUCCESS if transmit worked
 */
ErrorStatus GPS_Init(void);

/** GPS Update Measurements
 * @brief Update All GPS measurements
 * @return SUCCESS or ERROR if read worked
 */
ErrorStatus GPS_UpdateMeasurements(void);

/** GPS Read Data
 * Wrapper function for XQueueReceive and should not be called from an ISR
 * 
 * @param Data
 * @return pdTRUE if CAN message was successfully fetched from queue, pdFALSE if queue is empty
 */
BaseType_t GPS_ReadData(GPSData_t *Data);

#endif