#include "CoordinateTranfer.h"
#include <math.h>

const double _earth_r_a = 6378137.000;
const double _earth_r_b = 6356752.3141;
const double _earth_e = 0.006694380022;

#define DEG2RAD 0.01745329252

void CoordinateTranfer::LonLat2XYZ(double dLon, double dLat, double dAlt, double &x, double &y, double &z)
{
    double B = dLat;
    double L = dLon >= 0 ? dLon : 360.0 + dLon;
    L = L * PI / 180.0;
    B = B * PI / 180.0;

    double N = _earth_r_a / sqrt(1 - _earth_e * sin(B) * sin(B));

    x = (N + dAlt) * cos(B) * cos(L);
    y = (N + dAlt) * cos(B) * sin(L);
    z = ((1 - _earth_e) * N + dAlt) * sin(B);
}

void CoordinateTranfer::CreateTransferMatrix(double dLon, double dLat, double matrix[3][3])
{
    double B = dLat;
    double L = dLon >= 0 ? dLon : 360.0 + dLon;
    B = B * PI / 180.0;
    L = L * PI / 180.0;

    matrix[0][0] = -sin(B) * cos(L);
    matrix[0][1] = -sin(B) * sin(L);
    matrix[0][2] = cos(B);

    matrix[1][0] = -sin(L);
    matrix[1][1] = cos(L);
    matrix[1][2] = 0;

    matrix[2][0] = cos(B) * cos(L);
    matrix[2][1] = cos(B) * sin(L);
    matrix[2][2] = sin(B);
}

void CoordinateTranfer::World2Local(double dLon, double dLat, double dAlt, double dLon1, double dLat1, double dAlt1, double &x, double &y, double &z)
{
    double xp, yp, zp;
    LonLat2XYZ(dLon1, dLat1, dAlt1, xp, yp, zp);
    WorldXYZ2Local(dLon, dLat, dAlt, xp, yp, zp, x, y, z);
}

void CoordinateTranfer::World2Local(double dLon, double dLat, double dAlt, int nPtNum, double pLonOri[], double pLatOri[], double pAltOri[]
                                    , double px[], double py[], double pz[])
{
    double m[3][3];
    CreateTransferMatrix(dLon, dLat, m);

    double xo, yo, zo, x_, y_, z_;
    LonLat2XYZ(dLon, dLat, dAlt, xo, yo, zo);

    for (int i = 0; i < nPtNum; ++ i)
    {
        LonLat2XYZ(pLonOri[i], pLatOri[i], pAltOri[i], x_, y_, z_);

        x_ = x_ - xo;
        y_ = y_ - yo;
        z_ = z_ - zo;

        px[i] = m[0][0] * x_ + m[0][1] * y_ + m[0][2] * z_;
        py[i] = m[1][0] * x_ + m[1][1] * y_ + m[1][2] * z_;
        pz[i] = m[2][0] * x_ + m[2][1] * y_ + m[2][2] * z_;
    }
}

void CoordinateTranfer::XYZ2LonLat(double x, double y, double z, double &dLon, double &dLat, double &dAlt)
{
    int nSpace = 0;
    if (y >= 0)
    {
        if (x >= 0)
        {
            nSpace = 1;
        }
        else
        {
            nSpace = 2;
        }
    }
    else
    {
        if (x >= 0)
        {
            nSpace = 4;
        }
        else
        {
            nSpace = 3;
        }
    }

    double L = atan(fabs(y / x)) * 180.0 / PI;

    switch (nSpace)
    {
    case 1:
        break;
    case 2:
        L = 180.0 - L;
        break;
    case 3:
        L = L - 180.0;
        break;
    case 4:
        L = - L;
        break;
    default:
        break;
    }

    const double K = 1 + _earth_e;
    const double P = _earth_e * _earth_r_a * _earth_r_a / _earth_r_b / sqrt(x * x + y * y);

    double t0, t1, t2;
    t0 = z / sqrt(x * x + y * y);
    t1 = t0;
    t2 = t0 + P * t1 / sqrt(K + t1 * t1);

    while (fabs(t2 - t1) > 1e-6)
    {
        t1 = t2;
        t2 = t0 + P * t1 / sqrt(K + t1 * t1);
    }

    double B = atan(t1);

    double N, H;
    if (z >= 0)
    {
        N = _earth_r_a / sqrt(1 - _earth_e * sin(B) * sin(B));
        H = sqrt(x * x + y * y) / cos(B) - N;
    }
    else
    {
        N = _earth_r_a / sqrt(1 - _earth_e * sin(-B) * sin(-B));
        H = sqrt(x * x + y * y) / cos(-B) - N;
    }

    dLon = L;
    dLat = B * 180.0 / PI;
    dAlt = H;
}

