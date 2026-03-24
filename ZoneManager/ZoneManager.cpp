#include "ZoneManager.h"
#include "ZoneManagerInterface.h"
#include "LyMessageBox.h"
#include "typedefine.h"
#include "qyzonedlg.h"
#include "IProj.h"
#include "AcmGeometry.h"
#include "shapefil.h"
#include "DrawShape.h"
#include "CreateShape.h"
#include <map3d/IEarth.h>
#include <map3d/EarthEvent.h>
#include <map3d/IGraphics.h>
#include <math.h>

#include <QDateTime>
#include <QPainter>
#include <QLineF>
#include <QList>
#include <QMouseEvent>
#include <QString>
#include <QSettings>
#include <QWidget>

ZoneManager &ZoneManager::Get()
{
    static ZoneManager ins;
    return ins;
}

QStringList g_listAirLineType = { tr2("途中点"), tr2("初始点"), tr2("发射点"), tr2("回转点"), tr2("退出点"), tr2("联络点"), tr2("导航点"), tr2("加油会合点"), tr2("编队会合点"), tr2("解散点"), tr2("掩护起始点"), tr2("掩护结束点"), tr2("区域进入点"), tr2("区域退出点"), tr2("区域活动点"), tr2("协同点"), tr2("控制点") };

ZoneManager::ZoneManager()
{
    m_pProj = nullptr;
    m_pQYZoneDlg = nullptr;
    m_nAirlineSn = -1;
    m_arrowType = 2;
    m_nModifyZone = 0;
    m_mkind = -1;
    m_bkind = -1;
    m_oriX = m_oriY = m_pyx = m_pyy = 0;
    b_oriX = b_oriY = b_pyx = b_pyy = 0;
    m_bJudgelandmark = false;
}

ZoneManager::~ZoneManager()
{
}

bool ZoneManager::DrawGraph(QPainter &painter, IProj *pProj)
{
    return true;
}

bool ZoneManager::DrawDynamic(QPainter &painter, IProj *pProj)
{
    if (m_pQYZoneDlg != nullptr)
    {
        m_pQYZoneDlg->DrawQYZoneData(&painter);
    }
    return true;
}

void ZoneManager::OnMouseButtonDown(QMouseEvent *e)
{
    QPoint pt1 = e->pos();
    double x = 0, y = 0;
    double jd = 0, wd = 0;
    m_pProj->xyDtoL(pt1.x(), pt1.y(), &x, &y);
    m_pProj->xyjw(x, y, &jd, &wd);
    ShowXinXi(jd, wd);
    if (e->button() == Qt::LeftButton)
    {
        if (m_pQYZoneDlg && m_pQYZoneDlg->isVisible() && m_pQYZoneDlg->m_bCreateFlag)
        {
            m_pQYZoneDlg->insertJWDpt(jd, wd, true);
        }

        if (m_pQYZoneDlg && m_pQYZoneDlg->m_bModifyFlag && (QApplication::keyboardModifiers() != Qt::ShiftModifier))
        {
            m_pQYZoneDlg->m_iCurSelDotIdx = IfOnQYPot(jd, wd, m_pQYZoneDlg->m_QYZoneTree->m_ZoneArray[m_pQYZoneDlg->m_QYZoneTree->m_CurInd]);
            if (m_pQYZoneDlg->m_iCurSelDotIdx != -1)
            {
                m_nModifyZone = 1;
                if (m_pQYZoneDlg->m_QYZoneTree->m_ZoneArray[m_pQYZoneDlg->m_QYZoneTree->m_CurInd].type == 4)
                {
                    m_ZOriLong = jd;
                    m_ZOriLat = wd;
                }
            }
            if (IfOnQYLine(jd, wd, m_pQYZoneDlg->m_QYZoneTree->m_ZoneArray[m_pQYZoneDlg->m_QYZoneTree->m_CurInd]) != -1)
            {
                m_nModifyZone = 2;
                m_ZOriLong = jd;
                m_ZOriLat = wd;

            }
        }
    }
    else if (e->button() == Qt::RightButton)
    {
        if (m_pQYZoneDlg && m_pQYZoneDlg->isVisible() && m_pQYZoneDlg->m_bModifyFlag == true)
        {
            m_pQYZoneDlg->insertJWDpt(jd, wd);
        }
    }
}

void ZoneManager::ShowXinXi(double jd, double wd)
{

}

