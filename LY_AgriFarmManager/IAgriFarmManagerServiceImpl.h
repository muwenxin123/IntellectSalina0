#ifndef IWEAPONMANAGESERVICEIMPL_H
#define IWEAPONMANAGESERVICEIMPL_H

#include <set>
#include <string>

#include <QMutex>
#include <QXmlStreamReader>

#include "DataManage/IAgriFarmManagerService.h"
#include "DataManage/AgriFarm/AgriFarmManagerDecl.h"

namespace qnzkna
{
namespace AgriFarmManager
{

class IAgriFarmManagerServiceImpl : public IAgriFarmManagerService
{
public:
	IAgriFarmManagerServiceImpl();
    virtual ~IAgriFarmManagerServiceImpl(void) {}

	/* 农场 */
	virtual bool GetFarmInfoList(AgriFarmInfoVec &vecFarmList) const;

	/* 盐田 */
	virtual bool GetFieldInfoList(AgriFieldInfoVec &vecFieldList) const;

	/* 传感器 */
	virtual bool GetMonitorInfoList(AgriMonitorInfoPtrVec &vecMonitorList) const;

	virtual bool GetRelateSensorList(const QString& fieldId, AgriMonitorInfoPtrVec &vecMonitorList) const;

	/* 种子 */
	virtual bool GetCropInfoList(AgriCropInfoPtrVec &vecCropList) const;

	virtual bool GetCropVarietyInfoList(AgriCropVarietyInfoPtrVec &vecCropVarietyList) const;

private:
	mutable QMutex                       m_mutex;
};

}
}

#endif
