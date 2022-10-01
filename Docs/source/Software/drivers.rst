********
Drivers
********

.. _can-ref:

Controller Area Network :term:`(CAN) <CAN Bus>`
===============================================

Purpose
-------

CAN drivers are used to record the state of the car through messages sent over CAN Bus.
The controls, array, and battery protection systems all send status and configuration 
messages using CAN, which are then recorded by Sunlight and logged/broadcast.

Usage
-----

``CAN_init()`` should be called before any CANBus usage. ``CAN_init()`` has a parameter, 
``mode``, which can be used to specify loopback vs normal operation. With the current 
software organization, this can be configured in ``config.h`` (``CAN_LOOPBACK``).
CAN messages are received with interrupts/callbacks, and placed into a FreeRTOS Queue. 
Message byte lengths and formatting is specified by a lookup table at the bottom of ``CANBus.c`` 
(``CanMetadataLUT``), with all CAN IDs listed in an enum ``CANId_t``. This enum and the 
lookup table must be updated with any CAN ID changes. CAN data is stored in a ``CANMSG_t`` 
struct containing the CAN ID and the CAN message payload, and can be retrieved from the 
queue by calling ``CAN_FetchMessage()``.

A very basic transmit function ``CAN_TransmitMessage`` exists currently for testing purposes 
only.

Additional Considerations
-------------------------

CAN is currently configured to use ``RX_FIFO0`` and the CAN1 interface. If another CAN 
HW interface is added, use CAN3 (not CAN2).

.. _eth-ref:

Ethernet
========

Purpose
-------

The Ethernet drivers send GPS, CAN, and IMU data to the data acquisition team.
The Sunlight operates as a Telnet client and data acquisition operates as a Telnet server.

Usage
-----

``Ethernet_Init()`` must be called first before using any Ethernet sending function. This init 
function is blocking and waits until a valid connection with a server is established. A separate 
``Ethernet_QueueInit()`` initializes the Ethernet send queue. Both initialization functions must 
be called, but the separate queue init function allows for messages to be pushed to the send queue 
before a valid client-server connection can be established.

``Ethernet_PutInQueue()`` adds messages to the send queue, and ``Ethernet_SendMessage()`` attempts 
to send one message from the queue over Ethernet. If a send fails, Ethernet will enter a blocking 
'wait for server connection' state until a connection is re-established.

Messages in the send queue are structured in a ``EthernetMSG_t`` struct containing an ``EthernetData_t`` 
union, which contains the raw data for each of CAN, IMU, and GPS. Each Ethernet message struct contains 
an ID specifying the message type (CAN/IMU/GPS) and a length field, which is the length in bytes of the 
whole message.

The IP used by Sunlight and the server IP are configured in ``config.h``.

Additional Considerations
-------------------------

The ID and length fields of the ``EthernetMSG_t`` struct are required since the receiving end must be 
notified of the length and type of an incoming message to parse. The ``EthernetMSG_t`` struct is very 
large since the data union must fit the large GPS data string. Thus, the Ethernet send queue size must 
not be very large.

Ethernet contains code specifically for robustness if the connection is lost.

.. _imu-ref:

Inertial Measurement Unit (IMU)
===============================

Purpose
-------

The IMU drivers collect data from the IMU on Sunlight, which contains accelerometer, gyroscope, and magnetometer data.

Usage
-----

``IMU_Init()`` will initialize the IMU driver. The initialization sequence includes a calibration sequence 
which calibrates the accelerometer and gyroscope only. The current board design makes the magnetometer 
extremely unreliable to calibrate. ``IMU_GetMeasurements()`` fetches measurements from the IMU. ``IMU_Calibrate()`` 
sends hard coded calibration values to avoid the delay of calibration, but this may not be necessary as 
magnetometer calibration is ignored.

IMU communicates over I2C.

Additional Considerations
-------------------------

``IMU_Init()`` is very finely tuned with timings and commands. Do not modify anything unless you know what you're doing.

.. _gps-ref:

Global Positioning System (GPS)
===============================

Purpose
-------

The GPS drivers interface with the GPS chip on Sunlight, which contains location and heading information about the car.

Usage
-----

First ``GPS_Init()`` must be called. It will initialize the peripheral using the commands described in the 
``char *init_commands[]`` variable. Descriptions of these commands are available in the 
`datasheet <https://www.digikey.com/htmldatasheets/production/1641571/0/0/1/pa6h-gps-module-command-set.html>`__. 
These commands can change the speed of communication, frequency of communication, type of data to send, and startup sequence to run.
Data is recieved by the interrupt-based ``GPS_Receive()``, which recieves full 'sentences' (see datasheet), parses for valid information, 
and pushes to a queue. ``GPS_ReadData()`` fetches GPS data from the queue, formatted in a ``GPSData_t`` struct. Each GPS data field is 
a ``char`` array (string).

GPS data contains a 'time' field which can is currently used to add timestamps to all data on the system.

The GPS communicates over UART at a configurable baud rate.

Additional Considerations
-------------------------

Be careful when changing commands in the command list. A baud rate change for the GPS means that the UART driver must also be changed. 
The antenna must be plugged in to recieve an accurate location.

.. _sdc-ref:

SD Card
=======

Purpose
-------

The SD Card is used to log data as a backup for broadcasting over Ethernet.

Usage
-----

The SD card driver implements a buffered logging system on top of FatFS. Init/Queue functionality and union-based SD card data 
is similar to Ethernet. 

Call ``SDCard_PutInQueue`` to queue up messages to be written. ``SDCard_Sort_Write_Data()`` will actually perform writes, but 
data is buffered first in an array and written in chunks when the buffer is filled. ``SDCard_SyncLogFiles()`` must be called 
to actually save files to the SD card.

Log files are split between IMU/GPS/CAN in CSV files ("IMU.csv", "GPS.csv", "CAN.csv").

Additional Considerations
-------------------------

An LED (``SDC_SYNC``) will blink every time an actual SD write is being performed. Removing the SD card when the LED is blinking 
may cause data corruption.

Currently there is no good way to ensure that the SD card is properly synced upon system shutdown.

The SD card + FATFS has an internal log buffer (``SDCard.c``) and an internal filesystem buffer (in FATFS). ``SDCard_Write()`` 
can be used to flush the internal log buffer by writing actual data to the SD card, but the filesystem buffer must be separately 
written using ``SDCard_SyncLogFiles()``. Data loss will occur if either buffer is not flushed before shutdown.

Real Time Clock (RTC)
=====================

RTC is not currently used (and there are no drivers).

LED
===

Purpose
-------

blinky lights

Usage
-----

``On`` -> On

``Off`` -> Off

``Toggle`` -> Toggle

Additional Considerations
-------------------------

A heartbeat task in ``main.c`` uses the LED drivers to blink the ``HEARTBEAT`` 
LED at a rate specified in ``config.h``.


.. Driver Doc Template
.. ===================

.. Purpose
.. -------

.. Usage
.. -----

.. Additional Considerations
.. -------------------------

