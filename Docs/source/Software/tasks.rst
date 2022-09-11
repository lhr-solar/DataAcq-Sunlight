******
Tasks
******

.. _init-task-ref:

Initialization Task
===================

Purpose
-------

The initialization task runs driver init functions and spawns the `Data Logging Task`, 
`Data Reading Task`, and `Broadcasting Task`. 

Functionality
-------------

Peripherals are initialized in this order (see :doc:`Drivers <drivers>` for more info):

1. :ref:`Ethernet Queue <can-ref>`
2. :ref:`SD Card <sdc-ref>` *initializing SD card only occurs if an SD card is inserted*
3. :ref:`CAN <can-ref>`
4. :ref:`GPS <gps-ref>`
5. :ref:`IMU <imu-ref>` 

followed by the `Data Logging Task`, `Data Reading Task`, and `Broadcasting Task`. 

The initialization task then kills itself.

Priority
--------

``osPriorityHigh`` - 
The initialization task must finish executing before any other task starts to run.

Timing Requirements
-------------------

Must finish executing before tasks are allowed to use peripherals.

Some drivers require a delay before they can be initialized. The ``osDelay(1000)`` 
at the start of the task helps make driver initialization more consistent.

Additional Considerations
-------------------------

The actual Ethernet driver is not initialized in this task, only the queue. This allows the 
Ethernet driver initialization to stall while waiting for a connetion without delaying the 
initialization of other peripherals.

The order that peripherals are initialized may determine whether initialization will consistently succeed.

.. _read-task-ref:

Data Reading Task
=================

Purpose
-------

The Data Reading Task collects data from sensors (IMU, GPS) and CAN and adds to the logging and broadcasting queues.

Functionality
-------------

The Data Reading Task runs through the following loop:

1. Fetch GPS data from queue (if queue is nonempty)

    1. Record timestamp (second-precision) and update logigng timestamp
    2. Add GPS data to logging and broadcasting queues

2. Poll IMU for data

    1. Add IMU data to logging and broadcasting queues

3. While the CAN queue is nonempty:

    * Fetch CAN message from CAN queue and add to logging and broadcasting queues

Priority
--------

``osPriorityNormal``

Shared Resources
----------------

| **SD Card Queue** - Shared with `Data Logging Task`
| **Ethernet Queue** - Shared with `Broadcasting Task`

Timing Requirements
-------------------

Must run with frequency greater than the average frequency of incoming CAN messages.

.. _log-task-ref:

Data Logging Task
=================

Purpose
-------

The Data Logging Task is in charge of logging data to the SD card as an offline backup 
for the data sent over Ethernet to Data Acquisition.

Functionality
-------------

The Data Logging Task runs through the following loop:

1. Pull one entry from the SD card queue and write to the SD card
2. If no entries exist in the queue:

    * Yield

3. If ``SDCARD_SYNC_PERIOD`` (``SDCard.h``) milliseconds has passed since the last sync:

    * Sync filesystem to SD card

Priority
--------

``osPriorityNormal``

Shared Resources
----------------

| **SD Card Queue** - Shared with `Data Reading Task`

Timing Requirements
-------------------

Must be able to execute with the same or higher frequency than data being collected by the `Data Reading Task`

Yields
------

Yields if SD card queue is empty.

.. _broadcast-task-ref:

Broadcasting Task
=================

Purpose
-------

The Broadcasting Task broadcasts data over Ethernet/radio to Data Acquisition.

Functionality
-------------

The Broadcasting Task runs through the following sequence:

1. Initialize the Ethernet driver and attempt to connect to the Data Acquisition system

Loop through the following:

1. Pull one entry from the Ethernet queue and send
2. If no entries exist in the queue:

    * Yield


Priority
--------

``osPriorityNormal``

Shared Resources
----------------

| **Ethernet Queue** - Shared with `Data Reading Task`

Timing Requirements
-------------------

Must be able to execute with the same or higher frequency than data being collected by the `Data Reading Task`

Yields
------

Yields if Ethernet queue is empty.

.. _heartbeat-task-ref:

Heartbeat Task
==============

Purpose
-------

The heartbeat task blinks an LED to show that Sunlight is running. The heartbeat period is configured in ``config.h``.

Priority
--------
``osPriorityNormal`` - 
The Heartbeat Task must be given the same priority as the most processor-intensive task to ensure it runs 
in the case that the other tasks saturate the processor time

`Idle Task <https://www.freertos.org/RTOS-idle-task.html>`_
===========================================================

The idle task is automatically created by the RTOS and runs when there are no other tasks able to run 
on the system. The idle task is able to execute a function (the Idle Task Hook). See the freeRTOS docs 
for more information.


.. Task Doc Template
.. =================

.. Purpose
.. -------

.. Functionality
.. -------------

.. Priority
.. --------

.. Shared Resources
.. ----------------

.. Timing Requirements
.. -------------------

.. Yields
.. ------

.. Additional Considerations
.. -------------------------