void ZoneManager::OnMouseButtonRelease(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
    {

        m_oriX = m_oriY = m_pyx = m_pyy = 0;
        m_mkind = -1;
        b_oriX = b_oriY = b_pyx = b_pyy = 0;
        m_bkind = -1;
        m_nModifyZone = 0;
        m_ZOriLat = m_ZOriLong = 0;

    }
}

void ZoneManager::OnMouseMove(QMouseEvent *e)
{
    bool bReturn = false;
    if (m_pProj == nullptr)
    {
        return;
    }
    QPoint pt1 = e->pos();
    double x = 0, y = 0;
    double jd = 0, wd = 0;
    m_pProj->xyDtoL(pt1.x(), pt1.y(), &x, &y);
    m_pProj->xyjw(x, y, &jd, &wd);

    if (m_pQYZoneDlg && m_pQYZoneDlg->isVisible() && m_pQYZoneDlg->m_bCreateFlag == true)
    {
        m_pQYZoneDlg->InsertMouseMoveJWD(jd, wd);
        return;
    }

    if ((e->buttons()&Qt::LeftButton))
    {

        if ((m_pQYZoneDlg != nullptr) && m_pQYZoneDlg->m_bModifyFlag && m_pQYZoneDlg->isVisible())
        {
            QUYU_Zone zone;
            QString strjd, strwd;
            int jj, ww;
            jj = jd * 3600;
            ww = wd * 3600;
            strjd = QStringLiteral("%1.%2'%3\"").arg((int)jj / 3600, 3, 10, QChar('0')).arg((int)jj % 3600 / 60, 2, 10, QChar('0')).arg((int)jj % 3600 % 60, 2, 10, QChar('0'));
            strwd = QStringLiteral("%1.%2'%3\"").arg((int)ww / 3600, 2, 10, QChar('0')).arg((int)ww % 3600 / 60, 2, 10, QChar('0')).arg((int)ww % 3600 % 60, 2, 10, QChar('0'));
            zone = m_pQYZoneDlg->m_QYZoneTree->m_ZoneArray[m_pQYZoneDlg->m_QYZoneTree->m_CurInd];
            if (m_nModifyZone == 1)
            {
                if (zone.type == 3 || zone.type == 4 || zone.type == 8 || zone.type == 7)
                {
                    zone.data.h[m_pQYZoneDlg->m_iCurSelDotIdx].x = jd;
                    zone.data.h[m_pQYZoneDlg->m_iCurSelDotIdx].y = wd;
                    m_pQYZoneDlg->m_QYZoneTree->m_ZoneArray[m_pQYZoneDlg->m_QYZoneTree->m_CurInd] = zone;
                    if (zone.type == 4)
                    {
                        double difX = jd - m_ZOriLong;
                        double difY = wd - m_ZOriLat;
                        m_pQYZoneDlg->m_QYZoneTree->m_ZoneArray[m_pQYZoneDlg->m_QYZoneTree->m_CurInd] = zone;
                        m_pQYZoneDlg->WriteKyList(zone, false);
                        m_ZOriLong = jd;
                        m_ZOriLat = wd;
                    }
                    else
                    {
                        m_pQYZoneDlg->WriteKyList(zone, false);
                    }
                }
            }

            if (m_nModifyZone == 2 && fabs(jd - m_ZOriLong) > 0.00001 && fabs(wd - m_ZOriLat) > 0.00001)
            {
                double difX = jd - m_ZOriLong;
                double difY = wd - m_ZOriLat;
                if (zone.type == 3 || zone.type == 8)
                {
                    for (int f = 0; f < zone.data.n; f++)
                    {
                        zone.data.h[f].x = zone.data.h[f].x + difX;
                        zone.data.h[f].y = zone.data.h[f].y + difY;
                    }
                    m_pQYZoneDlg->m_QYZoneTree->m_ZoneArray[m_pQYZoneDlg->m_QYZoneTree->m_CurInd] = zone;
                    m_pQYZoneDlg->WriteKyList(zone, false);
                    m_ZOriLong = jd;
                    m_ZOriLat = wd;
                }
                if (zone.type == 0 || zone.type == 5 || zone.type == 1 || zone.type == 6 || zone.type == 4)
                {
                    zone.data.h[0].x = zone.data.h[0].x + difX;
                    zone.data.h[0].y = zone.data.h[0].y + difY;
                    m_pQYZoneDlg->m_QYZoneTree->m_ZoneArray[m_pQYZoneDlg->m_QYZoneTree->m_CurInd] = zone;
                    m_pQYZoneDlg->WriteKyList(zone, false);
                    m_ZOriLong = jd;
                    m_ZOriLat = wd;
                }
            }
        }
    }

    if (m_pQYZoneDlg && m_pQYZoneDlg->m_bModifyFlag && !m_pQYZoneDlg->m_bCreateFlag)
    {
        if (IfOnQYLine(jd, wd, m_pQYZoneDlg->m_QYZoneTree->m_ZoneArray[m_pQYZoneDlg->m_QYZoneTree->m_CurInd]) != -1)

        {
            QApplication::setOverrideCursor(QCursor(QPixmap(":/Routeplugin/resources/curpick.cur"), 0, 0));
        }
        else if (IfOnQYPot(jd, wd, m_pQYZoneDlg->m_QYZoneTree->m_ZoneArray[m_pQYZoneDlg->m_QYZoneTree->m_CurInd]) != -1)

        {
            QApplication::setOverrideCursor(QCursor(QPixmap(":/Routeplugin/resources/arrowcop.cur"), 0, 0));
        }
        else
        {
            QApplication::setOverrideCursor(Qt::ArrowCursor);
        }
    }
    return;
}

