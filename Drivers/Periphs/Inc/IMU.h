#ifndef IMU_H
#define IMU_H

#include "main.h"
#include <stdlib.h>
#include <stdint.h>

typedef struct {
    int16_t accel_x;
    int16_t accel_y;
    int16_t accel_z;
    int16_t mag_x;
    int16_t mag_y;
    int16_t mag_z;
    int16_t gyr_x;
    int16_t gyr_y;
    int16_t gyr_z;
} IMUData_t;

//Initialize IMU to collect data
/*
 * @param: Data struct used to collect IMU Data
 * @return: SUCCESS or ERROR
 */
ErrorStatus IMU_Init(void);

//Update struct with new information
/*
 * @return: SUCCESS or ERROR
 */
ErrorStatus IMU_GetMeasurements(IMUData_t *Data);

#endif