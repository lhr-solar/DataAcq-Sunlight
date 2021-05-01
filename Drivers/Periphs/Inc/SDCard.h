#ifndef SDCARD_H
#define SDCARD_H

#include <stdio.h>
#include <string.h>
#include <stdarg.h> //for va_list var arg functions
#include "fatfs.h"


//All printing done by this module is via UART

/**
 * Initializes this module to print via the user's desired UART bus
 * NOTE: This function must be called before any other SDCard_() functions 
 * @param uartBus pointer to a UART handler
 * @return SUCCESS if no errors, ERROR if some error occured
**/
void SDCard_Init(UART_HandleTypeDef *uartBus);

/**
 * Prints desired message via UART
**/
void myprintf(const char *fmt, ...);

/**
 * Initializes the file drive for the SD card
 * @param FatFs FATFS handle
 * @return SUCCESS if no errors, ERROR if some error occured
**/
ErrorStatus SDCard_OpenFileSystem(FATFS FatFs);

/**
 * Prints the SD card's total drive space and available space.
 * @return SUCCESS if no errors, ERROR if some error occured
**/
ErrorStatus SDCard_GetStatistics();

/**
 * Prints given number of bytes of given text file
 * @param fil file handle
 * @param fileName name of file to be read
 * @param bytes maximum possible number of bytes that can be read
 * @return SUCCESS if no errors, ERROR if some error occured
**/
ErrorStatus SDCard_Read(FIL fil, char fileName[], uint32_t bytes);

/**
 * Writes desired message to a specific file on the card.
 * @param fil file handle
 * @param fileName name of file to be written to
 * @param message string to be written to the file
 * @return SUCCESS if no errors, ERROR if some error occured
**/
ErrorStatus SDCard_Write(FIL fil, char fileName[], char message[], uint32_t bytes);

/**
 * Closes the FATFS file drive
 * NOTE: This function must be called when all SD card operations are over. Similar to how fclose() is required when working with files.
**/
void SDCard_CloseFileSystem();

#endif