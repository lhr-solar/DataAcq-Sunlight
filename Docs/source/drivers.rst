*******
Drivers
*******

GPS Drivers
===========
Purpose
    The purpose of these drivers is to detect the location of the car so it can be used in race strategy to determine
    where it should go next.

Usage
    First the ``GPS_Init()`` must be called. It will initialize the peripheral to collect data using previous mesurements 
    taken (so past location). It will also only tell us information about our location, not including what satellites
    it is using. Then the ``GPS_UpdateMeasurements()`` function is called to update all the data collected. 
    This data is stored in the struct ``GPSData``. Neither of these function take or receive any data.

Additional Considerations
    This units initialization should be changed if it turns out we are not receiving correct data. For example, right 
    now it is initialized to think the car is stopped if it is going less than .2 m/s. If this ends up messing with our
    actual location, we should change it to something else.