#include "Bag_XygsZCNEO_Markp.h"
#include "Protocol/XygsZCNEO/Bag_XygsZCNEOType.h"
#include <math.h>

s32 target_pos_laser[3];
s32 target_pos_relheight[3] = { 1160000000, 394000000, 40000 };
float rel_height_cfg = 40;

MARK_PARA pos_para;

Vector3D get_visual_eular_vector(Vector3D &eul_plane, Vector3D &eul_enc, Vector3D &eul_cam)
{

    Vector3D unit;
    unit.x = 1;
    unit.y = 0;
    unit.z = 0;

    rotate3D_euler_zyx(&unit, &eul_enc);

    rotate3D_euler_zyx(&unit, &eul_plane);

    float yaw = atan2(unit.y, unit.x);
    unit = eul_cam;
    unit.z = yaw;
    return unit;
}

Line3D get_target_vector(MARK_PARA &pos)
{
    double x = double(pos.x) / 1e7;
    double y = double(pos.y) / 1e7;

    Vector3D unit;
    unit.x = 1;
    unit.y = 0;
    unit.z = 0;
    rotate3D_euler_zyx(&unit, &pos_para.ang_v);
    Line3D line_targ;
    line_targ.k = unit;
    unit.x = x;
    unit.y = y;
    unit.z = pos.z;
    line_targ.p = unit;
    line_targ.k.norm();
    return line_targ;
}

int targetp_relheight(MARK_PARA &pos)
{

    Line3D line_targ = get_target_vector(pos);
    Vector3D pt;
    float pitch = atan2(-line_targ.k.z, sqrt(line_targ.k.x * line_targ.k.x + line_targ.k.y * line_targ.k.y));

    if (pitch < deg2rad(2))
    {
        return 1;
    }
    float dist = pos.rz / sin(pitch);
    pt = line_targ.k * dist;
    double xx = xy2wgs(pt.x) / cos(deg2rad(line_targ.p.y)) + line_targ.p.x;
    double yy = xy2wgs(pt.y) + line_targ.p.y;
    pos.tx = xx * 1e7;
    pos.ty = yy * 1e7;
    pos.tz = pt.z * 1000;
    return 0;
}

int targetp_distance(MARK_PARA &pos)
{

    Line3D line_targ = get_target_vector(pos);
    Vector3D pt;
    pt = line_targ.k * pos.dist;
    double xx = xy2wgs(pt.x) / cos(deg2rad(line_targ.p.y)) + line_targ.p.x;
    double yy = xy2wgs(pt.y) + line_targ.p.y;
    pos.tx = xx * 1e7;
    pos.ty = yy * 1e7;
    pos.tz = pt.z * 1000;
    return 0;
}

Vector3D target_2_rot(MARK_PARA &pos, Vector3D &targ)
{
    double x = double(pos.x) / 1e7;
    double y = double(pos.y) / 1e7;

    targ.x = wgs2xy((targ.x - pos.x) / 1e7) * cos(pos.y / 1e7 / 180.0 * PI);
    targ.y = wgs2xy((targ.y - pos.y) / 1e7);
    targ.z = targ.z - pos.z;

    Vector3D eul;
    eul.x = 0;
    eul.y = -atan2(targ.z, sqrt(targ.x * targ.x + targ.y * targ.y));
    eul.z = atan2(targ.y, targ.x);
    return eul;
}

#define WIN32
#ifndef WIN32
extern "C"
{
#include "STM32F0xx_MODEM_UART.h"
#include "STM32F0xx_BGC_UART.h"
}

extern struct MODEM_INFO modem;
extern struct BGC_INFO bgc;
extern "C"
{
    void get_visual_eular(void)
    {
        pos_para.x = modem.status_lon_int;
        pos_para.y = modem.status_lat_int;
        pos_para.z = modem.status_altasl_int * 0.001f;
        pos_para.rz = modem.status_altrel_int * 0.001f;
        pos_para.ang_pl.x = deg2rad(modem.status_ext_imu[0]);
        pos_para.ang_pl.y = deg2rad(modem.status_ext_imu[1]);
        pos_para.ang_pl.z = deg2rad(90 - modem.status_ext_imu[2]);
        pos_para.ang_enc.x = deg2rad(modem.status_encoder[0]);
        pos_para.ang_enc.y = deg2rad(modem.status_encoder[1]);
        pos_para.ang_enc.z = -deg2rad(modem.status_encoder[2]);
        pos_para.ang_cam.x = deg2rad(bgc.imu[0]);
        pos_para.ang_cam.y = deg2rad(bgc.imu[1]);
        pos_para.ang_cam.z = deg2rad(90 - bgc.imu[2]);
        pos_para.dist = modem.status_laser_distance;
        pos_para.ang_v = get_visual_eular_vector(pos_para.ang_pl, pos_para.ang_enc, pos_para.ang_cam);
        modem.status_cam_imu[0] = bgc.imu[0];
        modem.status_cam_imu[1] = bgc.imu[1];
        modem.status_cam_imu[2] = 90 - rad2deg(pos_para.ang_v.z);
    }
    void posit_laser(void)
    {
        targetp_distance(pos_para);
        modem.status_target_lon_measurement_int = pos_para.tx;
        modem.status_target_lat_measurement_int = pos_para.ty;
        modem.status_target_altasl_measurement_int = pos_para.tz;
    }
    void posit_rel_height(void)
    {
        targetp_relheight(pos_para);
        modem.status_target_lon_estimation_int = pos_para.tx;
        modem.status_target_lat_estimation_int = pos_para.ty;
        modem.status_target_altasl_estimation_int = pos_para.tz;
    }
}
#else

void get_visual_eular(void)
{
    pos_para.ang_v = get_visual_eular_vector(pos_para.ang_pl, pos_para.ang_enc, pos_para.ang_cam);
}
void posit_laser(void)
{
    targetp_distance(pos_para);
    target_pos_laser[0] = pos_para.tx;
    target_pos_laser[1] = pos_para.ty;
    target_pos_laser[2] = pos_para.tz * 1000;
}
void posit_rel_height(void)
{
    targetp_relheight(pos_para);
    target_pos_relheight[0] = pos_para.tx;
    target_pos_relheight[1] = pos_para.ty;
    target_pos_relheight[2] = pos_para.tz * 1000;
}

void get_visual_eular_para(MARK_PARA &para)
{
    para.ang_v = get_visual_eular_vector(para.ang_pl, para.ang_enc, para.ang_cam);
}

void posit_rel_height_para(s32 &target_lon, s32 &target_lat, s32 &target_alt, MARK_PARA &para)
{
    targetp_relheight(para);
    target_lon = para.tx;
    target_lat = para.ty;
    target_alt = para.tz * 1000;
}

#endif
