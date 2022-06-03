#include "Tasks.h"
#include "IMU.h"
#include "GPS.h"
#include "CANBus.h"
#include "SDCard.h"
#include "radio.h"

//This function reads from IMU, CAN, and GPS data structures, copies their current values, and sends them both through ethernet 
// (and to be saved on SD card when SD card code is finished).

void DataReadingTask(void* argument){
    if (IMU_Init() != HAL_OK); //TODO: ERROR CHECKING HERE
    if (GPS_Init() == ERROR); //TODO: ERROR CHECKING HERE
    if (CAN_Init(CAN_MODE_LOOPBACK) != HAL_OK); //TODO: ERROR CHECKING HERE
    xSemaphoreTake(InitSem, 0);
    while(uxSemaphoreGetCount(InitSem) != 0);

    // TODO: cleanup everything

    while(1) {
        CANMSG_t CANData;
        IMUData_t IMUData;
        GPSData_t GPSData;
        SDCard_t SDCardData;
        EthernetMSG_t EthMessage;   

        //Send GPS data and log in SD card
        if (GPS_ReadData(&GPSData) != pdFALSE) {
            memcpy(SDCardData.time, GPSData.time, sizeof(GPSData.time)); //assign timestamp
            EthMessage.id= GPS;
            EthMessage.length = sizeof(GPSData);
            EthMessage.data.GPSData = GPSData;
            Ethernet_PutInQueue(&EthMessage);// TODO: Add error handling on Ethernet_PutInQueue()

            SDCardData.id = GPS_SDCard;
            SDCardData.data.GPSData = GPSData;
            SDCard_PutInQueue(&SDCardData); // TODO: Add error handling 
        }
        else {} // TODO: error handling for GPS_ReadData()

        // Send IMU data and log in SD card
        if (IMU_GetMeasurements(&IMUData) != HAL_ERROR){
            EthMessage.id = IMU;
            EthMessage.length = sizeof(IMUData); 
            EthMessage.data.IMUData = IMUData;
            Ethernet_PutInQueue(&EthMessage);// TODO: Add error handling on Ethernet_PutInQueue()

            SDCardData.id = IMU_SDCard;
            SDCardData.data.IMUData = IMUData;
            SDCard_PutInQueue(&SDCardData); // TODO: add error handling    
        } 
        else {} // TODO: error handling for IMU_GetMeasurements()

        // retrive and send as many CAN EthMessages as possible
        while (CAN_FetchMessage(&CANData) != pdFALSE) { 
            EthMessage.id= CAN;
            EthMessage.length = sizeof(CANData);
            EthMessage.data.CANData = CANData;
            Ethernet_PutInQueue(&EthMessage); // TODO: Add error handling on Ethernet_PutInQueue()
            
            SDCardData.id = CAN_SDCard;
            SDCardData.data.CANData = CANData;
            SDCard_PutInQueue(&SDCardData); // TODO: add error handling
        }
    }
}