bool ZoneManager::OnCommand(int nID)
{
    switch (nID)
    {
    case 33385:
        {
            if (m_pQYZoneDlg == nullptr)
            {
                m_pQYZoneDlg = new CQYZoneDlg(m_pWnd);
            }
            if (m_pQYZoneDlg->isVisible())
            {
                m_pQYZoneDlg->close();
            }
            else
            {
                m_pQYZoneDlg->show();
            }
        }
        break;
    default:
        break;
    }
    return false;
}

bool ZoneManager::OnCreate(int wParam, QWidget *pTab)
{
    InitDbPara();
    switch (wParam)
    {
    case 0:
        {
            m_pWnd = pTab;
        }
        break;
    default:
        break;
    }
    return false;
}

bool ZoneManager::OnTimer(int wParam)
{
    switch (wParam)
    {
    case 1:
        ZoneManagerInterface::Get().Refresh(1);
        break;
    case 2:
        break;
    case 3:
        break;
    }
    return false;
}

double GetAngle(double dx, double dy)
{
    double dAngle = atan2(dx, dy) * 180 / PI;
    if (dAngle < 0)
    {
        dAngle += 360;
    }
    return dAngle;
}
void ZoneManager::DrawDotLine(QPainter *p, int px0, int py0, int px, int py, int px2, int py2, int nCLR)
{
    p->save();
    QPen mPen;
    unsigned int uiV = nCLR;
    QColor c = QColor(uiV & 0xFF, (uiV >> 8) & 0xFF, (uiV >> 16) & 0xFF);
    mPen.setColor(c);
    mPen.setWidth(1);
    mPen.setStyle(Qt::DotLine);
    p->setPen(mPen);
    p->drawLine(QLine(QPoint(px0, py0), QPoint(px, py)));
    p->drawLine(QLine(QPoint(px2, py2), QPoint(px, py)));
    p->restore();
}

void ZoneManager::DrawArc(QPainter *p, int cx, int cy, int sx, int sy, int ex, int ey, long r)
{
    double angleStart = atan2((double)(sy - cy),
        (double)(sx - cx)) * 180.0 / 3.1415926;
    angleStart = abs(angleStart);
    double angleEnd = atan2((double)(ey - cy),
        (double)(ex - cx)) * 180.0 / 3.1415926;
    angleEnd = abs(angleEnd);
    if (sy - cy >= 0 && sx - cx > 0)
    {
        angleStart = -angleStart;
    };
    if (sy - cy > 0 && sx - cx <= 0)
    {
        angleStart = -angleStart;
    }
    if (sy - cy <= 0 && sx - cx < 0)
    {
        angleStart = angleStart;
    }
    if (sy - cy < 0 && sx - cx >= 0)
    {
        angleStart = angleStart;
    }
    if (ey - cy >= 0 && ex - cx > 0)
    {
        angleEnd = -angleEnd;
    }
    if (ey - cy > 0 && ex - cx <= 0)
    {
        angleEnd = -angleEnd;
    }
    if (ey - cy <= 0 && ex - cx < 0)
    {
        angleEnd = angleEnd;
    }
    if (ey - cy < 0 && ex - cx >= 0)
    {
        angleEnd = angleEnd;
    }
    double sweepAngle = angleEnd - angleStart;

    if (sweepAngle > 0)
    {
        sweepAngle = 360 - sweepAngle;
    }
    else
    {
        sweepAngle = 0 - sweepAngle;
    }
    QRectF rectangle(QPointF(cx - r, cy - r), QPointF(cx + r, cy + r));
    p->drawArc(rectangle, angleStart * 16, (0 - sweepAngle) * 16);
}

