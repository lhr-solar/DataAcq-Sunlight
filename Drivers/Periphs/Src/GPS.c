#include "GPS.h"
#include <stdio.h>

static QueueHandle_t GPSRxQueue;

ErrorStatus GPS_Init(void){
    GPSRxQueue = xQueueCreate(GPS_RX_QUEUE_SIZE, sizeof(GPSData_t)); //create queue
    //The first command starts the module with "Hot Start" using all previous data stored
    //The second command sends us only the information we want to recieve (nothing about satellites)
    //Sends data every 100 ms.
    //The third command says at what velocity to stop moving car (.2m/s at the moment)


    //uint8_t initCommands[100] = {"$PMTK101*32\r\n$PMTK220,1000*2F\r\n$PMTK314,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*2C\r\n$PMTK386,0.2*3F\r\n"}; 

    //try 17 instead of 19 fields
    uint8_t initCommands[100] = {"$PMTK101*32\r\n$PMTK220,100*1F\r\n$PMTK314,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*2A\r\n$PMTK386,0.2*3F\r\n"};
    

    //uint8_t initCommands[100] = {"$PMTK101*32\r\n$PMTK220,100*1F\r\n$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*2C\r\n$PMTK386,0.2*3F\r\n"}; 

    //send each command one at a time, read uart to see ack packet. How to read ack packet?


    //initCommands[99]=0;
    //Non-Blocking transmit
    if (HAL_UART_Transmit_IT(&huart1, initCommands, sizeof(initCommands)) != HAL_OK) return ERROR;
    return SUCCESS;
}

ErrorStatus GPS_UpdateMeasurements(void){
    GPSData_t GPSData;
    char data[75]; //plus one for null terminator?? 76
    //Non-Blocking receive. The number of bytes received will need to be tested based on whether we are receiving
    //characters for some fields or numbers
    //ex. $GPRMC,064951.000,A,2307.1256,N,12016.4438,E,0.03,165.48,260406,3.05,W,A*2C (75 characters)


    //error = HAL_UART_Receive_IT(&huart1, (uint8_t*)data, 72); //returning hal busy probably, causing error
    //was originally HAL_UART_Receive_IT
    if (HAL_UART_Receive(&huart1, (uint8_t*)data, 75, 1) != HAL_OK) {
        printf("Error Rx\n\r");
        return ERROR;
    }
    //while (HAL_UART_Receive_IT(&huart1, (uint8_t*)data, 72) != HAL_OK);
    //data[6] = '\0'; //replace comma with null character for strcmp
    //data[75]='\0';
    printf("%s\n\r", data);
    data[6] = '\0'; //replace comma with null character for strcmp
    if (strcmp(data, "$GPRMC") == 0){ //return ERROR; //if we receive data that we do not care about //THIS IS TRIGGERING ERROR MESSAGE IN GPSTest LOOP!!!
        GPSData.latitude_Deg[0] = data[20];
        GPSData.latitude_Deg[1] = data[21];
        GPSData.latitude_Deg[2] = data[22];
        GPSData.latitude_Deg[3] = data[23];

        GPSData.latitude_Min[0] = data[25];
        GPSData.latitude_Min[1] = data[26];
        GPSData.latitude_Min[2] = data[27];
        GPSData.latitude_Min[3] = data[28];
        //GPSData.latitude_Min[4] = data[27];
        //GPSData.latitude_Min[5] = data[28];
        GPSData.NorthSouth = data[40]; //was 30
        GPSData.longitude_Deg[0] = data[32];
        GPSData.longitude_Deg[1] = data[33];
        GPSData.longitude_Deg[2] = data[34];
        GPSData.longitude_Deg[3] = data[35];
        GPSData.longitude_Deg[4] = data[36];

        GPSData.longitude_Min[0] = data[38];
        GPSData.longitude_Min[1] = data[39];
        GPSData.longitude_Min[2] = data[40];
        GPSData.longitude_Min[5] = data[41];
        GPSData.EastWest = data[43];
        GPSData.speedInKnots[0] = data[45];
        GPSData.speedInKnots[1] = data[46]; //decimal point
        GPSData.speedInKnots[2] = data[47];
        GPSData.speedInKnots[2] = data[48]; 
        //.
        //GPSData.speedInKnots[3] = data[44]; //the decimal part
        GPSData.magneticVariation_Deg[0] = data[64];
        GPSData.magneticVariation_Deg[1] = data[65]; //the decimal point
        GPSData.magneticVariation_Deg[2] = data[66];
        GPSData.magneticVariation_Deg[3] = data[67]; 
        GPSData.magneticVariation_EastWest = data[69];
    }
    else {
        data[6] = ',';
         printf("%s\n\r", data);
        return ERROR;
    }
    printf("Receive to queue");
    if (xQueueSendToBack(GPSRxQueue, &GPSData, (TickType_t)0) == errQUEUE_FULL) return ERROR; //never received by q
    printf("Sent to queue\n\r");
    return SUCCESS;
}

BaseType_t GPS_ReadData(GPSData_t *Data){
    return xQueueReceive(GPSRxQueue, Data, (TickType_t)0);
}