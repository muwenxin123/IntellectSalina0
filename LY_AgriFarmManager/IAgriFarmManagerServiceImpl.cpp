#include <QDebug>

#include "IAgriFarmManagerServiceImpl.h"
#include "LY_AgriFarmManager.h"
#include "AgriFarmDatabaseHandler.h"

using namespace qnzkna::AgriFarmManager;

IAgriFarmManagerServiceImpl::IAgriFarmManagerServiceImpl()
{
    QMutexLocker locker(&m_mutex);

	// locker.unlock();

}

bool qnzkna::AgriFarmManager::IAgriFarmManagerServiceImpl::GetFarmInfoList(AgriFarmInfoVec &vecFarmList) const
{
	QMutexLocker locker(&m_mutex);

	vecFarmList = LY_AgriFarmManager::Get().GetpDbHandler()->GetFarmInfoList();

	return true;
}

bool qnzkna::AgriFarmManager::IAgriFarmManagerServiceImpl::GetFieldInfoList(AgriFieldInfoVec &vecFieldList) const
{
	QMutexLocker locker(&m_mutex);

	vecFieldList = LY_AgriFarmManager::Get().GetpDbHandler()->GetFieldInfoList();

	return true;
}

bool qnzkna::AgriFarmManager::IAgriFarmManagerServiceImpl::GetMonitorInfoList(AgriMonitorInfoPtrVec &vecMonitorList) const
{
	QMutexLocker locker(&m_mutex);

	vecMonitorList = LY_AgriFarmManager::Get().GetpDbHandler()->GetMonitorInfoList();

	return true;
}

bool qnzkna::AgriFarmManager::IAgriFarmManagerServiceImpl::GetRelateSensorList(const QString& fieldId, AgriMonitorInfoPtrVec &vecMonitorList) const
{
	QMutexLocker locker(&m_mutex);

	vecMonitorList = LY_AgriFarmManager::Get().GetpDbHandler()->GetRelateSensorList(fieldId);

	return true;
}

bool qnzkna::AgriFarmManager::IAgriFarmManagerServiceImpl::GetCropInfoList(AgriCropInfoPtrVec &vecCropList) const
{
	QMutexLocker locker(&m_mutex);

	vecCropList = LY_AgriFarmManager::Get().GetpDbHandler()->GetCropInfoList();

	return true;
}

bool qnzkna::AgriFarmManager::IAgriFarmManagerServiceImpl::GetCropVarietyInfoList(AgriCropVarietyInfoPtrVec &vecCropVarietyList) const
{
	QMutexLocker locker(&m_mutex);

	vecCropVarietyList = LY_AgriFarmManager::Get().GetpDbHandler()->GetCropVarietyInfoList();

	return true;
}

