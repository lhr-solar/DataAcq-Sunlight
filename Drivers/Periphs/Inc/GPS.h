#ifndef GPS_H
#define GPS_H

#include <stdlib.h>
#include <stdint.h>
#include "main.h"
#include <string.h>

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

//Initialize GPS Module and struct
/*
 * @param: struct that will be used to collect data
 * @return: ERROR or SUCCESS
 */
ErrorStatus GPS_Init(GPSData_t *Data);

//Update Measurments from GPS Module
/*
 * @return: ERROR or SUCCESS 
 */
ErrorStatus GPS_UpdateMeasurements(void);

#endif