/**
 * @file IMU.c
 * @brief Inertial Measurement Unit API - BNO055
 * 
 * @copyright Copyright (c) 2022 UT Longhorn Racing Solar
 * 
 */

#include "IMU.h"
#include "config.h"
#include <stdbool.h>
#include <stdio.h>
#include "cmsis_os.h"
#include <stdlib.h>

#define DEV_ADDR 0x28
#define ADDR (DEV_ADDR << 1)

//This struct is initialized to values collected in clean environment, they will be overwritten if IMUCalibrate is called
//but will go back to original values at reset
static uint8_t IMUCalibData[] = {2, 0, 0, 0, -13, -1, 69, 0, 3, 1, 156, 1, 0, 0, 0 ,0 ,0, 0, 232, 3, 23, 2};

// Register definitions
#define ACC_NM_SET     0x16
#define ACC_CONFIG     0x08
#define GYR_CONFIG     0x0A
#define GYR_CONFIG1    0x0B
#define MAG_CONFIG     0x09
#define CALIB_STAT     0x35  // Returns 0xFF if fully IMUcalibrated
#define INT_STA        0x37 //In normal mode this should return all 0's (no interrupts)
#define UNIT_SEL       0x3B
#define PWR_MODE       0x3E
#define SYS_TRIGGER    0x3F //Setting this register to 0x01 starts a self test of the IMU
//400 ms after the test starts, the below register will either remain 0(success) or return 3(failure)
#define SYS_ERROR      0x3A 
#define ST_RESULT      0x36 //This register will show a 0 for whichever device failed the self test (pg. 50 of datasheet)
#define SYS_STATUS     0x39
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
#define ACC_OFFSET_X_LSB 0x55
#define ACC_RADIUS_LSB 0x67
#define ACC_RADIUS_MSB 0x68
#define MAG_RADIUS_LSB 0x69 
#define MAG_RADIUS_MSB 0x6A

//These defines are for calibrating the IMU without having to move it
#define RADIUS_LSB_CALIB 0xF4
#define RADIUS_MSB_CALIB 0x01
#define NUM_REGISTERS    22

/*
The IMU works by way of communication through I2C. Registers are read/written to configure the IMU. 
The IMU requries some operation mode and a calibration for it to function.
This initialization loads the IMU with hard coded calibration data so calibration is only needed once.
*/

// Send and read data through I2C with delay
// Use for repeated communication with the IMU
// data is a pointer to a byte array, size is length of message, err is a HAL_StatusTypeDef variable

#define SEND(data, size, err) \
    do { \
        osDelay(50); \
        err = HAL_I2C_Master_Transmit(&hi2c1, ADDR, (data), (size), HAL_MAX_DELAY); \
        osDelay(50); \
    } while (0)

#define READ(addr, data, size, err) \
    do { \
        osDelay(50); \
        err = HAL_I2C_Mem_Read(&hi2c1, ADDR, (addr), I2C_MEMADD_SIZE_8BIT, (data), (size), HAL_MAX_DELAY); \
        osDelay(50); \
    } while (0)

// Just used to wait for the IMU to power on.
static void IMU_WaitForPower() {
    uint8_t config[2];
    HAL_StatusTypeDef error __attribute__ ((unused));
    // Wait for the chip to power on
    // The CHIP_ID register (0x00) is set to a fixed value of 0xA0
    config[0] = 0;
    while(config[0] != 0xA0) {
        READ(CHIP_ID, config, 1, error);
    }
    
    printf("Power Received\n\r");
    
}

/** 
 * @brief Initialize IMU to collect data
 * @param None
 * @return HAL_StatusTypeDef - OK, ERROR, BUSY, or TIMEOUT
 */
