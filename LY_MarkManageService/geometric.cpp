#include "geometric.h"
#include<math.h>

double deg_range(double d)
{
    while (d > 180)
    {
        d -= 360;
    }
    while (d <= -180)
    {
        d += 360;
    }
    return d;
}
double rad_range(double d)
{
    while (d > PI)
    {
        d -= PI * 2;
    }
    while (d <= -PI)
    {
        d += PI * 2;
    }
    return d;
}

Vector2D operator+(Vector2D p1, Vector2D p2)
{
    p1.x += p2.x;
    p1.y += p2.y;
    return p1;
}
Vector2D operator-(Vector2D p1, Vector2D p2)
{
    p1.x -= p2.x;
    p1.y -= p2.y;
    return p1;
}
double operator*(Vector2D p1, Vector2D p2)
{
    return p1.x * p2.x + p1.y * p2.y;
}
Vector2D operator+(Vector2D p1, double k)
{
    p1.x += k;
    p1.y += k;
    return p1;
}
Vector2D operator-(Vector2D p1, double k)
{
    p1.x -= k;
    p1.y -= k;
    return p1;
}
Vector2D operator*(Vector2D p1, double k)
{
    p1.x *= k;
    p1.y *= k;
    return p1;
}
Vector2D operator/(Vector2D p1, double k)
{
    p1.x /= k;
    p1.y /= k;
    return p1;
}
double Vector2D::magnitude(void)
{
    return sqrt(x * x + y * y);
}
double Vector2D::norm(void)
{
    double mag = magnitude();
    if (fabs(mag) > Eps)
    {
        x /= mag;
        y /= mag;
    }
    return mag;
}
double Vector2D::distance(Vector2D p1)
{
    p1 = p1 - *this;
    return p1.norm();
}
double Vector2D::angle(Vector2D p1)
{
    return acos((p1.x * x + p1.y * y) / sqrt(p1.x * p1.x + p1.y * p1.y) / sqrt(x * x + y * y));
}

Vector3D operator+(Vector3D p1, Vector3D p2)
{
    p1.x += p2.x;
    p1.y += p2.y;
    p1.z += p2.z;
    return p1;
}
Vector3D operator-(Vector3D p1, Vector3D p2)
{
    p1.x -= p2.x;
    p1.y -= p2.y;
    p1.z -= p2.z;
    return p1;
}
double operator*(Vector3D p1, Vector3D p2)
{
    return p1.x * p2.x + p1.y * p2.y + p1.z * p2.z;
}
Vector3D operator^(Vector3D p1, Vector3D p2)
{
    Vector3D p = Vector3DEmpty;
    p.x = p1.y * p2.z - p1.z * p2.y;
    p.y = p1.z * p2.x - p1.x * p2.z;
    p.z = p1.x * p2.y - p1.y * p2.x;
    return p;
}
Vector3D operator+(Vector3D p1, double k)
{
    p1.x += k;
    p1.y += k;
    p1.z += k;
    return p1;
}
Vector3D operator-(Vector3D p1, double k)
{
    p1.x -= k;
    p1.y -= k;
    p1.z -= k;
    return p1;
}
Vector3D operator*(Vector3D p1, double k)
{
    p1.x *= k;
    p1.y *= k;
    p1.z *= k;
    return p1;
}
Vector3D operator/(Vector3D p1, double k)
{
    p1.x /= k;
    p1.y /= k;
    p1.z /= k;
    return p1;
}
double Vector3D::magnitude(void)
{
    return sqrt(x * x + y * y + z * z);
}
double Vector3D::norm(void)
{
    double mag = magnitude();
    if (fabs(mag) > Eps)
    {
        x /= mag;
        y /= mag;
        z /= mag;
    }
    return mag;
}
double Vector3D::distance(Vector3D p1)
{
    p1 = p1 - *this;
    return p1.norm();
}
double Vector3D::angle(Vector3D p1)
{
    return acos((p1.x * x + p1.y * y + p1.z * z) / sqrt(p1.x * p1.x + p1.y * p1.y + p1.z * p1.z)
                / sqrt(x * x + y * y + z * z));
}

