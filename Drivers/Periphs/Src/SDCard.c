/**
 * @file SDCard.c
 * @brief SD Card API
 * 
 * @copyright Copyright (c) 2022 UT Longhorn Racing Solar
 * 
 */

#include "SDCard.h"
#include "fatfs.h"
#include "main.h"
#include "config.h"
#include "LED.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#define SDCARD_WRITE_BUFSIZE        128
#define SDCARD_QUEUESIZE            32

static FATFS FatFs;
static QueueHandle_t SDCardQ; // information will be put on this
static const char * const filenames_list[] = {
    "can.csv",
    "imu.csv",
    "gps.csv"};
uint32_t SDCDroppedMessages = 0;    // for debugging purposes

static int SPrint_CAN(char *sdcard_write_buf, size_t bufsize, CANMSG_t *can, const char *time);
static int SPrint_IMU(char *sdcard_write_buf, size_t bufsize, IMUData_t *imu, const char *time);
static int SPrint_GPS(char *sdcard_write_buf, size_t bufsize, GPSData_t *gps, const char *time);
static FRESULT SDCard_Write(FIL fil, const char *fileName, const char *message, uint32_t size);

/**
 * @brief Mounts the drive and initializes Queue
 * @param None
 * @return FRESULT FR_OK if ok and other errors specified in ff.h
 */
FRESULT SDCard_Init() {
    //mount the drive
    SDCardQ = xQueueCreate(SDCARD_QUEUESIZE, sizeof(SDCard_t)); // creates the xQUEUE with the size of the fifo
    FRESULT fresult = f_mount(&FatFs, "", 1); //1=mount now
    #if DEBUGGINGMODE
    if (fresult != FR_OK) {
  	    debugprintf("f_mount error (%i)\r\n", (int)fresult);
    }
    #endif
    return fresult;
}

/**
 * @brief Reads how much memory is left in SD card-> Should be used for debugging purposes
 * @param None
 * @return FRESULT FR_OK if ok and other errors specified in ff.h
 */
FRESULT SDCard_GetStatistics() {
    DWORD free_clusters;
    DWORD free_sectors;
    DWORD total_sectors;
    FATFS *getFreeFs;
    FRESULT fresult;

    fresult = f_getfree("", &free_clusters, &getFreeFs);
    if(fresult != FR_OK){
        printf("f_getfree error (%i)\r\n", (int)fresult);
    }

    //Formula comes from ChaN's documentation
    total_sectors = (getFreeFs->n_fatent - 2) * getFreeFs->csize;
    free_sectors = free_clusters * getFreeFs->csize;

    printf("SD card stats:\r\n%10lu KiB total drive space.\r\n%10lu KiB available.\r\n", total_sectors / 2, free_sectors / 2);

    return fresult;
}

/** SD Card PutInQueue
 * @brief Put data in SD Card Queue
 * 
 * @param data Data to place in queue
 * @return BaseType_t - pdTrue if placed, errQUEUE_FULL if full
 */
BaseType_t SDCard_PutInQueue(SDCard_t* data) {
    BaseType_t success = xQueueSendToBack(SDCardQ, data, (TickType_t)0);
    if (success == errQUEUE_FULL) {
        SDCDroppedMessages++;
    }
    return success;
}

/**
 * @brief Formats data to be written to SD card based on type of data input. 
 * NOTE: Non-Blocking - Returns error if there is no data 
 * @param none
 * @return FRESULT FR_OK if ok, FR_DISK_ERR if SD queue is empty, and other errors specified in ff.h
 */
FRESULT SDCard_Sort_Write_Data(){
    // check if data from queue is from imu, gps, or can.
    // send data to corresponding file in sd card
    SDCard_t cardData;
    enum filenames_idx {CAN_FNAME = 0, IMU_FNAME, GPS_FNAME};
    static char message[SDCARD_WRITE_BUFSIZE];
    
    FIL file;
    uint8_t fname_idx = 0;
    uint16_t bytes_written = -1;

    if (xQueueReceive(SDCardQ, &cardData, (TickType_t)1) != pdTRUE) return FR_DISK_ERR;
    #ifdef DEBUGGINGMODE
        LED_Toggle(ARRAY);
    #endif
    // check ID of qdata for type of message, adjust message once we know what kind of message we are dealing with
    switch (cardData.id) {
        case CAN_SDCard:
            fname_idx = CAN_FNAME;
            bytes_written = SPrint_CAN(message, 
                                       SDCARD_WRITE_BUFSIZE, 
                                       &cardData.data.CANData,
                                       cardData.time);
            break;
        case IMU_SDCard:
            fname_idx = IMU_FNAME;
            bytes_written = SPrint_IMU(message, 
                                       SDCARD_WRITE_BUFSIZE, 
                                       &cardData.data.IMUData,
                                       cardData.time);
            break;
        case GPS_SDCard:
            fname_idx = GPS_FNAME;
            bytes_written = SPrint_GPS(message, 
                                       SDCARD_WRITE_BUFSIZE, 
                                       &cardData.data.GPSData,
                                       cardData.time);
            break;
        default:
            break;
    }

    if (bytes_written < 0) return FR_DISK_ERR;  // note: the error value is arbitrary
    #ifdef DEBUGGINGMODE
    debugprintf("Write: %s", message);
    #endif
    return SDCard_Write(file, filenames_list[fname_idx], message, bytes_written);
}

