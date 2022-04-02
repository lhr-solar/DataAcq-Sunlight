*******
Drivers
*******

Inertial Measurement Unit
=========================
Purpose
    The IMU drivers use a accelerometer, gyroscope, and magnetometer to collect data of the cars movement.
    This data can be used to calculate measurements such as acceleration and yaw when the car is turning.

Usage
    First the ``IMU_Init(IMUData_t* Data)`` must be called. It will initialize the peripheral to collect data using all 
    three measurment units. Later on, we can decide whether we actually need to collect data using everything (we can
    disable some to save power, such as the magnetometer). The struct passed in will collect all the data. Then the 
    ``IMU_UpdateMeasurements()`` function is called to update all the data collected. Both functions return whether or not
    the I2C transmit and receive were successful or not.

Additional Considerations
    The units can be initialized in many different ways. The way it is initialized now just collects the data.
    They can also be initialized in a way where calculations are done after the data is collected. If it turns out
    we need to use that method of initialization instead, the header file will have to be changed to include the 
    register addresses of where that data is located and the struct will have to be changed as well.