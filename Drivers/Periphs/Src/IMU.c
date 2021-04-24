#include "IMU.h"

IMUData_t IMUData;

void IMU_Init(){
    uint8_t config[2];
    config[0] = OPR_MODE; //register address
    config[1] = 7; //data to be transmitted 
    HAL_I2C_Master_Transmit_IT(&hi2c1, DEV_ADDR, config, 2); //Turn on Accelerometer, Gyroscope, and Magnetometer 
    config[0] = UNIT_SEL;
    config[1] = 0;
    HAL_I2C_Master_Transmit_IT(&hi2c1, DEV_ADDR, config, 2); //Read in m/s^2, Celcius, and degrees
}

void IMU_Rx(){
    uint8_t reg_addr = 8;
    uint8_t data[18];
    HAL_I2C_Master_Transmit_IT(&hi2c1, DEV_ADDR, &reg_addr, 1); //send register address to read from
    HAL_I2C_Master_Receive_IT(&hi2c1, DEV_ADDR, data, 18);
    IMUData.accel_x = (data[0] << 8) | data[1];
    IMUData.accel_y = (data[2] << 8) | data[3];
    IMUData.accel_z = (data[4] << 8) | data[5];
    IMUData.mag_x = (data[6] << 8) | data[7];
    IMUData.mag_y = (data[8] << 8) | data[9];
    IMUData.mag_z = (data[10] << 8) | data[11];
    IMUData.gyr_x = (data[12] << 8) | data[13];
    IMUData.gyr_y = (data[14] << 8) | data[15];
    IMUData.gyr_z = (data[16] << 8) | data[17];
}