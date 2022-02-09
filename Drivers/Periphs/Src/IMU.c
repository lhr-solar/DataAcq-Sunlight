#include "IMU.h"
#include <stdbool.h>
#include <stdio.h>

HAL_StatusTypeDef error;

#define DEV_ADDR 0x28
#define ADDR (DEV_ADDR << 1)

// Register definitions

#define ACC_NM_SET     0x16
#define ACC_CONFIG     0x08
#define GYR_CONFIG     0x0A
#define MAG_CONFIG     0x09
#define UNIT_SEL       0x3B
#define PWR_MODE       0x3E
#define SYS_TRIGGER    0x3F

#define CHIP_ID        0x00
#define OPR_MODE       0x3D
#define PAGE_ID        0x07
#define ACC_DATA_X_LSB 0x08
#define ACC_DATA_X_MSB 0x09
#define ACC_DATA_Y_LSB 0x0A
#define ACC_DATA_Y_MSB 0x0B
#define ACC_DATA_Z_LSB 0x0C
#define ACC_DATA_Z_MSB 0x0D
#define MAG_DATA_X_LSB 0x0E
#define MAG_DATA_X_MSB 0x0F
#define MAG_DATA_Y_LSB 0x10
#define MAG_DATA_Y_MSB 0x11
#define MAG_DATA_Z_LSB 0x12
#define MAG_DATA_Z_MSB 0x13
#define GYR_DATA_X_LSB 0x14
#define GYR_DATA_X_MSB 0x15
#define GYR_DATA_Y_LSB 0x16
#define GYR_DATA_Y_MSB 0x17
#define GYR_DATA_Z_LSB 0x18
#define GYR_DATA_Z_MSB 0x19

// Because I don't like writing this out every time
#define SEND(data, size) \
    HAL_I2C_Master_Transmit(&hi2c1, ADDR, (data), (size), HAL_MAX_DELAY)

// Just used to wait for the IMU to power on.
static void IMU_WaitForPower() {
    uint8_t config[2];

    // Wait for the chip to power on
    // The CHIP_ID register (0x00) is set to a fixed value of 0xA0
    config[0] = 0;
    while(config[0] != 0xA0) {
        //printf("Waiting for IMU to initialize...\n\r");
        HAL_I2C_Mem_Read(&hi2c1, ADDR, CHIP_ID, I2C_MEMADD_SIZE_8BIT, config, 1, HAL_MAX_DELAY);
    }
    printf("IMU is now responding! Continuing...\n\r");
}

ErrorStatus IMU_Init(){
    // Unfortunately, there is a lot of black magic in this initialization.
    // There's a log of writing default values to registers that should
    // already contain those values, but there appears to be something about
    // this that legitimately makes a difference.
    //
    // My recommendation, don't cut out anything here without extensive
    // testing.

    uint8_t config[4];

    // Pull the reset pin low, then high to reset the IMU
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);
    for(volatile int i = 0; i < 10; i++);   // Give it a moment of time, WAS i<10
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);

    IMU_WaitForPower();
    printf("check 2 ");

    // Verify our current mode
    HAL_I2C_Mem_Read(&hi2c1, ADDR, OPR_MODE, I2C_MEMADD_SIZE_8BIT, config, 1, HAL_MAX_DELAY);
    if((config[0] & 0xF) != 0) {
        // Need to enter configuration mode first
        config[0] = OPR_MODE;
        config[1] = 0;
        SEND(config, 2);
        // Wait for 50 ms. This is well over the amount of time required for this, but whatever.
        HAL_Delay(50);  // TODO: replace with a freertos delay if we plan on multithreading by this point, WAS 50
    }

    // The register map in this chip is split into multiple pages. Select page 0.
    config[0] = PAGE_ID;
    config[1] = 0;
    SEND(config, 2);

    // Reset the chip. We kind of did this above, but we're doing it again to be sure
    config[0] = SYS_TRIGGER;
    config[1] = 0x20;
    SEND(config, 2);

    // Wait for the reset to complete
    HAL_Delay(1000);    // TODO: replace with a freertos delay if we plan on multithreading by this point, WAS 1000

    // Select the power mode
    config[0] = PWR_MODE;
    config[1] = 0;
    SEND(config, 2);

    // Set the clock source
    config[0] = SYS_TRIGGER;
    config[1] = 0x00;
    SEND(config, 2);

    config[0] = UNIT_SEL;
    config[1] = 0;
    error = SEND(config, 2); //Read in m/s^2, Celcius, and degrees
    if (error != HAL_OK){return ERROR;}

    // Now configure for our operation mode
    // IMPORTANT: this needs to be the last configuration register written
    config[0] = OPR_MODE; //register address
    config[1] = 0xC; // The configuration mode (NDOF)   // TODO: think about lower-power modes
    // TODO: experiencing difficulties with the IT version, but these might need to be blocking anyways, or at least need a check
    error = SEND(config, 2); //Turn on Accelerometer, Gyroscope, and Magnetometer 
    if (error != HAL_OK){return ERROR;}

    // Wait for 20ms for the operating mode to change. This is well over the amount of time required, but whatever.
    HAL_Delay(20); //WAS 20
    
    return SUCCESS;
}

ErrorStatus IMU_GetMeasurements(IMUData_t *Data){
    uint8_t reg_addr = ACC_DATA_X_LSB;
    
    error = HAL_I2C_Mem_Read(&hi2c1, ADDR, reg_addr, I2C_MEMADD_SIZE_8BIT, (uint8_t*)Data, 18, HAL_MAX_DELAY);
    if (error != HAL_OK){return ERROR;}
    
    return SUCCESS;
}