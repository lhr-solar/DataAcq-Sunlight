/**
 * @file SDCard.h
 * @brief SD Card API
 * 
 * @copyright Copyright (c) 2022 UT Longhorn Racing Solar
 * 
 */

#ifndef SDCARD_H
#define SDCARD_H

#include "FreeRTOS.h"
#include "queue.h"
#include "fatfs.h"
#include "CANBus.h"
#include "IMU.h"
#include "GPS.h"

typedef enum{
    // this is where you have different types for the different messages that you might have 
    // for instance one for can 
    // one for gps, one for imu, one for CAN
    IMU_SDCard = 0x1,
    GPS_SDCard = 0x2,
    CAN_SDCard = 0x3
} SDCardID_t;

typedef union { 
	CANMSG_t CANData;
    IMUData_t IMUData;
    GPSData_t GPSData;
} SDCardData_t;

typedef struct {
    char time[9]; // time taken from GPS module
    SDCardID_t id;
	SDCardData_t data;
} SDCard_t;


/**
 * @brief Mounts the drive and intializes the queue
 * @param None
 * @return FRESULT FR_OK if ok and other errors specified in ff.h
 */
FRESULT SDCard_Init();

/**
 * @brief Reads how much memory is left in SD Card. Should be used for debugging purposes
 * @param None
 * @return FRESULT FR_OK if ok and other errors specified in ff.h
 */
FRESULT SDCard_GetStatistics();

/** SD Card PutInQueue
 * @brief Put data in SD Card Queue
 * 
 * @param data Data to place in queue
 * @return BaseType_t - pdTrue if placed, errQUEUE_FULL if full
 */
BaseType_t SDCard_PutInQueue(SDCard_t* data);

/**
 * @brief Formats data to be written to SD card based on type of data input. 
 * NOTE: Blocking - Waits for data to be in queue before writing
 * @param none
 * @return FRESULT FR_OK if ok and other errors specified in ff.h
 */
FRESULT SDCard_Sort_Write_Data();

/**
 * @brief Writes data to SD Card
 * @param fil File object structure. Will be initialized if not already
 * @param fileName Name of file to write to. Will be created if not existing. Appends data to end of file
 * @param message char array of data to write to SD Card
 * @param size size of data to write to file
 * @return FRESULT FR_OK if ok and other errors specified in ff.h
 */
FRESULT SDCard_Write(FIL fil, const char *fileName, const char *message, uint32_t size);

/**
 * @brief Unmounts the drive
 * @param None
 * @return FRESULT FR_OK if ok and other errors specified in ff.h
 */
FRESULT SDCard_CloseFileSystem();

/**
 * @brief Mounts the drive
 * @param None
 * @return FRESULT FR_OK if ok and other errors specified in ff.h
 */
FRESULT SDCard_OpenFileSystem();

/**
 * @brief Fetch number of dropped SD Card messages due to queue overfilling.
 *        Included for debug purposes
 * @return Number of dropped messages
 */
uint32_t SDCard_FetchDroppedMsgCnt();

#endif
