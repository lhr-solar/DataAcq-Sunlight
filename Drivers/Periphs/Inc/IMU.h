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


/* 
 * @brief Initialize IMU to collect data
 * @param None
 * @return: SUCCESS or ERROR
 */
ErrorStatus IMU_Init(void);


/* 
 * @brief Update struct with new information
 * @param *Data : struct used to collect IMU Data
 * @return: SUCCESS or ERROR
 */
ErrorStatus IMU_GetMeasurements(IMUData_t *Data);


/*
 * @brief Update struct with IMU Calibration register data. Used to hardcode calibration
 * @note The IMU must be in config mode (REG[0x3D]= 0) to read calibration registers
 * @param *Data : struct used to collect IMU calibration data
 * @return: SUCCESS or ERROR
 */
ErrorStatus IMU_GetCalibData(IMUCalibData_t *Data);


/*
 * @brief Use IMUCalibData_t struct to upload calibration profile to IMU
 * @note updating the last bytes of the Z offsets will set the respective peripheral bits in the calibration status register (0x35)
 * @note The IMU must be in config mode (REG[0x3D]= 0) to read calibration registers
 * @param *Data : struct holding IMU calibration data
 * @return: SUCCESS or ERROR
 */
ErrorStatus Calibrate(IMUCalibData_t *Data);

ErrorStatus DebugIMU(void);

#endif