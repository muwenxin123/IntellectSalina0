#include "ITaskManageImpl.h"
#include "Common/CommonString.h"
#include <QDir>
#include "DataManage/NebulaTask/NebulaRouteGroup.h"
#include "DataManage/NebulaTask/NebulaTask.h"
#include "TinyXML/tinyxml.h"
#include "QCString.h"
#include "QVariant"
#include "ServiceFramework/MessageReference.h"
#include "QSettings"
#include "lY_TaskManageServiceInterface.h"
#include "QSqlQuery"
#include <QDebug>
#include <QSqlError>

using namespace qnzkna::TaskManage;

ITaskManageImpl::ITaskManageImpl(void)
{
    m_nCurrectTask = NEBULATASK_ID_INVAILD;

	m_strDatabasePath = "../data/TaskRecord/";
    m_nFormScaleUnit = FORM_UNIT_SCALE_LEN;
    loadLoginName();
    InitDataBase();
    LoadDataBase();
    LoadRelationMissionMarkDB();
    LoadRelationMissionPtDB();
    LoadRelationMarkDB();
    LoadActionformDB();
    LoadPlatformDB();
    LoadPlatformLoadDB();
    LoadRelationTask();
    LoadCodeSet_Load_DB();
    LoadLineDB();

    LoadResourceSensorDB();
    LoadResourcePtDB();
    LoadResourceAmmoDB();

    LoadRelationPtDB();
    LoadRelationSensorDB();
    LoadRelationAmmoDB();

    LoadActionMarkRankListDB();

    LoadSchemeDB();
    LoadSchemePointDB();
    LoadSchemeAmmoDB();
    LoadSchemeSensorDB();

    LoadSchemePointFADB();

    LoadActionSchemeGLDB();

    LoadMissionAmmoDB();

}

ITaskManageImpl::~ITaskManageImpl(void)
{
    db.close();
}

bool ITaskManageImpl::ParseNebulaRouteGroupNode(TiXmlElement *pGroupNodeEle, CNebulaTask &nebulaTask, CNebulaRouteGroup &NebulaGroupInfo)
{
    if (!pGroupNodeEle)
    {
        return false;
    }

    const char *pChar = pGroupNodeEle->Attribute("ID");
    if (!pChar)
    {
        return false;
    }
    NebulaGroupInfo.SetGroupID(StringToInt(pChar));
    pChar = pGroupNodeEle->Attribute("Name");
    if (!pChar)
    {
        return false;
    }
    NebulaGroupInfo.SetGroupName(pChar);
    pChar = pGroupNodeEle->Attribute("FormMode");
    if (!pChar)
    {
        return false;
    }
    NebulaGroupInfo.SetFormMode((CNebulaRouteGroup::EFormMode)StringToInt(pChar));
    pChar = pGroupNodeEle->Attribute("FormationID");
    if (!pChar)
    {
        return false;
    }
    NebulaGroupInfo.SetFormationID(StringToInt(pChar));

    NebulaGroupInfo.SetTaskType(ENebulaTaskType::ENebulaTaskType_Unknown);
    const char *strTaskType = pGroupNodeEle->Attribute("TaskType");
    if (strTaskType != NULL)
    {
        NebulaGroupInfo.SetTaskType(NebulaTaskType(StringToInt(strTaskType)));
    }

    TiXmlElement *pXMLNebulaTaskPlatformList = pGroupNodeEle->FirstChildElement("NebulaTaskPlatformList");
    if (pXMLNebulaTaskPlatformList != NULL)
    {
        ParseNebulaTaskPlatformListNode(pXMLNebulaTaskPlatformList, nebulaTask);
    }

    TiXmlElement *pChildEle = pGroupNodeEle->FirstChildElement("NebulaRouteLine");
    while (pChildEle)
    {
        CNebulaRouteLine LineInfo;
        if (ParseNebulaRouteLineNode(pChildEle, LineInfo))
        {
            NebulaGroupInfo.AddRouteLine(LineInfo);
        }
        pChildEle = pChildEle->NextSiblingElement("NebulaRouteLine");
    }
    return true;
}

bool ITaskManageImpl::ParseNebulaTaskPlatformListNode(TiXmlElement *pXMLNebulaTaskPlatformList, CNebulaTask &nebulaTask)
{
    if (pXMLNebulaTaskPlatformList == NULL)
    {
        return false;
    }

    NebulaTaskPlatformMap &nebulaTaskPlatformMap = nebulaTask.GetPlatformInfoList();
    TiXmlElement          *pNebulaTaskPlatform = pXMLNebulaTaskPlatformList->FirstChildElement("NebulaTaskPlatform");
    while (pNebulaTaskPlatform)
    {
        NebulaTaskPlatform nebulaTaskPlatform;
        if (ParseNebulaTaskPlatformNode(pNebulaTaskPlatform, nebulaTaskPlatform))
        {
            nebulaTaskPlatformMap.push_back(std::pair<std::string, NebulaTaskPlatform>(nebulaTaskPlatform.GetPlatformID(), nebulaTaskPlatform));
        }
        pNebulaTaskPlatform = pNebulaTaskPlatform->NextSiblingElement("NebulaTaskPlatform");
    }
    return true;
}

bool ITaskManageImpl::ParseNebulaTaskPlatformNode(TiXmlElement *pXMLNebulaTaskPlatform, NebulaTaskPlatform &nebulaTaskPlatform)
{
    if (pXMLNebulaTaskPlatform == NULL)
    {
        return false;
    }

    const char *strWeaponID = pXMLNebulaTaskPlatform->Attribute("WeaponID");
    if (strWeaponID == NULL || strWeaponID[0] == '\0')
    {
        return false;
    }

    nebulaTaskPlatform.SetPlatformID(strWeaponID);

    TiXmlElement *pXMLNebulaTaskLoadList = pXMLNebulaTaskPlatform->FirstChildElement("NebulaTaskLoadList");
    if (pXMLNebulaTaskLoadList != NULL)
    {
        NebulaTaskLoadList &nebulaTaskLoadList = nebulaTaskPlatform.GetTaskLoadList();
        ParseNebulaTaskPlatformTaskLoadListNode(pXMLNebulaTaskLoadList, nebulaTaskLoadList);
    }

    TiXmlElement *pXMLNebulaTaskMarkList = pXMLNebulaTaskPlatform->FirstChildElement("NebulaTaskMarkList");
    if (pXMLNebulaTaskMarkList != NULL)
    {
        NebulaMarkTrackList &taskMarkList = nebulaTaskPlatform.GetMarkList();
        ParseNebulaTaskPlatformMarkListNode(pXMLNebulaTaskMarkList, taskMarkList);
    }

    TiXmlElement *pXMLNebulaTaskRegionList = pXMLNebulaTaskPlatform->FirstChildElement("NebulaTaskRegionList");
    if (pXMLNebulaTaskRegionList != NULL)
    {
        std::list<std::string> &taskRegionList = nebulaTaskPlatform.GetRegionList();
        ParseNebulaTaskPlatformRegionListNode(pXMLNebulaTaskRegionList, taskRegionList);
    }
    return true;
}

bool ITaskManageImpl::ParseNebulaTaskPlatformTaskLoadListNode(TiXmlElement *pXMLNebulaTaskPlatformTaskLoadList, NebulaTaskLoadList &nebulaTaskLoadList)
{
    if (pXMLNebulaTaskPlatformTaskLoadList == NULL)
    {
        return false;
    }

    TiXmlElement *pXMLNebulaTaskLoad = pXMLNebulaTaskPlatformTaskLoadList->FirstChildElement("NebulaTaskLoad");
    while (pXMLNebulaTaskLoad)
    {
        NebulaTaskLoad nebulaTaskLoad;

        const char *strLoadId = pXMLNebulaTaskLoad->Attribute("LoadId");
        if (strLoadId == NULL || strLoadId[0] == '\0')
        {
            pXMLNebulaTaskLoad = pXMLNebulaTaskLoad->NextSiblingElement("NebulaTaskLoad");
            continue;
        }
        nebulaTaskLoad.SetTaskLoadID(strLoadId);

        const char *strLoadType = pXMLNebulaTaskLoad->Attribute("LoadType");
        if (strLoadType == NULL || strLoadType[0] == '\0')
        {
            pXMLNebulaTaskLoad = pXMLNebulaTaskLoad->NextSiblingElement("NebulaTaskLoad");
            continue;
        }
        nebulaTaskLoad.SetTaskLoadType(ENebulaTaskLoadType(StringToInt(strLoadType)));

        const char *strLoadName = pXMLNebulaTaskLoad->Attribute("LoadName");
        if (strLoadName == NULL)
        {
            pXMLNebulaTaskLoad = pXMLNebulaTaskLoad->NextSiblingElement("NebulaTaskLoad");
            continue;
        }
        nebulaTaskLoad.SetTaskLoadName(strLoadName);

        TiXmlElement *pXMLLoadParameterList = pXMLNebulaTaskLoad->FirstChildElement("LoadParameterList");
        if (pXMLLoadParameterList != NULL)
        {
            TaskNoSortParamMap &taskNoSortParamMap = nebulaTaskLoad.GetTaskParamMap();
            TiXmlElement *pXMLLoadParameter = pXMLLoadParameterList->FirstChildElement("LoadParameter");
            while (pXMLLoadParameter)
            {
                std::string strKey = pXMLLoadParameter->Attribute("Key");
                std::string strValue = pXMLLoadParameter->Attribute("Value");
                if (strKey.empty() || strValue.empty())
                {
                    pXMLLoadParameter = pXMLLoadParameter->NextSiblingElement("LoadParameter");
                    continue;
                }
                taskNoSortParamMap.push_back(std::pair<std::string, std::string>(strKey, strValue));
                pXMLLoadParameter = pXMLLoadParameter->NextSiblingElement("LoadParameter");
            }
        }

        TiXmlElement *pXMLNebulaTaskSectionList = pXMLNebulaTaskLoad->FirstChildElement("TaskSectionList");
        if (pXMLNebulaTaskSectionList != NULL)
        {
            NebulaTaskSectionList &nebulaTaskSectionList = nebulaTaskLoad.GetTaskSectionList();
            TiXmlElement *pXMLTaskSection = pXMLNebulaTaskSectionList->FirstChildElement("TaskSection");
            while (pXMLTaskSection)
            {
                NebulaTaskSection nebulaTaskSection;

                const char *strTaskType = pXMLTaskSection->Attribute("TaskType");
                if (strTaskType == NULL)
                {
                    pXMLTaskSection = pXMLTaskSection->NextSiblingElement("TaskSection");
                    continue;
                }
                nebulaTaskSection.SetTaskType(ENebulaTaskType(StringToInt(strTaskType)));

                const char *strWorkMode = pXMLTaskSection->Attribute("WorkMode");
                if (strWorkMode == NULL)
                {
                    pXMLTaskSection = pXMLTaskSection->NextSiblingElement("TaskSection");
                    continue;
                }
                nebulaTaskSection.SetWorkMode(NebulaWorkMode(StringToInt(strWorkMode)));

                const char *strFlightSegmentBegin = pXMLTaskSection->Attribute("FlightSegmentBegin");
                const char *strFlightSegmentEnd = pXMLTaskSection->Attribute("FlightSegmentEnd");
                if (strFlightSegmentBegin != NULL && strFlightSegmentBegin != NULL)
                {
                    nebulaTaskSection.SetFlightSegment(StringToInt(strFlightSegmentBegin), StringToInt(strFlightSegmentEnd));
                }

                const char *strTimeSegmentStart = pXMLTaskSection->Attribute("TimeSegmentStart");
                const char *TimeSegmentEnd = pXMLTaskSection->Attribute("TimeSegmentEnd");
                if (strTimeSegmentStart != NULL && TimeSegmentEnd != NULL)
                {
                    char  *stopstring;
                    unsigned long long startTime = strtoull(strTimeSegmentStart, &stopstring, 10);
                    unsigned long long endTime = strtoull(TimeSegmentEnd, &stopstring, 10);
                    nebulaTaskSection.SetTimeSegment((startTime), (endTime));
                }

                TiXmlElement *pXMLSectionParameterList = pXMLTaskSection->FirstChildElement("SectionParameterList");
                if (pXMLSectionParameterList != NULL)
                {
                    TaskNoSortParamMap &taskSectionNoSortParamMap = nebulaTaskSection.GetParamList();
                    TiXmlElement *pXMLSectionParameter = pXMLSectionParameterList->FirstChildElement("SectionParameter");
                    while (pXMLSectionParameter)
                    {
                        std::string strKey = pXMLSectionParameter->Attribute("Key");
                        std::string strValue = pXMLSectionParameter->Attribute("Value");
                        if (strKey.empty() || strValue.empty())
                        {
                            pXMLSectionParameter = pXMLSectionParameter->NextSiblingElement("SectionParameter");
                            continue;
                        }
                        taskSectionNoSortParamMap.push_back(std::pair<std::string, std::string>(strKey, strValue));
                        pXMLSectionParameter = pXMLSectionParameter->NextSiblingElement("SectionParameter");
                    }
                }
                nebulaTaskSectionList.push_back(nebulaTaskSection);
                pXMLTaskSection = pXMLTaskSection->NextSiblingElement("TaskSection");
            }
        }
        nebulaTaskLoadList.push_back(nebulaTaskLoad);
        pXMLNebulaTaskLoad = pXMLNebulaTaskLoad->NextSiblingElement("NebulaTaskLoad");
    }
    return true;
}

bool ITaskManageImpl::ParseNebulaTaskPlatformMarkListNode(TiXmlElement *pXMLNebulaTaskMarkList, NebulaMarkTrackList &taskMarkList)
{
    if (pXMLNebulaTaskMarkList == NULL)
    {
        return false;
    }

    TiXmlElement *pXMLTaskMark = pXMLNebulaTaskMarkList->FirstChildElement("TaskMark");
    while (pXMLTaskMark)
    {
        std::string strStationId = pXMLTaskMark->Attribute("StationId");
        std::string strBatchId = pXMLTaskMark->Attribute("BatchId");
        if (strStationId.empty() || strBatchId.empty())
        {
            pXMLTaskMark = pXMLTaskMark->NextSiblingElement("TaskMark");
            continue;
        }
        taskMarkList.push_back(std::pair<std::string, std::string>(strStationId, strBatchId));
        pXMLTaskMark = pXMLTaskMark->NextSiblingElement("TaskMark");
    }
    return true;
}

bool ITaskManageImpl::ParseNebulaTaskPlatformRegionListNode(TiXmlElement *pXMLNebulaTaskRegionList, std::list<std::string> &taskRegionList)
{
    if (pXMLNebulaTaskRegionList == NULL)
    {
        return false;
    }

    TiXmlElement *pXMLTaskRegion = pXMLNebulaTaskRegionList->FirstChildElement("TaskRegion");
    while (pXMLTaskRegion)
    {
        std::string strRegionId = pXMLTaskRegion->Attribute("RegionId");
        if (strRegionId.empty())
        {
            pXMLTaskRegion = pXMLTaskRegion->NextSiblingElement("TaskRegion");
            continue;
        }
        taskRegionList.push_back(strRegionId);
        pXMLTaskRegion = pXMLTaskRegion->NextSiblingElement("TaskRegion");
    }
    return true;
}

bool ITaskManageImpl::ParseNebulaMarkInfoListNode(TiXmlElement *pXMLNebulaMarkInfoList, CNebulaTask &nebulaTask)
{
    if (pXMLNebulaMarkInfoList == NULL)
    {
        return false;
    }

    NebulaMarkInfoList &nebulaMarkInfoList = nebulaTask.GetMarkInfoList();
    TiXmlElement *pXMLNebulaMarkInfo = pXMLNebulaMarkInfoList->FirstChildElement("MarkInfo");
    while (pXMLNebulaMarkInfo)
    {
        NebulaMarkInfo nebulaMarkInfo;

        const char *strStationId = pXMLNebulaMarkInfo->Attribute("StationId");
        const char *strBatchId = pXMLNebulaMarkInfo->Attribute("BatchId");
        if (strStationId == NULL || strBatchId == NULL || strStationId[0] == '\0' || strBatchId[0] == '\0')
        {
            pXMLNebulaMarkInfo = pXMLNebulaMarkInfo->NextSiblingElement("MarkInfo");
            continue;
        }
        nebulaMarkInfo.SetMarkID(strStationId, strBatchId);

        const char *strProperty = pXMLNebulaMarkInfo->Attribute("Property");
        if (strProperty == NULL || strProperty[0] == '\0')
        {
            pXMLNebulaMarkInfo = pXMLNebulaMarkInfo->NextSiblingElement("MarkInfo");
            continue;
        }
        nebulaMarkInfo.SetMarkProperty(EMarkProperty(StringToInt(strProperty)));

        const char *strAttribute = pXMLNebulaMarkInfo->Attribute("Attribute");
        if (strAttribute == NULL || strAttribute[0] == '\0')
        {
            pXMLNebulaMarkInfo = pXMLNebulaMarkInfo->NextSiblingElement("MarkInfo");
            continue;
        }
        nebulaMarkInfo.SetMarkAttribute(EMarkAttribute(StringToInt(strAttribute)));

        const char *strMarkType = pXMLNebulaMarkInfo->Attribute("MarkType");
        if (strMarkType == NULL || strMarkType[0] == '\0')
        {
            pXMLNebulaMarkInfo = pXMLNebulaMarkInfo->NextSiblingElement("MarkInfo");
            continue;
        }
        nebulaMarkInfo.SetMarkType(EMarkType(StringToInt(strMarkType)));

        const char *strMovableType = pXMLNebulaMarkInfo->Attribute("MovableType");
        if (strMovableType == NULL || strMovableType[0] == '\0')
        {
            pXMLNebulaMarkInfo = pXMLNebulaMarkInfo->NextSiblingElement("MarkInfo");
            continue;
        }
        nebulaMarkInfo.SetMovableType(EMovableType(StringToInt(strMovableType)));

        const char *strDestoryMode = pXMLNebulaMarkInfo->Attribute("DestoryMode");
        if (strDestoryMode == NULL || strDestoryMode[0] == '\0')
        {
            pXMLNebulaMarkInfo = pXMLNebulaMarkInfo->NextSiblingElement("MarkInfo");
            continue;
        }
        nebulaMarkInfo.SetDestoryMode(ENebulaTaskDestroyMode(StringToInt(strDestoryMode)));

        const char *strThreaten = pXMLNebulaMarkInfo->Attribute("Threaten");
        if (strThreaten == NULL || strThreaten[0] == '\0')
        {
            pXMLNebulaMarkInfo = pXMLNebulaMarkInfo->NextSiblingElement("MarkInfo");
            continue;
        }
        nebulaMarkInfo.SetThreaten((StringToInt(strThreaten)));

        const char *strLongitude = pXMLNebulaMarkInfo->Attribute("Longitude");
        const char *strLatitude = pXMLNebulaMarkInfo->Attribute("Latitude");
        const char *strHeight = pXMLNebulaMarkInfo->Attribute("Height");
        if (strLongitude == NULL || strLatitude == NULL || strHeight == NULL || strLongitude[0] == '\0' || strLatitude[0] == '\0' || strHeight[0] == '\0')
        {
            pXMLNebulaMarkInfo = pXMLNebulaMarkInfo->NextSiblingElement("MarkInfo");
            continue;
        }
        nebulaMarkInfo.SetPosition(StringToDouble(strLongitude), StringToDouble(strLatitude), StringToInt(strHeight));

        const char *strDescription = pXMLNebulaMarkInfo->Attribute("Description");
        if (strDescription != NULL)
        {
            nebulaMarkInfo.SetDescription(strDescription);
        }

        nebulaMarkInfoList.push_back(nebulaMarkInfo);
        pXMLNebulaMarkInfo = pXMLNebulaMarkInfo->NextSiblingElement("MarkInfo");
    }
    return true;
}

bool ITaskManageImpl::ParseNebulaRegionInfoListNode(TiXmlElement *pXMLNebulaRegionInfoList, CNebulaTask &nebulaTask)
{
    if (pXMLNebulaRegionInfoList == NULL)
    {
        return false;
    }

    NebulaRegionInfoList &nebulaRegionInfoList = nebulaTask.GetRegionInfoList();
    TiXmlElement *pXMLNebulaRegionInfo = pXMLNebulaRegionInfoList->FirstChildElement("RegionInfo");
    while (pXMLNebulaRegionInfo)
    {

        const char *strRegionId = pXMLNebulaRegionInfo->Attribute("RegionId");
        if (strRegionId == NULL || strRegionId[0] == '\0')
        {
            pXMLNebulaRegionInfo = pXMLNebulaRegionInfo->NextSiblingElement("RegionInfo");
            continue;
        }

        const char *strProperty = pXMLNebulaRegionInfo->Attribute("Property");
        if (strProperty == NULL || strProperty[0] == '\0')
        {
            pXMLNebulaRegionInfo = pXMLNebulaRegionInfo->NextSiblingElement("RegionInfo");
            continue;
        }

        const char *strAttribute = pXMLNebulaRegionInfo->Attribute("Attribute");
        if (strAttribute == NULL || strAttribute[0] == '\0')
        {
            pXMLNebulaRegionInfo = pXMLNebulaRegionInfo->NextSiblingElement("RegionInfo");
            continue;
        }

        const char *strMarkType = pXMLNebulaRegionInfo->Attribute("RegionType");
        if (strMarkType == NULL || strMarkType[0] == '\0')
        {
            pXMLNebulaRegionInfo = pXMLNebulaRegionInfo->NextSiblingElement("RegionInfo");
            continue;
        }

        const char *strDestoryMode = pXMLNebulaRegionInfo->Attribute("DestoryMode");
        if (strDestoryMode == NULL || strDestoryMode[0] == '\0')
        {
            pXMLNebulaRegionInfo = pXMLNebulaRegionInfo->NextSiblingElement("RegionInfo");
            continue;
        }

        const char *strThreaten = pXMLNebulaRegionInfo->Attribute("Threaten");
        if (strThreaten == NULL || strThreaten[0] == '\0')
        {
            pXMLNebulaRegionInfo = pXMLNebulaRegionInfo->NextSiblingElement("RegionInfo");
            continue;
        }

        const char *strDownHeight = pXMLNebulaRegionInfo->Attribute("DownHeight");
        if (strDownHeight == NULL || strDownHeight[0] == '\0')
        {
            pXMLNebulaRegionInfo = pXMLNebulaRegionInfo->NextSiblingElement("RegionInfo");
            continue;
        }

        const char *strUpHeight = pXMLNebulaRegionInfo->Attribute("UpHeight");
        if (strUpHeight == NULL || strUpHeight[0] == '\0')
        {
            pXMLNebulaRegionInfo = pXMLNebulaRegionInfo->NextSiblingElement("RegionInfo");
            continue;
        }

        CRegionBase *regionBase = NULL;
        int regionType = StringToInt(strMarkType);
        switch (regionType)
        {
        case ERegionType_Circle:
        {

            const char *strLongitude = pXMLNebulaRegionInfo->Attribute("Longitude");
            const char *strLatitude = pXMLNebulaRegionInfo->Attribute("Latitude");
            const char *strHeight = pXMLNebulaRegionInfo->Attribute("Height");
            const char *strRadius = pXMLNebulaRegionInfo->Attribute("Radius");
            if (strLongitude == NULL || strLatitude == NULL || strRadius == NULL
                    || strLongitude[0] == '\0' || strLatitude[0] == '\0' || strRadius[0] == '\0')
            {
                break;
            }
            CRegionCircle *regionCircle = new CRegionCircle(StringToDouble(strLongitude), StringToDouble(strLatitude), StringToInt(strRadius));
            regionBase = regionCircle;
        }
        break;
        case ERegionType_Polygon:
        {

            CRegionPolygon::PointList listPoint;

            TiXmlElement *pXMLPoint = pXMLNebulaRegionInfo->FirstChildElement("Point");
            while (pXMLPoint)
            {

                const char *strPointLongitude = pXMLPoint->Attribute("Longitude");
                const char *strPointLatitude = pXMLPoint->Attribute("Latitude");
                const char *strPointHeight = pXMLPoint->Attribute("Height");
                if (strPointLongitude == NULL || strPointLatitude == NULL
                        || strPointLongitude[0] == '\0' || strPointLatitude[0] == '\0')
                {
                    pXMLPoint = pXMLPoint->NextSiblingElement("Point");
                    continue;
                }

                CRegionPolygon::Point pointPolygon;
                pointPolygon.fLongitude = StringToDouble(strPointLongitude);
                pointPolygon.fLatitude = StringToDouble(strPointLatitude);
                listPoint.push_back(pointPolygon);
                pXMLPoint = pXMLPoint->NextSiblingElement("Point");
            }
            CRegionPolygon *regionPolygon = new CRegionPolygon(listPoint);
            regionBase = regionPolygon;
        }
        break;
        case ERegionType_Ellipse:
        {

            const char *strLongitude = pXMLNebulaRegionInfo->Attribute("Longitude");
            const char *strLatitude = pXMLNebulaRegionInfo->Attribute("Latitude");
            const char *strHeight = pXMLNebulaRegionInfo->Attribute("Height");
            const char *strRadiusMajor = pXMLNebulaRegionInfo->Attribute("RadiusMajor");
            const char *strRadiusMinor = pXMLNebulaRegionInfo->Attribute("RadiusMinor");
            if (strLongitude == NULL || strLatitude == NULL || strRadiusMajor == NULL || strRadiusMinor == NULL
                    || strLongitude[0] == '\0' || strLatitude[0] == '\0' || strRadiusMajor[0] == '\0' || strRadiusMinor[0] == '\0')
            {
                break;
            }
            CRegionEllipse *regionEllipse = new CRegionEllipse();
            if (regionEllipse != NULL)
            {
                regionEllipse->SetCenter(StringToDouble(strLongitude), StringToDouble(strLatitude));
                regionEllipse->SetRadiusMajor(StringToDouble(strRadiusMajor));
                regionEllipse->SetRadiusMinor(StringToDouble(strRadiusMinor));
                regionEllipse->SetRatationAngle(0.0f);
                regionEllipse->SetStartArc(0.0f);
                regionEllipse->SetEndArc(360.0f);
                const char *strRatation = pXMLNebulaRegionInfo->Attribute("Ratation");
                if (strRatation != NULL && strRatation[0] != '\0')
                {
                    regionEllipse->SetRatationAngle(StringToDouble(strRatation));
                }
                const char *strStartArc = pXMLNebulaRegionInfo->Attribute("StartArc");
                if (strStartArc != NULL && strStartArc[0] != '\0')
                {
                    regionEllipse->SetStartArc(StringToDouble(strStartArc));
                }
                const char *strEndArc = pXMLNebulaRegionInfo->Attribute("EndArc");
                if (strEndArc != NULL && strEndArc[0] != '\0')
                {
                    regionEllipse->SetEndArc(StringToDouble(strEndArc));
                }
                regionBase = regionEllipse;
            }
        }
        break;
        case ERegionType_Sector:
        {

            const char *strLongitude = pXMLNebulaRegionInfo->Attribute("Longitude");
            const char *strLatitude = pXMLNebulaRegionInfo->Attribute("Latitude");
            const char *strHeight = pXMLNebulaRegionInfo->Attribute("Height");
            const char *strRadius = pXMLNebulaRegionInfo->Attribute("Radius");
            if (strLongitude == NULL || strLatitude == NULL || strRadius == NULL
                    || strLongitude[0] == '\0' || strLatitude[0] == '\0' || strRadius[0] == '\0')
            {
                break;
            }
            CRegionSector *regionSector = new CRegionSector();
            if (regionSector != NULL)
            {
                regionSector->SetCenter(StringToDouble(strLongitude), StringToDouble(strLatitude));
                regionSector->SetRadius(StringToDouble(strRadius));
                regionSector->SetRatationAngle(0.0f);
                regionSector->SetStartArc(0.0f);
                regionSector->SetEndArc(360.0f);
                const char *strRatation = pXMLNebulaRegionInfo->Attribute("Ratation");
                if (strRatation != NULL && strRatation[0] != '\0')
                {
                    regionSector->SetRatationAngle(StringToDouble(strRatation));
                }
                const char *strStartArc = pXMLNebulaRegionInfo->Attribute("StartArc");
                if (strStartArc != NULL && strStartArc[0] != '\0')
                {
                    regionSector->SetStartArc(StringToDouble(strStartArc));
                }
                const char *strEndArc = pXMLNebulaRegionInfo->Attribute("EndArc");
                if (strEndArc != NULL && strEndArc[0] != '\0')
                {
                    regionSector->SetEndArc(StringToDouble(strEndArc));
                }
                regionBase = regionSector;
            }
        }
        break;
        default:
            regionBase = NULL;
            break;
        }

        if (regionBase != NULL)
        {
            regionBase->SetRegionID(strRegionId);
            regionBase->SetHeight(StringToInt(strUpHeight), StringToInt(strDownHeight));
            regionBase->SetAttribute(ERegionAttribute(StringToInt(strAttribute)));
            regionBase->SetProperty(ERegionProperty(StringToInt(strProperty)));
        }

        NebulaRegionInfo nebulaRegionInfo;
        nebulaRegionInfo.SetRegion(regionBase);
        nebulaRegionInfo.SetDestoryMode(ENebulaTaskDestroyMode(StringToInt(strDestoryMode)));
        nebulaRegionInfo.SetThreaten(StringToInt(strThreaten));
        const char *strDescription = pXMLNebulaRegionInfo->Attribute("Description");
        if (strDescription != NULL)
        {
            nebulaRegionInfo.SetDescription(strDescription);
        }
        nebulaRegionInfoList.push_back(nebulaRegionInfo);
        pXMLNebulaRegionInfo = pXMLNebulaRegionInfo->NextSiblingElement("RegionInfo");
    }
    return true;
}

bool ITaskManageImpl::ParseNebulaRouteLineNode(TiXmlElement *pLineNodeEle, CNebulaRouteLine &NebulaLineInfo)
{
    if (!pLineNodeEle)
    {
        return false;
    }
    const char *pChar = pLineNodeEle->Attribute("WeaponID");
    if (!pChar)
    {
        return false;
    }
    NebulaLineInfo.SetWeaponID(pChar);

    pChar = pLineNodeEle->Attribute("FormOrdinalNum");
    if (pChar)
    {
        NebulaLineInfo.SetFormOrdinalNum(StringToInt(pChar));
    }

    pChar = pLineNodeEle->Attribute("MarkID");
    if (pChar)
    {
        NebulaLineInfo.SetMarkID(pChar);
    }

    TiXmlElement *pChildEle = pLineNodeEle->FirstChildElement("NebulaRoutePoint");
    while (pChildEle)
    {
        CNebulaRoutePoint pointInfo;
        if (ParseNebulaRoutePointNode(pChildEle, pointInfo))
        {
            NebulaLineInfo.AddPointToTail(pointInfo);
        }
        pChildEle = pChildEle->NextSiblingElement("NebulaRoutePoint");
    }
    return true;
}

bool ITaskManageImpl::ParseNebulaRoutePointNode(TiXmlElement *pPointNodeEle, CNebulaRoutePoint &NebulaPointInfo)
{
    if (!pPointNodeEle)
    {
        return false;
    }
    const char *pChar = pPointNodeEle->Attribute("Longitude");
    if (!pChar)
    {
        return false;
    }
    NebulaPointInfo.SetLongitude(StringToDouble(pChar));
    pChar = pPointNodeEle->Attribute("Latitude");
    if (!pChar)
    {
        return false;
    }
    NebulaPointInfo.SetLatitude(StringToDouble(pChar));
    pChar = pPointNodeEle->Attribute("Height");
    if (!pChar)
    {
        return false;
    }
    NebulaPointInfo.SetHeight(StringToInt(pChar));
    pChar = pPointNodeEle->Attribute("HeightCtrl");
    if (!pChar)
    {
        return false;
    }
    NebulaPointInfo.SetHeightCtrl((CNebulaRoutePoint::EHeightCtrl)(StringToInt(pChar)));
    pChar = pPointNodeEle->Attribute("LandFlag");
    if (!pChar)
    {
        return false;
    }
    NebulaPointInfo.SetLandFlag((CNebulaRoutePoint::ELandFlag)(StringToInt(pChar)));
    pChar = pPointNodeEle->Attribute("TaskFlag");
    if (!pChar)
    {
        return false;
    }
    NebulaPointInfo.SetTaskFlag((CNebulaRoutePoint::ETaskFlag)(StringToInt(pChar)));
    pChar = pPointNodeEle->Attribute("Speed");
    if (!pChar)
    {
        return false;
    }
    NebulaPointInfo.SetSpeed(StringToInt(pChar));
    pChar = pPointNodeEle->Attribute("HoverTime");
    if (!pChar)
    {
        return false;
    }
    NebulaPointInfo.SetHoverTime(StringToInt(pChar));

    pChar = pPointNodeEle->Attribute("MavCmd");
    if (!pChar)
    {
        return true;
    }
    NebulaPointInfo.SetMavCmd(std::strtoul(pChar, NULL, 10));

    pChar = pPointNodeEle->Attribute("MavFrame");
    if (!pChar)
    {
        return true;
    }
    NebulaPointInfo.SetMavFrame(std::strtoul(pChar, NULL, 10));

    pChar = pPointNodeEle->Attribute("Param1");
    if (!pChar)
    {
        return true;
    }
    NebulaPointInfo.SetMavParam1(StringToDouble(pChar));

    pChar = pPointNodeEle->Attribute("Param2");
    if (!pChar)
    {
        return true;
    }
    NebulaPointInfo.SetMavParam2(StringToDouble(pChar));

    pChar = pPointNodeEle->Attribute("Param3");
    if (!pChar)
    {
        return true;
    }
    NebulaPointInfo.SetMavParam3(StringToDouble(pChar));

    pChar = pPointNodeEle->Attribute("Param4");
    if (!pChar)
    {
        return true;
    }
    NebulaPointInfo.SetMavParam4(StringToDouble(pChar));

    pChar = pPointNodeEle->Attribute("Continue");
    if (!pChar)
    {
        return true;
    }
    NebulaPointInfo.SetMavAutoContinue(StringToInt(pChar) != 0);

    pChar = pPointNodeEle->Attribute("Current");
    if (!pChar)
    {
        return true;
    }
    NebulaPointInfo.SetMavIsCurrentItem(StringToInt(pChar) != 0);

    pChar = pPointNodeEle->Attribute("Radius");
    if (!pChar)
    {
        return true;
    }
    NebulaPointInfo.SetRadius(StringToInt(pChar));

    pChar = pPointNodeEle->Attribute("LineProPerTy");
    if (!pChar)
    {
        return true;
    }
    NebulaPointInfo.SetLineProPerTy((CNebulaRoutePoint::ELineProPerTy)(StringToInt(pChar)));

    pChar = pPointNodeEle->Attribute("TurnMode");
    if (!pChar)
    {
        return true;
    }
    NebulaPointInfo.SetTurnMode((CNebulaRoutePoint::ETurnMode)(StringToInt(pChar)));

    return true;
}

bool ITaskManageImpl::BuildNebulaRouteGroupNode(CNebulaTask &nebulaTask, CNebulaRouteGroup &NebulaGroupInfo, TiXmlElement *pGroupNodeEle)
{
    if (!pGroupNodeEle)
    {
        return false;
    }
    pGroupNodeEle->SetAttribute("ID", IntToString(NebulaGroupInfo.GetGroupID()).c_str());
    pGroupNodeEle->SetAttribute("Name", NebulaGroupInfo.GetGroupName());
    pGroupNodeEle->SetAttribute("FormMode", IntToString(NebulaGroupInfo.GetFormMode()).c_str());
    pGroupNodeEle->SetAttribute("FormationID", IntToString(NebulaGroupInfo.GetFormationID()).c_str());
    pGroupNodeEle->SetAttribute("TaskType", NebulaGroupInfo.GetTaskType());

    TiXmlElement *pXMLNebulaTaskPlatformList = new TiXmlElement("NebulaTaskPlatformList");
    if (pXMLNebulaTaskPlatformList != NULL)
    {
        BuildNebulaTaskPlatformListNode(nebulaTask, NebulaGroupInfo, pXMLNebulaTaskPlatformList);
        pGroupNodeEle->LinkEndChild(pXMLNebulaTaskPlatformList);
    }

    CNebulaRouteLine::RouteLineIDList LineIdList;
    NebulaGroupInfo.GetRouteLineIDArray(&LineIdList);
    CNebulaRouteLine::RouteLineIDListItr itrLineIdList = LineIdList.begin();
    for (; itrLineIdList != LineIdList.end(); itrLineIdList++)
    {
        CNebulaRouteLine line;
        if (NebulaGroupInfo.GetRouteLine(*itrLineIdList, &line))
        {
            TiXmlElement *pChildEle = new TiXmlElement("NebulaRouteLine");
            if (!BuildNebulaRouteLineNode(line, pChildEle))
            {
                delete pChildEle;
                continue;
            }
            pGroupNodeEle->LinkEndChild(pChildEle);
        }
    }
    return true;
}

bool ITaskManageImpl::BuildNebulaTaskPlatformListNode(CNebulaTask &nebulaTask, CNebulaRouteGroup &NebulaGroupInfo, TiXmlElement *pXMLNebulaTaskPlatformList)
{
    if (pXMLNebulaTaskPlatformList == NULL)
    {
        return false;
    }

    StringVector weaponIDList;
    NebulaGroupInfo.GetWeaponIDList(&weaponIDList);
    NebulaTaskPlatformMap &nebulaTaskPlatformMap = nebulaTask.GetPlatformInfoList();
    for (auto &nebulaTaskPlatform : nebulaTaskPlatformMap)
    {
        if (weaponIDList.end() == std::find(weaponIDList.begin(), weaponIDList.end(), nebulaTaskPlatform.second.GetPlatformID()))
        {
            continue;
        }
        TiXmlElement *pNebulaTaskPlatform = new TiXmlElement("NebulaTaskPlatform");
        if (pNebulaTaskPlatform != NULL && BuildNebulaTaskPlatformNode(nebulaTaskPlatform.second, pNebulaTaskPlatform))
        {
            pXMLNebulaTaskPlatformList->LinkEndChild(pNebulaTaskPlatform);
        }
        else if (pNebulaTaskPlatform != NULL)
        {
            delete pNebulaTaskPlatform;
            pNebulaTaskPlatform = NULL;
        }
    }
    return true;
}
bool ITaskManageImpl::BuildNebulaTaskPlatformNode(NebulaTaskPlatform &nebulaTaskPlatform, TiXmlElement *pXMLNebulaTaskPlatform)
{
    if (pXMLNebulaTaskPlatform == NULL)
    {
        return false;
    }
    pXMLNebulaTaskPlatform->SetAttribute("WeaponID", (nebulaTaskPlatform.GetPlatformID()).c_str());

    TiXmlElement *pXMLNebulaTaskLoadList = new TiXmlElement("NebulaTaskLoadList");
    if (pXMLNebulaTaskLoadList != NULL)
    {
        NebulaTaskLoadList &nebulaTaskLoadList = nebulaTaskPlatform.GetTaskLoadList();
        BuildNebulaTaskPlatformTaskLoadListNode(nebulaTaskLoadList, pXMLNebulaTaskLoadList);
        pXMLNebulaTaskPlatform->LinkEndChild(pXMLNebulaTaskLoadList);
    }

    TiXmlElement *pXMLNebulaTaskMarkList = new TiXmlElement("NebulaTaskMarkList");
    if (pXMLNebulaTaskMarkList != NULL)
    {
        NebulaMarkTrackList &taskMarkList = nebulaTaskPlatform.GetMarkList();
        BuildNebulaTaskPlatformMarkListNode(taskMarkList, pXMLNebulaTaskMarkList);
        pXMLNebulaTaskPlatform->LinkEndChild(pXMLNebulaTaskMarkList);
    }

    TiXmlElement *pXMLNebulaTaskRegionList = new TiXmlElement("NebulaTaskRegionList");
    if (pXMLNebulaTaskRegionList != NULL)
    {
        std::list<std::string> &taskRegionList = nebulaTaskPlatform.GetRegionList();
        BuildNebulaTaskPlatformRegionListNode(taskRegionList, pXMLNebulaTaskRegionList);
        pXMLNebulaTaskPlatform->LinkEndChild(pXMLNebulaTaskRegionList);
    }

    return true;
}

bool ITaskManageImpl::BuildNebulaTaskPlatformTaskLoadListNode(NebulaTaskLoadList &nebulaTaskLoadList, TiXmlElement *pXMLNebulaTaskPlatformTaskLoadList)
{
    if (pXMLNebulaTaskPlatformTaskLoadList == NULL)
    {
        return false;
    }

    for (auto &nebulaTaskLoad : nebulaTaskLoadList)
    {
        if (nebulaTaskLoad.GetTaskLoadID().empty())
        {
            continue;
        }
        std::string strLoadId = nebulaTaskLoad.GetTaskLoadID();

        TiXmlElement *pXMLNebulaTaskLoad = new TiXmlElement("NebulaTaskLoad");
        if (pXMLNebulaTaskLoad == NULL)
        {
            continue;
        }

        pXMLNebulaTaskLoad->SetAttribute("LoadId", nebulaTaskLoad.GetTaskLoadID().c_str());
        pXMLNebulaTaskLoad->SetAttribute("LoadType", nebulaTaskLoad.GetTaskLoadType());
        pXMLNebulaTaskLoad->SetAttribute("LoadName", nebulaTaskLoad.GetTaskLoadName().c_str());

        TiXmlElement *pXMLLoadParameterList = new TiXmlElement("LoadParameterList");
        if (pXMLLoadParameterList != NULL)
        {
            TaskNoSortParamMap &taskNoSortParamMap = nebulaTaskLoad.GetTaskParamMap();
            for (const auto taskNoSortParam : taskNoSortParamMap)
            {
                const std::string &strKey = taskNoSortParam.first;
                const std::string &strValue = taskNoSortParam.second;
                if (strKey.empty() || strValue.empty())
                {
                    continue;
                }
                TiXmlElement *pXMLLoadParameter = new TiXmlElement("LoadParameter");
                if (pXMLLoadParameter == NULL)
                {
                    continue;
                }
                pXMLLoadParameter->SetAttribute("Key", strKey.c_str());
                pXMLLoadParameter->SetAttribute("Value", strValue.c_str());

                pXMLLoadParameterList->LinkEndChild(pXMLLoadParameter);
            }
            pXMLNebulaTaskLoad->LinkEndChild(pXMLLoadParameterList);
        }

        TiXmlElement *pXMLNebulaTaskSectionList = new TiXmlElement("TaskSectionList");
        if (pXMLNebulaTaskSectionList != NULL)
        {
            NebulaTaskSectionList &nebulaTaskSectionList = nebulaTaskLoad.GetTaskSectionList();
            for (auto &nebulaTaskSection : nebulaTaskSectionList)
            {
                TiXmlElement *pXMLTaskSection = new TiXmlElement("TaskSection");
                if (pXMLTaskSection == NULL)
                {
                    continue;
                }

                pXMLTaskSection->SetAttribute("TaskType", nebulaTaskSection.GetTaskType());

                pXMLTaskSection->SetAttribute("WorkMode", nebulaTaskSection.GetWorkMode());

                pXMLTaskSection->SetAttribute("FlightSegmentBegin", nebulaTaskSection.GetFlightSegmentBegin());
                pXMLTaskSection->SetAttribute("FlightSegmentEnd", nebulaTaskSection.GetFlightSegmentEnd());

                pXMLTaskSection->SetAttribute("TimeSegmentStart", nebulaTaskSection.GetTimeSegmentBegin());
                pXMLTaskSection->SetAttribute("TimeSegmentEnd", nebulaTaskSection.GetTimeSegmentEnd());

                TiXmlElement *pXMLSectionParameterList = new TiXmlElement("SectionParameterList");
                if (pXMLSectionParameterList != NULL)
                {
                    TaskNoSortParamMap &taskSectionNoSortParamMap = nebulaTaskSection.GetParamList();
                    for (const auto &taskSectionNoSortParam : taskSectionNoSortParamMap)
                    {
                        const std::string &strKey = taskSectionNoSortParam.first;
                        const std::string &strValue = taskSectionNoSortParam.second;
                        if (strKey.empty() || strValue.empty())
                        {
                            continue;
                        }
                        TiXmlElement *pXMLSectionParameter = new TiXmlElement("SectionParameter");
                        if (pXMLSectionParameter == NULL)
                        {
                            continue;
                        }

                        pXMLSectionParameter->SetAttribute("Key", strKey.c_str());
                        pXMLSectionParameter->SetAttribute("Value", strValue.c_str());

                        pXMLSectionParameterList->LinkEndChild(pXMLSectionParameter);
                    }
                    pXMLTaskSection->LinkEndChild(pXMLSectionParameterList);
                }
                pXMLNebulaTaskSectionList->LinkEndChild(pXMLTaskSection);
            }
            pXMLNebulaTaskLoad->LinkEndChild(pXMLNebulaTaskSectionList);
        }
        pXMLNebulaTaskPlatformTaskLoadList->LinkEndChild(pXMLNebulaTaskLoad);
    }

    return true;
}

bool ITaskManageImpl::BuildNebulaTaskPlatformMarkListNode(NebulaMarkTrackList &taskMarkList, TiXmlElement *pXMLNebulaTaskMarkList)
{
    if (pXMLNebulaTaskMarkList == NULL)
    {
        return false;
    }

    for (const auto &taskMark : taskMarkList)
    {
        const std::string &strStationId = taskMark.first;
        const std::string &strBatchId = taskMark.second;
        if (strStationId.empty() || strBatchId.empty())
        {
            continue;
        }

        TiXmlElement *pXMLTaskMark = new TiXmlElement("TaskMark");
        if (pXMLTaskMark == NULL)
        {
            continue;
        }

        pXMLTaskMark->SetAttribute("StationId", strStationId.c_str());
        pXMLTaskMark->SetAttribute("BatchId", strBatchId.c_str());

        pXMLNebulaTaskMarkList->LinkEndChild(pXMLTaskMark);
    }

    return true;
}

bool ITaskManageImpl::BuildNebulaTaskPlatformRegionListNode(std::list<std::string> &taskRegionList, TiXmlElement *pXMLNebulaTaskRegionList)
{
    if (pXMLNebulaTaskRegionList == NULL)
    {
        return false;
    }

    for (const auto &taskRegion : taskRegionList)
    {
        const std::string &strRegionId = taskRegion;
        if (strRegionId.empty())
        {
            continue;
        }

        TiXmlElement *pXMLTaskRegion = new TiXmlElement("TaskRegion");
        if (pXMLTaskRegion == NULL)
        {
            continue;
        }

        pXMLTaskRegion->SetAttribute("RegionId", strRegionId.c_str());

        pXMLNebulaTaskRegionList->LinkEndChild(pXMLTaskRegion);
    }

    return true;
}

bool ITaskManageImpl::BuildNebulaMarkInfoListNode(CNebulaTask &nebulaTask, TiXmlElement *pXMLNebulaMarkInfoList)
{
    if (pXMLNebulaMarkInfoList == NULL)
    {
        return false;
    }

    NebulaMarkInfoList &nebulaMarkInfoList = nebulaTask.GetMarkInfoList();
    for (auto &nebulaMarkInfo : nebulaMarkInfoList)
    {
        const std::string strStationId = nebulaMarkInfo.GetMarkStationID();
        const std::string strBatchId = nebulaMarkInfo.GetMarkBatchID();
        if (strStationId.empty() || strBatchId.empty())
        {
            continue;
        }

        TiXmlElement *pXMLNebulaMarkInfo = new TiXmlElement("MarkInfo");
        if (pXMLNebulaMarkInfo == NULL)
        {
            continue;
        }

        pXMLNebulaMarkInfo->SetAttribute("StationId", strStationId.c_str());
        pXMLNebulaMarkInfo->SetAttribute("BatchId", strBatchId.c_str());

        pXMLNebulaMarkInfo->SetAttribute("Property", nebulaMarkInfo.GetMarkProperty());

        pXMLNebulaMarkInfo->SetAttribute("Attribute", nebulaMarkInfo.GetMarkAttribute());

        pXMLNebulaMarkInfo->SetAttribute("MarkType", nebulaMarkInfo.GetMarkType());

        pXMLNebulaMarkInfo->SetAttribute("MovableType", nebulaMarkInfo.GetMovableType());

        pXMLNebulaMarkInfo->SetAttribute("DestoryMode", nebulaMarkInfo.GetDestoryMode());

        pXMLNebulaMarkInfo->SetAttribute("Threaten", IntToString(nebulaMarkInfo.GetThreaten()).c_str());

        pXMLNebulaMarkInfo->SetAttribute("Longitude", DoubleToString(nebulaMarkInfo.GetLongitude()).c_str());
        pXMLNebulaMarkInfo->SetAttribute("Latitude", DoubleToString(nebulaMarkInfo.GetLatitude()).c_str());
        pXMLNebulaMarkInfo->SetAttribute("Height", IntToString(nebulaMarkInfo.GetHeight()).c_str());

        pXMLNebulaMarkInfo->SetAttribute("Description", nebulaMarkInfo.GetDescription().c_str());

        pXMLNebulaMarkInfoList->LinkEndChild(pXMLNebulaMarkInfo);
    }
    return true;
}

bool ITaskManageImpl::BuildNebulaRegionInfoListNode(CNebulaTask &nebulaTask, TiXmlElement *pXMLNebulaRegionInfoList)
{
    if (pXMLNebulaRegionInfoList == NULL)
    {
        return false;
    }

    NebulaRegionInfoList &nebulaRegionInfoList = nebulaTask.GetRegionInfoList();
    for (auto &nebulaRegionInfo : nebulaRegionInfoList)
    {
        CRegionBase *regionBase = nebulaRegionInfo.GetRegion();
        if (regionBase == NULL)
        {
            continue;
        }
        const std::string strRegionId = regionBase->GetRegionID();
        if (strRegionId.empty() || strRegionId.empty())
        {
            continue;
        }

        TiXmlElement *pXMLNebulaRegionInfo = new TiXmlElement("RegionInfo");
        if (pXMLNebulaRegionInfo == NULL)
        {
            continue;
        }

        pXMLNebulaRegionInfo->SetAttribute("RegionId", strRegionId.c_str());

        pXMLNebulaRegionInfo->SetAttribute("RegionType", regionBase->GetRegionType());

        pXMLNebulaRegionInfo->SetAttribute("Property", regionBase->GetProperty());

        pXMLNebulaRegionInfo->SetAttribute("Attribute", regionBase->GetAttribute());

        pXMLNebulaRegionInfo->SetAttribute("DestoryMode", nebulaRegionInfo.GetDestoryMode());

        pXMLNebulaRegionInfo->SetAttribute("Threaten", nebulaRegionInfo.GetThreaten());

        pXMLNebulaRegionInfo->SetAttribute("DownHeight", regionBase->GetDownHeight());

        pXMLNebulaRegionInfo->SetAttribute("UpHeight", regionBase->GetUpHeight());

        if (typeid(*regionBase) == typeid(CRegionCircle))
        {
            CRegionCircle *regionCircle = dynamic_cast<CRegionCircle *>(regionBase);
            if (regionCircle != NULL)
            {

                pXMLNebulaRegionInfo->SetAttribute("Longitude", DoubleToString(regionCircle->GetCenterLongitude()).c_str());
                pXMLNebulaRegionInfo->SetAttribute("Latitude", DoubleToString(regionCircle->GetCenterLatitude()).c_str());
                pXMLNebulaRegionInfo->SetAttribute("Height", IntToString(regionCircle->GetDownHeight()).c_str());
                pXMLNebulaRegionInfo->SetAttribute("Radius", regionCircle->GetRadius());
            }
        }
        else if (typeid(*regionBase) == typeid(CRegionPolygon))
        {
            CRegionPolygon *regionPolygon = dynamic_cast<CRegionPolygon *>(regionBase);
            if (regionPolygon != NULL)
            {

                CRegionPolygon::PointList &listPoint = regionPolygon->GetPointList();

                for (const auto &point : listPoint)
                {
                    TiXmlElement *pXMLPoint = new TiXmlElement("Point");
                    if (pXMLPoint == NULL)
                    {
                        continue;
                    }
                    pXMLPoint->SetAttribute("Longitude", DoubleToString(point.fLongitude).c_str());
                    pXMLPoint->SetAttribute("Latitude", DoubleToString(point.fLatitude).c_str());
                    pXMLPoint->SetAttribute("Height", IntToString(regionPolygon->GetDownHeight()).c_str());
                    pXMLNebulaRegionInfo->LinkEndChild(pXMLPoint);
                }
            }
        }
        else if (typeid(*regionBase) == typeid(CRegionEllipse))
        {
            CRegionEllipse *regionEllipse = dynamic_cast<CRegionEllipse *>(regionBase);
            if (regionEllipse != NULL)
            {

                pXMLNebulaRegionInfo->SetAttribute("Longitude", DoubleToString(regionEllipse->GetCenterLongitude()).c_str());
                pXMLNebulaRegionInfo->SetAttribute("Latitude", DoubleToString(regionEllipse->GetCenterLatitude()).c_str());
                pXMLNebulaRegionInfo->SetAttribute("Height", IntToString(regionEllipse->GetDownHeight()).c_str());
                pXMLNebulaRegionInfo->SetDoubleAttribute("RadiusMajor", regionEllipse->GetRadiusMajor());
                pXMLNebulaRegionInfo->SetDoubleAttribute("RadiusMinor", regionEllipse->GetRadiusMinor());
                pXMLNebulaRegionInfo->SetDoubleAttribute("Ratation", regionEllipse->GetRatationAngle());
                pXMLNebulaRegionInfo->SetDoubleAttribute("StartArc", regionEllipse->GetStartArc());
                pXMLNebulaRegionInfo->SetDoubleAttribute("EndArc", regionEllipse->GetEndArc());
            }
        }
        else if (typeid(*regionBase) == typeid(CRegionSector))
        {
            CRegionSector *regionSector = dynamic_cast<CRegionSector *>(regionBase);
            if (regionSector != NULL)
            {

                pXMLNebulaRegionInfo->SetAttribute("Longitude", DoubleToString(regionSector->GetCenterLongitude()).c_str());
                pXMLNebulaRegionInfo->SetAttribute("Latitude", DoubleToString(regionSector->GetCenterLatitude()).c_str());
                pXMLNebulaRegionInfo->SetAttribute("Height", IntToString(regionSector->GetDownHeight()).c_str());
                pXMLNebulaRegionInfo->SetDoubleAttribute("Radius", regionSector->GetRadius());
                pXMLNebulaRegionInfo->SetDoubleAttribute("Ratation", regionSector->GetRatationAngle());
                pXMLNebulaRegionInfo->SetDoubleAttribute("StartArc", regionSector->GetStartArc());
                pXMLNebulaRegionInfo->SetDoubleAttribute("EndArc", regionSector->GetEndArc());
            }
        }
        pXMLNebulaRegionInfo->SetAttribute("Description", nebulaRegionInfo.GetDescription().c_str());
        pXMLNebulaRegionInfoList->LinkEndChild(pXMLNebulaRegionInfo);
    }
    return true;
}

bool ITaskManageImpl::BuildNebulaRouteLineNode(CNebulaRouteLine &NebulaLineInfo, TiXmlElement *pLineNodeEle)
{
    if (!pLineNodeEle)
    {
        return false;
    }
    pLineNodeEle->SetAttribute("WeaponID", NebulaLineInfo.GetWeaponID());
    pLineNodeEle->SetAttribute("FormOrdinalNum", NebulaLineInfo.GetFormOrdinalNum());
    pLineNodeEle->SetAttribute("MarkID", NebulaLineInfo.GetMarkID());
    CNebulaRoutePoint::RoutePointIDList PointIdList;
    NebulaLineInfo.GetRoutePointIDList(&PointIdList);
    CNebulaRoutePoint::RoutePointIDListItr itrList = PointIdList.begin();
    for (; itrList != PointIdList.end(); itrList++)
    {
        const CNebulaRoutePoint *pPoint = NebulaLineInfo.GetRoutePoint(*itrList);
        if (pPoint)
        {
            TiXmlElement *pChildEle = new TiXmlElement("NebulaRoutePoint");
            if (!BuildNebulaRoutePointNode((CNebulaRoutePoint)*pPoint, pChildEle))
            {
                delete pChildEle;
                continue;
            }
            pLineNodeEle->LinkEndChild(pChildEle);
        }
    }

    return true;
}

bool ITaskManageImpl::BuildNebulaRoutePointNode(const CNebulaRoutePoint &NebulaPointInfo, TiXmlElement *pPointNodeEle)
{
    if (!pPointNodeEle)
    {
        return false;
    }
    pPointNodeEle->SetAttribute("Longitude", DoubleToString(NebulaPointInfo.GetLongitude()).c_str());
    pPointNodeEle->SetAttribute("Latitude", DoubleToString(NebulaPointInfo.GetLatitude()).c_str());
    pPointNodeEle->SetAttribute("Height", IntToString(NebulaPointInfo.GetHeight()).c_str());
    pPointNodeEle->SetAttribute("HeightCtrl", IntToString((int)NebulaPointInfo.GetHeightCtrl()).c_str());
    pPointNodeEle->SetAttribute("LandFlag", IntToString((int)NebulaPointInfo.GetLandFlag()).c_str());
    pPointNodeEle->SetAttribute("TaskFlag", IntToString((int)NebulaPointInfo.GetTaskFlag()).c_str());
    pPointNodeEle->SetAttribute("Speed", IntToString(NebulaPointInfo.GetSpeed()).c_str());
    pPointNodeEle->SetAttribute("HoverTime", IntToString(NebulaPointInfo.GetHoverTime()).c_str());

    pPointNodeEle->SetAttribute("MavCmd", std::to_string(NebulaPointInfo.GetMavCmd()).c_str());
    pPointNodeEle->SetAttribute("MavFrame", std::to_string(NebulaPointInfo.GetMavFrame()).c_str());
    pPointNodeEle->SetAttribute("Param1", DoubleToString(NebulaPointInfo.GetMavParam1()).c_str());
    pPointNodeEle->SetAttribute("Param2", DoubleToString(NebulaPointInfo.GetMavParam2()).c_str());
    pPointNodeEle->SetAttribute("Param3", DoubleToString(NebulaPointInfo.GetMavParam3()).c_str());
    pPointNodeEle->SetAttribute("Param4", DoubleToString(NebulaPointInfo.GetMavParam4()).c_str());
    pPointNodeEle->SetAttribute("Continue", IntToString(NebulaPointInfo.GetMavAutoContinue()).c_str());
    pPointNodeEle->SetAttribute("Current", IntToString(NebulaPointInfo.GetMavIsCurrentItem()).c_str());

    pPointNodeEle->SetAttribute("Radius", IntToString(NebulaPointInfo.GetRadius()).c_str());
    pPointNodeEle->SetAttribute("LineProPerTy", IntToString(NebulaPointInfo.GetLineProPerTy()).c_str());
    pPointNodeEle->SetAttribute("TurnMode", IntToString(NebulaPointInfo.GetTurnMode()).c_str());

    return true;
}

INebulaTaskManageService::NebulaTaskFileNameList ITaskManageImpl::GetAllNebulaTaskFileName()
{

    QString strPath = m_strDatabasePath.c_str();
    QString strTemp;

    NebulaTaskFileNameList nameList;
    QDir findFile(strPath);
    findFile.setFilter(QDir::Files);
    QStringList filters;
    filters << "*.dat";
    findFile.setNameFilters(filters);
    QStringList list = findFile.entryList(QDir::Files);
    foreach (QFileInfo file, list)
    {
        nameList.push_back(file.fileName().toStdString());
    }
    return nameList;
}

INebulaTaskManageService::EOperatorNebulaTask ITaskManageImpl::OpenNebulaTaskFile(const char *strFileName)
{

    if (NULL == strFileName)
    {
        return INebulaTaskManageService::EOperatorNebulaTask_ErrNoFile;
    }

    QString cstrFileName = strFileName;
    if (-1 == cstrFileName.lastIndexOf(".dat"))
    {
        cstrFileName += ".dat";
    }

    QString cstrFullPath = QString::fromStdString(m_strDatabasePath) + cstrFileName;

    QFile findFile;
    findFile.setFileName(cstrFullPath);
    bool bFind = findFile.exists();

    if (!bFind)
    {
        return INebulaTaskManageService::EOperatorNebulaTask_ErrNoFile;
    }

    std::string strDataPath = cstrFullPath.toLocal8Bit().data();
    TiXmlDocument doc;
    bool bLoadFile = doc.LoadFile(strDataPath.c_str());
    if (!bLoadFile)
    {
        return INebulaTaskManageService::EOperatorNebulaTask_ErrBadFile;
    }
    TiXmlElement *pRootEle = doc.RootElement();
    if (!pRootEle)
    {
        return INebulaTaskManageService::EOperatorNebulaTask_ErrBadFormat;
    }
    TiXmlElement *pNebulaTaskEle = pRootEle->FirstChildElement("NebulaTask");
    while (pNebulaTaskEle)
    {
        const char *pChar = pNebulaTaskEle->Attribute("ID");
        if (!pChar)
        {
            return INebulaTaskManageService::EOperatorNebulaTask_ErrBadFormat;
        }
        unsigned int nTaskID = StringToInt(pChar);
        pChar = pNebulaTaskEle->Attribute("Name");
        if (!pChar)
        {
            return INebulaTaskManageService::EOperatorNebulaTask_ErrBadFormat;
        }
        const char *strTaskName = pChar;
        pChar = pNebulaTaskEle->Attribute("CodeName");
        if (!pChar)
        {
            return INebulaTaskManageService::EOperatorNebulaTask_ErrBadFormat;
        }
        unsigned int nCodeName = StringToInt(pChar);

        CNebulaTask nebulaTask;
        nebulaTask.SetTaskID(nTaskID);
        nebulaTask.SetTaskName(strTaskName);
        nebulaTask.SetCodeName(nCodeName);
        nebulaTask.SetTaskTime(0, 0);
        nebulaTask.SetTaskType(ENebulaTaskType::ENebulaTaskType_Unknown);

        const char *strStartTime = pNebulaTaskEle->Attribute("StartTime");
        const char *strEndTime = pNebulaTaskEle->Attribute("EndTime");
        if (strStartTime != NULL && strEndTime != NULL)
        {
            char  *stopstring;
            unsigned long long startTime = strtoull(strStartTime, &stopstring, 10);
            unsigned long long endTime = strtoull(strEndTime, &stopstring, 10);
            nebulaTask.SetTaskTime(startTime, endTime);
        }

        const char *strTaskType = pNebulaTaskEle->Attribute("TaskType");
        if (strTaskType != NULL)
        {
            nebulaTask.SetTaskType(ENebulaTaskType(StringToInt(strTaskType)));
        }

        TiXmlElement *pChildEle = pNebulaTaskEle->FirstChildElement("NebulaRouteGroup");
        while (pChildEle)
        {
            CNebulaRouteGroup groupInfo;
            if (ParseNebulaRouteGroupNode(pChildEle, nebulaTask, groupInfo))
            {
                nebulaTask.AddRouteGroup(groupInfo);
            }
            else
            {
                return INebulaTaskManageService::EOperatorNebulaTask_ErrBadFormat;
            }
            pChildEle = pChildEle->NextSiblingElement("NebulaRouteGroup");
        }

        TiXmlElement *pXMLNebulaMarkInfoList = pNebulaTaskEle->FirstChildElement("NebulaMarkInfoList");
        if (pXMLNebulaMarkInfoList != NULL)
        {
            ParseNebulaMarkInfoListNode(pXMLNebulaMarkInfoList, nebulaTask);
        }

        TiXmlElement *pXMLNebulaRegionInfoList = pNebulaTaskEle->FirstChildElement("NebulaRegionInfoList");
        if (pXMLNebulaRegionInfoList != NULL)
        {
            ParseNebulaRegionInfoListNode(pXMLNebulaRegionInfoList, nebulaTask);
        }

        m_mapNebulaTask.insert(std::make_pair(nTaskID, nebulaTask.Copy()));
        pNebulaTaskEle = pNebulaTaskEle->NextSiblingElement("NebulaTask");
    }
    INebulaTaskManageSubject::Notify();

    return INebulaTaskManageService::EOperatorNebulaTask_Succeed;
}

INebulaTaskManageService::EOperatorNebulaTask ITaskManageImpl::SaveNebulaTaskFile(const char *strFileName)
{

    if (NULL == strFileName)
    {
        return INebulaTaskManageService::EOperatorNebulaTask_ErrNoFile;
    }
	QString cstrFileName = QString::fromLocal8Bit(strFileName);
    if (-1 == cstrFileName.lastIndexOf(".dat"))
    {
        cstrFileName += ".dat";
    }

    QString cstrFullPath = QString::fromStdString(m_strDatabasePath) + cstrFileName;

    TiXmlDocument docObject;
    TiXmlDeclaration *pDeclarationEle = new TiXmlDeclaration("1.0", "GB2312", "");
    docObject.LinkEndChild(pDeclarationEle);
    TiXmlElement *pRootEle = new TiXmlElement("NebulaTaskList");
    docObject.LinkEndChild(pRootEle);
    NebulaTaskPtrMapItr itrList = m_mapNebulaTask.begin();
    for (; itrList != m_mapNebulaTask.end(); itrList++)
    {
        CNebulaTask *pNebulaTask = itrList->second;
        if (pNebulaTask)
        {
            TiXmlElement *pTaskEle = new TiXmlElement("NebulaTask");
            pRootEle->LinkEndChild(pTaskEle);
            pTaskEle->SetAttribute("ID", IntToString(pNebulaTask->GetTaskID()).c_str());
            pTaskEle->SetAttribute("Name", pNebulaTask->GetTaskName());
            pTaskEle->SetAttribute("CodeName", pNebulaTask->GetCodeName());
            pTaskEle->SetAttribute("StartTime", pNebulaTask->GetTaskStartTime());
            pTaskEle->SetAttribute("EndTime", std::to_string(pNebulaTask->GetTaskEndTime()).c_str());
            pTaskEle->SetAttribute("TaskType", pNebulaTask->GetTaskType());

            CNebulaRouteGroup::RouteGroupIDList groupIdList;
            pNebulaTask->GetRouteGroupIDArray(&groupIdList);
            CNebulaRouteGroup::RouteGroupIDListItr itrGroupList = groupIdList.begin();
            for (; itrGroupList != groupIdList.end(); itrGroupList++)
            {
                CNebulaRouteGroup grooupInfo;
                pNebulaTask->GetRouteGroup(*itrGroupList, &grooupInfo);
                TiXmlElement *pGroupEle = new TiXmlElement("NebulaRouteGroup");
                if (!BuildNebulaRouteGroupNode(*pNebulaTask, grooupInfo, pGroupEle))
                {
                    delete pGroupEle;
                    continue;
                }
                pTaskEle->LinkEndChild(pGroupEle);
            }

            TiXmlElement *pXMLNebulaMarkInfoList = new TiXmlElement("NebulaMarkInfoList");
            if (pXMLNebulaMarkInfoList != NULL)
            {
                BuildNebulaMarkInfoListNode(*pNebulaTask, pXMLNebulaMarkInfoList);
                pTaskEle->LinkEndChild(pXMLNebulaMarkInfoList);
            }

            TiXmlElement *pXMLNebulaRegionInfoList = new TiXmlElement("NebulaRegionInfoList");
            if (pXMLNebulaRegionInfoList != NULL)
            {
                BuildNebulaRegionInfoListNode(*pNebulaTask, pXMLNebulaRegionInfoList);
                pTaskEle->LinkEndChild(pXMLNebulaRegionInfoList);
            }
        }
    }
    bool bSave = docObject.SaveFile(cstrFullPath.toLocal8Bit());
    if (!bSave)
    {
        return INebulaTaskManageService::EOperatorNebulaTask_ErrNullPath;
    }
    INebulaTaskManageSubject::Notify();
    return INebulaTaskManageService::EOperatorNebulaTask_Succeed;
}

INebulaTaskManageService::EOperatorNebulaTask ITaskManageImpl::RemoveNebulaTaskFile(const char *strFileName)
{
    ;
    if (NULL == strFileName)
    {
        return INebulaTaskManageService::EOperatorNebulaTask_ErrNoFile;
    }

    QString cstrFileName = strFileName;
    if (-1 == cstrFileName.lastIndexOf(".dat"))
    {
        cstrFileName += ".dat";
    }
    QString cstrFullPath = QString::fromStdString(m_strDatabasePath) + cstrFileName;

    QFile findFile;
    findFile.setFileName(cstrFullPath);
    bool bFind = findFile.exists();
    if (!bFind)
    {
        return INebulaTaskManageService::EOperatorNebulaTask_ErrNoFile;
    }

    bool bDelteFile = findFile.remove();
    if (bDelteFile)
    {
        return INebulaTaskManageService::EOperatorNebulaTask_Succeed;
    }
    return INebulaTaskManageService::EOperatorNebulaTask_ErrDelFile;
}

INebulaTaskManageService::EOperatorNebulaTask ITaskManageImpl::IsExistNebulaTaskFile(const char *strFileName)
{
    if (NULL == strFileName)
    {
        return INebulaTaskManageService::EOperatorNebulaTask_ErrNoFile;
    }
    NebulaTaskFileNameList NameList = ITaskManageImpl::GetAllNebulaTaskFileName();
    NebulaTaskFileNameListItr itrNameList = NameList.begin();
    for (; itrNameList != NameList.end(); itrNameList++)
    {
        std::string strName = *itrNameList;
        if (strName == strFileName)
        {
            return  INebulaTaskManageService::EOperatorNebulaTask_Succeed;
        }
    }
    return INebulaTaskManageService::EOperatorNebulaTask_ErrNoFile;
}

bool ITaskManageImpl::GetNebulaTaskIDList(CNebulaTask::LPNebulaTaskIDList TaskIdList)
{
    if (!TaskIdList)
    {
        return false;
    }
    ;
    NebulaTaskPtrMapItr itrTask = m_mapNebulaTask.begin();
    for (; itrTask != m_mapNebulaTask.end(); itrTask++)
    {
        unsigned int nId = itrTask->first;
        TaskIdList->push_back(nId);
    }
    return true;
}

void ITaskManageImpl::ClearNebulaTaskList()
{
    ;
    NebulaTaskPtrMapItr itrTask = m_mapNebulaTask.begin();
    for (; itrTask != m_mapNebulaTask.end(); itrTask++)
    {
        CNebulaTask *pNebulatask = itrTask->second;
        if (pNebulatask)
        {
            delete pNebulatask;
            pNebulatask = NULL;
        }
    }
    m_mapNebulaTask.clear();
    INebulaTaskManageSubject::Notify();
}

bool ITaskManageImpl::GetNebulaTask(unsigned int nNebulaTaskId, LPNebulaTask taskInfo)
{
    if (NEBULATASK_ID_INVAILD == nNebulaTaskId || !taskInfo)
    {
        return false;
    }

    ;
    NebulaTaskPtrMapItr itrTask = m_mapNebulaTask.find(nNebulaTaskId);
    if (itrTask != m_mapNebulaTask.end())
    {
        *taskInfo = *(itrTask->second);
        return true;
    }
    return false;
}

CNebulaTask *ITaskManageImpl::GetNebulaTask(unsigned int nTaskID)
{
    NebulaTaskPtrMapItr	itrMap = m_mapNebulaTask.find(nTaskID);
    if (itrMap != m_mapNebulaTask.end())
    {
        return  itrMap->second;
    }
    return NULL;
}

CNebulaTask *ITaskManageImpl::GetNebulaTask(const char *nTaskName)
{
    NebulaTaskPtrMapItr	itrMap = m_mapNebulaTask.begin();
    for (; itrMap != m_mapNebulaTask.end(); itrMap++)
    {
        if (itrMap->second->GetTaskName() == nTaskName)
        {
            return  itrMap->second;
        }
    }
    return NULL;
}

bool ITaskManageImpl::SetCurrentNebulaTaskID(unsigned int nNebulaTask)
{
    ;
    m_nCurrectTask = nNebulaTask;
    return true;
}

bool ITaskManageImpl::GetCurrentNebulaTaskID(unsigned int &nNebulaTask)
{
    ;
    nNebulaTask = m_nCurrectTask;
    return true;
}

bool ITaskManageImpl::GetCurrentNebulaLastPoint(const char *pWeaponId, LPNebulaRoutePoint pLastPoint)
{
    if (!pWeaponId || !pLastPoint)
    {
        return false;
    }
    ;
    if (NEBULATASK_ID_INVAILD == m_nCurrectTask)
    {
        return false;
    }
    CNebulaTask *pTask = GetNebulaTask(m_nCurrectTask);
    if (!pTask)
    {
        return false;
    }
    return pTask->GetNebulaRoutePointTail(pWeaponId, pLastPoint);
}

bool ITaskManageImpl::GetCurrentNebulaLine(const char *pWeaponId, LPNebulaRouteLine pLine)
{
    if ("" == pWeaponId || !pLine)
    {
        return false;
    }
    ;
    if (NEBULATASK_ID_INVAILD == m_nCurrectTask)
    {
        return false;
    }
    CNebulaTask *pTask = GetNebulaTask(m_nCurrectTask);
    if (!pTask)
    {
        return false;
    }
    return pTask->GetNebulaRouteLine(pWeaponId, pLine);
}

bool ITaskManageImpl::SetNebulaTask(const CNebulaTask &data)
{
    if (NEBULATASK_ID_INVAILD == data.GetTaskID())
    {
        return false;
    }
    ;
    NebulaTaskPtrMapItr itrTask = m_mapNebulaTask.find(data.GetTaskID());
    if (itrTask != m_mapNebulaTask.end())
    {
        *(itrTask->second) = data;
        INebulaTaskManageSubject::Notify();
        return true;
    }
    return false;
}

bool ITaskManageImpl::AddNebulaTask(const CNebulaTask &data)
{
    if (NEBULATASK_ID_INVAILD == data.GetTaskID())
    {
        return false;
    }
    ;
    NebulaTaskPtrMapItr itrTask = m_mapNebulaTask.find(data.GetTaskID());
    if (itrTask != m_mapNebulaTask.end())
    {
        return false;
    }
    CNebulaTask *pNewTask = data.Copy();
    m_mapNebulaTask.insert(std::make_pair(data.GetTaskID(), pNewTask));
    INebulaTaskManageSubject::Notify();
    return true;

}

bool ITaskManageImpl::RemoveNebulaTask(unsigned int nNebulaTaskId)
{
    if (NEBULATASK_ID_INVAILD == nNebulaTaskId)
    {
        return false;
    }
    ;
    NebulaTaskPtrMapItr itrTask = m_mapNebulaTask.find(nNebulaTaskId);
    if (itrTask != m_mapNebulaTask.end())
    {
        CNebulaTask *pNebulaTask = itrTask->second;
        if (pNebulaTask)
        {
            delete pNebulaTask;
            pNebulaTask = NULL;
        }
        m_mapNebulaTask.erase(itrTask);
        INebulaTaskManageSubject::Notify();
        return true;
    }
    return false;
}

unsigned int ITaskManageImpl::CreateNebulaTaskID()
{
    static unsigned int nTask = 1;
    while (IsNebulaExistTask(nTask))
    {
        nTask++;
    }
    return nTask;
}

bool ITaskManageImpl::SetNebulaTaskName(unsigned int nTaskId, const char *strTaskName)
{
    if (NEBULATASK_ID_INVAILD == nTaskId || !strTaskName)
    {
        return false;
    }
    ;
    NebulaTaskPtrMapItr itrTask = m_mapNebulaTask.find(nTaskId);
    if (itrTask != m_mapNebulaTask.end())
    {
        itrTask->second->SetTaskName(strTaskName);
        INebulaTaskManageSubject::Notify();
        return true;
    }
    return false;
}

bool ITaskManageImpl::GetNebulaTaskName(unsigned int nTaskId, std::string &strTaskName)
{
    if (NEBULATASK_ID_INVAILD == nTaskId)
    {
        return false;
    }
    ;
    NebulaTaskPtrMapItr itrTask = m_mapNebulaTask.find(nTaskId);
    if (itrTask != m_mapNebulaTask.end())
    {
        strTaskName = itrTask->second->GetTaskName();
        return true;
    }
    return false;
}

bool ITaskManageImpl::GetNebulaTaskID(const char *strTaskName, unsigned int &nTaskId)
{
    if (NULL == strTaskName)
    {
        return false;
    }
    ;
    NebulaTaskPtrMapItr itrTask = m_mapNebulaTask.begin();
    for (; itrTask != m_mapNebulaTask.end(); itrTask++)
    {
        if (0 == strcmp(itrTask->second->GetTaskName(), strTaskName))
        {
            nTaskId = itrTask->first;
            return true;
        }

    }
    return false;

}

bool ITaskManageImpl::IsNebulaExistTask(unsigned int nTaskId)
{
    if (NEBULATASK_ID_INVAILD == nTaskId)
    {
        return false;
    }
    ;
    NebulaTaskPtrMapItr itrTask = m_mapNebulaTask.find(nTaskId);
    if (itrTask != m_mapNebulaTask.end())
    {
        return true;
    }
    return false;
}

bool ITaskManageImpl::IsExistNebulaTask(const char *strTaskName)
{
    if (NULL == strTaskName)
    {
        return false;
    }
    ;
    NebulaTaskPtrMapItr itrTask = m_mapNebulaTask.begin();
    for (; itrTask != m_mapNebulaTask.end(); itrTask++)
    {
        if (itrTask->second->GetTaskName() == strTaskName)
        {
            return true;
        }
    }
    return false;
}

bool ITaskManageImpl::GetNebulaRouteGroupIDList(unsigned int nTaskId, CNebulaRouteGroup::LPRouteGroupIDList GroupIDList)
{
    if (NEBULATASK_ID_INVAILD == nTaskId || !GroupIDList)
    {
        return false;
    }
    ;
    CNebulaTask *pNebulaTask = GetNebulaTask(nTaskId);
    if (pNebulaTask)
    {
        GroupIDList->clear();
        return pNebulaTask->GetRouteGroupIDArray(GroupIDList);
    }
    return false;
}

bool ITaskManageImpl::GetNebulaRouteGroupNum(unsigned int nTaskId, int &nGroupNum)
{
    ;
    CNebulaTask *pNebulaTask = GetNebulaTask(nTaskId);
    if (pNebulaTask)
    {
        nGroupNum = pNebulaTask->GetRouteGroupNum();
        return true;
    }
    return false;
}

bool ITaskManageImpl::GetNebulaRouteGroup(unsigned int nTaskId, unsigned int nGroupId, LPNebulaRouteGroup lpGroup)
{
    if (NEBULATASK_ID_INVAILD == nTaskId || NEBULAROUTEGROUP_ID_INVAILD == nGroupId || !lpGroup)
    {
        return false;
    }
    ;
    CNebulaTask *pNebulaTask = GetNebulaTask(nTaskId);
    if (pNebulaTask)
    {
        return pNebulaTask->GetRouteGroup(nGroupId, lpGroup);
    }
    return false;
}

bool ITaskManageImpl::AddNebulaRouteGroup(unsigned int nTaskId, const CNebulaRouteGroup &groupInfo)
{
    if (NEBULATASK_ID_INVAILD == nTaskId)
    {
        return false;
    }
    ;
    CNebulaTask *pNebulaTask = GetNebulaTask(nTaskId);
    if (pNebulaTask)
    {
        return pNebulaTask->AddRouteGroup(groupInfo);
    }
    return false;
}

unsigned int ITaskManageImpl::CreateNebulaGroupID(unsigned int nTaskId)
{
    if (NEBULATASK_ID_INVAILD == nTaskId)
    {
        return false;
    }
    ;
    CNebulaTask *pNebulaTask = GetNebulaTask(nTaskId);
    if (pNebulaTask)
    {
        return pNebulaTask->CreateGroupID();
    }
    return false;
}

bool ITaskManageImpl::SetNebulaRouteGroup(unsigned int nTaskId, const CNebulaRouteGroup &RouteGroup)
{
    if (NEBULATASK_ID_INVAILD == nTaskId)
    {
        return false;
    }
    ;
    CNebulaTask *pNebulaTask = GetNebulaTask(nTaskId);
    if (pNebulaTask)
    {
        return pNebulaTask->SetRouteGroup(RouteGroup);
    }
    return false;
}

bool ITaskManageImpl::IsExistNebulaRouteGroup(unsigned int nTaskId, unsigned int nGroupId)
{
    if (NEBULATASK_ID_INVAILD == nTaskId || NEBULAROUTEGROUP_ID_INVAILD == nGroupId)
    {
        return false;
    }
    ;
    CNebulaTask *pNebulaTask = GetNebulaTask(nTaskId);
    if (pNebulaTask)
    {
        return pNebulaTask->IsExistRouteGroup(nGroupId);
    }
    return false;
}

bool ITaskManageImpl::GetNebulaRouteGroupName(unsigned int nTaskId, unsigned int nGroupId, std::string &strGroupName)
{
    if (NEBULATASK_ID_INVAILD == nTaskId || NEBULAROUTEGROUP_ID_INVAILD == nGroupId)
    {
        return false;
    }
    ;
    CNebulaTask *pNebulaTask = GetNebulaTask(nTaskId);
    if (pNebulaTask)
    {
        return pNebulaTask->GetRouteGroupName(nGroupId, strGroupName);
    }
    return false;
}

bool ITaskManageImpl::SetNebulaRouteGroupName(unsigned int nTaskId, unsigned int nGroupId, const char *strGroupName)
{
    if (NEBULATASK_ID_INVAILD == nTaskId || NEBULAROUTEGROUP_ID_INVAILD == nGroupId)
    {
        return false;
    }
    ;
    CNebulaTask *pNebulaTask = GetNebulaTask(nTaskId);
    if (pNebulaTask)
    {
        return pNebulaTask->SetRouteGroupName(nGroupId, strGroupName);
    }
    return false;
}

bool ITaskManageImpl::GetNebulaRouteGroupID(unsigned int nTaskId, const char *strGroupName, unsigned int &nGroupId)
{
    if (NEBULATASK_ID_INVAILD == nTaskId || NULL == strGroupName)
    {
        return false;
    }
    ;
    CNebulaTask *pNebulaTask = GetNebulaTask(nTaskId);
    if (pNebulaTask)
    {
        return pNebulaTask->GetRouteGroupID(strGroupName, nGroupId);
    }
    return false;
}

bool ITaskManageImpl::RemoveNebulaRouteGroup(unsigned int nTaskId, unsigned int nGroupId)
{
    if (NEBULATASK_ID_INVAILD == nTaskId || NEBULAROUTEGROUP_ID_INVAILD == nGroupId)
    {
        return false;
    }
    ;
    CNebulaTask *pNebulaTask = GetNebulaTask(nTaskId);
    if (pNebulaTask)
    {
        return pNebulaTask->RemoveRouteGroup(nGroupId);
    }
    return false;
}

bool ITaskManageImpl::GetNebulaRouteGroupFormMode(unsigned int nTaskId, unsigned int nGroupId, CNebulaRouteGroup::EFormMode &eMode, unsigned int &nFormId)
{
    if (NEBULATASK_ID_INVAILD == nTaskId || NEBULAROUTEGROUP_ID_INVAILD == nGroupId)
    {
        return false;
    }
    ;
    CNebulaTask *pNebulaTask = GetNebulaTask(nTaskId);
    if (pNebulaTask)
    {
        return pNebulaTask->GetRouteGroupForm(nGroupId, eMode, nFormId);
    }
    return false;
}

bool ITaskManageImpl::SetNebulaRouteGroupFormMode(unsigned int nTaskId, unsigned int nGroupId, CNebulaRouteGroup::EFormMode eMode, unsigned int nFormId)
{
    if (NEBULATASK_ID_INVAILD == nTaskId || NEBULAROUTEGROUP_ID_INVAILD == nGroupId)
    {
        return false;
    }
    ;
    CNebulaTask *pNebulaTask = GetNebulaTask(nTaskId);
    if (pNebulaTask)
    {
        return pNebulaTask->SetRouteGroupForm(nGroupId, eMode, nFormId);
    }
    return false;
}

bool ITaskManageImpl::GetNebulaWeaponIDList(unsigned int nTaskId, unsigned int nGroupId, LPStringVector lpVector)
{
    if (NEBULATASK_ID_INVAILD == nTaskId || NEBULAROUTEGROUP_ID_INVAILD == nGroupId)
    {
        return false;
    }
    ;
    CNebulaTask *pNebulaTask = GetNebulaTask(nTaskId);
    CNebulaRouteGroup groupInfo;
    if (pNebulaTask && pNebulaTask->GetRouteGroup(nGroupId, &groupInfo))
    {
        return groupInfo.GetWeaponIDList(lpVector);
    }
    return false;
}

bool ITaskManageImpl::GetNebulaRouteLineIDList(unsigned int nTaskId, unsigned int nGroupId, CNebulaRouteLine::LPRouteLineIDList lpList)
{
    if (NEBULATASK_ID_INVAILD == nTaskId || NEBULAROUTEGROUP_ID_INVAILD == nGroupId)
    {
        return false;
    }
    ;
    CNebulaTask *pNebulaTask = GetNebulaTask(nTaskId);
    CNebulaRouteGroup groupInfo;
    if (pNebulaTask && pNebulaTask->GetRouteGroup(nGroupId, &groupInfo))
    {
        return groupInfo.GetRouteLineIDArray(lpList);
    }
    return false;
}

bool ITaskManageImpl::GetNebulaRouteLineNum(unsigned int nTaskId, unsigned int nGroupId, int &nLineNum)
{
    if (NEBULATASK_ID_INVAILD == nTaskId || NEBULAROUTEGROUP_ID_INVAILD == nGroupId)
    {
        return false;
    }
    ;
    CNebulaTask *pNebulaTask = GetNebulaTask(nTaskId);
    CNebulaRouteGroup groupInfo;
    if (pNebulaTask && pNebulaTask->GetRouteGroup(nGroupId, &groupInfo))
    {
        nLineNum = groupInfo.GetRouteLinesNum();
        return true;
    }
    return false;
}

bool ITaskManageImpl::IsExistNebulaRouteLine(unsigned int nTaskId, unsigned int nGroupId, unsigned int nRouteLine)
{
    if (NEBULATASK_ID_INVAILD == nTaskId || NEBULAROUTEGROUP_ID_INVAILD == nGroupId)
    {
        return false;
    }
    ;
    CNebulaTask *pNebulaTask = GetNebulaTask(nTaskId);
    CNebulaRouteGroup groupInfo;
    if (pNebulaTask && pNebulaTask->GetRouteGroup(nGroupId, &groupInfo))
    {
        return groupInfo.IsExistRouteLine(nRouteLine);
    }
    return false;
}

bool ITaskManageImpl::GetNebulaRouteLine(unsigned int nTaskId, unsigned int nGroupId, unsigned int nLineId, LPNebulaRouteLine lpLine)
{
    if (NEBULATASK_ID_INVAILD == nTaskId || NEBULAROUTEGROUP_ID_INVAILD == nGroupId)
    {
        return false;
    }
    ;
    CNebulaTask *pNebulaTask = GetNebulaTask(nTaskId);
    CNebulaRouteGroup groupInfo;
    if (pNebulaTask && pNebulaTask->GetRouteGroup(nGroupId, &groupInfo))
    {
        return groupInfo.GetRouteLine(nLineId, lpLine);
    }
    return false;
}

bool ITaskManageImpl::RemoveNebulaRouteLine(unsigned int nTaskId, unsigned int nGroupId, unsigned int nLineId)
{
    if (NEBULATASK_ID_INVAILD == nTaskId || NEBULAROUTEGROUP_ID_INVAILD == nGroupId)
    {
        return false;
    }
    ;
    CNebulaTask *pNebulaTask = GetNebulaTask(nTaskId);
    if (pNebulaTask)
    {
        return pNebulaTask->RemoveNebulaRouteLine(nGroupId, nLineId);
    }
    return false;
}

bool ITaskManageImpl::SetNebulaRouteLine(unsigned int nTaskId, unsigned int nGroupId, const CNebulaRouteLine &data)
{
    if (NEBULATASK_ID_INVAILD == nTaskId || NEBULAROUTEGROUP_ID_INVAILD == nGroupId)
    {
        return false;
    }
    ;
    CNebulaTask *pNebulaTask = GetNebulaTask(nTaskId);
    if (pNebulaTask)
    {
        return pNebulaTask->SetNebulaRouteLine(nGroupId, data);
    }
    return false;
}

bool ITaskManageImpl::AddNebulaRouteLine(unsigned int nTaskId, unsigned int nGroupId, const CNebulaRouteLine &data)
{
    if (NEBULATASK_ID_INVAILD == nTaskId || NEBULAROUTEGROUP_ID_INVAILD == nGroupId)
    {
        return false;
    }
    ;
    CNebulaTask *pNebulaTask = GetNebulaTask(nTaskId);
    if (pNebulaTask)
    {
        return pNebulaTask->AddNebulaRouteLine(nGroupId, data);
    }
    return false;
}

bool ITaskManageImpl::GetNebulaRouteLineTail(unsigned int nTaskId, unsigned int nGroupId, LPNebulaRouteLine data)
{
    if (NEBULATASK_ID_INVAILD == nTaskId || NEBULAROUTEGROUP_ID_INVAILD == nGroupId)
    {
        return false;
    }
    ;
    CNebulaTask *pNebulaTask = GetNebulaTask(nTaskId);
    if (pNebulaTask)
    {
        return pNebulaTask->GetNebulaRouteLineTail(nGroupId, data);
    }
    return false;
}

bool ITaskManageImpl::GetNebulaRouteLineHead(unsigned int nTaskId, unsigned int nGroupId, LPNebulaRouteLine data)
{
    if (NEBULATASK_ID_INVAILD == nTaskId || NEBULAROUTEGROUP_ID_INVAILD == nGroupId)
    {
        return false;
    }
    ;
    CNebulaTask *pNebulaTask = GetNebulaTask(nTaskId);
    if (pNebulaTask)
    {
        return pNebulaTask->GetNebulaRouteLineHead(nGroupId, data);
    }
    return false;
}

bool ITaskManageImpl::SwapNebulaRouteLinePosition(unsigned int nTaskId, unsigned int nGroupId, unsigned int nLineID_1, unsigned int nLineID_2)
{
    if (NEBULATASK_ID_INVAILD == nTaskId || NEBULAROUTEGROUP_ID_INVAILD == nGroupId)
    {
        return false;
    }
    ;
    CNebulaTask *pNebulaTask = GetNebulaTask(nTaskId);
    if (pNebulaTask)
    {
        return pNebulaTask->SwapNebulaRouteLinePosition(nGroupId, nLineID_1, nLineID_2);
    }
    return false;
}

bool ITaskManageImpl::GetNebulaWeaponID(unsigned int nTaskId, unsigned int nGroupId, unsigned int nLineId, std::string &strWeaponId)
{
    if (NEBULATASK_ID_INVAILD == nTaskId || NEBULAROUTEGROUP_ID_INVAILD == nGroupId)
    {
        return false;
    }
    ;
    CNebulaTask *pNebulaTask = GetNebulaTask(nTaskId);
    if (pNebulaTask)
    {
        CNebulaRouteGroup groupInfo;
        bool bGetValue = pNebulaTask->GetRouteGroup(nGroupId, &groupInfo);
        if (bGetValue)
        {
            return groupInfo.GetWeaponID(nLineId, strWeaponId);
        }
    }
    return false;
}

void ITaskManageImpl::SetFormUnitScaleLen(unsigned int nUnit)
{
    m_nFormScaleUnit = nUnit;
}

int ITaskManageImpl::GetFormUnitScaleLen()
{
    return m_nFormScaleUnit;
}

bool ITaskManageImpl::GetNebulaRoutePointIDList(unsigned int nTaskId, unsigned int nGroupId, unsigned int nLineId, CNebulaRoutePoint::LPRoutePointIDList lp)
{
    if (NEBULATASK_ID_INVAILD == nTaskId || NEBULAROUTEGROUP_ID_INVAILD == nGroupId || NEBULAROUTELINE_ID_INVAILD == nLineId)
    {
        return false;
    }
    lp->clear();
    ;
    CNebulaTask *pNebulaTask = GetNebulaTask(nTaskId);
    if (pNebulaTask)
    {
        CNebulaRouteGroup groupInfo;
        bool bGetValue = pNebulaTask->GetRouteGroup(nGroupId, &groupInfo);
        if (bGetValue)
        {
            CNebulaRouteLine lineInfo;
            bGetValue = groupInfo.GetRouteLine(nLineId, &lineInfo);
            if (bGetValue)
            {
                return lineInfo.GetRoutePointIDList(lp);
            }
        }
    }
    return false;
}

bool ITaskManageImpl::GetNebulaRoutePointNum(unsigned int nTaskId, unsigned int nGroupId, unsigned int nLineId, int &nPointNum)
{
    if (NEBULATASK_ID_INVAILD == nTaskId || NEBULAROUTEGROUP_ID_INVAILD == nGroupId || NEBULAROUTELINE_ID_INVAILD == nLineId)
    {
        return false;
    }
    ;
    CNebulaTask *pNebulaTask = GetNebulaTask(nTaskId);
    if (pNebulaTask)
    {
        CNebulaRouteGroup groupInfo;
        bool bGetValue = pNebulaTask->GetRouteGroup(nGroupId, &groupInfo);
        if (bGetValue)
        {
            CNebulaRouteLine lineInfo;
            bGetValue = groupInfo.GetRouteLine(nLineId, &lineInfo);
            if (bGetValue)
            {
                nPointNum = lineInfo.GetRoutePointNum();
                return true;
            }
        }
    }
    return false;
}

bool ITaskManageImpl::IsExistNebulaRoutePoint(unsigned int nTaskId, unsigned int nGroupId, unsigned int nLineId, unsigned int nPointId)
{
    if (NEBULATASK_ID_INVAILD == nTaskId || NEBULAROUTEGROUP_ID_INVAILD == nGroupId || NEBULAROUTELINE_ID_INVAILD == nLineId || 0 == nPointId)
    {
        return false;
    }
    ;
    CNebulaTask *pNebulaTask = GetNebulaTask(nTaskId);
    if (pNebulaTask)
    {
        CNebulaRouteGroup groupInfo;
        bool bGetValue = pNebulaTask->GetRouteGroup(nGroupId, &groupInfo);
        if (bGetValue)
        {
            CNebulaRouteLine lineInfo;
            bGetValue = groupInfo.GetRouteLine(nLineId, &lineInfo);
            if (bGetValue)
            {
                unsigned int nPointNum = lineInfo.GetRoutePointNum();
                if (nPointId > nPointNum)
                {
                    return false;
                }
                return true;
            }
        }
    }
    return false;
}

bool ITaskManageImpl::GetNebulaRoutePoint(unsigned int nTaskId, unsigned int nGroupId, unsigned int nLineId, unsigned int nPointId, LPNebulaRoutePoint lp)
{
    if (NEBULATASK_ID_INVAILD == nTaskId || NEBULAROUTEGROUP_ID_INVAILD == nGroupId || NEBULAROUTELINE_ID_INVAILD == nLineId || 0 == nPointId || !lp)
    {
        return false;
    }
    ;
    CNebulaTask *pNebulaTask = GetNebulaTask(nTaskId);
    if (pNebulaTask)
    {
        CNebulaRouteGroup groupInfo;
        bool bGetValue = pNebulaTask->GetRouteGroup(nGroupId, &groupInfo);
        if (bGetValue)
        {
            CNebulaRouteLine lineInfo;
            bGetValue = groupInfo.GetRouteLine(nLineId, &lineInfo);
            if (bGetValue)
            {
                const CNebulaRoutePoint *pPoint = lineInfo.GetRoutePoint(nPointId);
                if (pPoint)
                {
                    *lp = *pPoint;
                    return true;
                }
            }
        }
    }
    return false;
}

bool ITaskManageImpl::RemoveNebulaRoutePoint(unsigned int nTaskId, unsigned int nGroupId, unsigned int nLineId, unsigned int nPointId)
{
    if (NEBULATASK_ID_INVAILD == nTaskId || NEBULAROUTEGROUP_ID_INVAILD == nGroupId || NEBULAROUTELINE_ID_INVAILD == nLineId || 0 == nPointId)
    {
        return false;
    }
    ;
    CNebulaTask *pNebulaTask = GetNebulaTask(nTaskId);
    if (pNebulaTask)
    {
        return pNebulaTask->RemoveNebulaRoutePoint(nGroupId, nLineId, nPointId);
    }
    return false;
}

bool ITaskManageImpl::RemoveNebulaAllRoutePoint(unsigned int nTaskId, unsigned int nGroupId, unsigned int nLineId)
{
    if (NEBULATASK_ID_INVAILD == nTaskId || NEBULAROUTEGROUP_ID_INVAILD == nGroupId || NEBULAROUTELINE_ID_INVAILD == nLineId)
    {
        return false;
    }
    ;
    CNebulaTask *pNebulaTask = GetNebulaTask(nTaskId);
    if (pNebulaTask)
    {
        return pNebulaTask->RemoveNebulaAllRoutePoint(nGroupId, nLineId);
    }
    return false;
}

bool ITaskManageImpl::SetNebulaRoutePoint(unsigned int nTaskId, unsigned int nGroupId, unsigned int nLineId, unsigned int nPointId, const CNebulaRoutePoint &data)
{
    if (NEBULATASK_ID_INVAILD == nTaskId || NEBULAROUTEGROUP_ID_INVAILD == nGroupId || NEBULAROUTELINE_ID_INVAILD == nLineId || 0 == nPointId)
    {
        return false;
    }
    ;
    CNebulaTask *pNebulaTask = GetNebulaTask(nTaskId);
    if (pNebulaTask)
    {
        return pNebulaTask->SetNebulaRoutePoint(nGroupId, nLineId, nPointId, data);
    }
    return false;
}

bool ITaskManageImpl::AddNebulaRoutePoint(unsigned int nTaskId, unsigned int nGroupId, unsigned int nLineId, const CNebulaRoutePoint &data)
{
    if (NEBULATASK_ID_INVAILD == nTaskId || NEBULAROUTEGROUP_ID_INVAILD == nGroupId || NEBULAROUTELINE_ID_INVAILD == nLineId)
    {
        return false;
    }
    ;
    CNebulaTask *pNebulaTask = GetNebulaTask(nTaskId);
    if (pNebulaTask)
    {
        return pNebulaTask->AddNebulaRoutePoint(nGroupId, nLineId, data);
    }
    return false;
}

bool ITaskManageImpl::InsertNeublaRoutrPoint(unsigned int nTaskId, unsigned int nGroupId, unsigned int nLineId, unsigned int nPointId, const CNebulaRoutePoint &data)
{
    if (NEBULATASK_ID_INVAILD == nTaskId || NEBULAROUTEGROUP_ID_INVAILD == nGroupId || NEBULAROUTELINE_ID_INVAILD == nLineId || -1 == nPointId)
    {
        return false;
    }
    ;
    CNebulaTask *pNebulaTask = GetNebulaTask(nTaskId);
    if (pNebulaTask)
    {
        return pNebulaTask->InsertNebulaRoutePoint(nGroupId, nLineId, nPointId, data);
    }
    return false;
}

bool ITaskManageImpl::GetNebulaRoutePointTail(unsigned int nTaskId, unsigned int nGroupId, unsigned int nLineId, LPNebulaRoutePoint lpdata)
{
    if (NEBULATASK_ID_INVAILD == nTaskId || NEBULAROUTEGROUP_ID_INVAILD == nGroupId || NEBULAROUTELINE_ID_INVAILD == nLineId || !lpdata)
    {
        return false;
    }
    ;
    CNebulaTask *pNebulaTask = GetNebulaTask(nTaskId);
    if (pNebulaTask)
    {
        return pNebulaTask->GetNebulaRoutePointTail(nGroupId, nLineId, lpdata);
    }
    return false;
}

bool ITaskManageImpl::GetNebulaRoutePointHead(unsigned int nTaskId, unsigned int nGroupId, unsigned int nLineId, LPNebulaRoutePoint lpdata)
{
    if (NEBULATASK_ID_INVAILD == nTaskId || NEBULAROUTEGROUP_ID_INVAILD == nGroupId || NEBULAROUTELINE_ID_INVAILD == nLineId || !lpdata)
    {
        return false;
    }
    ;
    CNebulaTask *pNebulaTask = GetNebulaTask(nTaskId);
    if (pNebulaTask)
    {
        return pNebulaTask->GetNebulaRoutePointHead(nGroupId, nLineId, lpdata);
    }
    return false;
}

void ITaskManageImpl::InitDataBase()
{
    QSettings ini("../data/conf/ly.ini", QSettings::IniFormat);
    ini.beginGroup("Database");
    QString strDbName = ini.value("DbName", "LY").toString();
    QString strDbIP = ini.value("SvrIP", "192.168.1.115").toString();
    QString strDbUser = ini.value("User", "root").toString();
    QString strDbPass = ini.value("Password", "123456").toString();
    QString strDbDriver = ini.value("Driver", "QMYSQL").toString();

    db = QSqlDatabase::addDatabase(strDbDriver);
    db.setHostName(strDbIP);
    db.setPort(3306);
    db.setDatabaseName(strDbName);
    db.setUserName(strDbUser);
    db.setPassword(strDbPass);
    bool bresult = db.open();
}

void ITaskManageImpl::LoadDataBase()
{
    QMutexLocker locker(&sMutex);

    QSqlQuery query;

    bool success = query.exec("SELECT `RWBH`, `XDMC`, `RWBD`, `RWLX`, `ZCQBYQ`, `KSSJ`, `CHSJ`, `JSSJ`, `BZ`,`QBYXSJ` ,`QSJD` , `QSWD`, `QSGD` ,`ZXZT` ,`DBRWLX`, `DBRWLY` FROM `RWSJ_RWLB`");
    if (success)
    {
        m_NeubulaMissionVec.clear();

        while (query.next())
        {
			QString str1 = tr2(query.value(0).toString().toLocal8Bit().data());
			QString str2 = tr2(query.value(1).toString().toLocal8Bit().data());
			QString str3 = tr2(query.value(2).toString().toLocal8Bit().data());
			QString str4 = tr2(query.value(3).toString().toLatin1().data());

			QString str15 = tr2(query.value(4).toString().toLocal8Bit().data());

			QString str19 = tr2(query.value(5).toString().toLocal8Bit().data());
			QString str20 = tr2(query.value(6).toString().toLocal8Bit().data());
			QString str21 = tr2(query.value(7).toString().toLocal8Bit().data());
			QString str22 = tr2(query.value(8).toString().toLocal8Bit().data());
			QString str23 = tr2(query.value(9).toString().toLocal8Bit().data());
			QString str24 = tr2(query.value(10).toString().toLatin1().data());
			QString str25 = tr2(query.value(11).toString().toLatin1().data());
			QString str26 = tr2(query.value(12).toString().toLatin1().data());
			QString str27 = tr2(query.value(13).toString().toLatin1().data());
			QString str28 = tr2(query.value(14).toString().toLatin1().data());
			QString str29 = tr2(query.value(15).toString().toLocal8Bit().data());

            CNeubulaMission stMission;
            stMission.SetID(str1.toStdString());
            stMission.SetName(str2.toStdString());
            stMission.SetBelong(str3.toStdString());
            stMission.SetType(stMission.GetTypeENum(str4.toInt()));

            stMission.SetDetectDemand(str15.toStdString());

            stMission.SetStartTime(str19.toStdString());
            stMission.SetAttackTime(str20.toStdString());
            stMission.SetFinishTime(str21.toStdString());
            stMission.SetRemark(str22.toStdString());
            stMission.SetValidTime(str23.toStdString());
            stMission.SetStartLon(str24.toDouble());
            stMission.SetStartLat(str25.toDouble());
            stMission.SetStartAlt(str26.toDouble());
            stMission.SetState(stMission.GetStateENum(str27.toInt()));
            stMission.SetDBMissionType(str28.toInt());
            stMission.SetDBSource(str29.toStdString());

            m_NeubulaMissionVec.push_back(stMission);
        }
    }
}

bool ITaskManageImpl::EditDBData(CNeubulaMission &data)
{
    std::string strID = data.GetID();

    if (!IsIDExisted(strID))
    {
        return false;
    }

    QString strSql("UPDATE RWSJ_RWLB set ");
    strSql += "`XDMC` = '"			+ QString::fromStdString(data.GetName()) + "',";
    strSql += "`RWBD` = '"			+ QString::fromStdString(data.GetBelong()) + "',";
    strSql += "`RWLX` = "			+ QString::number(data.GetTypeENum(data.GetType())) + ",";

    strSql += "`ZCQBYQ` = '"		+ QString::fromStdString(data.GetrDetectDemand()) + "',";

    strSql += "`KSSJ` = '"			+ QString::fromStdString(data.GetStartTime()) + "',";
    strSql += "`CHSJ` = '"			+ QString::fromStdString(data.GetAttackTime()) + "',";
    strSql += "`JSSJ` = '"			+ QString::fromStdString(data.GetFinishTime()) + "',";
    strSql += "`BZ` = '"			+ QString::fromStdString(data.GetRemark()) + "',";
    strSql += "`QBYXSJ` = '"		+ QString::fromStdString(data.GetValidTime()) + "',";
    strSql += "`QSJD` = "			+ QString::number(data.GetStartLon(), 'f', 7) + ",";
    strSql += "`QSWD` = "			+ QString::number(data.GetStartLat(), 'f', 7) + ",";
    strSql += "`QSGD` = "			+ QString::number(data.GetStartAlt(), 'f', 2) + ",";
    strSql += "`ZXZT` = "			+ QString::number(data.GetStateENum(data.GetState())) + ",";
    strSql += "`DBRWLX` = "			+ QString::number(data.GetDBMissionType()) + ",";
    strSql += "`DBRWLY` = '"		+ QString::fromStdString(data.GetDBSource()) + "' ";
    strSql += "where `RWBH` = '"	+ QString::fromStdString(data.GetID()) + "'";
    QSqlQuery query;
    bool success = query.exec(strSql);
    if (success)
    {
        int len = sendSystemUpdate(ESystemUpdateType::ESystemUpdateType_Task, ESystemOperatorType::ESystemOperatorType_Modify, strID.data());
    }

    RemoveRelationMissionMarkDBData(strID);
    for (auto MarkID : data.GetMarkIDList())
    {
        CRelationMissionMark stRelation;
        stRelation.SetID(GetMaxRelationMissionMarkID());
        stRelation.SetMissionID(strID);
        stRelation.SetMarkID(MarkID);
        stRelation.SetMarkDamage(data.GetMarkDamage(MarkID));
        AddRelationMissionMarkDBData(stRelation);
    }

    RemoveRelationMissionPtDBData(strID);
    for (auto MissionPT : data.GetPlatInfoMap())
    {
        AddRelationMissionPtDBData(MissionPT);
    }

    return success;
}

bool ITaskManageImpl::EditDBData(CNeubulaMission &data, std::string &strOldID)
{
    QString strSql("UPDATE RWSJ_RWLB set ");
    strSql += "`RWBH` = '" + QString::fromStdString(data.GetID()) + "',";
    strSql += "`XDMC` = '" + QString::fromStdString(data.GetName()) + "',";
    strSql += "`RWBD` = '" + QString::fromStdString(data.GetBelong()) + "',";
    strSql += "`RWLX` = " + QString::number(data.GetTypeENum(data.GetType())) + ",";

    strSql += "`ZCQBYQ` = '" + QString::fromStdString(data.GetrDetectDemand()) + "',";

    strSql += "`KSSJ` = '" + QString::fromStdString(data.GetStartTime()) + "',";
    strSql += "`CHSJ` = '" + QString::fromStdString(data.GetAttackTime()) + "',";
    strSql += "`JSSJ` = '" + QString::fromStdString(data.GetFinishTime()) + "',";
    strSql += "`BZ` = '" + QString::fromStdString(data.GetRemark()) + "',";
    strSql += "`QBYXSJ` = '" + QString::fromStdString(data.GetValidTime()) + "',";
    strSql += "`QSJD` = " + QString::number(data.GetStartLon(), 'f', 7) + ",";
    strSql += "`QSWD` = " + QString::number(data.GetStartLat(), 'f', 7) + ",";
    strSql += "`QSGD` = " + QString::number(data.GetStartAlt(), 'f', 2) + ",";
    strSql += "`ZXZT` = " + QString::number(data.GetStateENum(data.GetState())) + ",";
    strSql += "`DBRWLX` = " + QString::number(data.GetDBMissionType()) + ",";
    strSql += "`DBRWLY` = '" + QString::fromStdString(data.GetDBSource()) + "' ";
    strSql += "where `RWBH` = '" + QString::fromStdString(strOldID) + "'";
    QSqlQuery query;
    bool success = query.exec(strSql);
    if (success)
    {
        int len = sendSystemUpdate(ESystemUpdateType::ESystemUpdateType_Task, ESystemOperatorType::ESystemOperatorType_Modify, strOldID.data());
    }

    RemoveRelationMissionMarkDBData(data.GetID());
    for (auto MarkID : data.GetMarkIDList())
    {
        CRelationMissionMark stRelation;
        stRelation.SetID(GetMaxRelationMissionMarkID());
        stRelation.SetMissionID(data.GetID());
        stRelation.SetMarkID(MarkID);
        stRelation.SetMarkDamage(data.GetMarkDamage(MarkID));
        AddRelationMissionMarkDBData(stRelation);
    }

    RemoveRelationMissionPtDBData(data.GetID());
    for (auto MissionPT : data.GetPlatInfoMap())
    {
        AddRelationMissionPtDBData(MissionPT);
    }

    return success;
}

bool ITaskManageImpl::AddDBData(CNeubulaMission &data)
{
    std::string strID = data.GetID();

    if (IsIDExisted(strID))
    {
        return false;
    }

    QString strSql("INSERT INTO RWSJ_RWLB (`RWBH`, `XDMC`, `RWBD`, `RWLX`, `ZCQBYQ`, `KSSJ`, `CHSJ`, `JSSJ`, `BZ`,`QBYXSJ`, `QSJD`, `QSWD`, `QSGD`,`ZXZT`,`DBRWLX`, `DBRWLY`) VALUES (");

    strSql += "'" + QString::fromStdString(data.GetID()) + "',";
    strSql += "'" + QString::fromStdString(data.GetName()) + "',";
    strSql += "'" + QString::fromStdString(data.GetBelong()) + "',";
    strSql +=		QString::number(data.GetTypeENum(data.GetType())) + ",";

    strSql += "'" + QString::fromStdString(data.GetrDetectDemand()) + "',";

    strSql += "'" + QString::fromStdString(data.GetStartTime()) + "',";
    strSql += "'" + QString::fromStdString(data.GetAttackTime()) + "',";
    strSql += "'" + QString::fromStdString(data.GetFinishTime()) + "',";
    strSql += "'" + QString::fromStdString(data.GetRemark()) + "',";
    strSql += "'" + QString::fromStdString(data.GetValidTime()) + "',";
    strSql +=		QString::number(data.GetStartLon(), 'f', 7) + ",";
    strSql +=		QString::number(data.GetStartLat(), 'f', 7) + ",";
    strSql +=		QString::number(data.GetStartAlt(), 'f', 2) + ",";
    strSql +=		QString::number(data.GetStateENum(data.GetState())) + ",";
    strSql +=		QString::number(data.GetDBMissionType()) + ",";
    strSql += "'" + QString::fromStdString(data.GetDBSource()) + "' ";
    strSql += ")";
    QSqlQuery query;
    bool success = query.exec(strSql);

    if (success)
    {
        int len = sendSystemUpdate(ESystemUpdateType::ESystemUpdateType_Task, ESystemOperatorType::ESystemOperatorType_Add, strID.data());
    }

    RemoveRelationMissionMarkDBData(strID);
    for (auto MarkID : data.GetMarkIDList())
    {
        CRelationMissionMark stRelation;
        stRelation.SetID(GetMaxRelationMissionMarkID());
        stRelation.SetMissionID(strID);
        stRelation.SetMarkID(MarkID);
        stRelation.SetMarkDamage(data.GetMarkDamage(MarkID));
        AddRelationMissionMarkDBData(stRelation);
    }

    RemoveRelationMissionPtDBData(strID);
    for (auto MissionPT : data.GetPlatInfoMap())
    {
        AddRelationMissionPtDBData(MissionPT);
    }

    return success;
}

bool ITaskManageImpl::RemoveDBData(std::string strID)
{

    if (!IsIDExisted(strID))
    {
        return false;
    }

    QString strSql("DELETE FROM RWSJ_RWLB where RWBH = '");
    strSql += QString::fromStdString(strID) + "'";
    QSqlQuery query;
    bool success = query.exec(strSql);
    if (success)
    {
        int len = sendSystemUpdate(ESystemUpdateType::ESystemUpdateType_Task, ESystemOperatorType::ESystemOperatorType_Delete, strID.data());
    }
    return success;

}

bool ITaskManageImpl::IsIDExisted(std::string strID)
{
    QString strSql("SELECT  * FROM `RWSJ_RWLB` WHERE `RWBH` = ");
    strSql += "'" + QString::fromStdString(strID) + "'";
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

void qnzkna::TaskManage::ITaskManageImpl::UpdataMissionMarkList()
{
    MarkIDVec IDVec;
    if (m_RelationMissionMarkVec.size() == 0)
    {
        return;
    }

    for (auto &data : m_NeubulaMissionVec)
    {
        CRelationMissionMarkVec MarkVec = GetRelationMissionMark(data.GetID());
        MarkIDVec IDList;
        for (auto itr : MarkVec)
        {
            IDList.push_back(itr.GetMarkID());
        }
        data.SetMarkIDList(IDList);
    }
}

void qnzkna::TaskManage::ITaskManageImpl::UpdataMissionMarkDamage()
{
    if (m_RelationMissionMarkVec.size() == 0)
    {
        return;
    }

    for (auto &data : m_NeubulaMissionVec)
    {
        CRelationMissionMarkVec MarkVec = GetRelationMissionMark(data.GetID());
        data.SetMarkDamageVec(MarkVec);
    }
}

void qnzkna::TaskManage::ITaskManageImpl::UpdataMissionAmmoVec()
{
    MarkIDVec IDVec;
    if (m_NebulaMissionAmmoVec.size() == 0)
    {
        return;
    }

    for (auto &data : m_NeubulaMissionVec)
    {
        CRelationMissionAmmoVec MissionAmmoVec = GetMissionIDByMissionAmmo(data.GetID());
        data.SetMissionAmmoVec(MissionAmmoVec);
    }
}

void ITaskManageImpl::LoadPlatformDB()
{
    QMutexLocker locker(&sMutex2);

    QSqlQuery query;

    QString strSql = "SELECT `PTBH`, `JX`, `RWBH`, `MBID1`, `MBMC1`, `MBHS1`, `MBID2`, `MBMC2`, `MBHS2`";
    strSql += ", `DY1`, `DY2`, `DXID` , `DXNID` , `HXID`, `CZY`, `YW`, `LX`,`BZ`";
    strSql += ", `PTMC`, `ZXSD`, `ZWSD` , `FHZB` , `WQGJLX`, `GDSL`, `SYXUSJ`";
    strSql += " FROM `RWSJ_WQPTLB`";
    bool success = query.exec(strSql);
    if (success)
    {
        m_MissionPlatformVec.clear();

        while (query.next())
        {
			QString str1 = tr2(query.value(0).toString().toLocal8Bit().data());
			QString str2 = tr2(query.value(1).toString().toLocal8Bit().data());
			QString str3 = tr2(query.value(2).toString().toLocal8Bit().data());
			QString str4 = tr2(query.value(3).toString().toLocal8Bit().data());
			QString str5 = tr2(query.value(4).toString().toLocal8Bit().data());
			QString str6 = tr2(query.value(5).toString().toLatin1().data());
			QString str7 = tr2(query.value(6).toString().toLocal8Bit().data());
			QString str8 = tr2(query.value(7).toString().toLocal8Bit().data());
			QString str9 = tr2(query.value(8).toString().toLatin1().data());
			QString str10 = tr2(query.value(9).toString().toLocal8Bit().data());
			QString str11 = tr2(query.value(10).toString().toLocal8Bit().data());
			QString str12 = tr2(query.value(11).toString().toLocal8Bit().data());
			QString str13 = tr2(query.value(12).toString().toLocal8Bit().data());
			QString str14 = tr2(query.value(13).toString().toLocal8Bit().data());
			QString str15 = tr2(query.value(14).toString().toLocal8Bit().data());
			QString str16 = tr2(query.value(15).toString().toLocal8Bit().data());
			QString str17 = tr2(query.value(16).toString().toLatin1().data());
			QString str18 = tr2(query.value(17).toString().toLocal8Bit().data());

			QString str19 = tr2(query.value(18).toString().toLocal8Bit().data());
			QString str20 = tr2(query.value(19).toString().toLocal8Bit().data());
			QString str21 = tr2(query.value(20).toString().toLocal8Bit().data());
			QString str22 = tr2(query.value(21).toString().toLocal8Bit().data());
			QString str23 = tr2(query.value(22).toString().toLocal8Bit().data());
			QString str24 = tr2(query.value(23).toString().toLocal8Bit().data());
			QString str25 = tr2(query.value(24).toString().toLocal8Bit().data());

			CNeubulaMission stMission;
			CNebulaMissionPlatform stPlatform;
            strncpy(stPlatform.strID, str1.toLocal8Bit(), MAX_PLATFORM_LEN_1);
            strncpy(stPlatform.strMode, str2.toLocal8Bit(), MAX_PLATFORM_LEN_1);
            strncpy(stPlatform.strMissionID, str3.toLocal8Bit(), MAX_PLATFORM_LEN_1);
            strncpy(stPlatform.strMarkID01, str4.toLocal8Bit(), MAX_PLATFORM_LEN_1);
            strncpy(stPlatform.strMarkName01, str5.toLocal8Bit(), MAX_PLATFORM_LEN_1);
            strncpy(stPlatform.strMarkID02, str7.toLocal8Bit(), MAX_PLATFORM_LEN_1);
            strncpy(stPlatform.strMarkName02, str8.toLocal8Bit(), MAX_PLATFORM_LEN_1);
            strncpy(stPlatform.strGZ01, str10.toLocal8Bit(), MAX_PLATFORM_LEN_1);
            strncpy(stPlatform.strGZ02, str11.toLocal8Bit(), MAX_PLATFORM_LEN_1);
            strncpy(stPlatform.strForm, str12.toLocal8Bit(), MAX_PLATFORM_LEN_1);
            strncpy(stPlatform.strFormID, str13.toLocal8Bit(), MAX_PLATFORM_LEN_1);
            strncpy(stPlatform.strLineID, str14.toLocal8Bit(), MAX_PLATFORM_LEN_1);
            strncpy(stPlatform.strUser, str15.toLocal8Bit(), MAX_PLATFORM_LEN_1);
            strncpy(stPlatform.strSeat, str16.toLocal8Bit(), MAX_PLATFORM_LEN_1);
            strncpy(stPlatform.strRemark, str18.toLocal8Bit(), MAX_PLATFORM_LEN_1);
            strncpy(stPlatform.strName, str19.toLocal8Bit(), MAX_PLATFORM_LEN_1);
            strncpy(stPlatform.strLoadType, str23.toLocal8Bit(), MAX_PLATFORM_LEN_1);
            stPlatform.EMarkHurm01 = stMission.GetHurtDegreeENum(str6.toInt());
            stPlatform.EMarkHurm02 = stMission.GetHurtDegreeENum(str9.toInt());
			stPlatform.EType = stPlatform.GetPlatformTypeENum(str17.toInt());
			stPlatform.dVelocityp = str20.toDouble();
			stPlatform.dTurnSpeed = str21.toDouble();
			stPlatform.dprotectnorm = str22.toDouble();
			stPlatform.nLoadNum = str24.toDouble();
			stPlatform.nFlyTime = str25.toDouble();

			m_MissionPlatformVec.push_back(stPlatform);
        }
    }
}

bool ITaskManageImpl::EditPlatformDBData(CNebulaMissionPlatform &data)
{
    std::string strID = data.strID;

    if (!IsPlatformIDExisted(strID))
    {
        return false;
    }
    CNeubulaMission stMission;

    QString strSql("UPDATE RWSJ_WQPTLB set ");
    strSql += "`JX` = '" +			tr2(data.strMode) + "',";
    strSql += "`RWBH` = '" +		tr2(data.strMissionID) + "',";
    strSql += "`MBID1` = '" +		tr2(data.strMarkID01) + "',";
    strSql += "`MBMC1` = '" +		tr2(data.strMarkName01) + "',";
    strSql += "`MBHS1` = " +		QString::number(stMission.GetHurtDegreeENum(data.EMarkHurm01)) + ",";
    strSql += "`MBID2` = '" +		tr2(data.strMarkID02) + "',";
    strSql += "`MBMC2` = '" +		tr2(data.strMarkName02) + "',";
    strSql += "`MBHS2` = " +		QString::number(stMission.GetHurtDegreeENum(data.EMarkHurm02)) + ",";
    strSql += "`DY1` = '" +			tr2(data.strGZ01) + "',";
    strSql += "`DY2` = '" +			tr2(data.strGZ02) + "',";
    strSql += "`DXID` = '" +		tr2(data.strForm) + "',";
    strSql += "`DXNID` = '" +		tr2(data.strFormID) + "',";
    strSql += "`HXID` = '" +		tr2(data.strLineID) + "',";
    strSql += "`CZY` = '" +			tr2(data.strUser) + "',";
    strSql += "`YW` = '" +			tr2(data.strSeat) + "',";
	strSql += "`LX` = " +			QString::number(data.GetPlatformTypeENum(data.EType)) + ",";
    strSql += "`BZ` = '" +			tr2(data.strRemark) + "',";

    strSql += "`PTMC` = '" +		tr2(data.strName) + "',";
	strSql += "`ZXSD` = " +			QString::number(data.dVelocityp, 'f', 2) + ",";
	strSql += "`ZWSD` = " +			QString::number(data.dTurnSpeed, 'f', 2) + ",";
	strSql += "`FHZB` = " +			QString::number(data.dprotectnorm, 'f', 2) + ",";
    strSql += "`WQGJLX` = '" +		tr2(data.strLoadType) + "',";
	strSql += "`GDSL` = " +			QString::number(data.nLoadNum, 'f', 2);
	strSql += "`SYXUSJ` = " +		QString::number(data.nFlyTime);

    strSql += "where `PTBH` = '" +	tr2(data.strID) + "'";
    QSqlQuery query;
    bool success = query.exec(strSql);
    return success;
}

bool ITaskManageImpl::AddPlatformDBData(CNebulaMissionPlatform &data)
{
    std::string strID = data.strID;

    if (IsPlatformIDExisted(strID))
    {
        return false;
    }
    CNeubulaMission stMission;

    QString strSql("INSERT INTO RWSJ_WQPTLB (`PTBH`, `JX`, `RWBH`,`MBID1`, `MBMC1`, `MBHS1`, `MBID2`, `MBMC2`, `MBHS2`, `DY1`, `DY2`, `DXID` , `DXNID` , `HXID`,`CZY`, `YW`, `LX`, `BZ`, `PTMC`, `ZXSD`, `ZWSD` , `FHZB` , `WQGJLX`, `GDSL`, `SYXUSJ`) VALUES (");
    strSql += "'" + tr2(data.strID) + "',";
    strSql += "'" + tr2(data.strMode) + "',";
    strSql += "'" + tr2(data.strMissionID) + "',";
    strSql += "'" + tr2(data.strMarkID01) + "',";
    strSql += "'" + tr2(data.strMarkName01) + "',";
    strSql +=		QString::number(stMission.GetHurtDegreeENum(data.EMarkHurm01)) + ",";
    strSql += "'" + tr2(data.strMarkID02) + "',";
    strSql += "'" + tr2(data.strMarkName02) + "',";
    strSql +=		QString::number(stMission.GetHurtDegreeENum(data.EMarkHurm02)) + ",";
    strSql += "'" + tr2(data.strGZ01) + "',";
    strSql += "'" + tr2(data.strGZ01) + "',";
    strSql += "'" + tr2(data.strForm) + "',";
    strSql += "'" + tr2(data.strFormID) + "',";
    strSql += "'" + tr2(data.strLineID) + "',";
    strSql += "'" + tr2(data.strUser) + "',";
    strSql += "'" + tr2(data.strSeat) + "',";
	strSql +=		QString::number(data.GetPlatformTypeENum(data.EType)) + ",";
    strSql += "'" + tr2(data.strRemark) + "',";
    strSql += "'" + tr2(data.strName) + "',";
	strSql +=		QString::number(data.dVelocityp, 'f', 2) + ",";
	strSql +=		QString::number(data.dTurnSpeed, 'f', 2) + ",";
	strSql +=		QString::number(data.dprotectnorm, 'f', 2) + ",";
    strSql += "'" + tr2(data.strLoadType) + "',";
	strSql +=		QString::number(data.nLoadNum, 'f', 2) + "',";
	strSql +=		QString::number(data.nFlyTime, 'f', 2);
    strSql += ")";
    QSqlQuery query;
    bool success = query.exec(strSql);
    return success;
}

bool ITaskManageImpl::RemovePlatformDBData(std::string strID)
{

    if (!IsPlatformIDExisted(strID))
    {
        return false;
    }

    QString strSql("DELETE FROM RWSJ_WQPTLB where PTBH = '");
    strSql += QString::fromStdString(strID) + "'";
    QSqlQuery query;
    bool success = query.exec(strSql);
    return success;
}

bool ITaskManageImpl::IsPlatformIDExisted(std::string strID)
{
    QString strSql("SELECT  * FROM `RWSJ_WQPTLB` WHERE `PTBH` = ");
    strSql += "'" + QString::fromStdString(strID) + "'";
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
bool ITaskManageImpl::LoadPodNatureDB()
{
    QMutexLocker locker(&sRsMutex7);

    QSqlQuery query;
    QString strSql = "SELECT `BH`, `DCLX`, `DCXH`, `DCMC`, `GJDM`, `JZ`, `ZDTCJL`, `ZDFWJD`, `FYZDJ`, `FYZXJ`, `ZDJSD`, `ZDJJSD` ";
    strSql += ", `SZWDJD` , `JFBL`, `JWZJD`, `GXBJBS`, `GJSCJ`, `ZYSCJ`, `ZXGZD`, `CHF`, `DFH`, `JJ`, `KDT`,`TW`, `YS`";
    strSql += ", `CJDTFW` , `SPXSGS`, `XSJJ`, `BCFW`";
    strSql += ", `QZP` , `KGZPT1`, `KGZPT2`";
    strSql += " FROM `JCXX_DC`";

    bool success = query.exec(strSql);
    if (!success)
    {
        return false;
    }

    m_NebulaPodNatureVec.clear();

    while (query.next())
    {
		QString str1 = tr2(query.value(0).toString().toLatin1().data());
		QString str2 = tr2(query.value(1).toString().toLatin1().data());
		QString str3 = tr2(query.value(2).toString().toLatin1().data());
		QString str4 = tr2(query.value(3).toString().toLocal8Bit().data());
		QString str5 = tr2(query.value(4).toString().toLatin1().data());
		QString str6 = tr2(query.value(5).toString().toLatin1().data());
		QString str7 = tr2(query.value(6).toString().toLatin1().data());
		QString str8 = tr2(query.value(7).toString().toLatin1().data());
		QString str9 = tr2(query.value(8).toString().toLatin1().data());
		QString str10 = tr2(query.value(9).toString().toLatin1().data());
		QString str11 = tr2(query.value(10).toString().toLatin1().data());
		QString str12 = tr2(query.value(11).toString().toLatin1().data());
		QString str13 = tr2(query.value(12).toString().toLatin1().data());
		QString str14 = tr2(query.value(13).toString().toLatin1().data());
		QString str15 = tr2(query.value(14).toString().toLatin1().data());
		QString str16 = tr2(query.value(15).toString().toLatin1().data());
		QString str17 = tr2(query.value(16).toString().toLatin1().data());
		QString str18 = tr2(query.value(17).toString().toLatin1().data());
		QString str19 = tr2(query.value(18).toString().toLatin1().data());
		QString str20 = tr2(query.value(19).toString().toLatin1().data());
		QString str21 = tr2(query.value(20).toString().toLatin1().data());
		QString str22 = tr2(query.value(21).toString().toLatin1().data());
		QString str23 = tr2(query.value(22).toString().toLatin1().data());
		QString str24 = tr2(query.value(23).toString().toLatin1().data());
		QString str25 = tr2(query.value(24).toString().toLatin1().data());
		QString str26 = tr2(query.value(25).toString().toLatin1().data());
		QString str27 = tr2(query.value(26).toString().toLatin1().data());
		QString str28 = tr2(query.value(27).toString().toLatin1().data());
		QString str29 = tr2(query.value(28).toString().toLatin1().data());
		QString str30 = tr2(query.value(29).toString().toLatin1().data());
		QString str31 = tr2(query.value(30).toString().toLocal8Bit().data());
		QString str32 = tr2(query.value(31).toString().toLocal8Bit().data());
		CNebulaPodNature info;
		info.nRemark = str1.toInt();
		info.nPodType = str2.toInt();
		info.nPodVersion = str3.toInt();
        strncpy(info.strPodName, str4.toLocal8Bit(), MAX_RESOURCES_LEN_1);
		info.nCountryNode = str5.toInt();
		info.fNetWeight = str6.toFloat();
		info.dMaxProbeDis = str7.toDouble();
		info.dMaxPosition = str8.toDouble();
		info.dMaxPitchingAngle = str9.toDouble();
		info.dMinPitchingAngle = str10.toInt();
		info.dMaxAngleV =str11.toDouble();
		info.dMaxAngleAcceleV = str12.toDouble();
		info.dSteadyPrecision = str13.toDouble();
		info.dAngleResolution = str14.toFloat();
		info.dAnglePostion = str15.toInt();
		info.sZoomRatio = str16.toShort();
		info.fWideAngleView = str17.toFloat();
		info.fFarthestAngle = str18.toFloat();
		info.dMinIlluminometer = str19.toDouble();

		info.dLongFormat = str20.toDouble();
		info.dShortFormat = str21.toDouble();
		info.dFocalLength = str22.toDouble();
		info.sWideDynamic = str23.toShort();
		info.sLucency = str24.toShort();
		info.sNightView = str25.toShort();
		info.sSceneDynamicScope = str26.toShort();
		info.sVideoDisplayForm = str27.toShort();
		info.fPixelPitch = str28.toFloat();
		info.sWavelengthCover = str29.toShort();
		info.nFullFrameFreq = str30.toInt();
        strncpy(info.strLoadPod_1, str31.toLocal8Bit(), 8);
        strncpy(info.strLoadPod_2, str32.toLocal8Bit(), 8);
        m_NebulaPodNatureVec.push_back(info);
    }
    return true;
}

CNebulaPodNatureVec ITaskManageImpl::GetPodNatureData()
{
    if (m_NebulaPodNatureVec.empty())
    {
        LoadPodNatureDB();
    }
    return m_NebulaPodNatureVec;
}

bool ITaskManageImpl::LoadPlatNatureDB()
{

    QMutexLocker locker(&sReMutex8);

    QSqlQuery query;
    QString strSql = "SELECT `BH`, `PTLX`, `PTXH`, `PTMC`, `PTCD`, `PTKD`, `PTGD`, `JSCL`, `FJJZ`, `ZDFZ`, `EDFZ`, `XHSJ` ";
    strSql += ", `ZDSX` , `ZDHC`, `ZZBJ`, `ZDPFSD`, `ZWSD`, `PSSD`, `FHZB`, `XHSD`, `XHGD`, `KFNL`, `QJFS`, `FSMJ`,`DTFW`, `DTPL`";
    strSql += ", `DTDK` , `DTKGR`, `GZWDXX`, `GZWDSX`";
    strSql += ", `WQGJLX` , `GDSL`, `DLLX`, `ZDZYL` ,`BFYL`,`ZDZDC`,`BCDC`,`ZDQFTJ`,`ZYRW`,`TBBM`,`TPBM`,`SWMXBM`,`BZ`";
    strSql += " FROM `JCXX_PT`";

    bool success = query.exec(strSql);
    if (!success)
	{
		qDebug() << "Insert failed. Error:" << query.lastError().text();

        return false;
    }

    m_NebulaPlatNatureVec.clear();
    while (query.next())
    {
		QString str1 = tr2(query.value(0).toString().toLatin1().data());
		QString str2 = tr2(query.value(1).toString().toLatin1().data());
		QString str3 = tr2(query.value(2).toString().toLatin1().data());
		QString str4 = tr2(query.value(3).toString().toLocal8Bit().data());
		QString str5 = tr2(query.value(4).toString().toLatin1().data());
		QString str6 = tr2(query.value(5).toString().toLatin1().data());
		QString str7 = tr2(query.value(6).toString().toLatin1().data());
		QString str8 = tr2(query.value(7).toString().toLatin1().data());
		QString str9 = tr2(query.value(8).toString().toLatin1().data());
		QString str10 = tr2(query.value(9).toString().toLatin1().data());
		QString str11 = tr2(query.value(10).toString().toLatin1().data());
		QString str12 = tr2(query.value(11).toString().toLatin1().data());
		QString str13 = tr2(query.value(12).toString().toLatin1().data());
		QString str14 = tr2(query.value(13).toString().toLatin1().data());
		QString str15 = tr2(query.value(14).toString().toLatin1().data());
		QString str16 = tr2(query.value(15).toString().toLatin1().data());
		QString str17 = tr2(query.value(16).toString().toLatin1().data());
		QString str18 = tr2(query.value(17).toString().toLatin1().data());
		QString str19 = tr2(query.value(18).toString().toLatin1().data());
		QString str20 = tr2(query.value(19).toString().toLatin1().data());
		QString str21 = tr2(query.value(20).toString().toLatin1().data());
		QString str22 = tr2(query.value(21).toString().toLatin1().data());
		QString str23 = tr2(query.value(22).toString().toLatin1().data());
		QString str24 = tr2(query.value(23).toString().toLatin1().data());
		QString str25 = tr2(query.value(24).toString().toLatin1().data());
		QString str26 = tr2(query.value(25).toString().toLatin1().data());
		QString str27 = tr2(query.value(26).toString().toLatin1().data());
		QString str28 = tr2(query.value(27).toString().toLatin1().data());
		QString str29 = tr2(query.value(28).toString().toLatin1().data());
		QString str30 = tr2(query.value(29).toString().toLatin1().data());
		QString str31 = tr2(query.value(30).toString().toLocal8Bit().data());
		QString str32 = tr2(query.value(31).toString().toLatin1().data());
		QString str33 = tr2(query.value(32).toString().toLatin1().data());
		QString str34 = tr2(query.value(33).toString().toLatin1().data());
		QString str35 = tr2(query.value(34).toString().toLatin1().data());
		QString str36 = tr2(query.value(35).toString().toLatin1().data());
		QString str37 = tr2(query.value(36).toString().toLatin1().data());
		QString str38 = tr2(query.value(37).toString().toLocal8Bit().data());
		QString str39 = tr2(query.value(38).toString().toLocal8Bit().data());
		QString str40 = tr2(query.value(39).toString().toLocal8Bit().data());
		QString str41 = tr2(query.value(40).toString().toLocal8Bit().data());
		QString str42 = tr2(query.value(41).toString().toLocal8Bit().data());
		QString str43 = tr2(query.value(42).toString().toLocal8Bit().data());

		CNebulaPlatNature info;
		info.nNum = str1.toInt();
		info.nType = str2.toInt();
		info.nModel = str3.toInt();
		info.nPlatLength = str5.toDouble();
		info.nPlatWidth = str6.toDouble();
		info.nPlatHeight = str7.toDouble();
		info.sMaterial = str8.toShort();
		info.nNetWeight = str9.toDouble();
		info.nMaxLoad = str10.toDouble();
		info.nLimitLoad = str11.toDouble();
		info.nEnduranceTime = str12.toInt();
		info.nMaxRise = str13.toDouble();
		info.nMaxVoyage = str14.toDouble();
		info.nFightRadius = str15.toDouble();
		info.nMaxFlySpeed = str16.toDouble();
		info.nWheelSpeed = str17.toDouble();
		info.nClimbSpeed = str18.toDouble();
		info.nEntrenchNorm = str19.toDouble();
		info.nCruiseSpeed = str20.toDouble();
		info.nCruiseHeight = str21.toInt();
		info.sResistPower = str22.toShort();
		info.sTakeAndLand = str23.toShort();
		info.nReflexFunc = str24.toDouble();
		info.nRadioDistance = str25.toDouble();
		info.nRadioRate = str26.toInt();
		info.nRadioBandwidth = str27.toInt();
		info.nResistDisturb = str28.toInt();
		info.nMinWorkTemperature = str29.toInt();
		info.nMaxWorkTemperature = str30.toInt();
		info.nMountpointCount = str32.toInt();
		info.sDynamicType = str33.toShort();
		info.nMaxOilNum = str34.toDouble();
		info.nBackupsOilNum = str35.toDouble();
		info.nMaxLoadBattery = str36.toDouble();
		info.nBackupsBattery = str37.toDouble();
        strncpy(info.strPlatName, str4.toLocal8Bit(), MAX_RESOURCES_LEN_1);
        strncpy(info.strPylonsType, str31.toLocal8Bit(), MAX_MISSION_LEN_1);
        strncpy(info.strMinTakeoff, str38.toLocal8Bit(), MAX_MISSION_LEN_3);
        strncpy(info.strMainTask, str39.toLocal8Bit(), MAX_MISSION_LEN_3);
        strncpy(info.strIcoCode, str40.toLocal8Bit(), MAX_MISSION_LEN_3);
        strncpy(info.strPictureCode, str41.toLocal8Bit(), MAX_MISSION_LEN_3);
        strncpy(info.strModeCode, str42.toLocal8Bit(), MAX_MISSION_LEN_3);
        strncpy(info.strRemark, str43.toLocal8Bit(), MAX_MISSION_LEN_3);
        m_NebulaPlatNatureVec.push_back(info);
    }
    return true;
}
CNebulaPlatNatureVec ITaskManageImpl::GetPlatNatureData()
{
    if (m_NebulaPlatNatureVec.empty())
    {
        LoadPlatNatureDB();
    }
    return m_NebulaPlatNatureVec;
}
bool ITaskManageImpl::LoadAmmoNatureDB()
{
    QMutexLocker locker(&sReMutex9);

    QSqlQuery query;
    QString strSql = "SELECT `BH`, `DYXH`, `DYLX`, `DYMC`, `SHMB`, `GJDM`, `DTZL`, `ZDBZL`, `CSSD`, `ZDBXZ`, `YXLX`, `FAFS` ";
    strSql += ", `ZDTZ` , `YDFF`, `SSBJ`, `MZJD`, `DFSSGL`, `SFSSGL`, `3FSSGL`, `DBZSJL`, `CLZSJL`, `DYDL`, `DC`,`DJ`, `BZXN`";
    strSql += ", `ZDGJJL` , `TBBM`, `TPBM`, `SWMXBM`,`BZ`";
    strSql += " FROM `JCXX_DY`";

    bool success = query.exec(strSql);
    if (!success)
    {
        return false;
    }

    m_NebulaAmmoNatureVec.clear();
    while (query.next())
    {
		QString str1 = tr2(query.value(0).toString().toLatin1().data());
		QString str2 = tr2(query.value(1).toString().toLatin1().data());
		QString str3 = tr2(query.value(2).toString().toLatin1().data());
		QString str4 = tr2(query.value(3).toString().toLocal8Bit().data());
		QString str5 = tr2(query.value(4).toString().toLocal8Bit().data());
		QString str6 = tr2(query.value(5).toString().toLatin1().data());
		QString str7 = tr2(query.value(6).toString().toLatin1().data());
		QString str8 = tr2(query.value(7).toString().toLatin1().data());
		QString str9 = tr2(query.value(8).toString().toLatin1().data());
		QString str10 = tr2(query.value(9).toString().toLocal8Bit().data());
		QString str11 = tr2(query.value(10).toString().toLatin1().data());
		QString str12 = tr2(query.value(11).toString().toLocal8Bit().data());
		QString str13 = tr2(query.value(12).toString().toLatin1().data());
		QString str14 = tr2(query.value(13).toString().toLocal8Bit().data());
		QString str15 = tr2(query.value(14).toString().toLatin1().data());
		QString str16 = tr2(query.value(15).toString().toLatin1().data());
		QString str17 = tr2(query.value(16).toString().toLatin1().data());
		QString str18 = tr2(query.value(17).toString().toLatin1().data());
		QString str19 = tr2(query.value(18).toString().toLatin1().data());
		QString str20 = tr2(query.value(19).toString().toLatin1().data());
		QString str21 = tr2(query.value(20).toString().toLatin1().data());
		QString str22 = tr2(query.value(21).toString().toLatin1().data());
		QString str23 = tr2(query.value(22).toString().toLatin1().data());
		QString str24 = tr2(query.value(23).toString().toLatin1().data());
		QString str25 = tr2(query.value(24).toString().toLatin1().data());
		QString str26 = tr2(query.value(25).toString().toLatin1().data());
		QString str27 = tr2(query.value(26).toString().toLocal8Bit().data());
		QString str28 = tr2(query.value(27).toString().toLocal8Bit().data());
		QString str29 = tr2(query.value(28).toString().toLocal8Bit().data());
		QString str30 = tr2(query.value(29).toString().toLocal8Bit().data());

		CNebulaAmmoNature info;
		info.nNum = str1.toInt();
		info.nAmmoModel = str2.toInt();
		info.nAmmoType = str3.toInt();
		info.nCountryNum = str6.toInt();
		info.nAmmoWeight = str7.toDouble();
		info.nFightWeight = str8.toDouble();
		info.nStartSpeed = str9.toDouble();
		info.strIgniteWay = str11.toShort();
		info.strGuidanceWay = str13.toShort();
		info.nHarmRadius = str15.toInt();
		info.nHitratePre = str16.toDouble();
		info.nOneHarmScope = str17.toFloat();
		info.nDoubleHarmScope = str18.toFloat();
		info.nthreeHarmScope = str19.toFloat();
		info.nOnlyarmsLaserDis = str20.toDouble();
		info.nCarLaserDis = str21.toDouble();
		info.nAmmoEquivalent = str22.toDouble();
		info.nAmmoLength = str23.toDouble();
		info.nAmmoRadius = str24.toDouble();
		info.strExplodePro = str25.toShort();
		info.nMaxAttackDis = str26.toDouble();
        strncpy(info.strRemark, str30.toLocal8Bit(), MAX_MISSION_LEN_3);
        strncpy(info.strIcoCode, str27.toLocal8Bit(), MAX_MISSION_LEN_3);
        strncpy(info.strAmmoName, str4.toLocal8Bit(), MAX_RESOURCES_LEN_1);
        strncpy(info.strGuideWay, str14.toLocal8Bit(), MAX_RESOURCES_LEN_1);
        strncpy(info.strModeCode, str29.toLocal8Bit(), MAX_MISSION_LEN_3);
        strncpy(info.strLaunchWay, str12.toLocal8Bit(), MAX_LAUNCHWAY_LEN);
        strncpy(info.strSuitMark, str5.toLocal8Bit(), MAX_RESOURCES_LEN_1);
        strncpy(info.strPictureCode, str28.toLocal8Bit(), MAX_MISSION_LEN_3);
        strncpy(info.strFightQuality, str10.toLocal8Bit(), MAX_RESOURCES_LEN_1);
		m_NebulaAmmoNatureVec.push_back(info);
	}
	return true;
}

CNebulaAmmoNatureVec ITaskManageImpl::GetAmmoNatureData()
{
    if (m_NebulaAmmoNatureVec.empty())
    {
        LoadAmmoNatureDB();
    }
    return m_NebulaAmmoNatureVec;
}

void ITaskManageImpl::LoadPlatformLoadDB()
{
    QMutexLocker locker(&sMutex2);

    QSqlQuery query;

    QString strSql = "SELECT `BH`, `XH`, `SL`, `PTBH`, `PTXH`, `RWBH`, `MBID1`, `MBMC1`, `MBHS1`, `MBID2`, `MBMC2`, `MBHS2` ";
    strSql += ", `TDDMC` , `TDSJ`, `TDFS`, `GZDMC`, `KSSJ`, `SMJD`, `SMSD`, `CZMS`,`LY`, `BZ`";
    strSql += ", `YXSC` , `SJJD`, `HSFW`, `DYDL`";
    strSql += ", `TXFW` , `GZPV`, `GZMS`, `TXDK`, `KGR`, `CHF`, `DHF`, `FOCAL`";
    strSql += " FROM `RWSJ_DYZHLB`";
    bool success = query.exec(strSql);
    if (success)
    {
        m_MissionPlatformLoadVec.clear();
        while (query.next())
        {
			QString str1 = tr2(query.value(0).toString().toLocal8Bit().data());
			QString str2 = tr2(query.value(1).toString().toLocal8Bit().data());
			QString str3 = tr2(query.value(2).toString().toLatin1().data());
			QString str4 = tr2(query.value(3).toString().toLocal8Bit().data());
			QString str5 = tr2(query.value(4).toString().toLocal8Bit().data());
			QString str6 = tr2(query.value(5).toString().toLocal8Bit().data());
			QString str7 = tr2(query.value(6).toString().toLocal8Bit().data());
			QString str8 = tr2(query.value(7).toString().toLocal8Bit().data());
			QString str9 = tr2(query.value(8).toString().toLatin1().data());
			QString str10 = tr2(query.value(9).toString().toLocal8Bit().data());
			QString str11 = tr2(query.value(10).toString().toLocal8Bit().data());
			QString str12 = tr2(query.value(11).toString().toLatin1().data());
			QString str13 = tr2(query.value(12).toString().toLocal8Bit().data());
			QString str14 = tr2(query.value(13).toString().toLocal8Bit().data());
			QString str15 = tr2(query.value(14).toString().toLatin1().data());
			QString str16 = tr2(query.value(15).toString().toLocal8Bit().data());
			QString str17 = tr2(query.value(16).toString().toLocal8Bit().data());
			QString str18 = tr2(query.value(17).toString().toLocal8Bit().data());
			QString str19 = tr2(query.value(18).toString().toLocal8Bit().data());
			QString str20 = tr2(query.value(19).toString().toLocal8Bit().data());
			QString str21 = tr2(query.value(20).toString().toLatin1().data());
			QString str22 = tr2(query.value(21).toString().toLocal8Bit().data());

			QString str23 = tr2(query.value(22).toString().toLatin1().data());
			QString str24 = tr2(query.value(23).toString().toLatin1().data());
			QString str25 = tr2(query.value(24).toString().toLatin1().data());
			QString str26 = tr2(query.value(25).toString().toLatin1().data());

			QString str27 = tr2(query.value(26).toString().toLatin1().data());
			QString str28 = tr2(query.value(27).toString().toLatin1().data());
			QString str29 = tr2(query.value(28).toString().toLatin1().data());
			QString str30 = tr2(query.value(29).toString().toLatin1().data());
			QString str31 = tr2(query.value(30).toString().toLatin1().data());

			QString str32 = tr2(query.value(31).toString().toLatin1().data());
			QString str33 = tr2(query.value(32).toString().toLatin1().data());
			QString str34 = tr2(query.value(33).toString().toLatin1().data());

			CNeubulaMission stMission;
			CNebulaMissionPlatformLoad stPlatformLoad;
			stPlatformLoad.nNum = str3.toInt();
            stPlatformLoad.EMarkHurm01 = stMission.GetHurtDegreeENum(str9.toInt());
            stPlatformLoad.EMarkHurm02 = stMission.GetHurtDegreeENum(str12.toInt());
			stPlatformLoad.ePeltMode = stPlatformLoad.GetPeltModeENum(str15.toInt());
			stPlatformLoad.eType = stPlatformLoad.GetTypeENum(str21.toInt());
            strncpy(stPlatformLoad.strID, str1.toLocal8Bit(), MAX_PLATFORM_LOAD_LEN_1);
            strncpy(stPlatformLoad.strMode, str2.toLocal8Bit(), MAX_PLATFORM_LOAD_LEN_1);
            strncpy(stPlatformLoad.strPlatformID, str4.toLocal8Bit(), MAX_PLATFORM_LOAD_LEN_1);
            strncpy(stPlatformLoad.strPlatformMode, str5.toLocal8Bit(), MAX_PLATFORM_LOAD_LEN_1);
            strncpy(stPlatformLoad.strMissionID, str6.toLocal8Bit(), MAX_PLATFORM_LOAD_LEN_1);
            strncpy(stPlatformLoad.strMarkID01, str7.toLocal8Bit(), MAX_PLATFORM_LOAD_LEN_1);
            strncpy(stPlatformLoad.strMarkName01, str8.toLocal8Bit(), MAX_PLATFORM_LOAD_LEN_1);
            strncpy(stPlatformLoad.strMarkID02, str10.toLocal8Bit(), MAX_PLATFORM_LOAD_LEN_1);
            strncpy(stPlatformLoad.strMarkName02, str11.toLocal8Bit(), MAX_PLATFORM_LOAD_LEN_1);
            strncpy(stPlatformLoad.strThrowPointName, str13.toLocal8Bit(), MAX_PLATFORM_LOAD_LEN_1);
            strncpy(stPlatformLoad.strThrowTime, str14.toLocal8Bit(), MAX_PLATFORM_LOAD_LEN_1);
            strncpy(stPlatformLoad.strWorkPointName, str16.toLocal8Bit(), MAX_PLATFORM_LOAD_LEN_1);
            strncpy(stPlatformLoad.strWorkTime, str17.toLocal8Bit(), MAX_PLATFORM_LOAD_LEN_1);
            strncpy(stPlatformLoad.strScanRange, str18.toLocal8Bit(), MAX_PLATFORM_LOAD_LEN_1);
            strncpy(stPlatformLoad.strScanSpeed, str19.toLocal8Bit(), MAX_PLATFORM_LOAD_LEN_1);
            strncpy(stPlatformLoad.streOperate, str20.toLocal8Bit(), MAX_PLATFORM_LOAD_LEN_1);
            strncpy(stPlatformLoad.strRemark, str22.toLocal8Bit(), MAX_PLATFORM_LOAD_LEN_1);

			stPlatformLoad.nFiringRange = str23.toInt();
			stPlatformLoad.dAttackprecision = str24.toDouble();
			stPlatformLoad.dHurtRange = str25.toDouble();
			stPlatformLoad.dHurtEquivalent = str26.toDouble();

			stPlatformLoad.dCommunicateRange = str27.toDouble();
			stPlatformLoad.nWorkFrequency = str28.toInt();
			stPlatformLoad.nWorkMode = str29.toInt();
			stPlatformLoad.nWorkBandWidth = str30.toInt();
			stPlatformLoad.nResistJam = str31.toInt();

			stPlatformLoad.dPic_Length = str32.toDouble();
			stPlatformLoad.dPic_width = str33.toDouble();
			stPlatformLoad.dFocal_dis = str34.toDouble();

			m_MissionPlatformLoadVec.push_back(stPlatformLoad);
        }
    }
}

bool ITaskManageImpl::EditPlatformLoadDBData(CNebulaMissionPlatformLoad &data)
{
    std::string strID = data.strID;

    if (!IsPlatformLoadIDExisted(strID))
    {
        return false;
    }
    CNeubulaMission stMission;

    QString strSql("UPDATE RWSJ_DYZHLB set ");
    strSql += "`XH` = '" +		tr2(data.strMode) + "',";
	strSql += "`SL` = " +		QString::number(data.nNum) + ",";
    strSql += "`PTBH` = '" +	tr2(data.strPlatformID) + "',";
    strSql += "`PTXH` = '" +	tr2(data.strPlatformMode) + "',";
    strSql += "`RWBH` = '" +	tr2(data.strMissionID) + "',";
    strSql += "`MBID1` = '" +	tr2(data.strMarkID01) + "',";
    strSql += "`MBMC1` = '" +	tr2(data.strMarkName01) + "',";
    strSql += "`MBHS1` = " +	QString::number(stMission.GetHurtDegreeENum(data.EMarkHurm01)) + ",";
    strSql += "`MBID2` = '" +	tr2(data.strMarkID02) + "',";
    strSql += "`MBMC2` = '" +	tr2(data.strMarkName02) + "',";
    strSql += "`MBHS2` = " +	QString::number(stMission.GetHurtDegreeENum(data.EMarkHurm02)) + ",";
    strSql += "`TDDMC` = '" +	tr2(data.strThrowPointName) + "',";
    strSql += "`TDSJ` = '" +	tr2(data.strThrowTime) + "',";
	strSql += "`TDFS` = " +		QString::number(data.GetPeltModeENum(data.ePeltMode)) + ",";
    strSql += "`GZDMC` = '" +	tr2(data.strWorkPointName) + "',";
    strSql += "`KSSJ` = '" +	tr2(data.strWorkTime) + "',";
    strSql += "`SMJD` = '" +	tr2(data.strScanRange) + "',";
    strSql += "`SMSD` = '" +	tr2(data.strScanSpeed) + "',";
    strSql += "`CZMS` = '" +	tr2(data.streOperate) + "',";
	strSql += "`LY` = "	+		QString::number(data.GetTypeENum(data.eType)) + ",";
    strSql += "`BZ` = '" +		tr2(data.strRemark) + "',";

	strSql += "`YXSC` = " +		QString::number(data.nFiringRange) + ",";
	strSql += "`SJJD` = " +		QString::number(data.dAttackprecision,'f',2) + ",";
	strSql += "`HSFW` = " +		QString::number(data.dHurtRange, 'f',2) + ",";
	strSql += "`DYDL` = " +		QString::number(data.dHurtEquivalent, 'f',2) + ",";

	strSql += "`TXFW` = " + QString::number(data.dCommunicateRange, 'f', 2) + ",";
	strSql += "`GZPV` = " + QString::number(data.nWorkFrequency) + ",";
	strSql += "`GZMS` = " + QString::number(data.nWorkMode) + ",";
	strSql += "`TXDK` = " + QString::number(data.nWorkBandWidth) + ",";
	strSql += "`KGR` = " + QString::number(data.nResistJam) + ",";
	strSql += "`CHF` = " + QString::number(data.dPic_Length) + ",";
	strSql += "`DHF` = " + QString::number(data.dPic_width) + ",";
	strSql += "`FOCAL` = " + QString::number(data.dFocal_dis);

    strSql += " where `BH` = '" + tr2(data.strID) + "'";
    QSqlQuery query;
    bool success = query.exec(strSql);
    return success;
}

bool ITaskManageImpl::AddPlatformLoadDBData(CNebulaMissionPlatformLoad &data)
{
    std::string strID = data.strID;

    if (IsPlatformLoadIDExisted(strID))
    {
        return false;
    }
    CNeubulaMission stMission;

    QString strSql("INSERT INTO RWSJ_DYZHLB (`BH`, `XH`, `SL`,`PTBH`,`PTXH`,`RWBH`,`MBID1`, `MBMC1`, `MBHS1`, `MBID2`, `MBMC2`, `MBHS2`, `TDDMC`, `TDSJ`, `TDFS` , `GZDMC` , `KSSJ`, `SMJD`,`SMSD`,`CZMS`,`LY`, `BZ`, `YXSC` , `SJJD`, `HSFW`, `DYDL`, `TXFW` , `GZPV`, `GZMS`, `TXDK`, `KGR`, `CHF`, `DHF`, `FOCAL`) VALUES (");
    strSql += "'" + tr2(data.strID) + "',";
    strSql += "'" + tr2(data.strMode) + "',";
	strSql +=		QString::number(data.nNum) + ",";
    strSql += "'" + tr2(data.strPlatformID) + "',";
    strSql += "'" + tr2(data.strPlatformMode) + "',";
    strSql += "'" + tr2(data.strMissionID) + "',";
    strSql += "'" + tr2(data.strMarkID01) + "',";
    strSql += "'" + tr2(data.strMarkName01) + "',";
    strSql +=		QString::number(stMission.GetHurtDegreeENum(data.EMarkHurm01)) + ",";
    strSql += "'" + tr2(data.strMarkID02) + "',";
    strSql += "'" + tr2(data.strMarkName02) + "',";
    strSql +=		QString::number(stMission.GetHurtDegreeENum(data.EMarkHurm02)) + ",";
    strSql += "'" + tr2(data.strThrowPointName) + "',";
    strSql += "'" + tr2(data.strThrowTime) + "',";
	strSql +=		QString::number(data.GetPeltModeENum(data.ePeltMode)) + ",";
    strSql += "'" + tr2(data.strWorkPointName) + "',";
    strSql += "'" + tr2(data.strWorkTime) + "',";
    strSql += "'" + tr2(data.strScanRange) + "',";
    strSql += "'" + tr2(data.strScanSpeed) + "',";
    strSql += "'" + tr2(data.streOperate) + "',";
	strSql +=		QString::number(data.GetTypeENum(data.eType)) + ",";
    strSql += "'" + tr2(data.strRemark) + "',";

	strSql += "`YXSC` = " + QString::number(data.nFiringRange) + ",";
	strSql += "`SJJD` = " + QString::number(data.dAttackprecision, 'f',2) + ",";
	strSql += "`HSFW` = " + QString::number(data.dHurtRange, 'f',2) + ",";
	strSql += "`DYDL` = " + QString::number(data.dHurtEquivalent, 'f',2) + ",";

	strSql += "`TXFW` = " + QString::number(data.dCommunicateRange, 'f', 2) + ",";
	strSql += "`GZPV` = " + QString::number(data.nWorkFrequency) + ",";
	strSql += "`GZMS` = " + QString::number(data.nWorkMode) + ",";
	strSql += "`TXDK` = " + QString::number(data.nWorkBandWidth) + ",";
	strSql += "`KGR`= " + QString::number(data.nResistJam) + ",";
	strSql += "`CHF`= " + QString::number(data.dPic_Length) + ",";
	strSql += "`DHF`= " + QString::number(data.dPic_width) + ",";
	strSql += "`FOCAL`= " + QString::number(data.dFocal_dis);

    strSql += ")";
    QSqlQuery query;
    bool success = query.exec(strSql);
    return success;
}

bool ITaskManageImpl::RemovePlatformLoadDBData(std::string strID)
{

    if (!IsPlatformLoadIDExisted(strID))
    {
        return false;
    }

    QString strSql("DELETE FROM RWSJ_DYZHLB where BH = '");
    strSql += QString::fromStdString(strID) + "'";
    QSqlQuery query;
    bool success = query.exec(strSql);
    return success;
}

bool ITaskManageImpl::IsPlatformLoadIDExisted(std::string strID)
{
    QString strSql("SELECT  * FROM `RWSJ_DYZHLB` WHERE `BH` = ");
    strSql += "'" + QString::fromStdString(strID) + "'";
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

void ITaskManageImpl::LoadActivatedActionIDFormDB()
{
    QSqlQuery query;
    bool success = query.exec("SELECT `ZZXD_TYSX`.`XDBH` FROM `ZZXD_TYSX` WHERE `ZZXD_TYSX`.`JHBZ` = '1';");
    if (success && query.size() == 1 && query.next())
    {
        bool ok = false;
        const int ActionGetID = query.value(0).toInt(&ok);
        if (m_ActivatedActionID != ActionGetID)
        {
            LoadActionformDB();
        }
    }
    else
    {
        m_ActivatedActionID = -1;
    }
}

void ITaskManageImpl::LoadActionformDB()
{
    QMutexLocker locker(&sMutex4);

    QSqlQuery query;
    bool success = query.exec("SELECT `XDBH`, `XDMC`, `XDDH`, `KSSJ`, `JSSJ`, `BZ` ,`JHBZ`,`XDMD`,`XDQY`,`QBLB` ,`QSJD` ,`QSWD` ,`QSGD` FROM `ZZXD_TYSX`");
    if (success)
    {
        m_NeubulaActionVec.clear();

        while (query.next())
        {
			QString str1 = tr2(query.value(0).toString().toLatin1().data());
			QString str2 = tr2(query.value(1).toString().toLocal8Bit().data());
			QString str3 = tr2(query.value(2).toString().toLocal8Bit().data());
			QString str4 = tr2(query.value(3).toString().toLocal8Bit().data());
			QString str5 = tr2(query.value(4).toString().toLocal8Bit().data());
			QString str6 = tr2(query.value(5).toString().toLocal8Bit().data());
			QString str7 = tr2(query.value(6).toString().toLocal8Bit().data());
			QString str8 = tr2(query.value(7).toString().toLocal8Bit().data());
			QString str9 = tr2(query.value(8).toString().toLocal8Bit().data());
			QString str10 = tr2(query.value(9).toString().toLocal8Bit().data());
			QString str11 = tr2(query.value(10).toString().toLatin1().data());
			QString str12 = tr2(query.value(11).toString().toLatin1().data());
			QString str13 = tr2(query.value(12).toString().toLatin1().data());

			CNeubulaAction stAction;
			stAction.SetID(str1.toInt());
			stAction.SetName(str2.toStdString());
			stAction.SetCode(str3.toStdString());
			stAction.SetStartTime(str4.toStdString());
			stAction.SetFinishTime(str5.toStdString());
			stAction.SetRemark(str6.toStdString());
			if (str7 == "1")
			{
				stAction.SetActivateStates(true);
                m_ActivatedActionID = stAction.GetID();
			}
			else
			{
				stAction.SetActivateStates(false);
			}

			stAction.SetObject(str8.toStdString());
			stAction.SetAreaID(str9.toStdString());
			stAction.SetInfo(str10.toStdString());

			stAction.SetStartPos(str11.toDouble(), str12.toDouble(), str13.toDouble());

            auto tarVec = GetRelationMark(stAction.GetID());
			std::vector <std::string> tmpVec;
			for (auto tarID : tarVec)
			{
                tmpVec.push_back(tarID.GetMarkID());
			}
            stAction.SetMarkIDList(tmpVec);

			auto platVec = GetRelationPtResource(stAction.GetID());
			ResourcesVec tmpRe;
			for (auto platInfo : platVec)
			{
				std::pair<std::string, int> mPair = std::make_pair(platInfo.GetResourceName(), platInfo.GetResourceNum());
				tmpRe.push_back(mPair);
			}
			stAction.SetPlatInfoVec(tmpRe);

			auto sensorVec = GetRelationSensorResource(stAction.GetID());
			tmpRe.clear();
			for (auto sensor : sensorVec)
			{
				std::pair<std::string, int> mPair = std::make_pair(sensor.GetResourceName(), sensor.GetResourceNum());
				tmpRe.push_back(mPair);
			}
			stAction.SetSensorInfoVec(tmpRe);

			auto AmmoVec = GetRelationAmmoResource(stAction.GetID());
			tmpRe.clear();
			for (auto ammo : AmmoVec)
			{
				std::pair<std::string, int> mPair = std::make_pair(ammo.GetResourceName(), ammo.GetResourceNum());
				tmpRe.push_back(mPair);
			}
			stAction.SetAmmoInfoVec(tmpRe);

			m_NeubulaActionVec.push_back(stAction);
        }
    }
}

bool ITaskManageImpl::EditActionDBData(CNeubulaAction &data)
{
    int nID = data.GetID();

    if (!IsActionIDExisted(nID))
    {
        return false;
    }

    QString strSql("UPDATE ZZXD_TYSX set ");
    strSql += "`XDMC` = '" + QString::fromStdString(data.GetName()) + "',";
    strSql += "`XDDH` = '" + QString::fromStdString(data.GetCode()) + "',";
    strSql += "`KSSJ` = '" + QString::fromStdString(data.GetStartTime()) + "',";
    strSql += "`JSSJ` = '" + QString::fromStdString(data.GetFinishTime()) + "',";
    strSql += "`BZ` = '" + QString::fromStdString(data.GetRemark()) + "',";
    strSql += "`XDMD` = '" + QString::fromStdString(data.GetObject()) + "',";
    strSql += "`XDQY` = '" + QString::fromStdString(data.GetAreaID()) + "',";
    strSql += "`QBLB` = '" + QString::fromStdString(data.GetInfo()) + "',";
    strSql += "`QSJD` = " + QString::number(data.GetStartLon(), 'f', 7) + ",";
    strSql += "`QSWD` = " + QString::number(data.GetStartLat(), 'f', 7) + ",";
    strSql += "`QSGD` = " + QString::number(data.GetStartAlt(), 'f', 2) + ",";
    strSql += "`JHBZ` = '" + QString::number(data.GetActivateStates()) + "'";

    strSql += "where `XDBH` = " + QString::number(data.GetID());
    QSqlQuery query;
    bool success = query.exec(strSql);
    return success;
}

bool ITaskManageImpl::AddActionDBData(CNeubulaAction &data)
{
    int nID = data.GetID();

    if (IsActionIDExisted(nID))
    {
        return false;
    }

    CNeubulaAction stAction;

    QString strSql("INSERT INTO ZZXD_TYSX (`XDBH`, `XDMC`, `XDDH`,`KSSJ`,`JSSJ`,`BZ`,`XDMD`,`XDQY`,`QBLB`,`QSJD`,`QSWD`,`QSGD`,`JHBZ`) VALUES (");
    strSql += QString::number(data.GetID()) + ",";
    strSql += "'" + QString::fromStdString(data.GetName()) + "',";
    strSql += "'" + QString::fromStdString(data.GetCode()) + "',";
    strSql += "'" + QString::fromStdString(data.GetStartTime()) + "',";
    strSql += "'" + QString::fromStdString(data.GetFinishTime()) + "',";
    strSql += "'" + QString::fromStdString(data.GetRemark()) + "',";
    strSql += "'" + QString::fromStdString(data.GetObject()) + "',";
    strSql += "'" + QString::fromStdString(data.GetAreaID()) + "',";
    strSql += "'" + QString::fromStdString(data.GetInfo()) + "',";
    strSql +=		QString::number(data.GetStartLat(), 'f', 7) + ",";
    strSql +=		QString::number(data.GetStartAlt(), 'f', 2) + ",";
    strSql +=		QString::number(data.GetStartLon(), 'f', 7) + ",";
    strSql += "'" + QString::number(data.GetActivateStates()) + "'";
    strSql += ")";
    QSqlQuery query;
    bool success = query.exec(strSql);
    return success;
}

bool ITaskManageImpl::RemoveActionDBData(int nID)
{

    if (!IsActionIDExisted(nID))
    {
        return false;
    }

    QString strSql("DELETE FROM ZZXD_TYSX where XDBH = ");
    strSql += QString::number(nID) ;
    QSqlQuery query;
    bool success = query.exec(strSql);
    return success;
}

bool ITaskManageImpl::IsActionIDExisted(int nID)
{
    QString strSql("SELECT  * FROM `ZZXD_TYSX` WHERE `XDBH` = ");
    strSql += "'" + QString::number(nID) + "'";
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

bool ITaskManageImpl::SetActionActivateStatesDB(int nID)
{
    QSqlQuery query;

    bool success = query.exec("SELECT `XDBH` FROM `ZZXD_TYSX` WHERE `JHBZ` = '1'");
    if (success)
    {
        while (query.next())
        {
			QString str1 = tr2(query.value(0).toString().toLatin1().data());
            if (nID != str1.toInt())
            {
                QString strSql("UPDATE ZZXD_TYSX set `JHBZ` = '0' WHERE `XDBH` = '");
                strSql += str1 + "'";
                success = query.exec(strSql);
            }
        }
    }

    QString strSql("UPDATE ZZXD_TYSX set `JHBZ` = '1' WHERE `XDBH` = '");
    strSql += QString::number(nID) + "'";
    success = query.exec(strSql);
    return success;
}

void qnzkna::TaskManage::ITaskManageImpl::UpdataAcitonMarkList()
{
    MarkIDVec IDVec;
    if (m_RelationActionMarkVec.size() == 0)
    {
        return;
    }

    for (auto &data : m_NeubulaActionVec)
    {
        CRelationActionMarkVec MarkVec = GetRelationMark(data.GetID());
        MarkIDVec IDList;
        for (auto itr : MarkVec)
        {
            IDList.push_back(itr.GetMarkID());
        }
        data.SetMarkIDList(IDList);
    }
}

void ITaskManageImpl::LoadRelationMarkDB()
{
    QMutexLocker locker(&sMutex5);

    QSqlQuery query;
    bool success = query.exec("SELECT `BH`, `XDBH`, `MBBH` FROM `Relation_RWLB_MB`");
    if (success)
    {
        m_RelationActionMarkVec.clear();

        while (query.next())
        {
			QString str1 = tr2(query.value(0).toString().toLatin1().data());
			QString str2 = tr2(query.value(1).toString().toLatin1().data());
			QString str3 = tr2(query.value(2).toString().toLocal8Bit().data());

            CRelationActionMark stRelation;
            stRelation.SetID(str1.toInt());
            stRelation.SetActionID(str2.toInt());
            stRelation.SetMarkID(str3.toStdString());

            m_RelationActionMarkVec.push_back(stRelation);
        }
    }

    UpdataAcitonMarkList();
}

bool ITaskManageImpl::EditRelationMarkDBData(CRelationActionMark &data)
{
    return true;
}

bool ITaskManageImpl::AddRelationMarkDBData(CRelationActionMark &data)
{

    if (IsRelationMarkDBExisted(data))
    {
        return false;
    }

    QString strSql("INSERT INTO Relation_RWLB_MB (`BH`, `XDBH`, `MBBH`) VALUES (");
    strSql += QString::number(data.GetID()) + ",";
    strSql += QString::number(data.GetActionID()) + ",";
    strSql += "'" + QString::fromStdString(data.GetMarkID()) + "'";
    strSql += ")";
    QSqlQuery query;
    bool success = query.exec(strSql);

    return success;
}

bool ITaskManageImpl::RemoveRelationMarkDBData(CRelationActionMark &data)
{

    if (!IsRelationMarkDBExisted(data))
    {
        return false;
    }

    QString strSql("DELETE FROM Relation_RWLB_MB where XDBH = ");
    strSql += QString::number(data.GetActionID());
    strSql += " AND `MBBH` = '" + QString::fromStdString(data.GetMarkID()) + "'";
    QSqlQuery query;
    bool success = query.exec(strSql);

    return success;
}

bool ITaskManageImpl::IsRelationMarkDBExisted(CRelationActionMark &data)
{
    QString strSql("SELECT  * FROM `Relation_RWLB_MB` WHERE `XDBH` = ");
    strSql += "'" + QString::number(data.GetActionID()) + "'";
    strSql += "AND `MBBH` = '" + QString::fromStdString(data.GetMarkID()) + "'";

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

int qnzkna::TaskManage::ITaskManageImpl::GetMaxRelationMarkID()
{
    QSqlQuery query;
    int nNoMax = 0;
    bool success = query.exec("SELECT MAX(`BH`) from Relation_RWLB_MB");
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

void qnzkna::TaskManage::ITaskManageImpl::LoadRelationMissionMarkDB()
{
    QMutexLocker locker(&sReMissionMark);

    QSqlQuery query;
    bool success = query.exec("SELECT `BH`, `RWBH`, `MBBH`, `RWYQ` FROM `Relation_RW_MB`");
    if (success)
    {
        m_RelationMissionMarkVec.clear();

        while (query.next())
        {
			QString str1 = tr2(query.value(0).toString().toLatin1().data());
			QString str2 = tr2(query.value(1).toString().toLocal8Bit().data());
			QString str3 = tr2(query.value(2).toString().toLocal8Bit().data());
			QString str4 = tr2(query.value(3).toString().toLatin1().data());

            CRelationMissionMark stRelation;
            stRelation.SetID(str1.toInt());
            stRelation.SetMissionID(str2.toStdString());
            stRelation.SetMarkID(str3.toStdString());
            stRelation.SetMarkDamage(str4.toInt());

            m_RelationMissionMarkVec.push_back(stRelation);
        }
    }

    UpdataMissionMarkList();
    UpdataMissionMarkDamage();
}

bool qnzkna::TaskManage::ITaskManageImpl::EditRelationMissionMarkDBData(CRelationMissionMark &data)
{
    return true;
}

bool qnzkna::TaskManage::ITaskManageImpl::AddRelationMissionMarkDBData(CRelationMissionMark &data)
{
    QMutexLocker locker(&sReMissionMark);

    if (IsRelationMissionMarkDBExisted(data))
    {
        return false;
    }

    QString strSql("INSERT INTO Relation_RW_MB (`BH`, `RWBH`, `MBBH`, `RWYQ`) VALUES (");
    strSql += QString::number(data.GetID()) + ",";
    strSql += "'" + QString::fromStdString(data.GetMissionID()) + "',";
    strSql += "'" + QString::fromStdString(data.GetMarkID()) + "',";
    strSql += QString::number(data.GetMarkDamage());
    strSql += ")";
    QSqlQuery query;
    bool success = query.exec(strSql);
    return success;
}

bool qnzkna::TaskManage::ITaskManageImpl::RemoveRelationMissionMarkDBData(CRelationMissionMark &data)
{
    QMutexLocker locker(&sReMissionMark);

    if (!IsRelationMissionMarkDBExisted(data))
    {
        return false;
    }

    QString strSql("DELETE FROM Relation_RW_MB where RWBH = ");
    strSql += "'" + QString::fromStdString(data.GetMissionID()) + "' ";
    strSql += "AND `MBBH` = '" + QString::fromStdString(data.GetMarkID()) + "'";
    QSqlQuery query;
    bool success = query.exec(strSql);
    return success;
}

bool qnzkna::TaskManage::ITaskManageImpl::RemoveRelationMissionMarkDBData(const std::string &strMission)
{
    QMutexLocker locker(&sReMissionMark);
    QString strSql("DELETE FROM Relation_RW_MB where RWBH = ");
    strSql += "'" + QString::fromStdString(strMission) + "'";
    QSqlQuery query;
    bool success = query.exec(strSql);
    return success;
}

bool qnzkna::TaskManage::ITaskManageImpl::IsRelationMissionMarkDBExisted(CRelationMissionMark &data)
{
    QString strSql("SELECT  * FROM `Relation_RW_MB` WHERE `RWBH` = ");
    strSql += "'" + QString::fromStdString(data.GetMissionID()) + "' ";
    strSql += "AND `MBBH` = '" + QString::fromStdString(data.GetMarkID()) + "'";

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

int qnzkna::TaskManage::ITaskManageImpl::GetMaxRelationMissionMarkID()
{
    QSqlQuery query;
    int nNoMax = 0;
    bool success = query.exec("SELECT MAX(`BH`) from Relation_RW_MB");
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

void ITaskManageImpl::LoadRelationTask()
{
    QMutexLocker locker(&sMutex7);

    QSqlQuery query;
    bool success = query.exec("SELECT `BH`, `XDBH`, `RWBH`, `PTBH`, `RWGZ`, `RWMB`, `HXBH` FROM `Relation_RWLB_PT_GZ`");
    if (success)
    {
        m_RelationActionTaskVec.clear();

        while (query.next())
        {
			QString str1 = tr2(query.value(0).toString().toLatin1().data());
			QString str2 = tr2(query.value(1).toString().toLatin1().data());
			QString str3 = tr2(query.value(2).toString().toLocal8Bit().data());
			QString str4 = tr2(query.value(3).toString().toLocal8Bit().data());
			QString str5 = tr2(query.value(4).toString().toLocal8Bit().data());
			QString str6 = tr2(query.value(5).toString().toLocal8Bit().data());
			QString str7 = tr2(query.value(6).toString().toLocal8Bit().data());

            CRelationActionTask stRelation;
            stRelation.SetID(str1.toInt());
            stRelation.SetActionID(str2.toInt());
            stRelation.SetTaskID(str3.toStdString());
            stRelation.SetPlatfromID(str4.toStdString());
            stRelation.SetLoadID(str5.toStdString());
            stRelation.SetMarkID(str6.toStdString());
            stRelation.SetLineID(str7.toStdString());

            m_RelationActionTaskVec.push_back(stRelation);
        }
    }

    UpdateRelationTaskShowData();
}

bool ITaskManageImpl::EditRelationTaskDBData(const CRelationActionTask data)
{
    int nID = data.GetID();

    if (!IsRelationTaskDBExisted(nID))
    {
        return false;
    }

    QString strSql("UPDATE Relation_RWLB_PT_GZ set ");
    strSql += "`XDBH` = " + QString::number(data.GetActionID()) + ",";
    strSql += "`RWBH` = '" + QString::fromStdString(data.GetTaskID()) + "',";
    strSql += "`PTBH` = '" + QString::fromStdString(data.GetPlatfromID()) + "',";
    strSql += "`RWGZ` = '" + QString::fromStdString(data.GetLoadID()) + "',";
    strSql += "`RWMB` = '" + QString::fromStdString(data.GetMarkID()) + "',";
    strSql += "`HXBH` = '" + QString::fromStdString(data.GetLineID()) + "'";
    strSql += "where `BH` = " + QString::number(data.GetID());

    QSqlQuery query;
    bool success = query.exec(strSql);
    return success;
}

bool ITaskManageImpl::EditRelationTaskDBData_LineID(const int nId, const std::string strLineID)
{

    if (!IsRelationTaskDBExisted(nId))
    {
        return false;
    }

    QString strSql("UPDATE Relation_RWLB_PT_GZ set ");
    strSql += "`HXBH` = '" + QString::fromStdString(strLineID) + "'";
    strSql += "where `BH` = " + QString::number(nId);

    QSqlQuery query;
    bool success = query.exec(strSql);
    return success;
}

bool ITaskManageImpl::AddRelationTaskDBData(const CRelationActionTask data)
{

    int nID = data.GetID();
    if (IsRelationTaskDBExisted(nID))
    {
        return false;
    }

    QString strSql("INSERT INTO Relation_RWLB_PT_GZ (`BH`, `XDBH`, `RWBH`, `PTBH`, `RWGZ`, `RWMB`, `HXBH`) VALUES (");
    strSql += QString::number(data.GetID()) + ",";
    strSql += QString::number(data.GetActionID()) + ",";
    strSql += "'" + QString::fromStdString(data.GetTaskID()) + "',";
    strSql += "'" + QString::fromStdString(data.GetPlatfromID()) + "',";
    strSql += "'" + QString::fromStdString(data.GetLoadID()) + "',";
    strSql += "'" + QString::fromStdString(data.GetMarkID()) + "',";
    strSql += "'" + QString::fromStdString(data.GetLineID()) + "'";
    strSql += ")";
    QSqlQuery query;
    bool success = query.exec(strSql);
    return success;
}

bool ITaskManageImpl::RemoveRelationTaskDBData(const int nID)
{

    if (!IsRelationTaskDBExisted(nID))
    {
        return false;
    }

    QString strSql("DELETE FROM Relation_RWLB_PT_GZ where BH = ");
    strSql += QString::number(nID);
    QSqlQuery query;
    bool success = query.exec(strSql);
    return success;
}

bool ITaskManageImpl::IsRelationTaskDBExisted(const int ID)
{
    QString strSql("SELECT  * FROM `Relation_RWLB_PT_GZ` WHERE `BH` = ");
    strSql += QString::number(ID);

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

void ITaskManageImpl::UpdateRelationTaskShowData()
{
    m_AcitonRelationShowVec.clear();
    for (auto data : m_RelationActionTaskVec)
    {
        if (data.GetActionID() == m_ActivatedActionID)
        {

            CNebulaAcitonRelation_PlatForm stRePlatForm;
            stRePlatForm.nDBID = data.GetID();
            stRePlatForm.strPlatformID = data.GetPlatfromID();
            stRePlatForm.strLoadIDVec.push_back(data.GetLoadID());
            stRePlatForm.strMarkIDvec.push_back(data.GetMarkID());
            stRePlatForm.strLineID = data.GetLineID();

            CNebulaAcitonRelation_Task stReTask;
            stReTask.nDBID = data.GetID();
            stReTask.strTaskID = data.GetTaskID();
            stReTask.PlatFormVec.push_back(stRePlatForm);

            CNebulaAcitonRelation staAcitonRelation;
            staAcitonRelation.nActionID = m_ActivatedActionID;
            staAcitonRelation.ReTaskVec.push_back(stReTask);

            AddRelationTaskShowData_Action(staAcitonRelation);
        }
    }
}

void ITaskManageImpl::AddRelationTaskShowData_Action(const CNebulaAcitonRelation stAction)
{
    auto itr = m_AcitonRelationShowVec.begin();
    for (; itr != m_AcitonRelationShowVec.end(); itr++)
    {
        if (itr->nActionID == stAction.nActionID)
        {
            for (auto data : stAction.ReTaskVec)
            {
                AddRelationTaskShowData_Task(itr->nActionID, data);
            }
            break;
        }
    }

    if (itr == m_AcitonRelationShowVec.end())
    {
        m_AcitonRelationShowVec.push_back(stAction);
    }
}

void ITaskManageImpl::AddRelationTaskShowData_Task(const int nActionID, const CNebulaAcitonRelation_Task stTask)
{
    auto itr = m_AcitonRelationShowVec.begin();
    for (; itr != m_AcitonRelationShowVec.end(); itr++)
    {
        if (itr->nActionID == nActionID)
        {
            auto itrTask = itr->ReTaskVec.begin();
            for (; itrTask != itr->ReTaskVec.end(); itrTask++)
            {
                if (itrTask->strTaskID == stTask.strTaskID)
                {
                    for (auto data : stTask.PlatFormVec)
                    {
                        AddRelationTaskShowData_Platform(nActionID, itrTask->strTaskID, data);
                    }
                    break;
                }
            }

            if (itrTask == itr->ReTaskVec.end())
            {
                itr->ReTaskVec.push_back(stTask);
            }

            return;
        }
    }
}

void ITaskManageImpl::AddRelationTaskShowData_Platform(const int nActionID, std::string strTaskID, const CNebulaAcitonRelation_PlatForm stPlatForm)
{
    auto itr = m_AcitonRelationShowVec.begin();
    for (; itr != m_AcitonRelationShowVec.end(); itr++)
    {
        if (itr->nActionID == nActionID)
        {
            auto itrTask = itr->ReTaskVec.begin();
            for (; itrTask != itr->ReTaskVec.end(); itrTask++)
            {
                if (itrTask->strTaskID == strTaskID)
                {
                    auto itrPlatform = itrTask->PlatFormVec.begin();
                    for (; itrPlatform != itrTask->PlatFormVec.end(); itrPlatform++)
                    {
                        if (itrPlatform->strPlatformID == stPlatForm.strPlatformID)
                        {

                            for (auto data : stPlatForm.strLoadIDVec)
                            {
                                itrPlatform->strLoadIDVec.push_back(data);
                            }

                            for (auto data : stPlatForm.strMarkIDvec)
                            {
                                itrPlatform->strMarkIDvec.push_back(data);
                            }

                            break;
                        }
                    }

                    if (itrPlatform == itrTask->PlatFormVec.end())
                    {
                        itrTask->PlatFormVec.push_back(stPlatForm);
                        return;
                    }
                }
            }
        }
    }
}

void ITaskManageImpl::LoadCodeSet_Load_DB()
{
    QMutexLocker locker(&sMutex6);

    QSqlQuery query;
    bool success = query.exec("SELECT `BH`, `XDBH`, `RWBH`, `HXBH`, `PTBH`, `GZBH`, `HDBH`, `HDJD`, `HDWD`, `HDGD`, `ZLSX`, `ZLBH`, `ZLCS`, `BZ` FROM `ZLJ_CGQ`");
    if (success)
    {
        m_CodeSet_LoadVec.clear();

        while (query.next())
        {
			QString str1 = tr2(query.value(0).toString().toLatin1().data());
			QString str2 = tr2(query.value(1).toString().toLatin1().data());
			QString str3 = tr2(query.value(2).toString().toLocal8Bit().data());
			QString str4 = tr2(query.value(3).toString().toLocal8Bit().data());
			QString str5 = tr2(query.value(4).toString().toLocal8Bit().data());
			QString str6 = tr2(query.value(5).toString().toLocal8Bit().data());
			QString str7 = tr2(query.value(6).toString().toLatin1().data());
			QString str8 = tr2(query.value(7).toString().toLatin1().data());
			QString str9 = tr2(query.value(8).toString().toLatin1().data());
			QString str10 = tr2(query.value(9).toString().toLatin1().data());
			QString str11 = tr2(query.value(10).toString().toLatin1().data());
			QString str12 = tr2(query.value(11).toString().toLatin1().data());
			const QByteArray& data13 = query.value(12).toByteArray();
			QString str13 = query.value(12).toString();
			QString str14 = tr2(query.value(13).toString().toLocal8Bit().data());

			CCodeSet_Load_DB stRelation;
			stRelation.SetID(str1.toInt());
			stRelation.SetActionID(str2.toInt());
			stRelation.SetTaskID(str3.toStdString());
			stRelation.SetLineID(str4.toStdString());
			stRelation.SetPlatformID(str5.toStdString());
			stRelation.SetLoadID(str6.toStdString());
			stRelation.SetPointID(str7.toInt());
			stRelation.SetLon(str8.toDouble());
			stRelation.SetLat(str9.toDouble());
			stRelation.SetAlt(str10.toDouble());
			stRelation.SetSuite(str11.toInt());
			stRelation.SetCodeID(str12.toInt());
			stRelation.SetParameter(std::string(data13.data(), data13.size()));
			stRelation.SetRemark(str14.toStdString());

            m_CodeSet_LoadVec.push_back(stRelation);
        }
    }
    locker.unlock();

    UpdateCodeSet_Load_Activated();
}

bool ITaskManageImpl::EditLoadCodeSet_Load_DB(const CCodeSet_Load_DB data)
{
    int nID = data.GetID();

    if (!IsCodeSetIDExisted_Load_DB(nID))
    {
        return false;
    }

    QString strSql("UPDATE ZLJ_CGQ set ");
    strSql += "`XDBH` = " + QString::number(data.GetActionID()) + ",";
    strSql += "`RWBH` = '" + QString::fromStdString(data.GetTaskID()) + "',";
    strSql += "`HXBH` = '" + QString::fromStdString(data.GetLineID()) + "',";
    strSql += "`PTBH` = '" + QString::fromStdString(data.GetPlatformID()) + "',";
    strSql += "`GZBH` = '" + QString::fromStdString(data.GetLoadID()) + "',";
    strSql += "`HDBH` = " + QString::number(data.GetPointID()) + ",";
    strSql += "`HDJD` = " + QString::number(data.GetLon(), 'f', 7) + ",";
    strSql += "`HDWD` = " + QString::number(data.GetLat(), 'f', 7) + ",";
    strSql += "`HDGD` = " + QString::number(data.GetAlt(), 'f', 2) + ",";
    strSql += "`ZLSX` = " + QString::number(data.GetSuite()) + ",";
    strSql += "`ZLBH` = " + QString::number(data.GetCodeID()) + ",";
    strSql += "`ZLCS` = '" + QString::fromStdString(data.GetParameter()) + "',";
    strSql += "`BZ` = '" + QString::fromStdString(data.GetRemark()) + "'";
    strSql += "where `BH` = " + QString::number(data.GetID());
    QSqlQuery query;
    bool success = query.exec(strSql);
    return success;
}

bool ITaskManageImpl::AddLoadCodeSet_Load_DB(const CCodeSet_Load_DB data)
{
    int nID = data.GetID();

    if (IsCodeSetIDExisted_Load_DB(nID))
    {
        return false;
    }

    QString strSql("INSERT INTO ZLJ_CGQ (BH, XDBH, RWBH, HXBH, PTBH, GZBH, HDBH, HDJD, HDWD, HDGD, ZLSX, ZLBH, ZLCS, BZ) VALUES (:BH,:XDBH,:RWBH,:HXBH,:PTBH,:GZBH,:HDBH,:HDJD,:HDWD,:HDGD,:ZLSX,:ZLBH,:ZLCS,:BZ)");
    QSqlQuery query;
    query.prepare(strSql);
    query.bindValue(":BH", (data.GetID()));
    query.bindValue(":XDBH", (data.GetActionID()));
    query.bindValue(":RWBH", QString::fromStdString(data.GetTaskID()));
    query.bindValue(":HXBH", QString::fromStdString(data.GetLineID()));
    query.bindValue(":PTBH", QString::fromStdString(data.GetPlatformID()));
    query.bindValue(":GZBH", QString::fromStdString(data.GetLoadID()));
    query.bindValue(":HDBH", (data.GetPointID()));
    query.bindValue(":HDJD", (data.GetLon()));
    query.bindValue(":HDWD", (data.GetLat()));
    query.bindValue(":HDGD", (data.GetAlt()));
    query.bindValue(":ZLSX", (data.GetSuite()));
    query.bindValue(":ZLBH", (data.GetCodeID()));
    query.bindValue(":ZLCS", QByteArray(data.GetParameter().c_str(), data.GetParameter().size()));
    query.bindValue(":BZ", QString::fromStdString(data.GetRemark()));
    bool success = query.exec();

    return success;
}

bool ITaskManageImpl::RemoveCodeSet_Load_DB(const int nID)
{

    if (!IsCodeSetIDExisted_Load_DB(nID))
    {
        return false;
    }

    QString strSql("DELETE FROM ZLJ_CGQ where BH = ");
    strSql += QString::number(nID);
    QSqlQuery query;
    bool success = query.exec(strSql);
    return success;
}

bool ITaskManageImpl::IsCodeSetIDExisted_Load_DB(const int strID)
{
    QString strSql("SELECT  * FROM `ZLJ_CGQ` WHERE `BH` = ");
    strSql += "'" + QString::number(strID) + "'";
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

bool LessSort(CCodeSet_Load_DB st1, CCodeSet_Load_DB st2)
{
    return (st1.GetPointID() < st2.GetPointID());
}

void ITaskManageImpl::UpdateCodeSet_Load_Activated()
{
    QMutexLocker locker(&sMutex6);

    m_CodeSet_LoadVec_Activating.clear();

    if (m_ActivatedActionID == -1)
    {
        return;
    }

    for (auto data : m_CodeSet_LoadVec)
    {
        if (data.GetActionID() == m_ActivatedActionID)
        {
            m_CodeSet_LoadVec_Activating.push_back(data);
        }
    }

    std::sort(m_CodeSet_LoadVec_Activating.begin(), m_CodeSet_LoadVec_Activating.end(), LessSort);
}

int ITaskManageImpl::GetMaxCodeSet_Load_ID_DB()
{
    QSqlQuery query;
    int nNoMax = 0;
    bool success = query.exec("SELECT MAX(`BH`) from ZLJ_CGQ");
    if (success)
    {
        while (query.next())
        {
			QString str01 = tr2(query.value(0).toString().toLatin1().data());
            nNoMax = str01.toInt();
            return nNoMax;
        }
    }
    return nNoMax;
}

void ITaskManageImpl::LoadLineDB()
{
    QMutexLocker locker(&sMutex8);

    QSqlQuery query;
    bool success = query.exec("SELECT `HXBH`, `HXMC`, `HXLX`, `HXXY` ,`SFXS` ,`RWBH` FROM `RWJS_HXLB`");
    if (success)
    {
        m_NeubulaMissionLineVec.clear();

        while (query.next())
        {
			QString str1 = tr2(query.value(0).toString().toLatin1().data());
			QString str2 = tr2(query.value(1).toString().toLocal8Bit().data());
			QString str3 = tr2(query.value(2).toString().toLocal8Bit().data());
			QString str4 = tr2(query.value(3).toString().toLocal8Bit().data());
			QString str5 = tr2(query.value(4).toString().toLocal8Bit().data());
			QString str6 = tr2(query.value(5).toString().toLocal8Bit().data());

			CNeubulaMissionLine stLine;
			stLine.nID = str1.toInt();
            strncpy(stLine.strName, str2.toLocal8Bit(), MAX_LINE_LEN_1);
            strncpy(stLine.strMissionID, str6.toLocal8Bit(), MAX_LINE_LEN_1);
			stLine.cType = str3.toInt();
			stLine.cProtocolType = str4.toInt();
			stLine.cShowFlag = str5.toInt();

            QString strSql = "SELECT `No`, `HXBH`, `JD`, `WD`, `GD`, `SD`, `XTSJ`, `GDKZ`, `BJ`, `HXSX`, `ZWSX`, `JLD`, `HXDLX`, `HDMC`, `ZSDASJ` FROM `RWSJ_HDLB` WHERE `HXBH` = ";
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
				QString str12 = tr2(queryPoint.value(11).toString().toLatin1().data());
				QString str13 = tr2(queryPoint.value(12).toString().toLatin1().data());
				QString str14 = tr2(queryPoint.value(13).toString().toLocal8Bit().data());
				QString str15 = tr2(queryPoint.value(14).toString().toLocal8Bit().data());

				CNeubulaMissionLinePoint stPoint;
				stPoint.nNo = str01.toInt();
				stPoint.nLineID = str02.toInt();
				stPoint.dLon = str03.toDouble();
				stPoint.dLat = str04.toDouble();
				stPoint.dAlt = str05.toDouble();
				stPoint.dSpeed = str06.toDouble();
				stPoint.dHoverTime = str07.toDouble();
				stPoint.cHeightCtrl = str08.toInt();
				stPoint.DRadius = str09.toDouble();
				stPoint.cLineMode = str10.toInt();
				stPoint.cTurnMode = str11.toInt();
				stPoint.cLanding = str12.toInt();
				stPoint.cType = str13.toInt();
                strncpy(stPoint.cName, str14.toLocal8Bit(), MAX_LINE_LEN_1);
                strncpy(stPoint.dReachTime, str15.toLocal8Bit(), MAX_LINE_LEN_1);

                stLine.LinePointVec.push_back(stPoint);
            }

            m_NeubulaMissionLineVec.push_back(stLine);
        }
    }
}

void ITaskManageImpl::LoadLineDBByTask(const std::string &taskID)
{
    QMutexLocker locker(&sMutex8);
    {
        CNeubulaMissionLineVec::iterator iter = m_NeubulaMissionLineVec.begin();
        for (; iter != m_NeubulaMissionLineVec.end();)
        {
            char tmpTaskID[MAX_MISSION_LEN_1];
            strncpy(tmpTaskID, QString::fromStdString(taskID).toLocal8Bit(), MAX_MISSION_LEN_1);
            if (0 == strcmp(tmpTaskID, iter->strMissionID))
            {
                iter = m_NeubulaMissionLineVec.erase(iter);
                continue ;
            }
            iter++;
        }
    }

    QSqlQuery query;
    bool success = query.exec("SELECT `HXBH`, `HXMC`, `HXLX`, `HXXY` ,`SFXS` ,`RWBH` FROM `RWJS_HXLB` WHERE `RWBH` = '" + QString::fromStdString(taskID) + "'");
    if (success)
    {
        while (query.next())
        {
            QString str1 = tr2(query.value(0).toString().toLatin1().data());
            QString str2 = tr2(query.value(1).toString().toLocal8Bit().data());
            QString str3 = tr2(query.value(2).toString().toLocal8Bit().data());
            QString str4 = tr2(query.value(3).toString().toLocal8Bit().data());
            QString str5 = tr2(query.value(4).toString().toLocal8Bit().data());
            QString str6 = tr2(query.value(5).toString().toLocal8Bit().data());

            CNeubulaMissionLine stLine;
            stLine.nID = str1.toInt();
            strncpy(stLine.strName, str2.toLocal8Bit(), MAX_LINE_LEN_1);
            strncpy(stLine.strMissionID, str6.toLocal8Bit(), MAX_LINE_LEN_1);
            stLine.cType = str3.toInt();
            stLine.cProtocolType = str4.toInt();
            stLine.cShowFlag = str5.toInt();

            QString strSql = "SELECT `No`, `HXBH`, `JD`, `WD`, `GD`, `SD`, `XTSJ`, `GDKZ`, `BJ`, `HXSX`, `ZWSX`, `JLD`, `HXDLX`, `HDMC`, `ZSDASJ` FROM `RWSJ_HDLB` WHERE `HXBH` = ";
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
                QString str12 = tr2(queryPoint.value(11).toString().toLatin1().data());
				QString str13 = tr2(queryPoint.value(12).toString().toLatin1().data());
				QString str14 = tr2(queryPoint.value(13).toString().toLocal8Bit().data());
				QString str15 = tr2(queryPoint.value(14).toString().toLocal8Bit().data());

                CNeubulaMissionLinePoint stPoint;
                stPoint.nNo = str01.toInt();
                stPoint.nLineID = str02.toInt();
                stPoint.dLon = str03.toDouble();
                stPoint.dLat = str04.toDouble();
                stPoint.dAlt = str05.toDouble();
                stPoint.dSpeed = str06.toDouble();
                stPoint.dHoverTime = str07.toDouble();
                stPoint.cHeightCtrl = str08.toInt();
                stPoint.DRadius = str09.toDouble();
                stPoint.cLineMode = str10.toInt();
                stPoint.cTurnMode = str11.toInt();
                stPoint.cLanding = str12.toInt();
				stPoint.cType = str13.toInt();
                strncpy(stPoint.cName, str14.toLocal8Bit(), MAX_LINE_LEN_1);
                strncpy(stPoint.dReachTime, str15.toLocal8Bit(), MAX_LINE_LEN_1);

                stLine.LinePointVec.push_back(stPoint);
            }

            bool findLine = false;
            size_t findIndex = 0;
            for (size_t index = 0; index < m_NeubulaMissionLineVec.size(); index++)
            {
                const CNeubulaMissionLine &neubulaMissionLine = m_NeubulaMissionLineVec.at(index);
                char tmpTaskID[MAX_MISSION_LEN_1];
                strncpy(tmpTaskID, QString::fromStdString(taskID).toLocal8Bit(), MAX_MISSION_LEN_1);
                if ((0 == strcmp(tmpTaskID, neubulaMissionLine.strMissionID)) && neubulaMissionLine.nID == stLine.nID)
                {
                    findLine  = true;
                    findIndex = index;
                    break;
                }
            }

            if (findLine && findIndex > 0 && findIndex < m_NeubulaMissionLineVec.size())
            {
                m_NeubulaMissionLineVec.at(findIndex) = stLine;
            }
            else
            {
                m_NeubulaMissionLineVec.push_back(stLine);
            }
        }
    }
}

bool ITaskManageImpl::AddlineDB(const CNeubulaMissionLine data)
{

    if (IsLineExistedDB(data.nID))
    {
        return false;
    }

    QString strSql("INSERT INTO RWJS_HXLB (`HXBH`, `RWBH`, `HXMC`, `HXLX`, `HXXY`,`SFXS`) VALUES (");
    strSql += QString::number(data.nID) + ",";
    strSql += "'" + tr2(data.strMissionID) + "',";
    strSql += "'" + tr2(data.strName) + "',";
    strSql += QString::number(data.cType) + ",";
    strSql += QString::number(data.cProtocolType) + ",";
    strSql += QString::number(data.cShowFlag);
    strSql += ")";

    QSqlQuery query;
    bool success = query.exec(strSql);

    for (auto point : data.LinePointVec)
    {
        AddLinePointDB(point);
    }

    if (success)
    {
        int len = sendSystemUpdate(ESystemUpdateType::ESystemUpdateType_Route, ESystemOperatorType::ESystemOperatorType_Add, std::to_string(data.nID).data());
    }
    return success;
}

bool ITaskManageImpl::EditLineDB(const CNeubulaMissionLine data)
{

    if (!IsLineExistedDB(data.nID))
    {
        return false;
    }

    QString strSql("UPDATE RWJS_HXLB set ");
    strSql += "`RWBH` = '" + tr2(data.strMissionID) + "',";
    strSql += "`HXMC` = '" + tr2(data.strName) + "',";
    strSql += "`HXLX` = " + QString::number(data.cType) + ",";
    strSql += "`HXXY` = " + QString::number(data.cProtocolType) + ",";
    strSql += "`SFXS` = " + QString::number(data.cShowFlag) + " ";
    strSql += "where `HXBH` = " + QString::number(data.nID);
    QSqlQuery query;
    bool success = query.exec(strSql);
    RemoveLinePointDB(data.nID);
    for (auto point : data.LinePointVec)
    {
        AddLinePointDB(point);
    }
    if (success)
    {
        int len = sendSystemUpdate(ESystemUpdateType::ESystemUpdateType_Route, ESystemOperatorType::ESystemOperatorType_Modify, std::to_string(data.nID).data());
    }
    return success;
}

bool ITaskManageImpl::RemovelineDB(const int nID)
{

    if (!IsLineExistedDB(nID))
    {
        return false;
    }

    QString strSql("DELETE FROM RWJS_HXLB where HXBH = ");
    strSql += QString::number(nID);

    QSqlQuery query;
    bool success = query.exec(strSql);
    RemoveLinePointDB(nID);

    if (success)
    {
        int len = sendSystemUpdate(ESystemUpdateType::ESystemUpdateType_Route, ESystemOperatorType::ESystemOperatorType_Delete, std::to_string(nID).data());
    }
    return success;

}

bool ITaskManageImpl::IsLineExistedDB(const int nID)
{
    QString strSql("SELECT  * FROM `RWJS_HXLB` WHERE `HXBH` = ");
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

bool ITaskManageImpl::SetLineShowStatesDB(const int nLineID, const bool bShow)
{

    if (!IsLineExistedDB(nLineID))
    {
        return false;
    }

    QString strSql("UPDATE RWJS_HXLB set ");
    strSql += "`SFXS` = " + QString::number(bShow) + " ";
    strSql += "where `HXBH` = " + QString::number(nLineID);
    QSqlQuery query;
    bool success = query.exec(strSql);
    return success;
}

bool ITaskManageImpl::AddLinePointDB(const CNeubulaMissionLinePoint data)
{

    if (IsLinePointExistedDB(data.nLineID, data.nNo))
    {
        return false;
    }

    QSqlQuery query;
    int nNoMax = GetMissionLinePointMaxID();

    QString strSql("INSERT INTO RWSJ_HDLB (`No`, `HXBH`, `JD`, `WD`, `GD`, `SD`, `XTSJ`, `GDKZ`, `BJ`, `HXSX`, `ZWSX`, `JLD`, `HXDLX`, `HDMC`, `ZSDASJ`) VALUES (");
    strSql += QString::number(nNoMax) + ",";
    strSql += QString::number(data.nLineID) + ",";
    strSql += QString::number(data.dLon, 'f', 7) + ",";
    strSql += QString::number(data.dLat, 'f', 7) + ",";
    strSql += QString::number(data.dAlt, 'f', 2) + ",";
    strSql += QString::number(data.dSpeed, 'f', 2) + ",";
    strSql += QString::number(data.dHoverTime, 'f', 2) + ",";
    strSql += QString::number(data.cHeightCtrl) + ",";
    strSql += QString::number(data.DRadius, 'f', 2) + ",";
    strSql += QString::number(data.cLineMode) + ",";
    strSql += QString::number(data.cTurnMode) + ",";
    strSql += QString::number(data.cLanding) + ",";
    strSql += QString::number(data.cType) + ",";
    strSql += "'" + tr2(data.cName) + "',";
    strSql += "'" + tr2(data.dReachTime) + "'";
    strSql += ")";

    bool success = query.exec(strSql);
    return success;
}

bool ITaskManageImpl::AddLinePointsDB(const CNeubulaMissionLinePointVec data)
{
    for (auto itr : data)
    {
        AddLinePointDB(itr);
    }
    return true;
}

bool ITaskManageImpl::EditLinePointDB(const CNeubulaMissionLinePoint data)
{

    if (!IsLinePointExistedDB(data.nLineID, data.nNo))
    {
        return false;
    }

    QString strSql("UPDATE RWSJ_HDLB set ");
    strSql += "`HXBH` = " + QString::number(data.nLineID) + ",";
    strSql += "`JD` = " + QString::number(data.dLon, 'f', 7) + ",";
    strSql += "`WD` = " + QString::number(data.dLat, 'f', 7) + ",";
    strSql += "`GD` = " + QString::number(data.dAlt, 'f', 2) + ",";
    strSql += "`SD` = " + QString::number(data.dSpeed, 'f', 2) + ",";
    strSql += "`XTSJ` = " + QString::number(data.dHoverTime, 'f', 2) + ",";
    strSql += "`GDKZ` = " + QString::number(data.cHeightCtrl) + ",";
    strSql += "`BJ` = " + QString::number(data.DRadius, 'f', 2) + ",";
    strSql += "`HXSX` = " + QString::number(data.cLineMode) + ",";
    strSql += "`ZWSX` = " + QString::number(data.cTurnMode) + ",";
    strSql += "`JLD` = " + QString::number(data.cLanding) + ",";
    strSql += "`HXDLX` = " + QString::number(data.cLanding) + ",";
    strSql += "`HDMC` = '" + tr2(data.cName) + "',";
    strSql += "`ZSDASJ` =  '" + tr2(data.dReachTime) + "'";
    strSql += "where `No` = " + QString::number(data.nNo);

    QSqlQuery query;
    bool success = query.exec(strSql);
    return success;
}

bool ITaskManageImpl::InsertLinePointDB(const CNeubulaMissionLinePoint data)
{

    updatePointsIndexFormNow(data.nNo, data.nLineID, 1);

    QSqlQuery query;

    QString strSql("INSERT INTO RWSJ_HDLB (`No`, `HXBH`, `JD`, `WD`, `GD`, `SD`, `XTSJ`, `GDKZ`, `BJ`, `HXSX`, `ZWSX`, `JLD`, `HXDLX`, `HDMC`, `ZSDASJ`) VALUES (");
    strSql += QString::number(data.nNo) + ",";
    strSql += QString::number(data.nLineID) + ",";
    strSql += QString::number(data.dLon, 'f', 7) + ",";
    strSql += QString::number(data.dLat, 'f', 7) + ",";
    strSql += QString::number(data.dAlt, 'f', 2) + ",";
    strSql += QString::number(data.dSpeed, 'f', 2) + ",";
    strSql += QString::number(data.dHoverTime, 'f', 2) + ",";
    strSql += QString::number(data.cHeightCtrl) + ",";
    strSql += QString::number(data.DRadius, 'f', 2) + ",";
    strSql += QString::number(data.cLineMode) + ",";
    strSql += QString::number(data.cTurnMode) + ",";
    strSql += QString::number(data.cLanding);
    strSql += QString::number(data.cType) + ",";
    strSql += "'" + QString::fromStdString(data.cName) + "',";
    strSql += "'" + QString::fromStdString(data.dReachTime) + "'";
    strSql += ")";

    bool success = query.exec(strSql);
    return success;

}

bool ITaskManageImpl::RemoveLinePointDB(const int nLineID, const int nPointID)
{

    if (!IsLinePointExistedDB(nLineID, nPointID))
    {
        return false;
    }

    QString strSql("DELETE FROM RWSJ_HDLB where `No` = ");
    strSql += QString::number(nPointID) + " && `HXBH` = ";
    strSql += QString::number(nLineID);

    QSqlQuery query;
    bool success = query.exec(strSql);

    updatePointsIndexFormNow(nPointID + 1, nLineID, -1);

    return success;
}

bool ITaskManageImpl::RemoveLinePointDB(const int nLineID)
{
    QString strSql("DELETE FROM RWSJ_HDLB where `HXBH` = ");
    strSql += QString::number(nLineID);

    QSqlQuery query;
    bool success = query.exec(strSql);
    return success;
}

bool ITaskManageImpl::IsLinePointExistedDB(const int nLineID, const int nPointID)
{
    QString strSql("SELECT  * FROM `RWSJ_HDLB` WHERE `HXBH` = ");
    strSql +=  QString::number(nLineID) + " && `No` = ";
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

void ITaskManageImpl::LoadResourceSensorDB()
{
    QMutexLocker locker(&sRsMutex1);

    QSqlQuery query;
    QString strSql = "SELECT `CGQZBLX`, `CGQZBXH`, `CGQZBMC`, `CGQZBJC`, `CGQZBTP`,`CGQZBBZ` FROM `ZZZY_CGQZY`";
    bool success = query.exec(strSql);
    if (success)
    {
        m_NebulaResourcesSensorVec.clear();

        while (query.next())
        {
			QString str1 = tr2(query.value(0).toString().toLatin1().data());
			QString str2 = tr2(query.value(1).toString().toLatin1().data());
			QString str3 = tr2(query.value(2).toString().toLocal8Bit().data());
			QString str4 = tr2(query.value(3).toString().toLocal8Bit().data());
			QString str5 = tr2(query.value(4).toString().toLocal8Bit().data());
			QString str6 = tr2(query.value(5).toString().toLocal8Bit().data());

            LPNebulaResourcesSensor stSensor;
            stSensor.SetLoadType(str1.toInt());
            stSensor.SetLoadModel(str2.toInt());
            std::string mName = str3.toStdString().c_str();
            stSensor.SetLoadName(mName);
            stSensor.SetLoadShortName(str4.toStdString());
            stSensor.SetLoadPic(str5.toStdString());
            stSensor.SetLoadRemark(str6.toStdString());

            m_NebulaResourcesSensorVec.push_back(stSensor);
        }
    }
}

void ITaskManageImpl::LoadResourcePtDB()
{
    QMutexLocker locker(&sRsMutex2);

    QSqlQuery query;
    QString strSql = "SELECT `PTZBLX`, `PTZBXH`, `PTZBMC`, `PTZBJC`, `PTZBTP`,`PTZBBZ`";
    strSql += " FROM `ZZZY_PTZY`";
    bool success = query.exec(strSql);
    if (success)
    {
        m_NebulaResourcesPtVec.clear();

        while (query.next())
        {
			QString str1 = tr2(query.value(0).toString().toLatin1().data());
			QString str2 = tr2(query.value(1).toString().toLatin1().data());
			QString str3 = tr2(query.value(2).toString().toLocal8Bit().data());
			QString str4 = tr2(query.value(3).toString().toLocal8Bit().data());
			QString str5 = tr2(query.value(4).toString().toLocal8Bit().data());
			QString str6 = tr2(query.value(5).toString().toLocal8Bit().data());

            LPNebulaResourcesPt stSensor;
            short int ntype = str1.toInt();
            stSensor.SetLoadType(ntype);
            stSensor.SetLoadModel(str2.toInt());
            stSensor.SetLoadName(str3.toStdString());
            stSensor.SetLoadShortName(str4.toStdString());
            stSensor.SetLoadPic(str5.toStdString());
            stSensor.SetLoadRemark(str6.toStdString());

            m_NebulaResourcesPtVec.push_back(stSensor);
        }
    }
}

void ITaskManageImpl::LoadResourceAmmoDB()
{
    QMutexLocker locker(&sRsMutex3);

    QSqlQuery query;
    QString strSql = "SELECT `DYZBLX`, `DYZBXH`, `DYZBMC`, `DYZBJC`, `DYZBTP`,`DYZBBZ`";
    strSql += " FROM `ZZZY_DYZY`";
    bool success = query.exec(strSql);
    if (success)
    {
        m_NebulaResourcesAmmoVec.clear();

        while (query.next())
        {
			QString str1 = tr2(query.value(0).toString().toLatin1().data());
			QString str2 = tr2(query.value(1).toString().toLatin1().data());
			QString str3 = tr2(query.value(2).toString().toLocal8Bit().data());
			QString str4 = tr2(query.value(3).toString().toLocal8Bit().data());
			QString str5 = tr2(query.value(4).toString().toLocal8Bit().data());
			QString str6 = tr2(query.value(5).toString().toLocal8Bit().data());

            LPNebulaResourcesAmmo stSensor;
            short int ntype = str1.toInt();
            stSensor.SetLoadType(ntype);
            stSensor.SetLoadModel(str2.toInt());
            stSensor.SetLoadName(str3.toStdString());
            stSensor.SetLoadShortName(str4.toStdString());
            stSensor.SetLoadPic(str5.toStdString());
            stSensor.SetLoadRemark(str6.toStdString());

            m_NebulaResourcesAmmoVec.push_back(stSensor);
        }
    }
}

CNeubulaMissionVec ITaskManageImpl::GetAllMission()
{
    QMutexLocker locker(&sMutex);
    return m_NeubulaMissionVec;
}

CNeubulaMissionVec ITaskManageImpl::GetActivatingMissions()
{
    QMutexLocker locker(&sMutex);
    UpdateRelationTaskShowData();
    m_NeubulaActivatingMissionVec.clear();
    auto itr = m_NeubulaMissionVec.begin();
    for (; itr != m_NeubulaMissionVec.end(); itr++)
    {
        if (IsTaskInAction(itr->GetID()))
        {
            m_NeubulaActivatingMissionVec.push_back(*itr);
        }
    }
    return m_NeubulaActivatingMissionVec;
}

std::vector<std::string> qnzkna::TaskManage::ITaskManageImpl::GetMissionsByAction(int nActionID)
{
    std::vector<std::string> reStrs;

    QSqlQuery query;
    QString sqlStr = "SELECT `RWBH` FROM `Relation_RWLB_PT_GZ` WHERE `XDBH` = '";
    sqlStr = sqlStr + QString::number(nActionID) + "'";
    bool success = query.exec(sqlStr);
    if (success)
    {
        reStrs.clear();

        while (query.next())
        {
			QString str1 = tr2(query.value(0).toString().toLocal8Bit().data());
            reStrs.push_back(str1.toStdString());
        }
    }
    return reStrs;

}

CNeubulaMission ITaskManageImpl::GetMission(const std::string &strID)
{
    QMutexLocker locker(&sMutex);
    CNeubulaMission stInfo;
    for (auto data : m_NeubulaMissionVec)
    {
        if (0 == strcmp(strID.c_str(), QString::fromStdString(data.GetID().c_str()).toLocal8Bit()))
        {
            stInfo = data;
            break;
        }
    }
    return stInfo;
}

CNeubulaMissionVec ITaskManageImpl::GetMissionByModel(std::string &strMode)
{
    QMutexLocker locker(&sMutex);
    CNeubulaMissionVec wndVec;

    return wndVec;
}

CNeubulaMissionVec ITaskManageImpl::GetMissionByLoadModel(std::string &strMode)
{
    QMutexLocker locker(&sMutex);
    CNeubulaMissionVec wndVec;

    return wndVec;
}

bool ITaskManageImpl::SetMission(CNeubulaMission &data)
{
    QMutexLocker locker(&sMutex);
    auto itr = m_NeubulaMissionVec.begin();
    bool bsucc = true;
    for (; itr != m_NeubulaMissionVec.end(); itr++)
    {
        if (0 == strcmp(itr->GetID().c_str(), data.GetID().c_str()))
        {
            bsucc = EditDBData(data);
            *itr = data;
            break;
        }
    }

    if (itr == m_NeubulaMissionVec.end())
    {
        m_NeubulaMissionVec.push_back(data);
        bsucc = AddDBData(data);
    }

    return bsucc;
}

bool ITaskManageImpl::SetMission(CNeubulaMission &data, std::string strOldID)
{
    QMutexLocker locker(&sMutex);
    auto itr = m_NeubulaMissionVec.begin();
    for (; itr != m_NeubulaMissionVec.end(); itr++)
    {
        if (0 == strcmp(itr->GetID().c_str(), strOldID.c_str()))
        {
            EditDBData(data, strOldID);
            *itr = data;
            break;
        }
    }

    return true;
}

bool ITaskManageImpl::RmoveMission(const std::string &strID)
{
    QMutexLocker locker(&sMutex);

    auto itr = m_NeubulaMissionVec.begin();
    for (; itr != m_NeubulaMissionVec.end(); itr++)
    {
        if (0 == strcmp(itr->GetID().c_str(), strID.c_str()))
        {
            m_NeubulaMissionVec.erase(itr);
            RemoveDBData(strID);
            return true;
        }
    }
    return false;
}

bool ITaskManageImpl::RmoveAllMission()
{
    QMutexLocker locker(&sMutex);
    m_NeubulaMissionVec.clear();
    return true;
}

bool ITaskManageImpl::IsExisted(const std::string &strID)
{
    QMutexLocker locker(&sMutex);
    CNeubulaMission stInfo;
    for (auto data : m_NeubulaMissionVec)
    {
        if (0 == strcmp(strID.c_str(), QString::fromStdString(data.GetID().c_str()).toLocal8Bit()))
        {
            return true;
        }
    }
    return false;
}

bool ITaskManageImpl::LoadData()
{
    LoadDataBase();
    LoadRelationTask();
    LoadRelationMissionMarkDB();
    LoadRelationMissionPtDB();
    LoadMissionAmmoDB();
    return true;
}

StringVector ITaskManageImpl::GetTaskLineByFileName(std::string &strID)
{
    StringVector LineIDList;

    if (strID == "")
    {
        return LineIDList;
    }

    QString cstrFileName = QString::fromStdString(strID);
    if (-1 == cstrFileName.lastIndexOf(".dat"))
    {
        cstrFileName += ".dat";
    }

    QString cstrFullPath = QString::fromStdString(m_strDatabasePath) + cstrFileName;

    QFile findFile;
    findFile.setFileName(cstrFullPath);
    bool bFind = findFile.exists();

    if (!bFind)
    {
        return LineIDList;
    }

    std::string strDataPath = cstrFullPath.toLocal8Bit().data();
    TiXmlDocument doc;
    bool bLoadFile = doc.LoadFile(strDataPath.c_str());
    if (!bLoadFile)
    {
        return LineIDList;
    }
    TiXmlElement *pRootEle = doc.RootElement();
    if (!pRootEle)
    {
        return LineIDList;
    }
    TiXmlElement *pNebulaTaskEle = pRootEle->FirstChildElement("NebulaTask");
    while (pNebulaTaskEle)
    {
        const char *pChar = pNebulaTaskEle->Attribute("ID");
        if (!pChar)
        {
            return LineIDList;
        }
        unsigned int nTaskID = StringToInt(pChar);
        pChar = pNebulaTaskEle->Attribute("Name");
        if (!pChar)
        {
            return LineIDList;
        }
        const char *strTaskName = pChar;
        pChar = pNebulaTaskEle->Attribute("CodeName");
        if (!pChar)
        {
            return LineIDList;
        }
        unsigned int nCodeName = StringToInt(pChar);

        CNebulaTask nebulaTask;
        nebulaTask.SetTaskID(nTaskID);
        nebulaTask.SetTaskName(strTaskName);
        nebulaTask.SetCodeName(nCodeName);
        nebulaTask.SetTaskTime(0, 0);
        nebulaTask.SetTaskType(ENebulaTaskType::ENebulaTaskType_Unknown);

        const char *strStartTime = pNebulaTaskEle->Attribute("StartTime");
        const char *strEndTime = pNebulaTaskEle->Attribute("EndTime");
        if (strStartTime != NULL && strEndTime != NULL)
        {
            char  *stopstring;
            unsigned long long startTime = strtoull(strStartTime, &stopstring, 10);
            unsigned long long endTime = strtoull(strEndTime, &stopstring, 10);
            nebulaTask.SetTaskTime(startTime, endTime);
        }

        const char *strTaskType = pNebulaTaskEle->Attribute("TaskType");
        if (strTaskType != NULL)
        {
            nebulaTask.SetTaskType(ENebulaTaskType(StringToInt(strTaskType)));
        }

        TiXmlElement *pChildEle = pNebulaTaskEle->FirstChildElement("NebulaRouteGroup");
        while (pChildEle)
        {
            CNebulaRouteGroup groupInfo;
            if (ParseNebulaRouteGroupNode(pChildEle, nebulaTask, groupInfo))
            {
                nebulaTask.AddRouteGroup(groupInfo);
                groupInfo.GetWeaponIDList(&LineIDList);
                return LineIDList;
            }
            else
            {
                return LineIDList;
            }
            pChildEle = pChildEle->NextSiblingElement("NebulaRouteGroup");
        }
        break;
    }
    return LineIDList;
}

CNebulaRouteLine ITaskManageImpl::GetTaskLinePointByFileName(std::string &strID, std::string strUAVID)
{
    CNebulaRouteLine PointListVec;

    if (strID == "")
    {
        return PointListVec;
    }

    if (strUAVID == "")
    {
        return PointListVec;
    }

    QString cstrFileName = QString::fromStdString(strID);
    if (-1 == cstrFileName.lastIndexOf(".dat"))
    {
        cstrFileName += ".dat";
    }

    QString cstrFullPath = QString::fromStdString(m_strDatabasePath) + cstrFileName;

    QFile findFile;
    findFile.setFileName(cstrFullPath);
    bool bFind = findFile.exists();

    if (!bFind)
    {
        return PointListVec;
    }

    std::string strDataPath = cstrFullPath.toLocal8Bit().data();
    TiXmlDocument doc;
    bool bLoadFile = doc.LoadFile(strDataPath.c_str());

    if (!bLoadFile)
    {
        return PointListVec;
    }

    TiXmlElement *pRootEle = doc.RootElement();

    if (!pRootEle)
    {
        return PointListVec;
    }

    TiXmlElement *pNebulaTaskEle = pRootEle->FirstChildElement("NebulaTask");
    while (pNebulaTaskEle)
    {
        const char *pChar = pNebulaTaskEle->Attribute("ID");

        if (!pChar)
        {
            return PointListVec;
        }

        unsigned int nTaskID = StringToInt(pChar);
        pChar = pNebulaTaskEle->Attribute("Name");

        if (!pChar)
        {
            return PointListVec;
        }

        const char *strTaskName = pChar;
        pChar = pNebulaTaskEle->Attribute("CodeName");

        if (!pChar)
        {
            return PointListVec;
        }

        unsigned int nCodeName = StringToInt(pChar);

        CNebulaTask nebulaTask;
        const char *strStartTime = pNebulaTaskEle->Attribute("StartTime");
        const char *strEndTime = pNebulaTaskEle->Attribute("EndTime");
        const char *strTaskType = pNebulaTaskEle->Attribute("TaskType");
        if (strTaskType != NULL)
        {
            nebulaTask.SetTaskType(ENebulaTaskType(StringToInt(strTaskType)));
        }

        TiXmlElement *pChildEle = pNebulaTaskEle->FirstChildElement("NebulaRouteGroup");
        while (pChildEle)
        {
            CNebulaRouteGroup groupInfo;
            if (ParseNebulaRouteGroupNode(pChildEle, nebulaTask, groupInfo))
            {
                groupInfo.GetRouteLine(strUAVID.c_str(), &PointListVec);
                return PointListVec;
            }
            else
            {
                return PointListVec;
            }
            pChildEle = pChildEle->NextSiblingElement("NebulaRouteGroup");
        }
        break;
    }
    return PointListVec;
}

CNebulaMissionPlatformVec ITaskManageImpl::GetAllMissionPlatform()
{
    QMutexLocker locker(&sMutex2);
    return m_MissionPlatformVec;
}

CNebulaMissionPlatform ITaskManageImpl::GetMissionPlatform(const std::string &strID)
{
    QMutexLocker locker(&sMutex2);
    CNebulaMissionPlatform stInfo;
    for (auto data : m_MissionPlatformVec)
    {
        if (0 == strcmp(strID.c_str(), data.strID))
        {
            stInfo = data;
            break;
        }
    }
    return stInfo;
}

CNebulaMissionPlatformVec ITaskManageImpl::GetMissionPlatformByMode(std::string &strID)
{
    QMutexLocker locker(&sMutex2);
    CNebulaMissionPlatformVec PlatformVec;
    for (auto data : m_MissionPlatformVec)
    {
        if (0 == strcmp(strID.c_str(), data.strMode))
        {
            PlatformVec.push_back(data);
        }
    }
    return PlatformVec;
}

CNebulaMissionPlatformVec qnzkna::TaskManage::ITaskManageImpl::GetMissionPlatformByMissionId(std::string &strID)
{
    QMutexLocker locker(&sMutex2);
    CNebulaMissionPlatformVec PlatformVec;
    for (auto data : m_MissionPlatformVec)
    {
        if (0 == strcmp(strID.c_str(), data.strMissionID))
        {
            PlatformVec.push_back(data);
        }
    }
    return PlatformVec;
}

bool ITaskManageImpl::SetMissionPlatform(CNebulaMissionPlatform &data)
{
    QMutexLocker locker(&sMutex2);
    auto itr = m_MissionPlatformVec.begin();
    for (; itr != m_MissionPlatformVec.end(); itr++)
    {
        if (0 == strcmp(itr->strID, data.strID))
        {
            EditPlatformDBData(data);
            *itr = data;
            break;
        }
    }

    if (itr == m_MissionPlatformVec.end())
    {
        m_MissionPlatformVec.push_back(data);
        AddPlatformDBData(data);
    }

    return true;
}

bool ITaskManageImpl::RmoveMissionPlatform(const std::string &strID)
{
    QMutexLocker locker(&sMutex2);

    auto itr = m_MissionPlatformVec.begin();
    for (; itr != m_MissionPlatformVec.end(); itr++)
    {
        if (0 == strcmp(itr->strID, strID.c_str()))
        {
            m_MissionPlatformVec.erase(itr);
            RemovePlatformDBData(strID);
            return true;
        }
    }
    return false;
}

bool ITaskManageImpl::RmoveAllMissionPlatform()
{
    QMutexLocker locker(&sMutex2);
    m_MissionPlatformVec.clear();
    return true;
}

bool ITaskManageImpl::IsMissionPlatformExisted(std::string &strID)
{
    QMutexLocker locker(&sMutex2);
    for (auto data : m_MissionPlatformVec)
    {
        if (0 == strcmp(strID.c_str(), data.strID))
        {
            return true;
        }
    }
    return false;
}

bool ITaskManageImpl::LoadMissionPlatformData()
{
    LoadPlatformDB();
    return true;

}

CNebulaMissionPlatformLoadVec ITaskManageImpl::GetAllMissionPlatformLoad()
{
    QMutexLocker locker(&sMutex3);
    return m_MissionPlatformLoadVec;
}

CNebulaMissionPlatformLoad ITaskManageImpl::GetMissionPlatformLoad(const std::string &strID)
{
    QMutexLocker locker(&sMutex3);
    CNebulaMissionPlatformLoad stInfo;
    for (auto data : m_MissionPlatformLoadVec)
    {
        if (0 == strcmp(strID.c_str(), data.strID))
        {
            stInfo = data;
            break;
        }
    }
    return stInfo;
}

bool ITaskManageImpl::GetMissionPlatformLoad(const QString &strMode, CNebulaMissionPlatformLoadVec &loadVec)
{
    QMutexLocker locker(&sMutex3);
    CNebulaMissionPlatformLoad stInfo;
    for (auto data : m_MissionPlatformLoadVec)
    {
        if (0 == strMode.compare(tr2(data.strMode)))
        {
            loadVec.push_back(data);
        }
    }
    return true;
}

bool ITaskManageImpl::SetMissionPlatformLoad(CNebulaMissionPlatformLoad &data)
{
    QMutexLocker locker(&sMutex3);
    auto itr = m_MissionPlatformLoadVec.begin();
    for (; itr != m_MissionPlatformLoadVec.end(); itr++)
    {
        if (0 == strcmp(itr->strID, data.strID))
        {
            EditPlatformLoadDBData(data);
            *itr = data;
            break;
        }
    }

    if (itr == m_MissionPlatformLoadVec.end())
    {
        m_MissionPlatformLoadVec.push_back(data);
        AddPlatformLoadDBData(data);
    }

    return true;
}

bool ITaskManageImpl::RmoveMissionPlatformLoad(const std::string &strID)
{
    QMutexLocker locker(&sMutex3);

    auto itr = m_MissionPlatformLoadVec.begin();
    for (; itr != m_MissionPlatformLoadVec.end(); itr++)
    {
        if (0 == strcmp(itr->strID, strID.c_str()))
        {
            m_MissionPlatformLoadVec.erase(itr);
            RemovePlatformLoadDBData(strID);
            return true;
        }
    }
    return false;
}

bool ITaskManageImpl::RmoveAllMissionPlatformLoad()
{
    QMutexLocker locker(&sMutex3);
    m_MissionPlatformLoadVec.clear();
    return true;
}

bool ITaskManageImpl::IsMissionPlatformLoadExisted(const string &strID)
{
    QMutexLocker locker(&sMutex3);
    CNeubulaMission stInfo;
    for (auto data : m_MissionPlatformLoadVec)
    {
        if (0 == strcmp(strID.c_str(), data.strID))
        {
            return true;
        }
    }
    return false;
}

bool ITaskManageImpl::LoadMissionPlatformLoadData()
{
    LoadPlatformLoadDB();
    return true;

}

CNeubulaActionVec  ITaskManageImpl::GetAllNebulaAction()
{
    LoadRelationPtDB();
    LoadRelationSensorDB();
    LoadRelationAmmoDB();
    LoadActionformDB();
    LoadRelationMarkDB();
    QMutexLocker locker(&sMutex4);
    return m_NeubulaActionVec;
}

CNeubulaAction  ITaskManageImpl::GetNebulaAction(int &nID)
{
    QMutexLocker locker(&sMutex4);
    CNeubulaAction stInfo;
    for (auto data : m_NeubulaActionVec)
    {
        if (data.GetID() == nID)
        {
            stInfo = data;
            return stInfo;
        }
    }
    return stInfo;
}

bool ITaskManageImpl::SetNebulaAction(CNeubulaAction &data)
{
    QMutexLocker locker(&sMutex4);

    auto itr = m_NeubulaActionVec.begin();
    for (; itr != m_NeubulaActionVec.end(); itr++)
    {
        if (data.GetID() == itr->GetID())
        {
            EditActionDBData(data);
            *itr = data;
            break;
        }
    }

    if (itr == m_NeubulaActionVec.end())
    {
        m_NeubulaActionVec.push_back(data);
        AddActionDBData(data);
    }
    return true;
}

bool  ITaskManageImpl::RmoveNebulaAction(int &nID)
{
    QMutexLocker locker(&sMutex4);

    auto itr = m_NeubulaActionVec.begin();
    for (; itr != m_NeubulaActionVec.end(); itr++)
    {
        if (itr->GetID() == nID)
        {
            m_NeubulaActionVec.erase(itr);
            RemoveActionDBData(nID);
            return true;
        }
    }
    return false;
}

bool  ITaskManageImpl::RmoveAllNebulaAction()
{
    QMutexLocker locker(&sMutex4);
    m_NeubulaActionVec.clear();
    return true;
}

bool  ITaskManageImpl::IsNebulaActionExisted(int &nID)
{
    QMutexLocker locker(&sMutex4);
    CNeubulaMission stInfo;
    for (auto data : m_NeubulaActionVec)
    {
        if (nID == data.GetID())
        {
            return true;
        }
    }
    return false;
}

bool ITaskManageImpl::SetActivatingActionID(int &nID)
{
    m_ActivatedActionID = nID;

    for (auto &data : m_NeubulaActionVec)
    {
        if (nID != data.GetID())
        {
            data.SetActivateStates(false);
        }
    }

    SetActionActivateStatesDB(nID);

    INebulaTaskManageSubject::Notify();

    return true;
}

int ITaskManageImpl::GetActivatingActionID()
{
    LoadActionformDB();
    return m_ActivatedActionID;
}

int ITaskManageImpl::GetActivatingActionIDFormDB()
{
    LoadActivatedActionIDFormDB();
    return m_ActivatedActionID;
}

int ITaskManageImpl::GetSelectedActivatingActionIDLocal()
{
    QMutexLocker locker(&sMutex5);
    return m_ActivatedActionID;
}

CRelationActionMarkVec ITaskManageImpl::GetAllRelationMark()
{
    QMutexLocker locker(&sMutex5);
    return m_RelationActionMarkVec;
}

CRelationActionMarkVec ITaskManageImpl::GetRelationMark(const int &nActionID)
{

    CRelationActionMarkVec NewVec;
    for (auto data : m_RelationActionMarkVec)
    {
        if (data.GetActionID() == nActionID)
        {
            NewVec.push_back(data);
        }
    }
    return NewVec;
}
bool ITaskManageImpl::RmoveRelationMark(CRelationActionMark &data)
{
    QMutexLocker locker(&sMutex5);
    if (IsRelationMarkExisted(data))
    {
        auto itr = m_RelationActionMarkVec.begin();
        for (; itr != m_RelationActionMarkVec.end(); itr++)
        {
            if ((data.GetActionID() == itr->GetActionID()) &&
                    (0 == strcmp(data.GetMarkID().c_str(), itr->GetMarkID().c_str()))
               )
            {
                m_RelationActionMarkVec.erase(itr);
                break;
            }
        }

    }
    RemoveRelationMarkDBData(data);
    return true;
}
bool ITaskManageImpl::RmoveRelationMark(std::vector<CRelationActionMark> &data)
{
    QMutexLocker locker(&sMutex5);
    for (auto target : data)
    {
        if (IsRelationMarkExisted(target))
        {
            auto itr = m_RelationActionMarkVec.begin();
            for (; itr != m_RelationActionMarkVec.end(); itr++)
            {
                if ((target.GetActionID() == itr->GetActionID()) &&
                        (0 == strcmp(target.GetMarkID().c_str(), itr->GetMarkID().c_str()))
                   )
                {
                    m_RelationActionMarkVec.erase(itr);
                    break;
                }
            }

        }
        RemoveRelationMarkDBData(target);
    }
    return true;
}

bool ITaskManageImpl::AddRelationMark(std::vector<std::string> strMark)
{
    QMutexLocker locker(&sMutex5);

    int nId = GetMaxRelationMarkID();
    for (auto target : strMark)
    {
        CRelationActionMark stRelation;
        stRelation.SetID(nId);
        stRelation.SetActionID(m_ActivatedActionID);
        stRelation.SetMarkID(target);

        if (IsRelationMarkExisted(stRelation) == true)
        {
            return false;
        }

        m_RelationActionMarkVec.push_back(stRelation);

        AddRelationMarkDBData(stRelation);
    }
    return true;
}
bool ITaskManageImpl::AddRelationMark(std::string strMark)
{
    QMutexLocker locker(&sMutex5);

    int nId = GetMaxRelationMarkID();
    CRelationActionMark stRelation;
    stRelation.SetID(nId);
    stRelation.SetActionID(m_ActivatedActionID);
    stRelation.SetMarkID(strMark);

    if (IsRelationMarkExisted(stRelation) == true)
    {
        return false;
    }

    m_RelationActionMarkVec.push_back(stRelation);

    AddRelationMarkDBData(stRelation);

    return true;
}
bool ITaskManageImpl::IsRelationMarkExisted(CRelationActionMark &data)
{
    for (auto dataRelation : m_RelationActionMarkVec)
    {
        if ((dataRelation.GetActionID() == data.GetActionID()) &&
                (0 == strcmp(dataRelation.GetMarkID().c_str(), data.GetMarkID().c_str()))
           )
        {
            return true;
        }
    }
    return false;
}

CRelationActionTaskVec ITaskManageImpl::GetAllRelationTask()
{
    LoadRelationTask();
    QMutexLocker locker(&sMutex7);
    return m_RelationActionTaskVec;
}

CRelationActionTaskVec ITaskManageImpl::GetAllRelationTaskByAction(const int nActionID)
{
    QMutexLocker locker(&sMutex7);
    CRelationActionTaskVec RelationActionTaskVecNew;
    for (auto data : m_RelationActionTaskVec)
    {
        if (data.GetActionID() == nActionID)
        {
            RelationActionTaskVecNew.push_back(data);
        }
    }
    return RelationActionTaskVecNew;
}

bool ITaskManageImpl::AddRelationTask(const CRelationActionTask stRelation)
{
    QMutexLocker locker(&sMutex7);

    int nID = stRelation.GetID();
    if (IsRelationTaskExisted(nID))
    {
        return false;
    }

    m_RelationActionTaskVec.push_back(stRelation);

    bool bresult = AddRelationTaskDBData(stRelation);
    return bresult;

}

bool ITaskManageImpl::EditRelationTask(const CRelationActionTask stRelation)
{
    QMutexLocker locker(&sMutex7);

    int nID = stRelation.GetID();
    if (!IsRelationTaskExisted(nID))
    {
        return false;
    }

    for (auto &data : m_RelationActionTaskVec)
    {
        if (data.GetID() == stRelation.GetID())
        {
            data = stRelation;
        }
    }

    bool bresult = EditRelationTaskDBData(stRelation);
    return bresult;
}

bool ITaskManageImpl::RmoveRelationTask(const int nID)
{
    QMutexLocker locker(&sMutex7);

    if (!IsRelationTaskExisted(nID))
    {
        return false;
    }

    bool bresult = RemoveRelationTaskDBData(nID);
    return bresult;
}

bool ITaskManageImpl::RmoveRelationTaskLine(const int nActionID, const std::string strtaskID, const std::string strLineID, const std::string strNewLineID)
{
    QMutexLocker locker(&sMutex7);

    for (auto &data : m_RelationActionTaskVec)
    {
        if (data.GetActionID() == nActionID)
        {
            if (0 == strcmp(data.GetLineID().c_str(), strLineID.c_str()))
            {
                data.SetLineID(strNewLineID);

                EditRelationTaskDBData_LineID(data.GetID(), strNewLineID);
            }
        }
    }
    return true;
}

bool ITaskManageImpl::IsRelationTaskExisted(const int nID)
{
    for (auto data : m_RelationActionTaskVec)
    {
        if (data.GetID() == nID)
        {
            return true;
        }
    }
    return false;
}

CNebulaAcitonRelationVec ITaskManageImpl::GetActivatingShowData()
{
    UpdateRelationTaskShowData();
    return m_AcitonRelationShowVec;
}

CCodeSet_Load_DB_Vec ITaskManageImpl::GetAllCodeSet_Load()
{
    LoadCodeSet_Load_DB();
    return m_CodeSet_LoadVec_Activating;
}

CCodeSet_Load_DB ITaskManageImpl::GetCodeSet_Load(const int nID)
{
    QMutexLocker locker(&sMutex6);
    CCodeSet_Load_DB stCodeSet;
    for (auto data : m_CodeSet_LoadVec_Activating)
    {
        if (data.GetID() == nID)
        {
            stCodeSet = data;
            break;
        }
    }
    return stCodeSet;
}

CCodeSet_Load_DB_Vec ITaskManageImpl::GetCodeSet_Load(const std::string strLineID)
{
    QMutexLocker locker(&sMutex6);
    CCodeSet_Load_DB_Vec CodeSetVec;
    for (auto data : m_CodeSet_LoadVec_Activating)
    {
        if (0 == strcmp(data.GetLineID().c_str(), strLineID.c_str()))
        {
            CodeSetVec.push_back(data);
        }
    }
    return CodeSetVec;

}

CCodeSet_Load_DB_Vec ITaskManageImpl::GetCodeSet_Load(const std::string strTaskID, const std::string strPlatformID)
{
    QMutexLocker locker(&sMutex6);
    CCodeSet_Load_DB_Vec CodeSetVec;
    for (auto data : m_CodeSet_LoadVec_Activating)
    {
        if (0 == strcmp(data.GetTaskID().c_str(), strTaskID.c_str()) &&
                0 == strcmp(data.GetPlatformID().c_str(), strPlatformID.c_str()))
        {
            CodeSetVec.push_back(data);
        }
    }
    return CodeSetVec;
}

CCodeSet_Load_DB_Vec ITaskManageImpl::GetCodeSet_Load(const std::string strTaskID, const std::string strPlatformID, const int nPointID)
{
    QMutexLocker locker(&sMutex6);
    CCodeSet_Load_DB_Vec CodeSetVec;
    for (auto data : m_CodeSet_LoadVec_Activating)
    {
        if (0 == strcmp(data.GetTaskID().c_str(), strTaskID.c_str()) &&
                0 == strcmp(data.GetPlatformID().c_str(), strPlatformID.c_str()) &&
                nPointID == data.GetPointID())
        {
            CodeSetVec.push_back(data);
        }
    }
    return CodeSetVec;
}

bool ITaskManageImpl::RmoveCodeSet_Load(const int nID)
{
    QMutexLocker locker(&sMutex6);
    if (!IsCodeSet_LoadExisted(nID))
    {
        return false;
    }
    RemoveCodeSet_Load_DB(nID);

    auto itr = m_CodeSet_LoadVec.begin();
    for (; itr != m_CodeSet_LoadVec.end(); itr++)
    {
        if (nID == itr->GetID())
        {
            m_CodeSet_LoadVec.erase(itr);
            break;
        }
    }
    locker.unlock();

    UpdateCodeSet_Load_Activated();
    return true;
}

bool ITaskManageImpl::RmoveCodeSet_Load(const std::string strTaskID, const std::string strPlatformID, const std::string strLoad, const int nPoint)
{
    QMutexLocker locker(&sMutex6);

    auto itr = m_CodeSet_LoadVec.begin();
    for (; itr != m_CodeSet_LoadVec.end(); itr++)
    {
        if (0 == strcmp(itr->GetTaskID().c_str(), strTaskID.c_str()) &&
                0 == strcmp(itr->GetPlatformID().c_str(), strPlatformID.c_str()) &&
                0 == strcmp(itr->GetLoadID().c_str(), strLoad.c_str()) &&
                itr->GetPointID() == nPoint)
        {
            RemoveCodeSet_Load_DB(itr->GetID());
        }
    }
    locker.unlock();
    LoadCodeSet_Load_DB();
    UpdateCodeSet_Load_Activated();
    return true;
}

bool ITaskManageImpl::SetCodeSet_Load(const CCodeSet_Load_DB dataTemp)
{
    QMutexLocker locker(&sMutex6);

    int nID = dataTemp.GetID();
    if (IsCodeSet_LoadExisted(nID))
    {
        for (auto &data : m_CodeSet_LoadVec)
        {
            if (data.GetID() == dataTemp.GetID())
            {
                data = dataTemp;
                EditLoadCodeSet_Load_DB(dataTemp);
                break;
            }
        }
    }
    else
    {
        m_CodeSet_LoadVec.push_back(dataTemp);
        AddLoadCodeSet_Load_DB(dataTemp);
    }

    locker.unlock();

    UpdateCodeSet_Load_Activated();
    return true;
}

bool ITaskManageImpl::IsCodeSet_LoadExisted(const int nID)
{
    for (auto data : m_CodeSet_LoadVec)
    {
        if (nID == data.GetID())
        {
            return true;
        }
    }
    return false;
}

int ITaskManageImpl::GetCodeSet_Load_MaxID()
{
    return GetMaxCodeSet_Load_ID_DB();
}

CNeubulaMissionLineVec ITaskManageImpl::GetAllMissionLine()
{
    QMutexLocker locker(&sMutex8);
    return m_NeubulaMissionLineVec;
}

bool qnzkna::TaskManage::ITaskManageImpl::RmoveCodeSet_Load(const std::string strTaskID, const std::string strPlatformID, const int nPoint)
{
    QMutexLocker locker(&sMutex6);

    auto itr = m_CodeSet_LoadVec.begin();
    for (; itr != m_CodeSet_LoadVec.end(); itr++)
    {
        if (0 == strcmp(itr->GetTaskID().c_str(), strTaskID.c_str()) &&
                0 == strcmp(itr->GetPlatformID().c_str(), strPlatformID.c_str()) &&
                itr->GetPointID() == nPoint)
        {
            RemoveCodeSet_Load_DB(itr->GetID());
        }
    }
    locker.unlock();
    LoadCodeSet_Load_DB();
    UpdateCodeSet_Load_Activated();
    return true;
}

bool qnzkna::TaskManage::ITaskManageImpl::RmoveCodeSet_Load(const std::string strTaskID, const std::string strPlatformID, const int nPoint, const int Order)
{
    QMutexLocker locker(&sMutex6);

    auto itr = m_CodeSet_LoadVec.begin();
    for (; itr != m_CodeSet_LoadVec.end(); itr++)
    {
        if (0 == strcmp(itr->GetTaskID().c_str(), strTaskID.c_str()) &&
                0 == strcmp(itr->GetPlatformID().c_str(), strPlatformID.c_str()) &&
                itr->GetSuite() == Order &&
                itr->GetPointID() == nPoint)
        {
            RemoveCodeSet_Load_DB(itr->GetID());
        }
    }
    locker.unlock();
    LoadCodeSet_Load_DB();
    UpdateCodeSet_Load_Activated();
    return true;
}

bool qnzkna::TaskManage::ITaskManageImpl::SetCodeSet_Load1(const CCodeSet_Load_DB dataTime)
{
    QMutexLocker locker(&sMutex6);

    int nID = dataTime.GetID();
    if (IsCodeSet_LoadExisted(nID))
    {
        for (auto &data : m_CodeSet_LoadVec)
        {
            if (data.GetID() == dataTime.GetID())
            {
                data = dataTime;
                EditLoadCodeSet_Load_DB(dataTime);
                break;
            }
        }
    }
    else
    {
        m_CodeSet_LoadVec.push_back(dataTime);
        AddLoadCodeSet_Load_DB1(dataTime);
    }

    locker.unlock();

    UpdateCodeSet_Load_Activated();
    return true;
}

void qnzkna::TaskManage::ITaskManageImpl::LoadMissionLine()
{
    LoadLineDB();
}

CNeubulaMissionLineVec qnzkna::TaskManage::ITaskManageImpl::GetMissionLinesByTask(const std::string &taskID)
{

    LoadLineDBByTask(taskID);
    CNeubulaMissionLineVec reLines;
    reLines.clear();
    for (auto line : m_NeubulaMissionLineVec)
    {
        char tmpTaskID[MAX_MISSION_LEN_1];

		if (0 == strcmp(QString::fromStdString(taskID).toLocal8Bit(), line.strMissionID))
        {
            reLines.push_back(line);
        }
    }
    return reLines;
}

CNeubulaMissionLine ITaskManageImpl::GetMissionLine(const int nID)
{

    LoadLineDB();
    CNeubulaMissionLine stline;
    for (auto itr : m_NeubulaMissionLineVec)
    {
        if (itr.nID == nID)
        {
            stline = itr;
            break;
        }
    }
    return stline;
}

int ITaskManageImpl::GetMissionLineID(const std::string strTaskID, const std::string strPlatform)
{
    CNebulaAcitonRelationVec RelationVec = GetActivatingShowData();
    int nLineId = 0;
    for (auto itr : RelationVec)
    {
        if (itr.nActionID == m_ActivatedActionID)
        {
            for (auto task : itr.ReTaskVec)
            {
                if (0 == strcmp(task.strTaskID.c_str(), strTaskID.c_str()))
                {
                    for (auto platform : task.PlatFormVec)
                    {
                        if (0 == strcmp(platform.strPlatformID.c_str(), strPlatform.c_str()))
                        {
                            std::string strLineID = platform.strLineID;
                            nLineId = atoi(strLineID.c_str());
                            break;
                        }
                    }
                    break;
                }
            }
            break;
        }
    }
    return nLineId;

}

bool ITaskManageImpl::AddMissionLine(const CNeubulaMissionLine stLine)
{
    QMutexLocker locker(&sMutex8);

    if (IsMissionLineExited(stLine.nID) == true)
    {
        return false;
    }

    m_NeubulaMissionLineVec.push_back(stLine);
    bool bresult = AddlineDB(stLine);
    return bresult;
}

bool ITaskManageImpl::EditMissionLine(const CNeubulaMissionLine stLine)
{
    QMutexLocker locker(&sMutex8);

    auto itr = m_NeubulaMissionLineVec.begin();
    for (; itr != m_NeubulaMissionLineVec.end(); itr++)
    {
        if (itr->nID == stLine.nID)
        {
            *itr = stLine;
            EditLineDB(stLine);
            return true;
        }
    }
    return false;
}

bool ITaskManageImpl::RemoveMissionLine(const int nID)
{
    QMutexLocker locker(&sMutex8);
    auto itr = m_NeubulaMissionLineVec.begin();
    for (; itr != m_NeubulaMissionLineVec.end(); itr++)
    {
        if (itr->nID == nID)
        {
            m_NeubulaMissionLineVec.erase(itr);
            RemovelineDB(nID);
            return true;
        }
    }
    return false;
}

bool ITaskManageImpl::IsMissionLineExited(const int nID)
{
    auto itr = m_NeubulaMissionLineVec.begin();
    for (; itr != m_NeubulaMissionLineVec.end(); itr++)
    {
        if (itr->nID == nID)
        {
            return true;
        }
    }
    return false;
}

CNeubulaMissionLinePointVec ITaskManageImpl::GetMissionLinePoints(const int nID)
{
    QMutexLocker locker(&sMutex8);
    CNeubulaMissionLinePointVec  PointsVec;
    for (auto itr : m_NeubulaMissionLineVec)
    {
        if (itr.nID == nID)
        {
            PointsVec = itr.LinePointVec;
            break;
        }
    }
    return  PointsVec;
}

CNeubulaMissionLinePoint ITaskManageImpl::GetMissionLinePoint(const int nID, const int nPoint)
{
    CNeubulaMissionLinePoint  LinePoint;

    if (nPoint < 1)
    {
        return LinePoint;
    }

    QMutexLocker locker(&sMutex8);
    for (auto itr : m_NeubulaMissionLineVec)
    {
        if (itr.nID == nID)
        {
            auto itrpoint = itr.LinePointVec.begin();
            if (nPoint  > itr.LinePointVec.size())
            {
                return LinePoint;
            }
            itrpoint += nPoint - 1;
            LinePoint = *itrpoint;
            break;
        }
    }
    return  LinePoint;
}

bool ITaskManageImpl::AddMissionLinePoint(const CNeubulaMissionLinePoint stPoint)
{
    QMutexLocker locker(&sMutex8);
    for (auto &itr : m_NeubulaMissionLineVec)
    {
        if (itr.nID == stPoint.nLineID)
        {
            itr.LinePointVec.push_back(stPoint);
            AddLinePointDB(stPoint);
            return true;
        }
    }
    return false;
}

bool ITaskManageImpl::RemoveMissionLinePoints(const int nLineID)
{
    QMutexLocker locker(&sMutex8);
    for (auto &itr : m_NeubulaMissionLineVec)
    {
        if (itr.nID == nLineID)
        {
            itr.LinePointVec.clear();
            RemoveLinePointDB(nLineID);
            return true;
        }
    }
    return false;
}

bool ITaskManageImpl::RemoveMissionLinePoint(const int nLineID, const int nPoint)
{
    QMutexLocker locker(&sMutex8);
    for (auto &itr : m_NeubulaMissionLineVec)
    {
        if (itr.nID == nLineID)
        {
            auto itrPoint = itr.LinePointVec.begin();
            if (nPoint > itr.LinePointVec.size())
            {
                return false;
            }
            itrPoint += nPoint;

            RemoveLinePointDB(nLineID, itrPoint->nNo);

            itr.LinePointVec = getMissionLinePointsFornLineID(itr.nID);
            return true;
        }
    }
    return false;
}

bool ITaskManageImpl::InsertMissionLinePoint(const int nNo, const CNeubulaMissionLinePoint stPoint)
{
    QMutexLocker locker(&sMutex8);
    for (auto &itr : m_NeubulaMissionLineVec)
    {
        if (itr.nID == stPoint.nLineID)
        {

            InsertLinePointDB(stPoint);
            itr.LinePointVec = getMissionLinePointsFornLineID(itr.nID);
            return true;
        }
    }
    return false;
}

bool ITaskManageImpl::IsMissionLinePointExited(const int nLineID, const int nPoint)
{
    return false;
}

void ITaskManageImpl::SetMissionLiseShowStates(const int nLineID, const bool bShow)
{
    QMutexLocker locker(&sMutex8);
    for (auto &data : m_NeubulaMissionLineVec)
    {
        if (nLineID == data.nID)
        {
            data.cShowFlag = bShow;
            SetLineShowStatesDB(nLineID, bShow);
            break;
        }
    }
}

void qnzkna::TaskManage::ITaskManageImpl::updatePointsIndexFormNow(const int nowIndex, const int nLineID, const int disNum)
{

    int maxIndex = GetMissionLinePointMaxID();

    int curDis = disNum;

    if (curDis < 0)
    {
        int curIndex = nowIndex;
        while (curIndex <= maxIndex)
        {
            if (IsLinePointExistedDB(nLineID, curIndex))
            {

                QString updateStr = ("UPDATE RWSJ_HDLB set No = ");
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

                QString updateStr = ("UPDATE RWSJ_HDLB set No = ");
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

bool qnzkna::TaskManage::ITaskManageImpl::IsTaskInAction(const std::string strTaskID)
{
    if (m_AcitonRelationShowVec.size() == 0)
    {
        return false;
    }

    for (auto itr : m_AcitonRelationShowVec)
    {
        if (itr.nActionID != m_ActivatedActionID)
        {
            return false;
        }

        for (auto task : itr.ReTaskVec)
        {
            if (0 == strcmp(task.strTaskID.c_str(), strTaskID.c_str()))
            {
                return true;
            }
        }
        return false;
    }
    return false;
}

bool qnzkna::TaskManage::ITaskManageImpl::AddLoadCodeSet_Load_DB1(const CCodeSet_Load_DB data)
{
    int nID = data.GetID();

    if (IsCodeSetIDExisted_Load_DB(nID))
    {
        return false;
    }

    QString strSql("INSERT INTO ZLJ_CGQ (`BH`, `XDBH`, `RWBH`, `HXBH`, `PTBH`, `GZBH`, `HDBH`,`HDJD`, `HDWD`, `HDGD`, `ZLSX`, `ZLBH`, `ZLCS`, `BZ`) VALUES (");
    strSql += QString::number(data.GetID()) + ",";
    strSql += QString::number(data.GetActionID()) + ",";
    strSql += "'" + QString::fromStdString(data.GetTaskID()) + "',";
    strSql += "'" + QString::fromStdString(data.GetLineID()) + "',";
    strSql += "'" + QString::fromStdString(data.GetPlatformID()) + "',";
    strSql += "'" + QString::fromStdString(data.GetLoadID()) + "',";
    strSql += QString::number(data.GetPointID()) + ",";
    strSql += QString::number(data.GetLon(), 'f', 8) + ",";
    strSql += QString::number(data.GetLat(), 'f', 8) + ",";
    strSql += QString::number(data.GetAlt()) + ",";
    strSql += QString::number(data.GetSuite()) + ",";
    strSql += QString::number(data.GetCodeID()) + ",";
    strSql += "'" + QByteArray::fromStdString(data.GetParameter()) + "',";
    strSql += "'" + QString::fromStdString(data.GetRemark()) + "'";
    strSql += ")";
    QSqlQuery query;
    bool success = query.exec(strSql);

    return success;
}

void qnzkna::TaskManage::ITaskManageImpl::LoadRelationMissionPtDB()
{
    QMutexLocker locker(&sReMissionPT);

    QSqlQuery query;
    bool success = query.exec("SELECT `BH`, `RWBH`, `PTMC`, `PTSL`,`ZDCLX`,`CDCLX`,`ZGZWQ`,`ZGZSL`,`CGZWQ`,`CGZSL` FROM `Relation_RW_PTZY`");
    if (success)
    {
        m_RelationMissionPtVec.clear();

        while (query.next())
        {
			QString str1 = tr2(query.value(0).toString().toLatin1().data());
			QString str2 = tr2(query.value(1).toString().toLocal8Bit().data());
			QString str3 = tr2(query.value(2).toString().toLocal8Bit().data());
			QString str4 = tr2(query.value(3).toString().toLatin1().data());

			QString str5 = tr2(query.value(4).toString().toLocal8Bit().data());
			QString str6 = tr2(query.value(5).toString().toLocal8Bit().data());
			QString str7 = tr2(query.value(6).toString().toLocal8Bit().data());
			QString str8 = tr2(query.value(7).toString().toLatin1().data());
			QString str9 = tr2(query.value(8).toString().toLocal8Bit().data());
			QString str10 = tr2(query.value(9).toString().toLatin1().data());
            CRelationMissionResource stRelation;
            stRelation.SetID(str1.toInt());
            stRelation.SetMissionID(str2.toStdString());
            stRelation.SetResourceName(str3.toStdString());
            stRelation.SetResourceNum(str4.toInt());
            stRelation.SetPrimaryPod(str5.toStdString());
            stRelation.SetSecondPod(str6.toStdString());
            stRelation.SetPriMountType(str7.toStdString());
            stRelation.SetPriMountNum(str8.toInt());
            stRelation.SetSecMountType(str9.toStdString());
            stRelation.SetSecMountNum(str10.toInt());
            m_RelationMissionPtVec.push_back(stRelation);
        }
    }

    UpdataMissionPtList();
}

void qnzkna::TaskManage::ITaskManageImpl::LoadRelationMissionPtDBre()
{

    QSqlQuery query;
    bool success = query.exec("SELECT `BH`, `RWBH`, `PTMC`, `PTSL`,`ZDCLX`,`CDCLX`,`ZGZWQ`,`ZGZSL`,`CGZWQ`,`CGZSL` FROM `Relation_RW_PTZY`");
    if (success)
    {
        m_RelationMissionPtVec.clear();

        while (query.next())
        {
			QString str1 = tr2(query.value(0).toString().toLatin1().data());
			QString str2 = tr2(query.value(1).toString().toLocal8Bit().data());
			QString str3 = tr2(query.value(2).toString().toLocal8Bit().data());
			QString str4 = tr2(query.value(3).toString().toLatin1().data());

			QString str5 = tr2(query.value(4).toString().toLocal8Bit().data());
			QString str6 = tr2(query.value(5).toString().toLocal8Bit().data());
			QString str7 = tr2(query.value(6).toString().toLocal8Bit().data());
			QString str8 = tr2(query.value(7).toString().toLatin1().data());
			QString str9 = tr2(query.value(8).toString().toLocal8Bit().data());
			QString str10 = tr2(query.value(9).toString().toLatin1().data());
            CRelationMissionResource stRelation;
            stRelation.SetID(str1.toInt());
            stRelation.SetMissionID(str2.toStdString());
            stRelation.SetResourceName(str3.toStdString());
            stRelation.SetResourceNum(str4.toInt());
            stRelation.SetPrimaryPod(str5.toStdString());
            stRelation.SetSecondPod(str6.toStdString());
            stRelation.SetPriMountType(str7.toStdString());
            stRelation.SetPriMountNum(str8.toInt());
            stRelation.SetSecMountType(str9.toStdString());
            stRelation.SetSecMountNum(str10.toInt());
            m_RelationMissionPtVec.push_back(stRelation);
        }
    }
}

bool qnzkna::TaskManage::ITaskManageImpl::AddRelationMissionPtDBData(CRelationMissionResource &data)
{
    QMutexLocker locker(&sReMissionPT);
    QString strSql("INSERT INTO Relation_RW_PTZY (`BH`, `RWBH`, `PTMC`, `PTSL`, `ZDCLX`, `CDCLX`, `ZGZWQ`, `ZGZSL`, `CGZWQ`, `CGZSL`) VALUES (");
    strSql += QString::number(data.GetID()) + ",";
    strSql += "'" + QString::fromStdString(data.GetMissionID()) + "'" + ",";
    strSql += "'" + QString::fromStdString(data.GetResourceName()) + "'" + ",";
    strSql += QString::number(1) + ",";
    strSql += "'" + QString::fromStdString(data.GetPrimaryPoc().c_str()) + "'" + ",";
    strSql += "'" + QString::fromStdString(data.GetSecondPod()) + "'" + ",";
    strSql += "'" + QString::fromStdString(data.GetPriMountType()) + "'" + ",";
    strSql += QString::number(data.GetPriMountNum()) + ",";
    strSql += "'" + QString::fromStdString(data.GetSecMountType()) + "'" + ",";
    strSql += QString::number(data.GetSecMountNum());
    strSql += ")";
    QSqlQuery query;
    bool success = query.exec(strSql);
    return success;
}

bool qnzkna::TaskManage::ITaskManageImpl::RemoveRelationMissionPtDBData(std::string strMissionID)
{
    QMutexLocker locker(&sReMissionPT);

    QString strSql("DELETE FROM Relation_RW_PTZY where RWBH = ");
    strSql += "'" + QString::fromStdString(strMissionID) + "'";
    QSqlQuery query;
    bool success = query.exec(strSql);
    return success;
}

int qnzkna::TaskManage::ITaskManageImpl::GetMaxRelationMissionPtID()
{
    QSqlQuery query;
    int nNoMax = 0;
    bool success = query.exec("SELECT MAX(`BH`) from Relation_RW_PTZY");
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

void qnzkna::TaskManage::ITaskManageImpl::UpdataMissionPtList()
{
    if (m_NeubulaMissionVec.size() == 0)
    {
        return;
    }

    for (auto &data : m_NeubulaMissionVec)
    {
        CRelationMissionResourceVec ResourceVec = GetRelationMissionPtResource(data.GetID());

        data.SetPlatInfoMap(ResourceVec);
    }
}

void qnzkna::TaskManage::ITaskManageImpl::LoadRelationPtDB()
{
    QMutexLocker locker(&sRsMutex4);

    QSqlQuery query;
    bool success = query.exec("SELECT `BH`, `XDBH`, `PTMC`, `PTSL` FROM `Relation_XD_PTZY`");
    if (success)
    {
        m_RelationActionPtVec.clear();

        while (query.next())
        {
			QString str1 = tr2(query.value(0).toString().toLatin1().data());
			QString str2 = tr2(query.value(1).toString().toLatin1().data());
			QString str3 = tr2(query.value(2).toString().toLocal8Bit().data());
			QString str4 = tr2(query.value(3).toString().toLatin1().data());

            CRelationActionResource stRelation;
            stRelation.SetID(str1.toInt());
            stRelation.SetActionID(str2.toInt());
            stRelation.SetResourceName(str3.toStdString());
            stRelation.SetResourceNum(str4.toInt());

            m_RelationActionPtVec.push_back(stRelation);
        }
    }

    UpdataAcitonPtList();
}

void qnzkna::TaskManage::ITaskManageImpl::LoadRelationSensorDB()
{
    QMutexLocker locker(&sRsMutex5);

    QSqlQuery query;
    bool success = query.exec("SELECT `BH`, `XDBH`, `CGQMC`, `CGQSL` FROM `Relation_XD_CGQZY`");
    if (success)
    {
        m_RelationActionSensorVec.clear();

        while (query.next())
        {
			QString str1 = tr2(query.value(0).toString().toLatin1().data());
			QString str2 = tr2(query.value(1).toString().toLatin1().data());
			QString str3 = tr2(query.value(2).toString().toLocal8Bit().data());
			QString str4 = tr2(query.value(3).toString().toLatin1().data());

            CRelationActionResource stRelation;
            stRelation.SetID(str1.toInt());
            stRelation.SetActionID(str2.toInt());
            stRelation.SetResourceName(str3.toStdString());
            stRelation.SetResourceNum(str4.toInt());

            m_RelationActionSensorVec.push_back(stRelation);
        }
    }

    UpdataAcitonSensorList();
}

void qnzkna::TaskManage::ITaskManageImpl::LoadRelationAmmoDB()
{
    QMutexLocker locker(&sRsMutex5);

    QSqlQuery query;
    bool success = query.exec("SELECT `BH`, `XDBH`, `DYMC`, `DYSL` FROM `Relation_XD_DYZY`");
    if (success)
    {
        m_RelationActionAmmoVec.clear();

        while (query.next())
        {
			QString str1 = tr2(query.value(0).toString().toLatin1().data());
			QString str2 = tr2(query.value(1).toString().toLatin1().data());
			QString str3 = tr2(query.value(2).toString().toLocal8Bit().data());
			QString str4 = tr2(query.value(3).toString().toLatin1().data());

            CRelationActionResource stRelation;
            stRelation.SetID(str1.toInt());
            stRelation.SetActionID(str2.toInt());
            stRelation.SetResourceName(str3.toStdString());
            stRelation.SetResourceNum(str4.toInt());

            m_RelationActionAmmoVec.push_back(stRelation);
        }
    }

    UpdataAcitonAmmoList();
}

bool qnzkna::TaskManage::ITaskManageImpl::EditRelationPtDBData(CRelationActionResource &data)
{

    if (!IsRelationPtDBExisted(data))
    {
        return false;
    }

    QString strSql("UPDATE Relation_XD_PTZY set ");
    strSql += "`XDBH` = " + QString::number(data.GetActionID()) + ",";
    strSql += "`PTMC` = '" + QString::fromStdString(data.GetResourceName()) + "',";
    strSql += "`PTSL` = '" + QString::number(data.GetResourceNum()) + "'" + ",";
    strSql += "where `BH` = " + QString::number(data.GetID());

    QSqlQuery query;
    bool success = query.exec(strSql);
    return success;
}

bool qnzkna::TaskManage::ITaskManageImpl::EditRelationSensorDBData(CRelationActionResource &data)
{

    if (!IsRelationSensorDBExisted(data))
    {
        return false;
    }

    QString strSql("UPDATE Relation_XD_CGQZY set ");
    strSql += "`XDBH` = " + QString::number(data.GetActionID()) + ",";
    strSql += "`CGQMC` = '" + QString::fromStdString(data.GetResourceName()) + "',";
    strSql += "`CGQSL` = '" + QString::number(data.GetResourceNum()) + "'" + ",";
    strSql += "where `BH` = " + QString::number(data.GetID());

    QSqlQuery query;
    bool success = query.exec(strSql);
    return success;
}

bool qnzkna::TaskManage::ITaskManageImpl::EditRelationAmmoDBData(CRelationActionResource &data)
{

    if (!IsRelationAmmoDBExisted(data))
    {
        return false;
    }

    QString strSql("UPDATE Relation_XD_DYZY set ");
    strSql += "`XDBH` = " + QString::number(data.GetActionID()) + ",";
    strSql += "`DYMC` = '" + QString::fromStdString(data.GetResourceName()) + "',";
    strSql += "`DYSL` = '" + QString::number(data.GetResourceNum()) + "'" + ",";
    strSql += "where `BH` = " + QString::number(data.GetID());

    QSqlQuery query;
    bool success = query.exec(strSql);
    return success;
}

bool qnzkna::TaskManage::ITaskManageImpl::AddRelationPtDBData(CRelationActionResource &data)
{

    if (IsRelationPtDBExisted(data))
    {
        return false;
    }

    QString strSql("INSERT INTO Relation_XD_PTZY (`BH`, `XDBH`, `PTMC`, `PTSL`) VALUES (");
    strSql += QString::number(data.GetID()) + ",";
    strSql += QString::number(data.GetActionID()) + ",";
    strSql += "'" + QString::fromStdString(data.GetResourceName()) + "'" + ",";
    strSql += "'" + QString::number(data.GetResourceNum()) + "'";
    strSql += ")";
    QSqlQuery query;
    bool success = query.exec(strSql);
    return success;
}

bool qnzkna::TaskManage::ITaskManageImpl::AddRelationSensorDBData(CRelationActionResource &data)
{

    if (IsRelationSensorDBExisted(data))
    {
        return false;
    }

    QString strSql("INSERT INTO Relation_XD_CGQZY (`BH`, `XDBH`, `CGQMC`, `CGQSL`) VALUES (");
    strSql += QString::number(data.GetID()) + ",";
    strSql += QString::number(data.GetActionID()) + ",";
    strSql += "'" + QString::fromStdString(data.GetResourceName()) + "'" + ",";
    strSql += "'" + QString::number(data.GetResourceNum()) + "'";
    strSql += ")";
    QSqlQuery query;
    bool success = query.exec(strSql);
    return success;
}

bool qnzkna::TaskManage::ITaskManageImpl::AddRelationAmmoDBData(CRelationActionResource &data)
{

    if (IsRelationAmmoDBExisted(data))
    {
        return false;
    }

    QString strSql("INSERT INTO Relation_XD_DYZY (`BH`, `XDBH`, `DYMC`, `DYSL`) VALUES (");
    strSql += QString::number(data.GetID()) + ",";
    strSql += QString::number(data.GetActionID()) + ",";
    strSql += "'" + QString::fromStdString(data.GetResourceName()) + "'" + ",";
    strSql += "'" + QString::number(data.GetResourceNum()) + "'";
    strSql += ")";
    QSqlQuery query;
    bool success = query.exec(strSql);
    return success;
}

bool qnzkna::TaskManage::ITaskManageImpl::RemoveRelationPtDBData(CRelationActionResource &data)
{

    if (!IsRelationPtDBExisted(data))
    {
        return false;
    }

    QString strSql("DELETE FROM Relation_XD_PTZY where XDBH = ");
    strSql += QString::number(data.GetActionID()) + " ";
    strSql += "AND `PTMC` = '" + QString::fromStdString(data.GetResourceName()) + "'";
    QSqlQuery query;
    bool success = query.exec(strSql);
    return success;
}

bool qnzkna::TaskManage::ITaskManageImpl::RemoveRelationSensorDBData(CRelationActionResource &data)
{

    if (!IsRelationSensorDBExisted(data))
    {
        return false;
    }

    QString strSql("DELETE FROM Relation_XD_CGQZY where XDBH = ");
    strSql += QString::number(data.GetActionID()) + " ";
    strSql += "AND `CGQMC` = '" + QString::fromStdString(data.GetResourceName()) + "'";
    QSqlQuery query;
    bool success = query.exec(strSql);
    return success;
}

bool qnzkna::TaskManage::ITaskManageImpl::RemoveRelationAmmoDBData(CRelationActionResource &data)
{

    if (!IsRelationAmmoDBExisted(data))
    {
        return false;
    }

    QString strSql("DELETE FROM Relation_XD_DYZY where XDBH = ");
    strSql += QString::number(data.GetActionID()) + " ";
    strSql += "AND `DYMC` = '" + QString::fromStdString(data.GetResourceName()) + "'";
    QSqlQuery query;
    bool success = query.exec(strSql);
    return success;
}

bool qnzkna::TaskManage::ITaskManageImpl::IsRelationPtDBExisted(CRelationActionResource &data)
{
    QString strSql("SELECT  * FROM `Relation_XD_PTZY` WHERE `XDBH` = ");
    strSql += "'" + QString::number(data.GetActionID()) + "'";
    strSql += "AND `PTMC` = '" + QString::fromStdString(data.GetResourceName()) + "'";

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

bool qnzkna::TaskManage::ITaskManageImpl::IsRelationSensorDBExisted(CRelationActionResource &data)
{
    QString strSql("SELECT  * FROM `Relation_XD_CGQZY` WHERE `XDBH` = ");
    strSql += "'" + QString::number(data.GetActionID()) + "'";
    strSql += "AND `CGQMC` = '" + QString::fromStdString(data.GetResourceName()) + "'";

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

bool qnzkna::TaskManage::ITaskManageImpl::IsRelationAmmoDBExisted(CRelationActionResource &data)
{
    QString strSql("SELECT  * FROM `Relation_XD_DYZY` WHERE `XDBH` = ");
    strSql += "'" + QString::number(data.GetActionID()) + "'";
    strSql += "AND `DYMC` = '" + QString::fromStdString(data.GetResourceName()) + "'";

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

int qnzkna::TaskManage::ITaskManageImpl::GetMaxRelationPtID()
{
    QSqlQuery query;
    int nNoMax = 0;
    bool success = query.exec("SELECT MAX(`BH`) from Relation_XD_PTZY");
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

int qnzkna::TaskManage::ITaskManageImpl::GetMaxRelationSensorID()
{
    QSqlQuery query;
    int nNoMax = 0;
    bool success = query.exec("SELECT MAX(`BH`) from Relation_XD_CGQZY");
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

int qnzkna::TaskManage::ITaskManageImpl::GetMaxRelationAmmoID()
{
    QSqlQuery query;
    int nNoMax = 0;
    bool success = query.exec("SELECT MAX(`BH`) from Relation_XD_DYZY");
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

int qnzkna::TaskManage::ITaskManageImpl::GetMaxRelationTaskPtID()
{
    QSqlQuery query;
    int nNoMax = 0;
    bool success = query.exec("SELECT MAX(`BH`) from Relation_RW_PTZY");
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

void qnzkna::TaskManage::ITaskManageImpl::UpdataAcitonPtList()
{
    ResourcesVec ptVec;
    if (m_RelationActionPtVec.size() == 0)
    {
        return;
    }

    for (auto &data : m_NeubulaActionVec)
    {
        CRelationActionMarkVec MarkVec = GetRelationMark(data.GetID());
        MarkIDVec IDList;
        for (auto itr : MarkVec)
        {
            IDList.push_back(itr.GetMarkID());
        }
        data.SetMarkIDList(IDList);
    }
}

void qnzkna::TaskManage::ITaskManageImpl::UpdataAcitonSensorList()
{
    ResourcesVec sensorVec;

}

void qnzkna::TaskManage::ITaskManageImpl::UpdataAcitonAmmoList()
{
    ResourcesVec AmmoVec;

}

void qnzkna::TaskManage::ITaskManageImpl::ReLoadRelationMarkDB()
{
    LoadRelationMarkDB();
}

void qnzkna::TaskManage::ITaskManageImpl::ReLoadRelationMissionMarkDB()
{
    LoadRelationMissionMarkDB();
}

CRelationMissionMarkVec qnzkna::TaskManage::ITaskManageImpl::GetAllRelationMissionMark()
{
    return m_RelationMissionMarkVec;
}

CRelationMissionMarkVec qnzkna::TaskManage::ITaskManageImpl::GetRelationMissionMark(const std::string &strID)
{
    CRelationMissionMarkVec NewVec;
    for (auto data : m_RelationMissionMarkVec)
    {
        if (0 == strcmp(strID.c_str(), data.GetMissionID().c_str()))
        {
            NewVec.push_back(data);
        }
    }
    return NewVec;
}

bool qnzkna::TaskManage::ITaskManageImpl::RmoveRelationMissionMark(const CRelationMissionMark &data)
{
    for (auto target : m_RelationMissionMarkVec)
    {
        if (IsRelationMissionMarkExisted(data))
        {
            auto itr = m_RelationMissionMarkVec.begin();
            for (; itr != m_RelationMissionMarkVec.end(); itr++)
            {
                if ((0 == strcmp(target.GetMissionID().c_str(), itr->GetMissionID().c_str())) &&
                        (0 == strcmp(target.GetMarkID().c_str(), itr->GetMarkID().c_str()))
                   )
                {
                    m_RelationMissionMarkVec.erase(itr);
                    break;
                }
            }

        }
        RemoveRelationMissionMarkDBData(target);
    }
    return true;
}

bool qnzkna::TaskManage::ITaskManageImpl::RmoveRelationMissionMark(const std::string &strMissionID)
{
    bool bresult =  RemoveRelationMissionMarkDBData(strMissionID);
    LoadRelationMissionMarkDB();
    return bresult;
}

bool qnzkna::TaskManage::ITaskManageImpl::AddRelationMissionMark(const CRelationMissionMark &data)
{
    if (IsRelationMissionMarkExisted(data) == true)
    {
        return false;
    }

    m_RelationMissionMarkVec.push_back(data);

    CRelationMissionMark stCRelationMissionMark = data;
    AddRelationMissionMarkDBData(stCRelationMissionMark);

    return true;
}

bool qnzkna::TaskManage::ITaskManageImpl::IsRelationMissionMarkExisted(const CRelationMissionMark &data)
{
    for (auto dataRelation : m_RelationMissionMarkVec)
    {
        if ((0 == strcmp(data.GetMissionID().c_str(), dataRelation.GetMissionID().c_str())) &&
                (0 == strcmp(data.GetMarkID().c_str(), dataRelation.GetMarkID().c_str()))
           )
        {
            return true;
        }
    }
    return false;
}

void qnzkna::TaskManage::ITaskManageImpl::LoadActionMarkRankListDB()
{
    QMutexLocker locker(&sCsMutex1);
    QSqlQuery query;
    bool success = query.exec("SELECT `BH`, `XDBH`, `MBBH`, `MBHSYQ`, `RANK` FROM `ZZXD_JIPTL`");
    if (success)
    {
        m_ActionMarkRankVec.clear();

        while (query.next())
        {
			QString str1 = tr2(query.value(0).toString().toLatin1().data());
			QString str2 = tr2(query.value(1).toString().toLatin1().data());
			QString str3 = tr2(query.value(2).toString().toLocal8Bit().data());
			QString str4 = tr2(query.value(3).toString().toLocal8Bit().data());
			QString str5 = tr2(query.value(4).toString().toLatin1().data());

            CActionMarkRank stRelation;
            stRelation.SetID(str1.toInt());
            stRelation.SetActionID(str2.toInt());
            stRelation.SetMarkID(str3.toStdString());
            stRelation.SetMarkRequire(str4.toStdString());
            stRelation.SetRankLevel(str5.toInt());

            m_ActionMarkRankVec.push_back(stRelation);
        }
    }
}

bool qnzkna::TaskManage::ITaskManageImpl::IsMarkRankListDBExisted(CActionMarkRank &data)
{
    QString strSql("SELECT  * FROM `ZZXD_JIPTL` WHERE `XDBH` = ");
    strSql += "'" + QString::number(data.GetActionID()) + "'";
    strSql += "AND `MBBH` = '" + QString::fromStdString(data.GetMarkID()) + "'";

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

bool qnzkna::TaskManage::ITaskManageImpl::AddActionMarkRankListDB(CActionMarkRank &data)
{

    if (IsMarkRankListDBExisted(data))
    {
        return false;
    }

    QString strSql("INSERT INTO ZZXD_JIPTL (`BH`, `XDBH`, `MBBH`, `MBHSYQ`, `RANK`) VALUES (");
    strSql += QString::number(data.GetID()) + ",";
    strSql += QString::number(data.GetActionID()) + ",";
    strSql += "'" + QString::fromStdString(data.GetMarkID()) + "'" + ",";
    strSql += "'" + QString::fromStdString(data.GetMarkRequire()) + "'" + ",";
    strSql += "'" + QString::number(data.GetRankLevel()) + "'";
    strSql += ")";
    QSqlQuery query;
    bool success = query.exec(strSql);
    return success;
}

void qnzkna::TaskManage::ITaskManageImpl::LoadSchemeDB()
{
    QMutexLocker locker(&sFsMutex1);

    QSqlQuery query;
    bool success = query.exec("SELECT `FAID`, `MBID`, `QYID`, `FALX`, `HSYQ`, `ZCJSYQ`, `FZYQ` FROM `XDFA_TYSX`");
    if (success)
    {
        m_NebulaSchemeVec.clear();

        while (query.next())
        {
			QString str1 = tr2(query.value(0).toString().toLocal8Bit().data());
			QString str2 = tr2(query.value(1).toString().toLocal8Bit().data());
			QString str3 = tr2(query.value(2).toString().toLocal8Bit().data());
			QString str4 = tr2(query.value(3).toString().toLatin1().data());
			QString str5 = tr2(query.value(4).toString().toLatin1().data());
			QString str6 = tr2(query.value(5).toString().toLocal8Bit().data());
			QString str7 = tr2(query.value(6).toString().toLocal8Bit().data());

            NebulaScheme stScheme;
            stScheme.SetSchemeID(str1.toStdString());
            stScheme.SetMarkID(str2.toStdString());
            stScheme.SetRegionID(str3.toStdString());
            stScheme.SetSchemeType(str4.toInt());
            stScheme.SetDamageRequire(str5.toInt());
            stScheme.SetDetectRequire(str6.toStdString());
            stScheme.SetProtectRequire(str7.toStdString());

            m_NebulaSchemeVec.push_back(stScheme);
        }
    }
}

void qnzkna::TaskManage::ITaskManageImpl::LoadSchemePointDB()
{
    QMutexLocker locker(&sFsMutex1);

    QSqlQuery query;
    bool success = query.exec("SELECT `WZDID`, `WZDLX`, `JD`, `WD`, `GD` FROM `XDFA_ZS_WZD`");
    if (success)
    {
        m_NebulaSchemePointVec.clear();

        while (query.next())
        {
			QString str1 = tr2(query.value(0).toString().toLocal8Bit().data());
			QString str2 = tr2(query.value(1).toString().toLocal8Bit().data());
			QString str3 = tr2(query.value(2).toString().toLocal8Bit().data());
			QString str4 = tr2(query.value(3).toString().toLocal8Bit().data());
			QString str5 = tr2(query.value(4).toString().toLocal8Bit().data());

            NebulaSchemePoint stSchemePoint;
            stSchemePoint.SetPointID(str1.toStdString());
            stSchemePoint.SetPointType(str2.toInt());
            stSchemePoint.SetPosLon(str3.toDouble());
            stSchemePoint.SetPosLat(str4.toDouble());
            stSchemePoint.SetPosAlt(str5.toDouble());

            m_NebulaSchemePointVec.push_back(stSchemePoint);
        }
    }
}

void qnzkna::TaskManage::ITaskManageImpl::LoadSchemeAmmoDB()
{
    QMutexLocker locker(&sFsMutex1);

    QSqlQuery query;
    bool success = query.exec("SELECT `BH`, `FAID`, `DYLX`, `DYSL`, `FSDID`, `FSQID`, `FSSJ`, `PTLX`, `RWID`, `PTID` FROM `XDFA_DY_FS`");
    if (success)
    {
        m_NebulaSchemeAmmoVec.clear();

        while (query.next())
        {
			QString str1 = tr2(query.value(0).toString().toLatin1().data());
			QString str2 = tr2(query.value(1).toString().toLocal8Bit().data());
			QString str3 = tr2(query.value(2).toString().toLocal8Bit().data());
			QString str4 = tr2(query.value(3).toString().toLatin1().data());
			QString str5 = tr2(query.value(4).toString().toLocal8Bit().data());
			QString str6 = tr2(query.value(5).toString().toLocal8Bit().data());
			QString str7 = tr2(query.value(6).toString().toLocal8Bit().data());
			QString str8 = tr2(query.value(7).toString().toLocal8Bit().data());
			QString str9 = tr2(query.value(8).toString().toLocal8Bit().data());
			QString str10 = tr2(query.value(9).toString().toLocal8Bit().data());

            NebulaSchemeAmmo stSchemeAmmo;
            stSchemeAmmo.SetID(str1.toInt());
            stSchemeAmmo.SetSchemeID(str2.toStdString());
            stSchemeAmmo.SetAmmoType(str3.toStdString());
            stSchemeAmmo.SetAmmoNum(str4.toInt());
            stSchemeAmmo.SetShotPointID(str5.toStdString());
            stSchemeAmmo.SetShotRegionID(str6.toStdString());
            stSchemeAmmo.SetShotTime(str7.toStdString());
            stSchemeAmmo.SetPlatType(str8.toStdString());
            stSchemeAmmo.SetMissionID(str9.toStdString());
            stSchemeAmmo.SetPlatID(str10.toStdString());

            m_NebulaSchemeAmmoVec.push_back(stSchemeAmmo);
        }
    }
}

void qnzkna::TaskManage::ITaskManageImpl::LoadSchemeSensorDB()
{
    QMutexLocker locker(&sFsMutex1);

    QSqlQuery query;
    bool success = query.exec("SELECT `BH`, `FAID`, `DCLX`, `DCID`, `KJDID`, `KJSJ`, `GZDID`, `GZSJ`, `HDQID`, `GZMS`, `QBGS`, `GJDID`, `GJSJ`, `PTLX`, `RWID` , `PTID`FROM `XDFA_DC_SY`");
    if (success)
    {
        m_NebulaSchemeSensorVec.clear();

        while (query.next())
        {
			QString str1 = tr2(query.value(0).toString().toLatin1().data());
			QString str2 = tr2(query.value(1).toString().toLocal8Bit().data());
			QString str3 = tr2(query.value(2).toString().toLocal8Bit().data());
			QString str4 = tr2(query.value(3).toString().toLocal8Bit().data());
			QString str5 = tr2(query.value(4).toString().toLocal8Bit().data());
			QString str6 = tr2(query.value(5).toString().toLocal8Bit().data());
			QString str7 = tr2(query.value(6).toString().toLocal8Bit().data());
			QString str8 = tr2(query.value(7).toString().toLocal8Bit().data());
			QString str9 = tr2(query.value(8).toString().toLocal8Bit().data());
			QString str10 = tr2(query.value(9).toString().toLocal8Bit().data());
			QString str11 = tr2(query.value(10).toString().toLocal8Bit().data());
			QString str12 = tr2(query.value(11).toString().toLocal8Bit().data());
			QString str13 = tr2(query.value(12).toString().toLocal8Bit().data());
			QString str14 = tr2(query.value(13).toString().toLocal8Bit().data());
			QString str15 = tr2(query.value(14).toString().toLocal8Bit().data());
			QString str16 = tr2(query.value(15).toString().toLocal8Bit().data());

            NebulaSchemeSensor stSchemeSensor;
            stSchemeSensor.SetID(str1.toInt());
            stSchemeSensor.SetSchemeID(str2.toStdString());
            stSchemeSensor.SetSensorType(str3.toStdString());
            stSchemeSensor.SetSensorID(str4.toStdString());
            stSchemeSensor.SetPowerOnPointID(str5.toStdString());
            stSchemeSensor.SetPowerOnTime(str6.toStdString());
            stSchemeSensor.SetWorkPointID(str7.toStdString());
            stSchemeSensor.SetWorkTime(str8.toStdString());
            stSchemeSensor.SetRegionID(str9.toStdString());
            stSchemeSensor.SetWorkMode(str10.toStdString());
            stSchemeSensor.SetInfoType(str11.toStdString());
            stSchemeSensor.SetPowerOffPointID(str12.toStdString());
            stSchemeSensor.SetPowerOffTime(str13.toStdString());
            stSchemeSensor.SetPlatType(str14.toStdString());
            stSchemeSensor.SetMissionID(str15.toStdString());
            stSchemeSensor.SetPlatID(str16.toStdString());

            m_NebulaSchemeSensorVec.push_back(stSchemeSensor);
        }
    }
}

void qnzkna::TaskManage::ITaskManageImpl::LoadSchemePointFADB()
{
    QMutexLocker locker(&sFsMutex1);

    QSqlQuery query;
    bool success = query.exec("SELECT `BH`, `FAID`, `WZDID` FROM `XDFA_GL_WZD`");
    if (success)
    {
        m_NebulaSchemePointFAVec.clear();

        while (query.next())
        {
			QString str1 = tr2(query.value(0).toString().toLocal8Bit().data());
			QString str2 = tr2(query.value(1).toString().toLocal8Bit().data());
			QString str3 = tr2(query.value(2).toString().toLocal8Bit().data());

            NebulaSchemePointFA stSchemePointFA;
            stSchemePointFA.SetID(str1.toInt());
            stSchemePointFA.SetSchemeID(str2.toStdString());
            stSchemePointFA.SetPointID(str3.toStdString());
            m_NebulaSchemePointFAVec.push_back(stSchemePointFA);
        }
    }
}

void qnzkna::TaskManage::ITaskManageImpl::LoadActionSchemeGLDB()
{
    QMutexLocker locker(&sFsMutex1);

    QSqlQuery query;
    bool success = query.exec("SELECT `BH`, `XDBH`, `FAID` FROM `ZZXD_GL_FA`");
    if (success)
    {
        m_NebulaActionSchemeGLVec.clear();

        while (query.next())
        {
			QString str1 = tr2(query.value(0).toString().toLocal8Bit().data());
			QString str2 = tr2(query.value(1).toString().toLocal8Bit().data());
			QString str3 = tr2(query.value(2).toString().toLocal8Bit().data());

            NebulaActionSchemeGL stActionScheme;
            stActionScheme.SetID(str1.toInt());
            stActionScheme.SetActionID(str2.toInt());
            stActionScheme.SetSchemeID(str3.toStdString());
            m_NebulaActionSchemeGLVec.push_back(stActionScheme);
        }
    }
}

bool qnzkna::TaskManage::ITaskManageImpl::AddSchemeDB(NebulaScheme &data)
{

    if (IsSchemeDBExisted(data))
    {
        return false;
    }

    QString strSql("INSERT INTO XDFA_TYSX (`FAID`, `MBID`, `QYID`, `FALX`, `HSYQ`, `ZCJSYQ`, `FZYQ`) VALUES (");
    strSql += "'" + QString::fromStdString(data.GetSchemeID()) + "'" + ",";
    strSql += "'" + QString::fromStdString(data.GetMarkID()) + "'" + ",";
    strSql += "'" + QString::fromStdString(data.GetRegionID()) + "'" + ",";
    strSql += "'" + QString::number(data.GetSchemeType()) + "'" + ",";
    strSql += "'" + QString::number(data.GetDamageRequire()) + "'" + ",";
    strSql += "'" + QString::fromStdString(data.GetDetectRequire()) + "'" + ",";
    strSql += "'" + QString::fromStdString(data.GetProtectRequire()) + "'";
    strSql += ")";

    QSqlQuery query;
    bool success = query.exec(strSql);
    return success;
}

bool qnzkna::TaskManage::ITaskManageImpl::AddSchemePointDB(NebulaSchemePoint &data)
{

    if (IsSchemePointDBExisted(data))
    {
        return false;
    }

    QString strSql("INSERT INTO XDFA_ZS_WZD (`WZDID`, `WZDLX`, `JD`, `WD`, `GD`) VALUES (");
    strSql += QString::fromStdString(data.GetPointID()) + ",";
    strSql += QString::number(data.GetPointType()) + ",";
    strSql += "'" + QString::number(data.GetPosLon()) + "'" + ",";
    strSql += "'" + QString::number(data.GetPosLat()) + "'" + ",";
    strSql += "'" + QString::number(data.GetPosAlt()) + "'";
    strSql += ")";

    QSqlQuery query;
    bool success = query.exec(strSql);
    return success;
}

bool qnzkna::TaskManage::ITaskManageImpl::AddSchemeAmmoDB(NebulaSchemeAmmo &data)
{

    if (IsSchemeAmmoDBExisted(data))
    {
        return false;
    }

    QString strSql("INSERT INTO XDFA_DY_FS (`FAID`, `DYLX`, `DYSL`, `FSDID`, `FSQID`, `FSSJ`, `PTLX`, `RWID`, `PTID`) VALUES (");

    strSql += "'" + QString::fromStdString(data.GetSchemeID()) + "'" + ",";
    strSql += "'" + QString::fromStdString(data.GetAmmoType()) + "'" + ",";
    strSql += "'" + QString::number(data.GetAmmoNum()) + "'" + ",";
    strSql += "'" + QString::fromStdString(data.GetShotPointID()) + "'" + ",";
    strSql += "'" + QString::fromStdString(data.GetShotRegionID()) + "'" + ",";
    strSql += "'" + QString::fromStdString(data.GetShotTime()) + "'" + ",";
    strSql += "'" + QString::fromStdString(data.GetPlatType()) + "'" + ",";
    strSql += "'" + QString::fromStdString(data.GetMissionID()) + "'" + ",";
    strSql += "'" + QString::fromStdString(data.GetPlatID()) + "'";
    strSql += ")";

    QSqlQuery query;
    bool success = query.exec(strSql);
    return success;
}

bool qnzkna::TaskManage::ITaskManageImpl::AddSchemeSensorDB(NebulaSchemeSensor &data)
{

    if (IsSchemeSensorDBExisted(data))
    {
        return false;
    }

    QString strSql("INSERT INTO XDFA_DC_SY (`FAID`, `DCLX`, `DCID`, `KJDID`, `KJSJ`, `GZDID`, `GZSJ`, `HDQID`, `GZMS`, `QBGS`, `GJDID`, `GJSJ`, `PTLX`, `RWID`, `PTID`) VALUES (");

    strSql += "'" + QString::fromStdString(data.GetSchemeID()) + "'" + ",";
    strSql += "'" + QString::fromStdString(data.GetSensorType()) + "'" + ",";
    strSql += "'" + QString::fromStdString(data.GetSensorID()) + "'" + ",";
    strSql += "'" + QString::fromStdString(data.GetPowerOnPointID()) + "'" + ",";
    strSql += "'" + QString::fromStdString(data.GetPowerOnTime()) + "'" + ",";
    strSql += "'" + QString::fromStdString(data.GetWorkPointID()) + "'" + ",";
    strSql += "'" + QString::fromStdString(data.GetWorkTime()) + "'" + ",";
    strSql += "'" + QString::fromStdString(data.GetRegionID()) + "'" + ",";
    strSql += "'" + QString::fromStdString(data.GetWorkMode()) + "'" + ",";
    strSql += "'" + QString::fromStdString(data.GetInfoType()) + "'" + ",";
    strSql += "'" + QString::fromStdString(data.GetPowerOffPointID()) + "'" + ",";
    strSql += "'" + QString::fromStdString(data.GetPowerOffTime()) + "'" + ",";

    strSql += "'" + QString::fromStdString(data.GetPlatType()) + "'" + ",";
    strSql += "'" + QString::fromStdString(data.GetMissionID()) + "'" + ",";
    strSql += "'" + QString::fromStdString(data.GetPlatID()) + "'";
    strSql += ")";

    QSqlQuery query;
    bool success = query.exec(strSql);
    return success;
}

bool qnzkna::TaskManage::ITaskManageImpl::AddSchemePointFADB(NebulaSchemePointFA &data)
{

    if (IsSchemePointFADBExisted(data))
    {
        return false;
    }

    QString strSql("INSERT INTO XDFA_GL_WZD (`FAID`, `WZDID`) VALUES (");

    strSql += "'" + QString::fromStdString(data.GetSchemeID()) + "',";
    strSql += "'" + QString::fromStdString(data.GetPointID()) + "'";
    strSql += ")";

    QSqlQuery query;
    bool success = query.exec(strSql);
    return success;
}

bool qnzkna::TaskManage::ITaskManageImpl::AddActionSchemeGLDB(NebulaActionSchemeGL &data)
{

    if (IsActionSchemeGLDBExisted(data))
    {
        return false;
    }

    QString strSql("INSERT INTO ZZXD_GL_FA (`BH`, `XDBH`, `FAID`) VALUES (");
    strSql += QString::number(data.GetID()) + ",";
    strSql += QString::number(data.GetActionID()) + ",";
    strSql += "'" + QString::fromStdString(data.GetSchemeID()) + "'";
    strSql += ")";

    QSqlQuery query;
    bool success = query.exec(strSql);
    return success;
}

bool qnzkna::TaskManage::ITaskManageImpl::EditSchemeDB(NebulaScheme &data)
{

    if (!IsSchemeDBExisted(data))
    {
        return false;
    }

    QString strSql("UPDATE XDFA_TYSX set ");
    strSql += "`MBID` = '" + QString::fromStdString(data.GetMarkID()) + "',";
    strSql += "`QYID` = '" + QString::fromStdString(data.GetRegionID()) + "',";
    strSql += "`FALX` = '" + QString::number(data.GetSchemeType()) + "'" + ",";
    strSql += "`HSYQ` = " + QString::number(data.GetDamageRequire()) + ",";
    strSql += "`ZCJSYQ` = '" + QString::fromStdString(data.GetDetectRequire()) + "',";
    strSql += "`FZYQ` = '" + QString::fromStdString(data.GetProtectRequire()) + "'";
    strSql += "where `FAID` = " + QString::fromStdString(data.GetSchemeID());

    QSqlQuery query;
    bool success = query.exec(strSql);
    return success;
}

bool qnzkna::TaskManage::ITaskManageImpl::EditSchemePointDB(NebulaSchemePoint &data)
{

    if (!IsSchemePointDBExisted(data))
    {
        return false;
    }

    QString strSql("UPDATE XDFA_ZS_WZD set ");
    strSql += "`WZDLX` = " + QString::number(data.GetPointType()) + ",";
    strSql += "`JD` = '" + QString::number(data.GetPosLon()) + "'" + ",";
    strSql += "`WD` = " + QString::number(data.GetPosLat()) + ",";
    strSql += "`GD` = '" + QString::number(data.GetPosAlt()) + "'";
    strSql += "where `WZDID` = '" + QString::fromStdString(data.GetPointID()) + "'";

    QSqlQuery query;
    bool success = query.exec(strSql);
    return success;
}

bool qnzkna::TaskManage::ITaskManageImpl::EditSchemeAmmoDB(NebulaSchemeAmmo &data)
{

    if (!IsSchemeAmmoDBExisted(data))
    {
        return false;
    }

    QString strSql("UPDATE XDFA_DY_FS set ");
    strSql += "`FAID` = " + QString::fromStdString(data.GetSchemeID()) + ",";
    strSql += "`DYLX` = '" + QString::fromStdString(data.GetAmmoType()) + "',";
    strSql += "`DYSL` = '" + QString::number(data.GetAmmoNum()) + "'" + ",";
    strSql += "`FSDID` = " + QString::fromStdString(data.GetShotPointID()) + ",";
    strSql += "`FSQID` = '" + QString::fromStdString(data.GetShotRegionID()) + "',";
    strSql += "`FSSJ` = '" + QString::fromStdString(data.GetShotTime()) + "',";
    strSql += "`PTLX` = '" + QString::fromStdString(data.GetPlatID()) + "',";
    strSql += "`RWID` = '" + QString::fromStdString(data.GetMissionID()) + "',";
    strSql += "`PTID` = '" + QString::fromStdString(data.GetPlatID()) + "'" + ",";
    strSql += "where `BH` = " + QString::number(data.GetID()) + "'";

    QSqlQuery query;
    bool success = query.exec(strSql);
    return success;
}

bool qnzkna::TaskManage::ITaskManageImpl::EditSchemeSensorDB(NebulaSchemeSensor &data)
{

    if (!IsSchemeSensorDBExisted(data))
    {
        return false;
    }

    QString strSql("UPDATE XDFA_DC_SY set ");
    strSql += "`FAID` = " + QString::fromStdString(data.GetSchemeID()) + ",";
    strSql += "`DCLX` = '" + QString::fromStdString(data.GetSensorType()) + "',";
    strSql += "`DCID` = '" + QString::fromStdString(data.GetSensorID()) + "'" + ",";
    strSql += "`KJDID` = " + QString::fromStdString(data.GetPowerOnPointID()) + ",";
    strSql += "`KJSJ` = '" + QString::fromStdString(data.GetPowerOnTime()) + "',";
    strSql += "`GZDID` = '" + QString::fromStdString(data.GetWorkPointID()) + "',";
    strSql += "`GZSJ` = '" + QString::fromStdString(data.GetWorkTime()) + "',";
    strSql += "`HDQID` = '" + QString::fromStdString(data.GetRegionID()) + "',";
    strSql += "`GZMS` = '" + QString::fromStdString(data.GetWorkMode()) + "',";
    strSql += "`QBGS` = '" + QString::fromStdString(data.GetInfoType()) + "',";
    strSql += "`GJDID` = '" + QString::fromStdString(data.GetPowerOffPointID()) + "',";
    strSql += "`GJSJ` = '" + QString::fromStdString(data.GetPowerOffTime()) + "',";
    strSql += "`PTLX` = '" + QString::fromStdString(data.GetPlatType()) + "',";
    strSql += "`RWID` = '" + QString::fromStdString(data.GetMissionID()) + "',";
    strSql += "`PTID` = '" + QString::fromStdString(data.GetPlatID()) + "'" + ",";
    strSql += "where `BH` = " + QString::number(data.GetID()) + "'";

    QSqlQuery query;
    bool success = query.exec(strSql);
    return success;
}

bool qnzkna::TaskManage::ITaskManageImpl::EditSchemePointFADB(NebulaSchemePointFA &data)
{

    return true;
}

bool qnzkna::TaskManage::ITaskManageImpl::EditActionSchemeGLDB(NebulaActionSchemeGL &data)
{
    return true;
}

bool qnzkna::TaskManage::ITaskManageImpl::DeleteSchemeDB(NebulaScheme &data)
{

    if (!IsSchemeDBExisted(data))
    {
        return false;
    }

    QString strSql("DELETE FROM XDFA_TYSX where FAID = ");
    strSql += "'" + QString::fromStdString(data.GetSchemeID()) + "'";

    QSqlQuery query;
    bool success = query.exec(strSql);
    return success;
}

bool qnzkna::TaskManage::ITaskManageImpl::DeleteSchemePointDB(NebulaSchemePoint &data)
{

    if (!IsSchemePointDBExisted(data))
    {
        return false;
    }

    QString strSql("DELETE FROM XDFA_ZS_WZD where WZDID = ");
    strSql += "'" + QString::fromStdString(data.GetPointID()) + "'";

    QSqlQuery query;
    bool success = query.exec(strSql);
    return success;
}

bool qnzkna::TaskManage::ITaskManageImpl::DeleteSchemeAmmoDB(NebulaSchemeAmmo &data)
{

    if (!IsSchemeAmmoDBExisted(data))
    {
        return false;
    }

    QString strSql("DELETE FROM XDFA_DY_FS where BH = ");
    strSql += "'" + QString::number(data.GetID()) + "'";

    QSqlQuery query;
    bool success = query.exec(strSql);
    return success;
}

bool qnzkna::TaskManage::ITaskManageImpl::DeleteSchemeSensorDB(NebulaSchemeSensor &data)
{

    if (!IsSchemeSensorDBExisted(data))
    {
        return false;
    }

    QString strSql("DELETE FROM XDFA_DC_SY where BH = ");
    strSql += "'" + QString::number(data.GetID()) + "'";

    QSqlQuery query;
    bool success = query.exec(strSql);
    return success;

}

bool qnzkna::TaskManage::ITaskManageImpl::DeleteSchemePointFADB(NebulaSchemePointFA &data)
{

    if (!IsSchemePointFADBExisted(data))
    {
        return false;
    }

    QString strSql("DELETE FROM XDFA_GL_WZD where WZDID = ");
    strSql += "'" + QString::fromStdString(data.GetPointID()) + "'";
    strSql += "AND `FAID` = '" + QString::fromStdString(data.GetSchemeID()) + "'";

    QSqlQuery query;
    bool success = query.exec(strSql);
    return success;
}

bool qnzkna::TaskManage::ITaskManageImpl::DeleteActionSchemeGLDB(NebulaActionSchemeGL &data)
{

    if (!IsActionSchemeGLDBExisted(data))
    {
        return false;
    }

    QString strSql("DELETE FROM ZZXD_GL_FA where XDBH = ");
    strSql += QString::number(data.GetActionID()) + "'";
    strSql += "AND `FAID` = '" + QString::fromStdString(data.GetSchemeID()) + "'";

    QSqlQuery query;
    bool success = query.exec(strSql);
    return success;
}

bool qnzkna::TaskManage::ITaskManageImpl::IsSchemeDBExisted(NebulaScheme &scheme)
{
    QString strSql("SELECT  * FROM `XDFA_TYSX` WHERE `FAID` = ");
    strSql += "'" + QString::fromStdString(scheme.GetSchemeID()) + "'";

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

bool qnzkna::TaskManage::ITaskManageImpl::IsSchemePointDBExisted(NebulaSchemePoint &point)
{
    QString strSql("SELECT  * FROM `XDFA_ZS_WZD` WHERE `WZDID` = ");
    strSql += "'" + QString::fromStdString(point.GetPointID()) + "'";

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

bool qnzkna::TaskManage::ITaskManageImpl::IsSchemeAmmoDBExisted(NebulaSchemeAmmo &ammo)
{
    QString strSql("SELECT  * FROM `XDFA_DY_FS` WHERE `FAID` = ");
    strSql += "'" + QString::fromStdString(ammo.GetSchemeID()) + "'";
    strSql += "AND `BH` = '" + QString::number(ammo.GetID()) + "'";

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

bool qnzkna::TaskManage::ITaskManageImpl::IsSchemeSensorDBExisted(NebulaSchemeSensor &sensor)
{
    QString strSql("SELECT  * FROM `XDFA_DC_SY` WHERE `FAID` = ");
    strSql += "'" + QString::fromStdString(sensor.GetSchemeID()) + "'";
    strSql += "AND `BH` = '" + QString::number(sensor.GetID()) + "'";

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

bool qnzkna::TaskManage::ITaskManageImpl::IsSchemePointFADBExisted(NebulaSchemePointFA &data)
{
    QString strSql("SELECT  * FROM `XDFA_GL_WZD` WHERE `FAID` = ");
    strSql += "'" + QString::fromStdString(data.GetSchemeID()) + "'";
    strSql += "AND `WZDID` = '" + QString::fromStdString(data.GetPointID()) + "'";

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

bool qnzkna::TaskManage::ITaskManageImpl::IsActionSchemeGLDBExisted(NebulaActionSchemeGL &data)
{
    QString strSql("SELECT  * FROM `ZZXD_GL_FA` WHERE `XDBH` = ");
    strSql += "'" + QString::number(data.GetActionID()) + "'";
    strSql += "AND `FAID` = '" + QString::fromStdString(data.GetSchemeID()) + "'";
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

int qnzkna::TaskManage::ITaskManageImpl::GetMaxSchemeAmmoID()
{
    QSqlQuery query;
    int nNoMax = 0;
    bool success = query.exec("SELECT MAX(`BH`) from XDFA_DY_FS");
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

int qnzkna::TaskManage::ITaskManageImpl::GetMaxSchemeSensorID()
{
    QSqlQuery query;
    int nNoMax = 0;
    bool success = query.exec("SELECT MAX(`BH`) from XDFA_DC_SY");
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

void qnzkna::TaskManage::ITaskManageImpl::LoadMissionAmmoDB()
{
    QMutexLocker locker(&sMutexMissionAmmo);
    QSqlQuery query;
    bool success = query.exec("SELECT `RWBH`, `DYMC`, `DYSL` FROM `Relation_RW_DYZY`");
    if (success)
    {
        m_NebulaMissionAmmoVec.clear();
        while (query.next())
        {
			QString str1 = tr2(query.value(0).toString().toLocal8Bit().data());
			QString str2 = tr2(query.value(1).toString().toLocal8Bit().data());
			QString str3 = tr2(query.value(2).toString().toLatin1().data());
            CRelationMissionAmmo stMissionAmmo;
            stMissionAmmo.SetMissionID(str1.toStdString());
            stMissionAmmo.SetAmmoName(str2.toStdString());
            stMissionAmmo.SetAmmoNum(str3.toInt());
            m_NebulaMissionAmmoVec.push_back(stMissionAmmo);
        }
    }
    UpdataMissionAmmoVec();
}

bool qnzkna::TaskManage::ITaskManageImpl::AddMissionAmmoDB(const CRelationMissionAmmo &data)
{
    QString strSql("INSERT INTO Relation_RW_DYZY (`RWBH`, `DYMC`, `DYSL`) VALUES (");
    strSql += "'" + tr2(data.GetMissionID().c_str()) + "'" + ",";
    strSql += "'" + tr2(data.GetAmmoName().c_str()) + "'" + ",";
    strSql += "'" + QString::number(data.GetAmmoNum()) + "'";
    strSql += ")";

    QSqlQuery query;
    bool success = query.exec(strSql);
    return success;
}

bool qnzkna::TaskManage::ITaskManageImpl::DeleteMissionAmmoDB(std::string nMissionID)
{
    QString strSql("DELETE FROM Relation_RW_DYZY where RWBH = ");
    strSql += "'" + QString::fromStdString(nMissionID) + "'";
    QSqlQuery query;
    bool success = query.exec(strSql);
    return success;
}

bool qnzkna::TaskManage::ITaskManageImpl::IsMissionAmmoDBMissionIDDB(std::string nMissionID)
{
    QString strSql("SELECT  * FROM `Relation_RW_DYZY` WHERE `RWBH` = ");
    strSql += "'" + QString::fromStdString(nMissionID) + "'";
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

void qnzkna::TaskManage::ITaskManageImpl::loadLoginName(const std::string &strLoginFile)
{
    loginName.clear();
    TiXmlDocument docElement;
    if (!docElement.LoadFile(strLoginFile.c_str()))
    {
        return;
    }

    TiXmlElement *pRootEle = docElement.RootElement();
    if (!pRootEle)
    {
        return;
    }
    TiXmlElement *pRecordListEle = pRootEle->FirstChildElement("µÇÂ¼¼ÇÂ¼ÁÐ±í");
    if (!pRecordListEle)
    {
        return;
    }

    TiXmlElement *pRecordEle = pRecordListEle->FirstChildElement("µÇÂ¼¼ÇÂ¼");
    while (pRecordEle)
    {
        const char *pStrUserName = pRecordEle->Attribute("µÇÂ¼Ãû³Æ");
        loginName = pStrUserName;
        pRecordEle = pRecordEle->NextSiblingElement("µÇÂ¼¼ÇÂ¼");
    }
}

int qnzkna::TaskManage::ITaskManageImpl::sendSystemUpdate(ESystemUpdateType systemUpdateType, ESystemOperatorType systemOperatorType, const char *objectID)
{
    char sendBuf[sizeof(NetHeader) + sizeof(SystemUpdate)] = { 0 };
    NetHeader *pHeader = (NetHeader *)sendBuf;
    SystemUpdate *su = (SystemUpdate *)(sendBuf + sizeof(NetHeader));
    pHeader->ucType = NETHEADER_UCTYPE_SYSTEM_UPDATE;
    pHeader->ucSubType = 0x01;
    pHeader->unLength = sizeof(NetHeader) + sizeof(SystemUpdate);

    pHeader->unSeatType = 1;
    pHeader->unNodeType = 1;

    su->unMainType = XYGSSYSTEMCONTROL_SYSTEMCONTROL_UNMAINTYPE_SYSTEMUPDATE;
    memcpy(su->Operator, loginName.data(), loginName.size());
    su->updateType = systemUpdateType;
    su->operatorType = systemOperatorType;
    memcpy(su->unObjectId, objectID, strlen(objectID));

    int len = LY_TaskManageServiceInterface::Get().SendData(sendBuf, sizeof(sendBuf));
    return len;

}

void qnzkna::TaskManage::ITaskManageImpl::LoadMissionRelation_PlatRouteDB()
{
    QMutexLocker locker(&sMutexMissionRelation_PlatRoute);
    QSqlQuery query;
    bool success = query.exec("SELECT `BH`, `RWBH`, `PTBH`, `MBBH`, `HXBH` FROM `Relation_RW_PTHX`");
    if (success)
    {
        m_NebulaMissionRelation_PlatRouteVec.clear();
        while (query.next())
        {
			QString str1 = tr2(query.value(0).toString().toLatin1().data());
			QString str2 = tr2(query.value(1).toString().toLocal8Bit().data());
			QString str3 = tr2(query.value(2).toString().toLocal8Bit().data());
			QString str4 = tr2(query.value(3).toString().toLocal8Bit().data());
			QString str5 = tr2(query.value(4).toString().toLatin1().data());
            CNebulaMissionRelation_PlatRoute stMissionRelation_PlatRoute;
            stMissionRelation_PlatRoute.SetID(str1.toInt());
            stMissionRelation_PlatRoute.SetTaskID(str2.toStdString());
            stMissionRelation_PlatRoute.SetPlatformID(str3.toStdString());
            stMissionRelation_PlatRoute.SetMarkID(str4.toStdString());
            stMissionRelation_PlatRoute.SetRouteID(str5.toInt());
            m_NebulaMissionRelation_PlatRouteVec.push_back(stMissionRelation_PlatRoute);
        }
    }
}

bool qnzkna::TaskManage::ITaskManageImpl::AddMissionRelation_PlatRouteDB(const CNebulaMissionRelation_PlatRoute &data)
{
    QString strSql("INSERT INTO Relation_RW_PTHX (`RWBH`, `PTBH`, `MBBH`, `HXBH`) VALUES (");
    strSql += "'" + QString::fromStdString(data.GetTaskID()) + "'" + ",";
    strSql += "'" + QString::fromStdString(data.GetPlatformID()) + "'" + ",";
    strSql += "'" + QString::fromStdString(data.GetMarkID()) + "'" + ",";
    strSql += "'" + QString::number(data.GetRouteID()) + "'";
    strSql += ")";

    QSqlQuery query;
    bool success = query.exec(strSql);
    return success;
}

bool qnzkna::TaskManage::ITaskManageImpl::DeleteMissionRelation_PlatRouteDB(std::string nMissionID)
{
    QString strSql("DELETE FROM Relation_RW_PTHX where RWBH = ");
    strSql += "'" + QString::fromStdString(nMissionID) + "'";
    QSqlQuery query;
    bool success = query.exec(strSql);
    return success;
}

bool qnzkna::TaskManage::ITaskManageImpl::DeleteRowMissionRelation_PlatRouteDB(std::string sMissionID, int nRouteID)
{
    QString strSql("DELETE FROM Relation_RW_PTHX where RWBH = ");
    strSql += "'" + QString::fromStdString(sMissionID) + "'" + " && `HXBH` = ";
    strSql += QString::number(nRouteID);
    QSqlQuery query;
    bool success = query.exec(strSql);
    return success;
}

bool qnzkna::TaskManage::ITaskManageImpl::IsMissionRelation_PlatRouteDBMissionIDDB(std::string nMissionID)
{
    QString strSql("SELECT  * FROM `Relation_RW_PTHX` WHERE `RWBH` = ");
    strSql += "'" + QString::fromStdString(nMissionID) + "'";
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

bool qnzkna::TaskManage::ITaskManageImpl::IsMissionPouteRelation_PlatRouteDBMissionIDDB(std::string sMissionID, int nRouteID)
{
    QString strSql("SELECT  * FROM `Relation_RW_PTHX` WHERE `RWBH` = ");
    strSql += "'" + QString::fromStdString(sMissionID) + "'" + "AND `HXBH` =";
    strSql += QString::number(nRouteID);
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

void qnzkna::TaskManage::ITaskManageImpl::LoadResourceCarryingPlanDB()
{
    QMutexLocker locker(&sMutexCarryPlan);
    QSqlQuery query;
    bool success = query.exec("SELECT `BH`, `ZYLX`, `ZYMC`, `ZYSL`, `KYSL`, `BZ` FROM `RWGH_XXJH`");
    if (success)
    {
        m_CResourceCarryingVec.clear();
        while (query.next())
        {
			QString str1 = tr2(query.value(0).toString().toLatin1().data());
			QString str2 = tr2(query.value(1).toString().toLatin1().data());
			QString str3 = tr2(query.value(2).toString().toLocal8Bit().data());
			QString str4 = tr2(query.value(3).toString().toLatin1().data());
			QString str5 = tr2(query.value(4).toString().toLatin1().data());
			QString str6 = tr2(query.value(5).toString().toLocal8Bit().data());
            CResourceCarrying stResourceCarrying;
            stResourceCarrying.SetBH(str1.toInt());
            stResourceCarrying.SetResourceType(str2.toInt());
            stResourceCarrying.SetResourceName(str3.toStdString());
            stResourceCarrying.SetResourceNum(str4.toInt());
            stResourceCarrying.SetRemainNum(str5.toInt());
            stResourceCarrying.SetRemark(str6.toStdString());

            m_CResourceCarryingVec.push_back(stResourceCarrying);
        }
    }
}

bool qnzkna::TaskManage::ITaskManageImpl::AddResourceCarryingDB(const CResourceCarrying &data)
{
    std::string resName = data.GetResourceName();
    bool isExist = IsResourceCarryingDB(resName);

    if (isExist == false)
    {
        QString strSql("INSERT INTO RWGH_XXJH (`BH`, `ZYLX`, `ZYMC`, `ZYSL`, `KYSL`, `BZ`) VALUES (");
        strSql += "'" + QString::number(data.GetBH()) + "'" + ",";
        strSql += "'" + QString::number(data.GetResourceType()) + "'" + ",";
        strSql += "'" + QString::fromStdString(data.GetResourceName()) + "'" + ",";
        strSql += "'" + QString::number(data.GetResourceNum()) + "'" + ",";
        strSql += "'" + QString::number(data.GetRemainNum()) + "'" + ",";
        strSql += "'" + QString::fromStdString(data.GetRemark()) + "'";
        strSql += ")";

        QSqlQuery query;
        bool success = query.exec(strSql);
        return success;
    }
    else
    {
        QString strSql("UPDATE RWGH_XXJH set ");
        strSql += "`ZYSL` = " + QString::number(data.GetResourceNum()) + ",";
        strSql += "`KYSL` = " + QString::number(data.GetRemainNum()) + " ";
        strSql += "where `ZYMC` = " + QString::fromStdString(data.GetResourceName());

        QSqlQuery query;
        bool success = query.exec(strSql);
        return success;
    }

}

bool qnzkna::TaskManage::ITaskManageImpl::DeleteResourceCarryingDB()
{
    QString strSql("DELETE FROM RWGH_XXJH");
    QSqlQuery query;
    bool success = query.exec(strSql);
    return success;
}

bool qnzkna::TaskManage::ITaskManageImpl::IsResourceCarryingDB(std::string nResName)
{
    QString strSql("SELECT  * FROM `RWGH_XXJH` WHERE `ZYMC` = ");
    strSql += "'" + QString::fromStdString(nResName) + "'";
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

int qnzkna::TaskManage::ITaskManageImpl::GetCarryingPlanMaxBHDB()
{
    QSqlQuery query;
    int nNoMax = 0;
    bool success = query.exec("SELECT MAX(`BH`) from RWGH_XXJH");
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

void qnzkna::TaskManage::ITaskManageImpl::LoadOperationalInformationDB()
{
    QMutexLocker locker(&sMutexOperationalInformation);
    QSqlQuery query;
    bool success = query.exec("SELECT `BH`, `RWBH`, `XILX`, `ZYBH`, `BZ` FROM `RWSJ_ZZXIXS_CP`");
    if (success)
    {
        m_COperationalInformationVec.clear();
        while (query.next())
        {
			QString str1 = tr2(query.value(0).toString().toLatin1().data());
			QString str2 = tr2(query.value(1).toString().toLocal8Bit().data());
			QString str3 = tr2(query.value(2).toString().toLatin1().data());
			QString str4 = tr2(query.value(3).toString().toLocal8Bit().data());
			QString str5 = tr2(query.value(4).toString().toLocal8Bit().data());
            CNeubulaOperationalInformation stOperationalInformation;
            stOperationalInformation.SetID(str1.toInt());
            stOperationalInformation.SetTaskID(str2.toStdString());
            stOperationalInformation.SetInformationType(str3.toInt());
            stOperationalInformation.SetResourceID(str4.toStdString());
            stOperationalInformation.SetRemarks(str5.toStdString());

            m_COperationalInformationVec.push_back(stOperationalInformation);
        }
    }
}

bool qnzkna::TaskManage::ITaskManageImpl::AddOperationalInformationDB(const CNeubulaOperationalInformation &data)
{
    QString strSql("INSERT INTO RWSJ_ZZXIXS_CP (`BH`, `RWBH`, `XILX`, `ZYBH`, `BZ`) VALUES (");
    strSql += "'" + QString::number(data.GetID()) + "'" + ",";
    strSql += "'" + QString::fromStdString(data.GetTaskID()) + "'" + ",";
    strSql += "'" + QString::number(data.GetInformationType()) + "'" + ",";
    strSql += "'" + QString::fromStdString(data.GetResourceID()) + "'" + ",";
    strSql += "'" + QString::fromStdString(data.GetRemarks()) + "'";
    strSql += ")";

    QSqlQuery query;
    bool success = query.exec(strSql);
    return success;
}

bool qnzkna::TaskManage::ITaskManageImpl::DeleteOperationalInformationDB(int ntype, std::string sTemp)
{
    QString strSql("DELETE FROM RWSJ_ZZXIXS_CP where RWBH = ");
    strSql += "'" + QString::fromStdString(sTemp) + "'";
    strSql += "AND `XILX` = '" + QString::number(ntype) + "'";
    QSqlQuery query;
    bool success = query.exec(strSql);
    return success;
}

bool qnzkna::TaskManage::ITaskManageImpl::DeleteOperationalInformationDB(int ntype, std::string sTaskID, std::string stargetID)
{
    QString strSql("DELETE FROM RWSJ_ZZXIXS_CP where RWBH = ");
    strSql += "'" + QString::fromStdString(sTaskID) + "'";
    strSql += "AND `XILX` = '" + QString::number(ntype) + "'";
    strSql += "AND `ZYBH` = '" + QString::fromStdString(stargetID) + "'";
    QSqlQuery query;
    bool success = query.exec(strSql);
    return success;
}

bool qnzkna::TaskManage::ITaskManageImpl::IsOperationalInformationDB(int ntype, std::string sTemp)
{
    QString strSql("SELECT  * FROM `RWSJ_ZZXIXS_CP` WHERE `RWBH` = ");
    strSql += "'" + QString::fromStdString(sTemp) + "'";
    strSql += "AND `XILX` = '" + QString::number(ntype) + "'";
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

bool qnzkna::TaskManage::ITaskManageImpl::IsOperationalInformationDB(int ntype, std::string sTemp, std::string stargetID)
{
    QString strSql("SELECT  * FROM `RWSJ_ZZXIXS_CP` WHERE `RWBH` = ");
    strSql += "'" + QString::fromStdString(sTemp) + "'";
    strSql += "AND `XILX` = '" + QString::number(ntype) + "'";
    strSql += "AND `ZYBH` = '" + QString::fromStdString(stargetID) + "'";
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

int qnzkna::TaskManage::ITaskManageImpl::GetActiveSchemeMaxID()
{
    QSqlQuery query;
    int nNoMax = 0;
    bool success = query.exec("SELECT MAX(`BH`) from ZZXD_GL_FA");
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

int qnzkna::TaskManage::ITaskManageImpl::GetSchemeLocationMaxID()
{
    QSqlQuery query;
    int nNoMax = 0;
    bool success = query.exec("SELECT MAX(`BH`) from XDFA_GL_WZD");
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

void qnzkna::TaskManage::ITaskManageImpl::LoadMissionAmmo()
{
    LoadMissionAmmoDB();
}

bool qnzkna::TaskManage::ITaskManageImpl::AddMissionAmmo(const CRelationMissionAmmo &data)
{
    QMutexLocker locker(&sMutexMissionAmmo);

    if (AddMissionAmmoDB(data) == false)
    {
        return false;
    }

    m_NebulaMissionAmmoVec.push_back(data);
    return true;
}

bool qnzkna::TaskManage::ITaskManageImpl::RemoveMissionIDAmmo(std::string sMissionID)
{
    QMutexLocker locker(&sMutexMissionAmmo);

    if (IsMissionAmmoDBMissionIDDB(sMissionID) == false)
    {
        return false;
    }
    DeleteMissionAmmoDB(sMissionID);
    return true;
}

CRelationMissionAmmoVec qnzkna::TaskManage::ITaskManageImpl::GetAllissionAmmoVec()
{
    QMutexLocker locker(&sMutexMissionAmmo);
    return m_NebulaMissionAmmoVec;
}

CRelationMissionAmmoVec qnzkna::TaskManage::ITaskManageImpl::GetMissionIDByMissionAmmo(const std::string sMissionID)
{
    CRelationMissionAmmoVec stMissionAmmoVec;
    for (auto data : m_NebulaMissionAmmoVec)
    {
        if (data.GetMissionID() == sMissionID)
        {
            stMissionAmmoVec.push_back(data);
        }
    }
    return stMissionAmmoVec;
}

std::vector<MessageRecv> qnzkna::TaskManage::ITaskManageImpl::getMessageVecForActiveID(const int activeID, const char *recvTime)
{
    QMutexLocker locker(&MutexMessageRecv);
    std::vector<MessageRecv> reVecs;
    reVecs.clear();

    QSqlQuery query;
    QString queryStr = "SELECT  * FROM `messageRecvList` WHERE `XDBH` = ";
    queryStr += QString::number(activeID);
    if (bool success = query.exec(queryStr))
    {
        while (query.next())
        {
			QString str00 = tr2(query.value(0).toString().toLatin1().data());
			QString str01 = tr2(query.value(1).toString().toLatin1().data());
            if (recvTime != NULL)
            {
                char recv20[20] = { 0 };
                memcpy(recv20, recvTime, 20);
                std::string recvStr = recv20;
                time_t recvTimeT = Wdrecv_StringToTime_t(recvStr);
                time_t sqlTimeT = Wdrecv_StringToTime_t(str01.toStdString());
                if (sqlTimeT < recvTimeT)
                {
                    continue;
                }
            }

            QString str02 = query.value(2).toString();
            QString str03 = query.value(3).toString();
            QString str04 = query.value(4).toString();
            QString str05 = query.value(5).toString();
            QString str06 = query.value(6).toString();
            QString str07 = query.value(7).toString();
            QString str08 = query.value(8).toString();
            QString str09 = query.value(9).toString();
            QString str10 = query.value(10).toString();

            MessageRecv tmpMessage;
            tmpMessage.ActiveID = str00.toInt();
            tmpMessage.recvTime = str01.toStdString();
            tmpMessage.sendID = str02.toInt();
            tmpMessage.messageType = str04.toStdString();
            if (str03.toInt() == 0)
            {
                tmpMessage.isSeeked = false;
            }
            else
            {
                tmpMessage.isSeeked = true;
            }
            tmpMessage.messageTheme = str05.toStdString();
            tmpMessage.messageContent = str06.toStdString();
            tmpMessage.messageState = str07.toStdString();
            tmpMessage.messageGrade = str08.toStdString();
            tmpMessage.messageRetain = str09.toStdString();
            tmpMessage.messageAllContent = str10.toStdString();

            reVecs.push_back(tmpMessage);
        }
    }
    return reVecs;
}

bool qnzkna::TaskManage::ITaskManageImpl::setMessageToFalseByActionIDAndTime(const int activeID, const char *recvTime, const int sendID, const char *messageType)
{
    QMutexLocker locker(&MutexMessageRecv);
    QSqlQuery query;

    QString strSql("UPDATE messageRecvList set ");
    strSql += "`SFCK` = '" + QString::number(1) + "',";
    strSql += "where `XDBH` = '" + QString::number(activeID) + "'";
    strSql += "`JSSJ` = '" + QString(recvTime) + "'";
    strSql += "`FSF` = '" + QString::number(sendID) + "'";
    strSql += "`LX` = '" + QString(messageType) + "'";

    bool success = query.exec(strSql);
    return success;
}

std::vector<MessageRecv> qnzkna::TaskManage::ITaskManageImpl::getMessageVecForActiveID(const int activeID, const string recvTime)
{
    QMutexLocker locker(&MutexMessageRecv);
    std::vector<MessageRecv> reVecs;
    reVecs.clear();

    QSqlQuery query;
    QString queryStr = "SELECT  * FROM `messageRecvList` WHERE `XDBH` = ";
    queryStr += QString::number(activeID);
    if (bool success = query.exec(queryStr))
    {
        while (query.next())
        {
			QString str00 = tr2(query.value(0).toString().toLatin1().data());
			QString str01 = tr2(query.value(1).toString().toLatin1().data());
            if (recvTime != "0")
            {
                time_t recvTimeT = Wdrecv_StringToTime_t(recvTime);
                time_t sqlTimeT = Wdrecv_StringToTime_t(str01.toStdString());
                if (sqlTimeT < recvTimeT)
                {
                    continue;
                }
            }

            QString str02 = query.value(2).toString();
            QString str03 = query.value(3).toString();
            QString str04 = query.value(4).toString();
            QString str05 = query.value(5).toString();
            QString str06 = query.value(6).toString();
            QString str07 = query.value(7).toString();
            QString str08 = query.value(8).toString();
            QString str09 = query.value(9).toString();
            QString str10 = query.value(10).toString();

            MessageRecv tmpMessage;
            tmpMessage.ActiveID = str00.toInt();
            tmpMessage.recvTime = str01.toStdString();
            tmpMessage.sendID = str02.toInt();
            tmpMessage.messageType = std::string(str04.toLocal8Bit());
            if (str03.toInt() == 0)
            {
                tmpMessage.isSeeked = false;
            }
            else
            {
                tmpMessage.isSeeked = true;
            }
            tmpMessage.messageTheme = std::string(str05.toLocal8Bit());
            tmpMessage.messageContent = std::string(str06.toLocal8Bit());
            tmpMessage.messageState = std::string(str07.toLocal8Bit());
            tmpMessage.messageGrade = std::string(str08.toLocal8Bit());
            tmpMessage.messageRetain = std::string(str09.toLocal8Bit());
            tmpMessage.messageAllContent = std::string(str10.toLocal8Bit());

            reVecs.push_back(tmpMessage);
        }
    }
    return reVecs;
}

bool qnzkna::TaskManage::ITaskManageImpl::setMessageToFalseByActionIDAndTime(const int activeID, const string recvTime, const int sendID, const string messageType)
{
    QMutexLocker locker(&MutexMessageRecv);
    QSqlQuery query;

    QString strSql("UPDATE messageRecvList set ");
    strSql = strSql + "`SFCK` = " + QString::number(1) + " ";
    strSql = strSql + "where `XDBH` = " + QString::number(activeID) + " ";
    strSql = strSql + "AND `JSSJ` = '" + QString::fromStdString(recvTime) + "' ";
    strSql = strSql + "AND `FSF` = " + QString::number(sendID) + " ";
    strSql = strSql + "AND `LX` = '" + QString::fromStdString(messageType) + "'";

    bool success = query.exec(strSql);
    return success;
}

bool qnzkna::TaskManage::ITaskManageImpl::saveMessageRecv(MessageRecv message)
{
    QMutexLocker locker(&MutexMessageRecv);
    QSqlQuery query;

    QString strSql("INSERT INTO messageRecvList (`XDBH`, `JSSJ`, `FSF`, `SFCK`, `LX`, `WDZT`, `WDNR`, `ZT`, `WDDJ`, `BL`, `TCNR`) VALUES (");
    strSql +=  QString::number(message.ActiveID) + ", ";
    strSql += "'" + QString::fromStdString(message.recvTime) + "'" + ", ";
    strSql += QString::number(message.sendID) + ", ";
    if (message.isSeeked)
    {
        strSql += QString::number(1) + ", ";
    }
    else
    {
        strSql +=  QString::number(0) + ", ";
    }
	strSql += "'" + QString::fromLocal8Bit(message.messageType.c_str()) + "', ";
	strSql += "'" + QString::fromLocal8Bit(message.messageTheme.c_str()) + "', ";
	strSql += "'" + QString::fromLocal8Bit(message.messageContent.c_str()) + "', ";
	strSql += "'" + QString::fromLocal8Bit(message.messageState.c_str()) + "', ";
	strSql += "'" + QString::fromLocal8Bit(message.messageGrade.c_str()) + "', ";
	strSql += "'" + QString::fromLocal8Bit(message.messageRetain.c_str()) + "', ";
	QString content = QString::fromLocal8Bit(message.messageAllContent.c_str()).replace("'", "\\'");
    strSql += "'" + content + "' ";
    strSql += ")";

    bool success = query.exec(strSql);
    return success;
}

std::string qnzkna::TaskManage::ITaskManageImpl::getLoginName()
{
    return loginName;
}

CNebulaPlatNature qnzkna::TaskManage::ITaskManageImpl::GetPlatNatureInfoByPlatName(const std::string strPlatName)
{
    CNebulaPlatNature newPlatNature;
    if (m_NebulaPlatNatureVec.empty())
    {
        GetPlatNatureData();
    }
    for (auto data : m_NebulaPlatNatureVec)
    {
        if (tr2(data.GetPlatName().c_str()).toStdString() == strPlatName)
        {
            newPlatNature = data;
            break;
        }
    }
    return newPlatNature;
}

CNebulaAmmoNature qnzkna::TaskManage::ITaskManageImpl::GetAmmoNatureInfoByAmmoName(const std::string strAmmoName)
{
    CNebulaAmmoNature newAmmoNature;
    if (m_NebulaAmmoNatureVec.empty())
    {
        GetAmmoNatureData();
    }
    for (auto data : m_NebulaAmmoNatureVec)
    {
        if (tr2(data.GetAmmoName().c_str()).toStdString() == strAmmoName)
        {
            newAmmoNature = data;
            break;
        }
    }
    return newAmmoNature;
}

CNebulaPodNature qnzkna::TaskManage::ITaskManageImpl::GetPodNatureInfoByPodName(const std::string strPodName)
{
    CNebulaPodNature newPodNature;
    if (m_NebulaPodNatureVec.empty())
    {
        GetPodNatureData();
    }
    for (auto data : m_NebulaPodNatureVec)
    {
        if (tr2(data.GetPodName().c_str()).toStdString() == strPodName)
        {
            newPodNature = data;
            break;
        }
    }
    return newPodNature;
}

void qnzkna::TaskManage::ITaskManageImpl::LoadMissionRelation_PlatRoute()
{
    LoadMissionRelation_PlatRouteDB();
}

CNebulaMissionRelation_PlatRouteVec qnzkna::TaskManage::ITaskManageImpl::GetMissionRelation_PlatRouteData()
{
    return m_NebulaMissionRelation_PlatRouteVec;
}

bool qnzkna::TaskManage::ITaskManageImpl::AddMissionRelation_PlatRoute(const CNebulaMissionRelation_PlatRoute &data)
{
    QMutexLocker locker(&sMutexMissionRelation_PlatRoute);

    if (AddMissionRelation_PlatRouteDB(data) == false)
    {
        return false;
    }

    m_NebulaMissionRelation_PlatRouteVec.push_back(data);
    return true;
}

bool qnzkna::TaskManage::ITaskManageImpl::RemoveMissionRelation_PlatRoute(std::string sMissionID)
{
    QMutexLocker locker(&sMutexMissionRelation_PlatRoute);

    if (IsMissionRelation_PlatRouteDBMissionIDDB(sMissionID) == false)
    {
        return false;
    }
    DeleteMissionRelation_PlatRouteDB(sMissionID);
    CNebulaMissionRelation_PlatRouteVec         TempNebulaMissionRelation_PlatRouteVec;
    auto itr = m_NebulaMissionRelation_PlatRouteVec.begin();
    for (; itr != m_NebulaMissionRelation_PlatRouteVec.end(); itr++)
    {
        if (itr->GetTaskID() != sMissionID)
        {
            TempNebulaMissionRelation_PlatRouteVec.push_back(*itr);
        }
    }
    m_NebulaMissionRelation_PlatRouteVec = TempNebulaMissionRelation_PlatRouteVec;
    return true;
}

bool qnzkna::TaskManage::ITaskManageImpl::RemoveRowMissionRelation_PlatRoute(std::string sMissionID, int nRouteID)
{
    QMutexLocker locker(&sMutexMissionRelation_PlatRoute);

    if (IsMissionPouteRelation_PlatRouteDBMissionIDDB(sMissionID, nRouteID) == false)
    {
        return false;
    }
    DeleteRowMissionRelation_PlatRouteDB(sMissionID, nRouteID);
    CNebulaMissionRelation_PlatRouteVec         TempNebulaMissionRelation_PlatRouteVec;
    auto itr = m_NebulaMissionRelation_PlatRouteVec.begin();
    while (itr != m_NebulaMissionRelation_PlatRouteVec.end())
    {
        if (itr->GetTaskID() == sMissionID && itr->GetRouteID() == nRouteID)
        {
            itr = m_NebulaMissionRelation_PlatRouteVec.erase(itr);
        }
        else
        {
            ++itr;
        }
    }
    return true;
}

CNebulaMissionRelation_PlatRouteVec qnzkna::TaskManage::ITaskManageImpl::GetMissionIDByPlatRoute(const std::string sMissionID)
{
    CNebulaMissionRelation_PlatRouteVec stMissionRelation_PlatRouteVec;
    for (auto data : m_NebulaMissionRelation_PlatRouteVec)
    {
        if (data.GetTaskID() == sMissionID)
        {
            stMissionRelation_PlatRouteVec.push_back(data);
        }
    }
    return stMissionRelation_PlatRouteVec;
}

int qnzkna::TaskManage::ITaskManageImpl::GetRelation_PlatRouteNumber()
{
    return m_NebulaMissionRelation_PlatRouteVec.back().GetID() + 1;
}

CResourceCarryingVec qnzkna::TaskManage::ITaskManageImpl::GetResourceCarryingPlanInfo()
{
    if (m_CResourceCarryingVec.empty())
    {
        LoadResourceCarryingPlanDB();
    }
    return m_CResourceCarryingVec;
}

bool qnzkna::TaskManage::ITaskManageImpl::AddCarryingResource(const CResourceCarrying &data)
{
    bool success = AddResourceCarryingDB(data);
    return success;
}

bool qnzkna::TaskManage::ITaskManageImpl::DeleteCarryingResource()
{
    bool success = DeleteResourceCarryingDB();
    return success;
}

int qnzkna::TaskManage::ITaskManageImpl::GetCarryingPlanMaxBH()
{
    int maxBH = GetCarryingPlanMaxBHDB();
    return maxBH;
}

CNeubulaOperationalInformationVec qnzkna::TaskManage::ITaskManageImpl::GetOperationalInformation()
{
    LoadOperationalInformationDB();
    return m_COperationalInformationVec;
}

bool qnzkna::TaskManage::ITaskManageImpl::AddOperationalInformation(const CNeubulaOperationalInformation &data)
{
    bool success = AddOperationalInformationDB(data);
    return success;
}

bool qnzkna::TaskManage::ITaskManageImpl::DeleteOperationalInformation(int ntype, std::string sMissionID)
{
    QMutexLocker locker(&sMutexOperationalInformation);

    if (IsOperationalInformationDB(ntype, sMissionID) == false)
    {
        return false;
    }
    DeleteOperationalInformationDB(ntype, sMissionID);
    return true;
}

bool qnzkna::TaskManage::ITaskManageImpl::DeleteOperationalInformation(int ntype, std::string sMissionID, std::string sMarkID)
{
    QMutexLocker locker(&sMutexOperationalInformation);

    if (IsOperationalInformationDB(ntype, sMissionID, sMarkID) == false)
    {
        return false;
    }
    DeleteOperationalInformationDB(ntype, sMissionID, sMarkID);
    return true;
}

int qnzkna::TaskManage::ITaskManageImpl::GetRelationTaskMaxID()
{
    QSqlQuery query;
    int nNoMax = 0;
    bool success = query.exec("SELECT MAX(`BH`) from Relation_RWLB_PT_GZ");
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

int qnzkna::TaskManage::ITaskManageImpl::GetRelationPtMaxID()
{
    return GetMaxRelationPtID();
}

int qnzkna::TaskManage::ITaskManageImpl::GetRelationSensorMaxID()
{
    return GetMaxRelationSensorID();
}

int qnzkna::TaskManage::ITaskManageImpl::GetRelationAmmoMaxID()
{
    return GetMaxRelationAmmoID();
}

int qnzkna::TaskManage::ITaskManageImpl::GetRelationTaskPtMaxID()
{
    return GetMaxRelationTaskPtID();
}

CActionMarkRankVec qnzkna::TaskManage::ITaskManageImpl::GetActionMarkRankList(const int &nActionID)
{
    CActionMarkRankVec NewVec;
    for (auto data : m_ActionMarkRankVec)
    {
        if (data.GetActionID() == nActionID)
        {
            NewVec.push_back(data);
        }
    }
    return NewVec;
}

int qnzkna::TaskManage::ITaskManageImpl::GetActionMarkRankListMaxID()
{
    QSqlQuery query;
    int nNoMax = 0;
    bool success = query.exec("SELECT MAX(`BH`) from ZZXD_JIPTL");
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

bool qnzkna::TaskManage::ITaskManageImpl::AddActionMarkRankList(CActionMarkRank &data)
{
    if (IsMarkRankListDBExisted(data) == true)
    {
        return false;
    }

    m_ActionMarkRankVec.push_back(data);

    AddActionMarkRankListDB(data);

    return true;
}

void qnzkna::TaskManage::ITaskManageImpl::LoadRelationMissionPlatDB()
{
    LoadRelationMissionPtDBre();
}

CRelationMissionResourceVec qnzkna::TaskManage::ITaskManageImpl::GetAllRelationMissionPt()
{
    return m_RelationMissionPtVec;
}

CRelationMissionResourceVec qnzkna::TaskManage::ITaskManageImpl::GetRelationMissionPtResource(const std::string &strMissionID)
{
    LoadRelationMissionPtDBre();
    CRelationMissionResourceVec VecNew;
    for (auto data : m_RelationMissionPtVec)
    {
        if (0 == strcmp(data.GetMissionID().c_str(), strMissionID.c_str()))
        {
            VecNew.push_back(data);
        }
    }
    return VecNew;
}

bool qnzkna::TaskManage::ITaskManageImpl::RmoveMissionRelationPt(const std::string &strMissionID)
{
    bool bresult = RemoveRelationMissionPtDBData(strMissionID);
    LoadRelationMissionPtDB();
    return bresult;
}

bool qnzkna::TaskManage::ITaskManageImpl::AddMissionRelationPt(CRelationMissionResource &data)
{
    if (IsRelationMissionPtExisted(data) == true)
    {
        return false;
    }

    m_RelationMissionPtVec.push_back(data);
    bool bresult = AddRelationMissionPtDBData(data);
    return bresult;
}

int qnzkna::TaskManage::ITaskManageImpl::GetRelationMissionPtMaxID()
{
    return GetMaxRelationMissionPtID();
}

bool qnzkna::TaskManage::ITaskManageImpl::IsRelationMissionPtExisted(const CRelationMissionResource &data)
{
    for (auto dataRelation : m_RelationMissionPtVec)
    {
        if ((0 == strcmp(data.GetMissionID().c_str(), dataRelation.GetMissionID().c_str())) &&
                (0 == strcmp(data.GetResourceName().c_str(), dataRelation.GetResourceName().c_str()))
           )
        {
            return true;
        }
    }
    return false;

}

bool qnzkna::TaskManage::ITaskManageImpl::AddPlatFormMatch(std::string PlatTypeName, std::string weaponName, std::string MissionID, std::string CtrlerName)
{
    if (isPlatFormMatchExisted(PlatTypeName, weaponName, MissionID, CtrlerName) == true)
    {
        return false;
    }

    QMutexLocker locker(&sRePlatMatch);

    QString strSql("INSERT INTO RWSJ_PTPP (`ZBXH`, `ZBBH`, `RWBH`, `KZRY`) VALUES (");

    strSql += "'" + QString::fromStdString(PlatTypeName) + "'" + ",";
    strSql += "'" + QString::fromStdString(weaponName) + "'" + ",";
    strSql += "'" + QString::fromStdString(MissionID) + "'" + ",";
    strSql += "'" + QString::fromStdString(CtrlerName) + "'";
    strSql += ")";
    QSqlQuery query;
    bool success = query.exec(strSql);
    return success;
}

std::vector<std::string> qnzkna::TaskManage::ITaskManageImpl::readPlatFormMatch(std::string PlatTypeName, std::string MissionID)
{
    std::vector<std::string> reStrs;
    QMutexLocker locker(&sRePlatMatch);
    QSqlQuery query;
    QString strSql = ("SELECT `ZBBH` FROM `RWSJ_PTPP` where ZBXH = \"");
    strSql += QString::fromStdString(PlatTypeName);
    strSql += "\" AND `RWBH` = \"" + QString::fromStdString(MissionID) + "\"";
    bool success = query.exec(strSql);
    if (success)
    {
        reStrs.clear();

        while (query.next())
        {
            reStrs.push_back(query.value(0).toString().toStdString());
        }
    }
    return reStrs;
}

std::string qnzkna::TaskManage::ITaskManageImpl::readPlatFormMatchZBXH(std::string WeaponID, std::string MissionID)
{
    QMutexLocker locker(&sRePlatMatch);
    QSqlQuery query;
    QString strSql = ("SELECT `ZBXH` FROM `RWSJ_PTPP` where ZBBH = \"");
    strSql += QString::fromStdString(WeaponID);
    strSql += "\" AND `RWBH` = \"" + QString::fromStdString(MissionID) + "\"";
    bool success = query.exec(strSql);
    if (success)
    {
        while (query.next())
        {
            return query.value(0).toString().toStdString();
        }
    }
    return std::string("");
}

std::string qnzkna::TaskManage::ITaskManageImpl::readPlatFormMatchKZRY(std::string WeaponID, std::string MissionID)
{
    QMutexLocker locker(&sRePlatMatch);
    QSqlQuery query;
    QString strSql = ("SELECT `KZRY` FROM `RWSJ_PTPP` where ZBBH = \"");
    strSql += QString::fromStdString(WeaponID);
    strSql += "\" AND `RWBH` = \"" + QString::fromStdString(MissionID) + "\"";
    bool success = query.exec(strSql);
    if (success)
    {
        while (query.next())
        {
            return query.value(0).toString().toStdString();
        }
    }
    return std::string("");
}

std::vector<std::string> qnzkna::TaskManage::ITaskManageImpl::readPlatCtrlByMissionIDPlatName(std::string PlatTypeName, std::string MissionID)
{
    std::vector<std::string> reStrs;
    QMutexLocker locker(&sRePlatMatch);
    QSqlQuery query;
    QString strSql = ("SELECT `KZRY` FROM `RWSJ_PTPP` where ZBXH = \"");
    strSql += QString::fromStdString(PlatTypeName);
    strSql += "\" AND `RWBH` = \"" + QString::fromStdString(MissionID) + "\"";
    bool success = query.exec(strSql);
    if (success)
    {
        reStrs.clear();

        while (query.next())
        {
            reStrs.push_back(query.value(0).toString().toStdString());
        }
    }
    return reStrs;
}

std::vector<std::string> qnzkna::TaskManage::ITaskManageImpl::readPlatFormMatch(std::string MissionID)
{
    std::vector<std::string> reStrs;
    QMutexLocker locker(&sRePlatMatch);
    QSqlQuery query;
    QString strSql = ("SELECT `ZBBH` FROM `RWSJ_PTPP` ");
    strSql += "where `RWBH` = '" + tr2(MissionID.c_str()) + "'";
    bool success = query.exec(strSql);
    if (success)
    {
        reStrs.clear();

        while (query.next())
        {
            reStrs.push_back(query.value(0).toString().toStdString());
        }
    }
    return reStrs;
}

bool qnzkna::TaskManage::ITaskManageImpl::isPlatFormMatchExisted(std::string PlatTypeName, std::string weaponName, std::string MissionID, std::string CtrlerName)
{
    QString strSql("SELECT  * FROM `RWSJ_PTPP` WHERE `ZBXH` = ");
    strSql += "'" + QString::fromStdString(PlatTypeName) + "'";
    strSql += "AND `ZBBH` = '" + QString::fromStdString(weaponName) + "'";
    strSql += "AND `RWBH` = '" + QString::fromStdString(MissionID) + "'";
    strSql += "AND `KZRY` = '" + QString::fromStdString(CtrlerName) + "'";

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

bool qnzkna::TaskManage::ITaskManageImpl::delPlatFormMatch(std::string PlatTypeName, std::string MissionID)
{
    QString strSql("DELETE FROM RWSJ_PTPP where ZBXH = \"");
    strSql += QString::fromStdString(PlatTypeName);
    strSql += "\" AND `RWBH` = \"" + QString::fromStdString(MissionID) + "\"";
    QSqlQuery query;
    bool success = query.exec(strSql);
    return success;
}

std::vector<std::string> qnzkna::TaskManage::ITaskManageImpl::getRouteLineNames()
{
    std::vector<std::string> reNames;
    reNames.clear();
    QSqlQuery query;
    QString strSql = ("SELECT `QYNM` FROM `t_a_zone` where QYXZ = '8'");

    bool success = query.exec(strSql);
    if (success)
    {
        while (query.next())
        {
            reNames.push_back(query.value(0).toString().toStdString());
        }
    }
    return reNames;

}

std::vector<QVector3D> qnzkna::TaskManage::ITaskManageImpl::getRouteLinePointsForName(std::string routeName)
{
    std::vector<QVector3D> rePoints;
    rePoints.clear();

    QSqlQuery query;
    QString strSql = ("SELECT `XH`, `JD`, `WD` FROM `t_a_zone_p` where QYNM = \"");
    strSql = strSql + QString::fromStdString(routeName) + "\" order by `XH` asc";

    bool success = query.exec(strSql);
    if (success)
    {
        while (query.next())
        {
            rePoints.push_back(QVector3D(query.value(0).toDouble(), query.value(1).toDouble(), query.value(2).toDouble()));
        }
    }
    return rePoints;

}

std::map<std::string, int> qnzkna::TaskManage::ITaskManageImpl::GetUserNameAndRole()
{
    std::map<std::string, int> nameRoles;
    nameRoles.clear();
    QSqlQuery query;
    bool success = query.exec("SELECT name,role FROM user");
    if (success)
    {
        while (query.next())
        {
            nameRoles[query.value(0).toString().toStdString()] = query.value(1).toString().toInt();
        }
    }
    return nameRoles;
}

CNebulaSchemeVec qnzkna::TaskManage::ITaskManageImpl::GetAllScheme()
{
    QMutexLocker locker(&sFsMutex2);
    return m_NebulaSchemeVec;
}

CNebulaSchemePointVec qnzkna::TaskManage::ITaskManageImpl::GetAllSchemePoint()
{
    QMutexLocker locker(&sFsMutex2);
    return m_NebulaSchemePointVec;
}

CNebulaSchemeAmmoVec qnzkna::TaskManage::ITaskManageImpl::GetAllSchemeAmmo()
{
    QMutexLocker locker(&sFsMutex2);
    return m_NebulaSchemeAmmoVec;
}

CNebulaSchemeSensorVec qnzkna::TaskManage::ITaskManageImpl::GetAllSchemeSensor()
{
    QMutexLocker locker(&sFsMutex2);
    return m_NebulaSchemeSensorVec;
}

CNebulaSchemePointFAVec qnzkna::TaskManage::ITaskManageImpl::GetAllSchemePointFA()
{
    QMutexLocker locker(&sFsMutex2);
    return m_NebulaSchemePointFAVec;
}

CNebulaActionSchemeGLVec qnzkna::TaskManage::ITaskManageImpl::GetAllActionSchemeGL()
{
    QMutexLocker locker(&sFsMutex2);
    return m_NebulaActionSchemeGLVec;
}

NebulaScheme qnzkna::TaskManage::ITaskManageImpl::GetSchemeByID(const std::string strSchemeID)
{
    NebulaScheme newScheme;
    for (auto data : m_NebulaSchemeVec)
    {
        if (data.GetSchemeID() == strSchemeID)
        {
            newScheme = data;
        }
    }
    return newScheme;
}

CNebulaSchemePointVec qnzkna::TaskManage::ITaskManageImpl::GetPointsByPointID(const std::string strPointID)
{
    CNebulaSchemePointVec newVec;
    for (auto data : m_NebulaSchemePointVec)
    {
        if (data.GetPointID() == strPointID)
        {
            newVec.push_back(data);
        }
    }
    return newVec;
}

CNebulaSchemeAmmoVec qnzkna::TaskManage::ITaskManageImpl::GetAmmoBySchemeID(const std::string strSchemeID)
{
    LoadSchemeAmmoDB();
    CNebulaSchemeAmmoVec newVec;
    for (auto data : m_NebulaSchemeAmmoVec)
    {
        if (data.GetSchemeID() == strSchemeID)
        {
            newVec.push_back(data);
        }
    }
    return newVec;
}

CNebulaSchemeSensorVec qnzkna::TaskManage::ITaskManageImpl::GetSensorBySchemeID(const std::string strSchemeID)
{
    LoadSchemeSensorDB();
    CNebulaSchemeSensorVec newVec;
    for (auto data : m_NebulaSchemeSensorVec)
    {
        if (data.GetSchemeID() == strSchemeID)
        {
            newVec.push_back(data);
        }
    }
    return newVec;
}

CNebulaSchemePointFAVec qnzkna::TaskManage::ITaskManageImpl::GetPointsByFAPointID(const std::string strSchemeID)
{
    LoadSchemePointFADB();
    CNebulaSchemePointFAVec newVec;
    for (auto data : m_NebulaSchemePointFAVec)
    {
        if (data.GetSchemeID() == strSchemeID)
        {
            newVec.push_back(data);
        }
    }
    return newVec;
}

CNebulaSchemeVec qnzkna::TaskManage::ITaskManageImpl::GetSchemeByActionID(const int strActionID)
{
    CNebulaSchemeVec newVec;
    for (auto data : m_NebulaActionSchemeGLVec)
    {
        if (data.GetActionID() == strActionID)
        {
            for (auto temp : m_NebulaSchemeVec)
            {
                if (temp.GetSchemeID() == data.GetSchemeID())
                {
                    newVec.push_back(temp);
                }
            }
        }
    }
    return newVec;
}

CNebulaSchemeVec qnzkna::TaskManage::ITaskManageImpl::GetSchemeByMarkID(const std::string strMarkID)
{
    LoadSchemeDB();
    CNebulaSchemeVec newVec;
    for (auto data : m_NebulaSchemeVec)
    {
        if (data.GetMarkID() == strMarkID)
        {
            newVec.push_back(data);
        }
    }
    return newVec;
}

bool qnzkna::TaskManage::ITaskManageImpl::RmoveSchemeByID(NebulaScheme &data)
{
    QMutexLocker locker(&sFsMutex3);

    if (IsSchemeDBExisted(data) == false)
    {
        return false;
    }

    auto itr = m_NebulaSchemeVec.begin();
    for (; itr != m_NebulaSchemeVec.end(); itr++)
    {
        if ((data.GetSchemeID() == itr->GetSchemeID()))
        {
            m_NebulaSchemeVec.erase(itr);
            break;
        }
    }
    DeleteSchemeDB(data);
    return true;
}

bool qnzkna::TaskManage::ITaskManageImpl::RmovePointsByPointID(NebulaSchemePoint &data)
{
    QMutexLocker locker(&sFsMutex3);

    if (IsSchemePointDBExisted(data) == false)
    {
        return false;
    }

    auto itr = m_NebulaSchemePointVec.begin();
    for (; itr != m_NebulaSchemePointVec.end(); itr++)
    {
        if ((data.GetPointID() == itr->GetPointID()))
        {
            m_NebulaSchemePointVec.erase(itr);
            break;
        }
    }
    DeleteSchemePointDB(data);
    return true;
}

bool qnzkna::TaskManage::ITaskManageImpl::RmoveSchemeGLPointsByData(NebulaSchemePointFA &data)
{
    QMutexLocker locker(&sFsMutex3);

    if (IsSchemePointFADBExisted(data) == false)
    {
        return false;
    }

    auto itr = m_NebulaSchemePointFAVec.begin();
    for (; itr != m_NebulaSchemePointFAVec.end(); itr++)
    {
        if (data.GetPointID() == itr->GetPointID() && data.GetSchemeID() == itr->GetSchemeID())
        {
            m_NebulaSchemePointFAVec.erase(itr);
            break;
        }
    }
    DeleteSchemePointFADB(data);
    LoadSchemePointFADB();
    return true;
}

bool qnzkna::TaskManage::ITaskManageImpl::RmoveActionGLSchemeByData(NebulaActionSchemeGL &data)
{
    QMutexLocker locker(&sFsMutex3);

    if (IsActionSchemeGLDBExisted(data) == false)
    {
        return false;
    }

    auto itr = m_NebulaActionSchemeGLVec.begin();
    for (; itr != m_NebulaActionSchemeGLVec.end(); itr++)
    {
        if (data.GetActionID() == itr->GetActionID() && data.GetSchemeID() == itr->GetSchemeID())
        {
            m_NebulaActionSchemeGLVec.erase(itr);
            break;
        }
    }
    DeleteActionSchemeGLDB(data);
    return true;
}

bool qnzkna::TaskManage::ITaskManageImpl::RmoveAmmoBySchemeID(NebulaSchemeAmmo &data)
{
    QMutexLocker locker(&sFsMutex3);

    if (IsSchemeAmmoDBExisted(data) == false)
    {
        return false;
    }

    auto itr = m_NebulaSchemeAmmoVec.begin();
    for (; itr != m_NebulaSchemeAmmoVec.end(); itr++)
    {
        if ((data.GetSchemeID() == itr->GetSchemeID()) && (data.GetID() == itr->GetID()))
        {
            m_NebulaSchemeAmmoVec.erase(itr);
            break;
        }
    }
    DeleteSchemeAmmoDB(data);
    return true;
}

bool qnzkna::TaskManage::ITaskManageImpl::RmoveSensorBySchemeID(NebulaSchemeSensor &data)
{
    QMutexLocker locker(&sFsMutex3);

    if (IsSchemeSensorDBExisted(data) == false)
    {
        return false;
    }

    auto itr = m_NebulaSchemeSensorVec.begin();
    for (; itr != m_NebulaSchemeSensorVec.end(); itr++)
    {
        if ((data.GetSchemeID() == itr->GetSchemeID()) && (data.GetID() == itr->GetID()))
        {
            m_NebulaSchemeSensorVec.erase(itr);
            break;
        }
    }
    DeleteSchemeSensorDB(data);
    return true;
}

bool qnzkna::TaskManage::ITaskManageImpl::AddScheme(NebulaScheme &data)
{
    if (IsSchemeDBExisted(data) == true)
    {
        EditScheme(data);
        return false;
    }

    m_NebulaSchemeVec.push_back(data);
    AddSchemeDB(data);

    return true;
}

bool qnzkna::TaskManage::ITaskManageImpl::AddSchemePoint(NebulaSchemePoint &data)
{
    if (IsSchemePointDBExisted(data) == true)
    {
        EditSchemePoint(data);
        return false;
    }

    m_NebulaSchemePointVec.push_back(data);
    AddSchemePointDB(data);
    return true;
}

bool qnzkna::TaskManage::ITaskManageImpl::AddSchemeFAPoint(NebulaSchemePointFA &data)
{
    m_NebulaSchemePointFAVec.clear();
    if (IsSchemePointFADBExisted(data) == true)
    {
        return false;
    }

    m_NebulaSchemePointFAVec.push_back(data);
    AddSchemePointFADB(data);
    return true;
}

bool qnzkna::TaskManage::ITaskManageImpl::AddActionSchemeGL(NebulaActionSchemeGL &data)
{
    if (IsActionSchemeGLDBExisted(data) == true)
    {
        return false;
    }

    m_NebulaActionSchemeGLVec.push_back(data);
    AddActionSchemeGLDB(data);
    return true;
}

bool qnzkna::TaskManage::ITaskManageImpl::AddAmmoBySchemeID(NebulaSchemeAmmo &data)
{
    m_NebulaSchemeAmmoVec.clear();
    if (IsSchemeAmmoDBExisted(data) == true)
    {
        EditSchemeAmmo(data);
        return false;
    }

    m_NebulaSchemeAmmoVec.push_back(data);
    AddSchemeAmmoDB(data);
    return true;
}

bool qnzkna::TaskManage::ITaskManageImpl::AddSensorBySchemeID(NebulaSchemeSensor &data)
{
    m_NebulaSchemeSensorVec.clear();
    if (IsSchemeSensorDBExisted(data) == true)
    {
        EditSchemeSensor(data);
        return false;
    }

    m_NebulaSchemeSensorVec.push_back(data);
    AddSchemeSensorDB(data);
    return true;
}

bool qnzkna::TaskManage::ITaskManageImpl::EditScheme(const NebulaScheme &data)
{
    QMutexLocker locker(&sFsMutex3);

    NebulaScheme stRelation = data;
    std::string nID = data.GetSchemeID();
    if (!IsSchemeDBExisted(stRelation))
    {
        return false;
    }

    for (auto &data1 : m_NebulaSchemeVec)
    {
        if (data1.GetSchemeID() == stRelation.GetSchemeID())
        {
            data1 = stRelation;
        }
    }

    bool bresult = EditSchemeDB(stRelation);
    return bresult;
}

bool qnzkna::TaskManage::ITaskManageImpl::EditSchemePoint(const NebulaSchemePoint &data)
{
    QMutexLocker locker(&sFsMutex3);

    NebulaSchemePoint stRelation = data;
    std::string nID = data.GetPointID();
    if (!IsSchemePointDBExisted(stRelation))
    {
        return false;
    }

    for (auto &data1 : m_NebulaSchemePointVec)
    {
        if (data1.GetPointID() == stRelation.GetPointID())
        {
            data1 = stRelation;
        }
    }

    bool bresult = EditSchemePointDB(stRelation);
    return bresult;
}

bool qnzkna::TaskManage::ITaskManageImpl::EditSchemeAmmo(const NebulaSchemeAmmo &data)
{
    QMutexLocker locker(&sFsMutex3);

    NebulaSchemeAmmo stRelation = data;
    std::string nID = data.GetSchemeID();
    if (!IsSchemeAmmoDBExisted(stRelation))
    {
        return false;
    }

    for (auto &data1 : m_NebulaSchemeAmmoVec)
    {
        if (data1.GetSchemeID() == stRelation.GetSchemeID() && data1.GetID() == stRelation.GetID())
        {
            data1 = stRelation;
        }
    }

    bool bresult = EditSchemeAmmoDB(stRelation);
    return bresult;
}

bool qnzkna::TaskManage::ITaskManageImpl::EditSchemeSensor(const NebulaSchemeSensor &data)
{
    QMutexLocker locker(&sFsMutex3);

    NebulaSchemeSensor stRelation = data;
    std::string nID = data.GetSchemeID();
    if (!IsSchemeSensorDBExisted(stRelation))
    {
        return false;
    }

    for (auto &data1 : m_NebulaSchemeSensorVec)
    {
        if (data1.GetSchemeID() == stRelation.GetSchemeID() && data1.GetID() == stRelation.GetID())
        {
            data1 = stRelation;
        }
    }

    bool bresult = EditSchemeSensorDB(stRelation);
    return bresult;
}

int qnzkna::TaskManage::ITaskManageImpl::GetSchemeAmmoMaxID()
{
    return GetMaxSchemeAmmoID();
}

int qnzkna::TaskManage::ITaskManageImpl::GetSchemeSensorMaxID()
{
    return GetMaxSchemeSensorID();
}

NebulaResourcesVec ITaskManageImpl::GetSensorResourceFmDB()
{
    QMutexLocker locker(&sRsMutex1);
    return m_NebulaResourcesSensorVec;
}

NebulaResourcesVec ITaskManageImpl::GetPlatFormResourceFmDB()
{
    QMutexLocker locker(&sRsMutex2);
    return m_NebulaResourcesPtVec;
}

NebulaResourcesVec ITaskManageImpl::GetAmmoResourceFmDB()
{
    QMutexLocker locker(&sRsMutex3);
    return m_NebulaResourcesAmmoVec;
}

CRelationActionResourceVec qnzkna::TaskManage::ITaskManageImpl::GetAllRelationPt()
{
    QMutexLocker locker(&sRsMutex4);
    return m_RelationActionPtVec;
}

CRelationActionResourceVec qnzkna::TaskManage::ITaskManageImpl::GetAllRelationSensor()
{
    QMutexLocker locker(&sRsMutex5);
    return m_RelationActionSensorVec;
}

CRelationActionResourceVec qnzkna::TaskManage::ITaskManageImpl::GetAllRelationAmmo()
{
    QMutexLocker locker(&sRsMutex6);
    return m_RelationActionAmmoVec;
}

CRelationActionResourceVec qnzkna::TaskManage::ITaskManageImpl::GetRelationPtResource(const int &nActionID)
{
    CRelationActionResourceVec NewVec;
    for (auto data : m_RelationActionPtVec)
    {
        if (data.GetActionID() == nActionID)
        {
            NewVec.push_back(data);
        }
    }
    return NewVec;
}

CRelationActionResourceVec qnzkna::TaskManage::ITaskManageImpl::GetRelationSensorResource(const int &nActionID)
{
    CRelationActionResourceVec NewVec;
    for (auto data : m_RelationActionSensorVec)
    {
        if (data.GetActionID() == nActionID)
        {
            NewVec.push_back(data);
        }
    }
    return NewVec;
}

CRelationActionResourceVec qnzkna::TaskManage::ITaskManageImpl::GetRelationAmmoResource(const int &nActionID)
{
    CRelationActionResourceVec NewVec;
    for (auto data : m_RelationActionAmmoVec)
    {
        if (data.GetActionID() == nActionID)
        {
            NewVec.push_back(data);
        }
    }
    return NewVec;
}

bool qnzkna::TaskManage::ITaskManageImpl::RmoveRelationPt(CRelationActionResource &data)
{
    QMutexLocker locker(&sRsMutex4);

    if (IsRelationPtDBExisted(data) == false)
    {
        return false;
    }

    auto itr = m_RelationActionPtVec.begin();
    for (; itr != m_RelationActionPtVec.end(); itr++)
    {
        if ((data.GetActionID() == itr->GetActionID()) &&
                (0 == strcmp(data.GetResourceName().c_str(), itr->GetResourceName().c_str()))
           )
        {
            m_RelationActionPtVec.erase(itr);
            break;
        }
    }

    RemoveRelationPtDBData(data);
    return true;
}

bool qnzkna::TaskManage::ITaskManageImpl::RmoveRelationSensor(CRelationActionResource &data)
{
    QMutexLocker locker(&sRsMutex5);

    if (IsRelationSensorDBExisted(data) == false)
    {
        return false;
    }

    auto itr = m_RelationActionSensorVec.begin();
    for (; itr != m_RelationActionSensorVec.end(); itr++)
    {
        if ((data.GetActionID() == itr->GetActionID()) &&
                (0 == strcmp(data.GetResourceName().c_str(), itr->GetResourceName().c_str()))
           )
        {
            m_RelationActionSensorVec.erase(itr);
            break;
        }
    }

    RemoveRelationSensorDBData(data);
    return true;
}

bool qnzkna::TaskManage::ITaskManageImpl::RmoveRelationAmmo(CRelationActionResource &data)
{
    QMutexLocker locker(&sRsMutex5);

    if (IsRelationAmmoDBExisted(data) == false)
    {
        return false;
    }

    auto itr = m_RelationActionAmmoVec.begin();
    for (; itr != m_RelationActionAmmoVec.end(); itr++)
    {
        if ((data.GetActionID() == itr->GetActionID()) &&
                (0 == strcmp(data.GetResourceName().c_str(), itr->GetResourceName().c_str()))
           )
        {
            m_RelationActionAmmoVec.erase(itr);
            break;
        }
    }

    RemoveRelationAmmoDBData(data);
    return true;
}

bool qnzkna::TaskManage::ITaskManageImpl::AddRelationPt(CRelationActionResource &data)
{
    if (IsRelationPtDBExisted(data) == true)
    {
        EditRelationPt(data);
        return false;
    }

    m_RelationActionPtVec.push_back(data);

    AddRelationPtDBData(data);

    return true;
}

bool qnzkna::TaskManage::ITaskManageImpl::AddRelationSensor(CRelationActionResource &data)
{
    if (IsRelationSensorDBExisted(data) == true)
    {
        EditRelationSensor(data);
        return false;
    }

    m_RelationActionSensorVec.push_back(data);

    AddRelationSensorDBData(data);

    return true;
}

bool qnzkna::TaskManage::ITaskManageImpl::AddRelationAmmo(CRelationActionResource &data)
{
    if (IsRelationAmmoDBExisted(data) == true)
    {
        EditRelationAmmo(data);
        return false;
    }

    m_RelationActionAmmoVec.push_back(data);

    AddRelationAmmoDBData(data);

    return true;
}

bool qnzkna::TaskManage::ITaskManageImpl::EditRelationPt(const CRelationActionResource &data)
{
    QMutexLocker locker(&sMutex4);

    CRelationActionResource stRelation = data;
    int nID = data.GetID();
    if (!IsRelationPtDBExisted(stRelation))
    {
        return false;
    }

    for (auto &data1 : m_RelationActionPtVec)
    {
        if (data1.GetID() == stRelation.GetID())
        {
            data1 = stRelation;
        }
    }

    bool bresult = EditRelationPtDBData(stRelation);
    return bresult;
}

bool qnzkna::TaskManage::ITaskManageImpl::EditRelationSensor(const CRelationActionResource &data)
{
    QMutexLocker locker(&sMutex5);

    CRelationActionResource stRelation = data;
    int nID = data.GetID();
    if (!IsRelationSensorDBExisted(stRelation))
    {
        return false;
    }

    for (auto &data1 : m_RelationActionSensorVec)
    {
        if (data1.GetID() == stRelation.GetID())
        {
            data1 = stRelation;
        }
    }

    bool bresult = EditRelationSensorDBData(stRelation);
    return bresult;
}

bool qnzkna::TaskManage::ITaskManageImpl::EditRelationAmmo(const CRelationActionResource &data)
{
    QMutexLocker locker(&sMutex6);

    CRelationActionResource stRelation = data;
    int nID = data.GetID();
    if (!IsRelationAmmoDBExisted(stRelation))
    {
        return false;
    }

    for (auto &data1 : m_RelationActionAmmoVec)
    {
        if (data1.GetID() == stRelation.GetID())
        {
            data1 = stRelation;
        }
    }

    bool bresult = EditRelationAmmoDBData(stRelation);
    return bresult;
}

int ITaskManageImpl::GetMissionLineMaxID()
{
    QSqlQuery query;
    int nNoMax = 0;
    bool success = query.exec("SELECT MAX(`HXBH`) from RWJS_HXLB");
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

int qnzkna::TaskManage::ITaskManageImpl::GetMissionLinePointMaxID()
{
    QSqlQuery query;
    int nNoMax = 0;
    QString str = ("SELECT MAX(`No`) `No` from RWSJ_HDLB ");
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

int qnzkna::TaskManage::ITaskManageImpl::GetMissionLinePointMaxID(int nLineID)
{
    QSqlQuery query;
    int nNoMax = 0;
    QString str = ("SELECT MAX(`No`) `No` from RWSJ_HDLB WHERE `HXBH`=");
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

CNeubulaMissionLinePointVec qnzkna::TaskManage::ITaskManageImpl::getMissionLinePointsFornLineID(const int nLineID)
{

    CNeubulaMissionLinePointVec reVec;
    reVec.clear();

    QString strSql = "SELECT `No`, `HXBH`, `JD`, `WD`, `GD`, `SD`, `XTSJ`, `GDKZ`, `BJ`, `HXSX`, `ZWSX`, `JLD`, `HXDLX`, `HDMC`, `ZSDASJ` FROM `RWSJ_HDLB` WHERE `HXBH` = ";
    strSql += QString::number(nLineID);
    QSqlQuery queryPoint;
    bool success = queryPoint.exec(strSql);
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
		QString str12 = tr2(queryPoint.value(11).toString().toLatin1().data());
		QString str13 = tr2(queryPoint.value(12).toString().toLatin1().data());
		QString str14 = tr2(queryPoint.value(13).toString().toLocal8Bit().data());
		QString str15 = tr2(queryPoint.value(14).toString().toLatin1().data());

        CNeubulaMissionLinePoint stPoint;
        stPoint.nNo = str01.toInt();
        stPoint.nLineID = str02.toInt();
        stPoint.dLon = str03.toDouble();
        stPoint.dLat = str04.toDouble();
        stPoint.dAlt = str05.toDouble();
        stPoint.dSpeed = str06.toDouble();
        stPoint.dHoverTime = str07.toDouble();
        stPoint.cHeightCtrl = str08.toInt();
        stPoint.DRadius = str09.toDouble();
        stPoint.cLineMode = str10.toInt();
        stPoint.cTurnMode = str11.toInt();
        stPoint.cLanding = str12.toInt();
        stPoint.cType = str13.toInt();
        strncpy(stPoint.cName, str14.toLocal8Bit(), MAX_LINE_LEN_1);
        strncpy(stPoint.dReachTime, str15.toLocal8Bit(), MAX_LINE_LEN_1);

        reVec.push_back(stPoint);
    }
    return reVec;
}

time_t qnzkna::TaskManage::ITaskManageImpl::Wdrecv_StringToTime_t(std::string strTime)
{
    if (int reStr = strTime.find("/") != -1)
    {
        QString tmpStrTime = QString::fromStdString(strTime);
        QStringList dayHour = tmpStrTime.split(" ");
        if (dayHour.size() == 2)
        {
            QStringList YMD = dayHour.at(0).split("/");
            strTime.clear();
            QString qstrTime = YMD.at(0) + "-" + YMD.at(1) + "-" + YMD.at(2) + " ";
            QStringList hms = dayHour.at(1).split(":");
            if (hms.size() < 3)
            {
                qstrTime = qstrTime + hms.at(0) + ":" + hms.at(1) + ":" + "00";
            }
            else
            {
                qstrTime = qstrTime + hms.at(0) + ":" + hms.at(1) + ":" + hms.at(2);
            }
            strTime = qstrTime.toStdString();
        }
    }
    char *cha = (char *)strTime.data();
    tm tm_;
    int year, month, day, hour, minute, second;
    sscanf(cha, "%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &minute, &second);
    second %= 60;
    tm_.tm_year = year - 1900;
    tm_.tm_mon = month - 1;
    tm_.tm_mday = day;
    tm_.tm_hour = hour;
    tm_.tm_min = minute;
    tm_.tm_sec = second;
    tm_.tm_isdst = 0;
    time_t t_ = mktime(&tm_);
    return t_;

}

