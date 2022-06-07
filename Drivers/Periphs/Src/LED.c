/**
 * @file LED.c
 * @brief Interfaces with LED GPIO's
 * 
 * @copyright Copyright (c) 2022 UT Longhorn Racing Solar
 * 
 */

#include "LED.h"

/** LED Turn On
 * @brief This function turns on an LED
 * @param led is the led to turn on
 * @return none
 */
void LED_On(LED_t led){
    HAL_GPIO_WritePin(GPIOE, led, GPIO_PIN_SET);
}

/** LED Turn Off
 * @brief This function turns off an LED
 * @param led is the led to turn off
 * @return none
 */
void LED_Off(LED_t led){
    HAL_GPIO_WritePin(GPIOE, led, GPIO_PIN_RESET);
}

/** LED Toggle
 * @brief This function toggles an LED
 * @param led is the led to toggle
 * @return none
 */
void LED_Toggle(LED_t led){
    HAL_GPIO_TogglePin(GPIOE, led);
}