HAL_StatusTypeDef IMU_Init(){

    HAL_StatusTypeDef error = HAL_OK;
    // Unfortunately, there is a lot of black magic in this initialization.
    // There's a log of writing default values to registers that should
    // already contain those values, but there appears to be something about
    // this that legitimately makes a difference.
    //
    // My recommendation, don't cut out anything here without extensive
    // testing.

    osDelay(2000); //One reason to get a new IMU
    uint8_t config[2];

    //EDIT: Either this reset or the one below is unnecesary. 
    // Pull the reset pin low, then high to reset the IMU
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);
    for(volatile int i = 0; i < 10; i++);   // Give it a moment of time
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);

    IMU_WaitForPower();

    //EDIT: I dont believe this is necessary, since the IMU has a power on reset feature which starts the IMU in config mode
    // Verify our current mode. 
    READ(OPR_MODE, config, 1, error);
    if((config[0] & 0xF) != 0) {
        // Need to enter configuration mode first
        config[0] = OPR_MODE;
        config[1] = 0;
        SEND(config, 2, error);
        osDelay(50); // Wait for 50 ms. This is well over the amount of time required for this, but whatever.
    }
    
    printf("ERROR: %d\n\r", error);
    

    // Reset the chip. We kind of did this above, but we're doing it again to be sure
    config[0] = SYS_TRIGGER;
    config[1] = 0x20;
    SEND(config, 2, error);

    
    printf("ERROR: %d\n\r", error);
    

    // Wait for the reset to complete
    osDelay(1000);

    // EDIT: Already occurs after power on. 
    // The register map in this chip is split into multiple pages. Select page 0.
    config[0] = PAGE_ID;
    config[1] = 0;
    SEND(config, 2, error);

    printf("ERROR: %d\n\r", error);

    // Select the "normal" power mode. 
    // After reset, the IMU should already be in this normal mode
    config[0] = PWR_MODE;
    config[1] = 0;
    SEND(config, 2, error);

    printf("ERROR: %d\n\r", error);

    // Set the clock source. Run using internal 32kHz clock source
    config[0] = SYS_TRIGGER;
    config[1] = 0x00;
    SEND(config, 2, error);

    printf("ERROR: %d\n\r", error);
    
    //Read in m/s^2, Celcius, and degrees. pg 69 of ref manual for more options
    config[0] = UNIT_SEL;
    config[1] = 0;
    SEND(config, 2, error); 

    printf("ERROR: %d\n\r", error);

    // hard code calibration values
    error |= IMU_Calibrate();

    printf("ERROR: %d\n\r", error);
    printf("IMU Calibrated\n\r");

    // Now configure for our operation mode
    // IMPORTANT: this needs to be the last configuration register written
    config[0] = OPR_MODE; //register address
    config[1] = 0xC; // The configuration mode (NDOF)   // TODO: think about lower-power modes
    // TODO: experiencing difficulties with the IT version, but these might need to be blocking anyways, or at least need a check
    SEND(config, 2, error); //Turn on Accelerometer, Gyroscope, and Magnetometer with FMC (quicker calibration)
    
    printf("ERROR: %d\n\r", error);

    osDelay(20); // Wait for 20ms for the operating mode to change. This is well over the amount of time required, but whatever.

    // TODO: move this into a function that checks if we've uncalibrated. 
    const uint8_t calib_required = 
            ((CALIB_GYRO ? 3 : 0) << 4) | 
            ((CALIB_ACCEL ? 3 : 0) << 2) | 
            (CALIB_MAGNET ? 3 : 0);
    uint8_t calib_reg;
    
    do{ //3C is gyro, accel ;;FF is sys, gyr, accel, magnet ;;x30 is just gyro
        READ(CALIB_STAT, &calib_reg, 1, error);
        printf("%.2X\n\r", calib_reg);
    } while(calib_reg != calib_required);
        
    return error;
}

/**
 * @brief Update struct with new information
 * @param *Data : struct used to collect IMU Data
 * @return HAL_StatusTypeDef - OK, ERROR, BUSY, or TIMEOUT
 */
HAL_StatusTypeDef IMU_GetMeasurements(IMUData_t *Data){
    // Read 18 contiguous data registers within the IMU starting at 0x08 and stores data within struct fields
    return HAL_I2C_Mem_Read(&hi2c1, ADDR, ACC_DATA_X_LSB, I2C_MEMADD_SIZE_8BIT, (uint8_t*)Data, 18, HAL_MAX_DELAY);
}

/**
 * @brief Update struct with IMU Calibration register data. Used to hardcode calibration
 * @note The IMU must be in config mode (REG[0x3D]= 0) to read calibration registers
 * @param *Data : struct used to collect IMU calibration data
 * @return HAL_StatusTypeDef - OK, ERROR, BUSY, or TIMEOUT
 */
HAL_StatusTypeDef IMU_GetCalibData(){

    HAL_StatusTypeDef error = HAL_OK;
    // This function has to execute while the IMU is in configuration mode. Otherwise the registers cannot be read
    // TODO: check if we need to be in config mode to read these registers
    
    uint8_t config[4];
    config[0] = OPR_MODE; //register address
    config[1] = 0; // set to some configuration mode
    SEND(config, 2, error); // set IMU to configuration mode to extract calibration data
    
   //Read 22 contiguous bytes of calibration registers in the IMU. Load into calibration data struct fields
    READ(ACC_OFFSET_X_LSB, IMUCalibData, sizeof(IMUCalibData), error);
    return error;
}