void ZoneManager::DrawArrow(QPainter *painter, int x, int y, float k, int uiV, int style, int blc)
{
    painter->save();
    int	cRate = 1;

    QColor c = QColor(uiV & 0xFF, (uiV >> 8) & 0xFF, (uiV >> 16) & 0xFF);
    QBrush brush(c);
    float st = 2.0;
    float S = 4.0;
    switch (style)
    {
    case 0:
        st = 2.0;
        break;
    case 1:
        st = 3.5;
        break;
    case 2:
        st = 3.2;
        break;
    default:
        break;
    }
    st -= 1;
    float Beishu = 1;
    Beishu = 3000000 / (float)blc;
    double r = 0;
    if (Beishu < 1)
    {
        r = Beishu * cRate * st / 1.3;
    }
    else
    {
        r = cRate * st;
    }
    double deg, x1, y1;
    deg = k;
    QPoint p[] = { QPoint(-2, +6),  QPoint(+2, +6) };
    QPoint point;
    point.setX(x);
    point.setY(y);
    if (m_arrowType == 1)
    {
        QPoint aPoint[4];
        aPoint[0].setX(x);
        aPoint[0].setY(y);
        x1 = (p[0].x()) * cos(deg) - (p[0].y()) * sin(deg);
        y1 = (p[0].y()) * cos(deg) + (p[0].x()) * sin(deg);
        aPoint[1].setX(r * (int)(x1 + 10.5) - 10 * r + point.x());
        aPoint[1].setY(r * (int)(y1 + 10.5) - 10 * r + point.y());
        x1 = (p[1].x()) * cos(deg) - (p[1].y()) * sin(deg);
        y1 = (p[1].y()) * cos(deg) + (p[1].x()) * sin(deg);
        aPoint[2].setX(r * (int)(x1 + 10.5) - 10 * r + point.x());
        aPoint[2].setY(r * (int)(y1 + 10.5) - 10 * r + point.y());
        aPoint[3] = aPoint[0];
        painter->setBrush(brush);
        painter->drawPolygon(aPoint, 4);
    }
    else if (m_arrowType == 2)
    {
        QPoint aPoint[5];
        aPoint[0].setX(x);
        aPoint[0].setY(y);
        x1 = (p[0].x()) * cos(deg) - (p[0].y()) * sin(deg);
        y1 = (p[0].y()) * cos(deg) + (p[0].x()) * sin(deg);
        aPoint[3].setX(r * (int)(x1 + 10.5) - 10 * r + point.x());
        aPoint[3].setY(r * (int)(y1 + 10.5) - 10 * r + point.y());
        x1 = (p[1].x()) * cos(deg) - (p[1].y()) * sin(deg);
        y1 = (p[1].y()) * cos(deg) + (p[1].x()) * sin(deg);
        aPoint[1].setX(r * (int)(x1 + 10.5) - 10 * r + point.x());
        aPoint[1].setY(r * (int)(y1 + 10.5) - 10 * r + point.y());
        aPoint[2].setX(x - st * S * sin(deg));
        aPoint[2].setY(y + st * S * cos(deg));
        aPoint[4] = aPoint[0];
        painter->setBrush(brush);
        painter->drawPolygon(aPoint, 5);
    }
    else
    {
        QPainterPath path;
        for (int i = 0; i < 2; i++)
        {
            x1 = (p[i].x()) * cos(deg) - (p[i].y()) * sin(deg);
            y1 = (p[i].y()) * cos(deg) + (p[i].x()) * sin(deg);
            path.moveTo(r * (int)(x1 + 10.5) - 10 * r + point.x(), r * (int)(y1 + 10.5) - 10 * r + point.y());
            path.lineTo(point.x(), point.y());
        }
        painter->drawPath(path);
    }
    painter->restore();
}

QString ZoneManager::GetDotNamebyPoint(int sx, int sy, double *l, double *b)
{
    QString name = "";
    QStringList LayerStr;
    LayerStr << tr2("三级地标") << tr2("四级地标");
    int  BLCs[] = { 100000000, 100000000, 100000000, 100000000, 100000000, 100000000, 100000000, 100000000, 100000000, 100000000, 100000000, 100000000, 100000000, 2000000, 1000000 };
    int Bilichi = m_pProj->GetMapScale();
    for (int i = 0; i < 15; i++)
    {
        if (Bilichi < BLCs[i])
        {
            float jd = *l, wd = *b;
            name = CheckMapObjectByLayerName(sx, sy, LayerStr[i], &jd, &wd);
            if (name != "")
            {
                *l = jd;
                *b = wd;
                return name;
            }
        }
    }
    return name;
}

