#include "GPS.h"
#include <stdio.h>
#include "cmsis_os.h"

#define GPS_BUFSIZE     100
char GPSRxDataBuf[GPS_BUFSIZE];
static uint8_t GPSBufIdx = 0;
static uint8_t GPSRxByte;

static QueueHandle_t GPSRxQueue;
uint32_t GPSDroppedMessages = 0;    // for debugging purposes

ErrorStatus GPS_Init(){
    GPSRxQueue = xQueueCreate(GPS_RX_QUEUE_SIZE, sizeof(GPSData_t)); //create queue
    
    /*
     * Each command needs a checksum, the 2-byte hex value following the '*'. 
     * The checksum is the XOR of every character between the '$' and the '*':
     * Ex. 0x32 = 'P' ^ 'M' ^ 'T' ^ 'K' ^ '1' ^ '0' ^ '1'
     */
    char *init_commands[] = {
        "PMTK104",
        "PMTK220,1000",
        "PMTK251,9600",
        "PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0",
        "PMTK386,0.2"
    };

    char command_buf[64];
    char command_ending[] = "*  \r\n";
    for (uint8_t i = 0; i < sizeof(init_commands)/sizeof(*init_commands); i++) {
        uint8_t len = strlen(init_commands[i]);
        // checksum
        uint8_t checksum = 0;
        for (uint8_t j = 0; j < len; j++) {
            checksum ^= init_commands[i][j];
        }
        char checksum_str[3];
        sprintf(checksum_str, "%.2X", checksum);
        memcpy(&command_ending[1], checksum_str, 2);
        // construct final message
        command_buf[0] = '$';
        memcpy(&command_buf[1], init_commands[i], len);
        memcpy(&command_buf[len + 1], command_ending, sizeof(command_ending));
        // send
        printf("%s", command_buf);
        if (HAL_UART_Transmit(&huart1, (uint8_t *)command_buf, len + 6, 100) != HAL_OK) return ERROR;
        osDelay(500);
    }

    return SUCCESS;
}

void GPS_StartReading() {
    HAL_UART_Receive_IT(&huart1, &GPSRxByte, 1);
}

BaseType_t GPS_ReadData(GPSData_t *Data){
    return xQueueReceive(GPSRxQueue, Data, (TickType_t)0);
}

// Callback for a completed UART Rx transfer.
// Once the Rx transfer is complete, we can parse the input and 
// push to the queue.

static void GPS_Receive() {
    GPSData_t GPSData;
    printf("%s\n", GPSRxDataBuf);
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
        
    }

    
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART1) {
        char byte_received = (char)GPSRxByte;

        GPSRxDataBuf[GPSBufIdx] = byte_received;
        if (byte_received == '\n') {  // message end sequence is CRLF, we check for LF
            GPSRxDataBuf[GPSBufIdx - 1] = '\0'; // add null terminator to input string @ the CR
            GPSBufIdx = 0;  // reset buffer
            GPS_Receive();
        }
        else{
            GPSBufIdx = (GPSBufIdx + 1) % GPS_BUFSIZE;
        }

        HAL_UART_Receive_IT(huart, (uint8_t *)&GPSRxByte, 1);
    }
}