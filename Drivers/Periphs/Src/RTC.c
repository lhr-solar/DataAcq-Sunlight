//The Real Time Clock (RTC) communicates with the uC over I2C2
#include "RTC.h"

RTCData_t *RTCData;

void RTC_Init(RTCData_t *data){
    //configure RTC
    uint8_t config[2];
    config[0] = CONTROL_1; //register address
    config[1] = 0x08; //data to be transmitted 
    HAL_I2C_Master_Transmit_IT(&hi2c2, DEVICE_ADDRESS, config, 2); // select 12 hour mode 
    
    data = RTCData;
}

void RTC_UpdateMeasurements(){
    uint8_t data[7];
    uint8_t reg_addr = SECONDS;
    HAL_I2C_Master_Transmit_IT(&hi2c2, DEVICE_ADDRESS, &reg_addr, 1); //send register address to read from
    HAL_I2C_Master_Receive_IT(&hi2c2, DEVICE_ADDRESS, data, 7);
    RTCData->sec = data[0];
    RTCData->min = data[1];
    RTCData->hr = data[2];
    RTCData->days = data[3];
    RTCData->weekdays = data[4];
    RTCData->months = data[5];
    RTCData->years = data[6];




}