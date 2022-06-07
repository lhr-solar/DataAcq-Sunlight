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

The Ethernet drivers are sending GPS, CAN, and IMU data to the data acquisition team.
The sunlight is the client while data acquisition is the server

Usage

``Ethernet_Init()`` must be called first as a way to create the Ethernet Queue and create a socket when a connection has been established. 
Within the function, ``Ethernet_Init()`` calls the blocking function ``Ethernet_ConnectToServer()`` which is responsible for setting the ``servsocket`` variable to the corresponding socket value. 
After ``Ethernet_Init()`` has finished completing, ``Ethernet_SendMessage()`` is called repeatedly. ``Ethernet_SendMessage()`` is a blocking function which waits until there is data in the queue to send that data across ethernet.
The data that is sent across Ethernet is in a struct called EthernetMSG_t which contains specific information of either IMU, GPS, or CAN type. 
The struct consists of an id enum, length, and a union of data regardless of the type of data. 
Since the data is in a union, there will be zero padding at the MSB.

Additional Considerations

Sunlight could have been implemented as a server instead of a client. This implementation would make more sense if Ethernet had the option of sending
messages to multiple clients. The way that it is implemented, Sunlight is trying to connect to Data acquisition so that it can send information. 
As sunlight as the server, data acquisition would try and connect to sunlight in order to send information. In order to implement these changes, both 
``Ethernet_ConnectToServer()`` and ``Ethernet_Init()`` would need to be changed so that it waits for a client to connect and send messages to that specific client. 



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
