#include "GPS.h"
#include <stdio.h>

#define GPS_BUFSIZE     100
char GPSRxDataBuf[GPS_BUFSIZE];
static uint8_t GPSBufIdx = 0;

static QueueHandle_t GPSRxQueue;
UART_HandleTypeDef *GPS_UARTHandle;
uint32_t GPSDroppedMessages = 0;    // for debugging purposes

ErrorStatus GPS_Init(UART_HandleTypeDef *huart){
    GPS_UARTHandle = huart;
    GPSRxQueue = xQueueCreate(GPS_RX_QUEUE_SIZE, sizeof(GPSData_t)); //create queue
    
    /*
     * The first command starts the module with "Hot Start" using all previous data stored
     * The second command sends us only the information we want to recieve (nothing about satellites)
     * Sends data every 100 ms.
     * The third command says at what velocity to stop moving car (.2m/s at the moment)
     * 4th command says to increase baud rate to 115200
     * 
     * Each command needs a checksum, the 2-byte hex value following the '*'. 
     * The checksum is the XOR of every character between the '$' and the '*':
     * Ex. 0x32 = 'P' ^ 'M' ^ 'T' ^ 'K' ^ '1' ^ '0' ^ '1'
     */
    uint8_t initCommands[] = "$PMTK101*32\r\n$PMTK220,1000*1F\r\n$PMTK314,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*2A\r\n$PMTK386,0.2*3F\r\n";

    char *init_commands[] = {
        {"PMTK101"},
        {"PMTK220,1000"},
        {"PMTK314,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0"},
        {"PMTK386,0.2"}
    };
    char command_buf[64];

    for (uint8_t i = 0; i < sizeof(init_commands)/sizeof(*init_commands), i++) {
        command_buf[0] = '$';
        memcpy(&command_buf[1], init_commands[i], strlen(init_commands[i]));
    }

    // Initialize interrupts

    if (HAL_UART_Transmit(GPS_UARTHandle, initCommands, sizeof(initCommands)-1, 100) != HAL_OK) return ERROR;
    return SUCCESS;
}

void GPS_StartReading() {
    HAL_UART_Receive_IT(GPS_UARTHandle, (uint8_t *)&GPSRxDataBuf[GPSBufIdx], 1);
}

BaseType_t GPS_ReadData(GPSData_t *Data){
    return xQueueReceive(GPSRxQueue, Data, (TickType_t)0);
}

// Callback for a completed UART Rx transfer.
// Once the Rx transfer is complete, we can parse the input and 
// push to the queue.

static void GPS_Receive() {
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

        if (xQueueSendToBackFromISR(GPSRxQueue, &GPSData, NULL) == errQUEUE_FULL) {
            GPSDroppedMessages++;   // for debugging and metrics purposes
        }
        printf("Sent to queue\n\r");
    }
    else {
        // printf("Wrong GPS data type recieved\n\r");
    }

    
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart == GPS_UARTHandle) { 
        if (GPSRxDataBuf[GPSBufIdx] == '\r') {  // message end sequence is CRLF, we check for LF
            GPSRxDataBuf[GPSBufIdx] = '\0'; // add null terminator to input string
            GPSBufIdx = 0;  // reset buffer
            GPS_Receive();
        }
        
        HAL_UART_Receive_IT(GPS_UARTHandle, (uint8_t *)&GPSRxDataBuf[GPSBufIdx], 1);
        GPSBufIdx = (GPSBufIdx + 1) % GPS_BUFSIZE;  // TODO: 
    }
}