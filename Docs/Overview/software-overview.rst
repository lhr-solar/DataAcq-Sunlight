*****************
Software Overview
*****************

The software is responsible for

- Making sure that the Driver and Pit Crew can communicate
- Collecting Data from peripherals and other systems
- Sending data to the Pit Crew to determine status of vehicle

Hardware Abstraction Layer (HAL)
================================

The `HAL <https://www.st.com/resource/en/user_manual/dm00105879-description-of-stm32f4-hal-and-ll-drivers-stmicroelectronics.pdf>`_ 
is provided by STM and allows for easy use of peripheral drivers within the STM.

Drivers
=======

The drivers interface with the external hardware and is above the HAL. The Sunlight Drivers interface with

- `IMU BNO055: <https://www.bosch-sensortec.com/media/boschsensortec/downloads/datasheets/bst-bno055-ds000.pdf>`_
- `RTC PCF8523T: <https://www.nxp.com/docs/en/data-sheet/PCF8523.pdf>`_
- `GPS MTK3339: <https://www.adafruit.com/product/790>`_
- Ethernet
- SD Card
- :term:`CAN <CAN Bus>`


Tasks
=====

The Sunlight tasks use the drivers to collect and process the data in a timely manner. The following tasks make up Sunlight

- :ref:`Data Logging <Data Logging Thread>`
- :ref:`Data Reading <Data Reading Thread>`
- :ref:`Broadcasting <Broadcasting Thread>`