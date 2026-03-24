#include "TargetPosition.h"

#include <math.h>
#include <stdint.h>

#ifndef PI
    #define PI 3.141592653589793238462643383279
#endif

const double _earth_r_a = 6378137.000;
const double _earth_r_b = 6356752.3141;
const double _earth_e = 0.006694380022;

typedef struct __polar_earth_t
{
    double reYzPit;
    double reYzAzim;
} Polar_Earth;

static Polar_Earth anglePolar_geoidPolar(double angle_azimuth, double angle_ele, double angle_roll, double angle_pitch, double angle_heading)
{
    Polar_Earth polar_earth = {0};
    double ValueTpe = (sin(angle_pitch) * cos(angle_roll) - cos(angle_pitch) * sin(angle_heading) * sin(angle_roll)) * cos(angle_ele) * cos(angle_azimuth) + (cos(angle_pitch) * cos(angle_roll) + sin(angle_pitch) * sin(angle_heading) * sin(angle_roll)) * sin(angle_ele) - cos(angle_heading) * sin(angle_roll) * cos(angle_ele) * sin(angle_azimuth);
    polar_earth.reYzPit = asin(ValueTpe);

    double valueSTur = (sin(angle_pitch) * sin(angle_roll) + cos(angle_pitch) * sin(angle_heading) * cos(angle_roll)) * cos(angle_ele) * cos(angle_azimuth) + sin(angle_ele) * (sin(angle_roll) * cos(angle_pitch) - sin(angle_pitch) * sin(angle_roll) * sin(angle_heading)) + cos(angle_roll) * cos(angle_heading) * cos(angle_ele) * sin(angle_azimuth);
    double valueCTur = cos(angle_pitch) * cos(angle_heading) * cos(angle_ele) * cos(angle_azimuth) + sin(angle_ele) * (-sin(angle_pitch) * cos(angle_heading)) - sin(angle_heading) * cos(angle_ele) * sin(angle_azimuth);

    double sines = valueSTur / cos(polar_earth.reYzPit);
    double cosines = valueCTur / cos(polar_earth.reYzPit);

    if ((sines >= 0) && (cosines > 0))
    {
        polar_earth.reYzAzim = asin(sines);
    }
    else if ((sines >= 0) && (cosines <= 0))
    {
        polar_earth.reYzAzim = PI - asin(sines);
    }
    else if ((sines < 0) && (cosines > 0))
    {
        polar_earth.reYzAzim = asin(sines);
    }
    else
    {
        polar_earth.reYzAzim = -PI - asin(sines);
    }

    return polar_earth;
}

static Mark_Position compute_position(double longitude, double latitude, double angle_azimuth, double angle_ele, double altitude, double R)
{
    double r_l = sqrt(((_earth_r_a * _earth_r_a) * (_earth_r_b * _earth_r_b)) / ((_earth_r_a * _earth_r_a) * ((sin(latitude)) * (sin(latitude))) + (_earth_r_b * _earth_r_b) * (cos(latitude) * cos(latitude))));
    double nRe_X_axis_Scene = R * cos(angle_ele) * sin(angle_azimuth);
    double nRe_Y_axis_Scene = R * cos(angle_ele) * cos(angle_azimuth);

    double dangleLatitude = nRe_Y_axis_Scene / r_l;
    double targetLatitude = latitude + dangleLatitude;

    double dangleLongitude = nRe_X_axis_Scene / (r_l * cos(targetLatitude));
    double targetLongitude = longitude + dangleLongitude;

    Mark_Position target_position = {0};
    target_position.targetLatitude  = targetLatitude;
    target_position.targetLongitude = targetLongitude;
    target_position.targetAltitude  = altitude + R * sin(angle_ele);

    return target_position;
}

Mark_Position GetMarkPosition(const Rangefinder_Info &rangefinder_info, const Vehicle_Info &vehicle_info)
{
    Mark_Position target_position = {0};
    if (rangefinder_info.distance > 20.0)
    {
        double gimbalHeading = (rangefinder_info.gimbalHeading * PI) / 180.0;
        double gimbalPitch   = (rangefinder_info.gimbalPitch * PI) / 180.0;

        double roll  = (vehicle_info.roll * PI) / 180.0;
        double pitch = (vehicle_info.pitch * PI) / 180.0;
        double heading = (vehicle_info.heading * PI) / 180.0;

        Polar_Earth polar_earth = anglePolar_geoidPolar(gimbalHeading, gimbalPitch, roll, pitch, heading);
        target_position = compute_position(
                              (vehicle_info.longitude  * PI) / 180.0,
                              (vehicle_info.latitude   * PI) / 180.0,
                              polar_earth.reYzAzim,
                              polar_earth.reYzPit,
                              vehicle_info.altitude,
                              rangefinder_info.distance);

        target_position.targetLatitude = (target_position.targetLatitude * 180.0) / PI;
        target_position.targetLongitude = (target_position.targetLongitude * 180.0) / PI;
        target_position.targetAltitude  = target_position.targetAltitude;
    }

    return target_position;
}
