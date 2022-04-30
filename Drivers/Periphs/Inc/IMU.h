#ifndef IMU_H
#define IMU_H

#include "main.h"
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

/** 
 * @brief Initialize IMU to collect data
 * @param None
 * @return HAL_StatusTypeDef - OK, ERROR, BUSY, or TIMEOUT
 */
HAL_StatusTypeDef IMU_Init(void);

/**
 * @brief Update struct with new information
 * @param *Data : struct used to collect IMU Data
 * @return HAL_StatusTypeDef - OK, ERROR, BUSY, or TIMEOUT
 */
HAL_StatusTypeDef IMU_GetMeasurements(IMUData_t *Data);

/**
 * @brief Update global with IMU Calibration register data. Used to hardcode calibration
 * @note The IMU must be in config mode (REG[0x3D]= 0) to read calibration registers
 * @param None
 * @return HAL_StatusTypeDef - OK, ERROR, BUSY, or TIMEOUT
 */
HAL_StatusTypeDef IMU_GetCalibData(void);

/**
 * @brief Use pre-found data to upload calibration profile to IMU
 * @note updating the last bytes of the Z offsets will set the respective peripheral bits in the calibration status register (0x35)
 * @note The IMU must be in config mode (REG[0x3D]= 0) to read calibration registers
 * @param None
 * @return HAL_StatusTypeDef - OK, ERROR, BUSY, or TIMEOUT
 */
HAL_StatusTypeDef IMU_Calibrate(void); 

/** 
 * @brief This function aims to provide an all in one debug function. Call to help pinpoint an issue with the IMU
 * @param None
 * @return HAL_StatusTypeDef - OK, ERROR, BUSY, or TIMEOUT
 */
HAL_StatusTypeDef IMU_Debug(void);

#endif
