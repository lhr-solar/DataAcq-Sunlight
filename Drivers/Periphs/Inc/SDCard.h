#ifndef SDCARD_H
#define SDCARD_H

#include <stdio.h>
#include <string.h>
#include <stdarg.h> //for va_list var arg functions
#include "fatfs.h"
#include "CANBus.h"
#include "IMU.h"
#include "GPS.h"

<<<<<<< HEAD
#define SDCARD_WRITE_BUFSIZE        64
=======

//All printing done by this module is via UART
>>>>>>> Fixed header guard.

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
    SDCardID_t id;
	SDCardData_t data;
} SDCard_t;

<<<<<<< HEAD
=======
/**
 * Prints desired message via UART
**/
void myprintf(const char *fmt, ...);
>>>>>>> Added description comments for globals in Main.h. Made several SD card functions return ErrorStatus instead of spinning infinitely if an error occurs.

/**
 * @brief Mounts the drive
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
 * @brief Sorts data from IMU, GPS, and CAN and prints them to corresponding files on SD card 
 * @param carddata SDCard object structure. 
 * @return FRESULT FR_OK if ok and other errors specified in ff.h
 */
FRESULT SDCard_Sort_Write_Data(SDCard_t *carddata, const char *time);

#endif