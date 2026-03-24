#include "Measure.h"
#include "IProj.h"
#include "AcmGeometry.h"
#include "MeasureInterface.h"
#include "shapefil.h"
#include "DrawShape.h"
#include "lyUI/LyMessageBox.h"
#include "CreateShape.h"
#include <map3d/IEarth.h>
#include <map3d/EarthEvent.h>
#include <map3d/IGraphics.h>
#include <math.h>

#include <QPainter>
#include <QMouseEvent>
#include <QLineF>
#include <QString>

#include "QIntervisibility.h"
#include "QViewShed.h"

Measure &Measure::Get()
{
    static Measure ins;
    return ins;
}

void Measure::SetType(MeasureType type)
{
    m_type = type;
    Reset();
}

void Measure::SetIProj(IProj *pIProj)
{
    m_pProj = pIProj;
}

bool Measure::DrawDynamic(QPainter &painter, IProj *pProj)
{
    switch (m_type)
    {
    case MT_DISTANCE:
    {
        DrawMeasureDistance(painter, pProj);
    }
    break;
    case MT_MULTIDISTANCE:
    {
        DrawMeasureMultiDistance(painter, pProj);
    }
    break;
    case MT_AREA:
    {
        DrawMeasureArea(painter, pProj);
    }
    break;
    case MT_INTERVISIBILITY:
    case MT_BIINTERVISIBILITY:
    {
        DrawMeasureIntervisibility(painter, pProj);
    }
    break;
    case MT_VIEWSHED:
    {
        DrawMeasureViewShed(painter, pProj);
    }
    break;
    default:
        break;
    }

    return true;
}

bool Measure::OnCreate(int wParam, QWidget *p)
{
    if (wParam == 0)
    {
        m_pMapWidget = p;
    }
    return false;
}

bool Measure::OnMouseButtonPress(QMouseEvent *e)
{
    if (!CheckValidMouseState())
    {
        return false;
    }

    QPointF pt;
    m_pProj->Screen2Jwd(e->pos(), pt.rx(), pt.ry());
    if (e->button() == Qt::LeftButton)
    {
        OnLButtonDown(pt);
    }
    else if (e->button() == Qt::RightButton)
    {
        if (m_nMouseClick > 1)
        {
            if (m_type == MT_AREA)
            {
                m_nMouseClick = 0;
                m_Area.back() = pt;
            }
        }

        if (m_nMouseClick > 0)
        {
            if (m_type == MT_MULTIDISTANCE)
            {
                m_nMouseClick = 0;
                m_Area.back() = pt;
            }
        }
    }

    return false;
}

void Measure::OnLButtonDown(const QPointF &pt)
{
    switch (m_type)
    {
    case MT_DISTANCE:
    case MT_INTERVISIBILITY:
    case MT_BIINTERVISIBILITY:
    {
        m_nMouseClick++;
        m_Line.setPoints(pt, pt);
    }
    break;
    case MT_MULTIDISTANCE:
    case MT_AREA:
    {
        m_nMouseClick++;
        if (m_nMouseClick == 1)
        {
            m_Area.clear();
            m_Area.push_back(pt);
        }
        m_Area.back() = pt;
        m_Area.push_back(pt);
    }
    break;
    default:
        break;
    }
}

bool Measure::OnMouseButtonRelease(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
    {
        QPointF pt;
        m_pProj->Screen2Jwd(e->pos(), pt.rx(), pt.ry());
        return OnMouseUp(pt);
    }
    return false;
}

