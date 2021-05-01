#ifndef IMU_H
#define IMU_H

#define OPR_MODE 0x3D
#define ACC_NM_SET 0x16
#define ACC_CONFIG 0x08
#define GYR_CONFIG 0x0A
#define MAG_CONFIG 0x09
#define UNIT_SEL 0x3B
#define DEV_ADDR 0x2800 //Device address must be shifted to the left by 8 bits when using HAL_I2C library
#define ACC_DATA_Z_MSB 0x0D
#define ACC_DATA_Z_LSB 0x0C
#define ACC_DATA_Y_MSB 0x0B
#define ACC_DATA_Y_LSB 0x0A
#define ACC_DATA_X_MSB 0x09
#define ACC_DATA_X_LSB 0x08
#define GYR_DATA_X_LSB 0x14
#define GYR_DATA_X_MSB 0x15
#define GYR_DATA_Y_LSB 0x16
#define GYR_DATA_Y_MSB 0x17
#define GYR_DATA_Z_LSB 0x18
#define GYR_DATA_Z_MSB 0x19
#define MAG_DATA_X_LSB 0x0E
#define MAG_DATA_X_MSB 0x0F
#define MAG_DATA_Y_LSB 0x10
#define MAG_DATA_Y_MSB 0x11
#define MAG_DATA_Z_LSB 0x12
#define MAG_DATA_Z_MSB 0x13

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
    //int16_t accel_temp;
    //int16_t gyr_temp;
} IMUData_t;

//Initialize IMU to collect data
/*
 * @param: Data struct used to collect IMU Data
 * @return: SUCCESS or ERROR
 */
ErrorStatus IMU_Init(IMUData_t* Data);

//Update struct with new information
/*
 * @return: SUCCESS or ERROR
 */
ErrorStatus IMU_UpdateMeasurements(void);

#endif