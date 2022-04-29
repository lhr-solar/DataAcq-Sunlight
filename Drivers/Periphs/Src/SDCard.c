#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h> //for va_list var arg functions
#include "fatfs.h"
#include "SDCard.h"
#include "main.h"

static FATFS FatFs;

//If debugging mode is set printf's will be enabled and diagnostic information will be printed over UART. 
//This should be disabled when running on system
#define DEBUGGINGMODE   0

/**
 * @brief Mounts the drive
 * @param None
 * @return FRESULT FR_OK if ok and other errors specified in ff.h
 */
FRESULT SDCard_Init(){
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
 * @brief Reads how much memory is left in SD Card. Should be used for debugging purposes
 * @param None
 * @return FRESULT FR_OK if ok and other errors specified in ff.h
 */
FRESULT SDCard_GetStatistics(){
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
FRESULT SDCard_Write(FIL fil, char fileName[], char message[], uint32_t size){
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

FRESULT SDCard_Sort_Write_Data(SDCard_t card)
{
    //check if data from queue is from imu, gps, or can. How to determine what data is what from queue?
    //if imu, convert to char
    //gps is already char
    //don't convert can
    //send data to corresponding file in sd card
    //data reading task reads data into imu/gps/can. then this data is read into corresponding fields for sdcard. 
    //then this function is called to pick through the data and format it. Then its printed to sd card


    FRESULT fresult=FR_OK;
    FIL file;
    
    //check ID of qdata for type of message, adjust message once we know what kind of message we are dealing with
    if(card.id==CAN_SDCard)//CAN
    {
        //card.length=sizeof(card.data.CANData);
        //char message[card.length];
        char message[500];
         memset(message, 0, sizeof(card.data.CANData.payload.data)+sizeof(card.data.CANData.id));
        // memcpy(&message, card.data.CANData, sizeof(card.data.CANData));

        sprintf(message, " %d", card.data.CANData.id);
        
        sprintf(message + strlen(message)," %d", card.data.CANData.payload.data.b);
        sprintf(message + strlen(message)," %f", card.data.CANData.payload.data.f);
        sprintf(message + strlen(message)," %d", card.data.CANData.payload.data.h);
        sprintf(message + strlen(message)," %lu", card.data.CANData.payload.data.w);
        
    
        fresult=SDCard_Write(file, "CAN_DATA.txt", message ,sizeof(message));
    }

    else if(card.id==IMU_SDCard)//IMU
    {
        //card.length=sizeof(card.data.IMUData);
        //how much space does a 16 bit signed int take in a string?? message should start at different points for each convewrsion
        //int16_t is 2 bytes
        // char* ax;
        // char* ay;
        // char* az;
        // char* mx;
        // char* my;
        // char* mz;
        // char* gx;
        // char* gy;
        // char* gz;

        char message[500];
        memset(&message, 0, sizeof(card.data.IMUData)); // 

        //convert int to string and append each field to "message"
        sprintf(message, " %d", card.data.IMUData.accel_x);
        
        sprintf(message + strlen(message)," %d", card.data.IMUData.accel_y);
        
        sprintf(message + strlen(message)," %d", card.data.IMUData.accel_z);
        
        sprintf(message + strlen(message)," %d", card.data.IMUData.mag_x);
        
        sprintf(message + strlen(message)," %d", card.data.IMUData.mag_y );
        
        sprintf(message + strlen(message)," %d", card.data.IMUData.mag_z );
        
        sprintf(message + strlen(message), " %d",card.data.IMUData.gyr_x );
        
        sprintf(message + strlen(message)," %d", card.data.IMUData.gyr_y );
        
        sprintf(message + strlen(message), " %d",card.data.IMUData.gyr_z );
        sprintf(message+strlen(message), " \n");


        // sprintf(message, ax);
        // sprintf(message, ay);
        // sprintf(message, az);
        // sprintf(message, mx);
        // sprintf(message, my);
        // sprintf(message, mz);
        // sprintf(message, gx);
        // sprintf(message, gy);
        // sprintf(message, gz);
        
        //memset(&message, 0, sizeof(card.data.IMUData));
        //memcpy(&message, card.data.IMUData, sizeof(card.data.IMUData));
        fresult= SDCard_Write(file, "IMU_DATA.txt", message, sizeof(message));
    }

    else if(card.id==GPS_SDCard) //GPS
    {
        char message[500];

        //GPS data is already given as a string

        memset(&message, 0, sizeof(card.data.GPSData));
        sprintf(message," %s",card.data.GPSData.latitude_Deg) ;

        sprintf(message +strlen(message)," %s", card.data.GPSData.latitude_Min);
        sprintf(message+strlen(message)," %s", card.data.GPSData.NorthSouth);
        sprintf(message+strlen(message)," %s", card.data.GPSData.longitude_Deg);
        sprintf(message+strlen(message)," %s", card.data.GPSData.longitude_Min);
        sprintf(message+strlen(message)," %s", card.data.GPSData.EastWest);
        sprintf(message+strlen(message)," %s", card.data.GPSData.speedInKnots);
        sprintf(message+strlen(message)," %s", card.data.GPSData.magneticVariation_Deg);
        sprintf(message+strlen(message)," %s", card.data.GPSData.magneticVariation_EastWest);

        //memset(&message, 0, sizeof(card.data.GPSData));
        //memcpy(&message, card.data.GPSData, sizeof(card.data.GPSData));
        fresult= SDCard_Write(file, "GPS_DATA.txt", message, sizeof(message));
    }

    return fresult;

}

/**
 * @brief Unmounts the drive
 * @param None
 * @return FRESULT FR_OK if ok and other errors specified in ff.h
 */
FRESULT SDCard_CloseFileSystem(){
    //un-mount the drive
    return f_mount(NULL, "", 0);
}

//WON'T BE READING FROM SDCARD THROUGH MCU BUT JUST LEAVING FUNCTION JUST IN CASE
/**
 * Prints given number of bytes of given text file
 * @param fil file handle
 * @param fileName name of file to be read
 * @param bytes maximum possible number of bytes that can be read
 * @return SUCCESS if no errors, ERROR if some error occured
**/
/*
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
*/
