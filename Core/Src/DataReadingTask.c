#include "Tasks.h"
#include "IMU.h"
#include "GPS.h"
#include "CANBus.h"
#include "SDCard.h"
#include "radio.h"

//This function reads from IMU, CAN, and GPS data structures, copies their current values, and sends them both through ethernet 
// (and to be saved on SD card when SD card code is finished).

void DataReadingTask(void* argument){
    while(1) {
        CANMSG_t CAN_Data;
        IMUData_t IMU_Data;
        GPSData_t GPS_Data;
        SDCard_t SDCard_data;

        EthernetMSG_t Message;
        //BaseType_t status;
        BaseType_t CANBus_status;
        BaseType_t GPS_status;
        //BaseType_t SDCard_status;
        HAL_StatusTypeDef error;

        // Retrieve GPS data
        GPS_status = GPS_ReadData(&GPS_Data);
        // Assign SD card timestamp
        memcpy(SDCard_data.time, GPS_Data.time, sizeof(GPS_Data.time));

        //Send GPS data and log in SD card
        if (GPS_status != pdFALSE) {
            Message.id= GPS;
            Message.length = sizeof(GPS_Data);
            memcpy(&Message.data.GPSData, &GPS_Data, sizeof(GPS_Data));
            Ethernet_PutInQueue(&Message);// TODO: Add error handling on Ethernet_PutInQueue()

            SDCard_data.id = GPS_SDCard;
            memcpy(&SDCard_data.data.GPSData, &GPS_Data, sizeof(GPS_Data));
            SDCard_PutInQueue(&SDCard_data); // TODO: Add error handling 
            

        } else {} // TODO: error handling for GPS_ReadData()

        // Send IMU data and log in SD card
        error = IMU_GetMeasurements(&IMU_Data);
        if (error != HAL_ERROR){
            Message.id = IMU;
            Message.length = sizeof(IMU_Data); 
            memcpy(&Message.data.IMUData, &IMU_Data, sizeof(IMU_Data)); // copy to pass by value and not reference
            Ethernet_PutInQueue(&Message);// TODO: Add error handling on Ethernet_PutInQueue()

            SDCard_data.id = IMU_SDCard;
            memcpy(&SDCard_data.data.IMUData, &IMU_Data, sizeof(IMU_Data));
            SDCard_PutInQueue(&SDCard_data); // TODO: add error handling    
        
        } else {} // TODO: error handling for IMU_GetMeasurements()

        // Send CANBus data and log in SD card
        CANBus_status = CAN_FetchMessage(&CAN_Data);

        // retrive and send as many CAN messages as possible
        while (CANBus_status != pdFALSE) { 
            Message.id= CAN;
            Message.length = sizeof(CAN_Data);
            memcpy(&Message.data.CANData, &CAN_Data, sizeof(CAN_Data));
            Ethernet_PutInQueue(&Message); // TODO: Add error handling on Ethernet_PutInQueue()
            
            
            SDCard_data.id = CAN_SDCard;
            memcpy(&SDCard_data.data.CANData, &CAN_Data, sizeof(CAN_Data));
            SDCard_PutInQueue(&SDCard_data); // Todo: add error handling

            CANBus_status = CAN_FetchMessage(&CAN_Data);
        }
    }
}