/**
 * @brief Use pre-found data to upload calibration profile to IMU
 * @note updating the last bytes of the Z offsets will set the respective peripheral bits in the calibration status register (0x35)
 * @note The IMU must be in config mode (REG[0x3D]= 0) to read calibration registers
 * @param None
 * @return HAL_StatusTypeDef - OK, ERROR, BUSY, or TIMEOUT
 */
HAL_StatusTypeDef IMU_Calibrate(){

    HAL_StatusTypeDef error = HAL_OK;
    // This function must be executed while the IMU is in configuration mode. Otherwise registers are not writable
    uint8_t config[2];
    config[0] = OPR_MODE; //register address
    config[1] = 0; // set to some configuration mode
    SEND(config, 2, error); // set IMU to configuration mode to extract calibration data
    
    for (uint8_t reg=0; reg < NUM_REGISTERS; reg++) {
        config[0] = ACC_OFFSET_X_LSB + reg;
        config[1]= IMUCalibData[reg];
        SEND(config, 2, error);
    }

    // TODO: think about switching to NDOF here
    return error;
}

/** 
 * @brief This function aims to provide an all in one debug function. Call to help pinpoint an issue with the IMU
 * @param None
 * @return HAL_StatusTypeDef - OK, ERROR, BUSY, or TIMEOUT
 */
HAL_StatusTypeDef IMU_Debug(void){

    HAL_StatusTypeDef error = HAL_OK;
    uint8_t debug[2];
    // Call a Build in Self test. Results stored in ST_RESULT.
    error |= HAL_I2C_Mem_Read(&hi2c1, ADDR, SYS_TRIGGER, I2C_MEMADD_SIZE_8BIT, debug, 1, HAL_MAX_DELAY); 
    // Friendly write into SYS_Trigger
    debug[1] = debug[0] | 0x01;
    debug[0] = SYS_TRIGGER;

    SEND(debug, 2, error);
    if(error){
        printf("ERROR \n\r");
    }
    // Read results
    error |= HAL_I2C_Mem_Read(&hi2c1, ADDR, ST_RESULT, I2C_MEMADD_SIZE_8BIT, debug, 1, HAL_MAX_DELAY);

    // Interpret results on page 67 of reference manual
    printf("ST_RESULT: %X\n", debug[0]);

    // Check SYS_STATUS register. This will output various amounts of information. interpretation found on page 68 of ref manual
    
    error |= HAL_I2C_Mem_Read(&hi2c1, ADDR, SYS_STATUS, I2C_MEMADD_SIZE_8BIT, debug, 1, HAL_MAX_DELAY);

    // If SYS_STATUS returned a 1, check the error message in SYS_ERR register. interpet on pg 69 of ref manual
    printf("SYS_STATUS: %X\n", debug[0]);
    if (debug[0] == 1){
        error |= HAL_I2C_Mem_Read(&hi2c1, ADDR, SYS_ERROR, I2C_MEMADD_SIZE_8BIT, debug, 1, HAL_MAX_DELAY);
        printf("SYS_ERROR: %X\n", debug[0]);
    }

    // Switch to page 1 and read all 3 config registers. pg 77 for config register info. pg 26 for default values
    debug[0] = PAGE_ID;
    debug[1] = 1;
    SEND(debug, 2, error);

    error |= HAL_I2C_Mem_Read(&hi2c1, ADDR, ACC_CONFIG, I2C_MEMADD_SIZE_8BIT, debug, 1, HAL_MAX_DELAY);
    printf("ACC Config: %X\n", debug[0]);

    error |= HAL_I2C_Mem_Read(&hi2c1, ADDR, MAG_CONFIG, I2C_MEMADD_SIZE_8BIT, debug, 1, HAL_MAX_DELAY);
    printf("MAG config: %X\n", debug[0]);

    error |= HAL_I2C_Mem_Read(&hi2c1, ADDR, GYR_CONFIG, I2C_MEMADD_SIZE_8BIT, debug, 1, HAL_MAX_DELAY);
    printf("GYRO Config: %X\n", debug[0]);

    error |= HAL_I2C_Mem_Read(&hi2c1, ADDR, GYR_CONFIG1, I2C_MEMADD_SIZE_8BIT, debug, 1, HAL_MAX_DELAY);
    printf("GYRO power mode: %X\n", debug[0]);

    // TODO: think of better debugging tools
    return error;
}
