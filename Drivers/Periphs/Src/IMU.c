#include "IMU.h"
#include <stdbool.h>
#include <stdio.h>
#include "cmsis_os.h"

HAL_StatusTypeDef error;
IMUCalibData_t Calib_data;


#define DEV_ADDR 0x28
#define ADDR (DEV_ADDR << 1)

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






/*The IMU works by way of communication through I2C. Registers are read/written to configure the IMU. The IMU requries some operation mode and a calibration for it to function.
This initialization loads the IMU with hard coded calibration data so calibration is only needed once*/

// Send data through I2C
#define SEND(data, size) \
    osDelay(50); \
    HAL_I2C_Master_Transmit(&hi2c1, ADDR, (data), (size), HAL_MAX_DELAY); \
    osDelay(50)

// Just used to wait for the IMU to power on.
static void IMU_WaitForPower() {
    uint8_t config[2];

    // Wait for the chip to power on
    // The CHIP_ID register (0x00) is set to a fixed value of 0xA0
    config[0] = 0;
    while(config[0] != 0xA0) {
        HAL_I2C_Mem_Read(&hi2c1, ADDR, CHIP_ID, I2C_MEMADD_SIZE_8BIT, config, 1, HAL_MAX_DELAY);
        osDelay(50); //Allow time until next read should occur
    }
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

    //EDIT: Either this reset or the one below is unnecesary. 
    // Pull the reset pin low, then high to reset the IMU
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);
    for(volatile int i = 0; i < 10; i++);   // Give it a moment of time, WAS i<10
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);

    IMU_WaitForPower();

    //EDIT: I dont believe this is necessary, since the IMU has a power on reset feature which starts the IMU in config mode
    // Verify our current mode. 
    HAL_I2C_Mem_Read(&hi2c1, ADDR, OPR_MODE, I2C_MEMADD_SIZE_8BIT, config, 1, HAL_MAX_DELAY);
    if((config[0] & 0xF) != 0) {
        // Need to enter configuration mode first
        config[0] = OPR_MODE;
        config[1] = 0;
        SEND(config, 2);
        // Wait for 50 ms. This is well over the amount of time required for this, but whatever.
        osDelay(50);  // TODO: replace with a freertos delay if we plan on multithreading by this point, WAS 50
    }

    // Reset the chip. We kind of did this above, but we're doing it again to be sure
    config[0] = SYS_TRIGGER;
    config[1] = 0x20;
    error |=SEND(config, 2);
    if(error)
    {
        printf("ERROR2 \n\r");
    }
    


    // Wait for the reset to complete
    osDelay(1000);    // TODO: replace with a freertos delay if we plan on multithreading by this point, WAS 1000

    // EDIT: Already occurs after power on. 
    // The register map in this chip is split into multiple pages. Select page 0.
    config[0] = PAGE_ID;
    config[1] = 0;
    error |= SEND(config, 2);
    if(error)
    {
        printf("ERROR1 \n\r");
    }

    // Select the "normal" power mode. 
    // After reset, the IMU should already be in this normal mode
    config[0] = PWR_MODE;
    config[1] = 0;
    error |=SEND(config, 2);

    // Set the clock source. Run using internal 32kHz clock source
    config[0] = SYS_TRIGGER;
    config[1] = 0x00;
    error |=SEND(config, 2);
    
    //Read in m/s^2, Celcius, and degrees. pg 69 of ref manual for more options
    config[0] = UNIT_SEL;
    config[1] = 0;
    error |= SEND(config, 2); 

    // hard code calibration values
    error |= Calibrate(&Calib_data);

    // Now configure for our operation mode
    // IMPORTANT: this needs to be the last configuration register written
    config[0] = OPR_MODE; //register address
    config[1] = 0xC; // The configuration mode (NDOF)   // TODO: think about lower-power modes
    // TODO: experiencing difficulties with the IT version, but these might need to be blocking anyways, or at least need a check
    error |= SEND(config, 2); //Turn on Accelerometer, Gyroscope, and Magnetometer with FMC (quicker calibration)
    
    // Wait for 20ms for the operating mode to change. This is well over the amount of time required, but whatever.
    osDelay(20); //WAS 20

    // TODO: move this into a function that checks if we've uncalibrated. 
    uint8_t calib_reg = 0x00; 
    while((calib_reg >> 6) != 3) //3C is gyro, accel ;;FF is sys, gyr, accel, magnet ;;x30 is just gyro
    {
        HAL_I2C_Mem_Read(&hi2c1, ADDR, CALIB_STAT, I2C_MEMADD_SIZE_8BIT, &calib_reg, 1, HAL_MAX_DELAY);
        printf("Calibrating...[%.4X]\n\r", calib_reg);
    }
    
    
    printf("Done Calibrating!\n\r");
        
    return SUCCESS;
}



ErrorStatus IMU_GetMeasurements(IMUData_t *Data){
    // Read 18 contiguous data registers within the IMU starting at 0x08 and stores data within struct fields
    error = HAL_I2C_Mem_Read(&hi2c1, ADDR, ACC_DATA_X_LSB, I2C_MEMADD_SIZE_8BIT, (uint8_t*)Data, 18, HAL_MAX_DELAY);
    if (error != HAL_OK){return ERROR;}
    
    return SUCCESS;
}


