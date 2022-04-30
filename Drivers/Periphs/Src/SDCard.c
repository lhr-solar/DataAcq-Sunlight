#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h> //for va_list var arg functions
#include "fatfs.h"
#include "SDcard.h"
#include "main.h"

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
static FATFS FatFs;
=======
=======
>>>>>>> Removed myprintf
=======
>>>>>>> Added debugging mode and changed function headers
<<<<<<< HEAD
static FRESULT fresult;   //Result after operations
=======
FRESULT fresult;   //Result after operations

UART_HandleTypeDef *huart;  //pointer to UART handler

void SDCard_Init(UART_HandleTypeDef *uartBus){
    huart = uartBus;
}
>>>>>>> Added description comments for globals in Main.h. Made several SD card functions return ErrorStatus instead of spinning infinitely if an error occurs.
<<<<<<< HEAD
>>>>>>> Added description comments for globals in Main.h. Made several SD card functions return ErrorStatus instead of spinning infinitely if an error occurs.
=======
=======
static FRESULT fresult;   //Result after operations
>>>>>>> Removed myprintf
<<<<<<< HEAD
>>>>>>> Removed myprintf

//If debugging mode is set printf's will be enabled and diagnostic information will be printed over UART. 
//This should be disabled when running on system
#define DEBUGGINGMODE   1
=======
=======
static FATFS FatFs;
>>>>>>> Added debugging mode and changed function headers

//If debugging mode is set printf's will be enabled and diagnostic information will be printed over UART. 
//This should be disabled when running on system
#define DEBUGGINGMODE   0
>>>>>>> Added debugging mode and changed function headers

/**
 * @brief Mounts the drive
 * @param None
 * @return FRESULT FR_OK if ok and other errors specified in ff.h
 */
