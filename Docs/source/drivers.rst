*******
Drivers
*******

GPS Drivers
===========
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