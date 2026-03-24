#include "Dispose_targetAndUAVS.h"
#include "LY_CollaborativeSearchService.h"
#include "LY_CollaborativeSearchServiceInterface.h"
#include "LyMessageBox.h"
#include <QDateTime>

Dispose_targetAndUAVS_process::Dispose_targetAndUAVS_process()
{
	init();
}

void Dispose_targetAndUAVS_process::init()
{
	MarkTypes_used.clear();
	MarkTypes_used.push_back(ESystemMarkType_Fire);

	fireInfo[50] = 20;
	fireInfo[60] = 25;
	fireInfo[70] = 30;

	mDispose_UAVS_Route.setSafeDis(5);

	loadMarksFormDb();
	loadUAVIDS_fromXml();

}

void Dispose_targetAndUAVS_process::addMarkType_used(ESystemMarkType _markType)
{
	if (findMarkType_used(_markType) == MarkTypes_used.end())
	{
		MarkTypes_used.push_back(_markType);
	}
}

void Dispose_targetAndUAVS_process::delMarkType_used(ESystemMarkType _markType)
{
	auto markTypeItor = findMarkType_used(_markType);
	if (markTypeItor != MarkTypes_used.end())
	{
		markTypeItor = MarkTypes_used.erase(markTypeItor);
	}
}

std::vector<ESystemMarkType>::iterator Dispose_targetAndUAVS_process::findMarkType_used(ESystemMarkType _markType)
{
	return std::find(MarkTypes_used.begin(), MarkTypes_used.end(), _markType);
}

CSystemMarkInfoVec Dispose_targetAndUAVS_process::getMarks_All()
{
	return Marks_All;
}

void Dispose_targetAndUAVS_process::updateMarkList()
{
	std::vector<std::string> lastMarkID_all = Marks_AllID;
	loadMarksFormDb();

	std::vector<std::string> newAddIDs;
	newAddIDs.resize(Marks_AllID.size());
	std::vector<std::string> deletedIds;
	deletedIds.resize(lastMarkID_all.size());

	std::set_difference(Marks_AllID.begin(), Marks_AllID.end(), lastMarkID_all.begin(), lastMarkID_all.end(), newAddIDs.begin());
	std::set_difference(lastMarkID_all.begin(), lastMarkID_all.end(), Marks_AllID.begin(), Marks_AllID.end(), deletedIds.begin());

	for (auto newID : newAddIDs)
	{
		emit(signal_addaMarkID(QString::fromStdString(newID)));
	}
	for (auto delID : deletedIds)
	{
		emit(signal_delaMarkID(QString::fromStdString(delID)));
	}

}

void Dispose_targetAndUAVS_process::addOrUpdateMark(std::string markID)
{
	auto markSerItor = ICollaborativeSearchActivator::Get().getPIMarkManageService();
	if (markSerItor != NULL)
	{
		SystemMarkInfo newMark = markSerItor->GetMark(markID);
		if (newMark.GetID() != "" && findMarkType_used(newMark.GetType()) != MarkTypes_used.end())
		{
			bool newMarkIsHaved = false;
			for (int markIndex = 0; markIndex < Marks_All.size(); markIndex++)
			{
				if (Marks_All[markIndex].GetID() == newMark.GetID())
				{
					Marks_All[markIndex] = newMark;
					newMarkIsHaved = true;
				}
			}
			if (!newMarkIsHaved)
			{
				Marks_All.push_back(newMark);
			}
		}
	}
}

void Dispose_targetAndUAVS_process::loadUAVS_online()
{
	uavIDS.clear();
	auto RealTimeUAVSerItor = ICollaborativeSearchActivator::Get().getPIUAVRealTimeStateManageService();
	if (RealTimeUAVSerItor != NULL)
	{
		qnzkna::UAVRealTimeStateManage::NebulaUAVRealTimeStateMap nebulaUAVRealTimeStateMap;
		if (RealTimeUAVSerItor->GetUAVRealTimeStateList(&nebulaUAVRealTimeStateMap))
		{
			for (const auto& nebulaUAVRealTimeState : nebulaUAVRealTimeStateMap)
			{
				uavIDS.push_back(nebulaUAVRealTimeState.first);
			}
		}
	}

	int id = 110300;
	for (int i = 1; i < 6; i++)
	{
		uavIDS.push_back(std::to_string(id + i));
	}
}

void Dispose_targetAndUAVS_process::loadUAVIDS_fromXml()
{
	uavIDS_Xml.clear();
	uavIDS_Fire.clear();
	uavIDS_Throw.clear();
	auto weaponServiceItor = ICollaborativeSearchActivator::Get().getPWeaponManageService();
	if (weaponServiceItor != NULL)
	{
		qnzkna::WeaponManage::WeaponSystemMap weaponMap;
		if (weaponServiceItor->GetWeaponSystemMap(weaponMap))
		{
			auto weaponMapItor = weaponMap.begin();
			while (weaponMapItor != weaponMap.end())
			{
				auto weapComponentPtr = weaponMapItor->second.GetWeaponComponentMapPtr();
				auto weapComponentItor = weapComponentPtr->begin();
				while (weapComponentItor != weapComponentPtr->end())
				{
					uavIDS_Xml.push_back(weapComponentItor->second.getComponentID());
					weapComponentItor++;
				}
				weaponMapItor++;
			}
		}
	}
	auto sensorItor = ICollaborativeSearchActivator::Get().getPISensorManageService();
	if (sensorItor != NULL)
	{
		for (auto tempUavID : uavIDS_Xml)
		{
			qnzkna::SensorManage::SensorInfoMap tempSensorInfo;
			bool isGetSensorInfo = sensorItor->GetSensorInfosInWeaponComponent(tempUavID, &tempSensorInfo);
			if (isGetSensorInfo)
			{
				auto tempSensorItor = tempSensorInfo.begin();
				while (tempSensorItor != tempSensorInfo.end())
				{
					std::string fireType;
					if (tempSensorItor->second.GetSensorType() == SeneorType)
					{
						tempSensorItor->second.GetSensorParam(DisposeType_Key, fireType);
						if (fireType == DisposeType_Throw)
						{
							uavIDS_Throw.push_back(tempUavID);
						}
						else if (fireType == DisposeType_Fire)
						{
							uavIDS_Fire.push_back(tempUavID);
						}
					}
					tempSensorItor++;
				}
			}
		}
	}

}

