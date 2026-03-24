#include "IZoneManagerImpl.h"
#include "QSettings"
#include "LyMessageBox.h"
#include "QCString.h"
#include "QDb.h"
#include "LY_ZoneManagerServive.h"
#include "CreateShape.h"
#include "map3d/IEarth.h"
#include "ShapeClip.h"
#include "AcmGeometry.h"
#include "map3d/IAirLine.h"
#include "DrawShape.h"
#include "QtPositioning/QGeoCoordinate"
#include "vector3.h"
#include <queue>
#include <QHash>

#include <osgEarth/MapNode>
#include <osgEarthFeatures/Feature>
#include <osgEarthAnnotation/AnnotationUtils>
#include <osgEarthAnnotation/FeatureNode>

#include <osgEarthAnnotation/FeatureEditing>
#include <osgEarthAnnotation/AnnotationEditing>

#include <osg/CoordinateSystemNode>

#include <osg/Geometry>
#include "TinyXML/tinyxml.h"
#include <map3d/IEarth.h>
#include <map3d/EarthEvent.h>
#include <map3d/IEarth.h>
#include <map3d/ITrack.h>
#include <map3d/IGraphics.h>

#include <osgEarth/Map>

#include <osgViewer/Viewer>
#include <osgViewer/View>

#include <osgEarth/IntersectionPicker>

#include <osgUtil/LineSegmentIntersector>

#include "LY_ZoneManagerServiveInterface.h"

#include "drawAble_Polygon.h"
#include "drawAble_LineString.h"
#include "drawAble_Circle.h"
#include "drawAble_Rect.h"
#include <map>

#define PI			3.1415926
#define NPrecision	10000
using namespace qnzkna::ZoneManage;

CGetHeight IZoneManagerImpl::s_CGetHeight("../data/dem/90");

IZoneManagerImpl::IZoneManagerImpl(void)
{
	InitDataBase();
	LoadZoneBaseFormDB();
	LoadZoneBaseAdditionFormDB();
	LoadZoneRuleFromDB();
	LoadRelationFromDB();
	LoadLineDB();
	LoadZoneScaleIni();
	Init3DZones();
	LoadLableColorDB();

	m_strZoneFilePath = "../data/ZoneFile/";
}

IZoneManagerImpl::~IZoneManagerImpl(void)
{
	m_db.close();
}

void qnzkna::ZoneManage::IZoneManagerImpl::InitDataBase()
{
	QSettings ini("../data/conf/ly.ini", QSettings::IniFormat);
	ini.beginGroup("Database");
	QString strDbName = ini.value("DbName", "LY").toString();
	QString strDbIP = ini.value("SvrIP", "192.168.1.115").toString();
	QString strDbUser = ini.value("User", "root").toString();
	QString strDbPass = ini.value("Password", "123456").toString();
	QString strDbDriver = ini.value("Driver", "QMYSQL").toString();

	m_db = QSqlDatabase::addDatabase(strDbDriver);
	m_db.setHostName(strDbIP);
	m_db.setPort(3306);
	m_db.setDatabaseName(strDbName);
	m_db.setUserName(strDbUser);
	m_db.setPassword(strDbPass);
	bool bresult = m_db.open();
}

void qnzkna::ZoneManage::IZoneManagerImpl::LoadZoneBaseFormDB()
{
	if (!m_db.isOpen())
	{
		LyMessageBox::warning(nullptr, tr2("警告"), tr2("数据库连接失败!"));
		return;
	}
	m_ZoneBaseVec_All.clear();
	QSqlQuery query;
	QString strSQL = "select QYNM,XIANXING,QYLX,QYXZ,QYSG,QYXG,QYDS,QYCOLOR,TIANCHONG,XIANYIN,HDKD from `t_zone`  order by QYNM";
	bool success = query.exec(strSQL);
	if (success)
	{
		while (query.next())
		{
			QString str1 = tr2(query.value(0).toString().toLocal8Bit().data());
			QString str2 = tr2(query.value(1).toString().toLatin1().data());
			QString str3 = tr2(query.value(2).toString().toLatin1().data());
			QString str4 = tr2(query.value(3).toString().toLatin1().data());
			QString str5 = tr2(query.value(4).toString().toLatin1().data());
			QString str6 = tr2(query.value(5).toString().toLatin1().data());
			QString str7 = tr2(query.value(6).toString().toLatin1().data());
			QString str8 = tr2(query.value(7).toString().toLatin1().data());
			QString str9 = tr2(query.value(8).toString().toLatin1().data());
			QString str10 = tr2(query.value(9).toString().toLatin1().data());
			QString str11 = tr2(query.value(10).toString().toLatin1().data());

			CZoneBase stZone;
			strncpy(stZone.strID, str1.toLocal8Bit(), MAX_ZONE_LEN_48);
			stZone.nLineType = str2.toInt();
			stZone.nProperty = str3.toInt();
			stZone.nZoneType = str4.toInt();
			stZone.nAboveAlt = str5.toInt();
			stZone.nBottomhAlt = str6.toInt();
			stZone.nPointCount = str7.toInt();
			stZone.nLineColor = str8.toUInt();
			stZone.nFillType = str9.toInt();
			stZone.bShow = str10.toInt();
			stZone.nLineWidth = str11.toInt();

			LoadZoneBasePointFormDB(stZone);

			m_ZoneBaseVec_All.push_back(stZone);
		}
	}
}

void qnzkna::ZoneManage::IZoneManagerImpl::LoadZoneBasePointFormDB(CZoneBase& stData)
{
	if (stData.nPointCount == 0)
	{
		return;
	}

	QSqlQuery query;
	QString strSQL = QStringLiteral("select JD, WD from t_zone_p where QYNM='%1'").arg(tr2(stData.strID));
	bool success = query.exec(strSQL);
	if (!success)
	{
		return;
	}
	Zone_point_s_Vec TempVec;

	while (query.next())
	{
		Zone_point_s TmpData;

		QString str1 = tr2(query.value(0).toString().toLatin1().data());
		QString str2 = tr2(query.value(1).toString().toLatin1().data());

		TmpData.x = str1.toDouble();
		TmpData.y = str2.toDouble();

		TempVec.push_back(TmpData);
	}

	if (stData.nZoneType == 0)
	{
		if (TempVec.size() != 3)
		{
			return;
		}
		stData.stRoundParameter.dLon = TempVec[0].x;
		stData.stRoundParameter.dLat = TempVec[0].y;
		stData.stRoundParameter.nStartAngel = TempVec[1].x;
		stData.stRoundParameter.nEndAngel = TempVec[1].y;
		stData.stRoundParameter.nRadius = TempVec[2].x;
	}

	else if (stData.nZoneType == 1)
	{
		if (TempVec.size() != 3)
		{
			return;
		}
		stData.stRoundParameter.dLon = TempVec[0].x;
		stData.stRoundParameter.dLat = TempVec[0].y;
		stData.stRoundParameter.nStartAngel = TempVec[1].x;
		stData.stRoundParameter.nEndAngel = TempVec[1].y;
		stData.stRoundParameter.nRadius = TempVec[2].x;
	}

	else if (stData.nZoneType == 2)
	{
		if (TempVec.size() != 3)
		{
			return;
		}
		stData.stRoundParameter.dLon = TempVec[0].x;
		stData.stRoundParameter.dLat = TempVec[0].y;
		stData.stRoundParameter.nAngelChang = TempVec[1].x;
		stData.stRoundParameter.nRaidusChang = TempVec[2].x;
		stData.stRoundParameter.nRaidusDuan = TempVec[2].y;
	}

	else if (stData.nZoneType == 3 || stData.nZoneType == 8)
	{
		stData.stZonePointVec.clear();
		for (auto itr: TempVec)
		{
			ZonePoint stPoint;
			stPoint.dLon = itr.x;
			stPoint.dLat = itr.y;

			stData.stZonePointVec.push_back(stPoint);
		}
	}

	else if (stData.nZoneType == 4)
	{
		if (TempVec.size() == 3)
		{
			stData.stRoundParameter.dLon = TempVec[0].x;
			stData.stRoundParameter.dLat = TempVec[0].y;
			stData.stRoundParameter.nNorthAngel = TempVec[1].x;
			stData.stRoundParameter.nCornerRadius = TempVec[1].y;
			stData.stRoundParameter.nRaidusChang = TempVec[2].x;
			stData.stRoundParameter.nRaidusDuan = TempVec[2].y;

		}
		else if (TempVec.size() == 2)
		{
			stData.stRoundParameter.dLon = TempVec[0].x;
			stData.stRoundParameter.dLat = TempVec[0].y;
			stData.stRoundParameter.nRaidusChang = TempVec[1].x;
			stData.stRoundParameter.nRaidusDuan = TempVec[1].y;
		}
	}

	else if (stData.nZoneType == 5)
	{
		if (TempVec.size() != 3)
		{
			return;
		}
		stData.stRoundParameter.dLon = TempVec[0].x;
		stData.stRoundParameter.dLat = TempVec[0].y;
		stData.stRoundParameter.nRaidusChang = TempVec[2].x;
		stData.stRoundParameter.nRaidusDuan = TempVec[2].y;
	}

	else if (stData.nZoneType == 6)
	{
		if (TempVec.size() != 3)
		{
			return;
		}
		stData.stRoundParameter.dLon = TempVec[0].x;
		stData.stRoundParameter.dLat = TempVec[0].y;
		stData.stRoundParameter.nStartAngel = TempVec[1].x;
		stData.stRoundParameter.nEndAngel = TempVec[1].y;
		stData.stRoundParameter.nRaidusChang = TempVec[2].x;
		stData.stRoundParameter.nRaidusDuan = TempVec[2].y;
	}

	else if (stData.nZoneType == 7)
	{
		if (TempVec.size() != 1)
		{
			return;
		}
		stData.stRoundParameter.dLon = TempVec[0].x;
		stData.stRoundParameter.dLat = TempVec[0].y;
	}
}

bool qnzkna::ZoneManage::IZoneManagerImpl::AddZoneBaseToDB(const CZoneBase& stData)
{
	QString strID = QString::fromLocal8Bit(stData.strID);
	if (JudegeZoneIDIsExist(strID))
	{
		return false;
	}
	else
	{
		QString strSql = "insert into `Agri`.`t_zone`(QYNM, XIANXING, QYLX, QYXZ, QYSG, QYXG, QYDS, QYCOLOR, TIANCHONG,XIANYIN,HDKD) values(";
		strSql += "'" + QString::fromLocal8Bit(stData.strID) + "',";
		strSql +=		QString::number(stData.nLineType) + ",";
		strSql +=		QString::number(stData.nProperty) + ",";
		strSql +=		QString::number(stData.nZoneType) + ",";
		strSql +=		QString::number(stData.nAboveAlt) + ",";
		strSql +=		QString::number(stData.nBottomhAlt) + ",";
		strSql +=		QString::number(GetZonePointCount(stData)) + ",";
		strSql +=		QString::number(stData.nLineColor) + ",";
		strSql +=		QString::number(stData.nFillType) + ",";
		strSql +=		QString::number(stData.bShow) + ",";
		strSql +=		QString::number(stData.nLineWidth);
		strSql +=		")";
		QSqlQuery query;
		bool success = query.exec(strSql);
		if (success == false)
		{
			return false;
		}

		Zone_point_s_Vec TmpVec;
		success = TransformToDBData(stData, TmpVec);
		int nNo = 0;
		for (auto itr:TmpVec)
		{
			strSql = "insert into t_zone_p(QYNM, XH, JD, WD) values (";
			strSql += "'"+	QString::fromLocal8Bit(stData.strID) + "',";
			strSql +=		QString::number(nNo) + ",";
			if (itr.bDoubleFlag == true)
			{
				strSql += QString::number(itr.x, 'f', 7) + ",";
				strSql += QString::number(itr.y, 'f', 7);
			}
			else
			{
				strSql += QString::number(itr.x) + ",";
				strSql += QString::number(itr.y);
			}
			strSql += ")";

			success = query.exec(strSql);
			nNo++;

		}
		return true;
	}
}

bool qnzkna::ZoneManage::IZoneManagerImpl::EditZoneBaseToDB(const CZoneBase& stData)
{
	QString strID = QString::fromLocal8Bit(stData.strID);
	if (JudegeZoneIDIsExist(strID))
	{
		bool success = RemoveZoneBaseToDB(strID);
		if (success == false)
		{
			return false;
		}
		return AddZoneBaseToDB(stData);
	}
	else
	{
		return false;
	}

}

bool qnzkna::ZoneManage::IZoneManagerImpl::RemoveZoneBaseToDB(const QString& strID)
{
	QSqlQuery query;
	QString strSQL = "delete from t_zone where QYNM ='";
	strSQL += strID + "'";
	bool success = query.exec(strSQL);
	if (success == false)
	{
		return false;
	}

	return RemoveZoneBasePointFormDB(strID);
}

bool qnzkna::ZoneManage::IZoneManagerImpl::RemoveZoneBasePointFormDB(const QString& strID)
{
	QSqlQuery query;
	QString strSQL = "delete from t_zone_p where QYNM ='";
	strSQL += strID + "'";
	bool success = query.exec(strSQL);
	return success;
}