bool Measure::OnMouseUp(const QPointF &pt)
{
    if (!CheckValidMouseState())
    {
        return false;
    }

    switch (m_type)
    {
    case Measure::MT_DISTANCE:
        m_nMouseClick = 0;
        break;
    case Measure::MT_INTERVISIBILITY:
    case Measure::MT_BIINTERVISIBILITY:
    {
        m_nMouseClick = 0;
        m_Line.setP2(pt);
        QIntervisibility dlg(m_type == MT_BIINTERVISIBILITY);
        dlg.setLonLat(m_Line);
        if (dlg.exec() == QDialog::Accepted)
        {
            for (int k = 0; k < 2; k++)
            {
                if (m_pShp[k] != NULL)
                {
                    SHPDestroyObject(m_pShp[k]);
                    m_pShp[k] = NULL;
                }
                SHPObject *pShp = dlg.m_pShp[k];
                if (pShp == NULL)
                {
                    continue;
                }
                for (int i = 0; i < pShp->nVertices; i++)
                {
                    m_pProj->jwxy(pShp->padfX[i], pShp->padfY[i], &pShp->padfX[i], &pShp->padfY[i]);
                }
                m_pShp[k] = SHPCreateObject(pShp->nSHPType, -1, pShp->nParts,
                                            pShp->panPartStart, pShp->panPartType, pShp->nVertices,
                                            pShp->padfX, pShp->padfY, pShp->padfZ,
                                            pShp->bMeasureIsUsed ? pShp->padfM : NULL);
                SHPDestroyObject(pShp);
            }
            if (IEarth::GetEarth()->IsVisible())
            {
                On2d3dChanged(1);
                MeasureInterface::Get().Script("ResetMapTool()");
            }
        }
        m_Line.setLength(0);
    }
    break;
    case Measure::MT_MULTIDISTANCE:
        break;
    case Measure::MT_AREA:
        break;
    case Measure::MT_VIEWSHED:
    {
        QViewShed dlg;
        dlg.setLonLat(pt.x(), pt.y());
        if (dlg.exec() == QDialog::Accepted)
        {
            double x, y;
            m_pProj->jwxy(pt.x(), pt.y(), &x, &y);

            for (int k = 0; k < 2; k++)
            {
                if (m_pShp[k] != NULL)
                {
                    SHPDestroyObject(m_pShp[k]);
                    m_pShp[k] = NULL;
                }
            }
            SHPObject *pShp = dlg.m_pShp;
            for (int i = 0; i < pShp->nVertices; i++)
            {
                m_pProj->jwxy(pShp->padfX[i], pShp->padfY[i], &pShp->padfX[i], &pShp->padfY[i]);
            }
            m_pShp[0] = SHPCreateObject(pShp->nSHPType, -1, pShp->nParts,
                                        pShp->panPartStart, pShp->panPartType, pShp->nVertices,
                                        pShp->padfX, pShp->padfY, pShp->padfZ, pShp->bMeasureIsUsed ? pShp->padfM : NULL);
            SHPDestroyObject(pShp);
            if (IEarth::GetEarth()->IsVisible())
            {
                On2d3dChanged(1);
                MeasureInterface::Get().Script("ResetMapTool()");
            }
        }
    }
    break;
    default:
        break;
    }

    return false;
}

bool Measure::OnMouseButtonMove(QMouseEvent *e)
{
    QPointF pt;
    m_pProj->Screen2Jwd(e->pos(), pt.rx(), pt.ry());
    return OnMouseMove(pt);
}

bool Measure::OnMouseMove(const QPointF &pt)
{
    if (!CheckValidMouseState())
    {
        return false;
    }

    if (m_nMouseClick)
    {
        switch (m_type)
        {
        case MT_DISTANCE:
        case MT_INTERVISIBILITY:
        case MT_BIINTERVISIBILITY:
        {
            m_Line.setP2(pt);
        }
        break;
        case MT_MULTIDISTANCE:
        case MT_AREA:
        {
			if (m_Area.size() == 0) return false;
            m_Area.back() = pt;
        }
        break;
        default:
            break;
        }
    }

    return false;
}

bool Measure::OnMouseButtonDblClick(QMouseEvent *e)
{
    if (CheckValidMouseState() && m_pMapWidget)
    {
        m_pMapWidget->setCursor(Qt::ArrowCursor);
    }
    if (e->button() == Qt::RightButton)
    {
        Reset();
    }
    return false;
}

bool Measure::OnCommand(int nID)
{
    switch (nID)
    {
    case 33394:
    {
        SetState(Measure::MT_DISTANCE);
        return true;
    }
    break;
    case 33395:
    {
        SetState(Measure::MT_AREA);
        return true;
    }
    break;
    case 33391:
    {
        SetState(Measure::MT_INTERVISIBILITY);
        return true;
    }
    break;
    case 33392:
    {
        SetState(Measure::MT_BIINTERVISIBILITY);
        return true;
    }
    break;
    case 33393:
    {
        SetState(Measure::MT_VIEWSHED);
        return true;
    }
    break;
    case 33396:
    {
        SetState(Measure::MT_MULTIDISTANCE);
        return true;
    }
    break;
    default:
        break;
    }
    return false;
}

