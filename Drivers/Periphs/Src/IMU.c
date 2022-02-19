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

#define CALIB_STAT 0x35 //This register returns 0xFF if fully 

//These defines are not in IMU.h because they are only necessary for debugging
#define CALIB_STAT 0x35 //This register returns 0xFF if fully calibrated
//If the below register returns a 1 anywhere, that means that it will only send values if the accelerometer
//reads a change in data
#define INT_STA 0x37 //We are not using low power mode, this should return all 0's (no interrupts)
#define SYS_TRIGGER 0x3F //Setting this register to 0x01 starts a self test of the IMU
//400 ms after the test starts, the below register will either remain 0(success) or return 3(failure)
#define SYS_ERROR 0x3A 
#define ST_RESULT 0x36 //This register will show a 0 for whichever device failed the test (pg. 50 of datasheet)

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
        printf("Waiting for IMU to initialize...\n\r");
        HAL_I2C_Mem_Read(&hi2c1, ADDR, CHIP_ID, I2C_MEMADD_SIZE_8BIT, config, 1, HAL_MAX_DELAY);
    }
    printf("IMU is now responding! Continuing...\n\r");
    printf("Checking Power on Self Test POST to see if sensors/ microcontr funct./ respond right. \n\r");

    ////////////////////////////////////////////////////////////////////////////
    uint8_t st_reg[1];
    while(st_reg[0]!=0x0F) //checks to see if after POST whether or not sensors and microcon funct/ respond right. 
    //If not, the issue starts earlier (here). It could still be an issue with transmitting data (i2c)
    {
        printf("Doing POST... \n\r");
        printf("%d\n\r", st_reg[0]);
        HAL_I2C_Mem_Read(&hi2c1, ADDR, ST_RESULT, I2C_MEMADD_SIZE_8BIT, st_reg, 1, HAL_MAX_DELAY);

        
    }
    printf("POST complete! All 4 things good! \n\r");
     //HAL_I2C_Master_Transmit(&hi2c1, ADDR, (data), (size), HAL_MAX_DELAY)
     //how to write directly to SYS_TRIGGER for self-test?
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
    printf("check 2 \n\r");

    // Verify our current mode
    HAL_I2C_Mem_Read(&hi2c1, ADDR, OPR_MODE, I2C_MEMADD_SIZE_8BIT, config, 1, HAL_MAX_DELAY);
    if((config[0] & 0xF) != 0) {
        // Need to enter configuration mode first
        config[0] = OPR_MODE;
        config[1] = 0;
        SEND(config, 2);
        // Wait for 50 ms. This is well over the amount of time required for this, but whatever.
        printf("check 3 \n\r");
        HAL_Delay(50);  // TODO: replace with a freertos delay if we plan on multithreading by this point, WAS 50
        printf("check 4 \n\r");
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
    printf("check 5 \n\r");
    //HAL_Delay(1000);    // TODO: replace with a freertos delay if we plan on multithreading by this point, WAS 1000

    for(volatile int x=0; x<10000000; x++);
    ////////////////////////////////////////////////////////////maybe move POST here
    uint8_t st_reg[2];
    st_reg[0]=SYS_TRIGGER;
    st_reg[1]=0x01; //trigger self test
    SEND(st_reg, 2);
    //////////////then read ST_Result

    printf("check 6 \n\r");

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
    //HAL_Delay(20); //WAS 20
    for(volatile int y=0; y<10000000; y++);

    uint8_t calib_reg[1];
    while(calib_reg[0]!=0x30) //3C is gyro, accel ;;FF is sys, gyr, accel, magnet ;;x30 is just gyro
    {
        printf("Calibrating..\n\r");
        printf("%d\n\r", calib_reg[0]);
        HAL_I2C_Mem_Read(&hi2c1, ADDR, CALIB_STAT, I2C_MEMADD_SIZE_8BIT, calib_reg, 1, HAL_MAX_DELAY);
        //CALIB_STAT before, now CHIP_ID
        
    }

    //HAL_I2C_Mem_Read(&hi2c1, ADDR, CALIB_STAT, I2C_MEMADD_SIZE_8BIT, calib_reg, 1, HAL_MAX_DELAY);
    if(calib_reg[0]==0x30)
    {
        printf("Done Calibrating!\n\r");
    }

    if(calib_reg[0]==0xA0)
    {
        printf("Hal Mem Read Works!\n\r");
    }
    

    return SUCCESS;
}

ErrorStatus IMU_GetMeasurements(IMUData_t *Data){
    //uint8_t reg_addr = ACC_DATA_X_LSB; 
    
    error = HAL_I2C_Mem_Read(&hi2c1, ADDR, ACC_DATA_X_LSB, I2C_MEMADD_SIZE_8BIT, (uint8_t*)Data, 18, HAL_MAX_DELAY);
    //
    //this function is not complete, we need to read from all 3: gyro, magnetic, velocity. check on logic analyzer 
    //to see if the corresponding addresses for these are sending data
    //also confirm if I2C initialized correctly 
    // 2/9
    if (error != HAL_OK){return ERROR;}
    
    return SUCCESS;
}


