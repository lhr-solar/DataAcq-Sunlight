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
        BaseType_t CANBus_status;
        BaseType_t GPS_status;
        HAL_StatusTypeDef error;

        // Send GPS data
        GPS_status = GPS_ReadData(&GPS_Data);
        if (GPS_status != pdFALSE) {
            Message.id= GPS;
            Message.length = sizeof(GPS_Data);
            memcpy(&Message.data.GPSData, &GPS_Data, sizeof(GPS_Data));
            status = Ethernet_PutInQueue(&Message);// TODO: Add error handling on Ethernet_PutInQueue()

            // retrieve GPS time field and use as timestamp in SD card logging
            
        } else {} // TODO: error handling for GPS_ReadData()

        // Send IMU data
        error = IMU_GetMeasurements(&IMU_Data);
        if (error != HAL_ERROR){
            Message.id= IMU;
            Message.length = sizeof(IMU_Data); 
            memcpy(&Message.data.IMUData, &IMU_Data, sizeof(IMU_Data)); // copy to pass by value and not reference
            status = Ethernet_PutInQueue(&Message);// TODO: Add error handling on Ethernet_PutInQueue()
        } else {} // TODO: error handling for IMU_GetMeasurements()

        // Send CANBus data
        CANBus_status = CAN_FetchMessage(&CAN_Data);
        while (CANBus_status != pdFALSE) { // retrive and send as many CAN messages as possible
            Message.id= CAN;
            Message.length = sizeof(CAN_Data);
            memcpy(&Message.data.CANData, &CAN_Data, sizeof(CAN_Data));
            status = Ethernet_PutInQueue(&Message); // TODO: Add error handling on Ethernet_PutInQueue()
            CANBus_status = CAN_FetchMessage(&CAN_Data);
        }
    }
}
