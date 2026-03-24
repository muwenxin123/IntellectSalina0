#ifndef MEASURE_H
#define MEASURE_H

#include "IInterface.h"
#include <QPointF>
#include <QPainter>
#include <QCString.h>

class QMouseEvent;
class IProj;
class IUserMesh1;
class IUserMesh2;

class Measure
{
public:
    enum MeasureType { MT_DISTANCE, MT_MULTIDISTANCE, MT_AREA, MT_INTERVISIBILITY, MT_BIINTERVISIBILITY, MT_VIEWSHED };
    static Measure &Get();

    void SetType(MeasureType type);
    void SetIProj(IProj *pIProj);
    bool DrawDynamic(QPainter &painter, IProj *pProj);
    bool OnCreate(int wParam, QWidget *p);
    bool OnMouseButtonPress(QMouseEvent *e);
    bool OnMouseButtonRelease(QMouseEvent *e);
    bool OnMouseButtonMove(QMouseEvent *e);
    bool OnMouseButtonDblClick(QMouseEvent *e);
    bool OnMouseUp(const QPointF &pt);
    void OnLButtonDown(const QPointF &pt);
    bool OnMouseMove(const QPointF &pt);
    bool OnCommand(int nID);
    bool On2d3dChanged(int wParam);
    void OnEarthMessage(int wParam, void *lParam);

    inline const std::string GetDataDir() const
    {
        return m_strDataDir;
    };
    inline void SetDataDir(const char *lpszDir)
    {
        m_strDataDir = lpszDir;
    };

private:
    Measure();
    double TransformAngle(double angle) const;
    void DrawMeasureDistance(QPainter &painter, IProj *pProj);
    void DrawMeasureMultiDistance(QPainter &painter, IProj *pProj);
    void DrawMeasureArea(QPainter &painter, IProj *pProj);
    void DrawMeasureIntervisibility(QPainter &painter, IProj *pProj);
    void DrawMeasureViewShed(QPainter &painter, IProj *pProj);
    bool SetState(int type);
    int GetMouseState();
    void SetMouseState(int state);
    bool CheckValidMouseState();
    void Reset();

private:
    IProj		*m_pProj;
    SHPObject	*m_pShp[2];
    IUserMesh1	*m_pUserMesh[2];
    IUserMesh2	*m_pViewShed;
    QWidget		*m_pMapWidget;
    MeasureType m_type;

    unsigned char m_nMouseClick;
    QLineF      m_Line;
    QPolygonF   m_Area;
    std::string	m_strDataDir;
};

#endif