/**
 * @brief Unmounts the drive
 * @param None
 * @return FRESULT FR_OK if ok and other errors specified in ff.h
 */
FRESULT SDCard_CloseFileSystem(){
    // un-mount the drive
    return f_mount(NULL, "", 0);
}

/**
 * @brief Mounts the drive
 * @param None
 * @return FRESULT FR_OK if ok and other errors specified in ff.h
 */
FRESULT SDCard_OpenFileSystem(){
    // un-mount the drive
    return f_mount(&FatFs, "", 1); //1=mount now
}

/**
 * @brief Fetch number of dropped SD Card messages due to queue overfilling.
 *        Included for debug purposes
 * @return Number of dropped messages
 */
uint32_t SDCard_FetchDroppedMsgCnt() {
    return SDCDroppedMessages;
}

//STATIC FUNCTIONS FOR WRITING TO SD CARD WITH CORRECT FORMAT
// Wrappers for snprintf() for each of the message data types
// Each function takes a buffer to write to, the buffer size, 
// and a pointer to the respective data structs

static int SPrint_CAN(char *sdcard_write_buf, 
                      size_t bufsize, 
                      CANMSG_t *can, 
                      const char *time) {
    return snprintf(sdcard_write_buf, 
                    bufsize, 
                    "%.9s,%.3" PRIx16 ",%" PRIu8 ",%" PRIx64 "\r\n", 
                    time,
                    can->id, 
                    can->payload.idx, 
                    *(uint64_t *)can->payload.data.bytes);
}

static int SPrint_IMU(char *sdcard_write_buf, 
                      size_t bufsize, 
                      IMUData_t *imu, 
                      const char *time) {
    return snprintf(sdcard_write_buf, 
                    bufsize, 
                    "%.9s,%" PRId16 ",%" PRId16 ",%" PRId16 
                    ",%" PRId16 ",%" PRId16 ",%" PRId16 
                    ",%" PRId16 ",%" PRId16 ",%" PRId16 "\r\n",
                    time,
                    imu->accel_x,
                    imu->accel_y,
                    imu->accel_z,
                    imu->mag_x,
                    imu->mag_y,
                    imu->mag_z,
                    imu->gyr_x,
                    imu->gyr_y,
                    imu->gyr_z);
}

static int SPrint_GPS(char *sdcard_write_buf, 
                      size_t bufsize, 
                      GPSData_t *gps, 
                      const char *time) {
    char gps_str[sizeof(GPSData_t) + 1];
    memcpy(gps_str, gps, sizeof(GPSData_t));
    gps_str[sizeof(GPSData_t)] = '\0';

    return snprintf(sdcard_write_buf, 
                    bufsize, 
                    "%.9s,%s\r\n",
                    time,
                    gps_str);
}

/**
 * @brief Writes data to SD Card
 * @param fil File object structure. Will be initialized if not already
 * @param fileName Name of file to write to. Will be created if not existing. Appends data to end of file
 * @param message char array of data to write to SD Card
 * @param size size of data to write to file
 * @return FRESULT FR_OK if ok and other errors specified in ff.h
 * 
 * TODO: optimize this; the open and close are a huge bottleneck
 */
static FRESULT SDCard_Write(FIL fil, const char *fileName, const char *message, uint32_t size) {
    BYTE readBuf[size];
    FRESULT fresult;
    fresult = f_open(&fil, fileName, FA_WRITE | FA_OPEN_APPEND);

    #ifdef DEBUGGINGMODE
  	debugprintf("f_open error (%i)\r\n", fresult);
    #endif
    if (fresult != FR_OK) return fresult;

    //Copy in a string
    strncpy((char*)readBuf, message, strlen(message));
    UINT bytesWrote;
    fresult = f_write(&fil, readBuf, strlen(message), &bytesWrote);

    #ifdef DEBUGGINGMODE
  	debugprintf("f_write error (%i)\r\n", (int)fresult);
    #endif
    if (fresult != FR_OK) return fresult;

    //close your file!
    f_close(&fil);
    return fresult;
}
