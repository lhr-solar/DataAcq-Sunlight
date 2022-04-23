#ifndef GPS_H__
#define GPS_H__

typedef struct{
    char hr[2]; // Will not use these parameters unless we have to
    char min[2]; // ^^
    char sec[2]; // ^^
    char ms[3]; // ^^
    char latitude_Deg[2];
    char latitude_Min[6];
    char NorthSouth;
    char longitude_Deg[3];
    char longitude_Min[6];
    char EastWest;
    char speedInKnots[4];
    char day[2]; // Will not use these parameters unless we have to
    char month[2]; // ^^
    char year[4]; // ^^
    char magneticVariation_Deg[4];
    char magneticVariation_EastWest;
} GPSData_t;

#endif
