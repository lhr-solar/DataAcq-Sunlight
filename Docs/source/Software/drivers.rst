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

The Ethernet drivers are sending GPS, CAN, and IMU data to the data acquisition team
The sunlight is the client while data acquisition is the server

Usage



Additional Considerations



Inertial Measurement Unit (IMU)
===============================

Purpose

The IMU drivers use a accelerometer, gyroscope, and magnetometer to collect data of the car's movement. 
This data can be used to calculate measurements such as acceleration and yaw when the car is turning.

Usage

``IMU_Init()`` should be called. Then ``IMU_Calibrate()`` is called within IMU_Init(). This puts data into the calibration registers that was 
collected by calibrating the IMU by moving the module(on the board). The peripheral is initialized to collect data using all three measurment 
units. Later on, we can decide whether we actually need to collect data using everything (we can disable some to save power, such as 
the magnetometer). The struct ``IMUData_t`` will hold all the data on the car's motion. Then the ``IMU_UpdateMeasurements(*Data IMUData_t *Data)`` 
function is called to update all the data collected. All three functions: ``IMU_Init()``, ``IMU_Calibrate()``, and 
``IMU_UpdateMeasurements(*Data IMUData_t *Data)`` return whether or not the I2C transmit and receive were successful or not.

Additional Considerations

The units can be initialized in many different ways. The way it is initialized now just collects the data. They can also be initialized 
in a way where calculations are done after the data is collected. If it turns out we need to use that method of initialization instead, 
the header file will have to be changed to include the register addresses of where that data is located and the struct will have to be 
changed as well. Refer to the data sheet for more info on different types of initializations. ``IMU_Calibrate()`` uses calibration data that 
was collected in a clean environment (i.e not in a car with power signals and shaky movements). This data is stored in ``IMUCalibData[]``. 
The function ``IMU_GetCalibData()`` is used when the car is not in motion to manually shake the board and record the calibration 
values to hardcode in ``IMUCalibData[]``. If the data is bad with this method, one other implementation is to put the module in the car, 
and hope it calibrates through the random movements of the car.

Global Positioning System GPS
=============================
Purpose
    The purpose of these drivers is to detect the location of the car so it can be used in race strategy to determine
    where it should go next.

Usage
    First the ``GPS_Init(GPSData_t *Data)`` must be called. It will initialize the peripheral using the commands described in the 
    ``char *init_commands[]`` variable. Descriptions of these commands are available in the 
    `datasheet <https://www.digikey.com/htmldatasheets/production/1641571/0/0/1/pa6h-gps-module-command-set.html>`__. 
    These commands can change the speed of communication, frequency of communication, type of data to send, and startup sequence to run.
    It sends data to us at fixed intervals so the function callback ``GPS_Receive()`` handles that. ``GPS_ReadData()`` collects the data 
    from the queue and returns an error code if it could fetch the message or not.

Additional Considerations
    This units initialization should be changed if it turns out we are not receiving correct data. Also, the parsing in the callback will
    have to change if we are initializing to send us different data.

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
