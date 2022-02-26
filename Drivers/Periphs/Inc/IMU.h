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

typedef struct {
    int16_t accel_offset_x;
    int16_t accel_offset_y;
    int16_t accel_offset_z;
    int16_t mag_offset_x;
    int16_t mag_offset_y;
    int16_t mag_offset_z;
    int16_t gyr_offset_x;
    int16_t gyr_offset_y;
    int16_t gyr_offset_z;
    int16_t accel_radius;
    int16_t mag_radius;
} IMUCalibData_t;

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
ErrorStatus IMU_GetCalibData(IMUCalibData_t *Data);
ErrorStatus Calibrate(IMUCalibData_t *Data);

#endif