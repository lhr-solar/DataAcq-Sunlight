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

#define SDC_ID_ENUM_TO_IDX(x)       (x-1)   // x is of type SDCardID_t
typedef struct {
    const char fname[15];
    FIL file;
    int (* sprint)(char *, size_t, void *, const char *);
    char writebuf[SDCARD_WRITE_BUFSIZE];
    uint32_t bufidx;
} logfile_t;

static int SPrint_CAN(char *sdcard_write_buf, size_t bufsize, void *can, const char *time);
static int SPrint_IMU(char *sdcard_write_buf, size_t bufsize, void *imu, const char *time);
static int SPrint_GPS(char *sdcard_write_buf, size_t bufsize, void *gps, const char *time);

// Globals
static FATFS FatFs;
static QueueHandle_t SDCardQ;

static logfile_t LogFiles[SDC_ID_ENUM_TO_IDX(LARGEST_SDC_ID)] = {
    [SDC_ID_ENUM_TO_IDX(IMU_SDCard)] = {.fname = "imu.csv", .sprint = SPrint_IMU, .bufidx = 0},
    [SDC_ID_ENUM_TO_IDX(GPS_SDCard)] = {.fname = "gps.csv", .sprint = SPrint_GPS, .bufidx = 0},
    [SDC_ID_ENUM_TO_IDX(CAN_SDCard)] = {.fname = "can.csv", .sprint = SPrint_CAN, .bufidx = 0}
};

uint32_t SDCDroppedMessages = 0;    // for debugging purposes



/**
 * @brief Mounts the drive, initializes Queue, and opens all logging files
 * @return FRESULT FR_OK if ok and other errors specified in ff.h
 */
FRESULT SDCard_Init() {
    // mount the drive
    SDCardQ = xQueueCreate(SDCARD_QUEUESIZE, sizeof(SDCard_t)); // creates the xQUEUE with the size of the fifo
    FRESULT fresult = f_mount(&FatFs, "", 1); //1=mount now
    if (fresult != FR_OK) {
  	    debugprintf("f_mount error (%i)\r\n", (int)fresult);
    }

    for (uint32_t i = 0; i < SDC_ID_ENUM_TO_IDX(LARGEST_SDC_ID); i++) {
        fresult = f_open(&LogFiles[i].file, LogFiles[i].fname, FA_WRITE | FA_OPEN_APPEND);

        if (fresult != FR_OK) {
            debugprintf("f_open error %s (%d)\r\n", LogFiles[i].fname, (int)fresult);
            break;
        }
    }

    return fresult;
}

/**
 * @brief Reads how much memory is left in SD card-> Should be used for debugging purposes
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
 * @brief Formats data and writes to SD card based on type of data input. 
 *        Data is buffered and written in large chunks. 
 *        !!! DOES NOT SYNC DATA !!! You must call SDCard_SyncLogFiles() to save.
 * @note: Non-Blocking - Returns error if there is no data 
 * @param none
 * @return FRESULT FR_OK if ok, FR_DISK_ERR if SD queue is empty, and other errors specified in ff.h
 */
FRESULT SDCard_Sort_Write_Data(){
    // check if data from queue is from imu, gps, or can.
    // send data to corresponding file in sd card
    SDCard_t cardData;
    FRESULT success = FR_OK;

    if (xQueueReceive(SDCardQ, &cardData, (TickType_t)1) != pdTRUE) return SDC_QUEUE_EMPTY;
    if (((uint32_t)cardData.id) >= LARGEST_SDC_ID) return FR_DISK_ERR;

    logfile_t *log_file = &LogFiles[SDC_ID_ENUM_TO_IDX(cardData.id)];

    if (log_file->bufidx >= SDCARD_WRITE_BUFSIZE - SDCARD_MAX_MSGSIZE) {
        // flush buffer
        success = SDCard_Write(&log_file->file, log_file->writebuf, log_file->bufidx);
        log_file->bufidx = 0;
    }

    log_file->bufidx += log_file->sprint(
            log_file->writebuf + log_file->bufidx, 
            SDCARD_WRITE_BUFSIZE - log_file->bufidx,
            &cardData.data,
            cardData.time);
    
    return success;
}

/**
 * @brief Writes data to SD Card (Appends). 
 *        !!! DOES NOT SYNC DATA !!! You must call f_sync() or f_close() to save.
 * @param fp pointer to an initialized/opened FIL struct
 * @param buf buffer to write to SD Card
 * @param len length to write
 * @return FRESULT FR_OK if ok and other errors specified in ff.h
 */
FRESULT SDCard_Write(FIL *fp, const char *buf, size_t len) {
    FRESULT fresult;

    UINT bytes_written;
    fresult = f_write(fp, buf, len, &bytes_written);

    if (fresult != FR_OK || len != bytes_written) {
        debugprintf("f_write error (%i)\r\n", (int)fresult);
        f_close(fp);
    }

    return fresult;
}

/**
 * @brief Sync all open log files
 * 
 * @return FRESULT FR_OK if ok, or errors in ff.h
 */
FRESULT SDCard_SyncLogFiles() {
    LED_On(SDC_SYNC);

    FRESULT fresult;

    for (uint32_t i = 0; i < SDC_ID_ENUM_TO_IDX(LARGEST_SDC_ID); i++) {
        fresult = f_sync(&LogFiles[i].file);

        if (fresult != FR_OK) {
            debugprintf("f_open error %s (%d)\r\n", LogFiles[i].fname, (int)fresult);
            return fresult;
        }
    }

    LED_Off(SDC_SYNC);

    return fresult;
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
                      void *can, 
                      const char *time) {
    return snprintf(sdcard_write_buf, 
                    bufsize, 
                    "%.9s,%.3" PRIx16 ",%" PRIu8 ",%" PRIx64 "\n", 
                    time,
                    ((CANMSG_t *)can)->id, 
                    ((CANMSG_t *)can)->payload.idx, 
                    *(uint64_t *)(((CANMSG_t *)can)->payload.data.bytes));
}

static int SPrint_IMU(char *sdcard_write_buf, 
                      size_t bufsize, 
                      void *imu, 
                      const char *time) {
    return snprintf(sdcard_write_buf, 
                    bufsize, 
                    "%.9s,%" PRId16 ",%" PRId16 ",%" PRId16 
                    ",%" PRId16 ",%" PRId16 ",%" PRId16 
                    ",%" PRId16 ",%" PRId16 ",%" PRId16 "\n",
                    time,
                    ((IMUData_t *)imu)->accel_x,
                    ((IMUData_t *)imu)->accel_y,
                    ((IMUData_t *)imu)->accel_z,
                    ((IMUData_t *)imu)->mag_x,
                    ((IMUData_t *)imu)->mag_y,
                    ((IMUData_t *)imu)->mag_z,
                    ((IMUData_t *)imu)->gyr_x,
                    ((IMUData_t *)imu)->gyr_y,
                    ((IMUData_t *)imu)->gyr_z);
}

static int SPrint_GPS(char *sdcard_write_buf, 
                      size_t bufsize, 
                      void *gps, 
                      const char *time) {
    int len = snprintf(sdcard_write_buf, 
                          bufsize, 
                          "%.9s,", time);
    for (uint32_t i = 0; i < sizeof(GPSData_t); i++) {
        char letter = ((char *)gps)[i];
        sdcard_write_buf[i + len] = (letter) ? letter : ' '; // replace NULL with ' '
    }
    strcpy(sdcard_write_buf, "\n");

    return len + sizeof(GPSData_t) + 1; // +1 for \n character
}