std::vector<std::string> Dispose_targetAndUAVS_process::getUAVS_online()
{
	return uavIDS;
}

void Dispose_targetAndUAVS_process::addOrUpdateUAVforMark(std::string markID, std::string uavID)
{
	bool markIDisHaved = false;
	for (auto target_UAV : target_Match_UAVS)
	{
		if (target_UAV.target_info.targetID == markID)
		{
			markIDisHaved = true;

			auto pIMarkServive = ICollaborativeSearchActivator::Get().getPIMarkManageService();
			if (pIMarkServive != NULL)
			{
				SystemMarkInfo curMarkInfo = pIMarkServive->GetMark(markID);
				target_UAV.target_info.longitude = curMarkInfo.GetLon();
				target_UAV.target_info.latitude = curMarkInfo.GetLat();
				if (curMarkInfo.GetHeight() == 0)
				{
					target_UAV.target_info.altitude = LY_CollaborativeSearchService::Get().s_CGetHeight.
						GetHeight(curMarkInfo.GetLon(), curMarkInfo.GetLat());
				}
				else
				{
					target_UAV.target_info.altitude = curMarkInfo.GetHeight();
				}
			}

			bool uavIDisHaved = false;
			for (auto curUAV : target_UAV.uavsInfos)
			{
				if (curUAV.uavID == uavID)
				{
					auto pIUAVRealTimeStateManageService = ICollaborativeSearchActivator::Get().getPIUAVRealTimeStateManageService();
					if (pIUAVRealTimeStateManageService != NULL)
					{
						qnzkna::UAVRealTimeStateManage::RealTimeTelemetryInformation_t uavInfoTemp;
						pIUAVRealTimeStateManageService->GetUAVRealTimeState(uavID, uavInfoTemp);
						curUAV.longitude = uavInfoTemp.telemetryInfo.position.lng;
						curUAV.latitude = uavInfoTemp.telemetryInfo.position.lat;
						curUAV.altitude = uavInfoTemp.telemetryInfo.gpsHeigth;
					}
					uavIDisHaved = true;
					break;
				}
			}
			if (!uavIDisHaved)
			{
				DISPOSE_ROUTE::UAVInfo newUAVInfo;
				newUAVInfo.uavID = uavID;
				auto pIUAVRealTimeStateManageService = ICollaborativeSearchActivator::Get().getPIUAVRealTimeStateManageService();
				if (pIUAVRealTimeStateManageService != NULL)
				{
					qnzkna::UAVRealTimeStateManage::RealTimeTelemetryInformation_t uavInfoTemp;
					pIUAVRealTimeStateManageService->GetUAVRealTimeState(uavID, uavInfoTemp);
					newUAVInfo.longitude = uavInfoTemp.telemetryInfo.position.lng;
					newUAVInfo.latitude = uavInfoTemp.telemetryInfo.position.lat;
					newUAVInfo.altitude = uavInfoTemp.telemetryInfo.gpsHeigth;
					if (std::find(uavIDS_Fire.begin(), uavIDS_Fire.end(), uavID) != uavIDS_Fire.end())
					{
						newUAVInfo.uavType = DISPOSE_ROUTE::DISPOSE_FIRE;
					}
					else if (std::find(uavIDS_Throw.begin(), uavIDS_Throw.end(), uavID) != uavIDS_Throw.end())
					{
						newUAVInfo.uavType = DISPOSE_ROUTE::DISPOSE_THROW;
					}
					target_UAV.uavsInfos.push_back(newUAVInfo);
				}

			}
			break;
		}
	}
	if (!markIDisHaved)
	{
		auto pIMarkServive = ICollaborativeSearchActivator::Get().getPIMarkManageService();
		if (pIMarkServive != NULL)
		{
			SystemMarkInfo curMarkInfo = pIMarkServive->GetMark(markID);
			DISPOSE_ROUTE::target_UAVS_Info target_UAV;
			target_UAV.target_info.longitude = curMarkInfo.GetLon();
			target_UAV.target_info.latitude = curMarkInfo.GetLat();
			if (curMarkInfo.GetHeight() == 0)
			{
				target_UAV.target_info.altitude = LY_CollaborativeSearchService::Get().s_CGetHeight.
					GetHeight(curMarkInfo.GetLon(), curMarkInfo.GetLat());
			}
			else
			{
				target_UAV.target_info.altitude = curMarkInfo.GetHeight();
			}

			DISPOSE_ROUTE::UAVInfo newUAVInfo;
			newUAVInfo.uavID = uavID;
			auto pIUAVRealTimeStateManageService = ICollaborativeSearchActivator::Get().getPIUAVRealTimeStateManageService();
			if (pIUAVRealTimeStateManageService != NULL)
			{
				qnzkna::UAVRealTimeStateManage::RealTimeTelemetryInformation_t uavInfoTemp;
				pIUAVRealTimeStateManageService->GetUAVRealTimeState(uavID, uavInfoTemp);
				newUAVInfo.longitude = uavInfoTemp.telemetryInfo.position.lng;
				newUAVInfo.latitude = uavInfoTemp.telemetryInfo.position.lat;
				newUAVInfo.altitude = uavInfoTemp.telemetryInfo.gpsHeigth;
				if (std::find(uavIDS_Fire.begin(), uavIDS_Fire.end(), uavID) != uavIDS_Fire.end())
				{
					newUAVInfo.uavType = DISPOSE_ROUTE::DISPOSE_FIRE;
				}
				else if (std::find(uavIDS_Throw.begin(), uavIDS_Throw.end(), uavID) != uavIDS_Throw.end())
				{
					newUAVInfo.uavType = DISPOSE_ROUTE::DISPOSE_THROW;
				}

				target_UAV.uavsInfos.push_back(newUAVInfo);
				target_Match_UAVS.push_back(target_UAV);
			}
		}
	}
}

void Dispose_targetAndUAVS_process::delUAVforMark(std::string markID, std::string uavID)
{
	auto target_UAVItor = target_Match_UAVS.begin();
	while (target_UAVItor != target_Match_UAVS.end())
	{
		if (target_UAVItor->target_info.targetID == markID)
		{
			auto uavItor = target_UAVItor->uavsInfos.begin();
			while (uavItor != target_UAVItor->uavsInfos.end())
			{
				if (uavItor->uavID == uavID)
				{
					uavItor = target_UAVItor->uavsInfos.erase(uavItor);
					if (target_UAVItor->uavsInfos.empty())
					{
						target_UAVItor = target_Match_UAVS.erase(target_UAVItor);
					}
					break;
				}
				else
				{
					uavItor++;
				}

			}
			break;
		}
		else
		{
			target_UAVItor++;
		}
	}
}

