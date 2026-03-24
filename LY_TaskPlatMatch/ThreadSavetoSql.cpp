#include "ThreadSavetoSql.h"

ThreadSavetoSql::ThreadSavetoSql(QObject *parent)
{
}

ThreadSavetoSql::ThreadSavetoSql(std::vector<missionPlatType *> missions, std::map<std::string, std::string> _SystemSysNameAndID)
{
    setMissionPlats(missions);
    SystemSysNameAndID = _SystemSysNameAndID;

}

ThreadSavetoSql::~ThreadSavetoSql()
{
}

void ThreadSavetoSql::setMissionPlats(std::vector<missionPlatType *> missions)
{
    missionPlats = missions;
}

void ThreadSavetoSql::run()
{
    m_pITaskManageService = ITaskPlatMatchActivator::Get().getPTaskManageService();
    if (m_pITaskManageService == NULL)
    {
        return;
    }

    for (auto missionPlat : missionPlats)
    {
        std::string platTypeName = missionPlat->getPlatTypeName();
        std::string MissionID = missionPlat->getMissionID();
        std::vector<std::string> platNames = missionPlat->getPlatNames_Selected();
        std::vector<std::string> selCtrlers = missionPlat->getControlers();
        m_pITaskManageService->delPlatFormMatch(platTypeName, MissionID);
        for (int i = 0; i < platNames.size(); i++)
        {
            if (platNames.size() > i && selCtrlers.size() > i)
            {
                saveOneToSql(platTypeName, platNames[i], MissionID, selCtrlers[i]);
            }
        }
    }
    this->quit();
    this->wait();

}

void ThreadSavetoSql::saveOneToSql(std::string platTypeName, std::string weaponName, std::string MissionID, std::string CtrlerName)
{
    auto platNameItor = SystemSysNameAndID.find(weaponName);
    if (platNameItor != SystemSysNameAndID.end())
    {
        std::string platNameID = platNameItor->second;
        m_pITaskManageService->AddPlatFormMatch(platTypeName, platNameID, MissionID, CtrlerName);
    }
}
