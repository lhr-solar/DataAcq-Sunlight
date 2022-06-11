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

// writes will be performed in chunks less than/approximately this size
#define SDCARD_WRITE_BUFSIZE        128
#define SDCARD_SYNC_PERIOD          50      // sync period (milliseconds)
#define SDCARD_QUEUESIZE            32

// Error code for SDCard queue empty
// set to not conflict with FRESULT codes
#define SDC_QUEUE_EMPTY                 42

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
 * NOTE: Non-Blocking - Returns error if there is no data 
 * @param none
 * @return FRESULT FR_OK if ok, FR_DISK_ERR if SD queue is empty, and other errors specified in ff.h
 */
FRESULT SDCard_Sort_Write_Data();

/**
 * @brief Writes data to SD Card (Appends).
 * @param fp pointer to an initialized/opened FIL struct
 * @param buf buffer to write to SD Card
 * @param len length to write
 * @return FRESULT FR_OK if ok and other errors specified in ff.h
 */
FRESULT SDCard_Write(FIL *fp, const char *buf, size_t len);

/**
 * @brief Sync all open log files
 * 
 * @return FRESULT FR_OK if ok, or errors in ff.h
 */
FRESULT SDCard_SyncLogFiles();

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