QString ZoneManager::CheckMapObjectByLayerName(int sx, int sy, const QString &layer, float *jd, float *wd)
{
    QString name = "";
    float zx, zy;
    double dx, dy;
    MapVecMO &mvMo = ZoneManagerInterface::Get().QueryObjectByPos(layer.toLocal8Bit().data(), sx, sy);
    for (MapVecMO::iterator it = mvMo.begin(); it != mvMo.end(); it++)
    {
        for (VecMO::iterator it1 = it->second.begin(); it1 != it->second.end(); it1++)
        {
            it1->GetPosition(zx, zy);
            m_pProj->xyjw(zx, zy, &dx, &dy);
            *jd = dx;
            *wd = dy;
            name = tr2(it1->GetName());
        }
    }
    return name;
}

QString ZoneManager::RtnDir_and_Dis_str(double jd1, double wd1, double jd2, double wd2, int kind )
{
    QString  str = "";
    if (m_pProj == nullptr)
    {
        return str;
    }
    double dDistance = 0;
    double dAngle = 0;
    m_pProj->L_A(jd1, wd1, jd2, wd2, dDistance, dAngle);

    if (kind == 0)
    {
        int Ly;
        Ly = (int)(dAngle + 0.5);
        dDistance = dDistance / 1000.0 + 0.5;
        str = QStringLiteral("X:%1 S:%2").arg(Ly, 3, 10, QChar('0')).arg((int)dDistance);
    }
    else
    {
        str = QStringLiteral("(%1--%2)").arg((int)(dAngle + 0.5)).arg((int)(dDistance / 1000.0 + 0.5));
    }
    return str;
}

double ZoneManager::GetCircleK(double x1, double y1, double x2, double y2)
{
    if (m_pProj == nullptr)
    {
        return 0;
    }
    double dDistance = 0;
    double dAngle = 0;
    m_pProj->X_A(x1, y1, x2, y2, dDistance, dAngle);
    return dAngle / 180 * PI;
}

