/**
 * @file DataReadingTask.c
 * 
 * Task in charge of collecting data from sensors (IMU, GPS) and CAN.
 * Data is added to the logging and broadcasting queues.
 * @note Assumes that all lower level functions are initialized in main.c  
 * 
 * @copyright Copyright (c) 2022 UT Longhorn Racing Solar
 * 
 * TODO: error check
 */

#include "Tasks.h"
#include "IMU.h"
#include "GPS.h"
#include "CANBus.h"
#include "SDCard.h"
#include "radio.h"
#include "config.h"
#include "LED.h"
#include <string.h>
#include <stdio.h>

void DataReadingTask(void* argument){
    
    CANMSG_t CANData;
    IMUData_t IMUData;
    GPSData_t GPSData;
    SDCard_t SDCardData;
    EthernetMSG_t EthMessage;  

    while(1) {
        
        //Send GPS data and log in SD card
        if (GPS_ReadData(&GPSData) == pdTRUE) {
            memcpy(SDCardData.time, GPSData.time, sizeof(GPSData.time)); //assign timestamp
            EthMessage.id= GPS;
            EthMessage.length = sizeof(GPSData);
            EthMessage.data.GPSData = GPSData;
            Ethernet_PutInQueue(&EthMessage);

            SDCardData.id = GPS_SDCard;
            SDCardData.data.GPSData = GPSData;
            SDCard_PutInQueue(&SDCardData);
        }
        else {}

        // Send IMU data and log in SD card
        if (IMU_GetMeasurements(&IMUData) == HAL_OK){
            EthMessage.id = IMU;
            EthMessage.length = sizeof(IMUData); 
            EthMessage.data.IMUData = IMUData;
            Ethernet_PutInQueue(&EthMessage);

            SDCardData.id = IMU_SDCard;
            SDCardData.data.IMUData = IMUData;
            SDCard_PutInQueue(&SDCardData);
        } 
        else {}

        // retrieve and send as many CAN Messages as possible
        while (CAN_FetchMessage(&CANData) == pdTRUE) { 
            EthMessage.id= CAN;
            EthMessage.length = sizeof(CANData);
            EthMessage.data.CANData = CANData;
            Ethernet_PutInQueue(&EthMessage);
            
            SDCardData.id = CAN_SDCard;
            SDCardData.data.CANData = CANData;
            SDCard_PutInQueue(&SDCardData);
        }
    }
}
