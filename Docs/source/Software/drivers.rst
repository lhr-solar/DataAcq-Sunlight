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

``Ethernet_Init()`` is first called as a way to create the Queue and create a socket when a connection has been established.
``Ethernet_ConnectToServer()`` is a static blocking function called inside ``Ethernet_Init()`` that is either called when connection
first needs to be established or when connection is lost with data acquisition.  ``Ethernet_SendMessage()`` then collects data from the 
Ethernet Queue and sends through Ethernet - if there is no connection, it calls ``Ethernet_ConnectToServer()``. ``Ethernet_SendMessage()`` returns pdFalse
if the Queue is empty and pdTrue if the Queue is not empty. The struct EthernetMSG_t will hold the data packets of either information 
regarding IMU, GPS, or CAN. The struct consists of an id enum, length, and a union of data. Since the data is in a union, there will be zero padding
at the MSB.

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
