#include "Tasks.h"
#include "IMU.h"
#include "GPS.h"
#include "CANBus.h"
// TODO: add SD.h header
#include "radio.h"

//This function reads from IMU, CAN, and GPS data structures, copies their current values, and sends them both through ethernet 
// (and to be saved on SD card when SD card code is finished).

void DataReadingTask(void* argument){
    while(1) {
        CANMSG_t CAN_Data;
        IMUData_t IMU_Data;
        GPSData_t GPS_Data;

        EthernetMSG_t Message;
        BaseType_t status;
        HAL_StatusTypeDef error;

        // Send IMU data
        error = IMU_GetMeasurements(&IMU_Data);
        if (error != HAL_ERROR){
            Message.id= IMU;
            Message.length = sizeof(IMU_Data); 
            memcpy(&Message.data.IMUData, &IMU_Data, sizeof(IMU_Data)); // copy to pass by value and not reference
            status = Ethernet_PutInQueue(&Message);
        }

        // Send CANBus data
        status = CAN_FetchMessage(&CAN_Data);
        if (status != pdFALSE) {
            Message.id= CAN;
            Message.length = sizeof(CAN_Data);
            memcpy(&Message.data.CANData, &CAN_Data, sizeof(CAN_Data));
            status = Ethernet_PutInQueue(&Message);
        }
               // Send GPS data
        status = GPS_ReadData(&GPS_Data);
        if (status != pdFALSE) {
            Message.id= GPS;
            Message.length = sizeof(GPS_Data);
            memcpy(&Message.data.GPSData, &GPS_Data, sizeof(GPS_Data));
            status = Ethernet_PutInQueue(&Message);
        }
    }
}
