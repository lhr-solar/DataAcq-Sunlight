********
Drivers
********

Radio
=====
Purpose
    The purpose of this driver is to send messages to the pit crew. Ideally, the data sent across
    will be used to create a race strategy.

Usage
    First ``radio_Init()`` must be called. In this init function a semaphore is created that is used
    by the ``ethernetif`` file to collect data and store in ``pbuf``. A thread is also created to 
    do this. Then ``radio_RX`` will receive as many bytes as stored on this buffer. If there are no
    bytes, then it will return an ``ERROR``. ``radio_TX`` sends data across the ethernet and will 
    return an error if the buffer is not large enough to send the data. 

Additional Considerations
    The data sent across ethernet will follow the same structure as CAN messages. This is to keep
    everything uniform and easy to understand. This also makes it easier to send CAN messages 
    across to the pit crew.