bool Measure::On2d3dChanged(int wParam)
{
    IEarth *pEarth = IEarth::GetEarth();
    if (pEarth->IsVisible())
    {
        IGraphics *pGrp = pEarth->GetGraphics();
        for (int k = 0; k < 2; k++)
        {
            if (m_pShp[k] == NULL)
            {
                continue;
            }
            if (m_pUserMesh[k] != NULL)
            {
                pGrp->RemoveObject(m_pUserMesh[k]);
                m_pUserMesh[k] = NULL;
            }

            std::vector<double> vX(m_pShp[k]->nVertices);
            std::vector<double> vY(m_pShp[k]->nVertices);
            for (int i = 0; i < m_pShp[k]->nVertices; i++)
            {
                m_pProj->xyjw(m_pShp[k]->padfX[i], m_pShp[k]->padfY[i], &vX[i], &vY[i]);
            }
            SHPObject *pShp = SHPCreateObject(m_pShp[k]->nSHPType, -1, m_pShp[k]->nParts,
                                              m_pShp[k]->panPartStart, m_pShp[k]->panPartType, m_pShp[k]->nVertices,
                                              vX.data(), vY.data(), m_pShp[k]->padfZ,
                                              m_pShp[k]->bMeasureIsUsed ? m_pShp[k]->padfM : NULL);
            m_pUserMesh[k] = pGrp->CreateUserMesh1(pShp);
            m_pUserMesh[k]->SetColor(k == 0 ? RGBA(0, 255, 0, 255) : RGBA(255, 0, 0, 255));
            if (pShp->nSHPType > 20)
            {
                m_pUserMesh[k]->SetState(IUserMesh::POLYGONMODE, IUserMesh::USERMESH_LINE);
            }
            SHPDestroyObject(pShp);
        }
    }
    return false;
}

void Measure::OnEarthMessage(int wParam, void *lParam)
{
    if (wParam == IEarthEventHandler::Earth_RDOUBLECLICK)
    {
        Reset();
    }

    if (!CheckValidMouseState())
    {
        return;
    }

    pos3d *p3d = (pos3d *)lParam;
    QPointF pt(p3d->longitude, p3d->latitude);
    switch (wParam)
    {
    case IEarthEventHandler::Earth_LBUTTONDOWN:
    {
        OnLButtonDown(pt);
    }
    break;
    case IEarthEventHandler::Earth_LBUTTONUP:
    {
        OnMouseUp(pt);
    }
    break;
    case IEarthEventHandler::Earth_RBUTTONDOWN:
    {
        m_nMouseClick = 0;
    }
    break;
    case IEarthEventHandler::Earth_MOUSE_MOVE:
    {

    }
    break;
    default:
        break;
    }
}

Measure::Measure()
    : m_pProj(nullptr)
{
    m_nMouseClick = 0;
    for (int i = 0; i < 2; i++)
    {
        m_pUserMesh[i] = NULL;
        if (m_pShp[i] != NULL)
        {
            SHPDestroyObject(m_pShp[i]);
        }
        m_pShp[i] = NULL;
    }
    m_pViewShed = NULL;
}

double Measure::TransformAngle(double angle) const
{
    double targetAngle = 90 - angle;
    if (targetAngle < 0)
    {
        targetAngle += 360;
    }
    return targetAngle;
}

void Measure::DrawMeasureDistance(QPainter &painter, IProj *pProj)
{
    if (!pProj)
    {
        return;
    }

    QColor color(0, 0, 255, 250);
    if (!m_Line.isNull())
    {
        painter.save();
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setPen(color);

        QLine line;
        line.setP1(pProj->Jwd2Screen(m_Line.p1().x(), m_Line.p1().y()));
        line.setP2(pProj->Jwd2Screen(m_Line.p2().x(), m_Line.p2().y()));
        painter.drawLine(line);

        painter.setPen(color);
        double dA, dD;
        m_pProj->L_A(m_Line.p1().x(), m_Line.p1().y(), m_Line.p2().x(), m_Line.p2().y(), dD, dA);
        painter.drawText(line.p2().x() + 5, line.p2().y() - 5, QString::asprintf(tr2("%.2f°, %.2fm").toUtf8().data(), dA, dD));

        painter.restore();
    }
}

void Measure::DrawMeasureMultiDistance(QPainter &painter, IProj *pProj)
{
    if (!pProj || m_Area.isEmpty())
    {
        return;
    }

    QColor color(0, 0, 255, 65);
    painter.save();
    painter.setPen(color);
    painter.setBrush(color);

    if (m_Area.size() > 1)
    {
        double dA;
        double dS = 0;
        double dT = 0;
        int vcount = m_Area.count();

        QPolygon poly;
        for (int i = 0; i < vcount; ++i)
        {
            if (i > 0)
            {
                m_pProj->L_A(m_Area[i - 1].x(), m_Area[i - 1].y(), m_Area[i].x(), m_Area[i].y(), dS, dA);
                dS /= 1000;
            }
            dT += dS;
            poly.push_back(pProj->Jwd2Screen(m_Area[i].x(), m_Area[i].y()));
            painter.drawText(poly.back(), QString::asprintf("%.1f-%.1f", dS, dT));
        }

        painter.drawPolyline(poly);
    }
    painter.restore();
}