bool qnzkna::ZoneManage::IZoneManagerImpl::JudegeZoneIDIsExist(const QString& strID)
{
	QString strSql("SELECT  * FROM `t_zone` WHERE `QYNM` = ");
	strSql += "'" + strID + "'";
	QSqlQuery query;
	bool success = query.exec(strSql);
	if (success == false)
	{
		return false;
	}
	if (query.size() == 0)
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool qnzkna::ZoneManage::IZoneManagerImpl::TransformToDBData(const CZoneBase& stData, Zone_point_s_Vec& TmpVec)
{
	TmpVec.clear();

	if (stData.nZoneType == 0)
	{
		TmpVec.push_back(Zone_point_s(stData.stRoundParameter.dLon, stData.stRoundParameter.dLat,true));
		TmpVec.push_back(Zone_point_s(0, 360, false));
		TmpVec.push_back(Zone_point_s(stData.stRoundParameter.nRadius, 0, false));
	}

	else if (stData.nZoneType == 1)
	{
		TmpVec.push_back(Zone_point_s(stData.stRoundParameter.dLon, stData.stRoundParameter.dLat, true));
		TmpVec.push_back(Zone_point_s(stData.stRoundParameter.nStartAngel, stData.stRoundParameter.nEndAngel, false));
		TmpVec.push_back(Zone_point_s(stData.stRoundParameter.nRadius, 0, false));
	}

	else if (stData.nZoneType == 2)
	{
		TmpVec.push_back(Zone_point_s(stData.stRoundParameter.dLon, stData.stRoundParameter.dLat, true));
		TmpVec.push_back(Zone_point_s(stData.stRoundParameter.nAngelChang, 0, false));
		TmpVec.push_back(Zone_point_s(stData.stRoundParameter.nRaidusChang, stData.stRoundParameter.nRaidusDuan, false));
	}

	else if (stData.nZoneType == 3 || stData.nZoneType == 8)
	{
		for (auto itr :stData.stZonePointVec)
		{
			TmpVec.push_back(Zone_point_s(itr.dLon, itr.dLat, true));
		}
	}

	else if (stData.nZoneType == 4)
	{
		if (stData.stRoundParameter.nNorthAngel == 0 &&
			stData.stRoundParameter.nCornerRadius == 0)
		{
			TmpVec.push_back(Zone_point_s(stData.stRoundParameter.dLon, stData.stRoundParameter.dLat, true));
			TmpVec.push_back(Zone_point_s(stData.stRoundParameter.nRaidusChang, stData.stRoundParameter.nRaidusDuan, false));
		}
		else
		{
			TmpVec.push_back(Zone_point_s(stData.stRoundParameter.dLon, stData.stRoundParameter.dLat, true));
			TmpVec.push_back(Zone_point_s(stData.stRoundParameter.nNorthAngel, stData.stRoundParameter.nCornerRadius, false));
			TmpVec.push_back(Zone_point_s(stData.stRoundParameter.nRaidusChang, stData.stRoundParameter.nRaidusDuan, false));
		}
	}

	else if (stData.nZoneType == 5)
	{
		TmpVec.push_back(Zone_point_s(stData.stRoundParameter.dLon, stData.stRoundParameter.dLat, true));
		TmpVec.push_back(Zone_point_s(0, 360, false));
		TmpVec.push_back(Zone_point_s(stData.stRoundParameter.nRaidusChang, stData.stRoundParameter.nRaidusDuan, false));
	}

	else if (stData.nZoneType == 6)
	{
		TmpVec.push_back(Zone_point_s(stData.stRoundParameter.dLon, stData.stRoundParameter.dLat, true));
		TmpVec.push_back(Zone_point_s(stData.stRoundParameter.nStartAngel, stData.stRoundParameter.nEndAngel, false));
		TmpVec.push_back(Zone_point_s(stData.stRoundParameter.nRaidusChang, stData.stRoundParameter.nRaidusDuan, false));
	}

	else if (stData.nZoneType == 7)
	{
		TmpVec.push_back(Zone_point_s(stData.stRoundParameter.dLon, stData.stRoundParameter.dLat, true));
	}
	return true;
}

int qnzkna::ZoneManage::IZoneManagerImpl::GetZonePointCount(const CZoneBase& stData)
{
	if (stData.nZoneType == qnzkna::ZoneManage::CZoneBase::POLY ||
		stData.nZoneType == qnzkna::ZoneManage::CZoneBase::LINE )
	{
		return stData.stZonePointVec.size();
	}
	else
	{
		return 1;
	}
}

void qnzkna::ZoneManage::IZoneManagerImpl::DrawOneQuYu(QPainter *painter, CZoneBase &zone, bool bModify )
{
	double x, y;
	QString str, jwdstr;
	float  l, b;
	double centerX = 0, centerY = 0;
	QVector<QPointF> m_spoints;
	m_spoints.clear();

	if (zone.nZoneType == CZoneBase::CIRCLE)
	{
		LY_ZoneManagerServive::Get().L_X(zone.stRoundParameter.dLon, zone.stRoundParameter.dLat, &x, &y);
		LY_ZoneManagerServive::Get().X_S(x, y, &l, &b);
		DrawKongYuCircle(painter, x, y, zone.stRoundParameter.nRadius, zone.stRoundParameter.nStartAngel, zone.stRoundParameter.nEndAngel, zone.nZoneType, zone.nFillType);
		centerX = l;
		centerY = b;
	}
	else if (zone.nZoneType == CZoneBase::PIE)
	{
		LY_ZoneManagerServive::Get().L_X(zone.stRoundParameter.dLon, zone.stRoundParameter.dLat, &x, &y);
		LY_ZoneManagerServive::Get().X_S(x, y, &l, &b);

		QPainterPath path;
		DrawKongYuPie(painter, x, y, zone.stRoundParameter.nRadius, zone.stRoundParameter.nStartAngel, zone.stRoundParameter.nEndAngel, zone.nZoneType, zone.nFillType, path);
		painter->drawPath(path);

		centerX = l;
		centerY = b;
	}
	else if (zone.nZoneType == CZoneBase::ELLIPSE)
	{
		LY_ZoneManagerServive::Get().L_X(zone.stRoundParameter.dLon, zone.stRoundParameter.dLat, &x, &y);
		LY_ZoneManagerServive::Get().X_S(x, y, &l, &b);

		DrawQYEllipse(painter, x, y, zone.stRoundParameter.nRaidusChang, zone.stRoundParameter.nRaidusDuan, zone.stRoundParameter.nAngelChang, zone.nFillType);
		centerX = l;
		centerY = b;
	}
	else if (zone.nZoneType == CZoneBase::CIRCLERING)
	{
		LY_ZoneManagerServive::Get().L_X(zone.stRoundParameter.dLon, zone.stRoundParameter.dLat, &x, &y);
		LY_ZoneManagerServive::Get().X_S(x, y, &l, &b);

		DrawCircleRing(painter, x, y, zone);

		centerX = l;
		centerY = b;
	}
	else if (zone.nZoneType == CZoneBase::PIERING)
	{
		LY_ZoneManagerServive::Get().L_X(zone.stRoundParameter.dLon, zone.stRoundParameter.dLat, &x, &y);
		LY_ZoneManagerServive::Get().X_S(x, y, &l, &b);

		DrawArcRing(painter, x, y, zone);

		centerX = l;
		centerY = b;
	}
	else if (zone.nZoneType == CZoneBase::POLY || zone.nZoneType == CZoneBase::LINE)
	{
		for (auto itrPoint :zone.stZonePointVec)
		{
			LY_ZoneManagerServive::Get().L_X(itrPoint.dLon, itrPoint.dLat, &x, &y);
			LY_ZoneManagerServive::Get().X_S(x, y, &l, &b);

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
		LY_ZoneManagerServive::Get().L_X(zone.stRoundParameter.dLon, zone.stRoundParameter.dLat, &x, &y);
		LY_ZoneManagerServive::Get().X_S(x, y, &l, &b);

		DrawKongYuRect(painter, &zone, x, y);

		centerX = l;
		centerY = b;
	}
	else if (zone.nZoneType == CZoneBase::POINT)
	{
		LY_ZoneManagerServive::Get().L_X(zone.stRoundParameter.dLon, zone.stRoundParameter.dLat, &x, &y);
		LY_ZoneManagerServive::Get().X_S(x, y, &l, &b);

		DrawFG(painter, l - 3, b - 3, l + 3, b + 3);

		centerX = l;
		centerY = b;
	}

	if (zone.nZoneType == CZoneBase::RECTANGLE && zone.stZonePointVec.size() > 0)
	{
		centerX = centerX / zone.stZonePointVec.size();
		centerY = centerY / zone.stZonePointVec.size();
	}

}

void qnzkna::ZoneManage::IZoneManagerImpl::DrawKongYuCircle(QPainter *painter, double x, double y, float radius, float startangle, float endangle, int type, int filltype)
{
	double yx, yy;
	yx = x + radius * sin(startangle * PI / 180.);
	yy = y + radius * cos(startangle * PI / 180.);

	float l, b, ll, bb;
	LY_ZoneManagerServive::Get().X_S(x, y, &l, &b);
	LY_ZoneManagerServive::Get().X_S(yx, yy, &ll, &bb);

	double m_fRadius = sqrt(float((ll - l) * (ll - l) + (bb - b) * (bb - b)));

	painter->drawEllipse(QPointF(l, b), m_fRadius, m_fRadius);
}

void qnzkna::ZoneManage::IZoneManagerImpl::DrawKongYuPie(QPainter *pDC, double x, double y, float radius, float startangle, float endangle, int type, int filltype, QPainterPath &path)
{
	double yx, yy, xx, xy;
	yx = x + radius * sin(startangle * PI / 180.);
	yy = y + radius * cos(startangle * PI / 180.);
	xx = x + radius * sin(endangle * PI / 180.);
	xy = y + radius * cos(endangle * PI / 180.);

	float l, b, ll, bb, lll, bbb;
	LY_ZoneManagerServive::Get().X_S(x, y, &l, &b);
	LY_ZoneManagerServive::Get().X_S(yx, yy, &ll, &bb);
	LY_ZoneManagerServive::Get().X_S(xx, xy, &lll, &bbb);

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

	path.moveTo(pcenter);
	path.arcTo(rectangle, angleStart, (0 - sweepAngle));

}

void qnzkna::ZoneManage::IZoneManagerImpl::DrawQYEllipse(QPainter *painter, double x, double y, float fLRadius, float fSRadius, float fRotateAngle, int filltype)
{
	painter->save();

	float l, b, ll, bb;
	LY_ZoneManagerServive::Get().X_S(x, y, &l, &b);

	ll = LY_ZoneManagerServive::Get().m_pProj->Map2ScreenDistance(fLRadius);
	bb = LY_ZoneManagerServive::Get().m_pProj->Map2ScreenDistance(fSRadius);

	painter->translate(l, b);
	painter->rotate(fRotateAngle - 90);
	painter->drawEllipse(QPointF(0, 0), ll, bb);
	painter->restore();
}

void qnzkna::ZoneManage::IZoneManagerImpl::DrawCircleRing(QPainter *painter, double x, double y, CZoneBase &zone)
{
	painter->save();

	double yx, yy, dx, dy;
	yx = x + zone.stRoundParameter.nRaidusChang * sin(0 * PI / 180.);
	yy = y + zone.stRoundParameter.nRaidusChang * cos(0 * PI / 180.);

	dx = x + zone.stRoundParameter.nRaidusDuan * sin(0 * PI / 180.);
	dy = y + zone.stRoundParameter.nRaidusDuan * cos(0 * PI / 180.);

	float l, b, ll, bb, lll, bbb;
	LY_ZoneManagerServive::Get().X_S(x, y, &l, &b);
	LY_ZoneManagerServive::Get().X_S(yx, yy, &ll, &bb);
	LY_ZoneManagerServive::Get().X_S(dx, dy, &lll, &bbb);

	double m_OutRadius = sqrt(float((ll - l) * (ll - l) + (bb - b) * (bb - b)));
	double m_inRadius = sqrt(float((lll - l) * (lll - l) + (bbb - b) * (bbb - b)));

	QPainterPath path;
	path.addEllipse(QPointF(l, b), m_OutRadius, m_OutRadius);
	path.addEllipse(QPointF(l, b), m_inRadius, m_inRadius);
	path.setFillRule(Qt::OddEvenFill);

	painter->drawPath(path);
	painter->restore();
}

void qnzkna::ZoneManage::IZoneManagerImpl::DrawArcRing(QPainter *painter, float x, float y, CZoneBase &zone)
{
	double yx, yy, xx, xy;
	yx = x + zone.stRoundParameter.nRaidusChang * sin(zone.stRoundParameter.nStartAngel * PI / 180.);
	yy = y + zone.stRoundParameter.nRaidusChang * cos(zone.stRoundParameter.nStartAngel * PI / 180.);
	xx = x + zone.stRoundParameter.nRaidusChang * sin(zone.stRoundParameter.nEndAngel * PI / 180.);
	xy = y + zone.stRoundParameter.nRaidusChang * cos(zone.stRoundParameter.nEndAngel * PI / 180.);

	float l, b, ll, bb, lll, bbb;
	LY_ZoneManagerServive::Get().X_S(x, y, &l, &b);
	LY_ZoneManagerServive::Get().X_S(yx, yy, &ll, &bb);
	LY_ZoneManagerServive::Get().X_S(xx, xy, &lll, &bbb);

	double rd = sqrt(float((ll - l) * (ll - l) + (bb - b) * (bb - b)));

	double dx, dy;
	dx = x + zone.stRoundParameter.nRaidusDuan * sin(zone.stRoundParameter.nStartAngel * PI / 180.);
	dy = y + zone.stRoundParameter.nRaidusDuan * cos(zone.stRoundParameter.nStartAngel * PI / 180.);

	float ldx, ldy;
	LY_ZoneManagerServive::Get().X_S(dx, dy, &ldx, &ldy);
	double inrd = sqrt(float((ldy - l) * (ldx - l) + (ldy - b) * (ldy - b)));

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

	painter->restore();
}

void qnzkna::ZoneManage::IZoneManagerImpl::DrawKongYuRect(QPainter *painter, CZoneBase *pZone, double x1, double y1, bool flag )
{
	double centerX = 0, centerY = 0;
	QVector<QPointF> m_mPoints;
	calcPoint(x1, y1, pZone->stRoundParameter.nRaidusChang, pZone->stRoundParameter.nRaidusDuan, pZone->stRoundParameter.nNorthAngel, pZone->stRoundParameter.nCornerRadius, centerX, centerY, m_mPoints);

	painter->drawPolygon(m_mPoints);
}

void qnzkna::ZoneManage::IZoneManagerImpl::calcPoint(double x1, double y1, double l, double h, double angle, double type, double &centerx, double &centery, QVector<QPointF> &m_mPoints)
{
	m_mPoints.clear();
	double dis;
	double j1, j2, j3, j4;
	double dx1, dy1, dx2, dy2, dx3, dy3, dx4, dy4;
	if (type == 0)
	{
		dis = sqrt(l * l + h * h) / 2;
		if (LY_ZoneManagerServive::Get().m_pProj != nullptr)
		{
			j1 = angle - atan(h / l) * 180 / PI;
			LY_ZoneManagerServive::Get().m_pProj->A_X(x1, y1, dis, j1, dx1, dy1);

			j2 = angle + atan(h / l) * 180 / PI;
			LY_ZoneManagerServive::Get().m_pProj->A_X(x1, y1, dis, j2, dx2, dy2);

			j3 = j1 + 180;
			LY_ZoneManagerServive::Get().m_pProj->A_X(x1, y1, dis, j3, dx3, dy3);

			j4 = j2 + 180;
			LY_ZoneManagerServive::Get().m_pProj->A_X(x1, y1, dis, j4, dx4, dy4);

			int px, py;
			LY_ZoneManagerServive::Get().m_pProj->xyLtoD(x1, y1, &px, &py);
			centerx = (double)px;
			centery = (double)py;
		}
	}

	else
	{
		if (LY_ZoneManagerServive::Get().m_pProj != nullptr)
		{
			dis = h / 2;
			j1 = angle - 90;
			LY_ZoneManagerServive::Get().m_pProj->A_X(x1, y1, dis, j1, dx1, dy1);
			j4 = angle + 90;
			LY_ZoneManagerServive::Get().m_pProj->A_X(x1, y1, dis, j4, dx4, dy4);

			dis = l ;
			j2 = j3 = angle;
			LY_ZoneManagerServive::Get().m_pProj->A_X(dx1, dy1, dis, j2, dx2, dy2);
			LY_ZoneManagerServive::Get().m_pProj->A_X(dx4, dy4, dis, j3, dx3, dy3);

			double dx5, dy5;
			int px, py;
			LY_ZoneManagerServive::Get().m_pProj->A_X(x1, y1, dis / 2, angle, dx5, dy5);
			LY_ZoneManagerServive::Get().m_pProj->xyLtoD(dx5, dy5, &px, &py);
			centerx = (double)px;
			centery = (double)py;
		}
	}
	float ll, bb;
	LY_ZoneManagerServive::Get().X_S(dx1, dy1, &ll, &bb);
	m_mPoints.push_back(QPointF(ll, bb));

	LY_ZoneManagerServive::Get().X_S(dx2, dy2, &ll, &bb);
	m_mPoints.push_back(QPointF(ll, bb));

	LY_ZoneManagerServive::Get().X_S(dx3, dy3, &ll, &bb);
	m_mPoints.push_back(QPointF(ll, bb));

	LY_ZoneManagerServive::Get().X_S(dx4, dy4, &ll, &bb);
	m_mPoints.push_back(QPointF(ll, bb));
}

void qnzkna::ZoneManage::IZoneManagerImpl::DrawFG(QPainter *pDC, int x1, int y1, int x2, int y2)
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

void qnzkna::ZoneManage::IZoneManagerImpl::Init3DZones()
{
	if (m_ZoneBaseVec_Activated.size() == 0)
	{
		return;
	}

	for (auto itr: m_ZoneBaseVec_Activated)
	{
		Add3DZone(itr);
		Add3DZoneSHPObject(itr);
	}
}

void qnzkna::ZoneManage::IZoneManagerImpl::Add3DZone(const CZoneBase &zone)
{
	if (zone.bShow == false)
	{
		return;
	}

	CCreateShape cs;

	IEarth *pEarth = IEarth::GetEarth();
	if (pEarth==nullptr)
	{
		return;
	}
	IGraphics *pGrp = pEarth->GetGraphics();
	if (pGrp == nullptr)
	{
		return;
	}
	IProj *pProj = LY_ZoneManagerServive::Get().GetProj();
	if (pProj == nullptr)
	{
		return;
	}

	QString strID = QString::fromLocal8Bit(zone.strID);
	((Qt::PenStyle)zone.nLineType);
	unsigned int uiV = zone.nLineColor;
	unsigned int color1 = RGBA(uiV & 0xFF, (uiV >> 8) & 0xFF, (uiV >> 16) & 0xFF, 60);
	unsigned int color2 = RGBA(uiV & 0xFF, (uiV >> 8) & 0xFF, (uiV >> 16) & 0xFF, 60);
	((Qt::BrushStyle)zone.nFillType);

	float  l, b;
	double centerX = 0, centerY = 0;
	double x, y;
	double dZ = zone.nBottomhAlt;
	double dM = zone.nAboveAlt - zone.nBottomhAlt;
	SHPObject *pShp;
	QString str, jwdstr;

	if (zone.nZoneType == CZoneBase::CIRCLE)
	{

		pShp = cs.CreateCircle(0, 0, zone.stRoundParameter.nRadius,NPrecision);
		LY_ZoneManagerServive::Get().L_X(zone.stRoundParameter.dLon, zone.stRoundParameter.dLat, &x, &y);

		if (pShp == nullptr)
		{
			return;
		}

		SHPObject *pShp1 = cs.AddZM2SHPObject(true, pShp, &dZ, &dM);
		IUserMesh2 *pUM = pGrp->CreateUserMesh2(pShp1);
		pUM->SetColor(color1);
		pUM->SetPosition(zone.stRoundParameter.dLon, zone.stRoundParameter.dLat, 0);
		pUM->HeighLightEdge();
		pUM->SetState(GL_DEPTH_TEST, 1);

		SHPDestroyObject(pShp1);

		auto itrMesh = m_UserMesh2Map.find(strID);
		if (itrMesh == m_UserMesh2Map.end())
		{
			m_UserMesh2Map.insert(std::make_pair(strID, pUM));
		}
		else
		{
			Remove3DZoneUserMesh(strID);
			itrMesh->second = pUM;
		}
	}
	else if (zone.nZoneType == CZoneBase::PIE)
	{
		LY_ZoneManagerServive::Get().L_X(zone.stRoundParameter.dLon, zone.stRoundParameter.dLat, &x, &y);
		pShp = cs.CreatePie(x, y, zone.stRoundParameter.nRadius, fmod(450 - zone.stRoundParameter.nEndAngel, 360), fmod(450 - zone.stRoundParameter.nStartAngel, 360), NPrecision, zone.nFillType);
		if (pShp == nullptr)
		{
			return;
		}
		for (int k = 0; k < pShp->nVertices; k++)
		{
			LY_ZoneManagerServive::Get().X_L(pShp->padfX[k], pShp->padfY[k], &pShp->padfX[k], &pShp->padfY[k]);
		}

		SHPObject *pShp1 = cs.AddZM2SHPObject(true, pShp, &dZ, &dM);
		IUserMesh1 *pUM = pGrp->CreateUserMesh1(pShp1);
		pUM->SetColor(color1);
		pUM->HeighLightEdge();
		pUM->SetState(GL_DEPTH_TEST, 1);

		SHPDestroyObject(pShp1);

		auto itrMesh = m_UserMesh1Map.find(strID);
		if (itrMesh == m_UserMesh1Map.end())
		{
			m_UserMesh1Map.insert(std::make_pair(strID, pUM));
		}
		else
		{
			Remove3DZoneUserMesh(strID);
			itrMesh->second = pUM;
		}
	}
	else if (zone.nZoneType == CZoneBase::ELLIPSE)
	{
		pShp = cs.CreateEllipse(0, 0, zone.stRoundParameter.nRaidusDuan, zone.stRoundParameter.nRaidusChang, NPrecision, zone.nFillType);
		if (pShp == nullptr)
		{
			return;
		}

		SHPObject *pShp1 = cs.AddZM2SHPObject(true, pShp, &dZ, &dM);
		IUserMesh2 *pUM = pGrp->CreateUserMesh2(pShp1);
		pUM->SetColor(color1);
		pUM->HeighLightEdge();
		pUM->SetPosition(zone.stRoundParameter.dLon, zone.stRoundParameter.dLat, 0);
		pUM->SetYawPitchRoll(zone.stRoundParameter.nAngelChang, 0, 0);
		pUM->SetState(GL_DEPTH_TEST, 1);

		SHPDestroyObject(pShp1);

		auto itrMesh = m_UserMesh2Map.find(strID);
		if (itrMesh == m_UserMesh2Map.end())
		{
			m_UserMesh2Map.insert(std::make_pair(strID, pUM));
		}
		else
		{
			Remove3DZoneUserMesh(strID);
			itrMesh->second = pUM;
		}
	}
	else if (zone.nZoneType == CZoneBase::CIRCLERING)
	{
		CShapeClip sc;
		sc.SetOperateType(SHAPE_DIFF);
		SHPObject *pShp1 = cs.CreateCircle(0, 0, zone.stRoundParameter.nRaidusChang, NPrecision);
		if (pShp1 == nullptr)
		{
			return;
		}

		SHPObject *pShp2 = cs.CreateCircle(0, 0, zone.stRoundParameter.nRaidusDuan, NPrecision);
		if (pShp2 == nullptr)
		{
			return;
		}

		SHPObject *pShp3 = sc.Clip(pShp1, pShp2);
		if (pShp3 == nullptr)
		{
			return;
		}

		SHPObject *pShp4 = cs.AddZM2SHPObject(true, pShp3, &dZ, &dM);
		IUserMesh2 *pUM = pGrp->CreateUserMesh2(pShp4);
		pUM->SetColor(color1);
		pUM->SetPosition(zone.stRoundParameter.dLon, zone.stRoundParameter.dLat, 0);

		pUM->HeighLightEdge();
		pUM->SetState(GL_DEPTH_TEST, 1);

		SHPDestroyObject(pShp1);
		SHPDestroyObject(pShp2);
		SHPDestroyObject(pShp4);

		auto itrMesh = m_UserMesh2Map.find(strID);
		if (itrMesh == m_UserMesh2Map.end())
		{
			m_UserMesh2Map.insert(std::make_pair(strID, pUM));
		}
		else
		{
			Remove3DZoneUserMesh(strID);
			itrMesh->second = pUM;
		}
	}
	else if (zone.nZoneType == CZoneBase::PIERING)
	{
		CShapeClip sc;
		sc.SetOperateType(SHAPE_DIFF);

		SHPObject *pShp1 = cs.CreatePie(0, 0, zone.stRoundParameter.nRaidusChang, fmod(450 - zone.stRoundParameter.nEndAngel, 360), fmod(450 - zone.stRoundParameter.nStartAngel, 360),NPrecision);
		if (pShp1 == nullptr)
		{
			return;
		}

		SHPObject *pShp2 = cs.CreatePie(0, 0, zone.stRoundParameter.nRaidusDuan, fmod(450 - zone.stRoundParameter.nEndAngel, 360), fmod(450 - zone.stRoundParameter.nStartAngel, 360), NPrecision);
		if (pShp2 == nullptr)
		{
			return;
		}

		SHPObject *pShp3 = sc.Clip(pShp1, pShp2);
		if (pShp3 == nullptr)
		{
			return;
		}

		SHPObject *pShp4 = cs.AddZM2SHPObject(true, pShp3, &dZ, &dM);
		IUserMesh2 *pUM = pGrp->CreateUserMesh2(pShp4);
		pUM->SetColor(color1);
		pUM->HeighLightEdge();
		pUM->SetState(GL_DEPTH_TEST, 1);
		pUM->SetPosition(zone.stRoundParameter.dLon, zone.stRoundParameter.dLat, 0);

		SHPDestroyObject(pShp1);
		SHPDestroyObject(pShp2);
		SHPDestroyObject(pShp4);

		auto itrMesh = m_UserMesh2Map.find(strID);
		if (itrMesh == m_UserMesh2Map.end())
		{
			m_UserMesh2Map.insert(std::make_pair(strID, pUM));
		}
		else
		{
			Remove3DZoneUserMesh(strID);
			itrMesh->second = pUM;
		}
	}
	else if (zone.nZoneType == CZoneBase::POLY )
	{
		if (zone.stZonePointVec.size()>=100)
		{
			return;
		}
		DPOINT pDPOINT[100] = {};
		int nNo = 0;
		for (auto itrPoint :zone.stZonePointVec)
		{
			pDPOINT[nNo].x = itrPoint.dLon;
			pDPOINT[nNo].y = itrPoint.dLat;
			nNo++;
		}

		bool bConterClock = isconterclock(zone.stZonePointVec.size(), (DPOINT*)pDPOINT);
		std::vector<double> vX;
		std::vector<double> vY;
		std::vector<double> vZ;
		vX.reserve(zone.stZonePointVec.size() + 1);
		vY.reserve(zone.stZonePointVec.size() + 1);
		vZ.reserve(zone.stZonePointVec.size() + 1);
		if (!bConterClock)
		{
			for (auto itrPoint : zone.stZonePointVec)
			{
				vX.push_back(itrPoint.dLon);
				vY.push_back(itrPoint.dLat);
				vZ.push_back(s_CGetHeight.GetHeight(itrPoint.dLon, itrPoint.dLat));
			}
		}
		else
		{
			auto itr = zone.stZonePointVec.rbegin();
			for (;itr!= zone.stZonePointVec.rend();itr++)
			{
				vX.push_back(itr->dLon);
				vY.push_back(itr->dLat);
				vZ.push_back(s_CGetHeight.GetHeight(itr->dLon, itr->dLat));
			}
		}
#if 1
		vX.push_back(vX[0]);
		vY.push_back(vY[0]);
		vY.push_back(vZ[0]);

		pShp = SHPCreateSimpleObject(SHPT_POLYGON, vX.size(),vX.data(), vY.data(), vZ.data());
		if (pShp == nullptr)
		{
			return;
		}

		SHPObject *pShp1 = cs.AddZM2SHPObject(true, pShp, &dZ, &dM);

		IUserMesh1 *pUM = pGrp->CreateUserMesh1(pShp1);
		pUM->SetColor(color1);
		pUM->HeighLightEdge();
		pUM->SetState(GL_DEPTH_TEST, 1);

		SHPDestroyObject(pShp1);

		auto itrMesh = m_UserMesh1Map.find(strID);
		if (itrMesh == m_UserMesh1Map.end())
		{
			m_UserMesh1Map.insert(std::make_pair(strID, pUM));
		}
		else
		{
			Remove3DZoneUserMesh(strID);
			itrMesh->second = pUM;
		}
#else
		IUserMesh1 *pUM = nullptr;
		SHPObject *pShp = cs.CreatePolygon(vX.data(), vY.data(), vX.size(),
			100, true, nullptr, vZ.data());
		if (pShp != nullptr)
		{
			pUM = pEarth->GetGraphics()->CreateUserMesh1(pShp);
			if (pUM != nullptr)
			{
				pUM->SetColor(color1);
				pUM->SetState(IUserMesh::POLYGONMODE, IUserMesh::USERMESH_FILL);
				pUM->SetState(IUserMesh::LINEWIDTH, 2);
			}
		}

#endif
	}
	else if (zone.nZoneType == CZoneBase::LINE)
	{
#if 0
		std::vector<double> vX;
		std::vector<double> vY;

		for (auto itrPoint : zone.stZonePointVec)
		{
			vX.push_back(itrPoint.dLon);
			vY.push_back(itrPoint.dLat);
		}

		pShp = SHPCreateSimpleObject(SHPT_ARC, zone.stZonePointVec.size(),vX.data(), vY.data(), NULL);
		SHPObject *pShp1 = cs.AddZM2SHPObject(false, pShp, &dZ, &dM);
		IUserMesh1 *pUM = pGrp->CreateUserMesh1(pShp1);
		pUM->SetColor(color2);
		it->second.push_back(pUM);
		SHPDestroyObject(pShp1);

		auto itrMesh = m_UserMesh1Map.find(strID);
		if (itrMesh == m_UserMesh1Map.end())
		{
			m_UserMesh1Map.insert(std::make_pair(strID, pUM));
		}
		else
		{
			pEarth->GetGraphics()->RemoveObject(itrMesh->second);
			itrMesh->second = pUM;
		}
#else
		IAirLine* pIAirLine = pEarth->GetGraphics()->CreateAirLine();

		AirLineData airLine;

		airLine.cLineWidth = 3;
		if (m_nDiaphaneity == -1)
		{
			airLine.uColor = color1;
		}
		else
		{
			color1 = RGBA(uiV & 0xFF, (uiV >> 8) & 0xFF, (uiV >> 16) & 0xFF, m_nDiaphaneity);
			airLine.uColor = color1;
		}
		airLine.nRoadHeight = (zone.nAboveAlt - zone.nBottomhAlt)*m_dHeightScale;
		airLine.nRoadWidth = zone.nLineWidth * m_dWidthScale;

		for (auto itrPoint : zone.stZonePointVec)
		{
			AirLineDot dot;
			dot.dLon = itrPoint.dLon;
			dot.dLat = itrPoint.dLat;
			dot.fH = zone.nBottomhAlt;
			dot.fB = 15;
			dot.fV = 10;

			airLine.vAirLineDot.push_back(dot);
		}

		if (!pIAirLine) {
			return;
		}

		SHPObject *pSHPObject =pIAirLine->SetPoints(airLine);

		if (!pSHPObject) {
			return;
		}

		for (int k = 0; k < pSHPObject->nVertices; k++)
		{
			LY_ZoneManagerServive::Get().L_X(pSHPObject->padfX[k], pSHPObject->padfY[k], &pSHPObject->padfX[k], &pSHPObject->padfY[k]);
		}

		UpdateLocalSHPObject(strID, pSHPObject);
		auto itrMesh = m_UserLineMap.find(strID);
		if (itrMesh == m_UserLineMap.end())
		{
			m_UserLineMap.insert(std::make_pair(strID, pIAirLine));
		}
		else
		{
			Remove3DZoneUserMesh(strID);
			itrMesh->second = pIAirLine;
		}
#endif
	}
	else if (zone.nZoneType == CZoneBase::RECTANGLE)
	{
		double dX[5], dY[5];
		double x1 = zone.stRoundParameter.nRaidusDuan * 0.5;
		double y1 = zone.stRoundParameter.nRaidusChang * 0.5;
		double x2 = zone.stRoundParameter.nRaidusDuan;
		double y2 = zone.stRoundParameter.nRaidusChang * 0.5;
		if (zone.stRoundParameter.nCornerRadius == 0)
		{
			dX[4] = -x1;
			dY[4] = -y1;
			dX[0] = -x1;
			dY[0] = -y1;
			dX[3] = x1;
			dY[3] = -y1;
			dX[2] = x1;
			dY[2] = y1;
			dX[1] = -x1;
			dY[1] = y1;
		}
		else
		{
			dX[4] = 0;
			dY[4] = -y1;
			dX[0] = 0;
			dY[0] = -y1;
			dX[3] = x2;
			dY[3] = -y1;
			dX[2] = x2;
			dY[2] = y1;
			dX[1] = 0;
			dY[1] = y1;
		}
		pShp = cs.CreatePolygon(dX, dY, 5);
		if (pShp == nullptr)
		{
			return;
		}

		SHPObject *pShp1 = cs.AddZM2SHPObject(true, pShp, &dZ, &dM);
		IUserMesh2 *pUM = pGrp->CreateUserMesh2(pShp1);
		pUM->SetColor(color1);
		pUM->HeighLightEdge();
		pUM->SetPosition(zone.stRoundParameter.dLon, zone.stRoundParameter.dLat, 0);
		pUM->SetYawPitchRoll(zone.stRoundParameter.nNorthAngel, 0, 0);
		pUM->SetState(GL_DEPTH_TEST, 1);

		SHPDestroyObject(pShp1);

		auto itrMesh = m_UserMesh2Map.find(strID);
		if (itrMesh == m_UserMesh2Map.end())
		{
			m_UserMesh2Map.insert(std::make_pair(strID, pUM));
		}
		else
		{
			Remove3DZoneUserMesh(strID);
			itrMesh->second = pUM;
		}
	}
	else if (zone.nZoneType == CZoneBase::POINT)
	{
#if 0
		IOsgShape* pIOsgShape = pEarth->GetGraphics()->CreateShape(IOsgShape::ISHAPE_SPHERE,1000,1000);
		if (pIOsgShape != nullptr)
		{
			pIOsgShape->SetColor(color2);
			pIOsgShape->SetPosition(zone.stRoundParameter.dLon, zone.stRoundParameter.dLat, dZ);
			pIOsgShape->SetYawPitchRoll(zone.stRoundParameter.nNorthAngel, 0, 0);
		}
#else
		IUserMesh2 *pUM = pGrp->CreateUserMesh2();
		Draw3DGlobe(pUM,10000,4);
		pUM->SetColor(color2);
		pUM->SetPosition(zone.stRoundParameter.dLon, zone.stRoundParameter.dLat, dZ);
		pUM->SetState(GL_DEPTH_TEST, 1);

		auto itrMesh = m_UserMesh2Map.find(strID);
		if (itrMesh == m_UserMesh2Map.end())
		{
			m_UserMesh2Map.insert(std::make_pair(strID, pUM));
		}
		else
		{
			Remove3DZoneUserMesh(strID);
			itrMesh->second = pUM;
		}
#endif
	}
}

void qnzkna::ZoneManage::IZoneManagerImpl::Add3DZoneSHPObject(const CZoneBase &zone)
{
	CCreateShape csTmp;

	IEarth *pEarth = IEarth::GetEarth();
	if (pEarth == nullptr)
	{
		return;
	}
	IGraphics *pGrp = pEarth->GetGraphics();
	if (pGrp == nullptr)
	{
		return;
	}
	IProj *pProj = LY_ZoneManagerServive::Get().GetProj();
	if (pProj == nullptr)
	{
		return;
	}

	QString strID = QString::fromLocal8Bit(zone.strID);
	double x, y;
	SHPObject *pShp;
	QString str, jwdstr;
	if (zone.nZoneType == CZoneBase::CIRCLE)
	{
		LY_ZoneManagerServive::Get().L_X(zone.stRoundParameter.dLon, zone.stRoundParameter.dLat, &x, &y);
		pShp = csTmp.CreateCircle(x, y, zone.stRoundParameter.nRadius, NPrecision);
		if (pShp == nullptr)
		{
			return;
		}
		UpdateLocalSHPObject(strID, pShp);
	}
	else if (zone.nZoneType == CZoneBase::PIE)
	{
		LY_ZoneManagerServive::Get().L_X(zone.stRoundParameter.dLon, zone.stRoundParameter.dLat, &x, &y);
		pShp = csTmp.CreatePie(x, y, zone.stRoundParameter.nRadius, fmod(450 - zone.stRoundParameter.nEndAngel, 360), fmod(450 - zone.stRoundParameter.nStartAngel, 360), NPrecision, zone.nFillType);
		if (pShp == nullptr)
		{
			return;
		}
		for (int k = 0; k < pShp->nVertices; k++)
		{
			LY_ZoneManagerServive::Get().X_L(pShp->padfX[k], pShp->padfY[k], &pShp->padfX[k], &pShp->padfY[k]);
		}
		UpdateLocalSHPObject(strID, pShp);
	}
	else if (zone.nZoneType == CZoneBase::ELLIPSE)
	{
		LY_ZoneManagerServive::Get().L_X(zone.stRoundParameter.dLon, zone.stRoundParameter.dLat, &x, &y);
		pShp = csTmp.CreateEllipse(x, y, zone.stRoundParameter.nRaidusChang, zone.stRoundParameter.nRaidusDuan, NPrecision, zone.nFillType);
		double dDistance, dAngle;
		for (int k = 0; k < pShp->nVertices; k++)
		{

			pProj->X_A(x, y, pShp->padfX[k], pShp->padfY[k], dDistance, dAngle);
			pProj->A_X(x, y, dDistance, dAngle - zone.stRoundParameter.nAngelChang, pShp->padfX[k], pShp->padfY[k]);
		}
		UpdateLocalSHPObject(strID, pShp);
	}
	else if (zone.nZoneType == CZoneBase::CIRCLERING)
	{
		CShapeClip scTmp;
		scTmp.SetOperateType(SHAPE_DIFF);
		LY_ZoneManagerServive::Get().L_X(zone.stRoundParameter.dLon, zone.stRoundParameter.dLat, &x, &y);
		SHPObject *pShpTemp1 = csTmp.CreateCircle(x, y, zone.stRoundParameter.nRaidusChang, NPrecision);
		SHPObject *pShpTemp2 = csTmp.CreateCircle(x, y, zone.stRoundParameter.nRaidusDuan, NPrecision);
		SHPObject *pShpTemp3 = scTmp.Clip(pShpTemp1, pShpTemp2);
		UpdateLocalSHPObject(strID, pShpTemp3);
		SHPDestroyObject(pShpTemp1);
		SHPDestroyObject(pShpTemp2);
	}
	else if (zone.nZoneType == CZoneBase::PIERING)
	{
		CShapeClip scTmp;
		scTmp.SetOperateType(SHAPE_DIFF);
		LY_ZoneManagerServive::Get().L_X(zone.stRoundParameter.dLon, zone.stRoundParameter.dLat, &x, &y);
		SHPObject *pShpTemp1 = csTmp.CreatePie(x, y, zone.stRoundParameter.nRaidusChang, fmod(450 - zone.stRoundParameter.nEndAngel, 360), fmod(450 - zone.stRoundParameter.nStartAngel, 360), NPrecision);
		SHPObject *pShpTemp2 = csTmp.CreatePie(x, y, zone.stRoundParameter.nRaidusDuan, fmod(450 - zone.stRoundParameter.nEndAngel, 360), fmod(450 - zone.stRoundParameter.nStartAngel, 360), NPrecision);
		SHPObject *pShpTemp3 = scTmp.Clip(pShpTemp1, pShpTemp2);
		UpdateLocalSHPObject(strID, pShpTemp3);
		SHPDestroyObject(pShpTemp1);
		SHPDestroyObject(pShpTemp2);
	}
	else if (zone.nZoneType == CZoneBase::POLY)
	{
		if (zone.stZonePointVec.size() >= 100)
		{
			return;
		}
		DPOINT pDPOINT[100] = {};
		int nNo = 0;
		for (auto itrPoint : zone.stZonePointVec)
		{
			pDPOINT[nNo].x = itrPoint.dLon;
			pDPOINT[nNo].y = itrPoint.dLat;
			nNo++;
		}

		bool bConterClock = isconterclock(zone.stZonePointVec.size(), (DPOINT*)pDPOINT);
		std::vector<double> vX;
		std::vector<double> vY;
		std::vector<double> vZ;
		vX.reserve(zone.stZonePointVec.size() + 1);
		vY.reserve(zone.stZonePointVec.size() + 1);
		vZ.reserve(zone.stZonePointVec.size() + 1);
		if (!bConterClock)
		{
			for (auto itrPoint : zone.stZonePointVec)
			{
				vX.push_back(itrPoint.dLon);
				vY.push_back(itrPoint.dLat);
				vZ.push_back(s_CGetHeight.GetHeight(itrPoint.dLon, itrPoint.dLat));
			}
		}
		else
		{
			auto itr = zone.stZonePointVec.rbegin();
			for (; itr != zone.stZonePointVec.rend(); itr++)
			{
				vX.push_back(itr->dLon);
				vY.push_back(itr->dLat);
				vZ.push_back(s_CGetHeight.GetHeight(itr->dLon, itr->dLat));
			}
		}
		vX.push_back(vX[0]);
		vY.push_back(vY[0]);
		vY.push_back(vZ[0]);

		SHPObject *pShpTmp = SHPCreateSimpleObject(SHPT_POLYGON, vX.size(), vX.data(), vY.data(), NULL);
		for (int k = 0; k < pShpTmp->nVertices; k++)
		{
			LY_ZoneManagerServive::Get().L_X(pShpTmp->padfX[k], pShpTmp->padfY[k], &pShpTmp->padfX[k], &pShpTmp->padfY[k]);
		}
		UpdateLocalSHPObject(strID, pShpTmp);
	}
	else if (zone.nZoneType == CZoneBase::LINE)
	{

	}
	else if (zone.nZoneType == CZoneBase::RECTANGLE)
	{
		double dX[5], dY[5];
		double x1 = zone.stRoundParameter.nRaidusDuan * 0.5;
		double y1 = zone.stRoundParameter.nRaidusChang * 0.5;
		double x2 = zone.stRoundParameter.nRaidusDuan;
		double y2 = zone.stRoundParameter.nRaidusChang * 0.5;
		if (zone.stRoundParameter.nCornerRadius == 0)
		{
			dX[4] = -x1;
			dY[4] = -y1;
			dX[0] = -x1;
			dY[0] = -y1;
			dX[3] = x1;
			dY[3] = -y1;
			dX[2] = x1;
			dY[2] = y1;
			dX[1] = -x1;
			dY[1] = y1;
		}
		else
		{
			dX[4] = 0;
			dY[4] = -y1;
			dX[0] = 0;
			dY[0] = -y1;
			dX[3] = x2;
			dY[3] = -y1;
			dX[2] = x2;
			dY[2] = y1;
			dX[1] = 0;
			dY[1] = y1;
		}

		SHPObject *pShpTmp = csTmp.CreatePolygon(dX, dY, 5);
		double dDistance, dAngle;
		LY_ZoneManagerServive::Get().L_X(zone.stRoundParameter.dLon, zone.stRoundParameter.dLat, &x, &y);
		for (int k = 0; k < pShpTmp->nVertices; k++)
		{

			pProj->X_A(0, 0, pShpTmp->padfX[k], pShpTmp->padfY[k], dDistance, dAngle);
			pProj->A_X(x, y, dDistance, dAngle + zone.stRoundParameter.nNorthAngel, pShpTmp->padfX[k], pShpTmp->padfY[k]);
		}
		UpdateLocalSHPObject(strID, pShpTmp);
	}
	else if (zone.nZoneType == CZoneBase::POINT)
	{

	}
}

void qnzkna::ZoneManage::IZoneManagerImpl::Remove3DZone(const QString &strID)
{
	Remove3DZoneUserMesh(strID);

	auto& itr1 = m_UserMesh1Map.find(strID);
	if (itr1 != m_UserMesh1Map.end())
	{
		m_UserMesh1Map.erase(itr1);
	}

	auto& itr2 = m_UserMesh2Map.find(strID);
	if (itr2 != m_UserMesh2Map.end())
	{
		m_UserMesh2Map.erase(itr2);
	}

	auto& itr3 = m_UserLineMap.find(strID);
	if (itr3 != m_UserLineMap.end())
	{
		m_UserLineMap.erase(itr3);
	}
}

void qnzkna::ZoneManage::IZoneManagerImpl::Remove3DZoneUserMesh(const QString &strID)
{
	IEarth *pEarth = IEarth::GetEarth();
	if (pEarth == nullptr)
	{
		return;
	}

	auto& itr1 = m_UserMesh1Map.find(strID);
	if (itr1 != m_UserMesh1Map.end())
	{
		pEarth->GetGraphics()->RemoveObject(itr1->second);
	}

	auto& itr2 = m_UserMesh2Map.find(strID);
	if (itr2 != m_UserMesh2Map.end())
	{
		pEarth->GetGraphics()->RemoveObject(itr2->second);
	}

	auto& itr3 = m_UserLineMap.find(strID);
	if (itr3 != m_UserLineMap.end())
	{
		pEarth->GetGraphics()->RemoveObject(itr3->second);
	}
}

void qnzkna::ZoneManage::IZoneManagerImpl::Draw3DGlobe(IUserMesh2 *pUM ,double r, int precision )
{
	if (pUM == nullptr)
	{
		return;
	}

	int i, j;
	int m = 4 * precision;
	int n = 8 * precision;
	std::vector<VERTEX> v;
	v.push_back({ (float)0, (float)0, (float)r });
	double dDeltaZ = r / m;
	double dDeltaA = 2 * PI / n;
	for (i = 1; i <= m; i++)
	{
		float z = r - dDeltaZ*i;
		double dR = sqrt(r*r - z*z);
		for (j = 0; j < n; j++)
		{
			double dA = dDeltaA*j;
			float x = dR * cos(dA);
			float y = dR * sin(dA);
			v.push_back({ x, y, z });
		}
	}

	for (i = 1; i < m; i++)
	{
		float z = -dDeltaZ*i;
		double dR = sqrt(r*r - z*z);
		for (j = 0; j < n; j++)
		{
			double dA = dDeltaA*j;
			float x = dR * cos(dA);
			float y = dR * sin(dA);
			v.push_back({ x, y, z });
		}
	}
	v.push_back({ (float)0, (float)0, (float)-r });

	pUM->SetPoints(v.size(), v.data());
	std::vector<unsigned short> vs(n + 2);
	for (i = 0; i <= n; i++)
	{
		vs[i] = i;
	}
	vs[n + 1] = 1;
	pUM->SetIndexs(vs.size(), vs.data(), IUserMesh::USERMESH_TRIANGLE_FAN, false);

	for (i = 0; i <= n; i++)
	{
		vs[i] = v.size() - 1 - i;
	}
	vs[n + 1] = v.size() - 2;
	pUM->SetIndexs(vs.size(), vs.data(), IUserMesh::USERMESH_TRIANGLE_FAN, false);

	m--;
	m <<= 1;
	vs.resize((1 + n) << 1);
	for (i = 0; i < m; i++)
	{
		int a = 1 + i * n;
		int b = 1 + (i + 1)*n;
		int c = a;
		int d = b;
		int k = 0;
		for (j = 0; j < n; j++)
		{
			vs[k++] = a++;
			vs[k++] = b++;
		}
		vs[k++] = c;
		vs[k++] = d;
		pUM->SetIndexs(k, vs.data(), IUserMesh::USERMESH_QUAD_STRIP, (i == m - 1) ? true : false);
	}

	pUM->HeighLightEdge();
}

void qnzkna::ZoneManage::IZoneManagerImpl::LoadZoneBaseAdditionFormDB()
{
	if (!m_db.isOpen())
	{
		LyMessageBox::warning(nullptr, tr2("警告"), tr2("数据库连接失败!"));
		return;
	}
	m_CZoneBaseAdditionMap.clear();
	QSqlQuery query;
	QString strSQL = "select QYNM,XDBH,KSSJ,JSSJ from `t_zone_time` order by QYNM";
	bool success = query.exec(strSQL);
	if (success)
	{
		while (query.next())
		{
			QString str1 = tr2(query.value(0).toString().toLocal8Bit().data());
			QString str2 = tr2(query.value(1).toString().toLatin1().data());
			QString str3 = tr2(query.value(2).toString().toLatin1().data());
			QString str4 = tr2(query.value(3).toString().toLatin1().data());

			CZoneBaseAddition stZoneBase;
			strncpy(stZoneBase.strID, str1.toLocal8Bit(), MAX_ZONE_LEN_48);
			stZoneBase.nActionID = str2.toInt();
			stZoneBase.lStartTime = str3.toLongLong();
			stZoneBase.lEndTime = str4.toLongLong();

			auto itr = m_CZoneBaseAdditionMap.find(stZoneBase.nActionID);
			if (itr == m_CZoneBaseAdditionMap.end())
			{
				CZoneBaseAdditionVec stZoneBaseVec;
				stZoneBaseVec.push_back(stZoneBase);

				m_CZoneBaseAdditionMap.insert(std::make_pair(stZoneBase.nActionID, stZoneBaseVec));
			}
			else
			{
				itr->second.push_back(stZoneBase);
			}
		}
	}
}

bool qnzkna::ZoneManage::IZoneManagerImpl::AddZoneBaseAdditionToDB(const CZoneBaseAddition& stData)
{
	QString strSql = "insert into `Agri`.`t_zone_time`(QYNM, XDBH, KSSJ, JSSJ) values(";
	strSql += "'" + QString::fromLocal8Bit(stData.strID) + "',";
	strSql +=		QString::number(stData.nActionID) + ",";
	strSql +=		QString::number(stData.lStartTime) + ",";
	strSql +=		QString::number(stData.lEndTime);
	strSql += ")";
	QSqlQuery query;
	return query.exec(strSql);
}

bool qnzkna::ZoneManage::IZoneManagerImpl::RemoveZoneBaseAdditionToDB(const QString& strID, const int& nActionID)
{
	QSqlQuery query;
	QString strSQL = "delete from `Agri`.`t_zone_time` where QYNM ='";
	strSQL += strID + "' AND XDBH = ";
	strSQL += QString::number(nActionID);
	return query.exec(strSQL);
}

void qnzkna::ZoneManage::IZoneManagerImpl::UpdateLocalSHPObject(const QString &strID, SHPObject* pSHPObject)
{

	auto itr = m_ZoneSHPObjectMap.find(strID);
	if (itr == m_ZoneSHPObjectMap.end())
	{
		m_ZoneSHPObjectMap.insert(std::make_pair(strID, pSHPObject));
	}
	else
	{
		SHPDestroyObject(itr->second);
		itr->second = pSHPObject;
	}
}

void qnzkna::ZoneManage::IZoneManagerImpl::RemoveLocalSHPObject(const QString &strID)
{

	auto itr = m_ZoneSHPObjectMap.find(strID);
	if (itr != m_ZoneSHPObjectMap.end())
	{
		SHPDestroyObject(itr->second);
		m_ZoneSHPObjectMap.erase(itr);
	}
}

bool qnzkna::ZoneManage::IZoneManagerImpl::GetLocalSHPObject(const QString &strID, SHPObject& stSHPObject)
{

	auto itr = m_ZoneSHPObjectMap.find(strID);
	if (itr != m_ZoneSHPObjectMap.end())
	{
		stSHPObject = *itr->second;
		return true;
	}
	else
	{
		return false;
	}
}

IUserMesh2 * qnzkna::ZoneManage::IZoneManagerImpl::CreateRectMesh(IGraphics *pGrp, int x, int y, int z)
{
	int i, j, k, n;
	n = (x + y)*(z + 1) + ((x - 1)*(y - 1));
	n <<= 1;
	std::vector<VERTEX> v;
	v.reserve(n);

	for (k = 0; k <= z; k++)
	{
		for (i = 0; i <= x; i++)
		{
			v.push_back({ (float)i, (float)0, (float)k });
			v.push_back({ (float)i, (float)y, (float)k });
		}
		for (j = 1; j < y; j++)
		{
			v.push_back({ (float)0, (float)j, (float)k });
			v.push_back({ (float)x, (float)j, (float)k });
		}
	}
	for (i = 1; i < x; i++)
	{
		for (j = 1; j < y; j++)
		{
			v.push_back({ (float)i, (float)j, (float)z });
			v.push_back({ (float)i, (float)j, (float)0 });
		}
	}

	IUserMesh2 *pUM = pGrp->CreateUserMesh2();
	pUM->SetPoints(v.size(), v.data());

	j = (x + y) << 1;
	k = x << 1;
	n = j*z;
	std::vector<unsigned short> vs;
	vs.reserve(v.size() + (z + 1) * 4 + j * 2);
	for (i = 0; i < v.size(); i++)
	{
		vs.push_back(i);
	}
	for (i = 0; i <= z; i++)
	{
		vs.push_back(i*j + 0);
		vs.push_back(i*j + k);
		vs.push_back(i*j + 1);
		vs.push_back(i*j + k + 1);
	}
	for (i = 0; i < j; i++)
	{
		vs.push_back(i);
		vs.push_back(i + n);
	}
	pUM->SetIndexs(vs.size(), vs.data(), IUserMesh::USERMESH_LINES, false);

	vs.clear();
	vs.reserve(8);
	vs.push_back(0);
	vs.push_back(1);
	vs.push_back(k + 1);
	vs.push_back(k);
	vs.push_back(n + 0);
	vs.push_back(n + k);
	vs.push_back(n + k + 1);
	vs.push_back(n + 1);
	pUM->SetIndexs(vs.size(), vs.data(), IUserMesh::USERMESH_QUADS, false);

	vs.clear();
	vs.reserve(10);
	vs.push_back(n);
	vs.push_back(0);
	vs.push_back(k + n);
	vs.push_back(k);
	vs.push_back(k + n + 1);
	vs.push_back(k + 1);
	vs.push_back(n + 1);
	vs.push_back(1);
	vs.push_back(n);
	vs.push_back(0);
	pUM->SetIndexs(vs.size(), vs.data(), IUserMesh::USERMESH_QUAD_STRIP, true);

	return pUM;
}

IUserMesh2 * qnzkna::ZoneManage::IZoneManagerImpl::SetCubeColor(IGraphics *pGrp, int x, int y, int z, unsigned int color)
{
	int i;
	std::vector<VERTEX> v;
	v.reserve(8);

	v.push_back({ (float)x, (float)y, (float)z });
	v.push_back({ (float)(x + 1), (float)y, (float)z });
	v.push_back({ (float)(x + 1), (float)(y + 1), (float)z });
	v.push_back({ (float)x, (float)(y + 1), (float)z });
	v.push_back({ (float)x, (float)y, (float)(z + 1) });
	v.push_back({ (float)(x + 1), (float)y, (float)(z + 1) });
	v.push_back({ (float)(x + 1), (float)(y + 1), (float)(z + 1) });
	v.push_back({ (float)x, (float)(y + 1), (float)(z + 1) });

	IUserMesh2 *pUM = pGrp->CreateUserMesh2();
	pUM->SetPoints(v.size(), v.data());

	std::vector<unsigned short> vs;
	vs.clear();
	vs.reserve(8);
	vs.push_back(3);
	vs.push_back(2);
	vs.push_back(1);
	vs.push_back(0);
	vs.push_back(4);
	vs.push_back(5);
	vs.push_back(6);
	vs.push_back(7);
	pUM->SetIndexs(vs.size(), vs.data(), IUserMesh::USERMESH_QUADS, false);

	vs.clear();
	vs.reserve(10);
	vs.push_back(4);
	vs.push_back(0);
	vs.push_back(5);
	vs.push_back(1);
	vs.push_back(6);
	vs.push_back(2);
	vs.push_back(7);
	vs.push_back(3);
	vs.push_back(4);
	vs.push_back(0);
	pUM->SetIndexs(vs.size(), vs.data(), IUserMesh::USERMESH_QUAD_STRIP, true);
	pUM->SetColor(color);

	return pUM;
}

bool qnzkna::ZoneManage::IZoneManagerImpl::JudgeAltitudeintercept(const QString &strZoneID1, const QString &strZoneID2, double& dZ, double& dM)
{
	CZoneBase stZoneBase1, stZoneBase2;
	bool success = GetZoneBase(strZoneID1, stZoneBase1);
	if (success == false)
	{
		return false;
	}

	success = GetZoneBase(strZoneID2, stZoneBase2);
	if (success == false)
	{
		return false;
	}

	int nMaxBottomhAlt = std::max(stZoneBase1.nBottomhAlt, stZoneBase2.nBottomhAlt);
	int nMinAboveAlt = std::min(stZoneBase1.nAboveAlt, stZoneBase2.nAboveAlt);
	int nIntersect = nMinAboveAlt - nMaxBottomhAlt;
	if (nIntersect < 0)
	{
		return false;
	}
	else
	{
		dZ = nMaxBottomhAlt;
		dM = nIntersect;
		return true;
	}
}

void qnzkna::ZoneManage::IZoneManagerImpl::DrawQYZoneData(QPainter *painter)
{
	painter->save();
	painter->setRenderHint(QPainter::Antialiasing, true);

	for (auto itrZone : m_ZoneBaseVec_Activated)
	{
		if (itrZone.bShow == false)
		{
			continue;
		}
		QPen m_penCur;
		m_penCur.setStyle((Qt::PenStyle)itrZone.nLineType);
		m_penCur.setWidth(2);

		unsigned int uiV = itrZone.nLineColor;
		QColor color = QColor(uiV & 0xFF, (uiV >> 8) & 0xFF, (uiV >> 16) & 0xFF);
		color.setAlpha(100);
		m_penCur.setColor(color);

		QBrush brush;
		brush.setStyle((Qt::BrushStyle)itrZone.nFillType);
		brush.setColor(color);
		painter->setBrush(brush);
		painter->setFont(QFont("MS Serif", 11, QFont::Bold));
		painter->setPen(m_penCur);

		DrawOneQuYu(painter, itrZone);
	}

	painter->restore();

	if (m_replanAirwayDraw) {
		painter->save();
		painter->setRenderHint(QPainter::Antialiasing, true);

		QPen m_penCur;
		m_penCur.setStyle((Qt::PenStyle)m_replanAirwayZone.nLineType);
		m_penCur.setWidth(2);

		QColor color(Qt::blue);
		color.setAlpha(100);
		m_penCur.setColor(color);

		QBrush brush;
		brush.setStyle((Qt::BrushStyle)m_replanAirwayZone.nFillType);
		brush.setColor(color);
		painter->setBrush(brush);
		painter->setFont(QFont("MS Serif", 11, QFont::Bold));
		painter->setPen(m_penCur);

		DrawOneQuYu(painter, m_replanAirwayZone);

		painter->restore();
	}
}

void qnzkna::ZoneManage::IZoneManagerImpl::SetActivatedAciton(const int& nActionID)
{
	m_nActivatedAcitonID = nActionID;

	m_ZoneBaseVec_Activated.clear();
	auto itr = m_CZoneBaseAdditionMap.find(nActionID);
	if (itr != m_CZoneBaseAdditionMap.end())
	{
		for (auto itrZone : itr->second)
		{
			for (auto itrZoneBase :m_ZoneBaseVec_All)
			{
				if (0 == strcmp(itrZone.strID,itrZoneBase.strID))
				{
					m_ZoneBaseVec_Activated.push_back(itrZoneBase);
				}
			}
		}
	}
	Init3DZones();
}

CZoneBaseVec qnzkna::ZoneManage::IZoneManagerImpl::GetAllZoneBase()
{
	return m_ZoneBaseVec_Activated;
}

qnzkna::ZoneManage::CZoneBaseVec qnzkna::ZoneManage::IZoneManagerImpl::GetAllZoneBase(const int& nType)
{
	CZoneBaseVec ZoneVec;
	for (auto itr:m_ZoneBaseVec_Activated)
	{
		if (nType == itr.nZoneType)
		{
			ZoneVec.push_back(itr);
		}
	}
	return ZoneVec;
}

bool qnzkna::ZoneManage::IZoneManagerImpl::GetZoneBase(const QString& strID, CZoneBase& stData)
{
	for ( auto itr: m_ZoneBaseVec_Activated)
	{
		if (0 == strcmp(strID.toLocal8Bit(),itr.strID))
		{
			stData = itr;
			return true;
		}
	}
	return false;
}

bool qnzkna::ZoneManage::IZoneManagerImpl::AddZoneBase(const CZoneBase& stData, const long long& llStartTime, const long long& llEndTime )
{
	QString strID = QString::fromLocal8Bit(stData.strID);
	if (IsZoneBaseIDExisted(strID))
	{
		return false;
	}
	else
	{
		m_ZoneBaseVec_Activated.push_back(stData);
		Add3DZone(stData);
		Add3DZoneSHPObject(stData);

		CZoneBaseAddition stZoneBaseAddition;
		strncpy(stZoneBaseAddition.strID, stData.strID, MAX_ZONE_LEN_48);
		stZoneBaseAddition.nActionID = m_nActivatedAcitonID;
		stZoneBaseAddition.lStartTime = llStartTime;
		stZoneBaseAddition.lEndTime = llEndTime;
		AddZoneBaseAdditionToDB(stZoneBaseAddition);

		return AddZoneBaseToDB(stData);
	}
}

bool qnzkna::ZoneManage::IZoneManagerImpl::EditZoneBase(const CZoneBase& stData, const long long& llStartTime, const long long& llEndTime )
{
	for (auto& itr : m_ZoneBaseVec_Activated)
	{
		if (0 == strcmp(stData.strID, itr.strID))
		{
			itr = stData;
			Remove3DZone(QString::fromLocal8Bit(stData.strID));
			RemoveLocalSHPObject(QString::fromLocal8Bit(stData.strID));
			Add3DZone(stData);
			Add3DZoneSHPObject(stData);
			return EditZoneBaseToDB(stData);
		}
	}
	return false;
}

bool qnzkna::ZoneManage::IZoneManagerImpl::EditInTimeZoneBase(const CZoneBase & stData, const long long & llStartTime, const long long & llEndTime)
{
	for (auto& itr : m_ZoneBaseVec_Activated)
	{
		if (0 == strcmp(stData.strID, itr.strID))
		{
			itr = stData;
			return true;
		}
	}
	return false;
}

bool qnzkna::ZoneManage::IZoneManagerImpl::RemoveZoneBase(const QString &strID)
{
	Remove3DZone(strID);
	RemoveLocalSHPObject(strID);
	auto itr = m_ZoneBaseVec_Activated.begin();
	for (;itr != m_ZoneBaseVec_Activated.end();itr++)
	{
		if (0 == strcmp(strID.toLocal8Bit(), itr->strID))
		{
			RemoveZoneBaseAdditionToDB(strID, m_nActivatedAcitonID);
			m_ZoneBaseVec_Activated.erase(itr);
			break;
		}
	}
	return RemoveZoneBaseToDB(strID);
}

bool qnzkna::ZoneManage::IZoneManagerImpl::RemoveZoneBase()
{
	bool success = false;
	for (auto itr:m_ZoneBaseVec_Activated)
	{
		QString strID = QString::fromLocal8Bit(itr.strID);
		RemoveZoneBase(strID);
		if (success == false)
		{
			return false;
		}
	}
	m_ZoneBaseVec_Activated.clear();
	return true;
}

bool qnzkna::ZoneManage::IZoneManagerImpl::IsZoneBaseIDExisted(const QString &strID)
{
	return JudegeZoneIDIsExist(strID);
}

bool qnzkna::ZoneManage::IZoneManagerImpl::SetZoneBaseShowStates(const QString &strID, const bool bShowFlag)
{
	for (auto& itr : m_ZoneBaseVec_Activated)
	{
		if (0 == strcmp(strID.toLocal8Bit(), itr.strID))
		{
			itr.bShow = bShowFlag;
			if (bShowFlag == true)
			{
				Add3DZone(itr);

			}
			else
			{
				Remove3DZone(strID);
			}

			QString strSql("UPDATE t_zone set ");
			strSql += "`XIANYIN` = " + QString::number(bShowFlag) + " ";
			strSql += "where `QYNM` = '" + strID + "'";
			QSqlQuery query;
			return query.exec(strSql);
		}
	}
	return false;
}

bool qnzkna::ZoneManage::IZoneManagerImpl::SetZoneBaseColor(const QString &strID, const unsigned int nColor)
{
	for (auto& itr : m_ZoneBaseVec_Activated)
	{
		if (0 == strcmp(strID.toLocal8Bit(), itr.strID))
		{
			itr.nLineColor = nColor;

			Remove3DZone(strID);
			Add3DZone(itr);

			QString strSql("UPDATE t_zone set ");
			strSql += "`QYCOLOR` = " + QString::number(nColor) + " ";
			strSql += "where `QYNM` = '" + strID + "'";
			QSqlQuery query;
			return query.exec(strSql);
		}
	}
	return false;
}

bool qnzkna::ZoneManage::IZoneManagerImpl::GetZoneBaseValidTime(const QString &strID, TimeVec& StartTimeVec, TimeVec& EndTimeVec)
{
	StartTimeVec.clear();
	EndTimeVec.clear();

	auto itr = m_CZoneBaseAdditionMap.find(m_nActivatedAcitonID);
	if (itr == m_CZoneBaseAdditionMap.end())
	{
		return false;
	}

	for (auto itrZone :itr->second)
	{
		if (itrZone.lStartTime == 0 || itrZone.lEndTime == 0)
		{
			continue;
		}

		if (0 == strcmp(itrZone.strID,strID.toLocal8Bit()))
		{
			StartTimeVec.push_back(itrZone.lStartTime);
			EndTimeVec.push_back(itrZone.lEndTime);
		}
	}
	return true;
}

bool qnzkna::ZoneManage::IZoneManagerImpl::SetZoneBaseValidTime(const QString &strID, const TimeVec StartTimeVec, const TimeVec EndTimeVec)
{
	if (StartTimeVec.size() != EndTimeVec.size())
	{
		return false;
	}

	bool success = RemoveZoneBaseAdditionToDB(strID, m_nActivatedAcitonID);
	if (success == false)
	{
		return false;
	}

	for (int i=0;i<StartTimeVec.size();i++)
	{
		CZoneBaseAddition stZoneBaseAddition;
		strncpy(stZoneBaseAddition.strID, strID.toLocal8Bit(), MAX_ZONE_LEN_48);
		stZoneBaseAddition.nActionID = m_nActivatedAcitonID;
		stZoneBaseAddition.lStartTime = StartTimeVec[0];
		stZoneBaseAddition.lEndTime = EndTimeVec[0];
		success = AddZoneBaseAdditionToDB(stZoneBaseAddition);
		if (success == false)
		{
			return false;
		}

	}
	return true;
}

bool qnzkna::ZoneManage::IZoneManagerImpl::JudgeZoneIntersect(const QString &strZoneID1, const QString &strZoneID2, SHPObject * pIntersectZone )
{
	SHPObject stZone1,stZone2;

	bool success = GetLocalSHPObject(strZoneID1, stZone1);
	if (success == false)
	{
		return false;
	}

	success = GetLocalSHPObject(strZoneID2, stZone2);
	if (success == false)
	{
		return false;
	}

	CShapeClip scTmp;
	scTmp.SetOperateType(SHAPE_INT);
	pIntersectZone = scTmp.Clip(&stZone1, &stZone2);
	if (pIntersectZone == nullptr)
	{
		return false;
	}
	else
	{

		double dZ(0),dM(0);
		success = JudgeAltitudeintercept(strZoneID1, strZoneID2, dZ, dM);
		if (success == false)
		{
			return false;
		}

		CCreateShape cs;
		IEarth *pEarth = IEarth::GetEarth();
		if (pEarth == nullptr)
		{
			return false;
		}

		IGraphics *pGrp = pEarth->GetGraphics();
		if (pGrp == nullptr)
		{
			return false;
		}

		for (int k = 0; k < pIntersectZone->nVertices; k++)
		{
			LY_ZoneManagerServive::Get().X_L(pIntersectZone->padfX[k], pIntersectZone->padfY[k], &pIntersectZone->padfX[k], &pIntersectZone->padfY[k]);
		}

		SHPObject *pShp1 = cs.AddZM2SHPObject(true, pIntersectZone, &dZ, &dM);
		IUserMesh1 *pUM = pGrp->CreateUserMesh1(pShp1);
		unsigned int uiV = 0;
		unsigned int color1 = RGBA(uiV & 0xFF, (uiV >> 8) & 0xFF, (uiV >> 16) & 0xFF, 200);
		pUM->SetColor(color1);
		pUM->HeighLightEdge();
		SHPDestroyObject(pShp1);

		QString str = strZoneID1 + strZoneID2;
		auto itr = m_UserMeshIntersectMap.find(str);
		if (itr != m_UserMeshIntersectMap.end())
		{
			pEarth->GetGraphics()->RemoveObject(itr->second);
			itr->second = pUM;
		}
		else
		{
			m_UserMeshIntersectMap.insert(std::make_pair(QString(strZoneID1 + strZoneID2), pUM));
			m_UserMeshIntersectMap.insert(std::make_pair(QString(strZoneID2 + strZoneID1), pUM));
		}

		return true;
	}
}

bool qnzkna::ZoneManage::IZoneManagerImpl::JudgeZoneIntersect(const QString &strZoneID1, QStringList& strIDList)
{
	SHPObject SHPObjectZone;
	SHPObject* pIntersectZone = nullptr;
	bool success = GetLocalSHPObject(strZoneID1, SHPObjectZone);
	if (success == false)
	{
		return false;
	}

	strIDList.clear();
	CShapeClip scTmp;
	scTmp.SetOperateType(SHAPE_INT);
	for (auto itr:m_ZoneSHPObjectMap)
	{
		if (strZoneID1.compare(itr.first)!= 0)
		{
			pIntersectZone = scTmp.Clip(&SHPObjectZone, itr.second);
			if (pIntersectZone != nullptr)
			{
				double dZ, dM;
				success = JudgeAltitudeintercept(strZoneID1, itr.first, dZ, dM);
				if (success == true)
				{
					strIDList.push_back(itr.first);
				}
			}
		}
	}

	if (strIDList.size()>0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void qnzkna::ZoneManage::IZoneManagerImpl::ClearZoneIntersect()
{
	IEarth *pEarth = IEarth::GetEarth();
	if (pEarth == nullptr)
	{
		return;
	}
	IGraphics *pGrp = pEarth->GetGraphics();
	if (pGrp == nullptr)
	{
		return;
	}

	for (auto itr :m_UserMeshIntersectMap)
	{
		if (itr.second != nullptr)
		{
			pEarth->GetGraphics()->RemoveObject(itr.second);
		}
	}

	m_UserMeshIntersectMap.clear();
}

bool qnzkna::ZoneManage::IZoneManagerImpl::FindLineByZones(const QString &strZoneID1, const QString &strZoneID2, QStringList& strLineIDList)
{
	SHPObject stZone1, stZone2;

	bool success = GetLocalSHPObject(strZoneID1, stZone1);
	if (success == false)
	{
		return false;
	}

	success = GetLocalSHPObject(strZoneID2, stZone2);
	if (success == false)
	{
		return false;
	}

	strLineIDList.clear();
	for (auto itr :m_ZoneBaseVec_Activated)
	{
		if (itr.nZoneType == 8)
		{
			QString strLineID = QString::fromLocal8Bit(itr.strID);
			SHPObject SHPObjectTemp;
			success = GetLocalSHPObject(strLineID, SHPObjectTemp);
			if (success == false)
			{
				continue;
			}

			CShapeClip scTmp;
			scTmp.SetOperateType(SHAPE_INT);
			SHPObject* pIntersectZone1 = scTmp.Clip(&stZone1, &SHPObjectTemp);
			SHPObject* pIntersectZone2 = scTmp.Clip(&stZone2, &SHPObjectTemp);

			double dM, dZ;
			if (pIntersectZone1 != nullptr && pIntersectZone2 != nullptr)
			{
				if (JudgeAltitudeintercept(strLineID,strZoneID1, dM,dZ) == true &&
					JudgeAltitudeintercept(strLineID, strZoneID2,dM,dZ) == true)
				{
					strLineIDList.push_back(strLineID);
				}
			}
		}
	}
	if (strLineIDList.size() > 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool qnzkna::ZoneManage::IZoneManagerImpl::TransformRectangleToPoints(const QString &strZoneID, ZonePointVec& PointVec)
{
	IProj *pProj = LY_ZoneManagerServive::Get().GetProj();
	if (pProj == nullptr)
	{
		return false;
	}

	CZoneBase stZoneBase;
	if (!GetZoneBase(strZoneID, stZoneBase))
	{
		return false;
	}

	if (stZoneBase.nZoneType != qnzkna::ZoneManage::CZoneBase::RECTANGLE)
	{
		return false;
	}

	double dX[4], dY[4];
	double x1 = stZoneBase.stRoundParameter.nRaidusDuan * 0.5;
	double y1 = stZoneBase.stRoundParameter.nRaidusChang * 0.5;
	double x2 = stZoneBase.stRoundParameter.nRaidusDuan;
	double y2 = stZoneBase.stRoundParameter.nRaidusChang ;
	if (stZoneBase.stRoundParameter.nCornerRadius == 0)
	{
		dX[0] = -x1;
		dY[0] = -y1;
		dX[3] = x1;
		dY[3] = -y1;
		dX[2] = x1;
		dY[2] = y1;
		dX[1] = -x1;
		dY[1] = y1;
	}
	else
	{
		dX[0] = 0;
		dY[0] = -y1;
		dX[3] = x2;
		dY[3] = -y1;
		dX[2] = x2;
		dY[2] = y1;
		dX[1] = 0;
		dY[1] = y1;
	}

	PointVec.clear();
	double dDistance(0), dAngle(0),x(0),y(0),dLon(0),dLat(0);
	LY_ZoneManagerServive::Get().L_X(stZoneBase.stRoundParameter.dLon, stZoneBase.stRoundParameter.dLat, &x, &y);
	for (int k = 0; k < 4; k++)
	{

		pProj->X_A(0, 0, dX[k], dY[k], dDistance, dAngle);
		pProj->A_X(x, y, dDistance, dAngle + stZoneBase.stRoundParameter.nNorthAngel, dX[k], dY[k]);
		LY_ZoneManagerServive::Get().X_L(dX[k], dY[k], &dLon, &dLat);
		ZonePoint stPoint(dLon,dLat);
		PointVec.push_back(stPoint);
	}

	if (PointVec.size() > 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

#define Grid_Length		500
#define Grid_Width		500
#define Grid_Height		500

bool qnzkna::ZoneManage::IZoneManagerImpl::GriddingZone(const QString &strZoneID)
{
	if (strZoneID == "")
	{
		return false;
	}

	CZoneBase stZoneBase;
	if (!GetZoneBase(strZoneID, stZoneBase))
	{
		return false;
	}

	if (stZoneBase.nZoneType != 3)
	{
		return false;
	}

	if (stZoneBase.stZonePointVec.size() != 4)
	{
		return false;
	}

	QGeoCoordinate QGeoPoint1(stZoneBase.stZonePointVec[0].dLat, stZoneBase.stZonePointVec[0].dLon);
	QGeoCoordinate QGeoPoint2(stZoneBase.stZonePointVec[1].dLat, stZoneBase.stZonePointVec[1].dLon);
	QGeoCoordinate QGeoPoint3(stZoneBase.stZonePointVec[3].dLat, stZoneBase.stZonePointVec[3].dLon);

	double dBear1 = QGeoPoint1.azimuthTo(QGeoPoint2);
	double dBear2 = QGeoPoint1.azimuthTo(QGeoPoint3);

	double dDistance1 = QGeoPoint1.distanceTo(QGeoPoint2);
	double dDistance2 = QGeoPoint1.distanceTo(QGeoPoint3);
	double dDisChang, dDisDuan,dBearChang,dBearDuan;
	QGeoCoordinate QGeoPointChang, QGeoPointDuan;

	if (dDistance1 > dDistance2)
	{
		dDisChang = dDistance1;
		dDisDuan = dDistance2;
		dBearChang = dBear1;
		dBearDuan = dBear2;
		QGeoPointChang = QGeoPoint2;
		QGeoPointDuan = QGeoPoint3;
	}
	else
	{
		dDisChang = dDistance2;
		dDisDuan = dDistance1;
		dBearChang = dBear2;
		dBearDuan = dBear1;
		QGeoPointChang = QGeoPoint3;
		QGeoPointDuan = QGeoPoint2;
	}

	int nChangCount = dDisChang / Grid_Length;
	int nChangDuan = dDisDuan / Grid_Width;

	std::vector<ZonePointVec> ChlidZoneVec;
	for (int i = 0;i<nChangCount;i++)
	{

		for (int j = 0;j<nChangDuan;j++)
		{

			ZonePointVec stZonePointVec;
			ZonePoint stPoint1, stPoint2, stPoint3, stPoint4;
#if 0
			QGeoCoordinate QpointChangOld = QGeoPoint1.atDistanceAndAzimuth(i * Grid_Length, 0);
			QGeoCoordinate QpointDuanOld = QGeoPoint1.atDistanceAndAzimuth(j * Grid_Width, 90);
			QGeoCoordinate QpointChangNew = QGeoPoint1.atDistanceAndAzimuth((i+1) * Grid_Length, 0);
			QGeoCoordinate QpointDuanNew = QGeoPoint1.atDistanceAndAzimuth((j+1) * Grid_Width, 90);

			stPoint1.dLon = QpointChangOld.longitude();
			stPoint1.dLat = QpointChangOld.latitude();

			stPoint2.dLon = QpointChangNew.longitude();
			stPoint2.dLat = QpointChangNew.latitude();

			stPoint4.dLon = QpointDuanNew.longitude();
			stPoint4.dLat = stPoint1.dLat;

			stPoint3.dLon = stPoint4.dLon;
			stPoint3.dLat = QpointChangNew.latitude();
#else
			QGeoCoordinate QpointStartX = QGeoPoint1.atDistanceAndAzimuth(i * Grid_Length, 90);
			QGeoCoordinate QpointStartY = QGeoPoint1.atDistanceAndAzimuth(j * Grid_Width, 0);
			QGeoCoordinate QpointChangNew = QGeoPoint1.atDistanceAndAzimuth((i + 1) * Grid_Length, 90);
			QGeoCoordinate QpointDuanNew = QGeoPoint1.atDistanceAndAzimuth((j + 1) * Grid_Width, 0);

			stPoint1.dLon = QpointStartX.longitude();
			stPoint1.dLat = QpointStartY.latitude();

			stPoint2.dLon = QpointChangNew.longitude();
			stPoint2.dLat = stPoint1.dLat;

			stPoint4.dLon = stPoint1.dLon;
			stPoint4.dLat = QpointDuanNew.latitude();

			stPoint3.dLon = stPoint2.dLon;
			stPoint3.dLat = stPoint4.dLat;

#endif

			stZonePointVec.push_back(stPoint1);
			stZonePointVec.push_back(stPoint2);
			stZonePointVec.push_back(stPoint3);
			stZonePointVec.push_back(stPoint4);
			ChlidZoneVec.push_back(stZonePointVec);
		}
	}
	SHPObject *pShp;
	CCreateShape cs, csTmp;

	IEarth *pEarth = IEarth::GetEarth();
	if (pEarth == nullptr)
	{
		return false;
	}
	IGraphics *pGrp = pEarth->GetGraphics();
	if (pGrp == nullptr)
	{
		return false;
	}
	IProj *pProj = LY_ZoneManagerServive::Get().GetProj();
	if (pProj == nullptr)
	{
		return false;
	}

	QString strID = QString::fromLocal8Bit(stZoneBase.strID);
	unsigned int uiV = stZoneBase.nLineColor;
	unsigned int color1 = RGBA(127, 127, 127, 10);
	unsigned int color2 = RGBA(255, 0, 0, 175);

	float  l, b;
	double centerX = 0, centerY = 0;
	double x, y;
	double dZ = stZoneBase.nBottomhAlt;
	double dM = Grid_Height;
	for ( auto itrZone : ChlidZoneVec)
	{
		for (int i = 0;i<4;i++)
		{
			dZ = stZoneBase.nBottomhAlt + i*dM;
			DPOINT pDPOINT[4] = {};
			int nNo = 0;
			for (auto itrPoint : itrZone)
			{
				pDPOINT[nNo].x = itrPoint.dLon;
				pDPOINT[nNo].y = itrPoint.dLat;
				nNo++;
			}
			bool bConterClock = isconterclock(itrZone.size(), (DPOINT*)pDPOINT);
			std::vector<double> vX;
			std::vector<double> vY;
			vX.reserve(itrZone.size() + 1);
			vY.reserve(itrZone.size() + 1);
			if (!bConterClock)
			{
				for (auto itrPoint : itrZone)
				{
					vX.push_back(itrPoint.dLon);
					vY.push_back(itrPoint.dLat);
				}
			}
			else
			{
				auto itr = itrZone.rbegin();
				for (; itr != itrZone.rend(); itr++)
				{
					vX.push_back(itr->dLon);
					vY.push_back(itr->dLat);
				}
			}

			vX.push_back(vX[0]);
			vY.push_back(vY[0]);

			pShp = SHPCreateSimpleObject(SHPT_POLYGON, vX.size(), vX.data(), vY.data(), NULL);
			SHPObject *pShp1 = cs.AddZM2SHPObject(true, pShp, &dZ, &dM);

			IUserMesh1 *pUM = pGrp->CreateUserMesh1(pShp1);
			if (qrand() % 10 == 0)
			{
				pUM->SetColor(color2);
			}
			else
			{
				pUM->SetColor(color1);
			}

			pUM->HeighLightEdge();

			SHPDestroyObject(pShp1);
		}
	}
	return true;
}

bool qnzkna::ZoneManage::IZoneManagerImpl::ReplanAirway_1(const QString& strAirwayZoneID, const QString& strZoneID2)
{
	CZoneBase stAirwayZoneBase, stZoneBase2;
	bool success = GetZoneBase(strAirwayZoneID, stAirwayZoneBase);
	if (success == false)
	{
		return false;
	}

	success = GetZoneBase(strZoneID2, stZoneBase2);
	if (success == false)
	{
		return false;
	}

	CircumCircle ccircle = GetZoneCircumCircle(stZoneBase2);
	QGeoCoordinate centerGeoPoi(ccircle.centerLat, ccircle.centerLon);
	auto pointsList = stAirwayZoneBase.stZonePointVec;
	int n = pointsList.size();
	int lpIndex = 0, rpIndex = n - 1;
	for (int i = 0; i < n; i++) {
		QGeoCoordinate geoPoi(pointsList[i].dLat, pointsList[i].dLon);
		double disToCenter = geoPoi.distanceTo(centerGeoPoi);
		if (disToCenter < ccircle.radius) {
			break;
		}
		lpIndex = i;
	}

	for (int i = n - 1; i >= 0; i--) {
		QGeoCoordinate geoPoi(pointsList[i].dLat, pointsList[i].dLon);
		double disToCenter = geoPoi.distanceTo(centerGeoPoi);
		if (disToCenter < ccircle.radius) {
			break;
		}
		rpIndex = i;
	}

	ZonePointVec newAirway;
	if (lpIndex == n - 1) {
		QGeoCoordinate geoCenter(ccircle.centerLat, ccircle.centerLon);
		for (int i = 0; i < n - 1; i++) {
			newAirway.push_back(pointsList[i]);
			QGeoCoordinate geoP1(pointsList[i].dLat, pointsList[i].dLon);
			QGeoCoordinate geoP2(pointsList[i + 1].dLat, pointsList[i + 1].dLon);
			double l1 = geoP1.distanceTo(geoP2);
			double l2 = geoCenter.distanceTo(geoP1);
			double l3 = geoCenter.distanceTo(geoP2);

			double tp = (l1 + l2 + l3) / 2;
			double areaS = std::sqrt(tp*(tp - l1)*(tp - l2)*(tp - l3));
			double centerToL12Dis = areaS / l1;
			if (centerToL12Dis < ccircle.radius) {
				ZonePointVec tempNewway = GetNewAirwayBetween2Point(pointsList[i], pointsList[i + 1], ccircle);
				for (const auto& poi : tempNewway) {
					newAirway.push_back(poi);
				}
			}
			newAirway.push_back(pointsList[i + 1]);
		}

		stAirwayZoneBase.stZonePointVec.clear();
		stAirwayZoneBase.stZonePointVec = newAirway;
		EditZoneBase(stAirwayZoneBase);

		return true;
	}

	if (lpIndex >= 0 && rpIndex < n) {
		ZonePointVec newWay =
			GetNewAirwayBetween2Point(pointsList[lpIndex], pointsList[rpIndex], ccircle);
		for (int i = 0; i <= lpIndex; i++) {
			newAirway.push_back(pointsList[i]);
		}
		for (auto& poi : newWay) {
			newAirway.push_back(poi);
		}
		for (int i = rpIndex; i < n; i++) {
			newAirway.push_back(pointsList[i]);
		}

		stAirwayZoneBase.stZonePointVec.clear();
		stAirwayZoneBase.stZonePointVec = newAirway;
		EditZoneBase(stAirwayZoneBase);

		return true;
	}

	return false;
}

CircumCircle qnzkna::ZoneManage::IZoneManagerImpl::GetZoneCircumCircle(CZoneBase stZoneBase)
{
	CircumCircle ccicle;
	if (stZoneBase.nZoneType == CZoneBase::POLY) {
		auto pointVec = stZoneBase.stZonePointVec;
		int n = pointVec.size();
		ZonePoint centerZP;
		double maxDis = 0;
		for (int i = 0; i < n; i++) {
			for (int j = i + 1; j < n; j++) {
				QGeoCoordinate p1(pointVec[i].dLat, pointVec[i].dLon);
				QGeoCoordinate p2(pointVec[j].dLat, pointVec[j].dLon);
				double tempDis = p1.distanceTo(p2);
				if (tempDis > maxDis) {
					maxDis = tempDis;
					ccicle.centerLon = (p1.longitude() + p2.longitude()) / 2;
					ccicle.centerLat = (p1.latitude() + p2.latitude()) / 2;
				}
			}
		}

		QGeoCoordinate cenPoi(ccicle.centerLat, ccicle.centerLon);
		double maxRadius = 0;
		for (int i = 0; i < n; i++) {
			QGeoCoordinate p1(pointVec[i].dLat, pointVec[i].dLon);
			double tempRadius = p1.distanceTo(cenPoi);
			if (tempRadius > maxRadius) {
				maxRadius = tempRadius;
			}
		}
		ccicle.radius = maxRadius + 100;
	}
	else if (stZoneBase.nZoneType == CZoneBase::CIRCLE ||
		stZoneBase.nZoneType == CZoneBase::PIE ||
		stZoneBase.nZoneType == CZoneBase::PIERING) {
		ccicle.centerLon = stZoneBase.stRoundParameter.dLon;
		ccicle.centerLat = stZoneBase.stRoundParameter.dLat;
		ccicle.radius = stZoneBase.stRoundParameter.nRadius + 100;
	}
	else if (stZoneBase.nZoneType == CZoneBase::ELLIPSE ||
		stZoneBase.nZoneType == CZoneBase::RECTANGLE ||
		stZoneBase.nZoneType == CZoneBase::CIRCLERING) {
		ccicle.centerLon = stZoneBase.stRoundParameter.dLon;
		ccicle.centerLat = stZoneBase.stRoundParameter.dLat;
		ccicle.radius = std::max(stZoneBase.stRoundParameter.nRaidusChang, stZoneBase.stRoundParameter.nRaidusDuan) + 100;
	}

	return ccicle;
}

ZonePointVec qnzkna::ZoneManage::IZoneManagerImpl::GetNewAirwayBetween2Point(ZonePoint p1, ZonePoint p2, CircumCircle ccircle)
{
	ZonePointVec newWay;
	QGeoCoordinate geoP1(p1.dLat, p1.dLon);
	QGeoCoordinate geoP2(p2.dLat, p2.dLon);
	QGeoCoordinate centerGeo(ccircle.centerLat, ccircle.centerLon);
	QGeoCoordinate circleGeoTop = centerGeo.atDistanceAndAzimuth(ccircle.radius, 0);
	QGeoCoordinate circleGeoRight = centerGeo.atDistanceAndAzimuth(ccircle.radius, 90);
	QGeoCoordinate circleGeoBottom = centerGeo.atDistanceAndAzimuth(ccircle.radius, 180);
	QGeoCoordinate circleGeoleft = centerGeo.atDistanceAndAzimuth(ccircle.radius, 270);

	if (p1.dLon > p2.dLon && p1.dLat < p2.dLat) {
		if (p1.dLat > circleGeoleft.latitude() && p1.dLon > circleGeoleft.longitude()) {
			newWay.emplace_back(p1.dLon, circleGeoleft.latitude());
		}
		newWay.emplace_back(circleGeoBottom.longitude(), circleGeoleft.latitude());
		if (p2.dLon < circleGeoBottom.longitude() || p2.dLat < circleGeoBottom.latitude()) {
			return newWay;
		}
		newWay.emplace_back(circleGeoBottom.longitude(), circleGeoRight.latitude());
	}

	if (p1.dLon < p2.dLon && p1.dLat < p2.dLat) {
		if (p1.dLon<circleGeoleft.longitude() && p1.dLat > circleGeoleft.latitude()) {
			newWay.emplace_back(p1.dLon, circleGeoleft.latitude());
		}
		newWay.emplace_back(circleGeoTop.longitude(), circleGeoleft.latitude());
		if (p2.dLon > circleGeoTop.longitude() || p2.dLat < circleGeoTop.latitude()) {
			return newWay;
		}
		newWay.emplace_back(circleGeoTop.longitude(), circleGeoRight.latitude());
	}

	if (p1.dLon < p2.dLon && p1.dLat > p2.dLat) {
		if (p1.dLat < circleGeoRight.latitude() && p1.dLon < circleGeoRight.longitude()) {
			newWay.emplace_back(p1.dLon, circleGeoRight.latitude());
		}
		newWay.emplace_back(circleGeoTop.longitude(), circleGeoRight.latitude());
		if (p2.dLon > circleGeoTop.longitude() || p2.dLat > circleGeoTop.latitude()) {
			return newWay;
		}
		newWay.emplace_back(circleGeoTop.longitude(), circleGeoleft.latitude());
	}

	if (p1.dLon > p2.dLon && p1.dLat > p2.dLat) {
		if (p1.dLat < circleGeoRight.latitude() && p1.dLon > circleGeoRight.longitude()) {
			newWay.emplace_back(p1.dLon, circleGeoRight.latitude());
		}
		newWay.emplace_back(circleGeoBottom.longitude(), circleGeoRight.latitude());
		if (p2.dLat > circleGeoBottom.latitude() || p2.dLon < circleGeoBottom.longitude()) {
			return newWay;
		}
		newWay.emplace_back(circleGeoBottom.longitude(), circleGeoleft.latitude());
	}

	return newWay;
}

bool qnzkna::ZoneManage::IZoneManagerImpl::ReplanAirway_2(const QString& strAirwayZoneID, const QString& strZoneID2)
{
	CZoneBase stAirwayZoneBase, stZoneBase2;
	bool success = GetZoneBase(strAirwayZoneID, stAirwayZoneBase);
	if (success == false)
	{
		return false;
	}

	success = GetZoneBase(strZoneID2, stZoneBase2);
	if (success == false)
	{
		return false;
	}

	if (stAirwayZoneBase.stZonePointVec.size() < 2) {
		return false;
	}

	int stZonePointVecSize = stAirwayZoneBase.stZonePointVec.size();
	double lon_start = stAirwayZoneBase.stZonePointVec[0].dLon;
	double lat_start = stAirwayZoneBase.stZonePointVec[0].dLat;
	double alt_start = stAirwayZoneBase.nBottomhAlt;

	double lon_end = stAirwayZoneBase.stZonePointVec[stZonePointVecSize - 1].dLon;
	double lat_end = stAirwayZoneBase.stZonePointVec[stZonePointVecSize - 1].dLat;
	double alt_end = stAirwayZoneBase.nBottomhAlt;

	ZonePointVec replanAirway;
	QGeoCoordinate prePoi(stAirwayZoneBase.stZonePointVec[0].dLat, stAirwayZoneBase.stZonePointVec[0].dLon);
	QHash<QGeoCoordinate, QGeoCoordinate> prePoiMap;
	prePoiMap[prePoi] = prePoi;
	int lineBanWidth = stAirwayZoneBase.nLineWidth / 2;
	for (int i = 0; i < stZonePointVecSize - 1; i++) {
		replanAirway.push_back(stAirwayZoneBase.stZonePointVec[i]);
		QGeoCoordinate p1(stAirwayZoneBase.stZonePointVec[i].dLat, stAirwayZoneBase.stZonePointVec[i].dLon);
		QGeoCoordinate p2(stAirwayZoneBase.stZonePointVec[i + 1].dLat, stAirwayZoneBase.stZonePointVec[i + 1].dLon);
		auto angle = p1.azimuthTo(p2);
		auto dis = p2.distanceTo(p1);
		int step = 100;
		bool isFind = false;
		for (int j = 0; ; j++) {
			if (step * j > dis) break;
			QGeoCoordinate ptemp = p1.atDistanceAndAzimuth(step * j, angle);
			if (isPointInZone(ptemp, stZoneBase2)) {
				if (j == 0) {
					replanAirway.pop_back();
				}
				double pre2Lon = prePoiMap[prePoi].longitude();
				double pre2Lat = prePoiMap[prePoi].latitude();

				lon_start = pre2Lon;
				lat_start = pre2Lat;
				isFind = true;
				break;
			}
			prePoiMap[ptemp] = prePoi;
			prePoi = ptemp;
		}
		if (isFind)
			break;
	}

	int endIndex = stZonePointVecSize - 1;
	prePoi = QGeoCoordinate(stAirwayZoneBase.stZonePointVec[stZonePointVecSize - 1].dLat, stAirwayZoneBase.stZonePointVec[stZonePointVecSize - 1].dLon);
	prePoiMap.clear();
	prePoiMap[prePoi] = prePoi;
	for (int i = stZonePointVecSize - 1; i > 0; i--) {
		QGeoCoordinate p1(stAirwayZoneBase.stZonePointVec[i].dLat, stAirwayZoneBase.stZonePointVec[i].dLon);
		QGeoCoordinate p2(stAirwayZoneBase.stZonePointVec[i - 1].dLat, stAirwayZoneBase.stZonePointVec[i - 1].dLon);
		auto angle = p1.azimuthTo(p2);
		auto dis = p2.distanceTo(p1);
		int step = 100;
		bool isFind = false;
		for (int j = 0; ; j++) {
			if (step * j > dis) break;
			QGeoCoordinate ptemp = p1.atDistanceAndAzimuth(step * j, angle);
			if (isPointInZone(ptemp, stZoneBase2)) {
				if (j > 1) {
					endIndex = i;
				}
				else {
					endIndex = std::min(i + 1, stZonePointVecSize - 1);
				}
				double pre2Lon = prePoiMap[prePoi].longitude();
				double pre2Lat = prePoiMap[prePoi].latitude();
				lon_end = pre2Lon;
				lat_end = pre2Lat;
				isFind = true;
				break;
			}
			prePoiMap[ptemp] = prePoi;
			prePoi = ptemp;
		}
		if (isFind)
			break;
	}

	QGeoCoordinate geoCenter(stZoneBase2.stRoundParameter.dLat, stZoneBase2.stRoundParameter.dLon);
	QGeoCoordinate geoTop, geoRight, geoBottom, geoLeft;
	double maxLon = 0.0, minLon = 180.0, maxLat = 0.0, minLat = 180.0;

	if (stZoneBase2.nZoneType == CZoneBase::CIRCLE || stZoneBase2.nZoneType == CZoneBase::PIE) {
		maxLon = geoCenter.atDistanceAndAzimuth(stZoneBase2.stRoundParameter.nRadius, 90).longitude();
		maxLat = geoCenter.atDistanceAndAzimuth(stZoneBase2.stRoundParameter.nRadius, 0).latitude();
		minLon = geoCenter.atDistanceAndAzimuth(stZoneBase2.stRoundParameter.nRadius, 270).longitude();
		minLat = geoCenter.atDistanceAndAzimuth(stZoneBase2.stRoundParameter.nRadius, 180).latitude();
	}
	else if (stZoneBase2.nZoneType == CZoneBase::ELLIPSE) {
		double duijiaoDis = std::sqrt(stZoneBase2.stRoundParameter.nRaidusChang*stZoneBase2.stRoundParameter.nRaidusChang +
			stZoneBase2.stRoundParameter.nRaidusDuan*stZoneBase2.stRoundParameter.nRaidusDuan);
		double angle = 90 - (std::asin(stZoneBase2.stRoundParameter.nRaidusChang / duijiaoDis) * 180 / PI);
		QGeoCoordinate p1 = geoCenter.atDistanceAndAzimuth(duijiaoDis, angle - stZoneBase2.stRoundParameter.nAngelChang);
		QGeoCoordinate p2 = geoCenter.atDistanceAndAzimuth(duijiaoDis, 180 - angle - stZoneBase2.stRoundParameter.nAngelChang);
		QGeoCoordinate p3 = geoCenter.atDistanceAndAzimuth(duijiaoDis, 180 + angle - stZoneBase2.stRoundParameter.nAngelChang);
		QGeoCoordinate p4 = geoCenter.atDistanceAndAzimuth(duijiaoDis, 360 - angle - stZoneBase2.stRoundParameter.nAngelChang);

		std::priority_queue<double> pq;
		pq.push(p1.longitude());
		pq.push(p2.longitude());
		pq.push(p3.longitude());
		pq.push(p4.longitude());
		maxLon = pq.top();
		pq.pop();
		while (!pq.empty()) {
			minLon = pq.top();
			pq.pop();
		}
		pq.push(p1.latitude());
		pq.push(p2.latitude());
		pq.push(p3.latitude());
		pq.push(p4.latitude());
		maxLat = pq.top();
		pq.pop();
		while (!pq.empty()) {
			minLat = pq.top();
			pq.pop();
		}
	}
	else if (stZoneBase2.nZoneType == CZoneBase::POLY) {

		for (const auto& poi : stZoneBase2.stZonePointVec) {
			if (poi.dLon > maxLon) maxLon = poi.dLon;
			if (poi.dLon < minLon) minLon = poi.dLon;
			if (poi.dLat > maxLat) maxLat = poi.dLat;
			if (poi.dLat < minLat) minLat = poi.dLat;
		}
	}
	else if (stZoneBase2.nZoneType == CZoneBase::RECTANGLE) {
		double duijiaoDis = std::sqrt(stZoneBase2.stRoundParameter.nRaidusChang*stZoneBase2.stRoundParameter.nRaidusChang +
			stZoneBase2.stRoundParameter.nRaidusDuan*stZoneBase2.stRoundParameter.nRaidusDuan) / 2;
		double angle = 90 - (std::asin(stZoneBase2.stRoundParameter.nRaidusChang / 2 / duijiaoDis) * 180 / PI);
		QGeoCoordinate p1 = geoCenter.atDistanceAndAzimuth(duijiaoDis, angle - stZoneBase2.stRoundParameter.nNorthAngel);
		QGeoCoordinate p2 = geoCenter.atDistanceAndAzimuth(duijiaoDis, 180 - angle - stZoneBase2.stRoundParameter.nNorthAngel);
		QGeoCoordinate p3 = geoCenter.atDistanceAndAzimuth(duijiaoDis, 180 + angle - stZoneBase2.stRoundParameter.nNorthAngel);
		QGeoCoordinate p4 = geoCenter.atDistanceAndAzimuth(duijiaoDis, 360 - angle - stZoneBase2.stRoundParameter.nNorthAngel);

		std::priority_queue<double> pq;
		pq.push(p1.longitude());
		pq.push(p2.longitude());
		pq.push(p3.longitude());
		pq.push(p4.longitude());
		maxLon = pq.top();
		pq.pop();
		while (!pq.empty()) {
			minLon = pq.top();
			pq.pop();
		}
		pq.push(p1.latitude());
		pq.push(p2.latitude());
		pq.push(p3.latitude());
		pq.push(p4.latitude());
		maxLat = pq.top();
		pq.pop();
		while (!pq.empty()) {
			minLat = pq.top();
			pq.pop();
		}
	}
	else if (stZoneBase2.nZoneType == CZoneBase::CIRCLERING || stZoneBase2.nZoneType == CZoneBase::PIERING) {
		maxLon = geoCenter.atDistanceAndAzimuth(stZoneBase2.stRoundParameter.nRadius, 90).longitude();
		maxLat = geoCenter.atDistanceAndAzimuth(stZoneBase2.stRoundParameter.nRadius, 0).latitude();
		minLon = geoCenter.atDistanceAndAzimuth(stZoneBase2.stRoundParameter.nRadius, 270).longitude();
		minLat = geoCenter.atDistanceAndAzimuth(stZoneBase2.stRoundParameter.nRadius, 180).latitude();
	}

	double huansuan = 110000.0;
	minLon -= (SAFE_DISTANCE * 1.0 / huansuan);
	maxLon += (SAFE_DISTANCE * 1.0 / huansuan);
	minLat -= (SAFE_DISTANCE * 1.0 / huansuan);
	maxLat += (SAFE_DISTANCE * 1.0 / huansuan);

	std::priority_queue<double> pq;
	pq.push(maxLon);
	pq.push(minLon);
	pq.push(lon_start);
	pq.push(lon_end);
	maxLon = pq.top();
	while (!pq.empty()) {
		minLon = pq.top();
		pq.pop();
	}
	pq.push(maxLat);
	pq.push(minLat);
	pq.push(lat_start);
	pq.push(lat_end);
	maxLat = pq.top();
	while (!pq.empty()) {
		minLat = pq.top();
		pq.pop();
	}

	QGeoCoordinate leftup(maxLat, minLon);
	QGeoCoordinate rightup(maxLat, maxLon);
	QGeoCoordinate rightdown(minLat, maxLon);
	QGeoCoordinate leftdown(minLat, minLon);
	double mapLength = leftup.distanceTo(rightup);
	double mapWidth = leftup.distanceTo(leftdown);
	double angleLehgth = leftup.azimuthTo(rightup);
	double angleWidth = leftup.azimuthTo(leftdown);
	if (fabs(angleWidth - 0) < 1 && fabs(angleLehgth - 270) < 1)
		angleWidth += 360.0;
	if (fabs(angleLehgth - 0) < 1 && fabs(angleWidth - 270) < 1)
		angleLehgth += 360.0;
	double angleLine = (angleLehgth + angleWidth) / 2;
	double temp[] = { mapLength, mapWidth , angleLehgth , angleWidth , angleLine };
	std::vector<double> gridMapParams(temp, temp + sizeof(temp) / sizeof(temp[0]));

	double speed = 1000;
	m_pGridNode = createGridsNode(gridMapParams, leftup, stZoneBase2);

	std::vector<casic_vector3d> searchPointVec;
	std::vector<double> fly_param;
	fly_param.push_back(speed);
	fly_param.push_back(0);
	casic_vector3d startPos(lon_start, lat_start, alt_start);
	casic_vector3d endPos(lon_end, lat_end, alt_end);
	pathPlanning(startPos, endPos, fly_param, searchPointVec);

	sparsePath(searchPointVec);

	for (const auto& poi : searchPointVec) {
		replanAirway.emplace_back(poi.x, poi.y);
	}

	for (int i = endIndex; i < stZonePointVecSize; i++) {
		replanAirway.push_back(stAirwayZoneBase.stZonePointVec[i]);
	}

	stAirwayZoneBase.stZonePointVec.clear();
	stAirwayZoneBase.stZonePointVec = replanAirway;
	m_replanAirwayZone = stAirwayZoneBase;

	m_replanAirwayDraw = true;

	if (LyMessageBox::question(nullptr, tr2("提示"), tr2("新航路如地图上所示，是否替换当前航路？"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::No) {
		m_replanAirwayDraw = false;
		return false;
	}

	m_replanAirwayDraw = false;
	EditZoneBase(stAirwayZoneBase);
	return true;
}

bool qnzkna::ZoneManage::IZoneManagerImpl::AddCoverZoneBase(const CZoneBase& stData)
{
	if (stData.nZoneType != 3)
		return false;

	IEarth* pEarthptr = IEarth::GetEarth();
	osgEarth::MapNode* mapNode = (osgEarth::MapNode*)(pEarthptr->GetPtr(IEarth::EPtrType_EarthMapNode));
	if (m_FeatureNode_Parent == nullptr)
	{
		m_FeatureNode_Parent = new osg::Group;
		mapNode->addChild(m_FeatureNode_Parent);
	}

	{
		if (m_FeatureNode_Cover.find(std::string(stData.strID)) != m_FeatureNode_Cover.end())
		{
			m_FeatureNode_Parent->removeChild(m_FeatureNode_Cover.find(std::string(stData.strID))->second);
			m_FeatureNode_Cover.find(std::string(stData.strID))->second->~FeatureNode();
			m_FeatureNode_Cover.erase(m_FeatureNode_Cover.find(std::string(stData.strID)));
		}

		std::vector<osg::Vec3d> points;
		points.clear();
		for (auto point : stData.stZonePointVec)
		{
			points.push_back(osg::Vec3d(point.dLon, point.dLat, 10000));
		}
		osgEarth::Symbology::Geometry* geoMery = osgEarth::Symbology::Geometry::create(osgEarth::Symbology::Geometry::TYPE_POLYGON, &points);

		osgEarth::Features::Feature* feature = new osgEarth::Features::Feature(geoMery, mapNode->getMapSRS()->getGeographicSRS());
		osgEarth::Style* geomStyle = new osgEarth::Style;

		unsigned int uiV = stData.nLineColor;
		osgEarth::Symbology::Color tempCo (GETR(uiV) / 255.0, GETG(uiV) / 255.0, GETB(uiV) / 255.0, 0.5);
		unsigned uintd = tempCo.asRGBA();
		geomStyle->getOrCreate<osgEarth::Symbology::PolygonSymbol>()
			->fill()->color() = osgEarth::Symbology::Color(tempCo);
		geomStyle->getOrCreate<osgEarth::Symbology::AltitudeSymbol>()
			->clamping() = osgEarth::Symbology::AltitudeSymbol::CLAMP_TO_TERRAIN;
		geomStyle->getOrCreate<osgEarth::Symbology::AltitudeSymbol>()
			->technique() = osgEarth::Symbology::AltitudeSymbol::TECHNIQUE_DRAPE;
		geomStyle->getOrCreate<osgEarth::Symbology::AltitudeSymbol>()
			->verticalOffset() = 0.1;

		osg::ref_ptr<osgEarth::Annotation::FeatureNode> rp_featureNode = new osgEarth::Annotation::FeatureNode(mapNode, feature, *geomStyle);
		m_FeatureNode_Parent->addChild(rp_featureNode.get());
		m_FeatureNode_Cover[stData.strID] = rp_featureNode.get();
		return true;
	}
}

bool qnzkna::ZoneManage::IZoneManagerImpl::RemoveCoverZoneBase(const QString& strZoneID)
{
	auto stdZoneID = std::string(strZoneID.toLocal8Bit());
	if (m_FeatureNode_Cover.find(stdZoneID) != m_FeatureNode_Cover.end())
	{
		m_FeatureNode_Parent->removeChild(m_FeatureNode_Cover.find(stdZoneID)->second);

		m_FeatureNode_Cover.erase(m_FeatureNode_Cover.find(stdZoneID));
		return true;
	}
	else
	{
		return false;
	}
}

bool qnzkna::ZoneManage::IZoneManagerImpl::setCoverisShow(const QString & strZoneID, bool isShow)
{
	auto stdZoneID = std::string(strZoneID.toLocal8Bit());
	if (m_FeatureNode_Cover.find(stdZoneID) != m_FeatureNode_Cover.end())
	{
		m_FeatureNode_Cover.find(stdZoneID)->second->setNodeMask(isShow ? 1 : 0);
		return true;
	}
	return false;
}

void qnzkna::ZoneManage::IZoneManagerImpl::GetZoneRule(CZoneBaseRule& stData)
{
	stData = m_ZoneBaseRule;
}

bool qnzkna::ZoneManage::IZoneManagerImpl::SetZoneRule(const CZoneBaseRule& stData)
{
	m_ZoneBaseRule = stData;
	return EditZoneRuleToDB(stData);
}

QStringList qnzkna::ZoneManage::IZoneManagerImpl::GetZoneFileNames()
{
	QStringList strNameList;
	QDir findFile(m_strZoneFilePath);
	findFile.setFilter(QDir::Files);
	QStringList filters;
	filters << "*.xml";
	findFile.setNameFilters(filters);
	QStringList list = findFile.entryList(QDir::Files);
	foreach(QFileInfo file, list)
	{
		strNameList.push_back(file.fileName());
	}
	return strNameList;
}

bool qnzkna::ZoneManage::IZoneManagerImpl::OpenZoneFile(const QString& strName, CZoneBaseVec& ZoneFileVec)
{
	ZoneFileVec.clear();

	if ("" == strName)
		return false;

	QString cstrFileName = strName;
	if (-1 == cstrFileName.lastIndexOf(".xml"))
		cstrFileName += ".xml";

	QString cstrFullPath = m_strZoneFilePath + cstrFileName;

	QFile findFile;
	findFile.setFileName(cstrFullPath);
	bool bFind = findFile.exists();

	if (!bFind)
		return false;

	TiXmlDocument doc;
	bool bLoadFile = doc.LoadFile(cstrFullPath.toLocal8Bit().data());
	if (!bLoadFile)
		return false;

	TiXmlElement* pRootEle = doc.RootElement();
	if (!pRootEle)
		return false;

	int nTemp(0);
	double dTemp(0);
	TiXmlElement* pMissionInfo = pRootEle->FirstChildElement("区域信息");
	while (pMissionInfo)
	{
		CZoneBase stZoneBase;

		const char* pChar = pMissionInfo->Attribute("名称");
		if (!pChar)
			return false;
		std::string strTmp = pChar;
		if (strTmp.length() >= MAX_ZONE_LEN_48)
			return false;
		strncpy(stZoneBase.strID, strTmp.c_str(), MAX_ZONE_LEN_48);

		pChar = pMissionInfo->Attribute("形状");
		if (!pChar)
			return false;
		nTemp = atoi(pChar);
		stZoneBase.nZoneType = nTemp;

		pChar = pMissionInfo->Attribute("属性");
		if (!pChar)
			return false;
		nTemp = atoi(pChar);
		stZoneBase.nProperty = nTemp;

		pChar = pMissionInfo->Attribute("线形");
		if (!pChar)
			return false;
		nTemp = atoi(pChar);
		stZoneBase.nLineType = nTemp;

		pChar = pMissionInfo->Attribute("填充方式");
		if (!pChar)
			return false;
		nTemp = atoi(pChar);
		stZoneBase.nFillType = nTemp;

		pChar = pMissionInfo->Attribute("颜色");
		if (!pChar)
			return false;
		nTemp = atoi(pChar);
		stZoneBase.nLineColor = nTemp;

		pChar = pMissionInfo->Attribute("上高");
		if (!pChar)
			return false;
		nTemp = atoi(pChar);
		stZoneBase.nAboveAlt = nTemp;

		pChar = pMissionInfo->Attribute("底高");
		if (!pChar)
			return false;
		nTemp = atoi(pChar);
		stZoneBase.nBottomhAlt = nTemp;

		if (stZoneBase.nZoneType == CZoneBase::LINE)
		{

			pChar = pMissionInfo->Attribute("航道宽度");
			if (!pChar)
				return false;
			nTemp = atoi(pChar);
			stZoneBase.nLineWidth = nTemp;
		}

		TiXmlElement* pChildList = pMissionInfo->FirstChildElement("参数列表");
		if (pChildList != NULL)
		{
			if (stZoneBase.nZoneType == CZoneBase::CIRCLE)
			{
				TiXmlElement* pDefendTarge = pChildList->FirstChildElement("参数");
				{
					while (pDefendTarge)
					{

						pChar = pDefendTarge->Attribute("中心经度");
						if (!pChar)
							return false;

						dTemp = atof(pChar);
						stZoneBase.stRoundParameter.dLon = dTemp;

						pChar = pDefendTarge->Attribute("中心纬度");
						if (!pChar)
							return false;

						dTemp = atof(pChar);
						stZoneBase.stRoundParameter.dLat = dTemp;

						pChar = pDefendTarge->Attribute("半径");
						if (!pChar)
							return false;

						nTemp = atoi(pChar);
						stZoneBase.stRoundParameter.nRadius = nTemp;

						pDefendTarge = pDefendTarge->NextSiblingElement("参数");
					}
				}
			}
			else if (stZoneBase.nZoneType == CZoneBase::PIE)
			{
				TiXmlElement* pDefendTarge = pChildList->FirstChildElement("参数");
				{
					while (pDefendTarge)
					{

						pChar = pDefendTarge->Attribute("中心经度");
						if (!pChar)
							return false;

						dTemp = atof(pChar);
						stZoneBase.stRoundParameter.dLon = dTemp;

						pChar = pDefendTarge->Attribute("中心纬度");
						if (!pChar)
							return false;

						dTemp = atof(pChar);
						stZoneBase.stRoundParameter.dLat = dTemp;

						pChar = pDefendTarge->Attribute("起始角");
						if (!pChar)
							return false;

						nTemp = atoi(pChar);
						stZoneBase.stRoundParameter.nStartAngel = nTemp;

						pChar = pDefendTarge->Attribute("终止角");
						if (!pChar)
							return false;

						nTemp = atoi(pChar);
						stZoneBase.stRoundParameter.nEndAngel = nTemp;

						pChar = pDefendTarge->Attribute("半径");
						if (!pChar)
							return false;

						nTemp = atoi(pChar);
						stZoneBase.stRoundParameter.nRadius = nTemp;

						pDefendTarge = pDefendTarge->NextSiblingElement("参数");
					}
				}

			}
			else if (stZoneBase.nZoneType == CZoneBase::ELLIPSE)
			{
				TiXmlElement* pDefendTarge = pChildList->FirstChildElement("参数");
				{
					while (pDefendTarge)
					{

						pChar = pDefendTarge->Attribute("中心经度");
						if (!pChar)
							return false;

						dTemp = atof(pChar);
						stZoneBase.stRoundParameter.dLon = dTemp;

						pChar = pDefendTarge->Attribute("中心纬度");
						if (!pChar)
							return false;

						dTemp = atof(pChar);
						stZoneBase.stRoundParameter.dLat = dTemp;

						pChar = pDefendTarge->Attribute("短半轴");
						if (!pChar)
							return false;

						nTemp = atoi(pChar);
						stZoneBase.stRoundParameter.nRaidusDuan = nTemp;

						pChar = pDefendTarge->Attribute("长半轴");
						if (!pChar)
							return false;

						nTemp = atoi(pChar);
						stZoneBase.stRoundParameter.nRaidusChang = nTemp;

						pChar = pDefendTarge->Attribute("正北方向夹角");
						if (!pChar)
							return false;

						nTemp = atoi(pChar);
						stZoneBase.stRoundParameter.nAngelChang = nTemp;

						pDefendTarge = pDefendTarge->NextSiblingElement("参数");
					}
				}
			}
			else if (stZoneBase.nZoneType == CZoneBase::POLY)
			{
				TiXmlElement* pDefendTarge = pChildList->FirstChildElement("点");
				{
					while (pDefendTarge)
					{
						ZonePoint stZonePoint;

						pChar = pDefendTarge->Attribute("经度");
						if (!pChar)
							return false;

						dTemp = atof(pChar);
						stZonePoint.dLon = dTemp;

						pChar = pDefendTarge->Attribute("纬度");
						if (!pChar)
							return false;

						dTemp = atof(pChar);
						stZonePoint.dLat = dTemp;

						stZoneBase.stZonePointVec.push_back(stZonePoint);
						pDefendTarge = pDefendTarge->NextSiblingElement("点");
					}
				}
			}
			else if (stZoneBase.nZoneType == CZoneBase::RECTANGLE)
			{
				TiXmlElement* pDefendTarge = pChildList->FirstChildElement("参数");
				{
					while (pDefendTarge)
					{

						pChar = pDefendTarge->Attribute("中心经度");
						if (!pChar)
							return false;

						dTemp = atof(pChar);
						stZoneBase.stRoundParameter.dLon = dTemp;

						pChar = pDefendTarge->Attribute("中心纬度");
						if (!pChar)
							return false;

						dTemp = atof(pChar);
						stZoneBase.stRoundParameter.dLat = dTemp;

						pChar = pDefendTarge->Attribute("宽");
						if (!pChar)
							return false;

						nTemp = atoi(pChar);
						stZoneBase.stRoundParameter.nRaidusDuan = nTemp;

						pChar = pDefendTarge->Attribute("长");
						if (!pChar)
							return false;

						nTemp = atoi(pChar);
						stZoneBase.stRoundParameter.nRaidusChang = nTemp;

						pChar = pDefendTarge->Attribute("方向");
						if (!pChar)
							return false;

						nTemp = atoi(pChar);
						stZoneBase.stRoundParameter.nNorthAngel = nTemp;

						pDefendTarge = pDefendTarge->NextSiblingElement("参数");
					}
				}
			}
			else if (stZoneBase.nZoneType == CZoneBase::CIRCLERING)
			{
				TiXmlElement* pDefendTarge = pChildList->FirstChildElement("参数");
				{
					while (pDefendTarge)
					{

						pChar = pDefendTarge->Attribute("中心经度");
						if (!pChar)
							return false;

						dTemp = atof(pChar);
						stZoneBase.stRoundParameter.dLon = dTemp;

						pChar = pDefendTarge->Attribute("中心纬度");
						if (!pChar)
							return false;

						dTemp = atof(pChar);
						stZoneBase.stRoundParameter.dLat = dTemp;

						pChar = pDefendTarge->Attribute("内圈半径");
						if (!pChar)
							return false;

						nTemp = atoi(pChar);
						stZoneBase.stRoundParameter.nRaidusDuan = nTemp;

						pChar = pDefendTarge->Attribute("外圈半径");
						if (!pChar)
							return false;

						nTemp = atoi(pChar);
						stZoneBase.stRoundParameter.nRaidusChang = nTemp;

						pDefendTarge = pDefendTarge->NextSiblingElement("参数");
					}
				}
			}
			else if (stZoneBase.nZoneType == CZoneBase::PIERING)
			{
				TiXmlElement* pDefendTarge = pChildList->FirstChildElement("参数");
				{
					while (pDefendTarge)
					{

						pChar = pDefendTarge->Attribute("中心经度");
						if (!pChar)
							return false;

						dTemp = atof(pChar);
						stZoneBase.stRoundParameter.dLon = dTemp;

						pChar = pDefendTarge->Attribute("中心纬度");
						if (!pChar)
							return false;

						dTemp = atof(pChar);
						stZoneBase.stRoundParameter.dLat = dTemp;

						pChar = pDefendTarge->Attribute("内圈半径");
						if (!pChar)
							return false;

						nTemp = atoi(pChar);
						stZoneBase.stRoundParameter.nRaidusDuan = nTemp;

						pChar = pDefendTarge->Attribute("外圈半径");
						if (!pChar)
							return false;

						nTemp = atoi(pChar);
						stZoneBase.stRoundParameter.nRaidusChang = nTemp;

						pChar = pDefendTarge->Attribute("起始角");
						if (!pChar)
							return false;

						nTemp = atoi(pChar);
						stZoneBase.stRoundParameter.nStartAngel = nTemp;

						pChar = pDefendTarge->Attribute("终止角");
						if (!pChar)
							return false;

						nTemp = atoi(pChar);
						stZoneBase.stRoundParameter.nEndAngel = nTemp;

						pDefendTarge = pDefendTarge->NextSiblingElement("参数");
					}
				}
			}
			else if (stZoneBase.nZoneType == CZoneBase::POINT)
			{

			}
			else if (stZoneBase.nZoneType == CZoneBase::LINE)
			{
				TiXmlElement* pDefendTarge = pChildList->FirstChildElement("点");
				{
					while (pDefendTarge)
					{
						ZonePoint stZonePoint;

						pChar = pDefendTarge->Attribute("经度");
						if (!pChar)
							return false;

						dTemp = atof(pChar);
						stZonePoint.dLon = dTemp;

						pChar = pDefendTarge->Attribute("纬度");
						if (!pChar)
							return false;

						dTemp = atof(pChar);
						stZonePoint.dLat = dTemp;

						stZoneBase.stZonePointVec.push_back(stZonePoint);
						pDefendTarge = pDefendTarge->NextSiblingElement("点");
					}
				}
			}
			ZoneFileVec.push_back(stZoneBase);
		}
		pMissionInfo = pMissionInfo->NextSiblingElement("区域信息");
	}
	return true;
}

bool qnzkna::ZoneManage::IZoneManagerImpl::SaveZoneFile(const QString& strName)
{
	TiXmlDocument tiDoc;
	TiXmlDeclaration* ptrDecl = new TiXmlDeclaration("1.0", "GB2312", "");
	if (!ptrDecl)
		return false;

	tiDoc.LinkEndChild(ptrDecl);
	TiXmlElement* ptrRoot = new TiXmlElement("区域列表");
	if (!ptrRoot)
		return false;

	tiDoc.LinkEndChild(ptrRoot);
	for ( auto itrZone : m_ZoneBaseVec_Activated )
	{
		TiXmlElement* pChild01 = new TiXmlElement("区域信息");
		if (!pChild01)
			return false;

		pChild01->SetAttribute("名称", itrZone.strID);
		pChild01->SetAttribute("形状", itrZone.nZoneType);
		pChild01->SetAttribute("属性", itrZone.nProperty);
		pChild01->SetAttribute("线形", itrZone.nLineType);
		pChild01->SetAttribute("填充方式", itrZone.nFillType);
		pChild01->SetAttribute("颜色", itrZone.nLineColor);
		pChild01->SetAttribute("上高", itrZone.nAboveAlt);
		pChild01->SetAttribute("底高", itrZone.nBottomhAlt);
		if (itrZone.nZoneType == CZoneBase::LINE)
		{
			pChild01->SetAttribute("航道宽度", itrZone.nLineWidth);
		}
		ptrRoot->LinkEndChild(pChild01);

		TiXmlElement* pChild02 = new TiXmlElement("参数列表");
		if (!pChild02)
			return false;

		if (itrZone.nZoneType == CZoneBase::CIRCLE)
		{
			TiXmlElement* pChild0201 = new TiXmlElement("参数");
			if (!pChild0201)
				return false;

			pChild0201->SetAttribute("中心经度", QString::number(itrZone.stRoundParameter.dLon, 'f', 7).toStdString().c_str());
			pChild0201->SetAttribute("中心纬度", QString::number(itrZone.stRoundParameter.dLat, 'f', 7).toStdString().c_str());
			pChild0201->SetAttribute("半径", QString::number(itrZone.stRoundParameter.nRadius).toStdString().c_str());

			pChild02->LinkEndChild(pChild0201);
		}
		else if (itrZone.nZoneType == CZoneBase::PIE)
		{
			TiXmlElement* pChild0201 = new TiXmlElement("参数");
			if (!pChild0201)
				return false;

			pChild0201->SetAttribute("中心经度", QString::number(itrZone.stRoundParameter.dLon, 'f', 7).toStdString().c_str());
			pChild0201->SetAttribute("中心纬度", QString::number(itrZone.stRoundParameter.dLat, 'f', 7).toStdString().c_str());
			pChild0201->SetAttribute("起始角", QString::number(itrZone.stRoundParameter.nStartAngel).toStdString().c_str());
			pChild0201->SetAttribute("终止角", QString::number(itrZone.stRoundParameter.nEndAngel).toStdString().c_str());
			pChild0201->SetAttribute("半径", QString::number(itrZone.stRoundParameter.nRadius).toStdString().c_str());

			pChild02->LinkEndChild(pChild0201);
		}
		else if (itrZone.nZoneType == CZoneBase::ELLIPSE)
		{
			TiXmlElement* pChild0201 = new TiXmlElement("参数");
			if (!pChild0201)
				return false;

			pChild0201->SetAttribute("中心经度", QString::number(itrZone.stRoundParameter.dLon, 'f', 7).toStdString().c_str());
			pChild0201->SetAttribute("中心纬度", QString::number(itrZone.stRoundParameter.dLat, 'f', 7).toStdString().c_str());
			pChild0201->SetAttribute("短半轴", QString::number(itrZone.stRoundParameter.nRaidusDuan).toStdString().c_str());
			pChild0201->SetAttribute("长半轴", QString::number(itrZone.stRoundParameter.nRaidusChang).toStdString().c_str());
			pChild0201->SetAttribute("正北方向夹角", QString::number(itrZone.stRoundParameter.nAngelChang).toStdString().c_str());

			pChild02->LinkEndChild(pChild0201);
		}
		else if (itrZone.nZoneType == CZoneBase::CIRCLERING)
		{
			TiXmlElement* pChild0201 = new TiXmlElement("参数");
			if (!pChild0201)
				return false;

			pChild0201->SetAttribute("中心经度", QString::number(itrZone.stRoundParameter.dLon, 'f', 7).toStdString().c_str());
			pChild0201->SetAttribute("中心纬度", QString::number(itrZone.stRoundParameter.dLat, 'f', 7).toStdString().c_str());
			pChild0201->SetAttribute("内圈半径", QString::number(itrZone.stRoundParameter.nRaidusDuan).toStdString().c_str());
			pChild0201->SetAttribute("外圈半径", QString::number(itrZone.stRoundParameter.nRaidusChang).toStdString().c_str());

			pChild02->LinkEndChild(pChild0201);
		}
		else if (itrZone.nZoneType == CZoneBase::PIERING)
		{
			TiXmlElement* pChild0201 = new TiXmlElement("参数");
			if (!pChild0201)
				return false;

			pChild0201->SetAttribute("中心经度", QString::number(itrZone.stRoundParameter.dLon, 'f', 7).toStdString().c_str());
			pChild0201->SetAttribute("中心纬度", QString::number(itrZone.stRoundParameter.dLat, 'f', 7).toStdString().c_str());
			pChild0201->SetAttribute("内圈半径", QString::number(itrZone.stRoundParameter.nRaidusDuan).toStdString().c_str());
			pChild0201->SetAttribute("外圈半径", QString::number(itrZone.stRoundParameter.nRaidusChang).toStdString().c_str());
			pChild0201->SetAttribute("起始角", QString::number(itrZone.stRoundParameter.nStartAngel).toStdString().c_str());
			pChild0201->SetAttribute("终止角", QString::number(itrZone.stRoundParameter.nEndAngel).toStdString().c_str());

			pChild02->LinkEndChild(pChild0201);
		}
		else if (itrZone.nZoneType == CZoneBase::POLY)
		{
			int nNo = 1;
			for (auto itrpoint:itrZone.stZonePointVec)
			{
				TiXmlElement* pChild0201 = new TiXmlElement("点");
				if (!pChild0201)
					return false;

				pChild0201->SetAttribute("序号", QString::number(nNo).toStdString().c_str());
				pChild0201->SetAttribute("经度", QString::number(itrpoint.dLon, 'f', 7).toStdString().c_str());
				pChild0201->SetAttribute("纬度", QString::number(itrpoint.dLat, 'f', 7).toStdString().c_str());

				nNo++;
				pChild02->LinkEndChild(pChild0201);
			}

		}
		else if (itrZone.nZoneType == CZoneBase::LINE)
		{
			int nNo = 1;
			for (auto itrpoint : itrZone.stZonePointVec)
			{
				TiXmlElement* pChild0201 = new TiXmlElement("点");
				if (!pChild0201)
					return false;

				pChild0201->SetAttribute("序号", QString::number(nNo).toStdString().c_str());
				pChild0201->SetAttribute("经度", QString::number(itrpoint.dLon, 'f', 7).toStdString().c_str());
				pChild0201->SetAttribute("纬度", QString::number(itrpoint.dLat, 'f', 7).toStdString().c_str());

				nNo++;
				pChild02->LinkEndChild(pChild0201);
			}
		}
		else if (itrZone.nZoneType == CZoneBase::RECTANGLE)
		{
			TiXmlElement* pChild0201 = new TiXmlElement("参数");
			if (!pChild0201)
				return false;

			pChild0201->SetAttribute("中心经度", QString::number(itrZone.stRoundParameter.dLon, 'f', 7).toStdString().c_str());
			pChild0201->SetAttribute("中心纬度", QString::number(itrZone.stRoundParameter.dLat, 'f', 7).toStdString().c_str());
			pChild0201->SetAttribute("宽", QString::number(itrZone.stRoundParameter.nRaidusDuan).toStdString().c_str());
			pChild0201->SetAttribute("长", QString::number(itrZone.stRoundParameter.nRaidusChang).toStdString().c_str());
			pChild0201->SetAttribute("方向", QString::number(itrZone.stRoundParameter.nNorthAngel).toStdString().c_str());

			pChild02->LinkEndChild(pChild0201);
		}
		else if (itrZone.nZoneType == CZoneBase::POINT)
		{

		}
		pChild01->LinkEndChild(pChild02);
	}

	return tiDoc.SaveFile(strName.toLocal8Bit());
}

bool qnzkna::ZoneManage::IZoneManagerImpl::RemoveZoneFile(const QString& strName)
{
	if (NULL == strName)
	{
		return false;
	}
	QString strZoneFileName = strName;

	if (-1 == strZoneFileName.lastIndexOf(".xml"))
	{
		strZoneFileName += tr2(".xml");
	}
	QString cstrFullPath = m_strZoneFilePath + strZoneFileName;

	QFile findFile;
	findFile.setFileName(cstrFullPath);
	bool bFind = findFile.exists();
	if (!bFind)
	{
		return false;
	}

	bool bDelteFile = findFile.remove();
	if (bDelteFile)
	{
		return true;
	}
	return false;
}

bool qnzkna::ZoneManage::IZoneManagerImpl::IsExistZoneFile(const QString& strName)
{
	return false;
}

bool qnzkna::ZoneManage::IZoneManagerImpl::GetZoneListByAirLineID(const int& nAirLineID, QStringList& ZoneIDList)
{
	auto itr = m_RelationAirlineAndZoneMap.find(nAirLineID);
	if (itr != m_RelationAirlineAndZoneMap.end())
	{
		ZoneIDList = itr->second;
		return true;
	}
	else
	{
		return false;
	}
}

bool qnzkna::ZoneManage::IZoneManagerImpl::RemoveRelationByLineID(const int& nAirLineID)
{
	auto itr = m_RelationAirlineAndZoneMap.find(nAirLineID);
	if (itr != m_RelationAirlineAndZoneMap.end())
	{
		m_RelationAirlineAndZoneMap.erase(itr);
		return RemoveRelationFromDB(nAirLineID);
	}
	else
	{
		return false;
	}
}

bool qnzkna::ZoneManage::IZoneManagerImpl::SetRelationByLineID(const int& nAirLineID, const QStringList& ZoneIDList)
{
	auto itr = m_RelationAirlineAndZoneMap.find(nAirLineID);
	if (itr != m_RelationAirlineAndZoneMap.end())
	{
		itr->second = ZoneIDList;
		bool success = RemoveRelationFromDB(nAirLineID);
		if (success == false)
		{
			return false;
		}
		return AddRelationFromDB(nAirLineID,ZoneIDList);
	}
	else
	{
		m_RelationAirlineAndZoneMap.insert(std::make_pair(nAirLineID, ZoneIDList));
		return AddRelationFromDB(nAirLineID, ZoneIDList);
	}
}

bool qnzkna::ZoneManage::IZoneManagerImpl::RemoveCoverZoneBase()
{
	auto FeatureNodeItor = m_FeatureNode_Cover.begin();
	while (FeatureNodeItor != m_FeatureNode_Cover.end())
	{
		m_FeatureNode_Parent->removeChild(FeatureNodeItor->second);
		FeatureNodeItor++;
	}
	m_FeatureNode_Cover.clear();
	return true;
}

bool qnzkna::ZoneManage::IZoneManagerImpl::setCoverisShow(bool isShow)
{
	auto FeatureNodeItor = m_FeatureNode_Cover.begin();
	while (FeatureNodeItor != m_FeatureNode_Cover.end())
	{
		FeatureNodeItor->second->setNodeMask(isShow ? 1 : 0);
		FeatureNodeItor++;
	}
	return true;
}

void qnzkna::ZoneManage::IZoneManagerImpl::createADrawAble_3d(std::string polyID, int polyType, unsigned int _color, float _color_A)
{
	delADrawAble_3d(polyID);

	IEarth* IEarthPtr = IEarth::GetEarth();
	osgEarth::MapNode* mapNode = (osgEarth::MapNode*)(IEarthPtr->GetPtr(IEarth::EPtrType_EarthMapNode));
	if (m_FeatureNodeParent_draw_3d == nullptr)
	{
		m_FeatureNodeParent_draw_3d = new osg::Group;
		mapNode->addChild(m_FeatureNodeParent_draw_3d);
	}

	if (m_drawAble_3D != nullptr)
	{
		cancleADrawAble(polyID);
	}

	switch (polyType)
	{
	case 0:
	{
		m_drawAble_3D = new drawAble_Circle;
		m_drawAble_3D->setID(polyID);
		m_drawAble_3D->setMapNode(mapNode);
		m_drawAble_3D->setGroup(m_FeatureNodeParent_draw_3d);
		m_drawAble_3D->setColor(_color, _color_A);
	}
	break;
	case 1:
	{

	}
	break;
	case 2:
	{

	}
	break;
	case 3:
	{
		m_drawAble_3D = new drawAble_Polygon;
		m_drawAble_3D->setID(polyID);
		m_drawAble_3D->setMapNode(mapNode);
		m_drawAble_3D->setGroup(m_FeatureNodeParent_draw_3d);
		m_drawAble_3D->setColor(_color, _color_A);
	}
	break;
	case 4:
	{
		m_drawAble_3D = new drawAble_Rect;
		m_drawAble_3D->setID(polyID);
		m_drawAble_3D->setMapNode(mapNode);
		m_drawAble_3D->setGroup(m_FeatureNodeParent_draw_3d);
		m_drawAble_3D->setColor(_color, _color_A);
	}
	break;
	case 8:
	{
		m_drawAble_3D = new drawAble_LineString;
		m_drawAble_3D->setID(polyID);
		m_drawAble_3D->setMapNode(mapNode);
		m_drawAble_3D->setGroup(m_FeatureNodeParent_draw_3d);
		m_drawAble_3D->setColor(_color, _color_A);
	}
	break;
	default:
		break;
	}
}

void qnzkna::ZoneManage::IZoneManagerImpl::createADrawAble_3d(std::string polyID, int polyType, unsigned int _color, float _color_A, double _width, double _height, float _angle)
{
	delADrawAble_3d(polyID);

	IEarth* IEarthPtr = IEarth::GetEarth();
	osgEarth::MapNode* mapNode = (osgEarth::MapNode*)(IEarthPtr->GetPtr(IEarth::EPtrType_EarthMapNode));
	if (m_FeatureNodeParent_draw_3d == nullptr)
	{
		m_FeatureNodeParent_draw_3d = new osg::Group;
		mapNode->addChild(m_FeatureNodeParent_draw_3d);
	}

	if (m_drawAble_3D != nullptr)
	{
		cancleADrawAble(polyID);
	}

	switch (polyType)
	{
	case 0:
	{
		m_drawAble_3D = new drawAble_Circle;
		m_drawAble_3D->setID(polyID);
		m_drawAble_3D->setMapNode(mapNode);
		m_drawAble_3D->setGroup(m_FeatureNodeParent_draw_3d);
		m_drawAble_3D->setColor(_color, _color_A);
	}
	break;
	case 4:
	{
		m_drawAble_3D = new drawAble_Rect;
		m_drawAble_3D->setID(polyID);
		m_drawAble_3D->setMapNode(mapNode);
		m_drawAble_3D->setGroup(m_FeatureNodeParent_draw_3d);
		m_drawAble_3D->setColor(_color, _color_A);
		m_drawAble_3D->setRectInfo(_width, _height, _angle);
	}
	break;
	}
}

void qnzkna::ZoneManage::IZoneManagerImpl::endAndSaveCreateADrawAble_3d(std::string polyID)
{
	if (m_drawAble_3D != nullptr && m_drawAble_3D->getID() == polyID)
	{
		m_FeatureNodeMap_draw_3d[m_drawAble_3D->getID()] = m_drawAble_3D->getFeatureNode();
		m_FeatrueNodePosMap_draw_3d[m_drawAble_3D->getID()] = m_drawAble_3D->getPoints();
		m_drawAble_3D->~drawAble_Base();
		m_drawAble_3D = nullptr;
	}
}

void qnzkna::ZoneManage::IZoneManagerImpl::cancleADrawAble(std::string polyID)
{
	if (m_drawAble_3D != nullptr)
	{
		m_FeatureNodeParent_draw_3d->removeChild(m_drawAble_3D->getFeatureNode());
		m_drawAble_3D->~drawAble_Base();
		m_drawAble_3D = nullptr;
		delADrawAble_3d(polyID);
	}
}

void qnzkna::ZoneManage::IZoneManagerImpl::delADrawAble_3d(std::string polyID)
{
	auto drawablePtr = m_FeatureNodeMap_draw_3d.find(polyID);
	if (drawablePtr != m_FeatureNodeMap_draw_3d.end())
	{
		m_FeatureNodeParent_draw_3d->removeChild(drawablePtr->second);
		m_FeatureNodeMap_draw_3d.erase(drawablePtr);
	}
}

void qnzkna::ZoneManage::IZoneManagerImpl::addEditor_forDrawAble_3d(std::string polyID)
{
	osgEarth::MapNode* mapNode = (osgEarth::MapNode*)(IEarth::GetEarth()->GetPtr(IEarth::EPtrType_EarthMapNode));
	if (mapNode == nullptr)
		return;
	if (m_Editor_drawAble_3d != nullptr)
	{
		m_edited_drawAble_3d_name = "";
		int childs = m_Editor_drawAble_3d->getNumChildren();
		m_Editor_drawAble_3d->removeChildren(0, childs);
		mapNode->removeChild(m_Editor_drawAble_3d);
		m_Editor_drawAble_3d = nullptr;
	}
	auto drawAblePtr = m_FeatureNodeMap_draw_3d.find(polyID);
	if (drawAblePtr == m_FeatureNodeMap_draw_3d.end())
		return;
	auto featureNodePtr = drawAblePtr->second;
	if (dynamic_cast<osgEarth::Annotation::FeatureNode*>(featureNodePtr) != nullptr)
	{
		m_Editor_drawAble_3d = new osgEarth::Annotation::FeatureEditor(dynamic_cast<osgEarth::Annotation::FeatureNode*>(featureNodePtr));
	}
	else if (dynamic_cast<osgEarth::Annotation::CircleNode*>(featureNodePtr) != nullptr)
	{
		m_Editor_drawAble_3d = new osgEarth::Annotation::CircleNodeEditor(dynamic_cast<osgEarth::Annotation::CircleNode*>(featureNodePtr));
	}
	else if (dynamic_cast<osgEarth::Annotation::RectangleNode*>(featureNodePtr) != nullptr)
	{
		m_Editor_drawAble_3d = new osgEarth::Annotation::RectangleNodeEditor(dynamic_cast<osgEarth::Annotation::RectangleNode*>(featureNodePtr));
	}
	mapNode->addChild(m_Editor_drawAble_3d);
	m_edited_drawAble_3d_name = polyID;
}

void qnzkna::ZoneManage::IZoneManagerImpl::addEditor_forDrawAble_3d(const CZoneBase & stData)
{
	osgEarth::MapNode* mapNode = (osgEarth::MapNode*)(IEarth::GetEarth()->GetPtr(IEarth::EPtrType_EarthMapNode));
	if (mapNode == nullptr)
		return;
	if (m_edited_FeatureNodeParent == nullptr)
	{
		m_edited_FeatureNodeParent = new osg::Group;
		mapNode->addChild(m_edited_FeatureNodeParent);
	}
	if (m_Editor_drawAble_3d != nullptr)
	{
		m_edited_FeatureNodeParent->removeChild(m_edited_featureNode);
		m_edited_drawAble_3d_name = "";
		int childs = m_Editor_drawAble_3d->getNumChildren();
		m_Editor_drawAble_3d->removeChildren(0, childs);
		mapNode->removeChild(m_Editor_drawAble_3d);
		m_Editor_drawAble_3d = nullptr;
	}
	switch (stData.nZoneType)
	{
	case 0:
	{
		osgEarth::Style* geomStyle = new osgEarth::Style;

		osgEarth::Symbology::Color tempCo(GETR(stData.nLineColor) / 255.0, GETG(stData.nLineColor) / 255.0, GETB(stData.nLineColor) / 255.0, 0.5);

		geomStyle->getOrCreate<osgEarth::Symbology::PolygonSymbol>()
			->fill()->color() = osgEarth::Symbology::Color(tempCo);
		geomStyle->getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke().mutable_value().color() = tempCo;
		geomStyle->getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke()->width() = 4;
		geomStyle->getOrCreate<osgEarth::Symbology::PointSymbol>()->size() = 8;
		geomStyle->getOrCreate<osgEarth::Symbology::PointSymbol>()->fill().mutable_value().color() = tempCo;

		geomStyle->getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->clamping() = osgEarth::Symbology::AltitudeSymbol::CLAMP_TO_TERRAIN;
		geomStyle->getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->technique() = osgEarth::Symbology::AltitudeSymbol::TECHNIQUE_GPU;
		geomStyle->getOrCreate<osgEarth::Symbology::AltitudeSymbol>()
			->technique() = osgEarth::Symbology::AltitudeSymbol::TECHNIQUE_DRAPE;
		geomStyle->getOrCreate<osgEarth::Symbology::RenderSymbol>()->depthOffset().mutable_value().enabled() = true;

		geomStyle->getOrCreate<osgEarth::Symbology::AltitudeSymbol>()
			->verticalOffset() = 0.1;

		double height = 0.0;
		mapNode->getTerrain()->getHeight(mapNode->getMapSRS(), stData.stRoundParameter.dLon, stData.stRoundParameter.dLat, &height);
		osg::Vec3d pos3(stData.stRoundParameter.dLon, stData.stRoundParameter.dLat, height);
		m_edited_featureNode = new osgEarth::Annotation::CircleNode(mapNode,
			osgEarth::GeoPoint::GeoPoint(mapNode->getMapSRS()->getGeographicSRS(), pos3,
				osgEarth::AltitudeMode::ALTMODE_RELATIVE),
			stData.stRoundParameter.nRadius, *geomStyle);
		m_Editor_drawAble_3d = new osgEarth::Annotation::CircleNodeEditor(dynamic_cast<osgEarth::Annotation::CircleNode*>(m_edited_featureNode));
	}
	break;
	case 3:
	{
		osgEarth::Style* geomStyle = new osgEarth::Style;

		osgEarth::Symbology::Color tempCo(GETR(stData.nLineColor) / 255.0, GETG(stData.nLineColor) / 255.0, GETB(stData.nLineColor) / 255.0, 0.5);
		geomStyle->getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke().mutable_value().color() = tempCo;
		geomStyle->getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke()->width() = 4;
		geomStyle->getOrCreate<osgEarth::Symbology::PointSymbol>()->size() = 8;

		geomStyle->getOrCreate<osgEarth::Symbology::PolygonSymbol>()
			->fill()->color() = osgEarth::Symbology::Color(tempCo);
		geomStyle->getOrCreate<osgEarth::Symbology::AltitudeSymbol>()
			->clamping() = osgEarth::Symbology::AltitudeSymbol::CLAMP_TO_TERRAIN;
		geomStyle->getOrCreate<osgEarth::Symbology::AltitudeSymbol>()
			->technique() = osgEarth::Symbology::AltitudeSymbol::TECHNIQUE_DRAPE;
		geomStyle->getOrCreate<osgEarth::Symbology::AltitudeSymbol>()
			->verticalOffset() = 0.1;

		std::vector<osg::Vec3d> points;

		for (auto point : stData.stZonePointVec)
		{
			double height = 0.0;
			mapNode->getTerrain()->getHeight(mapNode->getMapSRS(), point.dLon, point.dLat, &height);
			points.push_back(osg::Vec3d(point.dLon, point.dLat, height));
		}

		osgEarth::Symbology::Geometry* geoMery = osgEarth::Symbology::Geometry::create(osgEarth::Symbology::Geometry::TYPE_POLYGON, &points);

		osgEarth::Features::Feature* feature = new osgEarth::Features::Feature(geoMery, mapNode->getMapSRS()->getGeographicSRS());
		m_edited_featureNode = new osgEarth::Annotation::FeatureNode(mapNode, feature, *geomStyle);
		m_Editor_drawAble_3d = new osgEarth::Annotation::FeatureEditor(dynamic_cast<osgEarth::Annotation::FeatureNode*>(m_edited_featureNode));
	}
	break;
	case 4:
	{
		osgEarth::Style* geomStyle = new osgEarth::Style;
		osgEarth::Symbology::Color tempCo(GETR(stData.nLineColor) / 255.0, GETG(stData.nLineColor) / 255.0, GETB(stData.nLineColor) / 255.0, 0.5);
		geomStyle->getOrCreate<osgEarth::Symbology::PolygonSymbol>()
			->fill()->color() = osgEarth::Symbology::Color(tempCo);
		geomStyle->getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke().mutable_value().color() = tempCo;
		geomStyle->getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke()->width() = 4;
		geomStyle->getOrCreate<osgEarth::Symbology::PointSymbol>()->size() = 8;
		geomStyle->getOrCreate<osgEarth::Symbology::PointSymbol>()->fill().mutable_value().color() = tempCo;

		geomStyle->getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->clamping() = osgEarth::Symbology::AltitudeSymbol::CLAMP_TO_TERRAIN;
		geomStyle->getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->technique() = osgEarth::Symbology::AltitudeSymbol::TECHNIQUE_GPU;
		geomStyle->getOrCreate<osgEarth::Symbology::AltitudeSymbol>()
			->technique() = osgEarth::Symbology::AltitudeSymbol::TECHNIQUE_DRAPE;
		geomStyle->getOrCreate<osgEarth::Symbology::RenderSymbol>()->depthOffset().mutable_value().enabled() = true;

		geomStyle->getOrCreate<osgEarth::Symbology::AltitudeSymbol>()
			->verticalOffset() = 0.1;

		double heightDev = 0.0;
		mapNode->getTerrain()->getHeight(mapNode->getMapSRS(), stData.stRoundParameter.dLon, stData.stRoundParameter.dLat, &heightDev);
		osg::Vec3d pos3(stData.stRoundParameter.dLon, stData.stRoundParameter.dLat, heightDev);
		double width = stData.stRoundParameter.nRaidusDuan;
		double height = stData.stRoundParameter.nRaidusChang;
		float radius = stData.stRoundParameter.nNorthAngel;
		osgEarth::GeoPoint geoPoint(mapNode->getMapSRS()->getGeographicSRS(), pos3,
			osgEarth::AltitudeMode::ALTMODE_RELATIVE);

		m_edited_featureNode = new osgEarth::Annotation::RectangleNode(mapNode,
			osgEarth::GeoPoint::GeoPoint(mapNode->getMapSRS()->getGeographicSRS(), pos3, osgEarth::AltitudeMode::ALTMODE_RELATIVE),
			width, height, *geomStyle);
		dynamic_cast<osgEarth::Annotation::RectangleNode*>(m_edited_featureNode)->setLocalRotation(osg::Quat(osg::inDegrees(-radius), osg::Vec3d(0.0, 0.0, 1.0)));

		m_Editor_drawAble_3d = new osgEarth::Annotation::RectangleNodeEditor(dynamic_cast<osgEarth::Annotation::RectangleNode*>(m_edited_featureNode));

	}
	break;
	case 8:
	{
		osgEarth::Style* geomStyle = new osgEarth::Style;
		osgEarth::Symbology::Color tempCo(GETR(stData.nLineColor) / 255.0, GETG(stData.nLineColor) / 255.0, GETB(stData.nLineColor) / 255.0, 0.5);
		geomStyle->getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke().mutable_value().color() = tempCo;
		geomStyle->getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke()->width() = 4;
		geomStyle->getOrCreate<osgEarth::Symbology::PointSymbol>()->size() = 8;
		geomStyle->getOrCreate<osgEarth::Symbology::PointSymbol>()->fill().mutable_value().color() = tempCo;

		geomStyle->getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->clamping() = osgEarth::Symbology::AltitudeSymbol::CLAMP_TO_TERRAIN;

		geomStyle->getOrCreate<osgEarth::Symbology::AltitudeSymbol>()
			->technique() = osgEarth::Symbology::AltitudeSymbol::TECHNIQUE_DRAPE;
		geomStyle->getOrCreate<osgEarth::Symbology::RenderSymbol>()->depthOffset().mutable_value().enabled() = true;
		geomStyle->getOrCreate<osgEarth::Symbology::AltitudeSymbol>()
			->verticalOffset() = 0.1;

		std::vector<osg::Vec3d> points;

		for (auto point : stData.stZonePointVec)
		{
			double height = 0.0;
			mapNode->getTerrain()->getHeight(mapNode->getMapSRS(), point.dLon, point.dLat, &height);
			points.push_back(osg::Vec3d(point.dLon, point.dLat, height));
		}

		osgEarth::Symbology::Geometry* geoMery = osgEarth::Symbology::Geometry::create(osgEarth::Symbology::Geometry::TYPE_LINESTRING, &points);

		osgEarth::Features::Feature* feature = new osgEarth::Features::Feature(geoMery, mapNode->getMapSRS()->getGeographicSRS());
		m_edited_featureNode = new osgEarth::Annotation::FeatureNode(mapNode, feature, *geomStyle);
		m_Editor_drawAble_3d = new osgEarth::Annotation::FeatureEditor(dynamic_cast<osgEarth::Annotation::FeatureNode*>(m_edited_featureNode));

	}
	break;
	default:
		break;
	}

	m_edited_FeatureNodeParent->addChild(m_edited_featureNode);
	mapNode->addChild(m_Editor_drawAble_3d);
}

void qnzkna::ZoneManage::IZoneManagerImpl::endEditing_forDrawAble_3d()
{
	osgEarth::MapNode* mapNode = (osgEarth::MapNode*)(IEarth::GetEarth()->GetPtr(IEarth::EPtrType_EarthMapNode));
	if (mapNode == nullptr)
		return;
	if (m_Editor_drawAble_3d != nullptr)
	{
		m_edited_FeatureNodeParent->removeChild(m_edited_featureNode);
		m_edited_drawAble_3d_name = "";
		int childs = m_Editor_drawAble_3d->getNumChildren();
		m_Editor_drawAble_3d->removeChildren(0, childs);
		mapNode->removeChild(m_Editor_drawAble_3d);
		m_Editor_drawAble_3d = nullptr;
	}
}

void qnzkna::ZoneManage::IZoneManagerImpl::delEditor_forDrawAble_3d(std::string polyID)
{
	osgEarth::MapNode* mapNode = (osgEarth::MapNode*)(IEarth::GetEarth()->GetPtr(IEarth::EPtrType_EarthMapNode));
	if (mapNode == nullptr)
		return;
	if (m_Editor_drawAble_3d != nullptr)
	{
		mapNode->removeChild(m_Editor_drawAble_3d);
		m_Editor_drawAble_3d = nullptr;
		m_edited_drawAble_3d_name = "";
	}
}

bool qnzkna::ZoneManage::IZoneManagerImpl::SetZoneShowScale(const int& nZoneType, const double& dHeightScale, const double& dWidthScale, const int&nDiaphaneity)
{
	if (nZoneType == CZoneBase::LINE)
	{
		m_dHeightScale = dHeightScale;
		m_dWidthScale = dWidthScale;
		m_nDiaphaneity = nDiaphaneity;
		for (auto itr:m_ZoneBaseVec_Activated)
		{
			QString strID = QString::fromLocal8Bit(itr.strID);
			if (itr.nZoneType == nZoneType)
			{
				if (itr.bShow == true)
				{
					Remove3DZone(strID);
					Add3DZone(itr);
				}
			}
		}
		return true;
	}
	return false;
}

bool qnzkna::ZoneManage::IZoneManagerImpl::SetZoneShowScaleDefault(const int& nZoneType)
{
	if (nZoneType == CZoneBase::LINE)
	{
		m_dHeightScale = 1;
		m_dWidthScale = 1;
		m_nDiaphaneity = -1;
		for (auto itr : m_ZoneBaseVec_Activated)
		{
			QString strID = QString::fromLocal8Bit(itr.strID);
			if (itr.nZoneType == nZoneType)
			{
				if (itr.bShow == true)
				{
					Remove3DZone(strID);
					Add3DZone(itr);
				}
			}
		}
		return true;
	}
	return false;
}

std::vector<int> qnzkna::ZoneManage::IZoneManagerImpl::GetLableColor()
{
	return m_vLableColorVec;
}

bool qnzkna::ZoneManage::IZoneManagerImpl::addLableColor(std::vector<int> Color)
{
	bool clear = ClearLableColorDB();
	if (clear)
	{
		if (AddLableColorDB(Color))
		{
			m_vLableColorVec = Color;
		}
	}
	return false;
}

std::vector<std::vector<shared_ptr<AStar::PlanNode>>> qnzkna::ZoneManage::IZoneManagerImpl::createGridsNode(
	const std::vector<double>& gridMapParams, const QGeoCoordinate& gridMapLeftup, const CZoneBase& obstacleZone)
{
	std::vector<std::vector<shared_ptr<AStar::PlanNode>>> GridsMap;
	GridsMap.clear();
	m_pObstacleArea.clear();

	if (gridMapParams.size() < 5) return GridsMap;

	int gridWidth = SAFE_DISTANCE / 2;
	int mapLength = gridMapParams[0], mapWidth = gridMapParams[1];
	int angleLength = gridMapParams[2], angleWidth = gridMapParams[3], angleLine = gridMapParams[4];

	QVector<int> isObIndexVec;
	int i = 0;
	for (int lp = 0; lp < mapLength; lp += gridWidth) {
		QGeoCoordinate pointToLeftup = gridMapLeftup.atDistanceAndAzimuth(lp, angleLength);
		std::vector<shared_ptr<AStar::PlanNode>> pointsLine;
		pointsLine.clear();
		int j = 0;
		bool isWOb = false;

		QVector<int> temp_isObIndexVec;
		if (isObIndexVec.size()) {
			temp_isObIndexVec = isObIndexVec;
		}

		isObIndexVec.clear();
		for (int wp = 0; wp < mapWidth; wp += gridWidth) {
			QGeoCoordinate pointRightdown = pointToLeftup.atDistanceAndAzimuth(wp, angleWidth);
			QGeoCoordinate gridCenterPoint = pointRightdown.atDistanceAndAzimuth(gridWidth * sqrt(2) / 2, angleLine);

			auto m_node = std::make_shared<AStar::PlanNode>(gridCenterPoint.longitude(), gridCenterPoint.latitude(), i, j);

			pointsLine.push_back(m_node);

			if (isWOb) {
				pointsLine[j]->setObstacle();
				isWOb = false;
			}

			if (isPointInZone(gridCenterPoint, obstacleZone)) {
				m_pObstacleArea.push_back(casic_vector3d(gridCenterPoint.longitude(), gridCenterPoint.latitude(), 0));
				pointsLine[j]->setObstacle();

				if (j - 1 >= 0) {
					pointsLine[j - 1]->setObstacle();
				}

				if (wp + gridWidth < mapWidth) {
					isWOb = true;
				}

				if (i - 1 >= 0) {
					GridsMap[i - 1][j]->setObstacle();
				}

				if (lp + gridWidth < mapLength) {
					isObIndexVec.push_back(j);
				}
			}
			j++;
		}

		for (const auto& index : temp_isObIndexVec) {
			pointsLine[index]->setObstacle();
		}

		GridsMap.push_back(pointsLine);
		i++;
	}

	return GridsMap;
}

void qnzkna::ZoneManage::IZoneManagerImpl::pathPlanning(casic_vector3d start, casic_vector3d goal, std::vector<double>& fly_param, std::vector<casic_vector3d>& searchPointList)
{
	auto sg1 = casic_vector3d(start.x, goal.y, goal.z);
	auto sg2 = casic_vector3d(goal.x, start.y, goal.z);
	QGeoCoordinate point1(start.y, start.x);
	QGeoCoordinate point2(sg1.y, sg1.x);
	QGeoCoordinate point3(goal.y, goal.x);
	QGeoCoordinate point4(sg2.y, sg2.x);

	double length_ = point1.distanceTo(point2);
	double width_ = point1.distanceTo(point4);
	double alt_ = start.z;

	std::vector<std::vector<std::vector<double>>> gridMap;
	for (int i = 0; i < m_pGridNode.size(); i++) {
		std::vector<std::vector<double>> tmp;
		for (int j = 0; j < m_pGridNode[i].size(); j++) {
			double lon = m_pGridNode[i][j]->coordinateX();
			double lat = m_pGridNode[i][j]->coordinateY();
			double index_x = m_pGridNode[i][j]->indexX();
			double index_y = m_pGridNode[i][j]->indexY();
			double is_ibstacle = 0;
			if (m_pGridNode[i][j]->isObstacle()) is_ibstacle = 1;
			double tarray[] = { lon, lat , index_x , index_y , is_ibstacle };
			std::vector<double> tmpNode(tarray, tarray + sizeof(tarray) / sizeof(tarray[0]));
			tmp.push_back(tmpNode);
		}
		gridMap.push_back(tmp);
	}

	AStar astar;
	astar.ObstacleUAVs = m_pObstacleArea;
	astar.GridMap = gridMap;
	astar.setFlyParam(fly_param[0], fly_param[1]);
	std::vector<casic_vector3d>posVec;
	astar.init(length_, width_, alt_, 1, start, goal);
	astar.pathPlan(posVec);

	fly_param.push_back(astar.getFlyTime());
	fly_param.push_back(astar.getGoalisFound() ? 1 : 0);

	searchPointList.push_back(start);
	for (auto pos_itor = posVec.rbegin(); pos_itor != posVec.rend(); ++pos_itor) {
		searchPointList.push_back(*pos_itor);
	}
	searchPointList.push_back(goal);
}

void qnzkna::ZoneManage::IZoneManagerImpl::sparsePath(std::vector<casic_vector3d> &SearchPointVec)
{
	double angleDiffMax = 10;

	if (SearchPointVec.size() == 0)
		return;
	auto pointItor = SearchPointVec.begin();
	QGeoCoordinate point1(pointItor->y, pointItor->x, pointItor->z);
	pointItor++;

	if (pointItor == SearchPointVec.end()) {
		return;
	}

	auto point2Itor = pointItor;
	QGeoCoordinate point2(pointItor->y, pointItor->x, pointItor->z);
	pointItor++;
	for (pointItor; pointItor != SearchPointVec.end();) {
		QGeoCoordinate point3(pointItor->y, pointItor->x, pointItor->z);
		double angle1_2 = point1.azimuthTo(point2);
		double angle2_3 = point2.azimuthTo(point3);
		double angleDiff = fabs(angle1_2 - angle2_3);
		if (angleDiff < angleDiffMax) {
			pointItor = SearchPointVec.erase(point2Itor);
			point2 = point3;
			point2Itor = pointItor;
			pointItor++;
		}
		else {
			point1 = point2;
			point2 = point3;
			point2Itor = pointItor;
			pointItor++;
		}
	}
}

bool qnzkna::ZoneManage::IZoneManagerImpl::isPointInZone(const QGeoCoordinate& point, const CZoneBase& obstacleZone)
{
	bool isInZone = false;
	std::vector<QPointF> zonePoints;
	QGeoCoordinate geoCenter(obstacleZone.stRoundParameter.dLat, obstacleZone.stRoundParameter.dLon);
	switch (obstacleZone.nZoneType) {
	case CZoneBase::CIRCLE: {
		if (point.distanceTo(geoCenter) <= obstacleZone.stRoundParameter.nRadius) {
			isInZone = true;
		}
	}break;
	case CZoneBase::PIE: {
		if (point.distanceTo(geoCenter) <= obstacleZone.stRoundParameter.nRadius) {
			double x, y;
			double radius = obstacleZone.stRoundParameter.nRadius;
			double startangle = obstacleZone.stRoundParameter.nStartAngel;
			double endangle = obstacleZone.stRoundParameter.nEndAngel;
			LY_ZoneManagerServive::Get().L_X(obstacleZone.stRoundParameter.dLon, obstacleZone.stRoundParameter.dLat, &x, &y);

			double yx, yy, xx, xy;
			yx = x + radius * sin(startangle * PI / 180.);
			yy = y + radius * cos(startangle * PI / 180.);
			xx = x + radius * sin(endangle * PI / 180.);
			xy = y + radius * cos(endangle * PI / 180.);

			double ll, bb, lll, bbb;
			LY_ZoneManagerServive::Get().X_L(yx, yy, &ll, &bb);
			LY_ZoneManagerServive::Get().X_L(xx, xy, &lll, &bbb);

			QGeoCoordinate startGeo(ll, bb);
			QGeoCoordinate endGeo(lll, bbb);
			double startAngel = geoCenter.azimuthTo(startGeo);
			double endAngel = geoCenter.azimuthTo(endGeo);
			double curAngle = geoCenter.azimuthTo(point);
			if (startAngel <= curAngle && curAngle <= endAngel) {
				isInZone = true;
			}
		}
	}break;
	case CZoneBase::ELLIPSE: {
		double duijiaoDis = std::sqrt(obstacleZone.stRoundParameter.nRaidusChang*obstacleZone.stRoundParameter.nRaidusChang +
			obstacleZone.stRoundParameter.nRaidusDuan*obstacleZone.stRoundParameter.nRaidusDuan);
		double angle = std::asin(obstacleZone.stRoundParameter.nRaidusDuan / duijiaoDis) * 180 / PI;
		QGeoCoordinate p1 = geoCenter.atDistanceAndAzimuth(duijiaoDis, obstacleZone.stRoundParameter.nAngelChang - angle);
		QGeoCoordinate p2 = geoCenter.atDistanceAndAzimuth(duijiaoDis, obstacleZone.stRoundParameter.nAngelChang + angle);
		QGeoCoordinate p3 = geoCenter.atDistanceAndAzimuth(duijiaoDis, obstacleZone.stRoundParameter.nAngelChang - angle + 180);
		QGeoCoordinate p4 = geoCenter.atDistanceAndAzimuth(duijiaoDis, obstacleZone.stRoundParameter.nAngelChang + angle + 180);

		zonePoints.clear();
		zonePoints.push_back(QPointF(p1.longitude(), p1.latitude()));
		zonePoints.push_back(QPointF(p2.longitude(), p2.latitude()));
		zonePoints.push_back(QPointF(p3.longitude(), p3.latitude()));
		zonePoints.push_back(QPointF(p4.longitude(), p4.latitude()));

	}break;
	case CZoneBase::POLY: {
		zonePoints.clear();
		for (const auto& poi : obstacleZone.stZonePointVec) {
			zonePoints.push_back(QPointF(poi.dLon, poi.dLat));
		}
	}break;
	case CZoneBase::RECTANGLE: {
		double duijiaoDis = std::sqrt(obstacleZone.stRoundParameter.nRaidusChang*obstacleZone.stRoundParameter.nRaidusChang +
			obstacleZone.stRoundParameter.nRaidusDuan*obstacleZone.stRoundParameter.nRaidusDuan) / 2;
		double angle = std::asin(obstacleZone.stRoundParameter.nRaidusDuan / 2 / duijiaoDis) * 180 / PI;
		QGeoCoordinate p1 = geoCenter.atDistanceAndAzimuth(duijiaoDis, angle + obstacleZone.stRoundParameter.nNorthAngel);
		QGeoCoordinate p2 = geoCenter.atDistanceAndAzimuth(duijiaoDis, 180 - angle + obstacleZone.stRoundParameter.nNorthAngel);
		QGeoCoordinate p3 = geoCenter.atDistanceAndAzimuth(duijiaoDis, 180 + angle + obstacleZone.stRoundParameter.nNorthAngel);
		QGeoCoordinate p4 = geoCenter.atDistanceAndAzimuth(duijiaoDis, 360 - angle + obstacleZone.stRoundParameter.nNorthAngel);

		zonePoints.clear();
		zonePoints.push_back(QPointF(p1.longitude(), p1.latitude()));
		zonePoints.push_back(QPointF(p2.longitude(), p2.latitude()));
		zonePoints.push_back(QPointF(p3.longitude(), p3.latitude()));
		zonePoints.push_back(QPointF(p4.longitude(), p4.latitude()));

	}break;
	case CZoneBase::CIRCLERING: {
		if (point.distanceTo(geoCenter) <= obstacleZone.stRoundParameter.nRaidusChang &&
			point.distanceTo(geoCenter) >= obstacleZone.stRoundParameter.nRaidusDuan) {
			isInZone = true;
		}
	}break;
	case CZoneBase::PIERING: {
		if (point.distanceTo(geoCenter) <= obstacleZone.stRoundParameter.nRaidusChang &&
			point.distanceTo(geoCenter) >= obstacleZone.stRoundParameter.nRaidusDuan) {
			double x, y;
			double radius = obstacleZone.stRoundParameter.nRaidusChang;
			double startangle = obstacleZone.stRoundParameter.nStartAngel;
			double endangle = obstacleZone.stRoundParameter.nEndAngel;
			LY_ZoneManagerServive::Get().L_X(obstacleZone.stRoundParameter.dLon, obstacleZone.stRoundParameter.dLat, &x, &y);

			double yx, yy, xx, xy;
			yx = x + radius * sin(startangle * PI / 180.);
			yy = y + radius * cos(startangle * PI / 180.);
			xx = x + radius * sin(endangle * PI / 180.);
			xy = y + radius * cos(endangle * PI / 180.);

			double ll, bb, lll, bbb;
			LY_ZoneManagerServive::Get().X_L(yx, yy, &ll, &bb);
			LY_ZoneManagerServive::Get().X_L(xx, xy, &lll, &bbb);

			QGeoCoordinate startGeo(ll, bb);
			QGeoCoordinate endGeo(lll, bbb);

			double startAngel = geoCenter.azimuthTo(startGeo);
			double endAngel = geoCenter.azimuthTo(endGeo);
			double curAngle = geoCenter.azimuthTo(point);
			if (startAngel <= curAngle && curAngle <= endAngel) {
				isInZone = true;
			}
		}
	}break;
	default:
		break;
	}

	if (zonePoints.size() > 0) {
		int count = zonePoints.size();
		int nCross = 0;
		for (int i = 0; i < count; i++)
		{
			QPointF p1 = zonePoints[i];
			QPointF p2 = zonePoints[(i + 1) % count];

			if (p1.y() == p2.y())
				continue;

			if (point.latitude() < std::min(p1.y(), p2.y()))
				continue;

			if (point.latitude() >= std::max(p1.y(), p2.y()))
				continue;

			double x = (point.latitude() - p1.y()) * (p2.x() - p1.x()) / (p2.y() - p1.y()) + p1.x();

			if (x > point.longitude())
				nCross++;
		}

		isInZone = (nCross % 2 == 1);
	}

	return isInZone;
}

void qnzkna::ZoneManage::IZoneManagerImpl::LoadZoneRuleFromDB()
{
	if (!m_db.isOpen())
	{
		LyMessageBox::warning(nullptr, tr2("警告"), tr2("数据库连接失败!"));
		return;
	}

	QSqlQuery query;
	QString strSQL = "select JJWLGJJL,JJQYGJJL,WRJPZGJJL,DDSJ,GDC from `t_zone_rule`";
	bool success = query.exec(strSQL);
	if (success)
	{
		while (query.next())
		{
			QString str1 = tr2(query.value(0).toString().toLatin1().data());
			QString str2 = tr2(query.value(1).toString().toLatin1().data());
			QString str3 = tr2(query.value(2).toString().toLatin1().data());
			QString str4 = tr2(query.value(3).toString().toLatin1().data());
			QString str5 = tr2(query.value(4).toString().toLatin1().data());

			m_ZoneBaseRule.nZoneSafeDis = str1.toInt();
			m_ZoneBaseRule.nZoneDangerDis = str2.toInt();
			m_ZoneBaseRule.nUAVSafeDis = str3.toInt();
			m_ZoneBaseRule.nWaitTime = str4.toInt();
			m_ZoneBaseRule.nAltDiff = str5.toInt();
		}
	}
}

bool qnzkna::ZoneManage::IZoneManagerImpl::EditZoneRuleToDB(const CZoneBaseRule& stData)
{
	QString strSql("UPDATE t_zone_rule set ");
	strSql += "`JJWLGJJL` = "	+ QString::number(stData.nZoneSafeDis) + ",";
	strSql += "`JJQYGJJL` = "	+ QString::number(stData.nZoneDangerDis) + ",";
	strSql += "`WRJPZGJJL` = "	+ QString::number(stData.nUAVSafeDis) + ",";
	strSql += "`DDSJ` = "		+ QString::number(stData.nWaitTime) + ",";
	strSql += "`GDC` = "		+ QString::number(stData.nAltDiff);
	QSqlQuery query;
	return query.exec(strSql);
}

void qnzkna::ZoneManage::IZoneManagerImpl::LoadRelationFromDB()
{
	if (!m_db.isOpen())
	{
		LyMessageBox::warning(nullptr, tr2("警告"), tr2("数据库连接失败!"));
		return;
	}

	m_RelationAirlineAndZoneMap.clear();
	QSqlQuery query;
	QString strSQL = "select QYNM,GLHXID from `t_zone_airline`";
	bool success = query.exec(strSQL);
	if (success)
	{
		while (query.next())
		{
			QString str1 = tr2(query.value(0).toString().toLocal8Bit().data());
			int nID = tr2(query.value(1).toString().toLatin1().data()).toInt();

			auto itr = m_RelationAirlineAndZoneMap.find(nID);
			if (itr == m_RelationAirlineAndZoneMap.end())
			{
				QStringList strIDList;
				strIDList.push_back(str1);

				m_RelationAirlineAndZoneMap.insert(std::make_pair(nID, strIDList));
			}
			else
			{
				itr->second.push_back(str1);
			}
		}
	}
}

bool qnzkna::ZoneManage::IZoneManagerImpl::AddRelationFromDB(const int& nAirLineID, const QStringList& ZoneIDList)
{
	for (auto itr:ZoneIDList)
	{
		QString strSql = "insert into `t_zone_airline`(QYNM, GLHXID) values(";
		strSql += "'" + itr + "',";
		strSql += QString::number(nAirLineID);
		strSql += ")";
		QSqlQuery query;
		bool success = query.exec(strSql);
		if (success == false)
		{
			return false;
		}
	}
	return true;
}

bool qnzkna::ZoneManage::IZoneManagerImpl::RemoveRelationFromDB(const int& nAirLineID)
{
	QSqlQuery query;
	QString strSQL = "delete from `t_zone_airline` where GLHXID =";
	strSQL += QString::number(nAirLineID);
	return query.exec(strSQL);
}

void qnzkna::ZoneManage::IZoneManagerImpl::LoadLableColorDB()
{
	if (!m_db.isOpen())
	{
		LyMessageBox::warning(nullptr, tr2("警告"), tr2("数据库连接失败!"));
		return;
	}
	m_vLableColorVec.clear();
	QSqlQuery query;
	QString strSQL = "select BJCOLOR,ZTCOLOR,BKCOLOR,BJTMD,ZTTMD,BKTMD,ZTDX from `t_zone_lablecolor`";
	bool success = query.exec(strSQL);
	if (success)
	{
		while (query.next())
		{
			QString str1 = tr2(query.value(0).toString().toLatin1().data());
			QString str2 = tr2(query.value(1).toString().toLatin1().data());
			QString str3 = tr2(query.value(2).toString().toLatin1().data());
			QString str4 = tr2(query.value(3).toString().toLatin1().data());
			QString str5 = tr2(query.value(4).toString().toLatin1().data());
			QString str6 = tr2(query.value(5).toString().toLatin1().data());
			QString str7 = tr2(query.value(6).toString().toLatin1().data());

			m_vLableColorVec.push_back(str1.toInt());
			m_vLableColorVec.push_back(str2.toInt());
			m_vLableColorVec.push_back(str3.toInt());
			m_vLableColorVec.push_back(str4.toInt());
			m_vLableColorVec.push_back(str5.toInt());
			m_vLableColorVec.push_back(str6.toInt());
			m_vLableColorVec.push_back(str7.toInt());
		}
	}
}

bool qnzkna::ZoneManage::IZoneManagerImpl::AddLableColorDB(std::vector<int> Color)
{
	QString strSql = "insert into `Agri`.`t_zone_lablecolor`(BJCOLOR,ZTCOLOR,BKCOLOR,BJTMD,ZTTMD,BKTMD,ZTDX) values(";
	for (auto itr : Color)
	{
		if (itr==Color[Color.size()-1])
		{
			strSql += QString::number(itr);
		}
		else {
			strSql += QString::number(itr) + ',';
		}
	}
	strSql += ")";
	QSqlQuery query;
	bool success = query.exec(strSql);
	if (success == false)
	{
		return false;
	}
	return true;
}

bool qnzkna::ZoneManage::IZoneManagerImpl::ClearLableColorDB()
{
	QSqlQuery query;
	QString strSQL = "DELETE FROM `t_zone_lablecolor`";
	bool success = query.exec(strSQL);
	if (!success)
	{
		LyMessageBox::warning(nullptr, tr2("警告"), tr2("数据库标签颜色删除失败!"));
		return false;
	}
	return true;
}

void qnzkna::ZoneManage::IZoneManagerImpl::LoadZoneScaleIni()
{
	QSettings settings("../data/conf/ZoneShowScale.ini", QSettings::IniFormat);
	settings.setIniCodec(QTextCodec::codecForName("utf-8"));
	settings.beginGroup("ZONE_SCALE_SETTING");

	double d1(0), d2(0);
	int n1(0);
	if (settings.contains("WidthScale") && settings.value("WidthScale").canConvert(QVariant::Type::Double))
	{
		d1 = settings.value("WidthScale", "").toDouble();
	}
	if (settings.contains("HeightScale") && settings.value("HeightScale").canConvert(QVariant::Type::Double))
	{
		d2 = settings.value("HeightScale", "").toDouble();
	}
	if (settings.contains("Diaphaneity") && settings.value("Diaphaneity").canConvert(QVariant::Type::Int))
	{
		n1 = settings.value("Diaphaneity", "").toInt();
	}
	settings.endGroup();

	m_dHeightScale = d1;
	m_dWidthScale = d2;
	m_nDiaphaneity = n1;
}


void qnzkna::ZoneManage::IZoneManagerImpl::LoadLineDB()
{
	if (!m_db.isOpen())
	{
		LyMessageBox::warning(nullptr, tr2("警告"), tr2("数据库连接失败!"));
		return;
	}

	QSqlQuery query;
	QString strSql = "SELECT `HXBH`, `HXMC`,`WRJBH`, `HXLX`, `SFXS` ,`GLHDID` , `KSSJ` , `JSSJ` , `ZXZT` ,`QFCD` ,`JLCD` FROM `KG_HXLB`";
	bool success = query.exec(strSql);
	if (success)
	{
		m_AirTrafficLineMap.clear();

		while (query.next())
		{
			QString str1 = tr2(query.value(0).toString().toLatin1().data());
			QString str2 = tr2(query.value(1).toString().toLocal8Bit().data());
			QString str3 = tr2(query.value(2).toString().toLocal8Bit().data());
			QString str4 = tr2(query.value(3).toString().toLatin1().data());
			QString str5 = tr2(query.value(4).toString().toLatin1().data());
			QString str6 = tr2(query.value(5).toString().toLocal8Bit().data());
			QString str7 = tr2(query.value(6).toString().toLatin1().data());
			QString str8 = tr2(query.value(7).toString().toLatin1().data());
			QString str9 = tr2(query.value(8).toString().toLatin1().data());
			QString str10 = tr2(query.value(9).toString().toLocal8Bit().data());
			QString str11 = tr2(query.value(10).toString().toLocal8Bit().data());

			CAirTrafficLine stLine;
			stLine.nID = str1.toInt();
			strncpy(stLine.strName, str2.toLocal8Bit(), MAX_AIR_LEN_1);
			strncpy(stLine.strUAVID, str3.toLocal8Bit(), MAX_AIR_LEN_1);
			stLine.cType = str4.toInt();
			stLine.cShowFlag = str5.toInt();
			stLine.strPlatId = str6;
			stLine.llStartTime = str7.toLongLong();
			stLine.llEndTime = str8.toLongLong();
			stLine.nStates = str9.toInt();
			strncpy(stLine.strTakeOffID, str10.toLocal8Bit(), MAX_AIR_LEN_48);
			strncpy(stLine.strLandID, str11.toLocal8Bit(), MAX_AIR_LEN_48);

			strSql = "SELECT `No`, `HXBH`, `JD`, `WD`, `GD`, `SD`, `XTSJ`, `ZWSX`, `JLD`, `HXDLX`, `ZSDASJ` FROM `KG_HDLB` WHERE `HXBH` = ";
			strSql += str1;
			QSqlQuery queryPoint;
			success = queryPoint.exec(strSql);
			while (queryPoint.next())
			{
				QString str01 = tr2(queryPoint.value(0).toString().toLatin1().data());
				QString str02 = tr2(queryPoint.value(1).toString().toLatin1().data());
				QString str03 = tr2(queryPoint.value(2).toString().toLatin1().data());
				QString str04 = tr2(queryPoint.value(3).toString().toLatin1().data());
				QString str05 = tr2(queryPoint.value(4).toString().toLatin1().data());
				QString str06 = tr2(queryPoint.value(5).toString().toLatin1().data());
				QString str07 = tr2(queryPoint.value(6).toString().toLatin1().data());
				QString str08 = tr2(queryPoint.value(7).toString().toLatin1().data());
				QString str09 = tr2(queryPoint.value(8).toString().toLatin1().data());
				QString str10 = tr2(queryPoint.value(9).toString().toLatin1().data());
				QString str11 = tr2(queryPoint.value(10).toString().toLatin1().data());

				CAirTrafficLinePoint stPoint;
				stPoint.nNo = str01.toInt();
				stPoint.nLineID = str02.toInt();
				stPoint.dLon = str03.toDouble();
				stPoint.dLat = str04.toDouble();
				stPoint.dAlt = str05.toDouble();
				stPoint.dSpeed = str06.toDouble();
				stPoint.dHoverTime = str07.toDouble();
				stPoint.cTurnMode = str08.toInt();
				stPoint.cLanding = str09.toInt();
				stPoint.cType = str10.toInt();
				stPoint.llReachTime = str11.toLongLong();

				stLine.LinePointVec.push_back(stPoint);
			}

			m_AirTrafficLineMap.insert(std::make_pair(stLine.nID, stLine));
		}
	}
}

bool qnzkna::ZoneManage::IZoneManagerImpl::AddlineDB(const CAirTrafficLine data)
{
	if (IsLineExistedDB(data.nID))
	{
		return false;
	}

	QString strSql("INSERT INTO KG_HXLB (`HXBH`, `HXMC`, `WRJBH`,`HXLX`, `SFXS`, `GLHDID`, `KSSJ`, `JSSJ`, `ZXZT`,`QFCD` ,`JLCD` ) VALUES (");
	strSql += QString::number(data.nID) + ",";
	strSql += "'" + QString::fromLocal8Bit(data.strName) + "',";
	strSql += "'" + QString::fromLocal8Bit(data.strUAVID) + "',";
	strSql += QString::number(data.cType) + ",";
	strSql += QString::number(data.cShowFlag) + ",";
	strSql += "'" + data.strPlatId + "',";
	strSql += QString::number(data.llStartTime) + ",";
	strSql += QString::number(data.llEndTime) + ",";
	strSql += QString::number(data.cType) + ", ";
	strSql += "'" + QString::fromLocal8Bit(data.strTakeOffID) + "',";
	strSql += "'" + QString::fromLocal8Bit(data.strLandID) + "'";

	strSql += ")";

	QSqlQuery query;
	bool success = query.exec(strSql);

	for (auto point : data.LinePointVec)
	{
		AddLinePointDB(point);
	}

	return success;
}

bool qnzkna::ZoneManage::IZoneManagerImpl::EditLineDB(const CAirTrafficLine data)
{
	if (!IsLineExistedDB(data.nID))
	{
		return false;
	}

	QString strSql("UPDATE KG_HXLB set ");
	strSql += "`HXMC` = '" + QString::fromLocal8Bit(data.strName) + "',";
	strSql += "`WRJBH` = '" + QString::fromLocal8Bit(data.strUAVID) + "',";
	strSql += "`HXLX` = " + QString::number(data.cType) + ",";
	strSql += "`SFXS` = " + QString::number(data.cShowFlag) + ",";
	strSql += "`GLHDID` = '" + data.strPlatId + "',";
	strSql += "`KSSJ` = " + QString::number(data.llStartTime) + ",";
	strSql += "`JSSJ` = " + QString::number(data.llEndTime) + ",";
	strSql += "`ZXZT` = " + QString::number(data.nStates) + ", ";
	strSql += "`QFCD` = '" + QString::fromLocal8Bit(data.strTakeOffID) + "',";
	strSql += "`JLCD` = '" + QString::fromLocal8Bit(data.strLandID) + "' ";
	strSql += "where `HXBH` = " + QString::number(data.nID);

	QSqlQuery query;
	bool success = query.exec(strSql);
	RemoveLinePointDB(data.nID);
	for (auto point : data.LinePointVec)
	{
		AddLinePointDB(point);
	}

	return success;
}

bool qnzkna::ZoneManage::IZoneManagerImpl::RemovelineDB(const int nID)
{
	if (!IsLineExistedDB(nID))
	{
		return false;
	}

	QString strSql("DELETE FROM KG_HXLB where HXBH = ");
	strSql += QString::number(nID);

	QSqlQuery query;
	bool success = query.exec(strSql);
	RemoveLinePointDB(nID);

	return success;

}

bool qnzkna::ZoneManage::IZoneManagerImpl::IsLineExistedDB(const int nID)
{
	QString strSql("SELECT  * FROM `KG_HXLB` WHERE `HXBH` = ");
	strSql += QString::number(nID);
	QSqlQuery query;
	bool success = query.exec(strSql);
	if (success == false)
	{
		return false;
	}
	if (query.size() == 0)
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool qnzkna::ZoneManage::IZoneManagerImpl::SetLineShowStatesDB(const int nLineID, const bool bShow)
{
	if (!IsLineExistedDB(nLineID))
	{
		return false;
	}

	QString strSql("UPDATE KG_HXLB set ");
	strSql += "`SFXS` = " + QString::number(bShow) + " ";
	strSql += "where `HXBH` = " + QString::number(nLineID);
	QSqlQuery query;
	bool success = query.exec(strSql);
	return success;
}

bool qnzkna::ZoneManage::IZoneManagerImpl::AddLinePointDB(const CAirTrafficLinePoint data)
{

	QSqlQuery query;
	int nNoMax = GetLinePointMaxID();

	QString strSql("INSERT INTO KG_HDLB ( KG_HDLB.`No`,KG_HDLB.HXBH,KG_HDLB.JD,KG_HDLB.WD,KG_HDLB.GD,KG_HDLB.SD,KG_HDLB.XTSJ,KG_HDLB.ZWSX,KG_HDLB.JLD,KG_HDLB.HXDLX,KG_HDLB.ZSDASJ ) VALUES (");
	strSql += QString::number(nNoMax) + ",";
	strSql += QString::number(data.nLineID) + ",";
	strSql += QString::number(data.dLon, 'f', 7) + ",";
	strSql += QString::number(data.dLat, 'f', 7) + ",";
	strSql += QString::number(data.dAlt, 'f', 2) + ",";
	strSql += QString::number(data.dSpeed, 'f', 2) + ",";
	strSql += QString::number(data.dHoverTime, 'f', 2) + ",";
	strSql += QString::number(data.cTurnMode) + ",";
	strSql += QString::number(data.cLanding) + ",";
	strSql += QString::number(data.cType) + ",";
	strSql += QString::number(data.llReachTime);
	strSql += " )";

	bool success = query.exec(strSql);
	return success;
}

bool qnzkna::ZoneManage::IZoneManagerImpl::AddLinePointsDB(const CAirTrafficLinePointVec data)
{
	for (auto itr : data)
	{
		AddLinePointDB(itr);
	}
	return true;
}

bool qnzkna::ZoneManage::IZoneManagerImpl::EditLinePointDB(const CAirTrafficLinePoint data)
{
	if (!IsLinePointExistedDB(data.nLineID, data.nNo))
	{
		return false;
	}

	QString strSql("UPDATE KG_HDLB set ");
	strSql += "`HXBH` = " + QString::number(data.nLineID) + ",";
	strSql += "`JD` = " + QString::number(data.dLon, 'f', 7) + ",";
	strSql += "`WD` = " + QString::number(data.dLat, 'f', 7) + ",";
	strSql += "`GD` = " + QString::number(data.dAlt, 'f', 2) + ",";
	strSql += "`SD` = " + QString::number(data.dSpeed, 'f', 2) + ",";
	strSql += "`XTSJ` = " + QString::number(data.dHoverTime, 'f', 2) + ",";
	strSql += "`ZWSX` = " + QString::number(data.cTurnMode) + ",";
	strSql += "`JLD` = " + QString::number(data.cLanding) + ",";
	strSql += "`HXDLX` = " + QString::number(data.cLanding) + ",";
	strSql += "`ZSDASJ` =  " + QString::number(data.cLanding) + " ";
	strSql += "where `No` = " + QString::number(data.nNo);

	QSqlQuery query;
	bool success = query.exec(strSql);
	return success;
}

bool qnzkna::ZoneManage::IZoneManagerImpl::InsertLinePointDB(const CAirTrafficLinePoint data)
{
	updatePointsIndexFormNow(data.nNo, data.nLineID, 1);

	QSqlQuery query;

	QString strSql("INSERT INTO KG_HDLB (`No`, `HXBH`, `JD`, `WD`, `GD`, `SD`, `XTSJ`, `GDKZ`, `BJ`, `HXSX`, `ZWSX`, `JLD`, `HXDLX`, `HDMC`, `ZSDASJ`) VALUES (");
	strSql += QString::number(data.nNo) + ",";
	strSql += QString::number(data.nLineID) + ",";
	strSql += QString::number(data.dLon, 'f', 7) + ",";
	strSql += QString::number(data.dLat, 'f', 7) + ",";
	strSql += QString::number(data.dAlt, 'f', 2) + ",";
	strSql += QString::number(data.dSpeed, 'f', 2) + ",";
	strSql += QString::number(data.dHoverTime, 'f', 2) + ",";
	strSql += QString::number(data.cTurnMode) + ",";
	strSql += QString::number(data.cLanding);
	strSql += QString::number(data.cType) + ",";
	strSql += QString::number(data.llReachTime) + " ";
	strSql += ")";

	bool success = query.exec(strSql);
	return success;
}

bool qnzkna::ZoneManage::IZoneManagerImpl::RemoveLinePointDB(const int nLineID)
{
	QString strSql("DELETE FROM KG_HDLB where `HXBH` = ");
	strSql += QString::number(nLineID);

	QSqlQuery query;
	bool success = query.exec(strSql);
	return success;
}

bool qnzkna::ZoneManage::IZoneManagerImpl::RemoveLinePointDB(const int nLineID, const int nPointID)
{
	if (!IsLinePointExistedDB(nLineID, nPointID))
	{
		return false;
	}

	QString strSql("DELETE FROM KG_HDLB where `No` = ");
	strSql += QString::number(nPointID) + " && `HXBH` = ";
	strSql += QString::number(nLineID);

	QSqlQuery query;
	bool success = query.exec(strSql);

	updatePointsIndexFormNow(nPointID + 1, nLineID, -1);

	return success;
}

bool qnzkna::ZoneManage::IZoneManagerImpl::IsLinePointExistedDB(const int nLineID, const int nPointID)
{
	QString strSql("SELECT  * FROM `KG_HDLB` WHERE `HXBH` = ");
	strSql += QString::number(nLineID) + " && `No` = ";
	strSql += QString::number(nPointID);
	QSqlQuery query;
	bool success = query.exec(strSql);
	if (success == false)
	{
		return false;
	}
	if (query.size() == 0)
	{
		return false;
	}
	else
	{
		return true;
	}
}

int qnzkna::ZoneManage::IZoneManagerImpl::GetLinePointMaxID()
{
	QSqlQuery query;
	int nNoMax = 0;
	QString str = ("SELECT MAX(`No`) `No` from KG_HDLB ");
	bool success = query.exec(str);
	if (success)
	{
		while (query.next())
		{
			QString str01 = tr2(query.value(0).toString().toLatin1().data());
			nNoMax = str01.toInt() + 1;
		}
	}
	return nNoMax;
}

void qnzkna::ZoneManage::IZoneManagerImpl::updatePointsIndexFormNow(const int nowIndex, const int nLineID, const int disNum)
{
	int maxIndex = GetLinePointMaxID();
	int curDis = disNum;

	if (curDis < 0)
	{
		int curIndex = nowIndex;
		while (curIndex <= maxIndex)
		{
			if (IsLinePointExistedDB(nLineID, curIndex))
			{
				QString updateStr = ("UPDATE KG_HDLB set No = ");
				updateStr = updateStr + "'" + QString::number(curIndex + curDis) + "' ";
				updateStr = updateStr + "where `HXBH` = '" + QString::number(nLineID) + "' AND `No` = '" + QString::number(curIndex) + "'";

				QSqlQuery query;
				int nNoMax = 0;
				bool success = query.exec(updateStr);
			}
			else
			{
				curDis--;
			}
			curIndex++;
		}
	}
	else
	{
		int curIndex = maxIndex;
		while (curIndex >= nowIndex)
		{
			if (IsLinePointExistedDB(nLineID, curIndex))
			{
				QString updateStr = ("UPDATE KG_HDLB set No = ");
				updateStr = updateStr + "'" + QString::number(curIndex + curDis) + "' ";
				updateStr = updateStr + "where `HXBH` = '" + QString::number(nLineID) + "' AND `No` = '" + QString::number(curIndex) + "'";

				QSqlQuery query;
				int nNoMax = 0;
				bool success = query.exec(updateStr);
			}
			else
			{
				curDis++;
			}
			curIndex--;
		}
	}
}

void qnzkna::ZoneManage::IZoneManagerImpl::OnEarthMessage_Pick(int wParam, void * lParam)
{

	if (m_drawAble_3D != nullptr)
	{
		m_drawAble_3D->OnEarthMessage(wParam, lParam);

		if (wParam == IEarthEventHandler::Earth_RDOUBLECLICK)
		{

			LY_ZoneManagerServiveInterface::Get().SendMsgToAll(IInterface::MSG_Command, 1001001, nullptr);
		}
		else if (wParam == IEarthEventHandler::Earth_LDOUBLECLICK)
		{
			LY_ZoneManagerServiveInterface::Get().SendMsgToAll(IInterface::MSG_Command, 1001002, lParam);
		}
	}

	if (m_Editor_drawAble_3d != nullptr)
	{
		if (wParam == IEarthEventHandler::Earth_RBUTTONDOWN)
		{
			if (dynamic_cast<osgEarth::Annotation::FeatureNode*>(m_edited_featureNode) != nullptr)
			{
				auto tempPtr = dynamic_cast<osgEarth::Annotation::FeatureNode*>(m_edited_featureNode);
				std::vector<osg::Vec3d> tempVecs = tempPtr->getFeature()->getGeometry()->asVector();
				std::vector<casic_vector3d> tempTransVec;
				for (auto vec : tempVecs)
				{
					tempTransVec.push_back(casic_vector3d(vec.x(), vec.y(), vec.z()));
				}
				LY_ZoneManagerServiveInterface::Get().SendMsgToAll(IInterface::MSG_Command, 1001004, &tempTransVec);
			}
			else if (dynamic_cast<osgEarth::Annotation::CircleNode*>(m_edited_featureNode) != nullptr)
			{
				auto tempPtr = dynamic_cast<osgEarth::Annotation::CircleNode*>(m_edited_featureNode);
				osgEarth::Annotation::LocalizedNode;
				osgEarth::GeoPoint po = tempPtr->getPosition();
				std::vector<casic_vector3d> tempVecs;
				tempVecs.push_back(casic_vector3d(po.vec3d().x(), po.vec3d().y(), po.vec3d().z()));
				osgEarth::Distance rasius = tempPtr->getRadius();
				tempVecs.push_back(casic_vector3d(rasius.getValue(), 0, 0));
				LY_ZoneManagerServiveInterface::Get().SendMsgToAll(IInterface::MSG_Command, 1001005, &tempVecs);
			}
			else if (dynamic_cast<osgEarth::Annotation::RectangleNode*>(m_edited_featureNode) != nullptr)
			{
				auto tempPtr = dynamic_cast<osgEarth::Annotation::RectangleNode*>(m_edited_featureNode);
				osgEarth::GeoPoint po = tempPtr->getPosition();
				std::vector<casic_vector3d> tempVecs;
				tempVecs.push_back(casic_vector3d(po.vec3d().x(), po.vec3d().y(), po.vec3d().z()));
				tempVecs.push_back(casic_vector3d(tempPtr->getWidth().getValue(), tempPtr->getHeight().getValue(), 0));
				LY_ZoneManagerServiveInterface::Get().SendMsgToAll(IInterface::MSG_Command, 1001006, &tempVecs);
			}
		}
	}

	return;

}

void qnzkna::ZoneManage::IZoneManagerImpl::LoadAirTrafficLine()
{
	LoadLineDB();
}

CAirTrafficLineMap qnzkna::ZoneManage::IZoneManagerImpl::GetAllAirTrafficLine()
{
	return m_AirTrafficLineMap;
}

CAirTrafficLine qnzkna::ZoneManage::IZoneManagerImpl::GetAllAirTrafficLine(const int &nLineID)
{
	CAirTrafficLine stLine;
	auto itr = m_AirTrafficLineMap.find(nLineID);
	if (itr != m_AirTrafficLineMap.end())
	{
		stLine = itr->second;
	}

	return stLine;
}

int qnzkna::ZoneManage::IZoneManagerImpl::GetAirTrafficLineMaxID()
{
	QSqlQuery query;
	int nNoMax = 0;
	bool success = query.exec("SELECT MAX(`HXBH`) from KG_HXLB");
	if (success)
	{
		while (query.next())
		{
			QString str01 = tr2(query.value(0).toString().toLatin1().data());
			nNoMax = str01.toInt();
		}
	}

	nNoMax = nNoMax + 1;
	return nNoMax;
}

int qnzkna::ZoneManage::IZoneManagerImpl::GetAirTrafficLinePointMaxID()
{
	return GetLinePointMaxID();
}

int qnzkna::ZoneManage::IZoneManagerImpl::GetAirTrafficLinePointMaxID(int nLineID)
{
	QSqlQuery query;
	int nNoMax = 0;
	QString str = ("SELECT MAX(`No`) `No` from KG_HDLB WHERE `HXBH`=");
	str = str + "'" + QString::number(nLineID) + "'";
	bool success = query.exec(str);
	if (success)
	{
		while (query.next())
		{
			QString str01 = tr2(query.value(0).toString().toLatin1().data());
			nNoMax = str01.toInt() + 1;
		}
	}
	return nNoMax;
}

CAirTrafficLinePointVec qnzkna::ZoneManage::IZoneManagerImpl::getAirTrafficLinePointsFornLineID(const int nLineID)
{
	CAirTrafficLinePointVec stLinePoint;
	auto itr = m_AirTrafficLineMap.find(nLineID);
	if (itr != m_AirTrafficLineMap.end())
	{
		stLinePoint = itr->second.LinePointVec;
	}

	return stLinePoint;
}

bool qnzkna::ZoneManage::IZoneManagerImpl::AddAirTrafficLine(const CAirTrafficLine stLine)
{
	if (IsAirTrafficLineExited(stLine.nID) == true)
	{
		return false;
	}

	m_AirTrafficLineMap.insert(std::make_pair(stLine.nID, stLine));
	bool bresult = AddlineDB(stLine);
	return bresult;
}

bool qnzkna::ZoneManage::IZoneManagerImpl::EditAirTrafficLine(const CAirTrafficLine stLine)
{
	auto itr = m_AirTrafficLineMap.find(stLine.nID);
	if (itr != m_AirTrafficLineMap.end())
	{
		itr->second = stLine;
		return EditLineDB(stLine);
	}

	return false;
}

bool qnzkna::ZoneManage::IZoneManagerImpl::RemoveAirTrafficLine(const int nID)
{
	auto itr = m_AirTrafficLineMap.find(nID);
	if (itr != m_AirTrafficLineMap.end())
	{
		m_AirTrafficLineMap.erase(itr);
		return RemovelineDB(nID);
	}

	return false;
}

bool qnzkna::ZoneManage::IZoneManagerImpl::IsAirTrafficLineExited(const int nID)
{
	return IsLineExistedDB(nID);
}

CAirTrafficLinePointVec qnzkna::ZoneManage::IZoneManagerImpl::GetAirTrafficLinePoints(const int nID)
{
	CAirTrafficLinePointVec  PointsVec;
	auto itr = m_AirTrafficLineMap.find(nID);
	if (itr != m_AirTrafficLineMap.end())
	{
		PointsVec = itr->second.LinePointVec;
	}

	return  PointsVec;
}

CAirTrafficLinePoint qnzkna::ZoneManage::IZoneManagerImpl::GetAirTrafficLinePoint(const int nID, const int nPoint)
{
	CAirTrafficLinePoint  LinePoint;

	if (nPoint < 1)
	{
		return LinePoint;
	}

	auto itr = m_AirTrafficLineMap.find(nID);
	if (itr != m_AirTrafficLineMap.end())
	{
		auto itrpoint = itr->second.LinePointVec.begin();
		if (nPoint > itr->second.LinePointVec.size())
		{
			return LinePoint;
		}
		itrpoint += nPoint - 1;
		LinePoint = *itrpoint;
	}

	return  LinePoint;
}

bool qnzkna::ZoneManage::IZoneManagerImpl::AddAirTrafficLinePoint(const CAirTrafficLinePoint stPoint)
{
	auto itr = m_AirTrafficLineMap.find(stPoint.nLineID);
	if (itr != m_AirTrafficLineMap.end())
	{
		itr->second.LinePointVec.push_back(stPoint);
		return AddLinePointDB(stPoint);
	}

	return false;
}

bool qnzkna::ZoneManage::IZoneManagerImpl::RemoveAirTrafficLinePoints(const int nLineID)
{
	auto itr = m_AirTrafficLineMap.find(nLineID);
	if (itr != m_AirTrafficLineMap.end())
	{
		itr->second.LinePointVec.clear();
		return RemoveLinePointDB(nLineID);
	}

	return false;
}

bool qnzkna::ZoneManage::IZoneManagerImpl::RemoveAirTrafficLinePoint(const int nLineID, const int nPoint)
{
	auto itr = m_AirTrafficLineMap.find(nLineID);
	if (itr != m_AirTrafficLineMap.end())
	{
		auto itrPoint = itr->second.LinePointVec.begin();
		if (nPoint > itr->second.LinePointVec.size())
		{
			return false;
		}

		itr->second.LinePointVec.erase(itrPoint);
		return RemoveLinePointDB(nLineID, nPoint);
	}
	return false;
}

bool qnzkna::ZoneManage::IZoneManagerImpl::IsAirTrafficLinePointExited(const int nLineID, const int nPoint)
{
	return false;
}

bool qnzkna::ZoneManage::IZoneManagerImpl::SetAirTrafficLiseShowStates(const int nLineID, const bool bShow)
{
	auto itr = m_AirTrafficLineMap.find(nLineID);
	if (itr != m_AirTrafficLineMap.end())
	{
		itr->second.cShowFlag = bShow;
		return SetLineShowStatesDB(nLineID, bShow);
	}
	return false;
}
