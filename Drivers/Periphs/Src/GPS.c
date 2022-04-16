#include "GPS.h"
#include <stdio.h>

static char GPSRxDataBuf[76];
static QueueHandle_t GPSRxQueue;
UART_HandleTypeDef *GPS_UARTHandle;
uint32_t GPSDroppedMessages = 0;    // for debugging purposes

ErrorStatus GPS_Init(UART_HandleTypeDef *huart){
    GPS_UARTHandle = huart;
    GPSRxQueue = xQueueCreate(GPS_RX_QUEUE_SIZE, sizeof(GPSData_t)); //create queue
    
    //The first command starts the module with "Hot Start" using all previous data stored
    //The second command sends us only the information we want to recieve (nothing about satellites)
    //Sends data every 100 ms.
    //The third command says at what velocity to stop moving car (.2m/s at the moment)
    //4th command says to increase baud rate to 115200
    uint8_t initCommands[] = "$PMTK101*32\r\n$PMTK220,100*1F\r\n$PMTK314,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*2C\r\n$PMTK386,0.2*3F\r\n$PMTK251,3115200*27\r\n";

    // Initialize interrupts


    if (HAL_UART_Transmit_IT(&huart1, initCommands, sizeof(initCommands)) != HAL_OK) return ERROR;
    return SUCCESS;
}

HAL_StatusTypeDef GPS_UpdateMeasurements(void){
    //Non-Blocking receive. The number of bytes received will need to be tested based on whether we are receiving
    //characters for some fields or numbers
    //ex. $GPRMC,064951.000,A,2307.1256,N,12016.4438,E,0.03,165.48,260406,3.05,W,A*2C (75 characters)
    printf("waiting for data\n\r");
    return HAL_UART_Receive_IT(&huart1, (uint8_t*)GPSRxDataBuf, 76); //returning hal busy probably, causing error
}

BaseType_t GPS_ReadData(GPSData_t *Data){
    return xQueueReceive(GPSRxQueue, Data, (TickType_t)0);
}

// Callback for a completed UART Rx transfer.
// Once the Rx transfer is complete, we can parse the input and 
// push to the queue.

static void GPS_Recieve() {
    GPSData_t GPSData;
    if (strncmp(GPSRxDataBuf, "$GPRMC", sizeof("$GPRMC")-1) == 0) {
        GPSData.latitude_Deg[0] = GPSRxDataBuf[20];
        GPSData.latitude_Deg[1] = GPSRxDataBuf[21];
        GPSData.latitude_Deg[2] = GPSRxDataBuf[22];
        GPSData.latitude_Deg[3] = GPSRxDataBuf[23];

        GPSData.latitude_Min[0] = GPSRxDataBuf[25];
        GPSData.latitude_Min[1] = GPSRxDataBuf[26];
        GPSData.latitude_Min[2] = GPSRxDataBuf[27];
        GPSData.latitude_Min[3] = GPSRxDataBuf[28];
        //GPSData.latitude_Min[4] = GPSRxDataBuf[27];
        //GPSData.latitude_Min[5] = GPSRxDataBuf[28];
        GPSData.NorthSouth = GPSRxDataBuf[40]; //was 30
        GPSData.longitude_Deg[0] = GPSRxDataBuf[32];
        GPSData.longitude_Deg[1] = GPSRxDataBuf[33];
        GPSData.longitude_Deg[2] = GPSRxDataBuf[34];
        GPSData.longitude_Deg[3] = GPSRxDataBuf[35];
        GPSData.longitude_Deg[4] = GPSRxDataBuf[36];

        GPSData.longitude_Min[0] = GPSRxDataBuf[38];
        GPSData.longitude_Min[1] = GPSRxDataBuf[39];
        GPSData.longitude_Min[2] = GPSRxDataBuf[40];
        GPSData.longitude_Min[5] = GPSRxDataBuf[41];
        GPSData.EastWest = GPSRxDataBuf[43];
        GPSData.speedInKnots[0] = GPSRxDataBuf[45];
        GPSData.speedInKnots[1] = GPSRxDataBuf[46]; //decimal point
        GPSData.speedInKnots[2] = GPSRxDataBuf[47];
        GPSData.speedInKnots[2] = GPSRxDataBuf[48]; 
        //.
        //GPSData.speedInKnots[3] = GPSRxDataBuf[44]; //the decimal part
        GPSData.magneticVariation_Deg[0] = GPSRxDataBuf[64];
        GPSData.magneticVariation_Deg[1] = GPSRxDataBuf[65]; //the decimal point
        GPSData.magneticVariation_Deg[2] = GPSRxDataBuf[66];
        GPSData.magneticVariation_Deg[3] = GPSRxDataBuf[67]; 
        GPSData.magneticVariation_EastWest = GPSRxDataBuf[69];
    }
    else {
        printf("Wrong GPS data type recieved\n\r");
    }

    if (xQueueSendToBackFromISR(GPSRxQueue, &GPSData, NULL) == errQUEUE_FULL) {
        GPSDroppedMessages++;
    }
    printf("Sent to queue\n\r");
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *GPS_UARTHandle) {
    printf("GPS RXNE interrupt\n\r");
    GPS_Recieve();
}