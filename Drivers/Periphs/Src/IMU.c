#include "IMU.h"

IMUData_t IMUData;

void IMU_Init(){
    uint16_t config = OPR_MODE << 8;
    config |= 7;
    HAL_I2C_Master_Transmit_IT(&hi2c1, DEV_ADDR, config, 1); //Turn on Accelerometer, Gyroscope, and Magnetometer
    config = 0;
    config = UNIT_SEL << 8;
    config &= 0xFF00;
    HAL_I2C_Master_Transmit_IT(&hi2c1, DEV_ADDR, config, 1); //Read in m/s^2, Celcius, and degrees
}

void IMU_Rx(){

}