void Dispose_targetAndUAVS_process::delUAVForAllMark(std::string uavID)
{
	auto target_UAVItor = target_Match_UAVS.begin();
	while (target_UAVItor != target_Match_UAVS.end())
	{
		bool target_UAVItorDeled = false;
		auto uavItor = target_UAVItor->uavsInfos.begin();
		while (uavItor != target_UAVItor->uavsInfos.end())
		{
			if (uavItor->uavID == uavID)
			{
				uavItor = target_UAVItor->uavsInfos.erase(uavItor);
				if (target_UAVItor->uavsInfos.empty())
				{
					target_UAVItor = target_Match_UAVS.erase(target_UAVItor);
					target_UAVItorDeled = true;
				}
				break;
			}
			else
			{
				uavItor++;
			}
		}
		if(!target_UAVItorDeled)
			target_UAVItor++;
	}
}

void Dispose_targetAndUAVS_process::addOrUpdateUAVforMark_unique(std::string markID, std::string uavID)
{
	if (markID.empty() || uavID.empty())
		return;
	delUAVForAllMark(uavID);
	bool markIDisHaved = false;
	for (auto &target_UAV : target_Match_UAVS)
	{
		if (target_UAV.target_info.targetID == markID)
		{
			markIDisHaved = true;

			auto pIMarkServive = ICollaborativeSearchActivator::Get().getPIMarkManageService();
			if (pIMarkServive != NULL)
			{
				SystemMarkInfo curMarkInfo = pIMarkServive->GetMark(markID);
				target_UAV.target_info.longitude = curMarkInfo.GetLon();
				target_UAV.target_info.latitude = curMarkInfo.GetLat();

				short altitude_Geo = LY_CollaborativeSearchService::Get().s_CGetHeight.
					GetHeight(curMarkInfo.GetLon(), curMarkInfo.GetLat());
				if (curMarkInfo.GetHeight() < altitude_Geo)
				{
					target_UAV.target_info.altitude = altitude_Geo;
				}
				else
				{
					target_UAV.target_info.altitude = curMarkInfo.GetHeight();
				}
			}

			DISPOSE_ROUTE::DISPOSE_WAY newUAVType = getUAVType(uavID);

			bool uavIDisHaved = false;
			for (auto & curUAV : target_UAV.uavsInfos)
			{
				DISPOSE_ROUTE::DISPOSE_WAY curUAVType = getUAVType(curUAV.uavID);
				if (curUAVType == newUAVType)
				{
					curUAV.uavID = uavID;
					auto pIUAVRealTimeStateManageService = ICollaborativeSearchActivator::Get().getPIUAVRealTimeStateManageService();
					if (pIUAVRealTimeStateManageService != NULL)
					{
						qnzkna::UAVRealTimeStateManage::RealTimeTelemetryInformation_t uavInfoTemp;
						if (pIUAVRealTimeStateManageService->GetUAVRealTimeState(uavID, uavInfoTemp))
						{
							curUAV.longitude = uavInfoTemp.telemetryInfo.position.lng;
							curUAV.latitude = uavInfoTemp.telemetryInfo.position.lat;
							curUAV.altitude = uavInfoTemp.telemetryInfo.gpsHeigth;
						}
						else
						{
							auto uavPosItor = uavIDSPos.find(uavID);
							if (uavPosItor != uavIDSPos.end())
							{
								curUAV.longitude = uavPosItor->second.x;
								curUAV.latitude = uavPosItor->second.y;
								curUAV.altitude = uavPosItor->second.z;
							}
						}
					}
					uavIDisHaved = true;
					break;
				}
			}
			if (!uavIDisHaved)
			{
				DISPOSE_ROUTE::UAVInfo newUAVInfo;
				newUAVInfo.uavID = uavID;
				auto pIUAVRealTimeStateManageService = ICollaborativeSearchActivator::Get().getPIUAVRealTimeStateManageService();
				if (pIUAVRealTimeStateManageService != NULL)
				{
					qnzkna::UAVRealTimeStateManage::RealTimeTelemetryInformation_t uavInfoTemp;
					if (pIUAVRealTimeStateManageService->GetUAVRealTimeState(uavID, uavInfoTemp))
					{
						newUAVInfo.longitude = uavInfoTemp.telemetryInfo.position.lng;
						newUAVInfo.latitude = uavInfoTemp.telemetryInfo.position.lat;
						newUAVInfo.altitude = uavInfoTemp.telemetryInfo.gpsHeigth;
					}
					else
					{
						auto uavPosItor = uavIDSPos.find(uavID);
						if (uavPosItor != uavIDSPos.end())
						{
							newUAVInfo.longitude = uavPosItor->second.x;
							newUAVInfo.latitude = uavPosItor->second.y;
							newUAVInfo.altitude = uavPosItor->second.z;
						}
					}
					newUAVInfo.uavType = getUAVType(uavID);

					target_UAV.uavsInfos.push_back(newUAVInfo);
				}

			}
			break;
		}
	}
	if (!markIDisHaved)
	{
		auto pIMarkServive = ICollaborativeSearchActivator::Get().getPIMarkManageService();
		if (pIMarkServive != NULL)
		{
			SystemMarkInfo curMarkInfo = pIMarkServive->GetMark(markID);
			DISPOSE_ROUTE::target_UAVS_Info target_UAV;
			target_UAV.target_info.targetID = markID;
			target_UAV.target_info.longitude = curMarkInfo.GetLon();
			target_UAV.target_info.latitude = curMarkInfo.GetLat();
			short altitude_Geo = LY_CollaborativeSearchService::Get().s_CGetHeight.
				GetHeight(curMarkInfo.GetLon(), curMarkInfo.GetLat());
			if (curMarkInfo.GetHeight() < altitude_Geo)
			{
				target_UAV.target_info.altitude = altitude_Geo;
			}
			else
			{
				target_UAV.target_info.altitude = curMarkInfo.GetHeight();
			}

			DISPOSE_ROUTE::UAVInfo newUAVInfo;
			newUAVInfo.uavID = uavID;
			auto pIUAVRealTimeStateManageService = ICollaborativeSearchActivator::Get().getPIUAVRealTimeStateManageService();
			if (pIUAVRealTimeStateManageService != NULL)
			{
				qnzkna::UAVRealTimeStateManage::RealTimeTelemetryInformation_t uavInfoTemp;
				if (pIUAVRealTimeStateManageService->GetUAVRealTimeState(uavID, uavInfoTemp))
				{
					newUAVInfo.longitude = uavInfoTemp.telemetryInfo.position.lng;
					newUAVInfo.latitude = uavInfoTemp.telemetryInfo.position.lat;
					newUAVInfo.altitude = uavInfoTemp.telemetryInfo.gpsHeigth;
				}
				else
				{
					auto uavPosItor = uavIDSPos.find(uavID);
					if (uavPosItor != uavIDSPos.end())
					{
						newUAVInfo.longitude = uavPosItor->second.x;
						newUAVInfo.latitude = uavPosItor->second.y;
						newUAVInfo.altitude = uavPosItor->second.z;
					}
				}

				newUAVInfo.uavType = getUAVType(uavID);

				target_UAV.uavsInfos.push_back(newUAVInfo);
				target_Match_UAVS.push_back(target_UAV);
			}
		}
	}
}

