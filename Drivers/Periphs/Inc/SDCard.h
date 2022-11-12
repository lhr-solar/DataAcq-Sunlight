/**
 * @file SDCard.h
 * @brief SD Card API
 * 
 * @copyright Copyright (c) 2022 UT Longhorn Racing Solar
 * 
 */

#ifndef SDCARD_H
#define SDCARD_H

#include "fatfs.h"
#include "CANBus.h"
#include "IMU.h"
#include "GPS.h"
#include <stdbool.h>

// writes will be performed in chunks less than/approximately this size
#define SDCARD_WRITE_BUFSIZE        512
#define SDCARD_SYNC_PERIOD          50      // sync period (milliseconds)
#define SDCARD_MAX_MSGSIZE          (sizeof(SDCard_t) + 11) // 11 is a result of length 9 time string + '\n' + '\0'
#define SDCARD_QUEUESIZE            32

typedef enum{
    // this is where you have different types for the different messages that you might have 
    // for instance one for can 
    // one for gps, one for imu, one for CAN
    
    // NOTE: MUST START FROM 0x1
    IMU_SDCard = 0x1,
    GPS_SDCard = 0x2,
    CAN_SDCard = 0x3,

    LARGEST_SDC_ID
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
 * @brief Mounts the drive, initializes Queue, and opens all logging files
 * @param queue_reader Task Handle of the sole reader of the queue
 * @return FRESULT FR_OK if ok and other errors specified in ff.h
 */
FRESULT SDCard_Init(TaskHandle_t queue_reader);

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
 * @brief Formats data and writes to SD card based on type of data input. 
 *        Data is buffered and written in large chunks. 
 *        Blocking: This will wait until the queue is nonempty
 *        !!! DOES NOT SYNC DATA !!! You must call SDCard_SyncLogFiles() to save.
 * 
 * @return FRESULT Filesystem errors specified in ff.h 
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