void Line2D::create(Vector2D &p1, Vector2D &p2)
{
    k = p2 - p1;
    p = p1;
}
Vector2D Line2D::vertical(void)
{
    Vector2D vec;
    vec.x = -k.y;
    vec.y = k.x;
    return vec;
}
Line2D Line2D::vertical(Vector2D &p1)
{
    Line2D ret;
    ret.k = vertical();
    ret.p = p1;
    return ret;
}
bool Line2D::cross(Line2D &l, Vector2D &p1)
{
    double t = 0;
    t = (k.x * l.k.y - k.y * l.k.x);
    if (fabs(t) < 0.000001)
    {
        return false;
    }
    t = (l.k.y * (l.p.x - p.x) + l.k.x * (p.y - l.p.y)) / t;
    p1.x = k.x * t + p.x;
    p1.y = k.y * t + p.y;
    return true;
}
Vector2D Line2D::Cal_P_Dis(Vector2D &p1, double d)
{
    double t;
    t = d / k.magnitude();
    return k * t + p1;
}
Vector2D Line2D::vert_point(Vector2D p1)
{
    Line2D lv;
    lv = vertical(p1);
    cross(lv, p1);
    return p1;
}
double Line2D::distance(Vector2D &p1)
{
    Vector2D p2 = vert_point(p1);
    return p2.distance(p1);
}

void Line3D::create(Vector3D &p1, Vector3D &p2)
{
    k = p2 - p1;
    p = p1;
}
Line3D Line3D::vertical(Vector3D &p1)
{
    Vector3D tmp;
    tmp = vert_point(p1);
    Line3D l;
    l.create(tmp, p1);
    return l;
}
bool Line3D::cross(Line3D &l, Vector3D &p1)
{

    Vector3D kv;
    kv = k ^ l.k;
    double d = kv.norm();
    if (fabs(d) < Eps)
    {
        return false;
    }

    d = k.x * (l.k.y * kv.z - l.k.z * kv.y) -
        k.y * (l.k.x * kv.z - l.k.z * kv.x) +
        k.z * (l.k.x * kv.y - l.k.y * kv.x);
    double bx = l.p.x - p.x;
    double by = l.p.y - p.y;
    double bz = l.p.z - p.z;
    double t1 = (l.k.y * kv.z - l.k.z * kv.y) * bx +
                (l.k.z * kv.x - l.k.x * kv.z) * by +
                (l.k.x * kv.y - l.k.y * kv.x) * bz;
    double t2 = (k.y * kv.z - k.z * kv.y) * bx +
                (k.z * kv.x - k.x * kv.z) * by +
                (k.x * kv.y - k.y * kv.x) * bz;
    t1 /= d;
    t2 /= d;
    p1 = (k * t1 + p + l.k * t2 + l.p) / 2;
    return true;
}
double Line3D::line_distance(Line3D &l)
{
    Vector3D kv;
    kv = k ^ l.k;
    kv.norm();

    return kv * (l.p - p);
}
Vector3D Line3D::Cal_P_Dis(Vector3D &p1, double d)
{
    double t;
    t = d / k.magnitude();
    return k * t + p1;
}
Vector3D Line3D::vert_point(Vector3D p1)
{
    Surface sf;
    sf.create_vert(*this, p1);
    sf.cross(*this, p1);
    return p1;
}
double Line3D::distance(Vector3D &p1)
{
    Vector3D tmp;
    tmp = vert_point(p1);
    return p1.distance(tmp);
}

void Surface::create(Line3D &l, Vector3D &p1)
{
    k = l.k ^ (p1 - l.p);
    b = k * p1;
}
void Surface::create_vert(Line3D &l, Vector3D &p1)
{
    k = l.k;
    b = k * p1;
}
bool Surface::cross(Line3D &l, Vector3D &p1)
{

    double AK = k * l.k;
    if (fabs(AK) < Eps)
    {
        return false;
    }
    double t = (b - k * l.p) / AK;
    p1 = l.k * t + l.p;
    return true;
}
Vector3D Surface::vert_point(Vector3D p1)
{
    Line3D l;
    l.k = k;
    l.p = p1;
    cross(l, p1);
    return p1;
}
double Surface::distance(Vector3D &p1)
{
    Vector3D tmp;
    tmp = vert_point(p1);
    return tmp.distance(p1);
}

Vector2D matrix_trans2D(Vector2D *k, double t[][3])
{
    int i, s;
    Vector2D ret;
    double m[3] = { k->x, k->y, 1 };
    double r[3] = { 0, 0, 0 };
    for (i = 0; i < 3; i++)
    {
        for (s = 0; s < 3; s++)
        {
            r[i] = m[s] * t[s][i] + r[i];
        }
    }
    ret.x = r[0];
    ret.y = r[1];
    return ret;
}

