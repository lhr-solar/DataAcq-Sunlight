#include "IMU.h"

IMUData_t* IMUData;
HAL_StatusTypeDef error;

ErrorStatus IMU_Init(IMUData_t* Data){
    uint8_t config[2];
    config[0] = OPR_MODE; //register address
    config[1] = 7; //data to be transmitted 
    error = HAL_I2C_Master_Transmit_IT(&hi2c1, DEV_ADDR, config, 2); //Turn on Accelerometer, Gyroscope, and Magnetometer 
    if (error != HAL_OK){return ERROR;}
    config[0] = UNIT_SEL;
    config[1] = 0;
    error = HAL_I2C_Master_Transmit_IT(&hi2c1, DEV_ADDR, config, 2); //Read in m/s^2, Celcius, and degrees
    if (error != HAL_OK){return ERROR;}
    IMUData = Data;
    return SUCCESS;
}

ErrorStatus IMU_UpdateMeasurements(){
    uint8_t reg_addr = 8;
    uint16_t data[9];
    error = HAL_I2C_Master_Transmit_IT(&hi2c1, DEV_ADDR, &reg_addr, 1); //send register address to read from
    if (error != HAL_OK){return ERROR;}
    error = HAL_I2C_Master_Receive_IT(&hi2c1, DEV_ADDR, data, 18);
    if (error != HAL_OK){return ERROR;}
    (*IMUData).accel_x = data[0];
    (*IMUData).accel_y = data[1];
    (*IMUData).accel_z = data[2];
    (*IMUData).mag_x = data[3];
    (*IMUData).mag_y = data[4];
    (*IMUData).mag_z = data[5];
    (*IMUData).gyr_x = data[6];
    (*IMUData).gyr_y = data[7];
    (*IMUData).gyr_z = data[8];
    return SUCCESS;
}