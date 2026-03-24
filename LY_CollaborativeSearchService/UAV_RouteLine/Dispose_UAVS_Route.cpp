#include "Dispose_UAVS_Route.h"
#include "LyMessageBox.h"
#include "QCString.h"
#include "ICollaborativeSearchActivator.h"
#include "LY_CollaborativeSearchService.h"
#include "RoutePlanningDialog.h"

void Dispose_UAVS_Route::setTargetUAVInfos(vector<DISPOSE_ROUTE::target_UAVS_Info>& _targetUAVInfos)
{
	targetUAVInfos.clear();
	targetUAVInfos = _targetUAVInfos;
}

void Dispose_UAVS_Route::setDispose_RouteFirst(DISPOSE_ROUTE::DISPOSE_ROUTE_FIRST _route_first)
{
	route_first = _route_first;
}

void Dispose_UAVS_Route::setHeight_safe_add(int _height_safe_add)
{
	height_safe_add = _height_safe_add;
}

void Dispose_UAVS_Route::setSafeDis(int _safeDis)
{
	safeDis = _safeDis;
}

void Dispose_UAVS_Route::setNoFlyZonePoints(std::vector<LLQPoints> _NoFlyZonePoints)
{
	NoFlyZonePoints = _NoFlyZonePoints;
}