void Dispose_targetAndUAVS_process::allocationUavFire()
{
	for (auto& target_UAVInfo : target_Match_UAVS)
	{
		std::vector<int> throwIndexs, fireIndexs;
		for(int uavIndex = 0; uavIndex < target_UAVInfo.uavsInfos.size(); uavIndex++)
		{
			switch (target_UAVInfo.uavsInfos[uavIndex].uavType)
			{
			case DISPOSE_ROUTE::DISPOSE_THROW:
			{
				throwIndexs.push_back(uavIndex);
			}
			break;
			case DISPOSE_ROUTE::DISPOSE_FIRE:
			{
				fireIndexs.push_back(uavIndex);
			}
			break;
			default:
				break;
			}
		}
		QGeoCoordinate targetGeo(target_UAVInfo.target_info.latitude, target_UAVInfo.target_info.longitude);

		if (!throwIndexs.empty())
		{
			std::vector<int> throwIndex_sort = sortUAVbyAngle(target_UAVInfo.target_info, target_UAVInfo.uavsInfos, throwIndexs);
			int uavCount = throwIndex_sort.size();

			switch (uavCount)
			{
			case 1:
			{
				target_UAVInfo.uavsInfos[throwIndex_sort[0]].longitude_fire = target_UAVInfo.target_info.longitude;
				target_UAVInfo.uavsInfos[throwIndex_sort[0]].latitude_fire = target_UAVInfo.target_info.latitude;
				target_UAVInfo.uavsInfos[throwIndex_sort[0]].altitude_fire = target_UAVInfo.target_info.altitude +
																			target_UAVInfo.uavsInfos[throwIndex_sort[0]].dispose_heiOrDis;
			}
			break;
			case 2:
			{
				QGeoCoordinate uav0Geo(target_UAVInfo.uavsInfos[throwIndex_sort[0]].longitude, target_UAVInfo.uavsInfos[throwIndex_sort[0]].latitude);
				double angleTarget2UAV0 = targetGeo.azimuthTo(uav0Geo);
				QGeoCoordinate uav1Geo(target_UAVInfo.uavsInfos[throwIndex_sort[1]].longitude, target_UAVInfo.uavsInfos[throwIndex_sort[1]].latitude);
				double angleUav0To1 = uav0Geo.azimuthTo(uav1Geo);

				int dis = target_UAVInfo.target_info.size_long >= 5 ? target_UAVInfo.target_info.size_long : 5;
				QGeoCoordinate target0FireGeo = targetGeo.atDistanceAndAzimuth(dis / 2.0, angleUav0To1 + 180);
				QGeoCoordinate target1FireGeo = targetGeo.atDistanceAndAzimuth(dis / 2.0, angleUav0To1);

				target_UAVInfo.uavsInfos[throwIndex_sort[0]].longitude_fire = target0FireGeo.longitude();
				target_UAVInfo.uavsInfos[throwIndex_sort[0]].latitude_fire = target0FireGeo.latitude();
				target_UAVInfo.uavsInfos[throwIndex_sort[0]].altitude_fire = target_UAVInfo.target_info.altitude +
																		target_UAVInfo.uavsInfos[throwIndex_sort[0]].dispose_heiOrDis;

				target_UAVInfo.uavsInfos[throwIndex_sort[1]].longitude_fire = target1FireGeo.longitude();
				target_UAVInfo.uavsInfos[throwIndex_sort[1]].latitude_fire = target1FireGeo.latitude();
				target_UAVInfo.uavsInfos[throwIndex_sort[1]].altitude_fire = target_UAVInfo.target_info.altitude +
																		target_UAVInfo.uavsInfos[throwIndex_sort[1]].dispose_heiOrDis;

			}
			break;
			case 3:
			{
				QGeoCoordinate uav0Geo(target_UAVInfo.uavsInfos[throwIndex_sort[0]].longitude, target_UAVInfo.uavsInfos[throwIndex_sort[0]].latitude);
				QGeoCoordinate uav1Geo(target_UAVInfo.uavsInfos[throwIndex_sort[1]].longitude, target_UAVInfo.uavsInfos[throwIndex_sort[1]].latitude);
				double angleUAV1ToTarget = uav1Geo.azimuthTo(targetGeo);
				double angleUav1To0 = uav1Geo.azimuthTo(uav0Geo);

				int dis = target_UAVInfo.target_info.size_long >= 5 ? target_UAVInfo.target_info.size_long : 5;
				QGeoCoordinate target1Fire_Geo = targetGeo.atDistanceAndAzimuth(dis / 2.0, angleUAV1ToTarget);
				QGeoCoordinate target0Fire_Geo;
				QGeoCoordinate target2Fire_Geo;
				if (angleUav1To0 > angleUAV1ToTarget)
				{
					target0Fire_Geo = target1Fire_Geo.atDistanceAndAzimuth(dis, angleUAV1ToTarget + 180 - 45);
					target2Fire_Geo = target1Fire_Geo.atDistanceAndAzimuth(dis, angleUAV1ToTarget + 180 + 45);
				}
				else
				{
					target0Fire_Geo = target1Fire_Geo.atDistanceAndAzimuth(dis, angleUAV1ToTarget + 180 + 45);
					target2Fire_Geo = target1Fire_Geo.atDistanceAndAzimuth(dis, angleUAV1ToTarget + 180 - 45);
				}
				target_UAVInfo.uavsInfos[throwIndex_sort[0]].longitude_fire = target0Fire_Geo.longitude();
				target_UAVInfo.uavsInfos[throwIndex_sort[0]].latitude_fire = target0Fire_Geo.latitude();
				target_UAVInfo.uavsInfos[throwIndex_sort[0]].altitude_fire = target_UAVInfo.target_info.altitude +
					target_UAVInfo.uavsInfos[throwIndex_sort[0]].dispose_heiOrDis;

				target_UAVInfo.uavsInfos[throwIndex_sort[1]].longitude_fire = target1Fire_Geo.longitude();
				target_UAVInfo.uavsInfos[throwIndex_sort[1]].latitude_fire = target1Fire_Geo.latitude();
				target_UAVInfo.uavsInfos[throwIndex_sort[1]].altitude_fire = target_UAVInfo.target_info.altitude +
					target_UAVInfo.uavsInfos[throwIndex_sort[1]].dispose_heiOrDis;
				target_UAVInfo.uavsInfos[throwIndex_sort[2]].longitude_fire = target2Fire_Geo.longitude();
				target_UAVInfo.uavsInfos[throwIndex_sort[2]].latitude_fire = target2Fire_Geo.latitude();
				target_UAVInfo.uavsInfos[throwIndex_sort[2]].altitude_fire = target_UAVInfo.target_info.altitude +
					target_UAVInfo.uavsInfos[throwIndex_sort[2]].dispose_heiOrDis;
			}
			break;
			}
		}
		if (!fireIndexs.empty())
		{
			std::vector<int> fireIndex_sort = sortUAVbyAngle(target_UAVInfo.target_info, target_UAVInfo.uavsInfos, fireIndexs);

			QGeoCoordinate uavOri(target_UAVInfo.uavsInfos[fireIndex_sort[0]].latitude, target_UAVInfo.uavsInfos[fireIndex_sort[0]].longitude);
			double angleTarget2uavOri = targetGeo.azimuthTo(uavOri);

			double angleFireOri = angleTarget2uavOri;

			int fireDis = target_UAVInfo.uavsInfos[fireIndex_sort[0]].dispose_heiOrDis;
			auto fireHei = 20;

			if (fireInfo.find(fireDis) != fireInfo.end())
			{
				fireHei = fireInfo.find(fireDis)->second;
			}
			QGeoCoordinate uavOriFirePos_Geo = targetGeo.atDistanceAndAzimuth(fireDis, angleFireOri);
			target_UAVInfo.uavsInfos[fireIndex_sort[0]].longitude_fire = uavOriFirePos_Geo.longitude();
			target_UAVInfo.uavsInfos[fireIndex_sort[0]].latitude_fire = uavOriFirePos_Geo.latitude();
			target_UAVInfo.uavsInfos[fireIndex_sort[0]].altitude_fire = target_UAVInfo.target_info.altitude + fireHei;

			if (fireIndex_sort.size() >= 2)
			{
				QGeoCoordinate uav1Geo(target_UAVInfo.uavsInfos[fireIndex_sort[1]].latitude, target_UAVInfo.uavsInfos[fireIndex_sort[1]].longitude);
				double angleTarget2Uav1 = targetGeo.azimuthTo(uav1Geo);
				int dis = (angleTarget2uavOri - angleTarget2Uav1) / std::abs(angleTarget2uavOri - angleTarget2Uav1);

				double angleFire_last = angleFireOri;
				for (int uavIndex = 1; uavIndex < fireIndexs.size(); uavIndex++)
				{
					int fireDis = target_UAVInfo.uavsInfos[fireIndex_sort[uavIndex]].dispose_heiOrDis;
					auto fireHei = 20;

					if (fireInfo.find(fireDis) != fireInfo.end())
					{
						fireHei = fireInfo.find(fireDis)->second;
					}
					double angleFire_now = angleFire_last + dis * 10.0;
					QGeoCoordinate uavFirePos_Geo = targetGeo.atDistanceAndAzimuth(fireDis, angleFire_now);

					target_UAVInfo.uavsInfos[fireIndex_sort[uavIndex]].longitude_fire = uavFirePos_Geo.longitude();
					target_UAVInfo.uavsInfos[fireIndex_sort[uavIndex]].latitude_fire = uavFirePos_Geo.latitude();
					target_UAVInfo.uavsInfos[fireIndex_sort[uavIndex]].altitude_fire = target_UAVInfo.target_info.altitude + fireHei;
				}

			}
		}

	}
}

