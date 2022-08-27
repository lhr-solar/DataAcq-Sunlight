/**
 * @file LED.h
 * @brief Interfaces with LED GPIO's
 * 
 * @note All the LED's are connected to Port E and all 
 * functions are wrappers of HAL GPIO functions
 * @copyright Copyright (c) 2022 UT Longhorn Racing Solar
 * 
 */

#ifndef LED_H
#define LED_H

#include "stm32f4xx.h"

typedef enum {
    // LED labels on the pcb
    HEARTBEAT   = GPIO_PIN_7, 
    BPS         = GPIO_PIN_8, 
    DASHBOARD   = GPIO_PIN_9, 
    MOTOR       = GPIO_PIN_10, 
    MPPT0       = GPIO_PIN_11, 
    MPPT1       = GPIO_PIN_12, 
    ARRAY       = GPIO_PIN_13,

    // Actual uses/definitions for each LED
    DR_TASK     = BPS,          // data reading task led
    DL_TASK     = DASHBOARD,    // data logging task led
    BC_TASK     = MOTOR,        // broadcasting task led
    ETH_CONNECT = MPPT1,        // turns on if ethernet established connection
    INIT_FAIL   = MPPT0,        // turns on if CAN/GPS/IMU initialization fails
    SDC_SYNC    = ARRAY
} LED_t;

/** LED Turn On
 * @brief This function turns on an LED
 * @param led is the led to turn on
 * @return none
 */
void LED_On(LED_t led);

/** LED Turn Off
 * @brief This function turns off an LED
 * @param led is the led to turn off
 * @return none
 */
void LED_Off(LED_t led);

/** LED Toggle
 * @brief This function toggles an LED
 * @param led is the led to toggle
 * @return none
 */
void LED_Toggle(LED_t led);

#endif
