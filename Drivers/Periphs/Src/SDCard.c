
#include <stdio.h>
#include <string.h>
#include <stdarg.h> //for va_list var arg functions
#include "fatfs.h"
#include "SDCard.h"
#include "main.h"

FRESULT fres;   //Result after operations

UART_HandleTypeDef *huart;  //pointer to UART handler

void SDCard_Init(UART_HandleTypeDef *uartBus){
    huart = uartBus;
}

void myprintf(const char *fmt, ...){
    static char buffer[256];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    int len = strlen(buffer);
    HAL_UART_Transmit(huart, (uint8_t*)buffer, len, -1);
}

void SDCard_OpenFileSystem(FATFS FatFs){
    //mount the drive
    fres = f_mount(&FatFs, "", 1); //1=mount now
    if (fres != FR_OK) {
  	myprintf("f_mount error (%i)\r\n", fres);
  	while(1);
    }
}

void SDCard_GetStatistics(){
    DWORD free_clusters;
    DWORD free_sectors;
    DWORD total_sectors;
    FATFS *getFreeFs;

    fres = f_getfree("", &free_clusters, &getFreeFs);
    if(fres != FR_OK){
        myprintf("f_getfree error (%i)\r\n", fres);
        while(1);
    }

    //Formula comes from ChaN's documentation
    total_sectors = (getFreeFs->n_fatent - 2) * getFreeFs->csize;
    free_sectors = free_clusters * getFreeFs->csize;

    myprintf("SD card stats:\r\n%10lu KiB total drive space.\r\n%10lu KiB available.\r\n", total_sectors / 2, free_sectors / 2);
}

void SDCard_Read(FIL fil, char fileName[], uint32_t bytes){
    //open file for reading
    fres = f_open(&fil, fileName, FA_READ);
    if (fres != FR_OK) {
  	    myprintf("f_open error (%i)\r\n");
  	while(1);
    }
        myprintf("File opened for reading!\r\n");
    
    //We can either use f_read OR f_gets to get data out of files
    //f_gets is a wrapper on f_read that does some string formatting for us
    BYTE readBuf[30];   //30 byte buffer
    TCHAR* rres = f_gets((TCHAR*)readBuf, 30, &fil);
    if(rres != 0) {
  	    myprintf("Read string from 'test.txt' contents: %s\r\n", readBuf);
    } else {
  	    myprintf("f_gets error (%i)\r\n", fres);
    }

    f_close(&fil);
}

void SDCard_Write(FIL fil, char fileName[], char message[], uint32_t bytes){
    BYTE readBuf[bytes];
    fres = f_open(&fil, fileName, FA_WRITE | FA_OPEN_ALWAYS | FA_CREATE_ALWAYS);
    if(fres == FR_OK) {
  	    myprintf("I was able to open 'write.txt' for writing\r\n");
    } else {
  	    myprintf("f_open error (%i)\r\n", fres);
    }

    //Copy in a string
    strncpy((char*)readBuf, message, strlen(message));
    UINT bytesWrote;
    fres = f_write(&fil, readBuf, strlen(message), &bytesWrote);
    if(fres == FR_OK) {
  	myprintf("Wrote %i bytes to 'write.txt'!\r\n", bytesWrote);
    } else {
  	myprintf("f_write error (%i)\r\n");
    }

    //close your file!
    f_close(&fil);
}

void SDCard_CloseFileSystem(){
    //de-mount the drive
    f_mount(NULL, "", 0);
}