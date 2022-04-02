********
Drivers
********

Controller Area Network :term:`(CAN) <CAN Bus>`
===============================================

Purpose

Usage

Additional Considerations

Ethernet
========

Purpose

Usage

Additional Considerations

Inertial Measurement Unit (IMU)
===============================

Purpose
The IMU drivers use a accelerometer, gyroscope, and magnetometer to collect data of the cars movement. 
This data can be used to calculate measurements such as acceleration and yaw when the car is turning.

Usage
`IMU_Init(IMUData_t* Data)` should be called followed by `IMU_Calibrate(IMUCalibData_t *Data)`. This puts in data that was collected by 
calibrating the IMU by moving the module(on the board). The peripheral is initialized to collect data using all three measurment 
units. Later on, we can decide whether we actually need to collect data using everything (we can disable some to save power, such as 
the magnetometer). The struct passed in will collect all the data. Then the `IMU_UpdateMeasurements()` function is called to update all 
the data collected. Both functions return whether or not the I2C transmit and receive were successful or not.

Additional Considerations
The units can be initialized in many different ways. The way it is initialized now just collects the data. They can also be initialized 
in a way where calculations are done after the data is collected. If it turns out we need to use that method of initialization instead, 
the header file will have to be changed to include the register addresses of where that data is located and the struct will have to be 
changed as well. `IMU_Calibrate()` uses calibration data that was collected in a clean environment (i.e not in a car with power signals
and shaky movements). If the data is bad, one other implementation is to put the module in the car, call `IMU_GetCalibData()` and hope
it calibrates through the random movements of the car. Then that data can be stored (in SD card) for future use.

Global Positioning System (GPS)
===============================

Purpose

Usage

Additional Considerations

SD Card
=======

Purpose

Usage

Additional Considerations

Real Time Clock (RTC)
=====================

Purpose

Usage

Additional Considerations
