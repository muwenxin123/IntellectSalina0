#include "CDlgTaskPlatMatch.h"
#include "lY_TaskPlatMatch.h"
#include "ITaskPlatMatchActivator.h"
#include "DataManage/NebulaTask/NebulaMissionAutoCommand.h"
#include "LyMessageBox.h"
#include "DataManage/Common/TypeDefine_WeaponComponent.h"

#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstdio>
#include <stdio.h>
#include <time.h>
#include <chrono>
#include <string>
#include <QDateTime>
#include <QString>

CDlgTaskPlatMatch::CDlgTaskPlatMatch(QWidget *parent)
    : QWidget(parent),
      ui(new Ui::CDlgTaskPlatMatch)
{
    ui->setupUi(this);

    m_pITaskManageService = ITaskPlatMatchActivator::Get().getPTaskManageService();

    connect(ui->pushButtonMatchSet, &QPushButton::clicked, this, &CDlgTaskPlatMatch::on_setMatch_clicked);
    connect(ui->tableWidget_tasks, SIGNAL(itemDoubleClicked(QTableWidgetItem *)), this, SLOT(on_setMatch_clickedItem(QTableWidgetItem *)));
    connect(ui->pushButtonMatch, &QPushButton::clicked, this, &CDlgTaskPlatMatch::on_pBMatch_clicked);
    connect(ui->pushButtonCheck, &QPushButton::clicked, this, &CDlgTaskPlatMatch::on_MatchCheck_clicked);

    UpdataWindowData();
}

CDlgTaskPlatMatch::~CDlgTaskPlatMatch()
{
}

void CDlgTaskPlatMatch::UpdataWindowData()
{
    if (m_pITaskManageService == NULL)
    {
        return;
    }
    m_MissionVec.clear();
    m_MissionVec = m_pITaskManageService->GetActivatingMissions();

    ui->tableWidget_tasks->clear();

    stackMissions();

    localWeaponNameBindToId();
    defaultPlatMatch_MissionTime();
    UpdataPlatList();
    saveToSql();
}

