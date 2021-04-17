#ifndef IMU_H
#define IMU_H

#define OPR_MODE 0x3D
#define ACC_NM_SET 0x16
#define ACC_CONFIG 0x08
#define GYR_CONFIG 0x0A
#define MAG_CONFIG 0x09
#define UNIT_SEL 0x3B
#define DEV_ADDR 0x28
#define ACC_DATA_Z_MSB 0x0D
#define ACC_DATA_Z_LSB 0x0C
#define ACC_DATA_Y_MSB 0x0B
#define ACC_DATA_Y_LSB 0x0A
#define ACC_DATA_X_MSB 0x09
#define ACC_DATA_X_LSB 0x08

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
    int16_t accel_temp;
    int16_t gyr_temp;
} IMUData_t;

void IMU_Init(void);

void IMU_Rx(void);

#endif