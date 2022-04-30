#include "Tasks.h"
#include "IMU.h"
#include "GPS.h"
#include "SD.h" // TODO: double check that this is the right header name
#include "radio.h"

// TODO: The ethernet send functions return BaseType_t which is just a long I think. Is it jsut returning the type that was placed on the stack?
//This function reads from IMU and GPS data structures, copies their current values, and sends them both through ethernet and to be saved on SD card.
//TODO: include CAN data
void DataReadingTask(void* argument){
    while(1) {
        IMUData_t IMU_Data;
        GPSData_t GPS_Data;
        EthernetMSG_t Message;

        HAL_StatusTypeDef error;

        error = IMU_GetMeasurements(*IMU_Data);
        Message.id= 0x1
        Message.length = sizeof(IMU_Data)  // TODO: Ensure this is correct
        Message.data.IMU_Data = IMU_Data // TODO: Make sure we're passing by value and not by reference
        // ethernetFIFO function

        //SD card functions TODO

        error = GPS_UpdateMeasurements() // this function seems to create a struct and send data all within its scope
        GPS_Data = GPS_ReadData(GPS_Data);  // TODO: Double check this is the proper return






    }

}