<<<<<<< HEAD
FRESULT SDCard_Init() {
=======
FRESULT SDCard_Init(){
>>>>>>> Added debugging mode and changed function headers
    //mount the drive
    FRESULT fresult = f_mount(&FatFs, "", 1); //1=mount now
    #ifdef DEBUGGINGMODE
    if (fresult != FR_OK) {
<<<<<<< HEAD
<<<<<<< HEAD
  	    printf("f_mount error (%i)\r\n", (int)fresult);
<<<<<<< HEAD
<<<<<<< HEAD
=======
=======
>>>>>>> Added debugging mode and changed function headers
=======
  	    myprintf("f_mount error (%i)\r\n", fresult);
>>>>>>> Added description comments for globals in Main.h. Made several SD card functions return ErrorStatus instead of spinning infinitely if an error occurs.
=======
  	    printf("f_mount error (%i)\r\n", (int)fresult);
>>>>>>> Removed myprintf
        return ERROR;
<<<<<<< HEAD
>>>>>>> Added description comments for globals in Main.h. Made several SD card functions return ErrorStatus instead of spinning infinitely if an error occurs.
=======
=======
>>>>>>> Added debugging mode and changed function headers
>>>>>>> Added debugging mode and changed function headers
    }
    #endif
    return fresult;
}

<<<<<<< HEAD
 * @brief Reads how much memory is left in SD card-> Should be used for debugging purposes
 * @param None
 * @return FRESULT FR_OK if ok and other errors specified in ff.h
 */
FRESULT SDCard_GetStatistics() {
=======
/**
 * @brief Reads how much memory is left in SD Card. Should be used for debugging purposes
 * @param None
 * @return FRESULT FR_OK if ok and other errors specified in ff.h
 */
FRESULT SDCard_GetStatistics(){
>>>>>>> Added debugging mode and changed function headers
    DWORD free_clusters;
    DWORD free_sectors;
    DWORD total_sectors;
    FATFS *getFreeFs;
<<<<<<< HEAD
<<<<<<< HEAD
    #endif
=======
=======
    FRESULT fresult;
>>>>>>> Added debugging mode and changed function headers

    fresult = f_getfree("", &free_clusters, &getFreeFs);
    #ifdef DEBUGGINGMODE
    if(fresult != FR_OK){
<<<<<<< HEAD
<<<<<<< HEAD
        printf("f_getfree error (%i)\r\n", (int)fresult);
=======
        myprintf("f_getfree error (%i)\r\n", fresult);
>>>>>>> Added description comments for globals in Main.h. Made several SD card functions return ErrorStatus instead of spinning infinitely if an error occurs.
=======
        printf("f_getfree error (%i)\r\n", (int)fresult);
<<<<<<< HEAD
>>>>>>> Removed myprintf
        return ERROR;
=======
>>>>>>> Added debugging mode and changed function headers
    }
<<<<<<< HEAD

>>>>>>> Added description comments for globals in Main.h. Made several SD card functions return ErrorStatus instead of spinning infinitely if an error occurs.
=======
    #endif
>>>>>>> Added debugging mode and changed function headers
    //Formula comes from ChaN's documentation
    total_sectors = (getFreeFs->n_fatent - 2) * getFreeFs->csize;
    free_sectors = free_clusters * getFreeFs->csize;

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
    #ifdef DEBUGGINGMODE
    printf("SD card stats:\r\n%10lu KiB total drive space.\r\n%10lu KiB available.\r\n", total_sectors / 2, free_sectors / 2);
    #endif
=======
=======
>>>>>>> Removed myprintf
=======
>>>>>>> Added debugging mode and changed function headers
<<<<<<< HEAD
    printf("SD card stats:\r\n%10lu KiB total drive space.\r\n%10lu KiB available.\r\n", total_sectors / 2, free_sectors / 2);
=======
    myprintf("SD card stats:\r\n%10lu KiB total drive space.\r\n%10lu KiB available.\r\n", total_sectors / 2, free_sectors / 2);
>>>>>>> Added description comments for globals in Main.h. Made several SD card functions return ErrorStatus instead of spinning infinitely if an error occurs.
=======
    printf("SD card stats:\r\n%10lu KiB total drive space.\r\n%10lu KiB available.\r\n", total_sectors / 2, free_sectors / 2);
>>>>>>> Removed myprintf
    return SUCCESS;
=======
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
>>>>>>> Added debugging mode and changed function headers
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
        //change sprintf to snprintf
        sprintf(message, "CAN ID: %d\n", card.data.CANData.id);
        
        sprintf(message + strlen(message),"Byte: %u\n", card.data.CANData.payload.data.b);
        sprintf(message + strlen(message),"Float: %f\n", card.data.CANData.payload.data.f);
        sprintf(message + strlen(message),"Halfword: %u\n", card.data.CANData.payload.data.h);
        sprintf(message + strlen(message),"Word: %lu\n", card.data.CANData.payload.data.w);
        
    
        fresult=SDCard_Write(file, "CAN_DATA.txt", message ,sizeof(message));
    }

    else if(card.id==IMU_SDCard)//IMU
    {
        char message[500];
        memset(&message, 0, sizeof(card.data.IMUData)); // 

        //convert int to string and append each field to "message"
        sprintf(message, "Accel x: %d\n", card.data.IMUData.accel_x);
        sprintf(message + strlen(message),"Accel y: %d\n", card.data.IMUData.accel_y);
        sprintf(message + strlen(message),"Accel z: %d\n", card.data.IMUData.accel_z);
        sprintf(message + strlen(message),"Mag x: %d\n", card.data.IMUData.mag_x);
        sprintf(message + strlen(message),"Mag y: %d\n", card.data.IMUData.mag_y );
        sprintf(message + strlen(message),"Mag z: %d\n", card.data.IMUData.mag_z );
        sprintf(message + strlen(message), "Gyr x: %d\n",card.data.IMUData.gyr_x );
        sprintf(message + strlen(message),"Gyr y: %d\n", card.data.IMUData.gyr_y );
        sprintf(message + strlen(message), "Gyr z: %d\n",card.data.IMUData.gyr_z );

        fresult= SDCard_Write(file, "IMU_DATA.txt", message, sizeof(message));
    }

    else if(card.id==GPS_SDCard) //GPS
    {
        char message[500];

        //GPS data is already given as a string

        memset(&message, 0, sizeof(card.data.GPSData));
        sprintf(message,"Lat Deg: %s",card.data.GPSData.latitude_Deg) ;

        sprintf(message +strlen(message),"Lat min: %s\n", card.data.GPSData.latitude_Min);
        sprintf(message+strlen(message),"Dir: %s", card.data.GPSData.NorthSouth);
        sprintf(message+strlen(message),"Long Deg: %s", card.data.GPSData.longitude_Deg);
        sprintf(message+strlen(message),"Long Min: %s", card.data.GPSData.longitude_Min);
        sprintf(message+strlen(message),"Dir: %s", card.data.GPSData.EastWest);
        sprintf(message+strlen(message),"Speed in Knots %s", card.data.GPSData.speedInKnots);
        sprintf(message+strlen(message),"Mag Var in Deg: %s", card.data.GPSData.magneticVariation_Deg);
        sprintf(message+strlen(message),"Mag Var Dir: %s", card.data.GPSData.magneticVariation_EastWest);

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
<<<<<<< HEAD
<<<<<<< HEAD
  	    printf("f_open error (%i)\r\n", (int)fresult);
        return ERROR;
    }
    
    printf("File opened for reading!\r\n");
=======
  	    myprintf("f_open error (%i)\r\n");
        return ERROR;
    }
    
    myprintf("File opened for reading!\r\n");
>>>>>>> Added description comments for globals in Main.h. Made several SD card functions return ErrorStatus instead of spinning infinitely if an error occurs.
=======
  	    printf("f_open error (%i)\r\n", (int)fresult);
        return ERROR;
    }
    
    printf("File opened for reading!\r\n");
>>>>>>> Removed myprintf
    
    //We can either use f_read OR f_gets to get data out of files
    //f_gets is a wrapper on f_read that does some string formatting for us
    BYTE readBuf[30];   //30 byte buffer
    TCHAR* rres = f_gets((TCHAR*)readBuf, 30, &fil);
    if(rres != 0) {
  	    printf("Read string from 'test.txt' contents: %s\r\n", (char*)readBuf);
    } else {
<<<<<<< HEAD
<<<<<<< HEAD
  	    printf("f_gets error (%i)\r\n", fresult);
=======
  	    myprintf("f_gets error (%i)\r\n", fresult);
>>>>>>> Added description comments for globals in Main.h. Made several SD card functions return ErrorStatus instead of spinning infinitely if an error occurs.
=======
  	    printf("f_gets error (%i)\r\n", fresult);
>>>>>>> Removed myprintf
    }
>>>>>>> Added description comments for globals in Main.h. Made several SD card functions return ErrorStatus instead of spinning infinitely if an error occurs.

    return fresult;
}
<<<<<<< HEAD

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
=======
*/
>>>>>>> Added debugging mode and changed function headers
