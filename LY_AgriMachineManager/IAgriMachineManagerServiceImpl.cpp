#include <QDebug>

#include "IAgriMachineManagerServiceImpl.h"
#include "LY_AgriMachineManager.h"
#include "AgriMachineDatabaseHandler.h"

using namespace qnzkna::AgriMachineManager;

IAgriMachineManagerServiceImpl::IAgriMachineManagerServiceImpl()
{
    QMutexLocker locker(&m_mutex);

	// locker.unlock();
}