void Dispose_targetAndUAVS_process::setDispose_RouteFirst(DISPOSE_ROUTE::DISPOSE_ROUTE_FIRST _route_first)
{
	mDispose_UAVS_Route.setDispose_RouteFirst(_route_first);
}

void Dispose_targetAndUAVS_process::setHeight_safe_add(int _height_safe_add)
{
	mDispose_UAVS_Route.setHeight_safe_add(_height_safe_add);
}

void Dispose_targetAndUAVS_process::setSafeDis(int _safeDis)
{
	mDispose_UAVS_Route.setSafeDis(_safeDis);
}

void Dispose_targetAndUAVS_process::setNoFlyZonePoints(std::vector<LLQPoints> _NoFlyZonePoints)
{
	mDispose_UAVS_Route.setNoFlyZonePoints(_NoFlyZonePoints);
}

void Dispose_targetAndUAVS_process::routeLines()
{
	allocationUavFire();
	mDispose_UAVS_Route.setTargetUAVInfos(target_Match_UAVS);
	mDispose_UAVS_Route.routeLines();

}

bool Dispose_targetAndUAVS_process::routeLinesUptoUAV()
{
	updateUav2Mission();
	bool find_routPath_up = false;
	auto routedLines = mDispose_UAVS_Route.getRoutedLines();

	auto routedLineItor = routedLines.begin();
	while (routedLineItor != routedLines.end())
	{

		std::vector<casic_vector3d> routLine = routedLineItor->second;
		std::string uavID = routedLineItor->first;

		auto missionItor = UAVID2Mission.find(uavID);
		if (missionItor != UAVID2Mission.end())
		{
			CNeubulaMission fireMission = missionItor->second;

			auto currMissionPlatVec = fireMission.GetPlatInfoMap();
			vector<int> LineIDVec;
			CNeubulaMissionLineVec missionLineVec = ICollaborativeSearchActivator::Get().getPTaskManageService()->GetMissionLinesByTask(fireMission.GetID());
			for (auto line : missionLineVec)
			{
				int lineID = line.nID;

				LineIDVec.push_back(line.nID);
			}

			int index = 0;

			CNeubulaMissionLine mline;
			if (index < currMissionPlatVec.size())
			{

				QString missionID = (QString::fromStdString(fireMission.GetID()));
				strncpy(mline.strMissionID, missionID.toLocal8Bit().data(), MAX_LINE_LEN_1);

				mline.cType = 0;
				mline.cProtocolType = 0;
				mline.cShowFlag = 1;

				if (index < LineIDVec.size())
					mline.nID = LineIDVec[index];
				else {

					LyMessageBox::information(NULL, tr2("提示"), tr2("规划航线和配置无人机不匹配，请检查配置"));
					return false;
				}
				QString stName = ("HX");
				stName += QString::number(mline.nID);
				strncpy(mline.strName, stName.toLocal8Bit().toStdString().c_str(), MAX_LINE_LEN_1);
				int index_i = 0;
				if (routLine.empty())
					continue;

				int startPosHei = routLine[0].z;

				routLine.erase(routLine.begin());

				std::vector<casic_vector3d> upLinePoints = routLine;
				int firePointIndex = routLine.size() - 1;

				for (int index = routLine.size() - 2; index >= 0; index--)
				{
					upLinePoints.push_back(routLine[index]);

				}

				int hei = routLine[1].z - routLine[0].z;

				upLinePoints.insert(upLinePoints.begin(), routLine[0]);
				upLinePoints.insert(upLinePoints.begin(), routLine[0]);
				firePointIndex += 2;

				upLinePoints.push_back(upLinePoints[0]);

				for (int indexP = 0; indexP < upLinePoints.size(); indexP++)
				{
					auto point = upLinePoints[indexP];
					CNeubulaMissionLinePoint mPoint;
					mPoint.nNo = index_i;
					mPoint.nLineID = mline.nID;
					mPoint.dLon = point.x;
					mPoint.dLat = point.y;
					mPoint.dAlt = upLinePoints[indexP].z ;
					if (indexP == firePointIndex)
					{

						mPoint.dHoverTime = 30;
						mPoint.cType = 5;
					}
					mline.LinePointVec.push_back(mPoint);
					index_i++;
				}

				ICollaborativeSearchActivator::Get().getPTaskManageService()->EditMissionLine(mline);
			}
			++index;

			struct FireProt
			{
				bool UpdateRoute = true;
				bool UpDownRroute = true;
			}prot;

			void *lParam = (void *)(&prot);
			if (lParam == nullptr) {
				 return false;
			}

			QString strTask = QString::fromStdString(fireMission.GetID());
			LY_CollaborativeSearchServiceInterface::Get().SendMsgToAll(IInterface::MSG_Command, 50011, (void*)(&strTask));
			LY_CollaborativeSearchServiceInterface::Get().SendMsgToAll(IInterface::MSG_MAP_IDENTIFY_MENU + 100, 50009, lParam);

			if (LY_CollaborativeSearchService::Get().m_SystemEventRecording) {
				QDateTime curdt = QDateTime::currentDateTime();
				QString strcurdt = curdt.toString("yyyy-MM-dd hh:mm:ss");
				LY_CollaborativeSearchService::Get().m_SystemEventRecording->sltRecvEvent(EventInfo(strcurdt, tr2("操作事件"), tr2("处置任务规划-UAV") + QString::fromStdString(routedLineItor->first) + tr2(" 航线上传")));
			}

			find_routPath_up = true;
		}
		routedLineItor++;
	}

	if (find_routPath_up) {
		LY_CollaborativeSearchServiceInterface::Get().SendMsgToAll(IInterface::MSG_Command, 50009, nullptr);
	}
}

