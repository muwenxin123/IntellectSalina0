#ifndef GEOMETRIC_H
#define GEOMETRIC_H

#include "Protocol/XygsZCNEO/Bag_XygsZCNEOType.h"

const double PI = 3.1415926;
#define deg2rad(a)	((a)*(PI/180.0))
#define rad2deg(a)	((a)*(180.0f/PI))

double deg_range(double d);
double rad_range(double d);

struct Vector2D
{
    double x;
    double y;
    double magnitude(void);
    double norm(void);
    double distance(Vector2D p1);
    double angle(Vector2D p1);
};
const Vector2D Vector2DEmpty = { 0, 0 };
Vector2D operator+(Vector2D p1, Vector2D p2);
Vector2D operator-(Vector2D p1, Vector2D p2);
double operator*(Vector2D p1, Vector2D p2);
Vector2D operator+(Vector2D p1, double k);
Vector2D operator-(Vector2D p1, double k);
Vector2D operator*(Vector2D p1, double k);
Vector2D operator/(Vector2D p1, double k);

struct Vector3D
{
    double x;
    double y;
    double z;
    double magnitude(void);
    double norm(void);
    double distance(Vector3D p1);
    double angle(Vector3D p1);
};
const Vector3D Vector3DEmpty = { 0, 0, 0 };
Vector3D operator+(Vector3D p1, Vector3D p2);
Vector3D operator-(Vector3D p1, Vector3D p2);
double operator*(Vector3D p1, Vector3D p2);
Vector3D operator^(Vector3D p1, Vector3D p2);
Vector3D operator+(Vector3D p1, double k);
Vector3D operator-(Vector3D p1, double k);
Vector3D operator*(Vector3D p1, double k);
Vector3D operator/(Vector3D p1, double k);

struct Line2D
{
    Vector2D k;
    Vector2D p;
    void create(Vector2D &p1, Vector2D &p2);
    Vector2D vertical(void);
    Line2D vertical(Vector2D &p1);
    bool cross(Line2D &l, Vector2D &p1);
    Vector2D Cal_P_Dis(Vector2D &p1, double d);
    Vector2D vert_point(Vector2D p1);
    double distance(Vector2D &p1);
};
const Line2D Line2DEmpty = { 0, 0, 0, 0 };

struct Line3D
{
    Vector3D k;
    Vector3D p;
    void create(Vector3D &p1, Vector3D &p2);
    Line3D vertical(Vector3D &p1);
    bool cross(Line3D &l, Vector3D &p1);
    double line_distance(Line3D &l);
    Vector3D Cal_P_Dis(Vector3D &p1, double d);
    Vector3D vert_point(Vector3D p1);
    double distance(Vector3D &p1);
};
const Line3D Line3DEmpty = { 0, 0, 0, 0, 0, 0 };

struct Surface
{
    Vector3D k;
    double b;
    void create(Line3D &l, Vector3D &p1);
    void create_vert(Line3D &l, Vector3D &p1);
    bool cross(Line3D &l, Vector3D &p1);
    Vector3D vert_point(Vector3D p1);
    double distance(Vector3D &p1);
};
const Surface SurfaceEmpty = { 0, 0, 0, 0 };

typedef struct
{
    Vector2D s;
    Vector2D e;
} Segment2D;
const Segment2D Segment2DEmpty = { 0, 0, 0, 0 };

typedef struct
{
    Vector3D s;
    Vector3D e;
} Segment3D;
const Segment3D Segment3DEmpty = { 0, 0, 0, 0, 0, 0 };

struct QuatVec
{
    double w;
    double x;
    double y;
    double z;
    double norm(void);
    void rot(Vector3D &p);
    Vector3D toEuler_zyx(void);
    Vector3D toEuler_zxy(void);
    void fromEuler_zyx(Vector3D &p);
    void fromAxis(Vector3D &axis, double angle);
};
void operator*=(QuatVec &q1, QuatVec &q2);
QuatVec operator*(QuatVec &q1, QuatVec &q2);

Vector2D matrix_trans2D(Vector2D *k, double t[][3]);
Vector3D matrix_trans3D(Vector3D *k, double t[][4]);
Vector2D translate2D(Vector2D *k, double tx, double ty);
Vector2D scale2D(Vector2D *k, double a, double d);
Vector2D scaleToVector2D(Vector2D *k, Vector2D *c, double a, double d);
Vector2D rotate2D(Vector2D *k, double r);
Vector2D rotateToVector2D(Vector2D *k, Vector2D *c, double r);
void rotate3D_euler(Vector3D *p, Vector3D *euler);

void rotate3D_euler_yxz(Vector3D *p, Vector3D *euler);

void rotate3D_euler_zxy(Vector3D *p, Vector3D *euler);

void rotate3D_euler_zyx(Vector3D *p, Vector3D *euler);

void rotate3D_euler_xyz(Vector3D *p, Vector3D *euler);

void Loader2Carrier(Vector3D &cur, Vector3D &dt);

const double EARTH_R = 6378137.0;
const double EARTH_R_SHORT = 6356752.0;

Vector3D blh_2_xyz_ellipse(Vector3D blh);

Vector3D xyz_2_blh_ellipse(Vector3D xyz);

Vector3D blh_2_xyz(Vector3D blh);

Vector3D xyz_2_blh(Vector3D xyz);

# endif
