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
The SD Card is used to collect data in the event that it could not be sent to the data acquisition system over ethernet. This means that
data is logged periodically and can be parsed later.

Usage
``SDCard_Init()`` mounts the SD card and should be called before any other function. ``SDCard_GetStatistics()`` should be used when debugging
and not in the main loop (unless to check if there is not enough data left). ``SDCard_PutInQueue()`` should be used by the thread placing
data in the queue while ``SDCard_Sort_Write_Data()`` will take the data from the queue or return an error if there is no data. The error
does not pertain to an empty queue. ``SDCard_t`` contains the data that will be written to the card. The ``id`` detects which file will
be written to. The timestamp should be sampled from the GPS drivers and should be accurate to around a second. In order to prevent data
loss in the event of a shutdown sequence it might be necessary to mount and unmount the SD Card periodically. This can be done with the 
functions ``SDCard_OpenFileSystem()`` & ``SDCard_CloseFileSystem()``.

Additional Considerations
It would be interesting to consider sending Data Acquisition a message if the SD Card gets close to becoming full. The helper functions in
this driver are dependent on the length of the time string being exactly 9 characters. We should also try calculating how fast the SD
card fills up so we can find an appropriately sized one.

Real Time Clock (RTC)
=====================

Purpose

Usage

Additional Considerations