std::map<std::string, std::vector<casic_vector3d>> Dispose_targetAndUAVS_process::getRoutedLines()
{
	return mDispose_UAVS_Route.getRoutedLines();
}

void Dispose_targetAndUAVS_process::setTargetUAVInfos(vector<DISPOSE_ROUTE::target_UAVS_Info>& _targetUAVInfos)
{
	mDispose_UAVS_Route.setTargetUAVInfos(_targetUAVInfos);
}

void Dispose_targetAndUAVS_process::setUAVStartPos_handle(std::string uavID, casic_vector3d pos)
{
	uavIDSPos[uavID] = pos;
	for (auto & target_UAV : target_Match_UAVS)
	{
		bool isSuccess = false;
		for (auto & uavInfo : target_UAV.uavsInfos)
		{
			if (uavInfo.uavID == uavID)
			{
				uavInfo.longitude = pos.x;
				uavInfo.latitude = pos.y;
				uavInfo.altitude = pos.z;
				isSuccess = true;
				break;
			}
		}
		if (isSuccess)
			break;
	}
}

void Dispose_targetAndUAVS_process::changeUAV_throwHeiOrFireDis(std::string uavID, int disOrHei)
{
	auto target_UAVItor = target_Match_UAVS.begin();
	while (target_UAVItor != target_Match_UAVS.end())
	{
		auto uavItor = target_UAVItor->uavsInfos.begin();
		while (uavItor != target_UAVItor->uavsInfos.end())
		{
			if (uavItor->uavID == uavID)
			{
				uavItor->dispose_heiOrDis = disOrHei;
				break;
			}
			else
			{
				uavItor++;
			}
		}
		target_UAVItor++;
	}
}

Dispose_UAVS_Route* Dispose_targetAndUAVS_process::getDispos_Uav_Route()
{
	return &mDispose_UAVS_Route;
}