Vector3D matrix_trans3D(Vector3D *k, double t[][4])
{
    int i, s;
    Vector3D ret;
    double m[4] = { k->x, k->y, k->z, 1 };
    double r[4] = { 0, 0, 0, 0 };
    for (i = 0; i < 4; i++)
    {
        for (s = 0; s < 4; s++)
        {
            r[i] = m[s] * t[s][i] + r[i];
        }
    }
    ret.x = r[0];
    ret.y = r[1];
    ret.z = r[2];
    return ret;
}

Vector2D translate2D(Vector2D *k, double tx, double ty)
{
    int i, s;
    Vector2D ret;
    double m[3] = { k->x, k->y, 1 };
    double t[3][3] = { { 1, 0, 0 }, { 0, 1, 0 }, { tx, ty, 1 } };
    double r[3] = { 0, 0, 0 };
    for (i = 0; i < 3; i++)
        for (s = 0; s < 3; s++)
        {
            r[i] = m[s] * t[s][i] + r[i];
        }
    ret.x = r[0];
    ret.y = r[1];
    return ret;
}

Vector2D scale2D(Vector2D *k, double a, double d)
{
    int i, s;
    Vector2D ret;
    double m[3] = { k->x, k->y, 1 };
    double t[3][3] = { { a, 0, 0 }, { 0, d, 0 }, { 0, 0, 1 } };
    double r[3] = { 0, 0, 0 };
    for (i = 0; i < 3; i++)
        for (s = 0; s < 3; s++)
        {
            r[i] = m[s] * t[s][i] + r[i];
        }
    ret.x = r[0];
    ret.y = r[1];
    return ret;
}

Vector2D scaleToVector2D(Vector2D *k, Vector2D *c, double a, double d)
{
    int i, s;
    Vector2D ret;
    double m[3] = { k->x, k->y, 1 };
    double t[3][3] = { { a, 0, 0 }, { 0, d, 0 }, { c->x *(1 - a), c->y *(1 - d), 1 } };
    double r[3] = { 0, 0, 0 };
    for (i = 0; i < 3; i++)
        for (s = 0; s < 3; s++)
        {
            r[i] = m[s] * t[s][i] + r[i];
        }
    ret.x = r[0];
    ret.y = r[1];
    return ret;
}

Vector2D rotate2D(Vector2D *k, double r)
{
    int i, s;
    Vector2D ret;
    double m[3] = { k->x, k->y, 1 };
    double t[3][3] = { { cos(r), sin(r), 0 }, { -sin(r), cos(r), 0 }, { 0, 0, 1 } };
    double n[3] = { 0, 0, 0 };
    for (i = 0; i < 3; i++)
        for (s = 0; s < 3; s++)
        {
            n[i] = m[s] * t[s][i] + n[i];
        }
    ret.x = n[0];
    ret.y = n[1];
    return ret;
}

Vector2D rotateToVector2D(Vector2D *k, Vector2D *c, double r)
{
    int i, s;
    Vector2D ret;
    double m[3] = { k->x, k->y, 1 };
    double t[3][3] = { { cos(r), sin(r), 0 }, { -sin(r), cos(r), 0 }, { c->x - c->x * cos(r) + c->y * sin(r), c->y - c->x * sin(r) - c->y * cos(r), 1 } };
    double n[3] = { 0, 0, 0 };
    for (i = 0; i < 3; i++)
        for (s = 0; s < 3; s++)
        {
            n[i] = m[s] * t[s][i] + n[i];
        }
    ret.x = n[0];
    ret.y = n[1];
    return ret;
}

void rotate3D_euler(Vector3D *p, Vector3D *euler)
{
    double sa = sin(euler->x);
    double sb = sin(euler->y);
    double sr = sin(euler->z);
    double ca = cos(euler->x);
    double cb = cos(euler->y);
    double cr = cos(euler->z);
    double x = 0, y = 0, z = 0;
    x = p->x * (cb * cr - sb * sa * sr) + p->y * (-cb * sr - sb * sa * cr) + p->z * (-sb * ca);
    y = p->x * (ca * sr) + p->y * (ca * cr) + p->z * (-sa);
    z = p->x * (sb * cr + cb * sa * sr) + p->y * (-sb * sr + cb * sa * cr) + p->z * (cb * ca);
    p->x = x;
    p->y = y;
    p->z = z;
}

