#include "ISensorManageImpl.h"
#include "TinyXML/tinyxml.h"

using namespace qnzkna::SensorManage;

ISensorManageImpl::ISensorManageImpl(void)
{
    LoadDatabase();
}

ISensorManageImpl::~ISensorManageImpl(void)
{
    SaveDatabasse();
}

void ISensorManageImpl::LoadDatabase()
{
    m_mapSensorInfoPtr.clear();
    TiXmlDocument docObject;
    std::string strDataPath = "../data/传感器管理文件.xml";
    if (!docObject.LoadFile(strDataPath.c_str()))
    {
        return;
    }
    TiXmlElement *pRootEle = docObject.RootElement();
    if (!pRootEle)
    {
        return;
    }
    TiXmlElement *pSensorInfoEle = pRootEle->FirstChildElement("SensorInfo");
    while (pSensorInfoEle)
    {
        std::string strType = pSensorInfoEle->Attribute("Type");
        std::string strID = pSensorInfoEle->Attribute("ID");
        std::string strName = pSensorInfoEle->Attribute("Name");
        std::string strSimpleName = pSensorInfoEle->Attribute("SimpleName");
        std::string strWeaponComponent = pSensorInfoEle->Attribute("WeaponComponent");
        CSensorInfo info;
        info.SetSensorType(strType);
        info.SetSensorID(strID);
        info.SetSensorName(strName);
        info.SetSensorSimpleName(strSimpleName);
        info.SetComponentID(strWeaponComponent);

        TiXmlElement *pParamListEle = pSensorInfoEle->FirstChildElement("ParameterList");
        if (pParamListEle)
        {
            TiXmlElement *pParamEle = pParamListEle->FirstChildElement("Parameter");
            while (pParamEle)
            {
                std::string strKey = pParamEle->Attribute("Key");
                std::string strValue = pParamEle->Attribute("Value");
                info.SetSensorParam(strKey, strValue);
                pParamEle = pParamEle->NextSiblingElement("Parameter");
            }
        }
        m_mapSensorInfoPtr.insert(std::make_pair(strID, info));
        pSensorInfoEle = pSensorInfoEle->NextSiblingElement("SensorInfo");
    }
}

void ISensorManageImpl::SaveDatabasse()
{

}

bool ISensorManageImpl::GetSensorInfo(const std::string &strSensorId, CSensorInfo *pScenarioInfo)
{
    if ("" == strSensorId || !pScenarioInfo)
    {
        return false;
    }

    QMutexLocker locker(&m_mutex);
    SensorInfoMapItr itrMap = m_mapSensorInfoPtr.find(strSensorId);
    if (itrMap != m_mapSensorInfoPtr.end())
    {
        *pScenarioInfo = itrMap->second;
        return true;
    }
    return false;
}

bool ISensorManageImpl::IsExistSensorInfo(const std::string &strSensorId)
{
    if ("" == strSensorId)
    {
        return false;
    }

    QMutexLocker locker(&m_mutex);
    SensorInfoMapItr itrMap = m_mapSensorInfoPtr.find(strSensorId);
    if (itrMap != m_mapSensorInfoPtr.end())
    {
        return true;
    }
    return false;
}

bool ISensorManageImpl::GetSensorInfoList(const std::string &strSensorType, SensorInfoMap *pList)
{
    if ("" == strSensorType || !pList)
    {
        return false;
    }

    QMutexLocker locker(&m_mutex);
    pList->clear();
    SensorInfoMapItr itrMap = m_mapSensorInfoPtr.begin();
    for (; itrMap != m_mapSensorInfoPtr.end(); itrMap ++)
    {
        if (itrMap->second.GetSensorType() == strSensorType)
        {
            pList->insert(std::make_pair(itrMap->first, itrMap->second));
        }
    }
    return true;
}

bool ISensorManageImpl::GetSensorInfosInWeaponComponent(const std::string &strWeaponComponentId, SensorInfoMap *pInfo)
{
    if ("" == strWeaponComponentId || !pInfo)
    {
        return false;
    }
    QMutexLocker locker(&m_mutex);
    SensorInfoMapItr itrMap = m_mapSensorInfoPtr.begin();
    for (; itrMap != m_mapSensorInfoPtr.end(); itrMap ++)
    {
        if (itrMap->second.GetComponentID() == strWeaponComponentId)
        {
            pInfo->insert(std::make_pair(itrMap->second.GetSensorID(), itrMap->second));
        }
    }
    return true;
}

bool ISensorManageImpl::RemoveSensorInfo(const std::string &strSensorId)
{
    if ("" == strSensorId)
    {
        return false;
    }

    QMutexLocker locker(&m_mutex);
    SensorInfoMapItr itrMap = m_mapSensorInfoPtr.find(strSensorId);
    if (itrMap != m_mapSensorInfoPtr.end())
    {
        m_mapSensorInfoPtr.erase(itrMap);
        return true;
    }
    return false;
}

bool ISensorManageImpl::SetSensorInfo(CSensorInfo &pSensorInfo)
{
    if ("" == pSensorInfo.GetSensorID())
    {
        return false;
    }

    QMutexLocker locker(&m_mutex);
    SensorInfoMapItr itrMap = m_mapSensorInfoPtr.find(pSensorInfo.GetSensorID());
    if (itrMap != m_mapSensorInfoPtr.end())
    {
        itrMap->second = pSensorInfo;
        return true;
    }
    m_mapSensorInfoPtr.insert(std::make_pair(pSensorInfo.GetSensorID(), pSensorInfo));
    return true;
}

bool ISensorManageImpl::GetSensorIdsInWeaponComponent(const std::string &strWeaponComponentId, std::vector<std::string> *pSensorId)
{
    if ("" == strWeaponComponentId || !pSensorId)
    {
        return false;
    }
    QMutexLocker locker(&m_mutex);
    SensorInfoMapItr itrMap = m_mapSensorInfoPtr.begin();
    for (; itrMap != m_mapSensorInfoPtr.end(); itrMap ++)
    {
        if (itrMap->second.GetComponentID() == strWeaponComponentId)
        {
            pSensorId->push_back(itrMap->first);
        }
    }
    return true;
}

bool ISensorManageImpl::GetSensorType(const std::string &strSensorID, std::string &strType)
{
    if ("" == strSensorID)
    {
        return false;
    }

    QMutexLocker locker(&m_mutex);
    SensorInfoMapItr itrMap = m_mapSensorInfoPtr.find(strSensorID);
    if (itrMap != m_mapSensorInfoPtr.end())
    {
        strType = itrMap->second.GetSensorType();
        return true;
    }
    return false;
}
