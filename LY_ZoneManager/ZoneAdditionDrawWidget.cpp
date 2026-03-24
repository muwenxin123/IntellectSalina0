#include "zoneadditiondrawwidget.h"
#include "ui_ZoneAdditionDrawWidget.h"

#include <QStringList>
#include "QCString.h"
#include "genstructdef.h"
#include "LyMessageBox.h"
#include "LY_ZoneManager.h"
#include <QGeoCoordinate>
#include <QPainter>

#include "IZoneManagerActivator.h"

using namespace qnzkna::ZoneManage;

ZoneAdditionDrawWidget::ZoneAdditionDrawWidget(QWidget *parent) :
	LyDialog(tr2("区域/航路划设"), parent),
    ui(new Ui::ZoneAdditionDrawWidget)
{
    ui->setupUi(centralWidget());

	initUI();
}

ZoneAdditionDrawWidget::~ZoneAdditionDrawWidget()
{
    delete ui;
}

void ZoneAdditionDrawWidget::initUI()
{
	m_pCDlgZoneParameterEdit = new CDlgZoneParameterEdit(this);
	connect(m_pCDlgZoneParameterEdit, &CDlgZoneParameterEdit::sign_lyqymc_changed, this, &ZoneAdditionDrawWidget::slot_ly_qymc_changed);

	initZoneBase();

	m_pCDlgZoneParameterEdit->SetZonebase(m_ZoneBase, true);
	m_pCDlgZoneParameterEdit->setGroupHiden();
	ui->verticalLayout->addWidget(m_pCDlgZoneParameterEdit);

	m_pButtonGroup = new QButtonGroup(this);
	m_pButtonGroup->addButton(ui->pb_yuanxing, 1);
	m_pButtonGroup->addButton(ui->pb_shanxing, 2);
	m_pButtonGroup->addButton(ui->pb_tuoyuan, 3);
	m_pButtonGroup->addButton(ui->pb_duobianxing, 4);
	m_pButtonGroup->addButton(ui->pb_juxing, 5);
	m_pButtonGroup->addButton(ui->pb_yuanhuan, 6);
	m_pButtonGroup->addButton(ui->pb_shanhuan , 7);
	m_pButtonGroup->addButton(ui->pb_dian, 8);
	m_pButtonGroup->addButton(ui->pb_hanglu, 9);
	connect(m_pButtonGroup, SIGNAL(buttonClicked(int)), this, SLOT(slotButtonChange(int)));

	QPushButton* btYes = new QPushButton(tr2("确定"), this);
	connect(btYes, &QPushButton::clicked, this, [&]() {
		qnzkna::ZoneManage::CZoneBase zone = GetCurrentValue();
		zone.bShow = true;
		if (m_pCDlgZoneParameterEdit->isQYMCEmpty()) {
			LyMessageBox::warning(this, tr2("提示"), tr2("区域名称不能为空！"), QMessageBox::Yes);
			return;
		}

		emit sig_cur_zone(zone);
		IZoneManagerActivator::Get().getPIZoneManageService()->cancleADrawAble(m_pCDlgZoneParameterEdit->GetZonneBase().strID);
		this->close();
	});
	QPushButton* btCancel = new QPushButton(tr2("取消"), this);
	connect(btCancel, &QPushButton::clicked, this, [&]() {
		if (LyMessageBox::warning(this, tr2("提示"), tr2("确定取消自建区域？"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
			return;
		IZoneManagerActivator::Get().getPIZoneManageService()->cancleADrawAble(m_pCDlgZoneParameterEdit->GetZonneBase().strID);
		this->close();
	});

	QHBoxLayout* hlayout = new QHBoxLayout(this);
	hlayout->addWidget(btYes);
	hlayout->addWidget(btCancel);
	ui->verticalLayout->addLayout(hlayout);

	slotButtonChange(1);

	this->setFixedSize(800, 400);
}

void ZoneAdditionDrawWidget::DrawQYZoneData(QPainter *painter)
{
	painter->save();
	painter->setRenderHint(QPainter::Antialiasing, true);

	m_ZoneBase = GetCurrentValue();

	QPen m_penCur;
	m_penCur.setStyle((Qt::PenStyle)m_ZoneBase.nLineType);
	m_penCur.setWidth(2);

	unsigned int uiV = m_ZoneBase.nLineColor;
	QColor color = QColor(uiV & 0xFF, (uiV >> 8) & 0xFF, (uiV >> 16) & 0xFF);
	color.setAlpha(100);
	m_penCur.setColor(color);

	QBrush brush;
	brush.setStyle((Qt::BrushStyle)m_ZoneBase.nFillType);
	brush.setColor(color);
	painter->setBrush(brush);
	painter->setFont(QFont("MS Serif", 11, QFont::Bold));
	painter->setPen(m_penCur);

	bool modifyFlg = false;

	QPolygonF zonePoly;
	QVector<QPolygonF> dragPolyVec;
	DrawOneQuYu(painter, m_ZoneBase, zonePoly, dragPolyVec);
	m_pZonePoly = zonePoly;
	m_DragPolyVec.clear();
	m_DragPolyVec = dragPolyVec;
	painter->restore();
}

void ZoneAdditionDrawWidget::insertJWDpt(double ll, double bb, bool bEndflg )
{
	m_pCDlgZoneParameterEdit->EditMouseMoveJWD(ll, bb);
}

void ZoneAdditionDrawWidget::InsertDotList(int index, const QString &str, const QString &strJD, const QString &strWD)
{
	QUYU_Zone zone;

}

qnzkna::ZoneManage::CZoneBase ZoneAdditionDrawWidget::GetCurrentValue()
{
	return m_pCDlgZoneParameterEdit->GetZonneBase();
}

void ZoneAdditionDrawWidget::DrawOneQuYu(QPainter *painter, qnzkna::ZoneManage::CZoneBase &zone, QPolygonF &zonePoly, QVector<QPolygonF> &dragPolyVec, bool bModify )
{
	double x, y;
	QString str, jwdstr;
	float  l, b;
	double centerX = 0, centerY = 0;
	QVector<QPointF> m_spoints;
	m_spoints.clear();

	if (zone.nZoneType == CZoneBase::CIRCLE)
	{
		LY_ZoneManager::Get().L_X(zone.stRoundParameter.dLon, zone.stRoundParameter.dLat, &x, &y);
		LY_ZoneManager::Get().X_S(x, y, &l, &b);
		DrawKongYuCircle(painter, x, y, zone.stRoundParameter.nRadius, zone.stRoundParameter.nStartAngel, zone.stRoundParameter.nEndAngel, zone.nZoneType, zone.nFillType, zonePoly, dragPolyVec);
		centerX = l;
		centerY = b;
	}
	else if (zone.nZoneType == CZoneBase::PIE)
	{
		LY_ZoneManager::Get().L_X(zone.stRoundParameter.dLon, zone.stRoundParameter.dLat, &x, &y);
		LY_ZoneManager::Get().X_S(x, y, &l, &b);

		DrawKongYuPie(painter, x, y, zone.stRoundParameter.nRadius, zone.stRoundParameter.nStartAngel, zone.stRoundParameter.nEndAngel, zone.nZoneType, zone.nFillType, zonePoly, dragPolyVec);
		centerX = l;
		centerY = b;
	}
	else if (zone.nZoneType == CZoneBase::ELLIPSE)
	{
		LY_ZoneManager::Get().L_X(zone.stRoundParameter.dLon, zone.stRoundParameter.dLat, &x, &y);
		LY_ZoneManager::Get().X_S(x, y, &l, &b);

		DrawQYEllipse(painter, x, y, zone.stRoundParameter.nRaidusChang, zone.stRoundParameter.nRaidusDuan, zone.stRoundParameter.nAngelChang, zone.nFillType, zonePoly, dragPolyVec);
		centerX = l;
		centerY = b;
	}
	else if (zone.nZoneType == CZoneBase::CIRCLERING)
	{
		LY_ZoneManager::Get().L_X(zone.stRoundParameter.dLon, zone.stRoundParameter.dLat, &x, &y);
		LY_ZoneManager::Get().X_S(x, y, &l, &b);

		DrawCircleRing(painter, x, y, zone, zonePoly, dragPolyVec);

		centerX = l;
		centerY = b;
	}
	else if (zone.nZoneType == CZoneBase::PIERING)
	{
		LY_ZoneManager::Get().L_X(zone.stRoundParameter.dLon, zone.stRoundParameter.dLat, &x, &y);
		LY_ZoneManager::Get().X_S(x, y, &l, &b);

		DrawArcRing(painter, x, y, zone, zonePoly, dragPolyVec);

		centerX = l;
		centerY = b;
	}
	else if (zone.nZoneType == CZoneBase::POLY || zone.nZoneType == CZoneBase::LINE)
	{
		for (auto itrPoint : zone.stZonePointVec)
		{
			LY_ZoneManager::Get().L_X(itrPoint.dLon, itrPoint.dLat, &x, &y);
			LY_ZoneManager::Get().X_S(x, y, &l, &b);

			m_spoints.push_back(QPointF(l, b));

			centerX += l;
			centerY += b;
		}

		if (zone.nZoneType == CZoneBase::POLY)
		{
			painter->drawPolygon(m_spoints);
		}
		else if (zone.nZoneType == CZoneBase::LINE)
		{
			painter->drawPolyline(m_spoints);
		}
	}
	else if (zone.nZoneType == CZoneBase::RECTANGLE)
	{
		LY_ZoneManager::Get().L_X(zone.stRoundParameter.dLon, zone.stRoundParameter.dLat, &x, &y);
		LY_ZoneManager::Get().X_S(x, y, &l, &b);

		DrawKongYuRect(painter, &zone, x, y, zonePoly, dragPolyVec);

		centerX = l;
		centerY = b;
	}
	else if (zone.nZoneType == CZoneBase::POINT)
	{
		LY_ZoneManager::Get().L_X(zone.stRoundParameter.dLon, zone.stRoundParameter.dLat, &x, &y);
		LY_ZoneManager::Get().X_S(x, y, &l, &b);

		DrawFG(painter, l - 3, b - 3, l + 3, b + 3);

		centerX = l;
		centerY = b;
	}
	else if (zone.nZoneType == CZoneBase::POLY || zone.nZoneType == CZoneBase::LINE)
	{
		for (auto itrPoint : zone.stZonePointVec)
		{
			LY_ZoneManager::Get().L_X(itrPoint.dLon, itrPoint.dLat, &x, &y);
			LY_ZoneManager::Get().X_S(x, y, &l, &b);

			m_spoints.push_back(QPointF(l, b));

			centerX += l;
			centerY += b;
		}

		if (zone.nZoneType == CZoneBase::POLY)
		{
			painter->drawPolygon(m_spoints);
		}
		else if (zone.nZoneType == CZoneBase::LINE)
		{
			painter->drawPolyline(m_spoints);
		}
	}

	if (zone.nZoneType == CZoneBase::RECTANGLE && zone.stZonePointVec.size() > 0)
	{
		centerX = centerX / zone.stZonePointVec.size();
		centerY = centerY / zone.stZonePointVec.size();
	}

	DrawFG(painter, centerX - 3, centerY - 3, centerX + 3, centerY + 3);
}

void ZoneAdditionDrawWidget::DrawFG(QPainter *pDC, int x1, int y1, int x2, int y2)
{
	pDC->save();
	QPen pen;
	pen.setStyle(Qt::NoPen);
	QColor color(Qt::green);
	color.setAlpha(130);
	QBrush brush;
	brush.setStyle(Qt::SolidPattern);
	brush.setColor(color);
	pDC->setBrush(brush);
	pDC->setPen(pen);

	QRect rect(QPoint(x1, y1), QPoint(x2, y2));
	pDC->drawRect(rect);
	pDC->restore();
}

void ZoneAdditionDrawWidget::DrawKongYuCircle(QPainter *painter, double x, double y, float radius, float startangle, float endangle, int type, int filltype, QPolygonF &zonePoly, QVector<QPolygonF> &dragPolyVec)
{
	double yx, yy;
	yx = x + radius * sin(startangle * PI / 180.);
	yy = y + radius * cos(startangle * PI / 180.);

	float l, b, ll, bb;
	LY_ZoneManager::Get().X_S(x, y, &l, &b);
	LY_ZoneManager::Get().X_S(yx, yy, &ll, &bb);

	double m_fRadius = sqrt(float((ll - l) * (ll - l) + (bb - b) * (bb - b)));

	painter->drawEllipse(QPointF(l, b), m_fRadius, m_fRadius);

	if (m_pZoneSelected) {
		auto pen = painter->pen();
		pen.setColor(QColor(255, 255, 255));
		painter->setPen(pen);
		painter->drawEllipse(QPointF(l - m_fRadius, b), m_pDragPolyRadius, m_pDragPolyRadius);
		painter->drawEllipse(QPointF(l, b - m_fRadius), m_pDragPolyRadius, m_pDragPolyRadius);
		painter->drawEllipse(QPointF(l + m_fRadius, b), m_pDragPolyRadius, m_pDragPolyRadius);
		painter->drawEllipse(QPointF(l, b + m_fRadius), m_pDragPolyRadius, m_pDragPolyRadius);

		QRegion r1(l - m_fRadius - m_pDragPolyRadius, b - m_pDragPolyRadius, 2 * m_pDragPolyRadius, 2 * m_pDragPolyRadius, QRegion::RegionType::Ellipse);
		QRegion r2(l - m_pDragPolyRadius, b - m_fRadius - m_pDragPolyRadius, 2 * m_pDragPolyRadius, 2 * m_pDragPolyRadius, QRegion::RegionType::Ellipse);
		QRegion r3(l + m_fRadius - m_pDragPolyRadius, b - m_pDragPolyRadius, 2 * m_pDragPolyRadius, 2 * m_pDragPolyRadius, QRegion::RegionType::Ellipse);
		QRegion r4(l - m_pDragPolyRadius, b + m_fRadius - m_pDragPolyRadius, 2 * m_pDragPolyRadius, 2 * m_pDragPolyRadius, QRegion::RegionType::Ellipse);
		QPainterPath p1;
		p1.addRegion(r1);
		dragPolyVec.push_back(p1.toFillPolygon());
		QPainterPath p2;
		p2.addRegion(r2);
		dragPolyVec.push_back(p2.toFillPolygon());
		QPainterPath p3;
		p3.addRegion(r3);
		dragPolyVec.push_back(p3.toFillPolygon());
		QPainterPath p4;
		p4.addRegion(r4);
		dragPolyVec.push_back(p4.toFillPolygon());

	}

	QRegion region(l - m_fRadius, b - m_fRadius, 2 * m_fRadius, 2 * m_fRadius, QRegion::RegionType::Ellipse);
	QPainterPath path;
	path.addRegion(region);
	zonePoly = path.toFillPolygon();
}

void ZoneAdditionDrawWidget::DrawKongYuPie(QPainter *painter, double x, double y, float radius, float startangle, float endangle, int type, int filltype, QPolygonF &zonePoly, QVector<QPolygonF> &dragPolyVec)
{
	double yx, yy, xx, xy;
	yx = x + radius * sin(startangle * PI / 180.);
	yy = y + radius * cos(startangle * PI / 180.);
	xx = x + radius * sin(endangle * PI / 180.);
	xy = y + radius * cos(endangle * PI / 180.);

	float l, b, ll, bb, lll, bbb;
	LY_ZoneManager::Get().X_S(x, y, &l, &b);
	LY_ZoneManager::Get().X_S(yx, yy, &ll, &bb);
	LY_ZoneManager::Get().X_S(xx, xy, &lll, &bbb);

	double rd = sqrt(float((ll - l) * (ll - l) + (bb - b) * (bb - b)));

	QPointF pcenter(l, b);
	QPointF m_StartPoint(ll, bb);
	QPointF m_EndPoint(lll, bbb);

	double angleStart = atan2((double)(m_StartPoint.y() - pcenter.y()),
		(double)(m_StartPoint.x() - pcenter.x())) * 180.0 / 3.1415926;
	double angleEnd = atan2((double)(m_EndPoint.y() - pcenter.y()),
		(double)(m_EndPoint.x() - pcenter.x())) * 180.0 / 3.1415926;

	angleStart = abs(angleStart);
	angleEnd = abs(angleEnd);

	if (m_StartPoint.y() - pcenter.y() >= 0 && m_StartPoint.x() - pcenter.x() > 0)
	{
		angleStart = -angleStart;
	};
	if (m_StartPoint.y() - pcenter.y() > 0 && m_StartPoint.x() - pcenter.x() <= 0)
	{
		angleStart = -angleStart;
	}
	if (m_StartPoint.y() - pcenter.y() <= 0 && m_StartPoint.x() - pcenter.x() < 0)
	{
		angleStart = angleStart;
	}
	if (m_StartPoint.y() - pcenter.y() < 0 && m_StartPoint.x() - pcenter.x() >= 0)
	{
		angleStart = angleStart;
	}

	if (m_EndPoint.y() - pcenter.y() >= 0 && m_EndPoint.x() - pcenter.x() > 0)
	{
		angleEnd = -angleEnd;
	}
	if (m_EndPoint.y() - pcenter.y() > 0 && m_EndPoint.x() - pcenter.x() <= 0)
	{
		angleEnd = -angleEnd;
	}
	if (m_EndPoint.y() - pcenter.y() <= 0 && m_EndPoint.x() - pcenter.x() < 0)
	{
		angleEnd = angleEnd;
	}
	if (m_EndPoint.y() - pcenter.y() < 0 && m_EndPoint.x() - pcenter.x() >= 0)
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

	QRectF rectangle(QPointF(pcenter.x() - rd, pcenter.y() - rd), QPointF(pcenter.x() + rd, pcenter.y() + rd));

	QPainterPath path;
	path.moveTo(pcenter);
	path.arcTo(rectangle, angleStart, (0 - sweepAngle));
	painter->drawPath(path);

	m_pZoneCenter = pcenter;
	m_pShanStartPoint = m_StartPoint;
	m_pShanEndPoint = m_EndPoint;

	if (m_pZoneSelected) {
		auto pen = painter->pen();
		pen.setColor(QColor(255, 255, 255));
		painter->setPen(pen);

		painter->drawEllipse(m_StartPoint, m_pDragPolyRadius, m_pDragPolyRadius);
		painter->drawEllipse(m_EndPoint, m_pDragPolyRadius, m_pDragPolyRadius);

		QRegion r1(m_StartPoint.x() - m_pDragPolyRadius, m_StartPoint.y() - m_pDragPolyRadius, 2 * m_pDragPolyRadius, 2 * m_pDragPolyRadius, QRegion::RegionType::Ellipse);
		QRegion r2(m_EndPoint.x() - m_pDragPolyRadius, m_EndPoint.y() - m_pDragPolyRadius, 2 * m_pDragPolyRadius, 2 * m_pDragPolyRadius, QRegion::RegionType::Ellipse);
		QPainterPath p1;
		p1.addRegion(r1);
		dragPolyVec.push_back(p1.toFillPolygon());
		QPainterPath p2;
		p2.addRegion(r2);
		dragPolyVec.push_back(p2.toFillPolygon());
	}

	zonePoly = path.toFillPolygon();
}

void ZoneAdditionDrawWidget::DrawQYEllipse(QPainter *painter, double x, double y, float fLRadius, float fSRadius, float fRotateAngle, int filltype, QPolygonF &zonePoly, QVector<QPolygonF> &dragPolyVec)
{
	painter->save();

	float l, b, ll, bb;
	LY_ZoneManager::Get().X_S(x, y, &l, &b);

	ll = LY_ZoneManager::Get().m_pProj->Map2ScreenDistance(fLRadius);
	bb = LY_ZoneManager::Get().m_pProj->Map2ScreenDistance(fSRadius);

	painter->translate(l, b);

	painter->drawEllipse(QPointF(0, 0), ll, bb);
	painter->restore();

	QRegion region(-ll, -bb, 2 * ll, 2 * bb, QRegion::RegionType::Ellipse);
	QPainterPath path;
	path.addRegion(region);
	path.translate(l, b);

	if (m_pZoneSelected) {
		auto pen = painter->pen();
		pen.setColor(QColor(255, 255, 255));
		painter->setPen(pen);
		painter->drawEllipse(QPointF(l - ll, b), m_pDragPolyRadius, m_pDragPolyRadius);
		painter->drawEllipse(QPointF(l, b - bb), m_pDragPolyRadius, m_pDragPolyRadius);
		painter->drawEllipse(QPointF(l + ll, b), m_pDragPolyRadius, m_pDragPolyRadius);
		painter->drawEllipse(QPointF(l, b + bb), m_pDragPolyRadius, m_pDragPolyRadius);

		QRegion r1(l - ll - m_pDragPolyRadius, b - m_pDragPolyRadius, 2 * m_pDragPolyRadius, 2 * m_pDragPolyRadius, QRegion::RegionType::Ellipse);
		QRegion r2(l - m_pDragPolyRadius, b - bb - m_pDragPolyRadius, 2 * m_pDragPolyRadius, 2 * m_pDragPolyRadius, QRegion::RegionType::Ellipse);
		QRegion r3(l + ll - m_pDragPolyRadius, b - m_pDragPolyRadius, 2 * m_pDragPolyRadius, 2 * m_pDragPolyRadius, QRegion::RegionType::Ellipse);
		QRegion r4(l - m_pDragPolyRadius, b + bb - m_pDragPolyRadius, 2 * m_pDragPolyRadius, 2 * m_pDragPolyRadius, QRegion::RegionType::Ellipse);
		QPainterPath p1;
		p1.addRegion(r1);
		dragPolyVec.push_back(p1.toFillPolygon());
		QPainterPath p2;
		p2.addRegion(r2);
		dragPolyVec.push_back(p2.toFillPolygon());
		QPainterPath p3;
		p3.addRegion(r3);
		dragPolyVec.push_back(p3.toFillPolygon());
		QPainterPath p4;
		p4.addRegion(r4);
		dragPolyVec.push_back(p4.toFillPolygon());
	}

	zonePoly = path.toFillPolygon();
}

void ZoneAdditionDrawWidget::DrawKongYuRect(QPainter *painter, qnzkna::ZoneManage::CZoneBase *pZone, double x1, double y1, QPolygonF &zonePoly, QVector<QPolygonF> &dragPolyVec, bool flag )
{
	double centerX = 0, centerY = 0;
	QVector<QPointF> m_mPoints;
	calcPoint(x1, y1, pZone->stRoundParameter.nRaidusChang, pZone->stRoundParameter.nRaidusDuan, pZone->stRoundParameter.nNorthAngel, pZone->stRoundParameter.nCornerRadius, centerX, centerY, m_mPoints);

	painter->drawPolygon(m_mPoints);

	if (m_pZoneSelected) {
		auto pen = painter->pen();
		pen.setColor(QColor(255, 255, 255));
		painter->setPen(pen);

		int n = m_mPoints.size();
		for (int i = 0; i < n; i++) {
			painter->drawEllipse(m_mPoints[i], m_pDragPolyRadius, m_pDragPolyRadius);
			QRegion r1(m_mPoints[i].x() - m_pDragPolyRadius, m_mPoints[i].y() - m_pDragPolyRadius, 2 * m_pDragPolyRadius, 2 * m_pDragPolyRadius, QRegion::RegionType::Ellipse);
			QPainterPath p1;
			p1.addRegion(r1);
			dragPolyVec.push_back(p1.toFillPolygon());
		}

		for (int i = 0; i < n; i++) {
			double tx = (m_mPoints[i].x() + m_mPoints[(i + 1) % n].x()) / 2;
			double ty = (m_mPoints[i].y() + m_mPoints[(i + 1) % n].y()) / 2;
			if (std::fabs(tx - centerX) < 0.001 && std::fabs(ty - centerY) < 0.001)
				continue;
			else {
				painter->drawEllipse(QPointF(tx, ty), m_pDragPolyRadius, m_pDragPolyRadius);
				QRegion r2(tx - m_pDragPolyRadius, ty - m_pDragPolyRadius, 2 * m_pDragPolyRadius, 2 * m_pDragPolyRadius, QRegion::RegionType::Ellipse);
				QPainterPath p2;
				p2.addRegion(r2);
				dragPolyVec.push_back(p2.toFillPolygon());
			}
		}
	}

	m_pZoneCenter = QPointF(centerX, centerY);
	zonePoly = m_mPoints;
}

void ZoneAdditionDrawWidget::DrawCircleRing(QPainter *painter, double x, double y, qnzkna::ZoneManage::CZoneBase &zone, QPolygonF &zonePoly, QVector<QPolygonF> &dragPolyVec)
{
	painter->save();

	double yx, yy, dx, dy;
	yx = x + zone.stRoundParameter.nRaidusChang * sin(0 * PI / 180.);
	yy = y + zone.stRoundParameter.nRaidusChang * cos(0 * PI / 180.);

	dx = x + zone.stRoundParameter.nRaidusDuan * sin(0 * PI / 180.);
	dy = y + zone.stRoundParameter.nRaidusDuan * cos(0 * PI / 180.);

	float l, b, ll, bb, lll, bbb;
	LY_ZoneManager::Get().X_S(x, y, &l, &b);
	LY_ZoneManager::Get().X_S(yx, yy, &ll, &bb);
	LY_ZoneManager::Get().X_S(dx, dy, &lll, &bbb);

	m_OutRadius = sqrt(float((ll - l) * (ll - l) + (bb - b) * (bb - b)));
	m_inRadius = sqrt(float((lll - l) * (lll - l) + (bbb - b) * (bbb - b)));

	QPainterPath path;
	path.addEllipse(QPointF(l, b), m_OutRadius, m_OutRadius);
	path.addEllipse(QPointF(l, b), m_inRadius, m_inRadius);
	path.setFillRule(Qt::OddEvenFill);
	painter->drawPath(path);

	if (m_pZoneSelected) {
		auto pen = painter->pen();
		pen.setColor(QColor(255, 255, 255));
		painter->setPen(pen);
		double m_fRadius = m_OutRadius;
		int n = 2;
		while (n--) {
			painter->drawEllipse(QPointF(l - m_fRadius, b), m_pDragPolyRadius, m_pDragPolyRadius);
			painter->drawEllipse(QPointF(l, b - m_fRadius), m_pDragPolyRadius, m_pDragPolyRadius);
			painter->drawEllipse(QPointF(l + m_fRadius, b), m_pDragPolyRadius, m_pDragPolyRadius);
			painter->drawEllipse(QPointF(l, b + m_fRadius), m_pDragPolyRadius, m_pDragPolyRadius);
			QRegion r1(l - m_fRadius - m_pDragPolyRadius, b - m_pDragPolyRadius, 2 * m_pDragPolyRadius, 2 * m_pDragPolyRadius, QRegion::RegionType::Ellipse);
			QRegion r2(l - m_pDragPolyRadius, b - m_fRadius - m_pDragPolyRadius, 2 * m_pDragPolyRadius, 2 * m_pDragPolyRadius, QRegion::RegionType::Ellipse);
			QRegion r3(l + m_fRadius - m_pDragPolyRadius, b - m_pDragPolyRadius, 2 * m_pDragPolyRadius, 2 * m_pDragPolyRadius, QRegion::RegionType::Ellipse);
			QRegion r4(l - m_pDragPolyRadius, b + m_fRadius - m_pDragPolyRadius, 2 * m_pDragPolyRadius, 2 * m_pDragPolyRadius, QRegion::RegionType::Ellipse);
			QPainterPath p1;
			p1.addRegion(r1);
			dragPolyVec.push_back(p1.toFillPolygon());
			QPainterPath p2;
			p2.addRegion(r2);
			dragPolyVec.push_back(p2.toFillPolygon());
			QPainterPath p3;
			p3.addRegion(r3);
			dragPolyVec.push_back(p3.toFillPolygon());
			QPainterPath p4;
			p4.addRegion(r4);
			dragPolyVec.push_back(p4.toFillPolygon());
			m_fRadius = m_inRadius;
		}
	}

	painter->restore();

	zonePoly = path.toFillPolygon();
}

void ZoneAdditionDrawWidget::DrawArcRing(QPainter *painter, float x, float y, qnzkna::ZoneManage::CZoneBase &zone, QPolygonF &zonePoly, QVector<QPolygonF> &dragPolyVec)
{
	double yx, yy, xx, xy;
	yx = x + zone.stRoundParameter.nRaidusChang * sin(zone.stRoundParameter.nStartAngel * PI / 180.);
	yy = y + zone.stRoundParameter.nRaidusChang * cos(zone.stRoundParameter.nStartAngel * PI / 180.);
	xx = x + zone.stRoundParameter.nRaidusChang * sin(zone.stRoundParameter.nEndAngel * PI / 180.);
	xy = y + zone.stRoundParameter.nRaidusChang * cos(zone.stRoundParameter.nEndAngel * PI / 180.);

	float l, b, ll, bb, lll, bbb;
	LY_ZoneManager::Get().X_S(x, y, &l, &b);
	LY_ZoneManager::Get().X_S(yx, yy, &ll, &bb);
	LY_ZoneManager::Get().X_S(xx, xy, &lll, &bbb);

	double rd = sqrt(float((ll - l) * (ll - l) + (bb - b) * (bb - b)));

	double nsx, nsy, nex, ney;
	nsx = x + zone.stRoundParameter.nRaidusDuan * sin(zone.stRoundParameter.nStartAngel * PI / 180.);
	nsy = y + zone.stRoundParameter.nRaidusDuan * cos(zone.stRoundParameter.nStartAngel * PI / 180.);
	nex = x + zone.stRoundParameter.nRaidusDuan * sin(zone.stRoundParameter.nEndAngel * PI / 180.);
	ney = y + zone.stRoundParameter.nRaidusDuan * cos(zone.stRoundParameter.nEndAngel * PI / 180.);
	float fnsx, fnsy, fnex, fney;
	LY_ZoneManager::Get().X_S(nsx, nsy, &fnsx, &fnsy);
	LY_ZoneManager::Get().X_S(nex, ney, &fnex, &fney);
	m_nStartPoint = QPointF(fnsx, fnsy);
	m_nEndPoint = QPointF(fnex, fney);

	float ldx, ldy;
	LY_ZoneManager::Get().X_S(nsx, nsy, &ldx, &ldy);
	double inrd = sqrt(float((ldx - l) * (ldx - l) + (ldy - b) * (ldy - b)));

	QPointF pcenter(l, b);
	m_wStartPoint = QPointF(ll, bb);
	m_wEndPoint = QPointF(lll, bbb);
	m_pZoneCenter = pcenter;
	m_pShanStartPoint = m_wStartPoint;
	m_pShanEndPoint = m_wEndPoint;

	double angleStart = atan2((double)(m_wStartPoint.y() - pcenter.y()),
		(double)(m_wStartPoint.x() - pcenter.x())) * 180.0 / 3.1415926;
	double angleEnd = atan2((double)(m_wEndPoint.y() - pcenter.y()),
		(double)(m_wEndPoint.x() - pcenter.x())) * 180.0 / 3.1415926;

	angleStart = abs(angleStart);
	angleEnd = abs(angleEnd);

	if (m_wStartPoint.y() - pcenter.y() >= 0 && m_wStartPoint.x() - pcenter.x() > 0)
	{
		angleStart = -angleStart;
	};
	if (m_wStartPoint.y() - pcenter.y() > 0 && m_wStartPoint.x() - pcenter.x() <= 0)
	{
		angleStart = -angleStart;
	}
	if (m_wStartPoint.y() - pcenter.y() <= 0 && m_wStartPoint.x() - pcenter.x() < 0)
	{
		angleStart = angleStart;
	}
	if (m_wStartPoint.y() - pcenter.y() < 0 && m_wStartPoint.x() - pcenter.x() >= 0)
	{
		angleStart = angleStart;
	}

	if (m_wEndPoint.y() - pcenter.y() >= 0 && m_wEndPoint.x() - pcenter.x() > 0)
	{
		angleEnd = -angleEnd;
	}
	if (m_wEndPoint.y() - pcenter.y() > 0 && m_wEndPoint.x() - pcenter.x() <= 0)
	{
		angleEnd = -angleEnd;
	}
	if (m_wEndPoint.y() - pcenter.y() <= 0 && m_wEndPoint.x() - pcenter.x() < 0)
	{
		angleEnd = angleEnd;
	}
	if (m_wEndPoint.y() - pcenter.y() < 0 && m_wEndPoint.x() - pcenter.x() >= 0)
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

	QRectF rectangle(QPointF(pcenter.x() - rd, pcenter.y() - rd), QPointF(pcenter.x() + rd, pcenter.y() + rd));
	QRectF rectangle1(QPointF(pcenter.x() - inrd, pcenter.y() - inrd), QPointF(pcenter.x() + inrd, pcenter.y() + inrd));

	painter->save();
	painter->setPen(Qt::NoPen);

	QPainterPath path;
	path.moveTo(pcenter);
	path.arcTo(rectangle, angleStart, (0 - sweepAngle));
	path.closeSubpath();

	path.moveTo(pcenter);
	path.arcTo(rectangle1, angleStart, (0 - sweepAngle));
	path.closeSubpath();

	path.setFillRule(Qt::OddEvenFill);
	painter->drawPath(path);

	if (m_pZoneSelected) {
		QPen m_penCur;
		m_penCur.setStyle((Qt::PenStyle)zone.nLineType);
		m_penCur.setWidth(2);

		unsigned int uiV = zone.nLineColor;
		QColor color = QColor(uiV & 0xFF, (uiV >> 8) & 0xFF, (uiV >> 16) & 0xFF);
		color.setAlpha(100);
		m_penCur.setColor(QColor(255, 255, 255));
		painter->setPen(m_penCur);
		painter->drawEllipse(m_wStartPoint, m_pDragPolyRadius, m_pDragPolyRadius);
		painter->drawEllipse(m_wEndPoint, m_pDragPolyRadius, m_pDragPolyRadius);
		painter->drawEllipse(m_nStartPoint, m_pDragPolyRadius, m_pDragPolyRadius);
		painter->drawEllipse(m_nEndPoint, m_pDragPolyRadius, m_pDragPolyRadius);

		QRegion r1(m_wStartPoint.x() - m_pDragPolyRadius, m_wStartPoint.y() - m_pDragPolyRadius, 2 * m_pDragPolyRadius, 2 * m_pDragPolyRadius, QRegion::RegionType::Ellipse);
		QRegion r2(m_wEndPoint.x() - m_pDragPolyRadius, m_wEndPoint.y() - m_pDragPolyRadius, 2 * m_pDragPolyRadius, 2 * m_pDragPolyRadius, QRegion::RegionType::Ellipse);
		QPainterPath p1;
		p1.addRegion(r1);
		dragPolyVec.push_back(p1.toFillPolygon());
		QPainterPath p2;
		p2.addRegion(r2);
		dragPolyVec.push_back(p2.toFillPolygon());
		QRegion r3(m_nStartPoint.x() - m_pDragPolyRadius, m_nStartPoint.y() - m_pDragPolyRadius, 2 * m_pDragPolyRadius, 2 * m_pDragPolyRadius, QRegion::RegionType::Ellipse);
		QRegion r4(m_nEndPoint.x() - m_pDragPolyRadius, m_nEndPoint.y() - m_pDragPolyRadius, 2 * m_pDragPolyRadius, 2 * m_pDragPolyRadius, QRegion::RegionType::Ellipse);
		QPainterPath p3;
		p3.addRegion(r3);
		dragPolyVec.push_back(p3.toFillPolygon());
		QPainterPath p4;
		p4.addRegion(r4);
		dragPolyVec.push_back(p4.toFillPolygon());
	}

	painter->restore();

	zonePoly = path.toFillPolygon();
}

void ZoneAdditionDrawWidget::calcPoint(double x1, double y1, double l, double h, double angle, double type, double &centerx, double &centery, QVector<QPointF> &m_mPoints)
{
	m_mPoints.clear();
	double dis;
	double j1, j2, j3, j4;
	double dx1, dy1, dx2, dy2, dx3, dy3, dx4, dy4;
	if (type == 0)
	{
		dis = sqrt(l * l + h * h) / 2;
		if (LY_ZoneManager::Get().m_pProj != nullptr)
		{
			j1 = angle - atan(h / l) * 180 / PI;
			LY_ZoneManager::Get().m_pProj->A_X(x1, y1, dis, j1, dx1, dy1);

			j2 = angle + atan(h / l) * 180 / PI;
			LY_ZoneManager::Get().m_pProj->A_X(x1, y1, dis, j2, dx2, dy2);

			j3 = j1 + 180;
			LY_ZoneManager::Get().m_pProj->A_X(x1, y1, dis, j3, dx3, dy3);

			j4 = j2 + 180;
			LY_ZoneManager::Get().m_pProj->A_X(x1, y1, dis, j4, dx4, dy4);

			int px, py;
			LY_ZoneManager::Get().m_pProj->xyLtoD(x1, y1, &px, &py);
			centerx = (double)px;
			centery = (double)py;
		}
	}

	else
	{
		if (LY_ZoneManager::Get().m_pProj != nullptr)
		{
			dis = h / 2;
			j1 = angle - 90;
			LY_ZoneManager::Get().m_pProj->A_X(x1, y1, dis, j1, dx1, dy1);
			j4 = angle + 90;
			LY_ZoneManager::Get().m_pProj->A_X(x1, y1, dis, j4, dx4, dy4);

			dis = l;
			j2 = j3 = angle;
			LY_ZoneManager::Get().m_pProj->A_X(dx1, dy1, dis, j2, dx2, dy2);
			LY_ZoneManager::Get().m_pProj->A_X(dx4, dy4, dis, j3, dx3, dy3);

			double dx5, dy5;
			int px, py;
			LY_ZoneManager::Get().m_pProj->A_X(x1, y1, dis / 2, angle, dx5, dy5);
			LY_ZoneManager::Get().m_pProj->xyLtoD(dx5, dy5, &px, &py);
			centerx = (double)px;
			centery = (double)py;
		}
	}
	float ll, bb;
	LY_ZoneManager::Get().X_S(dx1, dy1, &ll, &bb);
	m_mPoints.push_back(QPointF(ll, bb));

	LY_ZoneManager::Get().X_S(dx2, dy2, &ll, &bb);
	m_mPoints.push_back(QPointF(ll, bb));

	LY_ZoneManager::Get().X_S(dx3, dy3, &ll, &bb);
	m_mPoints.push_back(QPointF(ll, bb));

	LY_ZoneManager::Get().X_S(dx4, dy4, &ll, &bb);
	m_mPoints.push_back(QPointF(ll, bb));
}

void ZoneAdditionDrawWidget::EditMouseMoveJWD(double ll, double bb)
{
	m_pCDlgZoneParameterEdit->EditMouseMoveJWD(ll, bb);
}

void ZoneAdditionDrawWidget::EditMouseMoveRadius(int dragIndex, double ll, double bb)
{
	QGeoCoordinate curGeoCo(bb, ll);
	double jd = m_ZoneBase.stRoundParameter.dLon;
	double wd = m_ZoneBase.stRoundParameter.dLat;
	QGeoCoordinate yxGeoCo(wd, jd);

	int px, py;
	LY_ZoneManager::Get().L_S(ll, bb, &px, &py);
	QPointF curPoint(px, py);

	if (m_iQYZoneType == CZoneBase::CIRCLE || m_iQYZoneType == CZoneBase::PIE) {
		int raduis = curGeoCo.distanceTo(yxGeoCo);
		m_pCDlgZoneParameterEdit->EditMouseMoveRadius(raduis);
	}
	if (m_iQYZoneType == CZoneBase::PIE || m_iQYZoneType == CZoneBase::PIERING) {
		if (dragIndex >=0 && dragIndex < m_DragPolyVec.size()) {
			if (dragIndex % 2 == 0) {
				QLineF line_guding(m_pZoneCenter, m_pShanEndPoint);
				QLineF line_dong(m_pZoneCenter, curPoint);
				double ang = std::fabs(line_guding.angleTo(line_dong));
				int qsj = m_ZoneBase.stRoundParameter.nEndAngel - ang;
				m_pCDlgZoneParameterEdit->EditMouseMoveSEAngle(qsj, 0);
			}
			else {
				QLineF line_guding(m_pZoneCenter, m_pShanStartPoint);
				QLineF line_dong(m_pZoneCenter, curPoint);
				double ang = std::fabs(line_dong.angleTo(line_guding));
				int zzj = m_ZoneBase.stRoundParameter.nStartAngel + ang;
				m_pCDlgZoneParameterEdit->EditMouseMoveSEAngle(0, zzj);
			}
		}
	}
	if (m_iQYZoneType == CZoneBase::ELLIPSE) {
		if (dragIndex >= 0 && dragIndex < m_DragPolyVec.size()) {
			if (dragIndex % 2 == 0) {
				int cbz = curGeoCo.distanceTo(yxGeoCo);
				m_pCDlgZoneParameterEdit->EditMouseMoveCDRadius(cbz, -1);
			}
			else {
				int dbz = curGeoCo.distanceTo(yxGeoCo);
				m_pCDlgZoneParameterEdit->EditMouseMoveCDRadius(-1, dbz);
			}
		}
	}
	if (m_iQYZoneType == CZoneBase::CIRCLERING || m_iQYZoneType == CZoneBase::PIERING) {
		int n = m_DragPolyVec.size();
		if (dragIndex >= 0 && dragIndex < m_DragPolyVec.size()) {
			int wbj = m_ZoneBase.stRoundParameter.nRaidusChang;
			int nbj = m_ZoneBase.stRoundParameter.nRaidusDuan;
			if (dragIndex < n / 2) {
				int outRaduis = curGeoCo.distanceTo(yxGeoCo);
				if (outRaduis <= nbj)
					return;
				m_pCDlgZoneParameterEdit->EditMouseMoveWNRadius(outRaduis, -1);
			}
			else {
				int inRaduis = curGeoCo.distanceTo(yxGeoCo);
				if (inRaduis >= wbj)
					return;
				m_pCDlgZoneParameterEdit->EditMouseMoveWNRadius(-1, inRaduis);
			}
		}
	}
	if (m_iQYZoneType == CZoneBase::RECTANGLE) {
		if (dragIndex >= 0 && dragIndex < m_DragPolyVec.size()) {
			double tjd, twd;
			LY_ZoneManager::Get().S_L(m_pZoneCenter.x(), m_pZoneCenter.y(), &tjd, &twd);
			QGeoCoordinate yx(twd, tjd);
			if (dragIndex < 4) {
				double newdjx = curGeoCo.distanceTo(yx) * 2;
				int chang = m_ZoneBase.stRoundParameter.nRaidusChang;
				int kuan = m_ZoneBase.stRoundParameter.nRaidusDuan;
				double djx = std::sqrt(chang*chang + kuan*kuan);
				int newchang = newdjx * chang / djx;
				int newkuan = newdjx * kuan / djx;
				m_pCDlgZoneParameterEdit->EditMouseMoveChangKuan(newchang, newkuan);
			}
			else {
				int newdis = curGeoCo.distanceTo(yx) * 2;
				if (dragIndex % 2 == 0) {
					m_pCDlgZoneParameterEdit->EditMouseMoveChangKuan(newdis, -1);
				}
				else {
					m_pCDlgZoneParameterEdit->EditMouseMoveChangKuan(-1, newdis);
				}
			}
		}
	}

}

void ZoneAdditionDrawWidget::isZoneSelected(bool isSelected)
{
	m_pZoneSelected = isSelected;
}

bool ZoneAdditionDrawWidget::isNewNameValidate(const QString &name, char zname[])
{
	return true;
}

void ZoneAdditionDrawWidget::initZoneBase()
{
	m_ZoneBase.nZoneType = m_iQYZoneType;
	m_ZoneBase.nLineType = 1;
	m_ZoneBase.nFillType = 1;
	QColor d(Qt::red);
	unsigned int uiV = (d.blue() & 0xFF) << 16 | (d.green() & 0xFF) << 8 | (d.red() & 0xFF);
	m_ZoneBase.nLineColor = uiV;
	m_ZoneBase.nAboveAlt = 200;
	m_ZoneBase.nBottomhAlt = 0;
	m_ZoneBase.stRoundParameter.nRadius = 1000;
	m_ZoneBase.stRoundParameter.nStartAngel = 0;
	m_ZoneBase.stRoundParameter.nEndAngel = 120;
	m_ZoneBase.stRoundParameter.nRaidusChang = 1000;
	m_ZoneBase.stRoundParameter.nRaidusDuan = 500;
	m_ZoneBase.stRoundParameter.nAngelChang = 90;
	m_ZoneBase.nLineWidth = 100;
	m_ZoneBase.stZonePointVec.clear();
}

void ZoneAdditionDrawWidget::SetZonebase()
{
	m_pCDlgZoneParameterEdit->SetZonebase(m_ZoneBase, true);
}

CDlgZoneParameterEdit * ZoneAdditionDrawWidget::getCDlgZoneParameterEdit()
{
	return m_pCDlgZoneParameterEdit;
}

void ZoneAdditionDrawWidget::slot_ly_qymc_changed()
{
	auto zoneMangetServicePtr = IZoneManagerActivator::Get().getPIZoneManageService();
	if (zoneMangetServicePtr != nullptr)
	{
		m_ZoneBase = m_pCDlgZoneParameterEdit->GetZonneBase();
		if(m_ZoneBase.nZoneType == 4)
			zoneMangetServicePtr->createADrawAble_3d(m_ZoneBase.strID, m_ZoneBase.nZoneType, m_ZoneBase.nLineColor,0.5, m_ZoneBase.stRoundParameter.nRaidusDuan,
				m_ZoneBase.stRoundParameter.nRaidusChang, m_ZoneBase.stRoundParameter.nNorthAngel);
		else
			zoneMangetServicePtr->createADrawAble_3d(m_ZoneBase.strID, m_ZoneBase.nZoneType, m_ZoneBase.nLineColor);
	}
}

void ZoneAdditionDrawWidget::slotButtonChange(int index)
{
	m_iQYZoneType = index - 1;
	initZoneBase();
	if (m_iQYZoneType == qnzkna::ZoneManage::CZoneBase::LINE) {
		m_ZoneBase.nProperty = qnzkna::ZoneManage::CZoneBase::ZoneProperty::HL;
	}
	else {
		m_ZoneBase.nProperty = qnzkna::ZoneManage::CZoneBase::ZoneProperty::QT;
	}
	strcpy(m_ZoneBase.strID, m_pCDlgZoneParameterEdit->GetZonneBase().strID);

	m_pCDlgZoneParameterEdit->SetZonebase(m_ZoneBase, true);

	for (int i = 1; i <= 9; i++) {
		if (i == index)
			m_pButtonGroup->button(i)->setStyleSheet("border-image: url(:/images/1k/black/QPushButton/pressed.png);");
		else
			m_pButtonGroup->button(i)->setStyleSheet("border-image: url(:/images/1k/black/QPushButton/normal.png);");
	}

	auto zoneMangetServicePtr = IZoneManagerActivator::Get().getPIZoneManageService();
	if (zoneMangetServicePtr != nullptr)
	{
		if (m_ZoneBase.nZoneType == 4)
			zoneMangetServicePtr->createADrawAble_3d(m_ZoneBase.strID, m_ZoneBase.nZoneType, m_ZoneBase.nLineColor, 0.5, m_ZoneBase.stRoundParameter.nRaidusDuan,
				m_ZoneBase.stRoundParameter.nRaidusChang, m_ZoneBase.stRoundParameter.nNorthAngel);
		else
			zoneMangetServicePtr->createADrawAble_3d(m_ZoneBase.strID, m_ZoneBase.nZoneType, m_ZoneBase.nLineColor);
	}
}