bool Dispose_UAVS_Route::routeLines()
{
	bool isRouted = false;
	route_lines.clear();
	QString strTakeOffArea = LY_CollaborativeSearchService::Get().m_uav_dispose_route_dialog->strTakeOffArea;
	QString strEnterLine = LY_CollaborativeSearchService::Get().m_uav_dispose_route_dialog->strEnterLine;
	QString strOutLine = LY_CollaborativeSearchService::Get().m_uav_dispose_route_dialog->strOutLine;

	if (strTakeOffArea.isEmpty() || strEnterLine.isEmpty() || strOutLine.isEmpty())
	{
		LyMessageBox::warning(NULL, "Dispos_UAV_Route", tr2(" 未选择区域信息"));
		return false;
	}

	ICollaborativeSearchActivator::Get().getPIZoneManageService()->GetZoneBase(strTakeOffArea, takeOff_land_Area);
	LY_CollaborativeSearchService::Get().m_uav_dispose_route_dialog->takeOff_land_Area = takeOff_land_Area;

	int zoneType = 1;
	qnzkna::ZoneManage::CZoneBaseVec AllZoneVec = ICollaborativeSearchActivator::Get().getPIZoneManageService()->GetAllZoneBase();
	QPointF targetPosF;
	if (targetUAVInfos.size() > 0)
	{
		for (auto target_UAVInfo : targetUAVInfos)
		{
			if (target_UAVInfo.target_info.longitude == 0 || target_UAVInfo.target_info.latitude == 0 || target_UAVInfo.target_info.altitude == 0)
			{
				continue;
			}
			for (auto uavInfo : target_UAVInfo.uavsInfos)
			{
				targetPosF = QPointF(uavInfo.longitude_fire, uavInfo.latitude_fire);
				break;
			}

		}
	}
	int zType = qnzkna::ZoneManage::CZoneBase::ZoneProperty::JF;
	noFlyAreaVec.clear();
	for (auto &zone : AllZoneVec)
	{
		std::vector<QPointF> ZonePoints;
		if (zone.nZoneType == qnzkna::ZoneManage::CZoneBase::ZoneBaseType::RECTANGLE)
		{
			qnzkna::ZoneManage::ZonePointVec pointlist;
			ICollaborativeSearchActivator::Get().getPIZoneManageService()->TransformRectangleToPoints(tr2(zone.strID), pointlist);
			zone.stZonePointVec = pointlist;
		}

		if (zone.nProperty == zType)
		{
			noFlyAreaVec.push_back(zone);
		}

		for (auto pos : zone.stZonePointVec)
		{
			ZonePoints.push_back(QPointF(pos.dLon, pos.dLat));
		}
		if (LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->is_point_in_zone(targetPosF, ZonePoints) == true && searchArea.nProperty == 0)
		{
			searchArea = zone;
			LY_CollaborativeSearchService::Get().m_uav_dispose_route_dialog->searchArea = searchArea;
			LY_CollaborativeSearchService::Get().m_uav_dispose_route_dialog->searchZonePoints = ZonePoints;
		}
	}

	noFlyAreaPointVec.clear();
	for (auto nofly : noFlyAreaVec)
	{
		LLQPoints poVec;
		for (auto point : nofly.stZonePointVec)
		{
			poVec.push_back(QPointF(point.dLon, point.dLat));
		}
		noFlyAreaPointVec.push_back(poVec);
	}

	if (searchArea.nZoneType == 4)
	{
		qnzkna::ZoneManage::ZonePointVec pointlist;
		ICollaborativeSearchActivator::Get().getPIZoneManageService()->TransformRectangleToPoints(tr2(searchArea.strID), pointlist);

		searchArea.stZonePointVec = pointlist;
	}

	ICollaborativeSearchActivator::Get().getPIZoneManageService()->GetZoneBase(strEnterLine, minLineZone);

	ICollaborativeSearchActivator::Get().getPIZoneManageService()->GetZoneBase(strOutLine, returnLineZone);

	QPointF lineNearPos;
	QPointF areaNearPos;
	QGeoCoordinate areaNearGeo;
	int xdis = INT_MAX;
	int idis = 0;
	for (auto zonePos : searchArea.stZonePointVec)
	{
		QGeoCoordinate areaGeo(zonePos.dLat, zonePos.dLon);
		for (auto posline : minLineZone.stZonePointVec)
		{
			QGeoCoordinate lineGeo(posline.dLat, posline.dLon);
			double tpDIs = lineGeo.distanceTo(areaGeo);
			if (tpDIs < xdis)
			{
				xdis = tpDIs;
				lineNearPos = QPointF(posline.dLon, posline.dLat);
				areaNearPos = QPointF(areaGeo.longitude(), areaGeo.latitude());
				areaNearGeo = areaGeo;
			}
		}
	}

	QPointF returnNearPos;
	xdis = INT_MAX;
	idis = 0;
	for (auto zonePos : searchArea.stZonePointVec)
	{
		QGeoCoordinate areaGeo(zonePos.dLat, zonePos.dLon);
		for (auto posline : returnLineZone.stZonePointVec)
		{
			QGeoCoordinate lineGeo(posline.dLat, posline.dLon);
			double tpDIs = lineGeo.distanceTo(areaGeo);
			if (tpDIs < xdis)
			{
				xdis = tpDIs;
				returnNearPos = QPointF(posline.dLon, posline.dLat);
			}
		}
	}

	zones_used.clear();
	for (auto target_UAVInfo : targetUAVInfos)
	{
		if (target_UAVInfo.target_info.longitude == 0 || target_UAVInfo.target_info.latitude == 0 || target_UAVInfo.target_info.altitude == 0)
		{
			LyMessageBox::warning(NULL, "Dispos_UAV_Route", tr2("目标-") + tr2(target_UAVInfo.target_info.targetID.data()) + tr2(" 的经纬高为空，需要重新赋值！"));
			continue;
		}

		for (auto uavInfo : target_UAVInfo.uavsInfos)
		{
			if (uavInfo.longitude_fire == 0 || uavInfo.latitude_fire == 0 || uavInfo.altitude_fire == 0)
			{
				LyMessageBox::warning(NULL, "Dispos_UAV_Route", tr2("无人机-") + tr2(uavInfo.uavID.data()) + tr2(" 的占位点经纬高为空，需要重新赋值！"));
				continue;
			}

			casic_vector3d target_firePos(uavInfo.longitude_fire, uavInfo.latitude_fire, uavInfo.altitude_fire);

			casic_vector3d uavPos(uavInfo.longitude, uavInfo.latitude, uavInfo.altitude_fire);

			std::vector<casic_vector3d> temp_routeLine;
			temp_routeLine.clear();
			bool temp_routed = false;
			switch (uavInfo.uavType)
			{
			case DISPOSE_ROUTE::DISPOSE_THROW:
			{

				std::vector<LLQPoints> NoflyzonPoints_temp = NoFlyZonePoints;
				auto zones_usedItor = zones_used.find(uavInfo.altitude_fire);
				if (zones_usedItor != zones_used.end())
				{
					for (auto routedLine : zones_usedItor->second)
					{
						LineTransToZone(routedLine, NoflyzonPoints_temp);
					}
				}

				UAV_Dispose_Route_Base route_base;
				route_base.setNoFlyZonePoints(NoflyzonPoints_temp);
				route_base.noFlyAreaPointVec = noFlyAreaPointVec;

				casic_vector3d lineNearCas3d(lineNearPos.x(), lineNearPos.y(), uavInfo.altitude_fire);
				QGeoCoordinate lineP1(lineNearPos.y(), lineNearPos.x());
				QGeoCoordinate lineP2(target_firePos.y, target_firePos.x);
				double dis = lineP1.distanceTo(lineP2);
				if (lineP1.distanceTo(lineP2)> 5000)
				{
					LyMessageBox::warning(NULL, tr2("提示"),  tr2(" 距离计算异常！"));
					return false;
				}
				route_base.Dispose_route(lineNearCas3d, target_firePos, safeDis);
				std::vector<casic_vector3d> tmpPathPoints;

				double lineHeight = (minLineZone.nBottomhAlt + minLineZone.nAboveAlt) / 2;
				for (auto linepos : minLineZone.stZonePointVec)
				{
					temp_routeLine.push_back(casic_vector3d(linepos.dLon, linepos.dLat, lineHeight));
					LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->flyRoadPointsVec.push_back(casic_vector3d(linepos.dLon, linepos.dLat, lineHeight));
				}

				tmpPathPoints = route_base.getSearchMapPoints();

				for(int pointIndex = tmpPathPoints.size() - 1; pointIndex >= 0; pointIndex--)
				{
					temp_routeLine.push_back(tmpPathPoints[pointIndex]);
				}
				temp_routeLine.push_back(target_firePos);

				UAV_Dispose_Route_Base route_base1;
				route_base1.setNoFlyZonePoints(NoflyzonPoints_temp);
				route_base1.noFlyAreaPointVec = noFlyAreaPointVec;
				casic_vector3d lineReturnCas3d(returnNearPos.x(), returnNearPos.y(), uavInfo.altitude_fire);
				route_base1.Dispose_route(target_firePos, lineReturnCas3d, safeDis);
				tmpPathPoints.clear();
				tmpPathPoints = route_base1.getSearchMapPoints();

				lineHeight = (returnLineZone.nBottomhAlt + returnLineZone.nAboveAlt) / 2;
				for (int pointIndex = tmpPathPoints.size() - 1; pointIndex >= 0; pointIndex--)
				{
					if (pointIndex == 0)
					{
						casic_vector3d returnPos(tmpPathPoints[pointIndex].x, tmpPathPoints[pointIndex].y, lineHeight);
						temp_routeLine.push_back(returnPos);
					}
					else
					{
						temp_routeLine.push_back(tmpPathPoints[pointIndex]);
					}
				}

				std::vector<casic_vector3d>returnPosList;

				for (auto pt = returnLineZone.stZonePointVec.rbegin(); pt != returnLineZone.stZonePointVec.rend(); pt++)
				{
					temp_routeLine.push_back(casic_vector3d(pt->dLon, pt->dLat, lineHeight));
					LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->flyRoadPointsVec.push_back(casic_vector3d(pt->dLon, pt->dLat, uavInfo.altitude_fire));
				}
				LY_CollaborativeSearchService::Get().m_uav_dispose_route_dialog->draw3DDropBoomArea(target_firePos.y, target_firePos.x, target_firePos.z, 5);
				temp_routed = true;
			}
			break;
			case DISPOSE_ROUTE::DISPOSE_FIRE:
			{
				QGeoCoordinate target_GeoPos(target_UAVInfo.target_info.latitude, target_UAVInfo.target_info.longitude);
				QGeoCoordinate targetFire_GeoPos(target_firePos.y, target_firePos.x);
				double angle_target2Frie = target_GeoPos.azimuthTo(targetFire_GeoPos);
				QGeoCoordinate targetFireSec_GeoPos = targetFire_GeoPos.atDistanceAndAzimuth(5, angle_target2Frie);
				casic_vector3d targetFireSec_pos(targetFireSec_GeoPos.longitude(), targetFireSec_GeoPos.latitude(), target_firePos.z);

				std::vector<LLQPoints> NoflyzonPoints_temp = NoFlyZonePoints;
				auto zones_usedItor = zones_used.find(uavInfo.altitude_fire);
				if (zones_usedItor != zones_used.end())
				{
					for (auto routedLine : zones_usedItor->second)
					{
						LineTransToZone(routedLine, NoflyzonPoints_temp);
					}
				}

				UAV_Dispose_Route_Base route_base;
				route_base.setNoFlyZonePoints(NoflyzonPoints_temp);
				route_base.noFlyAreaPointVec = noFlyAreaPointVec;

				casic_vector3d lineNearCas3d(lineNearPos.x(), lineNearPos.y(), uavInfo.altitude_fire);
				route_base.Dispose_route(lineNearCas3d, target_firePos, safeDis);
				std::vector<casic_vector3d> tmpPathPoints;

				double lineHeight = (minLineZone.nBottomhAlt + minLineZone.nAboveAlt) / 2;
				for (auto linepos : minLineZone.stZonePointVec)
				{
					temp_routeLine.push_back(casic_vector3d(linepos.dLon, linepos.dLat, lineHeight));
					LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->flyRoadPointsVec.push_back(casic_vector3d(linepos.dLon, linepos.dLat, 0));
				}

				tmpPathPoints = route_base.getSearchMapPoints();

				for (int pointIndex = tmpPathPoints.size() - 1; pointIndex >= 0; pointIndex--)
				{
					temp_routeLine.push_back(tmpPathPoints[pointIndex]);
				}
				temp_routeLine.push_back(targetFireSec_pos);
				temp_routeLine.push_back(target_firePos);

				UAV_Dispose_Route_Base route_base1;
				route_base1.setNoFlyZonePoints(NoflyzonPoints_temp);
				route_base1.noFlyAreaPointVec = noFlyAreaPointVec;
				casic_vector3d lineReturnCas3d(returnNearPos.x(), returnNearPos.y(), uavInfo.altitude_fire);
				route_base1.Dispose_route(target_firePos, lineReturnCas3d, safeDis);
				tmpPathPoints.clear();
				tmpPathPoints = route_base1.getSearchMapPoints();

				for (int pointIndex = tmpPathPoints.size() - 1; pointIndex >= 0; pointIndex--)
				{
					if (pointIndex == 0)
					{
						casic_vector3d returnPos(tmpPathPoints[pointIndex].x, tmpPathPoints[pointIndex].y, lineHeight);
						temp_routeLine.push_back(returnPos);
					}
					else
					{
						temp_routeLine.push_back(tmpPathPoints[pointIndex]);
					}
				}

				std::vector<casic_vector3d>returnPosList;
				lineHeight = (returnLineZone.nBottomhAlt + returnLineZone.nAboveAlt) / 2;
				for (auto pt = returnLineZone.stZonePointVec.rbegin(); pt != returnLineZone.stZonePointVec.rend(); pt++)
				{
					temp_routeLine.push_back(casic_vector3d(pt->dLon, pt->dLat, lineHeight));
					LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->flyRoadPointsVec.push_back(casic_vector3d(pt->dLon, pt->dLat, 0));
				}

				QColor fireZoneColor(0, 0, 255, 150);
				LY_CollaborativeSearchService::Get().m_uav_dispose_route_dialog->draw3DFireBoom_For_M3_throw(target_firePos, angle_target2Frie + 180, 0, fireZoneColor, 4);
				temp_routed = true;
			}
			break;
			default:
				break;
			}

			if (temp_routed)
			{

				auto zones_usedItor = zones_used.find(uavInfo.altitude_fire);
				if (zones_usedItor != zones_used.end())
				{
					zones_usedItor->second.push_back(temp_routeLine);
				}
				else
				{
					std::vector<std::vector<casic_vector3d>> routedLines;
					routedLines.push_back(temp_routeLine);
					zones_used[uavInfo.altitude_fire] = routedLines;
				}

				if (route_first == DISPOSE_ROUTE::DISPOSE_ROUTE_FIRST_SAFE)
				{
					casic_vector3d lastPos = temp_routeLine[temp_routeLine.size() - 1];
					for (int pointIndex = 0; pointIndex < temp_routeLine.size(); pointIndex++)
					{
						if(pointIndex == 0)
							continue;
						else
						{
							temp_routeLine[pointIndex].z += height_safe_add;
						}
					}
					temp_routeLine.push_back(lastPos);
					route_lines[uavInfo.uavID] = temp_routeLine;
				}
				else
				{
					route_lines[uavInfo.uavID] = temp_routeLine;
				}
			}

		}
	}

	return true;
}

