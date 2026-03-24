#if !defined(GEN_STRUCT_H_INCLUDE)
#define GEN_STRUCT_H_INCLUDE

#include "const.h"
#include <QPoint>
#include <QTreeWidgetItem>

struct dPoint
{
    double dx;
    double dy;
};

struct Common
{
    short  attr;
    short  type;
    float  threat;
};

struct turn_point_s
{
    double x;
    double y;
};

struct	Poly
{
    short	n;
	struct	turn_point_s h[MAX_ky_point];
};

typedef  struct _My_Dot
{
    int type;
    float jd;
    float wd;
} My_Dot;

typedef  struct _New_Polyn
{
    bool  mshow;
    short linestyle;
    int   num;
    int    clr;
    char  name[100];
    QPoint m_nowpt;
    My_Dot  data[MAX_Polyn_num];
} New_Polyn;

typedef struct QUYU_Zone_
{
    char	name[MAX_ky_name];
    struct	Common attr;
    struct	Poly	data;
    float  	both;
    float  	toph;
    int		kycolor;
    unsigned char	type;
	unsigned char   xiankuan;
	unsigned char	xianxing;
    unsigned char   tianchong;
    unsigned char	show[3];
} QUYU_Zone;

#endif