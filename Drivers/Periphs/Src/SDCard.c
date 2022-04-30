#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h> //for va_list var arg functions
#include "fatfs.h"
#include "SDCard.h"
#include "main.h"

//If debugging mode is set printf's will be enabled and diagnostic information will be printed over UART. 
//This should be disabled when running on system
#define DEBUGGINGMODE   0
#define SDCARD_QUEUESIZE 16
static FATFS FatFs;
static QueueHandle_t SDCardQ; // information will be put on this
static const char * const filenames_list[] = {
    "can.csv",
    "imu.csv",
    "gps.csv"};

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
    #ifdef DEBUGGINGMODE
    if (fresult != FR_OK) {
  	    printf("f_mount error (%i)\r\n", (int)fresult);
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
    #ifdef DEBUGGINGMODE
    if(fresult != FR_OK){
        printf("f_getfree error (%i)\r\n", (int)fresult);
    }
    #endif
    //Formula comes from ChaN's documentation
    total_sectors = (getFreeFs->n_fatent - 2) * getFreeFs->csize;
    free_sectors = free_clusters * getFreeFs->csize;

    #ifdef DEBUGGINGMODE
    printf("SD card stats:\r\n%10lu KiB total drive space.\r\n%10lu KiB available.\r\n", total_sectors / 2, free_sectors / 2);
    #endif

    return fresult;
}

/** SD Card PutInQueue
 * @brief Put data in SD Card Queue
 * 
 * @param data Data to place in queue
 * @return BaseType_t - pdTrue if placed, errQUEUE_FULL if full
 */
BaseType_t SDCard_PutInQueue(SDCard_t* data) {
    return xQueueSendToBack(SDCardQ, data, (TickType_t)0);
}

/**
 * @brief Formats data to be written to SD card based on type of data input. 
 * NOTE: Blocking - Waits for data to be in queue before writing
 * @param none
 * @return FRESULT FR_OK if ok and other errors specified in ff.h
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
    printf("Write: %s", message);
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
                    "%.9s,%d,%d,%lu\r\n", 
                    time,
                    can->id, 
                    can->payload.idx, 
                    can->payload.data.w);
}

static int SPrint_IMU(char *sdcard_write_buf, 
                      size_t bufsize, 
                      IMUData_t *imu, 
                      const char *time) {
    return snprintf(sdcard_write_buf, 
                    bufsize, 
                    "%.9s,%d,%d,%d,%d,%d,%d,%d,%d,%d\r\n",
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
 */
static FRESULT SDCard_Write(FIL fil, const char *fileName, const char *message, uint32_t size) {
    BYTE readBuf[size];
    FRESULT fresult;
    fresult = f_open(&fil, fileName, FA_WRITE | FA_OPEN_APPEND);

    #ifdef DEBUGGINGMODE
  	printf("f_open error (%i)\r\n", fresult);
    #endif
    if (fresult != FR_OK) return fresult;

    //Copy in a string
    strncpy((char*)readBuf, message, strlen(message));
    UINT bytesWrote;
    fresult = f_write(&fil, readBuf, strlen(message), &bytesWrote);

    #ifdef DEBUGGINGMODE
  	printf("f_write error (%i)\r\n", (int)fresult);
    #endif
    if (fresult != FR_OK) return fresult;

    //close your file!
    f_close(&fil);
    return fresult;
}
