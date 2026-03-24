#ifndef AGRIMACHINEMANAGER_h__
#define AGRIMACHINEMANAGER_h__

#include <vector>
#include <memory>
#include <unordered_map>
#include "AgriMachine.h"

#include "DataManage/AgriMachineManager/AgriMachineManagerDecl.h"


class AgriMachineManager 
{
public:
	AgriMachineManager();
	~AgriMachineManager();

public:
	AgriMachineInfoPtrVec LoadMachineList();
};


#endif // AGRIMACHINEMANAGER_h__