std::map<std::string, std::vector<casic_vector3d>> Dispose_UAVS_Route::getRoutedLines()
{
	return route_lines;
}

void Dispose_UAVS_Route::clearRoutedLine()
{
	route_lines.clear();
	zones_used.clear();

}

void Dispose_UAVS_Route::LineTransToZone(std::vector<casic_vector3d> linePoints, std::vector<LLQPoints>& noflyZones)
{
	for (int i = 0; i < linePoints.size() - 1; i++)
	{
		QGeoCoordinate firstPos(linePoints[i].y, linePoints[i].x);
		QGeoCoordinate secPos(linePoints[i + 1].y, linePoints[i + 1].x);
		double dir = firstPos.azimuthTo(secPos);
		LLQPoints zonePoints;
		QGeoCoordinate point1 = firstPos.atDistanceAndAzimuth(safeDis / 2, dir - 135);
		QGeoCoordinate point2 = firstPos.atDistanceAndAzimuth(safeDis / 2, dir + 135);
		QGeoCoordinate point3 = secPos.atDistanceAndAzimuth(safeDis / 2, dir - 45);
		QGeoCoordinate point4 = secPos.atDistanceAndAzimuth(safeDis / 2, dir + 45);
		zonePoints.push_back(QPointF(point1.longitude(), point1.latitude()));
		zonePoints.push_back(QPointF(point2.longitude(), point2.latitude()));
		zonePoints.push_back(QPointF(point3.longitude(), point3.latitude()));
		zonePoints.push_back(QPointF(point4.longitude(), point4.latitude()));
		noflyZones.push_back(zonePoints);
	}
}
