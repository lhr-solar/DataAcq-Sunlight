#include <stdio.h>
#include <string.h>
#include <stdarg.h> //for va_list var arg functions
#include "fatfs.h"
#include "SDCard.h"
#include "main.h"

static FRESULT fresult;   //Result after operations

void SDCard_Init(){
    
}

ErrorStatus SDCard_OpenFileSystem(FATFS FatFs){
    //mount the drive
    fresult = f_mount(&FatFs, "", 1); //1=mount now
    if (fresult != FR_OK) {
  	    printf("f_mount error (%i)\r\n", (int)fresult);
        return ERROR;
    }
    return SUCCESS;
}

ErrorStatus SDCard_GetStatistics(){
    DWORD free_clusters;
    DWORD free_sectors;
    DWORD total_sectors;
    FATFS *getFreeFs;

    fresult = f_getfree("", &free_clusters, &getFreeFs);
    if(fresult != FR_OK){
        printf("f_getfree error (%i)\r\n", (int)fresult);
        return ERROR;
    }

    //Formula comes from ChaN's documentation
    total_sectors = (getFreeFs->n_fatent - 2) * getFreeFs->csize;
    free_sectors = free_clusters * getFreeFs->csize;

    printf("SD card stats:\r\n%10lu KiB total drive space.\r\n%10lu KiB available.\r\n", total_sectors / 2, free_sectors / 2);
    return SUCCESS;
}

ErrorStatus SDCard_Read(FIL fil, char fileName[], uint32_t bytes){
    //open file for reading
    fresult = f_open(&fil, fileName, FA_READ);
    if (fresult != FR_OK) {
  	    printf("f_open error (%i)\r\n", (int)fresult);
        return ERROR;
    }
    
    printf("File opened for reading!\r\n");
    
    //We can either use f_read OR f_gets to get data out of files
    //f_gets is a wrapper on f_read that does some string formatting for us
    BYTE readBuf[30];   //30 byte buffer
    TCHAR* rres = f_gets((TCHAR*)readBuf, 30, &fil);
    if(rres != 0) {
  	    printf("Read string from 'test.txt' contents: %s\r\n", (char*)readBuf);
    } else {
  	    printf("f_gets error (%i)\r\n", fresult);
    }

    f_close(&fil);
    return SUCCESS;
}

ErrorStatus SDCard_Write(FIL fil, char fileName[], char message[], uint32_t bytes){
    BYTE readBuf[bytes];
    fresult = f_open(&fil, fileName, FA_WRITE | FA_OPEN_ALWAYS | FA_CREATE_ALWAYS);
    if(fresult == FR_OK) {
  	    printf("I was able to open 'write.txt' for writing\r\n");
    } else {
  	    printf("f_open error (%i)\r\n", fresult);
        return ERROR;
    }

    //Copy in a string
    strncpy((char*)readBuf, message, strlen(message));
    UINT bytesWrote;
    fresult = f_write(&fil, readBuf, strlen(message), &bytesWrote);
    if(fresult == FR_OK) {
  	    printf("Wrote %i bytes to 'write.txt'!\r\n", bytesWrote);
    } else {
  	    printf("f_write error (%i)\r\n");
        return ERROR;
    }

    //close your file!
    f_close(&fil);
    return SUCCESS;
}

void SDCard_CloseFileSystem(){
    //de-mount the drive
    f_mount(NULL, "", 0);
}