void rotate3D_euler_zxy(Vector3D *p, Vector3D *euler)
{
    double sa = sin(euler->x);
    double sb = sin(euler->y);
    double sr = sin(euler->z);
    double ca = cos(euler->x);
    double cb = cos(euler->y);
    double cr = cos(euler->z);
    double x = 0, y = 0, z = 0;
    x = p->x * (cr * cb - sr * sa * sb) + p->y * (-sr * ca) + p->z * (cr * sb + sr * sa * cb);
    y = p->x * (sr * cb + cr * sa * sb) + p->y * (ca * cr) + p->z * (sr * sb - cr * sa * cb);
    z = p->x * (-ca * sb) + p->y * (sa) + p->z * (cb * ca);
    p->x = x;
    p->y = y;
    p->z = z;
}

void rotate3D_euler_zyx(Vector3D *p, Vector3D *euler)
{
    double sa = sin(euler->x);
    double sb = sin(euler->y);
    double sr = sin(euler->z);
    double ca = cos(euler->x);
    double cb = cos(euler->y);
    double cr = cos(euler->z);
    double x = 0, y = 0, z = 0;
    x = p->x * (cr * cb) + p->y * (-sr * ca + cr * sb * sa) + p->z * (sr * sa + cr * sb * ca);
    y = p->x * (sr * cb) + p->y * (ca * cr + sr * sb * sa) + p->z * (-cr * sa + sr * sb * ca);
    z = p->x * (-sb) + p->y * (cb * sa) + p->z * (cb * ca);
    p->x = x;
    p->y = y;
    p->z = z;
}

void rotate3D_euler_xyz(Vector3D *p, Vector3D *euler)
{
    double sa = sin(euler->x);
    double sb = sin(euler->y);
    double sr = sin(euler->z);
    double ca = cos(euler->x);
    double cb = cos(euler->y);
    double cr = cos(euler->z);
    double x = 0, y = 0, z = 0;
    x = p->x * (cb * cr) + p->y * (-cb * sr) + p->z * (sb);
    y = p->x * (ca * sr + cr * sa * sb) + p->y * (ca * cr - sa * sb * sr) + p->z * (-cb * sa);
    z = p->x * (sa * sr - ca * cr * sb) + p->y * (cr * sa + ca * sb * sr) + p->z * (ca * cb);
    p->x = x;
    p->y = y;
    p->z = z;
}

double QuatVec::norm(void)
{
    double s = sqrt(w * w + x * x + y * y + z * z);
    w /= s;
    x /= s;
    y /= s;
    z /= s;
    return s;
}

void operator*=(QuatVec &q1, QuatVec &q2)
{
    double w = q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z;
    double x = q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y;
    double y = q1.w * q2.y - q1.x * q2.z + q1.y * q2.w + q1.z * q2.x;
    double z = q1.w * q2.z + q1.x * q2.y - q1.y * q2.x + q1.z * q2.w;
    q1.w = w;
    q1.x = x;
    q1.y = y;
    q1.z = z;
    q1.norm();
}
QuatVec operator*(QuatVec q1, QuatVec &q2)
{
    q1 *= q2;
    return q1;
}

void QuatVec::rot(Vector3D &p)
{
    double x2 = x * x;
    double y2 = y * y;
    double z2 = z * z;
    double xy = x * y;
    double xz = x * z;
    double yz = y * z;
    double wx = w * x;
    double wy = w * y;
    double wz = w * z;
    double px = (1 - 2 * (y2 + z2)) * p.x + 2 * (xy - wz)     * p.y + 2 * (xz + wy)     * p.z;
    double py = 2 * (xy + wz)     * p.x + (1 - 2 * (x2 + z2)) * p.y + 2 * (yz - wx)     * p.z;
    double pz = 2 * (xz - wy)     * p.x + 2 * (yz + wx)     * p.y + (1 - 2 * (x2 + y2)) * p.z;
    p.x = px;
    p.y = py;
    p.z = pz;
}
Vector3D QuatVec::toEuler_zyx(void)
{
    Vector3D p;
    p.x = atan2(2 * (w * x + z * y), 1 - 2 * (y * y + x * x));
    p.y = asin(2 * (w * y - x * z));
    p.z = atan2(2 * (w * z + y * x), 1 - 2 * (y * y + z * z));
    return p;
}
Vector3D QuatVec::toEuler_zxy(void)
{
    Vector3D p;
    p.x = asin(2 * (w * x + y * z));
    p.y = -atan2(2 * (x * z - w * y), 1 - 2 * (y * y + x * x));
    p.z = -atan2(2 * (x * y - w * z), 1 - 2 * (x * x + z * z));
    return p;
}
void QuatVec::fromEuler_zyx(Vector3D &p)
{
    double cx = cos(p.x / 2);
    double sx = sin(p.x / 2);
    double cy = cos(p.y / 2);
    double sy = sin(p.y / 2);
    double cz = cos(p.z / 2);
    double sz = sin(p.z / 2);
    w = cx * cy * cz + sx * sy * sz;
    x = sx * cy * cz - cx * sy * sz;
    y = cx * sy * cz + sx * cy * sz;
    z = cx * cy * sz - sx * sy * cz;
}
void QuatVec::fromAxis(Vector3D &axis, double angle)
{
    angle /= 2;
    w = cos(angle);
    x = sin(angle) * axis.x;
    y = sin(angle) * axis.y;
    z = sin(angle) * axis.z;
}