void Dispose_targetAndUAVS_process::removeMeshs(IEarth * pEarth, std::vector<IUserMesh1*>& meshs)
{
	if (pEarth == NULL)
		return;
	if (!meshs.empty())
	{
		for (int index = 0; index < meshs.size(); index++)
		{
			pEarth->GetGraphics()->RemoveObject(meshs[index]);
		}
		meshs.clear();
	}
}
void Dispose_targetAndUAVS_process::removeMeshs(IEarth * pEarth, std::vector<IUserMesh2*>& meshs)
{
	if (pEarth == NULL)
		return;
	if (!meshs.empty())
	{
		for (int index = 0; index < meshs.size(); index++)
		{
			pEarth->GetGraphics()->RemoveObject(meshs[index]);
		}
		meshs.clear();
	}
}
void Dispose_targetAndUAVS_process::removeMeshs(IEarth * pEarth, std::vector<ILabel*>& meshs)
{
	if (pEarth == NULL)
		return;
	if (!meshs.empty())
	{
		for (int index = 0; index < meshs.size(); index++)
		{
			pEarth->GetGraphics()->RemoveObject(meshs[index]);
		}
		meshs.clear();
	}
}

void Dispose_targetAndUAVS_process::create3DLine(IEarth * pEarth, std::vector<casic_vector3d>& PointsVec, std::vector<IUserMesh1*> & Mesh1s, IUserMesh2Vec & Mesh2s,
								ILabelVec & labels, std::vector<int> lineIndexs, std::vector<QColor> lineColors, int lineWidth)
{
	if (pEarth == NULL)
		return;
	removeMeshs(pEarth, Mesh1s);
	removeMeshs(pEarth, Mesh2s);
	removeMeshs(pEarth, labels);

	if (!lineIndexs.empty() && lineColors.size() == (lineIndexs.size() - 1))
	{
		auto pointVecItor = PointsVec.begin();
		pointVecItor += lineIndexs[0];
		for (int index = 0; index < lineIndexs.size() - 1; index++)
		{
			std::vector<casic_vector3d> tempPoints(pointVecItor, pointVecItor + lineIndexs[index + 1]);
			create3DLine(pEarth, tempPoints, Mesh1s, lineColors[index], lineWidth);
		}
	}

	create3DLine_height(pEarth, PointsVec, Mesh1s, Mesh2s, 2);
}

void Dispose_targetAndUAVS_process::create3DLine(IEarth * pEarth, std::vector<casic_vector3d>& PointsVec, IUserMesh1Vec & Mesh1s, IUserMesh2Vec & Mesh2s,
						ILabelVec & labels, QColor lineColor, int lineWidth)
{
	if (pEarth == NULL)
		return;
	removeMeshs(pEarth, Mesh1s);
	removeMeshs(pEarth, Mesh2s);
	removeMeshs(pEarth, labels);
	create3DLine(pEarth, PointsVec, Mesh1s, lineColor, lineWidth);

	create3DLine_height(pEarth, PointsVec, Mesh1s, Mesh2s, 2);
}

void Dispose_targetAndUAVS_process::create3DLine(IEarth * pEarth, std::vector<casic_vector3d>& PointVec, IUserMesh1Vec & Mesh1s, QColor lineColor, int lineWidth)
{
	CCreateShape cs;
	std::vector<casic_vector3d> points = PointVec;
	std::vector<double> pdLon;
	pdLon.clear();
	std::vector<double> pdLat;
	pdLat.clear();
	std::vector<double> pdHeight;
	pdHeight.clear();

	for (int pointIndex = 0; pointIndex < points.size(); pointIndex++)
	{
		pdLon.push_back(points[pointIndex].x);
		pdLat.push_back(points[pointIndex].y);
		pdHeight.push_back(points[pointIndex].z);
	}
	IUserMesh1 *pUM = nullptr;
	SHPObject *pShp = cs.CreatePolyline(pdLon.data(), pdLat.data(), pdLon.size(),
		100, false, nullptr, pdHeight.data());
	if (pShp != nullptr)
	{

		pUM = pEarth->GetGraphics()->CreateUserMesh1(pShp);
		if (pUM != nullptr)
		{
			pUM->SetColor(lineColor.rgb());
			pUM->SetState(IUserMesh::POLYGONMODE, IUserMesh::USERMESH_LINE);
			pUM->SetState(IUserMesh::LINEWIDTH, lineWidth);
			pUM->SetState(IUserMesh::LIGHT, 1);
			pUM->SetVisible(true);
			Mesh1s.push_back(pUM);
		}

		pUM = pEarth->GetGraphics()->CreateUserMesh1(pShp);
		if (pUM != nullptr)
		{
			pUM->SetColor(lineColor.rgb());
			pUM->SetState(IUserMesh::POLYGONMODE, IUserMesh::USERMESH_POINT);
			pUM->SetState(IUserMesh::POINT, lineWidth * 2);
			pUM->SetVisible(true);
			Mesh1s.push_back(pUM);
		}
	}
}

void Dispose_targetAndUAVS_process::create3DLine_height(IEarth * pEarth, std::vector<casic_vector3d>& PointVec, IUserMesh1Vec & Mesh1s, IUserMesh2Vec& Mesh2s, int lineWidth, QColor lineColor)
{
	for (int pointIndex = 0; pointIndex < PointVec.size(); pointIndex++)
	{
		short int elevationTemp = LY_CollaborativeSearchService::Get().s_CGetHeight.
			GetHeight(PointVec[pointIndex].x, PointVec[pointIndex].y);

		double pdLon1[2] = { PointVec[pointIndex].x   , PointVec[pointIndex].x };
		double pdLat1[2] = { PointVec[pointIndex].y, PointVec[pointIndex].y };
		double pdHeight1[2] = { PointVec[pointIndex].z * 1.0, elevationTemp * 1.0 };

		IUserMesh1	*pUM1 = nullptr;
		CCreateShape cs1;
		SHPObject *pShp1 = cs1.CreatePolyline(pdLon1, pdLat1, 2, 100, false, nullptr, pdHeight1);
		if (pShp1 != nullptr)
		{
			pUM1 = pEarth->GetGraphics()->CreateUserMesh1(pShp1);
			if (pUM1 != nullptr)
			{
				pUM1->SetColor(lineColor.rgba());

				pUM1->SetState(IUserMesh::LINEWIDTH, lineWidth);
				pUM1->SetState(IUserMesh::LINESTIPPLE, 4);
				pUM1->SetVisible(true);
				Mesh1s.push_back(pUM1);
			}

			IUserMesh2	*pUM2 = nullptr;
			SHPObject *pShp2 = cs1.CreateCircle(0, 0, 2, 1e6);
			if (pShp2 != nullptr)
			{
				pUM2 = pEarth->GetGraphics()->CreateUserMesh2(pShp2);
				if (pUM2 != nullptr)
				{
					pUM2->SetColor(lineColor.rgba());
					pUM2->SetPosition(PointVec[pointIndex].x, PointVec[pointIndex].y, elevationTemp);
					pUM2->SetVisible(true);
					Mesh2s.push_back(pUM2);
				}
			}
		}
	}
}

