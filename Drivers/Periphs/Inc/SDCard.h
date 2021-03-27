#ifndef SDCARD_H
<<<<<<< HEAD
#define SDCARD_H
=======
#define _SDCARD_H
>>>>>>> First try at SDCard driver library.

#include <stdio.h>
#include <string.h>
#include <stdarg.h> //for va_list var arg functions
#include "fatfs.h"
<<<<<<< HEAD
#include "CANBus.h"
#include "IMU.h"
#include "GPS.h"

#define SDCARD_WRITE_BUFSIZE        64

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
=======

//All printing done by this module is via UART

/**
 * Initializes this module to print via the user's desired UART bus
 * NOTE: This function must be called before any other SDCard_() functions 
 * @param uartBus pointer to a UART handler
**/
void SDCard_Init(UART_HandleTypeDef *uartBus);

/**
 * Prints desired message via UART
**/
void myprintf(const char *fmt, ...);

/**
 * Initializes the file drive for the SD card
 @param FatFs FATFS handle
**/
void SDCard_OpenFileSystem(FATFS FatFs);

/**
 * Prints the SD card's total drive space and avaiable space.
**/
void SDCard_GetStatistics();

/**
 * Prints given number of bytes of given text file
 * @param fil file handle
 * @param fileName name of file to be read
 * @param bytes maximum possible number of bytes that can be read
**/
void SDCard_Read(FIL fil, char fileName[], uint32_t bytes);

/**
 * Writes desired message to a specific file on the card.
 * @param fil file handle
 * @param fileName name of file to be written to
 * @param message string to be written to the file
**/
void SDCard_Write(FIL fil, char fileName[], char message[], uint32_t bytes);

/**
 * Closes the FATFS file drive
 * NOTE: This function must be called when all SD card operations are over. Similar to how fclose() is required when working with files.
**/
void SDCard_CloseFileSystem();
>>>>>>> First try at SDCard driver library.

#endif