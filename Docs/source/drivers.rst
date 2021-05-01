********
Drivers
********

Radio
=====
Purpose
    The purpose of this driver is to send messages to the pit crew. Ideally, the data sent across
    will be used to create a race strategy.

Usage
    First ``radio_Init()`` must be called. This function creates a socket that will be used for the pit
    crew to connect to our radio. An error will be thrown if the socket was not initialized properly.
    If more functions are created, the socket must be connected at the start of every function and closed 
    at the end. ``radio_RX()`` is called to receive any information send from the pit crew. Although this will
    most likely be ``char`` commands, the pointer is ``void`` at the moment until changes are finalized. The 
    ``radio_TX()`` function accepts a ``CANPayload_t`` data type to be sent to the pit crew.


Additional Considerations
    The data sent across ethernet will follow the same structure as CAN messages. This is to keep
    everything uniform and easy to understand. This also makes it easier to send CAN messages 
    across to the pit crew.