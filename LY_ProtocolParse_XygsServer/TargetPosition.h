#ifndef __LYKJ_NEBULA_MARK_POSITION_H__
#define __LYKJ_NEBULA_MARK_POSITION_H__

#include <math.h>
#include <stdint.h>

typedef struct __vehicle_info_t
{
    double latitude;
    double longitude;
    double altitude;

    float roll;
    float pitch;
    float heading;
} Vehicle_Info;

typedef struct __rangefinder_info_t
{
    double  gimbalHeading;
    double  gimbalPitch;
    double  distance;
} Rangefinder_Info;

typedef struct __target_position_t
{
    double targetLatitude;
    double targetLongitude;
    double targetAltitude;
} Mark_Position;

Mark_Position GetMarkPosition(const Rangefinder_Info &rangefinder_info, const Vehicle_Info &vehicle_info);
#endif