#ifndef IWEAPONMANAGESERVICEIMPL_H
#define IWEAPONMANAGESERVICEIMPL_H

#include <set>
#include <string>

#include <QMutex>
#include <QXmlStreamReader>

#include "DataManage/IWeaponManageService.h"
#include "DataManage/WeaponSystem/WeaponSystem.h"
#include "DataManage/WeaponSystem/WeaponComponentTypeComparisonInfo.h"
#include "DataManage/WeaponSystem/WeaponSystemTypeComparisonInfo.h"

namespace qnzkna
{
namespace WeaponManage
{

class IWeaponManageServiceImpl : public IWeaponManageService
{
public:
    IWeaponManageServiceImpl();
    virtual ~IWeaponManageServiceImpl(void) {}

    std::string strFilePath() const;
    void setStrFilePath(const std::string &strFilePath);

    virtual std::string CreateWeaponSystemID() const;

    virtual bool GetWeaponSystemMap(WeaponSystemMap& weaponSystemMap) const;

    virtual bool GetWeaponSystemMap(WeaponSystemMap* weaponSystemMap, WeaponSystem_Type weaponSysType, WeaponSystem_Model weaponModelType) const;

    virtual bool SetWeaponSystemShowFlag(const std::string& strSystemID,bool bShowFlag);

    virtual bool SetWeaponSystem(const WeaponSystem& weaponSys);

    virtual bool RemoveWeaponSystem(const std::string& strSysID);

    virtual bool GetWeaponSystem(WeaponSystem* pWeaponSys, const std::string& strSysID) const;

    virtual bool GetWeaponSystemName(std::string& strSysName, const std::string& strSysID) const;

    virtual bool GetWeaponSystemFireRadius(double& dFireRadius, const std::string& strSysID) const;

    virtual bool SetWeaponSystemFireRadius(const std::string& strSysID, double dFireRadius);

    virtual bool GetWeaponSystemModel(WeaponSystem_Type& sysType, WeaponSystem_Model& sysModel, const std::string& strSysID) const;

    virtual bool GetWeaponSystemIdentify(WeaponSystemModelIdentifyInfo* pOutInfo, const std::string& strSysId);

    virtual bool GetWeaponComponentName(std::string& strComponentName, const std::string& strComponentID) const;

    virtual bool GetWeaponComponent(WeaponComponent* pWeaponModel, const std::string& strSysID, unsigned int nIndex) const;

    virtual bool GetWeaponComponent(WeaponComponent* pWeaponModel, const std::string& strModelID) const;

    virtual bool  GetWeaponComponentSeat(std::string& strPosID, unsigned int& nSeatIndex, const std::string& strSysID, unsigned int nModelIndex) const;

    virtual bool GetWeaponComponentSeat(std::string& strPosID, unsigned int& nSeatIndex, const std::string& strModelID) const;

    __declspec(deprecated("** this is a deprecated function: SetWeaponComponent **"))
    virtual bool SetWeaponComponent(const WeaponComponent& weaponModel);

    virtual bool AddWeaponComponent(const WeaponComponent& weaponModel);

    virtual bool SetWeaponComponentShowFlag(const std::string& strComponentID, const bool bShowFlag);

    virtual bool RemoveWeaponComponent(const std::string& strSysID, unsigned int nModelIndex);

    virtual bool RemoveWeaponComponent(const std::string& strComponentID);

	virtual bool GetWeaponSystemTypeComparisonInfoMap(WeaponSystemTypeComparisonInfoMap* pMap) const;

	virtual bool GetWeaponSystemTypeComparisonInfo(WeaponSystemTypeComparisonInfo* pInfo, WeaponSystem_Type nSystemType) const;

	virtual bool RemoveWeaponSystemTypeComparisonInfo(WeaponSystem_Type sysType);

	virtual bool SetWeaponSystemTypeComparisonInfo(const WeaponSystemTypeComparisonInfo& SysCategoryInfo);

	virtual bool GetWeaponComponentTypeComparisonInfoMap(WeaponComponentTypeComparisonInfoMap* pMap) const;

	virtual bool GetWeaponComponentTypeComparisonInfo(WeaponComponentTypeComparisonInfo* pInfo, WeaponComponent_Type nComponentType) const;

	virtual bool RemoveWeaponComponentTypeComparisonInfo(WeaponComponent_Type nWeaponType);

	virtual bool SetWeaponComponentTypeComparisonInfo(const WeaponComponentTypeComparisonInfo& ModelCategoryInfo);

	virtual void GetCWCTypeComMap(WeaponComponentTypeComparisonInfoMap&) const;

	virtual void GetCWSTypeComMap(WeaponSystemTypeComparisonInfoMap&) const;

    virtual bool IsExistWeaponSystem(const std::string& strSysID) const;

    virtual bool GetWeaponSystemCalcModelIndex(unsigned int& calcModeIndex, WeaponSystem_Type sysType, WeaponSystem_Model sysModel) const;

    virtual bool GetWeaponComponentShowFlag(bool& bShowFlags, const std::string& strWaeaponComponentId) const;

private:
    const WeaponComponent *GetWeaponComponent(const std::string& strSystemID, unsigned int nModelIndex) const;

    WeaponSystem* GetWeaponSystem(const std::string& strWeaponSysId);

    WeaponSystemModelIdentifyInfo* GetWSModelIdentifyInfo(const std::string& strSysId, const WeaponSystem_Model weaponSystemModel);

private:
    bool loadWeaponManage(const QString &fileName);
    bool readWeaponManage(QIODevice *device);
    bool loadWeaponComparison(const QString &fileName);
    bool readWeaponComparison(QIODevice *device);

    QString errorString(const QXmlStreamReader& xmlReader) const;

    static inline QString versionAttribute()
    {
        return QStringLiteral("version");
    }
    static inline QString hrefAttribute()
    {
        return QStringLiteral("href");
    }

private:
    void readWeaponSystemList();
    void readWeaponSystem();
    void readWeaponComponent(WeaponSystem& weaponSystem);

private:
    void readWeaponSystemComponentComparisonManage();
    void readWeaponSystemComparisonList();
    void readWeaponSystemTypeList(WeaponSystemTypeComparisonInfo &weaponSystemTypeComparisonInfo);
    void readSystemModel(WeaponSystemModelComparisonInfo &weaponSystemTypeComparisonInfo);
    void readComponentList(WeaponSystemTypeComparisonInfo &weaponSystemTypeComparisonInfo);
    void readWeaponComponentComparisonList();
    void readComponentType(WeaponComponentTypeComparisonInfo &weaponComponentTypeComparisonInfo);
    void readComponentModel(WeaponComponentModelComparisonInfo &weaponComponentModelComparisonInfo);

private:
    QXmlStreamReader                     m_XmlReaderWeaponManage;
    QXmlStreamReader                     m_XmlReaderWeaponComparison;
    std::string                          m_strWeaponManageXMLPath;
    std::string                          m_strComparisonXMLPath;
    mutable QMutex                       m_mutex;
    WeaponSystemMap                      m_mapWeaponSystem;
    WeaponComponentTypeComparisonInfoMap m_mapModelCategoryComparisonInfo;
    WeaponSystemTypeComparisonInfoMap    m_mapSystemCategoryComparisonInfo;
};

}
}

#endif
