*****************
Software Overview
*****************

The software is responsible for:

* Making sure that the Driver and Pit Crew can communicate
* Collecting Data from peripherals and other systems
* Sending data to the Pit Crew to determine status of vehicle

Hardware Abstraction Layer (HAL)
================================

The `HAL <https://www.st.com/resource/en/user_manual/dm00105879-description-of-stm32f4-hal-and-ll-drivers-stmicroelectronics.pdf>`_ 
is provided by STM and allows for easy use of peripheral drivers within the STM. The HAL layer was generated using
the `STMCube Code Generator <https://www.st.com/en/development-tools/stm32cubemx.html>`_.

Drivers
=======

The drivers interface with the external hardware and is above the HAL. The Sunlight Drivers interface with:

* `IMU BNO055 <https://www.bosch-sensortec.com/media/boschsensortec/downloads/datasheets/bst-bno055-ds000.pdf>`_
* `RTC PCF8523T <https://www.nxp.com/docs/en/data-sheet/PCF8523.pdf>`_
* `GPS MTK3339 <https://www.adafruit.com/product/790>`_
* `Ethernet <https://www.nongnu.org/lwip/2_1_x/index.html>`_
* `SD Card <http://elm-chan.org/fsw/ff/00index_e.html>`_
* :term:`CAN <CAN Bus>`

RTOS
====

The :term:`RTOS` uses a scheduler to run multiple threads. Running these threads creates “pseudo-concurrency”.
We use `freeRTOS <https://www.freertos.org/>`_

The RTOS uses a `priority based preemptive scheduler <https://en.wikipedia.org/wiki/Fixed-priority_pre-emptive_scheduling>`_. 

Info about RTOS concepts in can be found in the `freeRTOS documentation page <https://www.freertos.org/implementation/a00002.html>`_

Tasks
=====

The Sunlight tasks use the drivers to collect and process the data in a timely manner. The following tasks make up Sunlight:

* :ref:`Initialization <init-task-ref>`
* :ref:`Data Logging <log-task-ref>`
* :ref:`Data Reading <read-task-ref>`
* :ref:`Broadcasting <broadcast-task-ref>`
* :ref:`Heartbeat <heartbeat-task-ref>`