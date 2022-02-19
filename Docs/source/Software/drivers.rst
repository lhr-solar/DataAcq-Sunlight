********
Drivers
********

Controller Area Network :term:`(CAN) <CAN Bus>`
===============================================

Purpose
    This driver is meant to recieve :term:`CAN <CAN Bus>` messages. This allows us to collect data being sent from the rest of the car.

Usage
    The header file contains data types that are used to recieve CAN messages. Each ``CANMSG_t`` recieved will contain a ``CANID_t`` and ``CANPayload_t`` in a struct. ``CANPayload_t`` will contain the data (``CANData_t``) and id of the value if it is in an array. If it isn't in an array ``idx`` should be 0. ``CANData_t`` should only have the value of the data type being sent filled into the union. For example, if the data is a float, only fill in ``CANData.f`` and nothing else.

    Recieve functionality is supported through interrupts. Upon recieving a CAN message ``HAL_CAN_RxFifo0MsgPendingCallback()`` or ``HAL_CAN_RxFifo0MsgPendingCallback()`` will execute and add the new message to an Rx Queue.

    The following functions are used for accessing CAN functionality:

    ``CAN_Config()`` configures the CAN on HAL, including setting up the filter and initializing the CAN hardware. ``CAN_Config()`` also sets the Rx Queue, which needs to be initialized prior to calling ``CAN_Config()``.

    ``CAN_FetchMessage()`` is a wrapper for `xQueueReceive() <https://www.freertos.org/a00118.html>`_ and retrieves/removes a message from the Rx Queue.

    The ``CAN_Config()`` function returns a ``HAL_StatusTypeDef`` status and the ``CAN_FetchMessage()`` function returns a ``BaseType_t`` status.

Additional Considerations
    The CAN drivers work only with the RTOS running since the 
    `FreeRTOS Queue <https://www.freertos.org/a00018.html>`_ is used for recieving messages. 

    A basic transmit function ``CAN_TransmitMessage()`` is included for testing purposes. This function does not use the ``CANMSG_t`` struct; the function takes raw CAN message data to send.

Ethernet
========

Purpose

Usage

Additional Considerations

Inertial Measurement Unit (IMU)
===============================

Purpose

Usage

Additional Considerations

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
