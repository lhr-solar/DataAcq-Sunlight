#include "GPS.h"

char data[75];
GPSData_t* GPSData;
HAL_StatusTypeDef error;

ErrorStatus GPS_Init(GPSData_t *Data){
    //The first command starts the module with "Hot Start" using all previous data stored
    //The second command sends us only the information we want to recieve (nothing about satellites)
    //The third command says at what velocity to stop moving car (.2m/s at the moment)
    uint8_t initCommands[100] = {"$PMTK101*32\r\n$PMTK220,1000*2F\r\n$PMTK314,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*2C\r\n$PMTK386,0.2*3F\r\n"}; 
    //Non-Blocking transmit
    error = HAL_UART_Transmit_IT(&huart1, initCommands, sizeof(initCommands));
    if (error != HAL_OK) return ERROR;
    GPSData = Data;
    return SUCCESS;
}

ErrorStatus GPS_UpdateMeasurements(void){
    //Non-Blocking receive. The number of bytes received will need to be tested based on whether we are receiving
    //characters for some fields or numbers
    //ex. $GPRMC,064951.000,A,2307.1256,N,12016.4438,E,0.03,165.48,260406,3.05,W,A*2C (75 characters)
    error = HAL_UART_Receive_IT(&huart1, data, 75);
    if (error != HAL_OK) return ERROR;
    data[6] = "\n"; //replace comma with newline
    if (strcmp(data, "$GPRMC")) return; //if we receive data that we do not care about
    (*GPSData).latitude_Deg[0] = data[20];
    (*GPSData).latitude_Deg[1] = data[21];
    (*GPSData).latitude_Min[0] = data[22];
    (*GPSData).latitude_Min[1] = data[23];
    (*GPSData).latitude_Min[2] = data[25];
    (*GPSData).latitude_Min[3] = data[26];
    (*GPSData).latitude_Min[4] = data[27];
    (*GPSData).latitude_Min[5] = data[28];
    (*GPSData).NorthSouth = data[30];
    (*GPSData).longitude_Deg[0] = data[32];
    (*GPSData).longitude_Deg[1] = data[33];
    (*GPSData).longitude_Deg[2] = data[34];
    (*GPSData).longitude_Min[0] = data[35];
    (*GPSData).longitude_Min[1] = data[36];
    (*GPSData).longitude_Min[2] = data[38];
    (*GPSData).longitude_Min[3] = data[39];
    (*GPSData).longitude_Min[4] = data[40];
    (*GPSData).longitude_Min[5] = data[41];
    (*GPSData).EastWest = data[43];
    (*GPSData).speedInKnots[0] = data[45];
    (*GPSData).speedInKnots[1] = data[46];
    (*GPSData).speedInKnots[2] = data[47];
    (*GPSData).speedInKnots[3] = data[48];
    (*GPSData).magneticVariation_Deg[0] = data[64];
    (*GPSData).magneticVariation_Deg[1] = data[65];
    (*GPSData).magneticVariation_Deg[2] = data[66];
    (*GPSData).magneticVariation_Deg[3] = data[67];
    (*GPSData).magneticVariation_EastWest = data[69];
    return SUCCESS;
}