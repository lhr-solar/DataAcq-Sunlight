/**
 * @file GPS.c
 * @brief GPS API - PA6H
 * 
 * @copyright Copyright (c) 2022 UT Longhorn Racing Solar
 * 
 */

#include "GPS.h"
#include "main.h"
#include "cmsis_os.h"
#include "config.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define GPS_BUFSIZE     100

enum GPSFields {
    NAME = 0,
    TIME,
    STATUS,
    LATITUDE,
    NS,
    LONGITUDE,
    EW,
    SPEEDINKNOTS,
    COURSEINDEGREES,
    DATE,
    MAGNETICVAR,
    NUMFIELDS
};

char GPSRxDataBuf[GPS_BUFSIZE];
static uint8_t GPSBufIdx = 0;
static uint8_t GPSRxByte;
static QueueHandle_t GPSRxQueue;
static uint32_t GPSDroppedMessages = 0;    // for debugging purposes

/** GPSInit
 * @brief Initialize GPS, configure GPS
 * @return ERROR or SUCCESS if transmit worked
 */
ErrorStatus GPS_Init(){
    //Initializes RX interrupts for GPS communication
    HAL_UART_Receive_IT(&huart1, &GPSRxByte, 1);

    GPSRxQueue = xQueueCreate(GPS_RX_QUEUE_SIZE, sizeof(GPSData_t)); //create queue
    
    /*
     * Each command needs a checksum, the 2-byte hex value following the '*'. 
     * The checksum is the XOR of every character between the '$' and the '*':
     * Ex. 0x32 = 'P' ^ 'M' ^ 'T' ^ 'K' ^ '1' ^ '0' ^ '1'
     */
    const char * const init_commands[] = {
        "PMTK104", //This starts in cold start
        "PMTK220,1000", //This sends data every 1 second
        "PMTK251,9600", //This sets baud rate to 9600 b/s
        "PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0", //This sends us data with GPRMC configuration
        "PMTK386,0.2" //This says a standstill speed is .2m/s
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
        debugprintf("%s", command_buf);

        if (HAL_UART_Transmit(&huart1, (uint8_t *)command_buf, len + 6, 100) != HAL_OK) return ERROR;
        osDelay(500);
    }

    return SUCCESS;
}

/** GPS Read Data
 * Wrapper function for XQueueReceive and should not be called from an ISR
 * 
 * @param Data
 * @return pdTRUE if GPS message was successfully fetched from queue, pdFALSE if queue is empty
 */
BaseType_t GPS_ReadData(GPSData_t *Data){
    return xQueueReceive(GPSRxQueue, Data, (TickType_t)0);
}

/**
 * @brief Fetch number of dropped GPS messages due to queue overfilling.
 *        Included for debug purposes
 * @return Number of dropped messages
 */
uint32_t GPS_FetchDroppedMsgCnt() {
    return GPSDroppedMessages;
}

// Callback for a completed UART Rx transfer.
// Once the Rx transfer is complete, we can parse the input and 
// push to the queue.
static void GPS_Receive() {
    GPSData_t GPSData;
    memset(&GPSData, 0, sizeof(GPSData));
    debugprintf("recieved:%.*s\n\r", GPS_BUFSIZE, GPSRxDataBuf);
    if (strncmp(GPSRxDataBuf, "$GPRMC", sizeof("$GPRMC")-1) == 0) {
        uint16_t idx = 0;
        uint8_t field = 0;
        while (GPSRxDataBuf[idx] != '\0'){
            int structidx = 0;
            while (GPSRxDataBuf[idx] != ',' && field < NUMFIELDS){
                switch(field){
                    case NAME:
                    break;
                    case TIME:
                        if (GPSRxDataBuf[idx] == '.') break;
                        GPSData.time[structidx] = GPSRxDataBuf[idx];
                        structidx++;
                        break;
                    case STATUS:
                        GPSData.status = GPSRxDataBuf[idx];
                        break;
                    case LATITUDE:
                        GPSData.latitude[structidx] = GPSRxDataBuf[idx];
                        structidx++;
                        break;
                    case NS:
                        GPSData.NorthSouth = GPSRxDataBuf[idx];
                        break;
                    case LONGITUDE:
                        GPSData.longitude[structidx] = GPSRxDataBuf[idx];
                        structidx++;
                        break;
                    case EW:
                        GPSData.EastWest = GPSRxDataBuf[idx];
                        break;
                    case SPEEDINKNOTS:
                        GPSData.speedInKnots[structidx] = GPSRxDataBuf[idx];
                        structidx++;
                        break;
                    case COURSEINDEGREES:
                        GPSData.courseInDegrees[structidx] = GPSRxDataBuf[idx];
                        structidx++;
                        break;
                    case MAGNETICVAR:
                        GPSData.magneticVariation[structidx] = GPSRxDataBuf[idx];
                        structidx++;
                        break;
                }
                idx++;
            }
            idx++;
            field++;
        }

        if (xQueueSendToBackFromISR(GPSRxQueue, &GPSData, NULL) == errQUEUE_FULL) {
            GPSDroppedMessages++;   // for debugging and metrics purposes
        }
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
    