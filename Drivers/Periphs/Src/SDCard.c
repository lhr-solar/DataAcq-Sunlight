#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h> //for va_list var arg functions
#include "fatfs.h"
#include "SDcard.h"
#include "main.h"

static FATFS FatFs;

//If debugging mode is set printf's will be enabled and diagnostic information will be printed over UART. 
//This should be disabled when running on system
#define DEBUGGINGMODE   1

/**
 * @brief Mounts the drive
 * @param None
 * @return FRESULT FR_OK if ok and other errors specified in ff.h
 */
FRESULT SDCard_Init() {
    //mount the drive
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

/**
 * @brief Writes data to SD Card
 * @param fil File object structure. Will be initialized if not already
 * @param fileName Name of file to write to. Will be created if not existing. Appends data to end of file
 * @param message char array of data to write to SD Card
 * @param size size of data to write to file
 * @return FRESULT FR_OK if ok and other errors specified in ff.h
 */
FRESULT SDCard_Write(FIL fil, const char *fileName, const char *message, uint32_t size) {
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

// Wrappers for snprintf() for each of the message data types
// Each function takes a buffer to write to, the buffer size, 
// and a pointer to the respective data structs

static int SPrint_CAN(char *sdcard_write_buf, 
                      size_t bufsize, 
                      CANMSG_t *can, 
                      const char *time) {
    uint32_t data;

    switch (can->id) {
    // Handle messages with one byte of data
    case TRIP:
    case ALL_CLEAR:
    case CONTACTOR_STATE:
    case WDOG_TRIGGERED:
    case CAN_ERROR:
    case CHARGE_ENABLE:
        data = can->payload.data.b;
        break;

    // Handle messages with 4 byte data and optionally idx
    case CURRENT_DATA:
    case SOC_DATA:
    case VOLT_DATA:
    case TEMP_DATA:
        data = can->payload.data.w;
        break;

    // Handle invalid messages
    // TODO: add controls CAN messages
    default:
        return -1;
    }

    return snprintf(sdcard_write_buf, 
                    bufsize, 
                    "%s,%d,%d,%lu\r\n", 
                    time,
                    can->id, 
                    can->payload.idx, 
                    data);
}

static int SPrint_IMU(char *sdcard_write_buf, 
                      size_t bufsize, 
                      IMUData_t *imu, 
                      const char *time) {
    return snprintf(sdcard_write_buf, 
                    bufsize, 
                    "%s,%d,%d,%d,%d,%d,%d,%d,%d,%d\r\n",
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
                    "%s,%s\r\n",
                    time,
                    gps_str);
}


static const char * const filenames_list[] = {
        "can.csv",
        "imu.csv",
        "gps.csv"
    };
FRESULT SDCard_Sort_Write_Data(SDCard_t *carddata, const char *time) {
    // check if data from queue is from imu, gps, or can. How to determine what data is what from queue?
    // if imu, convert to char
    // gps is already char
    // don't convert can
    // send data to corresponding file in sd card
    // data reading task reads data into imu/gps/can. then this data is read into corresponding fields for sdcard-> 
    // then this function is called to pick through the data and format it. Then its printed to sd card

    enum filenames_idx {CAN_FNAME = 0, IMU_FNAME, GPS_FNAME};
    static char message[SDCARD_WRITE_BUFSIZE];
    
    FIL file;
    uint8_t fname_idx = 0;
    uint16_t bytes_written = -1;

    // check ID of qdata for type of message, adjust message once we know what kind of message we are dealing with
    switch (carddata->id) {

    case CAN_SDCard:
        fname_idx = CAN_FNAME;
        bytes_written = SPrint_CAN(message, 
                                   SDCARD_WRITE_BUFSIZE, 
                                   &carddata->data.CANData,
                                   time);
        break;
        
    case IMU_SDCard:
        fname_idx = IMU_FNAME;
        bytes_written = SPrint_IMU(message, 
                                   SDCARD_WRITE_BUFSIZE, 
                                   &carddata->data.IMUData,
                                   time);
        break;

    case GPS_SDCard:
        fname_idx = GPS_FNAME;
        bytes_written = SPrint_GPS(message, 
                                   SDCARD_WRITE_BUFSIZE, 
                                   &carddata->data.GPSData,
                                   time);
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
