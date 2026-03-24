#ifndef __BAG_XYGSZCNEO_MARKP__
#define __BAG_XYGSZCNEO_MARKP__

#include "geometric.h"

#define wgs2xy(x)	((x)*EARTH_R*PI/180)
#define xy2wgs(x)	((x)/EARTH_R/PI*180)

#pragma pack(1)
typedef struct _MARK_PARA
{
    s32 x;
    s32 y;
    float z;
    float rz;
    Vector3D ang_pl;
    Vector3D ang_enc;
    Vector3D ang_cam;
    Vector3D ang_v;
    float dist;
    s32 tx;
    s32 ty;
    float tz;

public:
    _MARK_PARA()
    {
        x = 116e7;
        y = 395e6;
    }

} MARK_PARA;
#pragma pack()

extern MARK_PARA pos_para;
extern float visual_eular[3];
extern s32 target_pos_laster[3];
extern s32 target_pos_relheight[3];
extern float rel_height_cfg;

void get_visual_eular(void);
void posit_laser(void);
void posit_rel_height(void);

void get_visual_eular_para(MARK_PARA &para);

void posit_rel_height_para(s32 &target_lon, s32 &target_lat, s32 &target_alt, MARK_PARA &para);

#endif