void CoordinateTranfer::Local2World(double dLon, double dLat, double dAlt, double x, double y, double z, double &dLon1, double &dLat1, double &dAlt1)
{
    double x_, y_, z_;
    Local2WorldXYZ(dLon, dLat, dAlt, x, y, z, x_, y_, z_);
    XYZ2LonLat(x_, y_, z_, dLon1, dLat1, dAlt1);
}

void CoordinateTranfer::Local2World(double dLon, double dLat, double dAlt, int nPtNum, double xN[], double yE[], double zUp[], double lon[], double lat[], double alt[])
{
    double m[3][3];
    CreateReverseMatrix(dLon, dLat, m);

    double xo, yo, zo, x_, y_, z_;
    LonLat2XYZ(dLon, dLat, dAlt, xo, yo, zo);

    for (int i = 0; i < nPtNum; ++ i)
    {
        x_ = m[0][0] * xN[i] + m[0][1] * yE[i] + m[0][2] * zUp[i];
        y_ = m[1][0] * xN[i] + m[1][1] * yE[i] + m[1][2] * zUp[i];
        z_ = m[2][0] * xN[i] + m[2][1] * yE[i] + m[2][2] * zUp[i];

        x_ += xo;
        y_ += yo;
        z_ += zo;

        XYZ2LonLat(x_, y_, z_, lon[i], lat[i], alt[i]);
    }
}

void CoordinateTranfer::CreateReverseMatrix(double dLon, double dLat, double matrix[3][3])
{
    double B = dLat;
    double L = dLon >= 0 ? dLon : 360.0 + dLon;
    B = B * PI / 180.0;
    L = L * PI / 180.0;

    matrix[0][0] = -sin(B) * cos(L);
    matrix[1][0] = -sin(B) * sin(L);
    matrix[2][0] = cos(B);

    matrix[0][1] = -sin(L);
    matrix[1][1] = cos(L);
    matrix[2][1] = 0;

    matrix[0][2] = cos(B) * cos(L);
    matrix[1][2] = cos(B) * sin(L);
    matrix[2][2] = sin(B);
}

void CoordinateTranfer::WorldXYZ2Local(double dLon, double dLat, double dAlt, double xp, double yp, double zp, double &x, double &y, double &z)
{
    double m[3][3];
    CreateTransferMatrix(dLon, dLat, m);

    double xo, yo, zo;
    LonLat2XYZ(dLon, dLat, dAlt, xo, yo, zo);

    double x_ = xp - xo;
    double y_ = yp - yo;
    double z_ = zp - zo;

    x = m[0][0] * x_ + m[0][1] * y_ + m[0][2] * z_;
    y = m[1][0] * x_ + m[1][1] * y_ + m[1][2] * z_;
    z = m[2][0] * x_ + m[2][1] * y_ + m[2][2] * z_;
}

void CoordinateTranfer::WorldXYZ2Local(double dLon, double dLat, double dAlt, int nPtNum, double x[], double y[], double z[], double xN[], double yE[], double zUp[])
{
    double m[3][3];
    CreateTransferMatrix(dLon, dLat, m);

    double xo, yo, zo;
    LonLat2XYZ(dLon, dLat, dAlt, xo, yo, zo);

    double x_, y_, z_;
    for (int i = 0; i < nPtNum; ++ i)
    {
        x_ = x[i] - xo;
        y_ = y[i] - yo;
        z_ = z[i] - zo;

        xN[i] = m[0][0] * x_ + m[0][1] * y_ + m[0][2] * z_;
        yE[i] = m[1][0] * x_ + m[1][1] * y_ + m[1][2] * z_;
        zUp[i] = m[2][0] * x_ + m[2][1] * y_ + m[2][2] * z_;
    }
}

