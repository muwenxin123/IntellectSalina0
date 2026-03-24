#ifndef IPOSITIONSERVICEIMPL_H
#define IPOSITIONSERVICEIMPL_H

#include <QMutex>

#include "DataManage/ISensorManageService.h"

namespace qnzkna
{
namespace SensorManage
{

class ISensorManageImpl: public ISensorManageService
{
public:
    ISensorManageImpl();

    virtual ~ISensorManageImpl();

public:

    virtual bool GetSensorInfo(const std::string& strSensorId, CSensorInfo* pScenarioInfo);

    virtual bool IsExistSensorInfo(const std::string& strSensorId);

    virtual bool GetSensorInfoList(const std::string& strSensorType, SensorInfoMap* pList);

	virtual bool GetSensorInfosInWeaponComponent(const std::string& strWeaponComponentId, SensorInfoMap* pInfo);

    virtual bool RemoveSensorInfo(const std::string& strSensorId);

    virtual bool SetSensorInfo( CSensorInfo& pSensorInfo);

	virtual bool GetSensorIdsInWeaponComponent(const std::string& strWeaponComponentId, std::vector<std::string>* pSensorId);

    virtual bool GetSensorType(const std::string& strWeaponComponentID, std::string& strType);

private:
    void LoadDatabase();
    void SaveDatabasse();

private:
    mutable QMutex      m_mutex;
   SensorInfoMap        m_mapSensorInfoPtr;
};

}
}

#endif