int ZoneManager::IfInPels3(double sx, double sy, double zx, double zy)
{
    if (m_pProj == nullptr)
    {
        return 0;
    }
    int sx2 = 0, sy2;
    m_pProj->xyLtoD(zx, zy, &sx2, &sy2);
    if ((sx - sx2) * (sx - sx2) + (sy - sy2) * (sy - sy2) <= 9)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int ZoneManager::IfOnLine(double x, double y, double x1, double y1, double x2, double y2, int nPixel, bool bScreen)
{
    double a = y1 - y2;
    double b = x2 - x1;
    double c = x1 * y2 - x2 * y1;
    if (a == 0 && b == 0)
    {
        return false;
    }
    double aa = (x - x1) * (x - x1) + (y - y1) * (y - y1);
    double bb = (x - x2) * (x - x2) + (y - y2) * (y - y2);
    double cc = (x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1);
    if (cc + aa >= bb && cc + bb >= aa)
    {
        double L = -1.0 * (a * x + b * y + c) / sqrt(a * a + b * b);
        if (bScreen)
        {
            L = m_pProj->Screen2MapDistance(fabs(L));
        }
        double dDelta = nPixel == 0 ? 30 : m_pProj->Screen2MapDistance(nPixel);
        if (fabs(L) <= dDelta)
        {
            return true;
        }
    }
    return false;
}

int ZoneManager::IfOnQYPot(double jd, double wd, const QUYU_Zone &zone)
{
    if (m_pProj == nullptr)
    {
        return -1;
    }
    QPoint pt = m_pProj->Jwd2Screen(jd, wd);
    QPoint cpoint;
    for (int i = 0; i < zone.data.n; i++)
    {
        cpoint = m_pProj->Jwd2Screen(zone.data.h[i].x, zone.data.h[i].y);
        if ((pt.x() - cpoint.x()) * (pt.x() - cpoint.x()) + (pt.y() - cpoint.y()) * (pt.y() - cpoint.y()) <= 9)
        {
            return i;
        }
    }
    return -1;
}

int ZoneManager::IfOnQYLine(double jd, double wd, const QUYU_Zone &zone)
{
    if (m_pProj == nullptr)
    {
        return -1;
    }
    long   pms;
    QPoint  pt = m_pProj->Jwd2Screen(jd, wd);
    QPoint cpoint1, cpoint2;
    if (zone.type == 3 || zone.type == 8)
    {
        double dDistance = 0;
        double dAngle = 0;
        int val = 5;
        for (int i = 0; i < zone.data.n; i++)
        {
            cpoint1 = m_pProj->Jwd2Screen(zone.data.h[i].x, zone.data.h[i].y);
            if (i == zone.data.n - 1)
            {
                cpoint2 = m_pProj->Jwd2Screen(zone.data.h[0].x, zone.data.h[0].y);
                m_pProj->L_A(zone.data.h[i].x, zone.data.h[i].y, zone.data.h[0].x, zone.data.h[0].y, dDistance, dAngle);
            }
            else
            {
                cpoint2 = m_pProj->Jwd2Screen(zone.data.h[i + 1].x, zone.data.h[i + 1].y);
                m_pProj->L_A(zone.data.h[i].x, zone.data.h[i].y, zone.data.h[i + 1].x, zone.data.h[i + 1].y, dDistance, dAngle);
            }
            if ((abs(pt.x() - cpoint1.x()) < val && abs(pt.y() - cpoint1.y()) < val) || (abs(pt.x() - cpoint2.x()) < val && abs(pt.y() - cpoint2.y()) < val))
            {
                return -1;
            }
            if (IfOnLine(pt.x(), pt.y(), cpoint1.x(), cpoint1.y(), cpoint2.x(), cpoint2.y()))
            {
                return i;
            }
        }

    }
    if (zone.type == 0 || zone.type == 5)
    {
        double  r = (double)zone.data.h[2].x;
        pms = m_pProj->Map2ScreenDistance(r * 1000);
        long pms1 = m_pProj->Map2ScreenDistance(zone.data.h[2].x * 1000);
        double  s;
        double a;
        double b;
        cpoint1 = m_pProj->Jwd2Screen(zone.data.h[0].x, zone.data.h[0].y);
        a = pt.x() - cpoint1.x();
        b = pt.y() - cpoint1.y();
        s = sqrt(a * a + b * b);
        if (fabs(s - pms) < 2 || fabs(s - pms1) < 2)
        {
            return 1;
        }
    }
    if (zone.type == 1 || zone.type == 6)
    {
        double dDistance = 0;
        double dAngle = 0;
        double  r = (double)zone.data.h[2].x;
        double r1 = (double)zone.data.h[2].y;
        long dj;
        m_pProj->L_A(zone.data.h[0].x, zone.data.h[0].y, jd, wd, dDistance, dAngle);

        if (((zone.data.h[1].y - zone.data.h[1].x) >= 0 && (dAngle >= zone.data.h[1].x && dAngle <= zone.data.h[1].y))
            || ((zone.data.h[1].y - zone.data.h[1].x) < 0 && (dAngle + 360 >= zone.data.h[1].x && dAngle <= zone.data.h[1].y)))
        {
            dj = m_pProj->Map2ScreenDistance(dDistance);
            pms = m_pProj->Map2ScreenDistance(r * 1000);
            long pms1 = m_pProj->Map2ScreenDistance(r1 * 1000);
            if (abs(dj - pms) < 2 || abs(dj - pms1) < 2)
            {
                return 1;
            }
        }
    }
    if (zone.type == 4)
    {
        float l, b;
        double dis;
        double j1, j2, j3, j4;
        double dx1, dy1, dx2, dy2, dx3, dy3, dx4, dy4;
        double x1, y1;
        L_X(zone.data.h[0].x, zone.data.h[0].y, &x1, &y1);
        if (zone.data.h[2].y == 0)
        {
            dis = sqrt(zone.data.h[1].x * zone.data.h[1].x + zone.data.h[1].y * zone.data.h[1].y) * 1000 / 2;
            if (m_pProj != nullptr)
            {
                j1 = zone.data.h[2].x - atan(zone.data.h[1].y / zone.data.h[1].x) * 180 / PI;
                m_pProj->A_X(x1, y1, dis, j1, dx1, dy1);
                j2 = zone.data.h[2].x + atan(zone.data.h[1].y / zone.data.h[1].x) * 180 / PI;
                m_pProj->A_X(x1, y1, dis, j2, dx2, dy2);
                j3 = j1 + 180;
                m_pProj->A_X(x1, y1, dis, j3, dx3, dy3);
                j4 = j2 + 180;
                m_pProj->A_X(x1, y1, dis, j4, dx4, dy4);
            }
        }
        else
        {
            if (m_pProj != nullptr)
            {
                dis = zone.data.h[1].y * 1000 / 2;
                j1 = zone.data.h[2].x - 90;
                m_pProj->A_X(x1, y1, dis, j1, dx1, dy1);
                j4 = zone.data.h[2].x + 90;
                m_pProj->A_X(x1, y1, dis, j4, dx4, dy4);
                dis = zone.data.h[1].x * 1000;
                j2 = j3 = zone.data.h[2].x;
                m_pProj->A_X(dx1, dy1, dis, j2, dx2, dy2);
                m_pProj->A_X(dx4, dy4, dis, j3, dx3, dy3);
            }
        }
        QPoint zonept[4];
        X_S(dx1, dy1, &l, &b);
        zonept[0].setX(l);
        zonept[0].setY(b);
        X_S(dx2, dy2, &l, &b);
        zonept[1].setX(l);
        zonept[1].setY(b);
        X_S(dx3, dy3, &l, &b);
        zonept[2].setX(l);
        zonept[2].setY(b);
        X_S(dx4, dy4, &l, &b);
        zonept[3].setX(l);
        zonept[3].setY(b);
        for (int n = 0; n < 4; n++)
        {
            if (n == 3)
            {
                if (IfOnLine(pt.x(), pt.y(), zonept[n].x(), zonept[n].y(), zonept[0].x(), zonept[0].y()))
                {
                    return 1;
                }
            }
            else
            {
                if (IfOnLine(pt.x(), pt.y(), zonept[n].x(), zonept[n].y(), zonept[n + 1].x(), zonept[n + 1].y()))
                {
                    return 1;
                }
            }
        }
    }
    return -1;
}

double ZoneManager::GetLineDir(double x, double y)
{
    double dir;
    if (fabs(y) < 0.00001)
    {
        if (x < 0.)
        {
            dir = 3 * PI / 2;
        }
        else
        {
            dir = PI / 2;
        }
    }
    else
    {
        dir = atan2(x, y);
        if (dir < 0.)
        {
            dir += 2 * PI;
        }
    }
    return (dir);
}

void ZoneManager::ConversionPoint(double &x, double &y, double x1, double y1, double x2, double y2)
{
    double x0 = x, y0 = y;
    double a = y1 - y2;
    double b = x2 - x1;
    double c = x1 * y2 - x2 * y1;
    y = (a * a * y0 - b * c - a * b * x0) / (a * a + b * b);
    x = x0 - (a * a * x0 + a * b * y0 + a * c) / (a * a + b * b);
}

bool ZoneManager::IsbetweenPoint(double s, double e, double x, double &Angle)
{
    if (e < s)
    {
        if (x >= 0 && x <= e)
        {
            x += 360;
        }
        e += 360;
    }
    if (x >= s && x <= e)
    {
        Angle = x - s;
        return true;
    }
    else
    {
        return false;
    }
}

void ZoneManager::RefreshTrackLayer()
{
    ZoneManagerInterface::Get().Refresh(1);
}

void ZoneManager::InitDbPara()
{
    QString strIni = GetConfDir();
    strIni += "/ly.ini";
    QFile file(strIni);
    if (file.exists())
    {
        QSettings ini(strIni, QSettings::IniFormat);
        ini.beginGroup("Database");
        m_strDbName = ini.value("DbName", "LY").toString().toLatin1().data();
        m_strDbIP = ini.value("SvrIP", "192.168.1.1").toString().toLatin1().data();
        m_strDbUser = ini.value("User", "root").toString().toLatin1().data();
        m_strDbPass = ini.value("Password", "123456").toString().toLatin1().data();
        m_strDbDriver = ini.value("Driver", "QMYSQL").toString().toLatin1().data();
        ini.endGroup();
    }
}

bool ZoneManager::CheckNameStr(const QString &str, int len)
{
    if (str.length() == 0 || str.length() >= len || str.contains("\\") || str.contains(" ") || str.contains("/") || str.contains(":")
        || str.contains("*") || str.contains("?") || str.contains("\"") || str.contains("<") || str.contains(">") || str.contains("|"))
    {
        LyMessageBox::warning(nullptr, tr2("警告"), tr2("名称过长或名称中不能为空或含有 \\ / : * ? \" < > | 字符，请重新输入"));
        return false;
    }
    else
    {
        return true;
    }
}

QString ZoneManager::GetKTimeformat(int dqdkj)
{
    QString str, intstr;
    int vl, h, m, s;
    if (dqdkj > 0)
    {
        intstr = "K+";
    }
    else
    {
        intstr = "K-";
    }
    vl = abs(dqdkj);
    h = vl / 3600;
    m = vl % 3600 / 60;
    s = vl % 60;
    if (h > 0)
    {
        str = tr2("%1:%2′%3″").arg(h).arg(m).arg(s);
    }
    else
    {
        if (m > 0)
        {
            if (s > 0)
            {
                str = tr2("%1′%2″").arg(m).arg(s);
            }
            else
            {
                str = tr2("%1′").arg(m);
            }

        }
        else
        {
            str = tr2("%1″").arg(s);
        }
    }
    if (dqdkj != 0)
    {
        intstr = intstr + str;
    }
    else
    {
        intstr = "K";
    }

    return intstr;

}

double ZoneManager::StrToJWD(const QString &str)
{
    int du, fen, miao;
    sscanf(str.toLocal8Bit().data(), "%d.%d'%d\"", &du, &fen, &miao);
    return 1.0 * du + 1.0 / 60 * fen + 1.0 / 3600 * miao;
}

double ZoneManager::GetAngle(int x1, int y1, int x2, int y2)
{
    double angle = -1.0, dis = 0.0;
    if (m_pProj != nullptr)
    {
        m_pProj->X_A(x1, y1, x2, y2, dis, angle);
    }
    return angle;
}

double ZoneManager::GetDistance(int x1, int y1, int x2, int y2)
{
    double angle = -1.0, dis = 0.0;
    if (m_pProj != nullptr)
    {
        m_pProj->X_A(x1, y1, x2, y2, dis, angle);
    }
    return dis;
}

void ZoneManager::X_S(double x, double y, float *l, float *b)
{
    if (m_pProj != nullptr && x < 1000000000. && x > -1000000000.)
    {
        int px, py;
        m_pProj->xyLtoD(x, y, &px, &py);
        *l = (float)px;
        *b = (float)py;
    }
}

void ZoneManager::S_X(float x, float y, double *xx, double *yy)
{
    QPoint pt;
    pt.setX((int)x);
    pt.setY((int)y);
    if (m_pProj != nullptr)
    {
        m_pProj->Screen2Meter(pt, *xx, *yy);
    }
}

void ZoneManager::L_X(double l, double b, double *x, double *y)
{
    float x1 = 0, y1 = 0;
    if (m_pProj != nullptr)
    {
        m_pProj->jwxy(l, b, x, y);
    }
}

void ZoneManager::X_L(double x, double y, double *l, double *b)
{
    float l1 = 0, b1 = 0;
    if (m_pProj != nullptr)
    {
        m_pProj->xyjw(x, y, l, b);
    }
}

void ZoneManager::L_S(double x, double y, int *sx, int *sy)
{
    if (m_pProj == nullptr)
    {
        return;
    }
    double zx, zy;
    m_pProj->jwxy((float)x, (float)y, &zx, &zy);
    m_pProj->xyLtoD((float)zx, (float)zy, sx, sy);
}

void ZoneManager::S_L(int sx, int sy, double *x, double *y)
{
    QPoint pt;
    pt.setX((int)sx);
    pt.setY((int)sy);
    double x1 = 0, y1 = 0;
    if (m_pProj != nullptr)
    {
        m_pProj->Screen2Meter(pt, x1, y1);
        m_pProj->xyjw((float)x1, (float)y1, x, y);
    }
    else
    {
        return;
    }
}

bool ZoneManager::On2d3dChanged(int wParam)
{
	if (m_pQYZoneDlg != NULL)
	{
		m_pQYZoneDlg->On2d3dChanged(wParam);
	}
	return false;
}

int ZoneManager::GetMapScale()
{
    int nRet = 0;
    if (m_pProj != nullptr)
    {
        return m_pProj->GetMapScale();
    }
    return 0;
}

QString ZoneManager::GetSaveFileNameEx(const QString &filename, int type)
{
    QString saveDir = GetDataDir() + "export/";

    QString szPath = "doc";
    QString strTitle = filename + QDateTime::currentDateTime().toString("yyyy_MM_dd_HH_mm") + ".doc";

    QString str = "";
    if (type == 0)
    {
        str = tr2("航线输出");
    }
    else if (type == 1)
    {
        str = tr2("区域输出");
    }
    else if (type == 2)
    {
        str = tr2("地标输出");
    }
    else if (type == 3)
    {
        str = tr2("区域输出");
    }

    szPath = QFileDialog::getSaveFileName(nullptr, str,
        saveDir + strTitle, "(*.doc)|*.doc||");

    return szPath;
}

void ZoneManager::OnEarthMessage(int wParam, void *lParam)
{

}