void CoordinateTranfer::Local2WorldXYZ(double dLon, double dLat, double dAlt, double x, double y, double z, double &x_, double &y_, double &z_)
{
    double m[3][3];
    CreateReverseMatrix(dLon, dLat, m);

    double xo, yo, zo;
    LonLat2XYZ(dLon, dLat, dAlt, xo, yo, zo);

    x_ = m[0][0] * x + m[0][1] * y + m[0][2] * z;
    y_ = m[1][0] * x + m[1][1] * y + m[1][2] * z;
    z_ = m[2][0] * x + m[2][1] * y + m[2][2] * z;

    x_ += xo;
    y_ += yo;
    z_ += zo;
}

void CoordinateTranfer::Local2Local(double dLon, double dLat, double dAlt, double dLon1, double dLat1, double dAlt1, double x1, double y1, double z1, double &x, double &y, double &z)
{
    double x_, y_, z_;
    Local2WorldXYZ(dLon1, dLat1, dAlt1, x1, y1, z1, x_, y_, z_);

    WorldXYZ2Local(dLon, dLat, dAlt, x_, y_, z_, x, y, z);
}

void CoordinateTranfer::LonLat2Polar(double dLon, double dLat, double dAlt, double dLon1, double dLat1, double dAlt1, double &b, double &d, double &h)
{
    double x, y, z;
    World2Local(dLon, dLat, dAlt, dLon1, dLat1, dAlt1, x, y, z);

    LocalXYZ2Polar(x, y, z, b, d, h);
}

#define	EARTH_R	6371004

double CoordinateTranfer::Local_Alt2Z(double altStation, double x, double y, double alt)
{
    return - (EARTH_R + altStation) + sqrt(pow(EARTH_R + alt, 2) - (x * x + y * y));
}

double CoordinateTranfer::Local_Z2Alt(double altStation, double x, double y, double z)
{
    double d = sqrt(x * x + y * y + z * z);
    double d1 = sqrt(x * x + y * y);
    if (d1 < 0.00001)
    {
        return 0;
    }

    double e = atan(z / d1);

    e = e + PI / 2.0f;
    return sqrt((EARTH_R + altStation) * (EARTH_R + altStation) + d * d - 2 * (EARTH_R + altStation) * d * cos(e)) - EARTH_R;
}

void CoordinateTranfer::LocalXYZ2Polar(double x, double y, double z, double &b, double &d, double &h)
{
    b = DFM::Direction(x, y);
    d = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
    h = z;
}

void CoordinateTranfer::Polar2LocalXYZ(double b, double d, double h, double &xN, double &yE, double &zUp)
{
    if (b < 0)
    {
        b += 360;
    }

    int nSpace = b / 90 + 1;
    b = b / 180.0 * PI;

    double hd = sqrt(d * d - h * h);
    switch (nSpace)
    {
    case 1:
        xN = cos(b) * hd;
        yE = sin(b) * hd;
        break;
    case 2:
        xN = -sin(b - PI * 0.5) * hd;
        yE = cos(b - PI * 0.5) * hd;
        break;
    case 3:
        xN = -cos(b - PI) * hd;
        yE = -sin(b - PI) * hd;
        break;
    case 4:
        xN = sin(b - PI * 1.5) * hd;
        yE = -cos(b - PI * 1.5) * hd;
        break;
    default:
        break;
    }

    zUp = h;
}

void CoordinateTranfer::Vector_World2Local(double dLon, double dLat, double x_, double y_, double z_, double &xN, double &yE, double &zUp)
{
    double m[3][3];
    CoordinateTranfer::CreateTransferMatrix(dLon, dLat, m);

    xN = m[0][0] * x_ + m[0][1] * y_ + m[0][2] * z_;
    yE = m[1][0] * x_ + m[1][1] * y_ + m[1][2] * z_;
    zUp = m[2][0] * x_ + m[2][1] * y_ + m[2][2] * z_;
}

void CoordinateTranfer::Vector_Local2World(double dLon, double dLat, double xN, double yE, double zUp, double &x, double &y, double &z)
{
    double m[3][3];
    CoordinateTranfer::CreateReverseMatrix(dLon, dLat, m);

    x = m[0][0] * xN + m[0][1] * yE + m[0][2] * zUp;
    y = m[1][0] * xN + m[1][1] * yE + m[1][2] * zUp;
    z = m[2][0] * xN + m[2][1] * yE + m[2][2] * zUp;
}