void CDlgTaskPlatMatch::UpdataPlatList()
{
    ui->tableWidget_tasks->clear();
    QStringList headerlabels;
    headerlabels << QObject::tr("No.") << QObject::tr("task number") << QObject::tr("task name") << QObject::tr("plat type")
                 << QObject::tr("plat model") << QObject::tr("amount") << QObject::tr("match plat") << QObject::tr("seat distribute");
    ui->tableWidget_tasks->setColumnCount(headerlabels.size());
    ui->tableWidget_tasks->setHorizontalHeaderLabels(headerlabels);
    ui->tableWidget_tasks->verticalHeader()->hide();
    ui->tableWidget_tasks->setRowCount(missionPlats.size());
    ui->tableWidget_tasks->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget_tasks->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget_tasks->setSelectionMode(QAbstractItemView::SingleSelection);

    ui->tableWidget_tasks->horizontalHeader()->setSectionResizeMode(6, QHeaderView::ResizeToContents);
    ui->tableWidget_tasks->horizontalHeader()->setSectionResizeMode(7, QHeaderView::ResizeToContents);
    for (int i = 0; i < missionPlats.size(); i++)
    {
        QTableWidgetItem *item0 = new QTableWidgetItem(QString::number(missionPlats[i]->getIndex()));
        item0->setTextColor(QColor(128, 128, 128));
        QTableWidgetItem *item1 = new QTableWidgetItem(QString::fromStdString(missionPlats[i]->getMissionID()));
        item1->setTextColor(QColor(128, 128, 128));
        QTableWidgetItem *item2 = new QTableWidgetItem(QString::fromStdString(missionPlats[i]->getMissionName()));
        item2->setTextColor(QColor(128, 128, 128));
        QTableWidgetItem *item3 = new QTableWidgetItem(QString::fromStdString(missionPlats[i]->getPlatTypeID()));
        item3->setTextColor(QColor(128, 128, 128));
        QTableWidgetItem *item4 = new QTableWidgetItem(QString::fromStdString(missionPlats[i]->getPlatTypeName()));
        item4->setTextColor(QColor(128, 128, 128));
        QTableWidgetItem *item5 = new QTableWidgetItem(QString::number(missionPlats[i]->getPlatNum()));
        item5->setTextColor(QColor(128, 128, 128));

        QString platNames;
        platNames.clear();
        for (int j = 0; j < missionPlats[i]->getPlatNames_Selected().size(); j++)
        {
            platNames += QString::fromStdString(missionPlats[i]->getPlatNames_Selected()[j]);
            if (j != missionPlats[i]->getPlatNames_Selected().size() - 1)
            {
                platNames += ";";
            }

        }
        QTableWidgetItem *item6 = new QTableWidgetItem(platNames);

        QString ctrlersName;
        ctrlersName.clear();
        for (int j = 0; j < missionPlats[i]->getControlers().size(); j++)
        {
            ctrlersName += QString::fromStdString(missionPlats[i]->getControlers()[j]);
            if (j != missionPlats[i]->getControlers().size() - 1)
            {
                ctrlersName += ";";
            }

        }
        QTableWidgetItem *item7 = new QTableWidgetItem(ctrlersName);

        ui->tableWidget_tasks->setItem(i, 0, item0);
        ui->tableWidget_tasks->setItem(i, 1, item1);
        ui->tableWidget_tasks->setItem(i, 2, item2);
        ui->tableWidget_tasks->setItem(i, 3, item3);
        ui->tableWidget_tasks->setItem(i, 4, item4);
        ui->tableWidget_tasks->setItem(i, 5, item5);
        ui->tableWidget_tasks->setItem(i, 6, item6);
        ui->tableWidget_tasks->setItem(i, 7, item7);
        ui->tableWidget_tasks->item(i, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->tableWidget_tasks->item(i, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->tableWidget_tasks->item(i, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->tableWidget_tasks->item(i, 3)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->tableWidget_tasks->item(i, 4)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->tableWidget_tasks->item(i, 5)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->tableWidget_tasks->item(i, 6)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->tableWidget_tasks->item(i, 7)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    }
    ui->tableWidget_tasks->show();

}

void CDlgTaskPlatMatch::defaultPlatMatch()
{

}

void CDlgTaskPlatMatch::localWeaponNameBindToId()
{
    if (ITaskPlatMatchActivator::Get().getPTaskManageService() == nullptr || ITaskPlatMatchActivator::Get().getPWeaponManageService() == nullptr) {
        return;
    }
    m_nActionID = ITaskPlatMatchActivator::Get().getPTaskManageService()->GetActivatingActionID();
    WeaponTypeNames.clear();
    ITaskPlatMatchActivator::Get().getPWeaponManageService()->GetWeaponSystemMap(weaponSystemMap);
    for (auto sysItr : weaponSystemMap)
    {
        auto weaponComPonentMap = sysItr.second.GetWeaponComponentMapPtr();
        for (auto weapon : *weaponComPonentMap)
        {
            SystemSysNameAndID[weapon.second.getComponentName()] = weapon.second.getComponentID();
            SystemSysIDAndName[weapon.second.getComponentID()] = weapon.second.getComponentName();

            short int tmpInt = weapon.second.getPWeaponModel()->getComponentType();
            auto tmpItor = WeaponTypeNames.find(tmpInt);
            if (tmpItor != WeaponTypeNames.end())
            {
                tmpItor->second.push_back(weapon.second.getComponentName());
            }
            else
            {
                std::vector<std::string> tmpNames;
                tmpNames.clear();
                tmpNames.push_back(weapon.second.getComponentName());
                WeaponTypeNames[tmpInt] = tmpNames;
            }

        }
    }

}

std::vector<std::string> CDlgTaskPlatMatch::WeaponIDToNames(const std::vector<std::string> IDs)
{
    std::vector<std::string> reStrs;
    reStrs.clear();
    for (int i = 0; i < IDs.size(); i++)
    {
        auto tmpItor = SystemSysIDAndName.find(IDs[i]);
        if (tmpItor != SystemSysIDAndName.end())
        {
            reStrs.push_back(tmpItor->second);
        }
    }
    return reStrs;

}

std::string CDlgTaskPlatMatch::PlatTypeEnumToStr(short int t)
{
    QString reStr;
    reStr.clear();
    switch (t)
    {
    case 0x01:
        reStr = QObject::tr("rotary wing uav");
        break;
    case 0x02:
        reStr = QObject::tr("helicopter uav");
        break;
    case 0x03:
        reStr = QObject::tr("fixed wing uav");
        break;
    case 0x04:
        reStr = QObject::tr("spin wing uav");
        break;
    case 0x05:
        reStr = QObject::tr("umbrella wing uav");
        break;
    case 0x06:
        reStr = QObject::tr("flapping uav");
        break;
    case 0x07:
        reStr = QObject::tr("unmanned airship");
        break;

    case 0x10:
        reStr = QObject::tr("accusation vehicle");
        break;
    case 0x11:
        reStr = QObject::tr("transport vehicle");
        break;
    case 0x12:
        reStr = QObject::tr("individual exoskeleton robot");
        break;
    case 0x13:
        reStr = QObject::tr("individual all terrain vehicle");
        break;
    case 0x14:
        reStr = QObject::tr("unmanned tracked weapon remote control station");
        break;

    case 0xE0:
        reStr = QObject::tr("IndividualSoldier");
        break;

    case 0x20:
        reStr = QObject::tr("Switch");
        break;
    case 0x21:
        reStr = QObject::tr("Router");
        break;
    case 0x22:
        reStr = QObject::tr("VCCS");
        break;
    case 0x23:
        reStr = QObject::tr("ACS");
        break;
    case 0x24:
        reStr = QObject::tr("serial device");
        break;
    case 0x25:
        reStr = QObject::tr("serial server");
        break;
    case 0x30:
        reStr = QObject::tr("integrated misssion pod");
        break;
    case 0x31:
        reStr = QObject::tr("SAR radar");
        break;

    case 0x40:
        reStr = QObject::tr("rocket");
        break;
    case 0x41:
        reStr = QObject::tr("bomb");
        break;
    case 0x42:
        reStr = QObject::tr("missile");
        break;

    case 0x50:
        reStr = QObject::tr("CalcServer");
        break;
    case 0x51:
        reStr = QObject::tr("DatabaseServer");
        break;
    case 0x52:
        reStr = QObject::tr("VideoServer");
        break;
    case 0x53:
        reStr = QObject::tr("NetServer");
        break;

    case 0x60:
        reStr = QObject::tr("ComputerHost");
        break;
    case 0x61:
        reStr = QObject::tr("WorkstationHost");
        break;
    case 0x62:
        reStr = QObject::tr("EdgeComputerHost");
        break;
    case 0x63:
        reStr = QObject::tr("MCUHost");
        break;
    case 0x64:
        reStr = QObject::tr("Pad");
        break;

    case 0x70:
        reStr = QObject::tr("UPS");
        break;
    case 0x71:
        reStr = QObject::tr("APGS");
        break;

    default :
        reStr = QObject::tr("unknown component");
    }

    return reStr.toStdString();
}

void CDlgTaskPlatMatch::saveToSql()
{

    ThreadSavetoSql *saveToSql = new ThreadSavetoSql(missionPlats, SystemSysNameAndID);
    connect(saveToSql, &QThread::quit, saveToSql, &QThread::deleteLater);
    saveToSql->start();
}

void CDlgTaskPlatMatch::saveOneToSql(std::string platTypeName, std::string weaponName, std::string MissionID, std::string CtrlerName)
{
    auto platNameItor = SystemSysNameAndID.find(weaponName);
    if (platNameItor != SystemSysNameAndID.end())
    {
        std::string platNameID = platNameItor->second;
        m_pITaskManageService->AddPlatFormMatch(platTypeName, platNameID, MissionID, CtrlerName);
    }
}

void CDlgTaskPlatMatch::saveMissionPlatToSql(missionPlatType *platType)
{
    std::string platTypeName = platType->getPlatTypeName();
    std::string MissionID = platType->getMissionID();
    std::vector<std::string> platNames = platType->getPlatNames_Selected();
    std::vector<std::string> selCtrlers = platType->getControlers();
    m_pITaskManageService->delPlatFormMatch(platTypeName, MissionID);
    for (int i = 0; i < platNames.size(); i++)
    {
        saveOneToSql(platTypeName, platNames[i], MissionID, selCtrlers[i]);
    }
}

std::vector<std::string> CDlgTaskPlatMatch::getAllCtrlers()
{
    std::map<std::string, int> ctrlRole = m_pITaskManageService->GetUserNameAndRole();
    std::vector<std::string> ctrlers;
    ctrlers.clear();
    auto ctrlroleItor = ctrlRole.begin();
    while (ctrlroleItor != ctrlRole.end())
    {
        if (ctrlroleItor->second == 1 || ctrlroleItor->second == 2)
        {
            ctrlers.push_back(ctrlroleItor->first);
        }
        ctrlroleItor++;
    }
    return ctrlers;
}

CDlgSetPlat *CDlgTaskPlatMatch::getDlgSetPlat()
{
    return DlgSetPlat;
}

QTableWidget *CDlgTaskPlatMatch::getTabWidget()
{
    return ui->tableWidget_tasks;
}

void CDlgTaskPlatMatch::setCurMission(int curIndex)
{
    if (curIndex < missionPlats.size())
    {
        auto mission = m_pITaskManageService->GetMission(missionPlats[curIndex]->getMissionID());
        DlgSetPlat->setTime(QString::fromStdString(mission.GetStartTime()), QString::fromStdString(mission.GetFinishTime()));
        DlgSetPlat->initMissionName(QString::fromStdString(missionPlats[curIndex]->getMissionName()));
        DlgSetPlat->setPlatType(*(missionPlats[curIndex]));
        DlgSetPlat->setWindowModality(Qt::ApplicationModal);
        QString Wintitle = QObject::tr("task plat match");
        DlgSetPlat->setWindowTitle(Wintitle);
        DlgSetPlat->show();

    }
}

void CDlgTaskPlatMatch::Update(qnzkna::TaskManage::INebulaTaskManageService *sub)
{
    m_nActionID = ITaskPlatMatchActivator::Get().getPTaskManageService()->GetActivatingActionID();
    UpdataWindowData();
}

map<string, map<string, string>> CDlgTaskPlatMatch::getMissionWeaponPlats()
{
    return missionWeaponPlatNames;
}

void CDlgTaskPlatMatch::removeMissionIDweaponID(string missionID, string weaponID, string platName)
{

}

void CDlgTaskPlatMatch::addMissionIDWeaponID(string missionID, string weaponID, string platName, int platIndex)
{

}

void CDlgTaskPlatMatch::updateMissionIDWeaponID(string missionID, string weaponID, string platName, int platIndex)
{

}

void CDlgTaskPlatMatch::stackMissions()
{
    std::map<string, pair<time_t, time_t>> missionTimes;
    missionTimes.clear();
    for (auto mission : m_MissionVec)
    {
        time_t startTime = StringToTime_t(mission.GetStartTime());
        time_t endTime = StringToTime_t(mission.GetFinishTime());
        missionTimes[mission.GetID()] = make_pair(startTime, endTime);
    }

    for (auto mission1 : m_MissionVec)
    {
        std::vector<std::string> missions;
        missions.clear();
        for (auto mission2 : m_MissionVec)
        {
            if (mission1.GetID() == mission2.GetID())
            {
                continue;
            }
            auto Itor1 = missionTimes.find(mission1.GetID());
            auto itor2 = missionTimes.find(mission2.GetID());
            if (Itor1 == missionTimes.end() || itor2 == missionTimes.end())
            {
                continue;
            }
            if ((Itor1->second.first >= itor2->second.first && Itor1->second.first <= itor2->second.second)
                    || (Itor1->second.second >= itor2->second.first && Itor1->second.second <= itor2->second.second)
                    || (itor2->second.first >= Itor1->second.first && itor2->second.first <= Itor1->second.second)
                    || (itor2->second.second >= Itor1->second.first && itor2->second.second <= Itor1->second.second))
            {
                missions.emplace_back(mission2.GetID());
            }
        }
        interMissions[mission1.GetID()] = missions;
    }

}

time_t CDlgTaskPlatMatch::StringToTime_t(std::string strTime)
{
    if (int reStr = strTime.find("/") != -1)
    {
        QString tmpStrTime = QString::fromStdString(strTime);
        QStringList dayHour = tmpStrTime.split(" ");
        if (dayHour.size() == 2)
        {
            QStringList YMD = dayHour.at(0).split("/");
            strTime.clear();
            QString qstrTime = YMD.at(0) + "-" + YMD.at(1) + "-" + YMD.at(2) + " ";
            QStringList hms = dayHour.at(1).split(":");
            if (hms.size() < 3)
            {
                qstrTime = qstrTime + hms.at(0) + ":" + hms.at(1) + ":" + "00";
            }
            else
            {
                qstrTime = qstrTime + hms.at(0) + ":" + hms.at(1) + ":" + hms.at(2);
            }
            strTime = qstrTime.toStdString();
        }
    }
    char *cha = (char *)strTime.data();
    tm tm_;
    int year, month, day, hour, minute, second;
    sscanf(cha, "%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &minute, &second);
    second %= 60;
    tm_.tm_year = year - 1900;
    tm_.tm_mon = month - 1;
    tm_.tm_mday = day;
    tm_.tm_hour = hour;
    tm_.tm_min = minute;
    tm_.tm_sec = second;
    tm_.tm_isdst = 0;
    time_t t_ = mktime(&tm_);
    return t_;

}

void CDlgTaskPlatMatch::deleteMissionIDWeapoinName(string MissionID, int weaponTypeInt, string weaponName)
{
    if (weaponName == " ")
    {
        return;
    }

    auto interMItor = interMissions.find(MissionID);
    if (interMItor != interMissions.end())
    {
        vector<string> missions = interMItor->second;
        missions.push_back(MissionID);
        for (auto tmpinterM : missions)
        {
            auto tmpMissionUsableITor = missionWeaponTypeAndNames_usabled.find(tmpinterM);
            if (tmpMissionUsableITor == missionWeaponTypeAndNames_usabled.end())
            {
                continue;
            }
            if (tmpMissionUsableITor->second.find(weaponTypeInt) == tmpMissionUsableITor->second.end())
            {
                continue;
            }
            if (tmpMissionUsableITor->second.find(weaponTypeInt)->second.size() == 0)
            {
                continue;
            }
            auto tmpusableNameItor = std::find(tmpMissionUsableITor->second.find(weaponTypeInt)->second.begin(),
                                               tmpMissionUsableITor->second.find(weaponTypeInt)->second.end(), weaponName);

            if (tmpusableNameItor != tmpMissionUsableITor->second.find(weaponTypeInt)->second.end())
            {
                tmpusableNameItor = tmpMissionUsableITor->second.find(weaponTypeInt)->second.erase(tmpusableNameItor);
            }
        }
    }
}

void CDlgTaskPlatMatch::addMissionIDWeapoinName(string MissionID, int weaponTypeInt, string weaponName)
{
    if (weaponName == " ")
    {
        return;
    }

    auto interMItor = interMissions.find(MissionID);
    if (interMItor != interMissions.end())
    {
        vector<string> missions = interMItor->second;
        missions.push_back(MissionID);
        for (auto tmpinterM : missions)
        {
            auto tmpMissionUsableITor = missionWeaponTypeAndNames_usabled.find(tmpinterM);
            if (tmpMissionUsableITor == missionWeaponTypeAndNames_usabled.end())
            {
                continue;
            }
            if (tmpMissionUsableITor->second.find(weaponTypeInt) == tmpMissionUsableITor->second.end())
            {
                continue;
            }
            if (tmpMissionUsableITor->second.find(weaponTypeInt)->second.size() == 0)
            {
                continue;
            }
            auto tmpusableNameItor = std::find(tmpMissionUsableITor->second.find(weaponTypeInt)->second.begin(),
                                               tmpMissionUsableITor->second.find(weaponTypeInt)->second.end(), weaponName);

            if (tmpusableNameItor == tmpMissionUsableITor->second.find(weaponTypeInt)->second.end())
            {
                tmpMissionUsableITor->second.find(weaponTypeInt)->second.push_back(weaponName);
            }
        }
    }
}

void CDlgTaskPlatMatch::updateMissionTypeOldToNewName(string missionID, int weaponTypeInt, string weaponTypeName, int platIndex, string oldName, string newName)
{
    int missionPlatIndex = 0;
    for (auto missionPlat : missionPlats)
    {

        if (missionPlat->getMissionID() == missionID && missionPlat->getPlatTypeName() == weaponTypeName)
        {
            auto selectedNames = missionPlat->getPlatNames_Selected();
            if (selectedNames.size() > platIndex && selectedNames[platIndex] == oldName)
            {
                selectedNames[platIndex] = newName;
                missionPlat->setPlatNames_Selected(selectedNames);
                deleteMissionIDWeapoinName(missionID, weaponTypeInt, newName);
                addMissionIDWeapoinName(missionID, weaponTypeInt, oldName);

                updatePlatShow(missionPlatIndex, missionPlat->getPlatNames_Selected());
                saveMissionPlatToSql(missionPlat);
                break;
            }

        }
        missionPlatIndex++;
    }
}

void CDlgTaskPlatMatch::swapMissionTypeOldToNewName(string missionID, string weaponTypeName, int OldIndex, string oldName, int NewIndex, string newName)
{
    int missionPlatIndex = 0;
    for (auto missionPlat : missionPlats)
    {
        if (missionPlat->getMissionID() == missionID && missionPlat->getPlatTypeName() == weaponTypeName)
        {
            auto selectedNames = missionPlat->getPlatNames_Selected();
            if (selectedNames.size() > OldIndex && selectedNames.size() > NewIndex)
            {
                selectedNames[OldIndex] = oldName;
                selectedNames[NewIndex] = newName;
                missionPlat->setPlatNames_Selected(selectedNames);

                updatePlatShow(missionPlatIndex, missionPlat->getPlatNames_Selected());
                saveMissionPlatToSql(missionPlat);
                break;
            }
        }
        missionPlatIndex++;
    }
}

void CDlgTaskPlatMatch::swapDiffMissionTypeOldToNewName(string missionID, int weaponTypeInt, string weaponTypeName, int OldIndex, string oldName, string newName)
{
    auto interMission = interMissions.find(missionID);
    vector<string> missions = interMission->second;

    int missionPlatIndex = 0;
    for (auto missionPlat : missionPlats)
    {
        if (missionPlat->getMissionID() == missionID && missionPlat->getPlatTypeName() == weaponTypeName)
        {
            auto selectedNames = missionPlat->getPlatNames_Selected();
            if (selectedNames.size() > OldIndex)
            {
                selectedNames[OldIndex] = newName;
                missionPlat->setPlatNames_Selected(selectedNames);

                updatePlatShow(missionPlatIndex, missionPlat->getPlatNames_Selected());
                saveMissionPlatToSql(missionPlat);
                missionPlatIndex++;
                continue;
            }
        }
        if (missionPlat->getMissionID() == missionID && missionPlat->getPlatTypeName() != weaponTypeName)
        {

            auto selectedNames = missionPlat->getPlatNames_Selected();
            if (auto newNameItor = std::find(selectedNames.begin(), selectedNames.end(), newName) != selectedNames.end())
            {
                for (int i = 0; i < selectedNames.size(); i++)
                {
                    if (selectedNames[i] == newName)
                    {
                        selectedNames[i] = oldName;
                        missionPlat->setPlatNames_Selected(selectedNames);
                        updatePlatShow(missionPlatIndex, missionPlat->getPlatNames_Selected());
                        saveMissionPlatToSql(missionPlat);
                        missionPlatIndex++;
                        break;
                    }

                }
                break;
            }
        }
        else if (auto interMissionItor = std::find(missions.begin(), missions.end(), missionPlat->getMissionID()) != missions.end())
        {

            auto selectedNames = missionPlat->getPlatNames_Selected();
            if (auto newNameItor = std::find(selectedNames.begin(), selectedNames.end(), newName) != selectedNames.end())
            {
                for (int i = 0; i < selectedNames.size(); i++)
                {
                    if (selectedNames[i] == newName)
                    {
                        selectedNames[i] = oldName;
                        missionPlat->setPlatNames_Selected(selectedNames);
                        updatePlatShow(missionPlatIndex, missionPlat->getPlatNames_Selected());
                        saveMissionPlatToSql(missionPlat);
                        missionPlatIndex++;
                        break;
                    }

                }
                break;
            }
        }
        missionPlatIndex++;
    }
}

void CDlgTaskPlatMatch::updatePlatShow(int missionPlatIndex, vector<string> platNames)
{
    if (missionPlatIndex < ui->tableWidget_tasks->rowCount())
    {
        QString nameStr;
        nameStr.clear();
        for (int i = 0; i < platNames.size(); i++)
        {
            nameStr += QString::fromStdString(platNames[i]);
            if (i != platNames.size() - 1)
            {
                nameStr += ";";
            }
        }
        ui->tableWidget_tasks->item(missionPlatIndex, 6)->setText(nameStr);
    }
}

void CDlgTaskPlatMatch::updateCtrler(string missionID, string weaponTypeName, int oldIndex, string newName)
{
    int missionPlatIndex = 0;
    for (auto missionPlat : missionPlats)
    {
        if (missionPlat->getMissionID() == missionID && missionPlat->getPlatTypeName() == weaponTypeName)
        {
            auto selectedNames = missionPlat->getControlers();
            if (selectedNames.size() > oldIndex)
            {
                selectedNames[oldIndex] = newName;
                missionPlat->setControlers(selectedNames);

                updateCtrlShow(missionPlatIndex, missionPlat->getControlers());
                saveMissionPlatToSql(missionPlat);
                break;
            }
        }
        missionPlatIndex++;
    }
}

void CDlgTaskPlatMatch::updateCtrlShow(int missionPlatIndex, vector<string> ctrlNames)
{
    if (missionPlatIndex < ui->tableWidget_tasks->rowCount())
    {
        QString nameStr;
        nameStr.clear();
        for (int i = 0; i < ctrlNames.size(); i++)
        {
            nameStr += QString::fromStdString(ctrlNames[i]);
            if (i != ctrlNames.size() - 1)
            {
                nameStr += ";";
            }
        }
        ui->tableWidget_tasks->item(missionPlatIndex, 7)->setText(nameStr);
    }
}

void CDlgTaskPlatMatch::defaultPlatMatch_MissionTime()
{
    missionPlats.clear();
    missionWeaponNameInterMIDs_used.clear();
    missionWeaponTypeAndNames_usabled.clear();

    NebulaResourcesVec resourcesVec = m_pITaskManageService->GetPlatFormResourceFmDB();

    QStringList missionIDList;
    missionIDList.clear();

    for (auto Mission0 : m_MissionVec)
    {
        CRelationMissionResourceVec missionPlatVec = m_pITaskManageService->GetRelationMissionPtResource(Mission0.GetID());
        if (missionPlatVec.empty())
        {
            continue;
        }

        std::map<std::string, pair<int, NebulaResources>>  missionReAndCount;
        for (auto missPlat : missionPlatVec)
        {
            for (auto nowRe : resourcesVec)
            {
                if (missPlat.GetResourceName() == nowRe.GetLoadName())
                {
                    if (missionReAndCount.find(nowRe.GetLoadName()) == missionReAndCount.end())
                    {
                        missionReAndCount[nowRe.GetLoadName()] = make_pair(missPlat.GetResourceNum(), nowRe);
                    }
                    else
                    {
                        missionReAndCount.find(nowRe.GetLoadName())->second.first += 1;
                    }

                }
            }
        }

        auto missPlatItor = missionReAndCount.begin();
        while (missPlatItor != missionReAndCount.end())
        {
            int replInd = missPlatItor->second.second.GetLoadType();
            auto WeaponItor = WeaponTypeNames.find(replInd);
            if (WeaponItor != WeaponTypeNames.end())
            {

                auto mUsableItor = missionWeaponTypeAndNames_usabled.find(Mission0.GetID());
                if (mUsableItor != missionWeaponTypeAndNames_usabled.end())
                {
                    auto weaponTypeItor = mUsableItor->second.find(replInd);
                    if (weaponTypeItor == mUsableItor->second.end())
                    {
                        mUsableItor->second[replInd] = WeaponItor->second;
                    }
                }
                else
                {
                    map<int, vector<string>> tmpWeaponTypeAndNames;
                    tmpWeaponTypeAndNames.clear();
                    tmpWeaponTypeAndNames[replInd] = WeaponItor->second;
                    missionWeaponTypeAndNames_usabled[Mission0.GetID()] = tmpWeaponTypeAndNames;
                }
            }
            missPlatItor++;
        }
    }

    unsigned short index = 1;
    for (auto Mission : m_MissionVec)
    {
        CRelationMissionResourceVec missionPlatVec = m_pITaskManageService->GetRelationMissionPtResource(Mission.GetID());
        if (missionPlatVec.empty())
        {
            continue;
        }

        std::map<std::string, pair<int, NebulaResources>>  missionReAndCount;
        for (auto missPlat : missionPlatVec)
        {
            for (auto nowRe : resourcesVec)
            {
                if (missPlat.GetResourceName() == nowRe.GetLoadName())
                {
                    if (missionReAndCount.find(nowRe.GetLoadName()) == missionReAndCount.end())
                    {
                        missionReAndCount[nowRe.GetLoadName()] = make_pair(missPlat.GetResourceNum(), nowRe);
                    }
                    else
                    {
                        missionReAndCount.find(nowRe.GetLoadName())->second.first += 1;
                    }

                }
            }
        }

        std::map<std::string, std::vector<std::string>> type2Names;
        type2Names.clear();

        auto missPlatItor = missionReAndCount.begin();
        while (missPlatItor != missionReAndCount.end())
        {
            NebulaResources rePlat = missPlatItor->second.second;

            short int replInd = rePlat.GetLoadType();
            missionPlatType *tmpPlatType = new missionPlatType;
            tmpPlatType->setIndex(index);
            index++;
            tmpPlatType->setMissionID(Mission.GetID());
            tmpPlatType->setMissionName(Mission.GetName());
            tmpPlatType->setWeaponType(replInd);
            tmpPlatType->setPlatTypeID(PlatTypeEnumToStr(replInd));
            tmpPlatType->setPlatTypeName(rePlat.GetLoadName());
            tmpPlatType->setPlatNum(missPlatItor->second.first);
            tmpPlatType->setAllControlers(getAllCtrlers());

            auto WeaponItor = WeaponTypeNames.find(replInd);
            if (WeaponItor != WeaponTypeNames.end())
            {
                std::vector<std::string> selectedNames;
                std::vector<std::string> selectedCtrlers;
                selectedCtrlers.clear();
                selectedNames.clear();
                tmpPlatType->setPlatNames(WeaponItor->second);

                std::vector<std::string> Sqlnames = WeaponIDToNames(m_pITaskManageService->readPlatFormMatch(rePlat.GetLoadName(),
                                                                    Mission.GetID()));
                std::vector<std::string> AllCtrlersName = getAllCtrlers();
                std::vector<std::string> sqlCtrlers = m_pITaskManageService->readPlatCtrlByMissionIDPlatName(rePlat.GetLoadName(),
                                                      Mission.GetID());

                if (Sqlnames.empty())
                {

                    auto MissionUsableITor = missionWeaponTypeAndNames_usabled.find(Mission.GetID());
                    if (MissionUsableITor == missionWeaponTypeAndNames_usabled.end())
                    {
                        continue;
                    }
                    if (MissionUsableITor->second.find(replInd) == MissionUsableITor->second.end())
                    {
                        continue;
                    }

                    for (int platIndex = 0; platIndex < missPlatItor->second.first; platIndex++)
                    {
                        auto usableNameItor = MissionUsableITor->second.find(replInd)->second.begin();
                        if (usableNameItor != MissionUsableITor->second.find(replInd)->second.end())
                        {
                            selectedNames.push_back(*usableNameItor);

                            deleteMissionIDWeapoinName(Mission.GetID(), replInd, *usableNameItor);

                        }
                        else
                        {
                            selectedNames.push_back(" ");
                        }

                    }

                }
                else
                {
                    int selectIndex = 0;
                    auto MissionUsableITor = missionWeaponTypeAndNames_usabled.find(Mission.GetID());
                    if (MissionUsableITor == missionWeaponTypeAndNames_usabled.end())
                    {
                        continue;
                    }
                    if (MissionUsableITor->second.find(replInd) == MissionUsableITor->second.end())
                    {
                        continue;
                    }

                    for (int platIndex = 0; platIndex < Sqlnames.size() && platIndex < missPlatItor->second.first; platIndex++)
                    {
                        string newName = Sqlnames[platIndex];
                        auto newNameItor = std::find(MissionUsableITor->second.find(replInd)->second.begin(),
                                                     MissionUsableITor->second.find(replInd)->second.end(), newName);
                        if (newNameItor != MissionUsableITor->second.find(replInd)->second.end())
                        {
                            selectIndex++;
                            selectedNames.push_back(newName);
                            deleteMissionIDWeapoinName(Mission.GetID(), replInd, newName);

                        }
                    }

                    for (int platIndex = selectIndex; platIndex < missPlatItor->second.first; platIndex++)
                    {
                        auto usableNameItor = MissionUsableITor->second.find(replInd)->second.begin();
                        if (usableNameItor != MissionUsableITor->second.find(replInd)->second.end())
                        {
                            selectedNames.push_back(*usableNameItor);

                            deleteMissionIDWeapoinName(Mission.GetID(), replInd, *usableNameItor);

                        }
                        else
                        {
                            selectIndex++;
                            selectedNames.push_back(" ");
                        }
                    }

                }
                tmpPlatType->setPlatNames_Selected(selectedNames);

                if (sqlCtrlers.empty())
                {
                    auto ctrlerItor = AllCtrlersName.begin();
                    while (ctrlerItor != AllCtrlersName.end())
                    {
                        for (int i = 0; i < missPlatItor->second.first; i++)
                        {
                            selectedCtrlers.push_back(*ctrlerItor);
                        }
                        if (selectedCtrlers.size() >= missPlatItor->second.first)
                        {
                            break;
                        }
                        ctrlerItor++;
                    }
                }
                else if (sqlCtrlers.size() >= missPlatItor->second.first)
                {
                    selectedCtrlers.assign(sqlCtrlers.begin(), sqlCtrlers.begin() + missPlatItor->second.first);
                }
                else
                {
                    selectedCtrlers.assign(sqlCtrlers.begin(), sqlCtrlers.begin() + sqlCtrlers.size());
                    while (selectedCtrlers.size() < missPlatItor->second.first)
                    {
                        selectedCtrlers.push_back(selectedCtrlers[0]);
                    }
                }
                tmpPlatType->setControlers(selectedCtrlers);
            }
            missionPlats.push_back(tmpPlatType);
            missionIDList.push_back(QString::fromStdString(tmpPlatType->getMissionID() + "/" + tmpPlatType->getPlatTypeName()));
            missPlatItor++;
        }
    }
    if (DlgSetPlat == nullptr)
    {
        DlgSetPlat = new CDlgSetPlat;
        connect(DlgSetPlat, &CDlgSetPlat::on_pbEnsure_isClicked, this, &CDlgTaskPlatMatch::updateMatched);

    }
    saveToSql();
    disconnect(DlgSetPlat->getMissionComBox(), SIGNAL(currentIndexChanged(int)), this, SLOT(setCurMission(int)));
    DlgSetPlat->initComBoBox(missionIDList);
    connect(DlgSetPlat->getMissionComBox(), SIGNAL(currentIndexChanged(int)), this, SLOT(setCurMission(int)));
    DlgSetPlat->hide();

}

void CDlgTaskPlatMatch::updateMatched()
{
    auto selectedItem = ui->tableWidget_tasks->selectedItems();
    if (selectedItem.empty())
    {
        return;
    }

    int selectedRow = DlgSetPlat->getComCruIndex();
    QString textStr;
    textStr.clear();
    if (missionPlats.size() <= selectedRow)
    {
        return;
    }
    std::vector<std::string> plats = missionPlats[selectedRow]->getPlatNames_Selected();
    for (int i = 0; i < plats.size(); i++)
    {
        textStr += QString::fromStdString(plats[i]);
        if (i != plats.size() - 1)
        {
            textStr += ";";
        }
    }

    QString textCtrlStr;
    textCtrlStr.clear();
    std::vector<std::string> ctrlers = DlgSetPlat->getCtrlers();
    for (int i = 0; i < ctrlers.size(); i++)
    {
        textCtrlStr += QString::fromStdString(ctrlers[i]);
        if (i != plats.size() - 1)
        {
            textCtrlStr += ";";
        }
    }

    ui->tableWidget_tasks->item(selectedRow, 6)->setText(textStr);
    ui->tableWidget_tasks->item(selectedRow, 7)->setText(textCtrlStr);
    saveMissionPlatToSql((missionPlats[selectedRow]));
    for (int i = 0; i < missionPlats.size(); i++)
    {
        if (missionPlats[i]->getMissionID() == DlgSetPlat->getMissionID() & i != selectedRow)
        {
            updateMatchedIndex(i);
        }
    }
}

void CDlgTaskPlatMatch::updateMatchedIndex(int SelectedRow)
{
    if (SelectedRow >= missionPlats.size())
    {
        return;
    }
    QString textStr;
    textStr.clear();
    std::vector<std::string> plats = missionPlats[SelectedRow]->getPlatNames_Selected();
    for (int i = 0; i < plats.size(); i++)
    {
        textStr += QString::fromStdString(plats[i]);
        if (i != plats.size() - 1)
        {
            textStr += ";";
        }
    }

    QString textCtrlStr;
    textCtrlStr.clear();
    std::vector<std::string> ctrlers = missionPlats[SelectedRow]->getControlers();
    for (int i = 0; i < ctrlers.size(); i++)
    {
        textCtrlStr += QString::fromStdString(ctrlers[i]);
        if (i != plats.size() - 1)
        {
            textCtrlStr += ";";
        }
    }

    ui->tableWidget_tasks->item(SelectedRow, 6)->setText(textStr);
    ui->tableWidget_tasks->item(SelectedRow, 7)->setText(textCtrlStr);
    saveMissionPlatToSql(missionPlats[SelectedRow]);
}

void CDlgTaskPlatMatch::on_pBMatch_clicked()
{
    defaultPlatMatch_MissionTime();
    UpdataPlatList();
    saveToSql();
}

void CDlgTaskPlatMatch::on_setMatch_clicked()
{

    std::ofstream fout;
    fout.open("tableRefresh.txt", std::ios_base::binary | std::ios_base::app);

    std::time_t t = std::time(nullptr);

    auto currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    std::string creatTime = std::to_string(currentTime);
    int sss = currentTime % 1000;

    QDateTime dateTime = QDateTime::fromSecsSinceEpoch(t);
    QString dataStr = dateTime.toString("yyyy-MM-dd hh:mm:ss");
    dataStr = dataStr + "." + QString::number(sss);
    fout << std::endl;
    fout << dataStr.toStdString() << std::endl;

    fout.close();

    auto selectedItem = ui->tableWidget_tasks->selectedItems();
    if (selectedItem.empty())
    {
        return;
    }
    int selectedRow = selectedItem.at(0)->row();

    if (DlgSetPlat == nullptr)
    {
        DlgSetPlat = new CDlgSetPlat;
        connect(DlgSetPlat, &CDlgSetPlat::on_pbEnsure_isClicked, this, &CDlgTaskPlatMatch::updateMatched);
    }
    disconnect(DlgSetPlat->getMissionComBox(), SIGNAL(currentIndexChanged(int)), this, SLOT(setCurMission(int)));
    DlgSetPlat->setComBoBoxIndex(selectedRow);
    connect(DlgSetPlat->getMissionComBox(), SIGNAL(currentIndexChanged(int)), this, SLOT(setCurMission(int)));
    setCurMission(selectedRow);

}

void CDlgTaskPlatMatch::on_setMatch_clickedItem(QTableWidgetItem *item)
{
    int selectedRow = item->row();
    if (DlgSetPlat == nullptr)
    {
        DlgSetPlat = new CDlgSetPlat;
        connect(DlgSetPlat, &CDlgSetPlat::on_pbEnsure_isClicked, this, &CDlgTaskPlatMatch::updateMatched);
    }
    disconnect(DlgSetPlat->getMissionComBox(), SIGNAL(currentIndexChanged(int)), this, SLOT(setCurMission(int)));
    DlgSetPlat->setComBoBoxIndex(selectedRow);
    connect(DlgSetPlat->getMissionComBox(), SIGNAL(currentIndexChanged(int)), this, SLOT(setCurMission(int)));
    setCurMission(selectedRow);
}

void CDlgTaskPlatMatch::on_MatchCheck_clicked()
{
    LY_TaskPlatMatch::Get().showtestSql();
    for (auto missionPlat : missionPlats)
    {
        std::vector<std::string> platNames = missionPlat->getPlatNames_Selected();
        auto platnameIter = std::find(platNames.begin(), platNames.end(), " ");
        if (platNames.size() < missionPlat->getPlatNum() || platnameIter != platNames.end())
        {
            QString infoStr = QObject::tr("task");
            infoStr += QString::fromStdString(missionPlat->getMissionID());
            infoStr += QObject::tr("task match not complete yet");
            LyMessageBox::information(NULL, QObject::tr("match check"), infoStr);

        }
    }
}

void CDlgTaskPlatMatch::on_pBMatchCancel_clicked()
{
    auto selectedItem = ui->tableWidget_tasks->selectedItems();
    if (selectedItem.empty())
    {
        return;
    }
    int selectedRow = selectedItem.at(0)->row();
    if (selectedRow < missionPlats.size())
    {
        std::string platTypeName = missionPlats[selectedRow]->getPlatTypeName();
        std::string MissionID = missionPlats[selectedRow]->getMissionID();
        m_pITaskManageService->delPlatFormMatch(platTypeName, MissionID);
    }

}

void CDlgTaskPlatMatch::tableWidget_cellClicked()
{
    CNeubulaActionVec ActionVec = ITaskPlatMatchActivator::Get().getPTaskManageService()->GetAllNebulaAction();
    for (auto data : ActionVec)
    {
        if (data.GetActivateStates() == true)
        {
            if (data.GetID() != m_nActionID)
            {
                if (data.GetID() != currActionID)
                {
                    m_bPressEvect = true;
                }
                if (m_bPressEvect)
                {
                    QString strTmp = QObject::tr("Do you want to switch to the current activation action?");
                    if (QMessageBox::StandardButton::Yes != LyMessageBox::question(this, QObject::tr("prompt"), strTmp, QMessageBox::Yes | QMessageBox::No))
                    {
                        m_bPressEvect = false;
                        currActionID = data.GetID();
                    }
                    else
                    {
                        Update(ITaskPlatMatchActivator::Get().getPTaskManageService());
                        m_nActionID = data.GetID();
                        ITaskPlatMatchActivator::Get().getPTaskManageService()->SetActivatingActionID(m_nActionID);
                    }
                }
            }
            break;
        }
    }
}