ErrorStatus IMU_GetCalibData(IMUCalibData_t *Data){
    // This function has to execute while the IMU is in configuration mode. Otherwise the registers cannot be read
    // TODO: check if we need to be in config mode to read these registers
    
    uint8_t config[4];
    config[0] = OPR_MODE; //register address
    config[1] = 0; // set to some configuration mode
    error |= SEND(config, 2); // set IMU to configuration mode to extract calibration data
    
   //Read 22 contiguous bytes of calibration registers in the IMU. Load into calibration data struct fields
    error = HAL_I2C_Mem_Read(&hi2c1, ADDR, ACC_OFFSET_X_LSB, I2C_MEMADD_SIZE_8BIT, (uint8_t*)Data, 22, HAL_MAX_DELAY);
    if (error != HAL_OK){return ERROR;}
    
    return SUCCESS;
}

ErrorStatus Calibrate(IMUCalibData_t *Data)
{
    // This function must be executed while the IMU is in configuration mode. Otherwise registers are not writable
    uint8_t config[4];
    config[0] = OPR_MODE; //register address
    config[1] = 0; // set to some configuration mode
    error |= SEND(config, 2); // set IMU to configuration mode to extract calibration data
    
    int16_t index =0x55;
    for (int32_t reg=0; reg < 22; reg +=1) {
        config[0] = index;
        config[1] = 0x01;

        error |=SEND(config, 2);

        index +=1;

    }

    index =0x67;
    for (int32_t rad=0; rad < 2; rad ++) {
        config[0] = index;
        config[1] = 0xF4;


        error |=SEND(config, 2);
        index +=1;

        config[0] = index;
        config[1] = 0x01;
        error |=SEND(config, 2);

        index +=1;

        

    }

    if(error)
    {
        printf("ERROR3 \n\r");
    }

    // TODO: think about switching to NDOF here
    
    return SUCCESS;
}

/* This function aims to provide an all in one debug function. Call to help pinpoint an issue with the IMU*/
ErrorStatus DebugIMU(void)
{
    uint8_t debug[2];
    // Call a Build in Self test. Results stored in ST_RESULT.
    HAL_I2C_Mem_Read(&hi2c1, ADDR, SYS_TRIGGER, I2C_MEMADD_SIZE_8BIT, debug, 1, HAL_MAX_DELAY); 
    // Friendly write into SYS_Trigger
    debug[1] = debug[0] | 0x01;
    debug[0] = SYS_TRIGGER;

    error |=SEND(debug, 2);
    if(error)
    {
        printf("ERROR2 \n\r");
    }
    // Read results
    HAL_I2C_Mem_Read(&hi2c1, ADDR, ST_RESULT, I2C_MEMADD_SIZE_8BIT, debug, 1, HAL_MAX_DELAY);

    // Interpret results on page 67 of reference manual
    printf("ST_RESULT: %X\n", debug[0]);

    // Check SYS_STATUS register. This will output various amounts of information. interpretation found on page 68 of ref manual
    
    HAL_I2C_Mem_Read(&hi2c1, ADDR, SYS_STATUS, I2C_MEMADD_SIZE_8BIT, debug, 1, HAL_MAX_DELAY);

    // If SYS_STATUS returned a 1, check the error message in SYS_ERR register. interpet on pg 69 of ref manual
    printf("SYS_STATUS: %X\n", debug);
    if (debug == 1){
        HAL_I2C_Mem_Read(&hi2c1, ADDR, SYS_ERROR, I2C_MEMADD_SIZE_8BIT, debug, 1, HAL_MAX_DELAY);
        printf("SYS_ERROR: %X\n", debug[0]);
    }

    // Switch to page 1 and read all 3 config registers. pg 77 for config register info. pg 26 for default values
    debug[0] = PAGE_ID;
    debug[1] = 1;
    error |= SEND(debug, 2);

    HAL_I2C_Mem_Read(&hi2c1, ADDR, ACC_CONFIG, I2C_MEMADD_SIZE_8BIT, debug, 1, HAL_MAX_DELAY);
    printf("ACC Config: %X\n", debug[0]);

    HAL_I2C_Mem_Read(&hi2c1, ADDR, MAG_CONFIG, I2C_MEMADD_SIZE_8BIT, debug, 1, HAL_MAX_DELAY);
    printf("MAG config: %X\n", debug[0]);

    HAL_I2C_Mem_Read(&hi2c1, ADDR, GYR_CONFIG, I2C_MEMADD_SIZE_8BIT, debug, 1, HAL_MAX_DELAY);
    printf("GYRO Config: %X\n", debug[0]);

    HAL_I2C_Mem_Read(&hi2c1, ADDR, GYR_CONFIG1, I2C_MEMADD_SIZE_8BIT, debug, 1, HAL_MAX_DELAY);
    printf("GYRO power mode: %X\n", debug[0]);

    // TODO: think of better debugging tools

}




