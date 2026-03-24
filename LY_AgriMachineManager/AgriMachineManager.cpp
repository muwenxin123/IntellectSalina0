#include <QDebug>

#include "LY_AgriMachineManager.h"
#include "AgriMachineManager.h"


AgriMachineManager::AgriMachineManager()
{
	LoadMachineList();
}

AgriMachineManager::~AgriMachineManager()
{

}

AgriMachineInfoPtrVec AgriMachineManager::LoadMachineList()
{
	return LY_AgriMachineManager::Get().GetpDbHandler()->GetMachineInfoList();
}