void Loader2Carrier(Vector3D &cur, Vector3D &dt)
{
    QuatVec qcur;
    QuatVec qdt;

    qcur.fromEuler_zyx(cur);
    qdt.fromEuler_zyx(dt);

    qcur *= qdt;

    cur = qcur.toEuler_zyx();

}

Vector3D blh_2_xyz_ellipse(Vector3D blh)
{

    blh.x = deg2rad(blh.x);
    blh.y = deg2rad(blh.y);
    const double a2 = EARTH_R * EARTH_R;
    const double b2 = EARTH_R_SHORT * EARTH_R_SHORT;
    const double e2 = (a2 - b2) / a2;
    double sinB = sin(blh.y);
    double w = sqrt(1 - (e2 * sinB * sinB));
    double n = EARTH_R / w;
    Vector3D tmp;
    tmp.x = (n + blh.z) * cos(blh.y) * cos(blh.x);
    tmp.y = (n + blh.z) * cos(blh.y) * sin(blh.x);
    tmp.z = (n * (1 - e2) + blh.z) * sinB;
    return tmp;
}

Vector3D xyz_2_blh_ellipse(Vector3D xyz)
{
    double x2 = xyz.x * xyz.x;
    double y2 = xyz.y * xyz.y;
    const double a2 = EARTH_R * EARTH_R;
    const double b2 = EARTH_R_SHORT * EARTH_R_SHORT;
    const double e2 = (a2 - b2) / a2;
    const double e12 = (a2 - b2) / b2;
    double sqrt_x2_y2 = sqrt(x2 + y2);
    double theata = atan2(EARTH_R * xyz.z, EARTH_R_SHORT * sqrt_x2_y2);
    double sintheata = sin(theata);
    double costheata = cos(theata);
    Vector3D tmp;
    tmp.x = atan2(xyz.y, xyz.x);
    tmp.y = atan2(xyz.z + EARTH_R_SHORT * e12 * sintheata * sintheata * sintheata,
                  sqrt_x2_y2 - EARTH_R * e2 * costheata * costheata * costheata);
    double sinB = sin(tmp.y);
    double n = EARTH_R / sqrt(1 - (e2 * sinB * sinB));
    tmp.z = sqrt_x2_y2 / cos(tmp.y) - n;
    tmp.x = rad2deg(tmp.x);
    tmp.y = rad2deg(tmp.y);
    return tmp;
}

Vector3D blh_2_xyz(Vector3D blh)
{

    blh.x = deg2rad(blh.x);
    blh.y = deg2rad(blh.y);
    Vector3D tmp;
    double sinB = sin(blh.y);
    double cosB = cos(blh.y);
    tmp.z = (EARTH_R + blh.z);
    tmp.x = tmp.z * cosB * cos(blh.x);
    tmp.y = tmp.z * cosB * sin(blh.x);
    tmp.z = tmp.z * sinB;
    return tmp;
}

Vector3D xyz_2_blh(Vector3D xyz)
{
    double x2 = xyz.x * xyz.x;
    double y2 = xyz.y * xyz.y;
    double z2 = xyz.z * xyz.z;
    Vector3D tmp;
    tmp.x = atan2(xyz.y, xyz.x);
    tmp.y = atan2(xyz.z, sqrt(x2 + y2));
    tmp.z = sqrt(x2 + y2 + z2) - EARTH_R;
    tmp.x = rad2deg(tmp.x);
    tmp.y = rad2deg(tmp.y);
    return tmp;
}