std::vector<std::string> Dispose_targetAndUAVS_process::getUAVIDS_throw()
{
	return uavIDS_Throw;
}

std::vector<std::string> Dispose_targetAndUAVS_process::getUAVIDS_Fire()
{
	return uavIDS_Fire;
}

void Dispose_targetAndUAVS_process::loadMarksFormDb()
{
	Marks_All.clear();
	Marks_AllID.clear();
	auto markSerItor = ICollaborativeSearchActivator::Get().getPIMarkManageService();
	if (markSerItor != NULL)
	{
		std::vector<QString> targetIDs = markSerItor->GetMarkIDListFromDB_ActivatingAction();

		for (auto targetID : targetIDs)
		{
			auto targetTemp = markSerItor->GetMark(targetID.toStdString());
			if (targetTemp.GetID() != "" && findMarkType_used(targetTemp.GetType()) != MarkTypes_used.end())
			{
				Marks_All.push_back(targetTemp);
				Marks_AllID.push_back(targetTemp.GetID());
			}
		}
	}
}

std::vector<int> Dispose_targetAndUAVS_process::sortUAVbyAngle(const DISPOSE_ROUTE::targetInfo & pTargetInfo, const std::vector<DISPOSE_ROUTE::UAVInfo>& pUAVInfos, const std::vector<int>& uavIndexs)
{
	std::vector<int> reUAVIndexs;
	reUAVIndexs.clear();

	if (uavIndexs.empty())
	{
		return reUAVIndexs;
	}

	QGeoCoordinate targetGeo(pTargetInfo.latitude, pTargetInfo.longitude);
	std::map<double, int> angleSortUAV;
	angleSortUAV.clear();
	std::map<double, std::map<double, int>> angle_DisSortUAV;
	angle_DisSortUAV.clear();
	for (auto uavIndex : uavIndexs)
	{
		QGeoCoordinate uavGeo(pUAVInfos[uavIndex].latitude, pUAVInfos[uavIndex].longitude);
		double angle = uavGeo.azimuthTo(targetGeo);
		auto angleSortUAVItor = angleSortUAV.find(angle);
		if (angleSortUAVItor != angleSortUAV.end())
		{
			double dis = uavGeo.distanceTo(targetGeo);
			auto angle_disSortUAVItor = angle_DisSortUAV.find(angle);
			if (angle_disSortUAVItor != angle_DisSortUAV.end())
			{
				angle_disSortUAVItor->second[dis] = uavIndex;
			}
			else
			{
				int oldIndex = angleSortUAVItor->second;
				QGeoCoordinate oldUavGeo(pUAVInfos[oldIndex].latitude, pUAVInfos[oldIndex].longitude);
				double oldDis = oldUavGeo.distanceTo(targetGeo);
				std::map<double, int> disSortUAV;
				disSortUAV[oldDis] = oldIndex;
				disSortUAV[dis] = uavIndex;
				angle_DisSortUAV[angle] = disSortUAV;
			}
		}
		else
		{
			angleSortUAV[angle] = uavIndex;
		}
	}

	auto angle_SortMinItor = angleSortUAV.begin();
	auto angle_SortMaxItor = angleSortUAV.rbegin();
	int angle_min = angle_SortMinItor->first;
	int angle_max = angle_SortMaxItor->first;
	if (std::abs(angle_max - angle_min) > 180)
	{
		while (angle_SortMinItor != angleSortUAV.end())
		{
			if (std::abs(angle_SortMinItor->first - angle_min) > 180)
			{
				double nowAngle = angle_SortMinItor->first;
				angleSortUAV[nowAngle - 360] = angle_SortMinItor->second;
				angle_SortMinItor = angleSortUAV.erase(angle_SortMinItor);
				if (angle_DisSortUAV.find(nowAngle) != angle_DisSortUAV.end())
				{
					angle_DisSortUAV[nowAngle - 360] = angle_DisSortUAV.find(nowAngle)->second;
				}
			}
			else
			{
				angle_SortMinItor++;
			}

		}

	}

	auto angleItor = angleSortUAV.begin();
	while (angleItor != angleSortUAV.end())
	{
		if (angle_DisSortUAV.find(angleItor->first) != angle_DisSortUAV.end())
		{
			auto disItor = angle_DisSortUAV.find(angleItor->first)->second.begin();
			while (disItor != angle_DisSortUAV.find(angleItor->first)->second.end())
			{
				reUAVIndexs.push_back(disItor->second);
				disItor++;
			}
		}
		else
		{
			reUAVIndexs.push_back(angleItor->second);
		}
		angleItor++;
	}

	return reUAVIndexs;
}

DISPOSE_ROUTE::DISPOSE_WAY Dispose_targetAndUAVS_process::getUAVType(std::string uavID)
{
	if (std::find(uavIDS_Fire.begin(), uavIDS_Fire.end(), uavID) != uavIDS_Fire.end())
	{
		return DISPOSE_ROUTE::DISPOSE_FIRE;
	}
	else if (std::find(uavIDS_Throw.begin(), uavIDS_Throw.end(), uavID) != uavIDS_Throw.end())
	{
		return DISPOSE_ROUTE::DISPOSE_THROW;
	}
}

void Dispose_targetAndUAVS_process::updateUav2Mission()
{

	auto activatingMissionVec = ICollaborativeSearchActivator::Get().getPTaskManageService()->GetActivatingMissions();
	if (!activatingMissionVec.empty())
	{
		for (auto ms : activatingMissionVec)
		{
			auto taskManageSer = ICollaborativeSearchActivator::Get().getPTaskManageService();
			if (taskManageSer != NULL)
			{
				QString string = QString::fromStdString(ms.GetID());
				std::vector<std::string> msUAVIDs = taskManageSer->readPlatFormMatch(string.toLocal8Bit().toStdString());
				for (auto uavID : msUAVIDs)
				{
					UAVID2Mission[uavID] = ms;
				}
			}

		}

	}
}