void Measure::DrawMeasureArea(QPainter &painter, IProj *pProj)
{
    if (!pProj || m_Area.isEmpty())
    {
        return;
    }

    QColor color(0, 0, 255, 65);
    painter.save();
    painter.setPen(color);
    painter.setBrush(color);

    QPoint p1, p2;
    if (m_Area.size() > 1)
    {
        double x, y;
        int vcount = m_Area.count();

        std::vector<DPOINT> polygon;
        QPolygon poly;
        for (int i = 0; i < vcount; ++i)
        {
            pProj->jwxy(m_Area[i].x(), m_Area[i].y(), &x, &y);
            DPOINT pt(x / 1000, y / 1000);
            polygon.push_back(pt);
            poly.push_back(pProj->Jwd2Screen(m_Area[i].x(), m_Area[i].y()));
        }

        double dArea = area_of_polygon(polygon.size(), polygon.data());
        color.setAlpha(150);
        painter.setPen(color);
        painter.drawPolygon(poly);
        painter.drawText(poly.back(), QString::number(fabs(dArea), 'f', 3));
    }
    painter.restore();
}

void Measure::DrawMeasureIntervisibility(QPainter &painter, IProj *pProj)
{
    DrawMeasureDistance(painter, pProj);
    CDrawShape ds;
    CDrawShape::Style style;
    if (m_pShp[0] != NULL)
    {
        style.clrPen = QColor(Qt::green);
        ds.Draw(m_pShp[0], painter, pProj, style);
    }
    if (m_pShp[1] != NULL)
    {
        style.clrPen = QColor(Qt::red);
        ds.Draw(m_pShp[1], painter, pProj, style);
    }
}

void Measure::DrawMeasureViewShed(QPainter &painter, IProj *pProj)
{
    if (m_pShp[0] != NULL)
    {
        CDrawShape ds;
        CDrawShape::Style style;
        style.clrFill = QColor(0, 0, 128, 32);
        style.clrPen = QColor(Qt::darkBlue);
        ds.Draw(m_pShp[0], painter, pProj, style);

    }
}

bool Measure::SetState(int type)
{
    int state = GetMouseState();
    if (state != IInterface::MouseStateNone && state != IInterface::MouseStateUser16 + type)
    {
        LyMessageBox::information(m_pMapWidget, tr2("鼠标"), tr2("鼠标不可用，请先结束当前鼠标操作！"));
        return false;
    }

    SetMouseState(IInterface::MouseStateUser16 + type);
    if (m_pMapWidget)
    {
        m_pMapWidget->setCursor(Qt::CrossCursor);
    }
    Measure::Get().SetType((Measure::MeasureType)type);
    IEarth *pEarth = IEarth::GetEarth();
    if (pEarth != NULL)
    {
        switch (type)
        {
        case Measure::MT_DISTANCE:
        case Measure::MT_INTERVISIBILITY:
        case Measure::MT_BIINTERVISIBILITY:
        {
            pEarth->GetUIManager()->EnableMeasureDistance();
        }
        break;
        case Measure::MT_MULTIDISTANCE:
        {
            pEarth->GetUIManager()->EnableMeasureMultiDistance();
        }
        break;
        case Measure::MT_AREA:
        {
            pEarth->GetUIManager()->EnableMeasureArea();
        }
        break;
        }
    }

    return true;
}

int Measure::GetMouseState()
{
    int state = *(int *)MeasureInterface::Get().Script("GetMouseState()");
    return state;
}

void Measure::SetMouseState(int state)
{
    int curState = GetMouseState();
    if (curState == IInterface::MouseStateNone || curState == IInterface::MouseStateUser16)
    {
        char szBuf[256];
        sprintf(szBuf, "SetMouseState(%d)", state);
        MeasureInterface::Get().Script(szBuf);
    }
}

bool Measure::CheckValidMouseState()
{
    int state = GetMouseState();
    if (state >= IInterface::MouseStateUser16 + Measure::MT_DISTANCE &&
            state <= IInterface::MouseStateUser16 + Measure::MT_VIEWSHED)
    {
        return true;
    }
    return false;
}

void Measure::Reset()
{
    m_Line.setLength(0);
    m_Area.clear();
    IEarth *pEarth = IEarth::GetEarth();
    IGraphics *pGrp = pEarth->GetGraphics();
    for (int k = 0; k < 2; k++)
    {
        if (m_pShp[k] != NULL)
        {
            SHPDestroyObject(m_pShp[k]);
            m_pShp[k] = NULL;
        }
        if (m_pUserMesh[k] != NULL)
        {
            pGrp->RemoveObject(m_pUserMesh[k]);
            m_pUserMesh[k] = NULL;
        }
    }
}