void CoordinateTranfer::Vector_Local2Local(double dLon, double dLat, double dLon1, double dLat1, double x1, double y1, double z1, double &x, double &y, double &z)
{
    double x_, y_, z_;
    Vector_Local2World(dLon1, dLat1, x1, y1, z1, x_, y_, z_);

    Vector_World2Local(dLon, dLat, x_, y_, z_, x, y, z);
}

void CoordinateTranfer::j_z(double dLon, double dLat, double dAlt, double b, double d, double alt, double &x, double &y, double &z)
{
    double vv0, vv1, vv2;
    double ff;

    vv0 = EARTH_R + dAlt;
    vv1 = EARTH_R + alt;
    vv2 = d;

    double vv = vv0 * vv0 + vv2 * vv2 - vv1 * vv1;
    vv1 = 2.0f * vv0 * vv2;
    ff = acos(vv / vv1) - PI / 2.0f;

    z = d * sin(ff);

    Polar2LocalXYZ(b, d, z, x, y, z);
}

void CoordinateTranfer::z_j(double dLon, double dLat, double dAlt, double x, double y, double z, double &b, double &d, double &alt)
{
    LocalXYZ2Polar(x, y, z, b, d, alt);

    alt = Local_Z2Alt(dAlt, x, y, z);
}

void CoordinateTranfer::LocalXYAlt2LonLat(double dLon, double dLat, double dAlt, double x, double y, double alt_, double &lon_, double &lat_)
{
    double z = Local_Alt2Z(dAlt, x, y, alt_);
    Local2World(dLon, dLat, dAlt, x, y, z, lon_, lat_, alt_);
}

double DFM::Direction(double x, double y)
{
    double b = 0.0;
    if (x == 0.0)
    {
        b = y >= 0.0 ? PI * 0.5 : PI * 1.5;
    }
    else
    {
        b = atan(fabs(y / x));

        if (x < 0 && y > 0)
        {
            b = PI - b;
        }
        else if (x < 0 && y < 0)
        {
            b = PI + b;
        }
        else if (x > 0 && y < 0)
        {
            b = 2 * PI - b;
        }
    }

    return b * 180.0 / PI;
}

double DFM::Distance(double dLon, double dLat, double dAlt, double dLon1, double dLat1, double dAlt1)
{
    double x, y, z, x1, y1, z1;
    CoordinateTranfer::LonLat2XYZ(dLon, dLat, dAlt, x, y, z);
    CoordinateTranfer::LonLat2XYZ(dLon1, dLat1, dAlt1, x1, y1, z1);

    return sqrt(pow(x - x1, 2) + pow(y - y1, 2) + pow(z - z1, 2));
}

double DFM::Direction(double dLon, double dLat, double dAlt, double dLon1, double dLat1, double dAlt1)
{
    double x, y, z;
    CoordinateTranfer::World2Local(dLon, dLat, dAlt, dLon1, dLat1, dAlt1, x, y, z);

    return Direction(x, y);
}

double DFM::HorizonDistance(double dLon, double dLat, double dAlt, double dLon1, double dLat1, double dAlt1)
{
    double x, y, z;
    CoordinateTranfer::World2Local(dLon, dLat, dAlt, dLon1, dLat1, dAlt1, x, y, z);

    return sqrt(pow(x, 2) + pow(y, 2));
}

double get_distance_from_lat_lon(double lat1, double lon1, double lat2, double lon2)
{

    double lat1Rad = lat1 * PI / 180.0;
    double lon1Rad = lon1 * PI / 180.0;
    double lat2Rad = lat2 * PI / 180.0;
    double lon2Rad = lon2 * PI / 180.0;
    double radius = _earth_r_a;
    double dLat = (lat2Rad - lat1Rad);
    double dLon = (lon2Rad - lon1Rad);
    double a = sin(dLat / 2.0) * sin(dLat / 2.0) +
               cos(lat1Rad) *  cos(lat2Rad) *
               sin(dLon / 2.0) * sin(dLon / 2.0);
    double c = 2.0 * atan2(sqrt(a), sqrt(1.0 - a));
    return (radius * c);
}
