#include "Tasks.h"
#include "IMU.h"
#include "GPS.h"
#include "CANBus.h"
// TODO: add SD.h header
#include "radio.h"

// TODO: The ethernet send functions return BaseType_t which is just a long I think. Is it jsut returning the type that was placed on the stack?
//This function reads from IMU and GPS data structures, copies their current values, and sends them both through ethernet and to be saved on SD card.
//TODO: include CAN data
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
            Message.length = sizeof(IMU_Data);  // TODO: Ensure this is correct
            //Message.data.IMUData = IMU_Data; // TODO: Make sure we're passing by value and not by reference
            memcpy(&Message.data.IMUData, &IMU_Data, sizeof(IMU_Data));
            status = Ethernet_PutInQueue(&Message);
        }


        // Send CANBus data
        status = CAN_FetchMessage(&CAN_Data);  // TODO: Double check this is the proper return
        if (status != pdFALSE) {
            Message.id= CAN;
            Message.length = sizeof(CAN_Data);
            //Message.data.CANData = CAN_Data; // TODO: Make sure we're passing by value and not by reference
            memcpy(&Message.data.CANData, &CAN_Data, sizeof(CAN_Data));
            status = Ethernet_PutInQueue(&Message);
        }


        // Send GPS data
        status = GPS_ReadData(&GPS_Data);  // TODO: Double check this is the proper return
        if (status != pdFALSE) {
            Message.id= GPS;
            Message.length = sizeof(GPS_Data);
            //Message.data.GPSData = GPS_Data; // TODO: Make sure we're passing by value and not by reference
            memcpy(&Message.data.GPSData, &GPS_Data, sizeof(GPS_Data));
            status = Ethernet_PutInQueue(&Message);
        }
    }

}
