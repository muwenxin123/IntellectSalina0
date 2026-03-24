#ifndef ZONEADDITIONDRAWWIDGET_H
#define ZONEADDITIONDRAWWIDGET_H

#include <QButtonGroup>
#include "LyDialog.h"
#include "CDlgZoneParameterEdit.h"

namespace Ui {
class ZoneAdditionDrawWidget;
}

class ZoneAdditionDrawWidget : public LyDialog
{
    Q_OBJECT

public:
    explicit ZoneAdditionDrawWidget(QWidget *parent = nullptr);
    ~ZoneAdditionDrawWidget();

	void initUI();
	void DrawQYZoneData(QPainter *painter);
	void insertJWDpt(double ll, double bb, bool bEndflg = false);
	void InsertDotList(int index, const QString &str, const QString &strJD, const QString &strWD);
	qnzkna::ZoneManage::CZoneBase GetCurrentValue();
	void DrawOneQuYu(QPainter *pDC, qnzkna::ZoneManage::CZoneBase &zone, QPolygonF &zonePoly, QVector<QPolygonF> &dragPolyVec, bool bModify = false);
	void DrawFG(QPainter *pDC, int x1, int y1, int x2, int y2);
	void DrawKongYuCircle(QPainter *pDC, double x, double y, float radius, float startangle, float endangle, int type, int filltype, QPolygonF &zonePoly, QVector<QPolygonF> &dragPolyVec);
	void DrawKongYuPie(QPainter *pDC, double x, double y, float radius, float startangle, float endangle, int type, int filltype, QPolygonF &zonePoly, QVector<QPolygonF> &dragPolyVec);
	void DrawQYEllipse(QPainter *pDC, double x, double y, float fLRadius, float fSRadius, float fRotateAngle, int filltype, QPolygonF &zonePoly, QVector<QPolygonF> &dragPolyVec);
	void DrawKongYuRect(QPainter *pDC, qnzkna::ZoneManage::CZoneBase *pZone, double x1, double y1, QPolygonF &zonePoly, QVector<QPolygonF> &dragPolyVec, bool flag = 0);
	void DrawCircleRing(QPainter *pDC, double x, double y, qnzkna::ZoneManage::CZoneBase &zone, QPolygonF &zonePoly, QVector<QPolygonF> &dragPolyVec);
	void DrawArcRing(QPainter *pDC, float x, float y, qnzkna::ZoneManage::CZoneBase &zone, QPolygonF &zonePoly, QVector<QPolygonF> &dragPolyVec);
	void calcPoint(double x, double y, double l, double h, double angle, double type, double &centerx, double &centery, QVector<QPointF> &pts);
	void EditMouseMoveJWD(double ll, double bb);
	void EditMouseMoveRadius(int dragIndex, double ll, double bb);
	void isZoneSelected(bool isSelected);
	bool isNewNameValidate(const QString &name, char zname[]);
	void initZoneBase();

	void SetZonebase();
	CDlgZoneParameterEdit* getCDlgZoneParameterEdit();

public slots:
	void slotButtonChange(int index);

	void slot_ly_qymc_changed();

public:
	QPolygonF m_pZonePoly;
	QVector<QPolygonF> m_DragPolyVec;
	int m_pDragPolyRadius = 8;
	int m_iQYZoneType;

signals:
	void sig_cur_zone(qnzkna::ZoneManage::CZoneBase zone);

private:
    Ui::ZoneAdditionDrawWidget*						ui;

	QButtonGroup*									m_pButtonGroup = nullptr;
	bool											m_pZoneSelected = false;
	QPointF											m_pZoneCenter;
	QPointF											m_pShanStartPoint;
	QPointF											m_pShanEndPoint;

	QPointF											m_wStartPoint;
	QPointF											m_wEndPoint;
	QPointF											m_nStartPoint;
	QPointF											m_nEndPoint;

	double											m_OutRadius;
	double											m_inRadius;

	qnzkna::ZoneManage::CZoneBase					m_ZoneBase;
	CDlgZoneParameterEdit * m_pCDlgZoneParameterEdit = Q_NULLPTR;
};

#endif
