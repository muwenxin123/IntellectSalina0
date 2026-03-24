#ifndef _THREADSAVETOSQL__
#define _THREADSAVETOSQL__

#include <QObject>
#include <QThread>
#include "MissionPlatType.h"
#include "DataManage/INebulaTaskManageService.h"
#include "ITaskManagerActivator.h"

class ThreadSavetoSql : public QThread
{
    Q_OBJECT

public:
    explicit ThreadSavetoSql(QObject *parent);
    ThreadSavetoSql(std::vector<missionPlatType *> missions, std::map<std::string, std::string> _SystemSysNameAndID);
    ~ThreadSavetoSql();
    void setMissionPlats(std::vector<missionPlatType *> missions);

protected:
    void run();

    void saveOneToSql(std::string platTypeName, std::string weaponName, std::string MissionID, std::string CtrlerName);

private:
    qnzkna::TaskManage::INebulaTaskManageService			*m_pITaskManageService = nullptr;
    std::vector<missionPlatType *> missionPlats;
    std::map<std::string, std::string> SystemSysNameAndID;

};

#endif