#include "CDlgMissionAllocation.h"

CDlgMissionAllocation::CDlgMissionAllocation(QWidget *parent):
    LyDialog(tr("Task Allocation"), parent),
    ui(new Ui::CDlgMissionAllocation)
{
    ui->setupUi(centralWidget());
    this->setResizable(true);
    this->resize(1200, 840);

    m_pITaskManageService = ITaskManagerActivator::Get().getPTaskManageService();
    if (m_pITaskManageService == nullptr)
    {
        return;
    }
    m_pAmmunitionManageService = ITaskManagerActivator::Get().getPAmmunitionManageService();
    if (m_pAmmunitionManageService == nullptr)
    {
        return;
    }
    connect(ui->pBEnsure, &QPushButton::clicked, this, &CDlgMissionAllocation::on_pbEnsure_clicked);
    connect(ui->pBCancel, &QPushButton::clicked, this, &CDlgMissionAllocation::on_pbCancel_clicked);
    connect(ui->cbxmode, &QComboBox::currentTextChanged, this, &CDlgMissionAllocation::on_cbxmode_currentTextChanged);
    connect(ui->btnconfirm, &QPushButton::clicked, this, &CDlgMissionAllocation::on_btnconfirm_clicked);
    connect(ui->btnexit, &QPushButton::clicked, this, &CDlgMissionAllocation::on_btnexit_clicked);
    connect(ui->pbtnOK, &QPushButton::clicked, this, &CDlgMissionAllocation::on_pbtnOK_Clicked);
    connect(ui->pbtnCancel, &QPushButton::clicked, this, &CDlgMissionAllocation::on_pbtnCancel_Clicked);
    connect(ui->cbxTaskID, SIGNAL(activated(const QString &)), this, SLOT(on_currentIndexChangedTaskID(const QString &)));
    connect(ui->btnComplete, &QPushButton::clicked, this, &CDlgMissionAllocation::on_btnComplete_clicked);
    connect(ui->btnCancel_1, &QPushButton::clicked, this, &CDlgMissionAllocation::on_btnCancel_1_clicked);
}

CDlgMissionAllocation::~CDlgMissionAllocation()
{
    delete ui;
}

void CDlgMissionAllocation::Init()
{

    QStringList headers;
    headers.clear();
    headers << tr("No.") << tr("Type of Plaform") << tr("Match Equipment") << tr("Control of Seats");
    ui->tableWidget->setColumnCount(headers.size());
    ui->tableWidget->setRowCount(0);
    ui->tableWidget->clearContents();
    ui->tableWidget->setHorizontalHeaderLabels(headers);
    ui->tableWidget->verticalHeader()->setHidden(true);
    ui->tableWidget->setSelectionBehavior(QTableWidget::SelectRows);
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->setAlternatingRowColors(true);
    ui->tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
    stackMissions();

    localWeaponNameBindToId();

    defaultPlatMatch_MissionTime();

    m_pRelationMissionResourceVec = ITaskManagerActivator::Get().getPTaskManageService()->GetAllRelationMissionPt();
    if (m_pRelationMissionResourceVec.empty())
    {
        LyMessageBox::information(NULL, tr("tip"), tr("Failed to load data!"));
        return;
    }
    ITaskManagerActivator::Get().getPAmmunitionManageService()->LoadAmmoDB();
    m_pMissionAllVec = ITaskManagerActivator::Get().getPTaskManageService()->GetActivatingMissions();
    m_pAllAmmoVec = ITaskManagerActivator::Get().getPAmmunitionManageService()->GetAllAmmoVec();
    m_pAmmoType = ITaskManagerActivator::Get().getPTaskManageService()->GetAmmoResourceFmDB();
    QStringList header;
    header << tr("Platform Number") << tr("Platform Type") << tr("Main Pod Tyoe") << tr("Secondary Pod Tyoe") << tr("Number of Primary Mount Types") << tr("Main Mount Number") << tr("Number of Secondary Mount Types") << tr("Secondary Mount Number");
    ui->tbwdata->setColumnCount(header.size());
    ui->tbwdata->setHorizontalHeaderLabels(header);
    ui->tbwdata->verticalHeader()->setHidden(true);
    ui->tbwdata->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tbwdata->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tbwdata->setSelectionBehavior(QTableWidget::SelectRows);
    ui->tbwdata->setAlternatingRowColors(true);
    LoadDBToTable();
    SetPlatTypeSize();

    m_pAmmunitionManageService->LoadAmmoMarkDB();
    ObtainAllData();
    CreateComboboxTaskID();
    CreateTableWidget();
    ObtainCurrentTextData();
    UpdatatbwMarkAmmoMatch();
}

void CDlgMissionAllocation::defaultPlatMatch_MissionTime()
{
    missionPlats.clear();
    missionWeaponNameInterMIDs_used.clear();
    missionWeaponTypeAndNames_usabled.clear();
    m_MissionVec.clear();
    m_MissionVec = m_pITaskManageService->GetActivatingMissions();

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

    disconnect(getMissionComBox(), SIGNAL(currentIndexChanged(int)), this, SLOT(setCurMission(int)));
    initComBoBox(missionIDList);
    connect(getMissionComBox(), SIGNAL(currentIndexChanged(int)), this, SLOT(setCurMission(int)));
    setCurMission(0);
}

void CDlgMissionAllocation::localWeaponNameBindToId()
{
    int m_nActionID = ITaskManagerActivator::Get().getPTaskManageService()->GetActivatingActionID();
    WeaponTypeNames.clear();
    ITaskManagerActivator::Get().getPWeaponManageService()->GetWeaponSystemMap(weaponSystemMap);
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

std::string CDlgMissionAllocation::PlatTypeEnumToStr(short int t)
{
    QString reStr;
    reStr.clear();
    switch (t)
    {
    case 0x01:
        reStr = tr("Rotor Wing UAV");
        break;
    case 0x02:
        reStr = tr("Helicopter UAV");
        break;
    case 0x03:
        reStr = tr("Fixed Wing UAV");
        break;
    case 0x04:
        reStr = tr("Self-rotor UAV");
        break;
    case 0x05:
        reStr = tr("Parafoil UAV");
        break;
    case 0x06:
        reStr = tr("Flapping Wing UAV");
        break;
    case 0x07:
        reStr = tr("Unmanned Airship");
        break;

    case 0x10:
        reStr = tr("Command and Control Vehicle");
        break;
    case 0x11:
        reStr = tr("Transport Vehicle");
        break;
    case 0x12:
        reStr = tr("Single Pawn Exoskeleton Robot");
        break;
    case 0x13:
        reStr = tr("Single Pawn All Terrain Vehicle");
        break;
    case 0x14:
        reStr = tr("Unmanned Tracked Weapon Remote Control Station");
        break;

    case 0xE0:
        reStr = tr("Single Pawn");
        break;

    case 0x20:
        reStr = tr("Vehicle Mounted Switch");
        break;
    case 0x21:
        reStr = tr("Vehicle Mounted Router");
        break;
    case 0x22:
        reStr = tr("Vehicle Communication Base Station");
        break;
    case 0x23:
        reStr = tr("Airborne Communication Base Station");
        break;
    case 0x24:
        reStr = tr("Serial Port Equipment");
        break;
    case 0x25:
        reStr = tr("Serial Port Server");
        break;
    case 0x30:
        reStr = tr("Integrated Mission Pod");
        break;
    case 0x31:
        reStr = tr("SAR Radar");
        break;

    case 0x40:
        reStr = tr("Rocket Launcher");
        break;
    case 0x41:
        reStr = tr("Bomb");
        break;
    case 0x42:
        reStr = tr("Missile");
        break;

    case 0x50:
        reStr = tr("Compute Server");
        break;
    case 0x51:
        reStr = tr("Database Server");
        break;
    case 0x52:
        reStr = tr("Video Server");
        break;
    case 0x53:
        reStr = tr("Network Server");
        break;

    case 0x60:
        reStr = tr("Computing Host");
        break;
    case 0x61:
        reStr = tr("Workstation Host");
        break;
    case 0x62:
        reStr = tr("Edge Computing Host");
        break;
    case 0x63:
        reStr = tr("Single Chip Microcomputer Host");
        break;
    case 0x64:
        reStr = tr("Handleld Terminal");
        break;

    case 0x70:
        reStr = tr("Uninterruptable Power Supply");
        break;
    case 0x71:
        reStr = tr("Vehicle Power Generation System");
        break;

    default:
        reStr = tr("Unknown Component");
    }

    return reStr.toStdString();
}

std::vector<std::string> CDlgMissionAllocation::getAllCtrlers()
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

std::vector<std::string> CDlgMissionAllocation::WeaponIDToNames(const std::vector<std::string> IDs)
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

void CDlgMissionAllocation::deleteMissionIDWeapoinName(string MissionID, int weaponTypeInt, string weaponName)
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

void CDlgMissionAllocation::stackMissions()
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

time_t CDlgMissionAllocation::StringToTime_t(std::string strTime)
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

void CDlgMissionAllocation::updateMissionTypeOldToNewName(string missionID, int weaponTypeInt, string weaponTypeName, int platIndex, string oldName, string newName)
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

                saveMissionPlatToSql(missionPlat);
                break;
            }

        }
        missionPlatIndex++;
    }
}

void CDlgMissionAllocation::addMissionIDWeapoinName(string MissionID, int weaponTypeInt, string weaponName)
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

void CDlgMissionAllocation::initComBoBox(QStringList strList)
{
    std::string TempMissionName = LY_TaskManage::Get().GetDlgMissionList()->m_sMissionName;
    ui->comboBoxMissionName->clear();
    for (auto data : strList)
    {
        QStringList str = data.split('/');
        std::string strMissionName = str[0].toStdString();
        if (strMissionName == TempMissionName)
        {
            ui->comboBoxMissionName->addItem(data);
        }

    }
}

void CDlgMissionAllocation::setComBoBoxIndex(int Index)
{
    if (ui->comboBoxMissionName->count() > Index)
    {
        ui->comboBoxMissionName->setCurrentIndex(Index);
    }
}

int CDlgMissionAllocation::getComCruIndex()
{
    return ui->comboBoxMissionName->currentIndex();
}

void CDlgMissionAllocation::initMissionName(QString strName)
{
    ui->lineEditMissionName->setText(strName);
}

QComboBox *CDlgMissionAllocation::getMissionComBox()
{
    return ui->comboBoxMissionName;
}

void CDlgMissionAllocation::setPlatType(missionPlatType _platType)
{
    platType = _platType;
    updateDialog();
    updateItemsColor();
}

missionPlatType CDlgMissionAllocation::getPlatType()
{
    return platType;
}

void CDlgMissionAllocation::updateDialog()
{
    ui->tableWidget->clearContents();

    ui->tableWidget->setRowCount(platType.getPlatNum());
    std::vector<std::string> names = platType.getPlatNames();
    std::vector<std::string> names_selected = platType.getPlatNames_Selected();
    std::vector<std::string> ctrlersName = platType.getControlers();
    std::vector<std::string> allCtrlers = platType.getAllControlers();

    if (names_selected.empty())
    {
        for (int i = 0; i < platType.getPlatNum(); i++)
        {
            QTableWidgetItem *item0 = new QTableWidgetItem(QString::number(i + 1));
            QTableWidgetItem *item1 = new QTableWidgetItem(QString::fromStdString(platType.getPlatTypeName()));

            QComboBox *tmpBox = new QComboBox;
            tmpBox->addItem(" ");
            tmpBox->setCurrentText(" ");

            QComboBox *tmpBoxCtrl = new QComboBox;
            tmpBoxCtrl->addItem(" ");
            tmpBoxCtrl->setCurrentText(" ");

            ui->tableWidget->setCellWidget(i, 3, tmpBox);

            connect(tmpBox, SIGNAL(currentTextChanged(const QString &)), this, SLOT(checkIsRepetition(const QString &)));
            connect(tmpBoxCtrl, SIGNAL(currentTextChanged(const QString &)), this, SLOT(ctrlChanged(const QString &)));

            ui->tableWidget->setItem(i, 0, item0);
            ui->tableWidget->setItem(i, 1, item1);
            ui->tableWidget->setCellWidget(i, 2, tmpBox);
            ui->tableWidget->setCellWidget(i, 3, tmpBoxCtrl);
            ui->tableWidget->item(i, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            ui->tableWidget->item(i, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

        }
    }
    QStringList namesList;
    QStringList allCtrlList;
    namesList.clear();
    allCtrlList.clear();
    namesList << " ";
    for (int i = 0; i < names.size(); i++)
    {
        namesList << QString::fromStdString(names[i]);
    }
    for (int i = 0; i < allCtrlers.size(); i++)
    {
        allCtrlList << QString::fromStdString(allCtrlers[i]);
    }

    for (int i = 0; i < names_selected.size(); i++)
    {
        QTableWidgetItem *item0 = new QTableWidgetItem(QString::number(i + 1));
        QTableWidgetItem *item1 = new QTableWidgetItem(QString::fromStdString(platType.getPlatTypeName()));

        QComboBox *tmpBox = new QComboBox;
        QComboBox *tmpBoxCtrl = new QComboBox;

        tmpBox->addItems(namesList);
        tmpBoxCtrl->addItems(allCtrlList);
        if (tmpBox->count() > 0)
        {
            tmpBox->setCurrentText(QString::fromStdString(names_selected[i]));
        }
        if (tmpBoxCtrl->count() > 0)
        {
            tmpBoxCtrl->setCurrentText(QString::fromStdString(ctrlersName[i]));
        }

        connect(tmpBox, SIGNAL(currentTextChanged(const QString &)), this, SLOT(checkIsRepetition(const QString &)));
        connect(tmpBoxCtrl, SIGNAL(currentTextChanged(const QString &)), this, SLOT(ctrlChanged(const QString &)));

        ui->tableWidget->setItem(i, 0, item0);
        ui->tableWidget->setItem(i, 1, item1);
        ui->tableWidget->setCellWidget(i, 2, tmpBox);
        ui->tableWidget->setCellWidget(i, 3, tmpBoxCtrl);
        ui->tableWidget->item(i, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->tableWidget->item(i, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    }

    if (ui->tableWidget->rowCount() <= 0)
    {
        this->resize(QSize(600, 400));

    }
    else
    {
        int Hei = ui->tableWidget->rowHeight(0);
        int wid = ui->tableWidget->columnWidth(0);
        this->resize(QSize(wid * 5, Hei * (ui->tableWidget->rowCount() + 6)));

    }

    this->show();
}

void CDlgMissionAllocation::setConBoxCurIndexNewName(int curIndex, string newName)
{
    if (curIndex < ui->tableWidget->rowCount())
    {
        ((QComboBox *)ui->tableWidget->cellWidget(curIndex, 2))->setCurrentText(QString::fromStdString(newName));
        auto tmpVec = platType.getPlatNames_Selected();
        tmpVec[curIndex] = newName;
        platType.setPlatNames_Selected(tmpVec);
        isSetRe = false;
    }
}

std::vector<std::string> CDlgMissionAllocation::getPlatNames()
{
    return platType.getPlatNames_Selected();
}

void CDlgMissionAllocation::updateItemsColor()
{
    auto missionWeaponTypeAndNames = missionWeaponTypeAndNames_usabled;
    if (missionWeaponTypeAndNames.empty())
    {
        return;
    }
    auto missionItor = missionWeaponTypeAndNames.find(platType.getMissionID());
    if (missionItor == missionWeaponTypeAndNames.end())
    {
        return;
    }
    auto weaponTypeItor = missionItor->second.find(platType.getWeaponType());
    if (weaponTypeItor == missionItor->second.end())
    {
        return;
    }
    auto names = weaponTypeItor->second;
    if (true)
    {
        auto platNames = platType.getPlatNames();
        for (int nameIndex = 0; nameIndex < platNames.size(); nameIndex++)
        {
            QColor *tmpColor = NULL;
            if (std::find(names.begin(), names.end(), platNames[nameIndex]) == names.end())
            {
                tmpColor = new QColor(Qt::red);
            }
            else
            {
                tmpColor = new QColor(Qt::green);
            }
            for (int i = 0; i < ui->tableWidget->rowCount(); i++)
            {
                QComboBox *tmpCom = (QComboBox *)ui->tableWidget->cellWidget(i, 2);
                if (tmpCom != NULL)
                {
                    tmpCom->setItemData(nameIndex + 1, QBrush(*tmpColor), Qt::TextColorRole);
                }
            }
            delete tmpColor;
        }
    }
}

void CDlgMissionAllocation::saveOneToSql(std::string platTypeName, std::string weaponName, std::string MissionID, std::string CtrlerName)
{
    auto platNameItor = SystemSysNameAndID.find(weaponName);
    if (platNameItor != SystemSysNameAndID.end())
    {
        std::string platNameID = platNameItor->second;
        isSaveSuccess = false;
        isSaveSuccess = m_pITaskManageService->AddPlatFormMatch(platTypeName, platNameID, MissionID, CtrlerName);

        if (!isSaveSuccess)
        {
            isSaveSuccess = !isSaveSuccess;
        }
    }
}

void CDlgMissionAllocation::saveMissionPlatToSql(missionPlatType *platType)
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
    if (isSaveSuccess)
    {
        LyMessageBox::information(NULL, tr("tip"), tr("Save successfully!"));
        m_pITaskManageService->LoadRelationMissionPlatDB();
    }
}

void CDlgMissionAllocation::updateCtrler(string missionID, string weaponTypeName, int oldIndex, string newName)
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

                saveMissionPlatToSql(missionPlat);
                break;
            }
        }
        missionPlatIndex++;
    }
}

void CDlgMissionAllocation::LoadDBToTable()
{
    ITaskManagerActivator::Get().getPAmmunitionManageService()->LoadAmmoPlatDB();
    QStringList list;
    for (auto info : m_pMissionAllVec)
    {
        list << tr(info.GetID().c_str());
    }
    ui->cbxmode->clear();
    ui->cbxmode->addItems(list);
    std::string listMissionName = LY_TaskManage::Get().GetDlgMissionList()->m_sMissionName;
    ui->cbxmode->setCurrentText(QString::fromStdString(listMissionName));
    ui->cbxmode->setEnabled(false);

}

void CDlgMissionAllocation::on_indicator_hidePopupedDJ()
{
    m_pCurrPlatAmmoVec.clear();
    for (int i = 0; i < ui->tbwdata->rowCount(); i++)
    {
        MutiComboBox *mutiComboBox = qobject_cast<MutiComboBox *>(ui->tbwdata->cellWidget(i, 7));
        MutiComboBox *mutiComboBox2 = qobject_cast<MutiComboBox *>(ui->tbwdata->cellWidget(i, 5));
        QString secmount, primount;
        if (mutiComboBox != nullptr)
        {
            secmount = mutiComboBox->lineEdit()->text();
        }
        if (mutiComboBox2 != nullptr)
        {
            primount = mutiComboBox2->lineEdit()->text();
        }
        m_pCurrPlatAmmoVec.push_back(make_pair(primount, secmount));
    }

    QStringList tmplist, tmplist2, tmpcurrlist;

    for (int k = 0; k < ui->tbwdata->rowCount(); k++)
    {
        MutiComboBox *mutiComboBoxexist = qobject_cast<MutiComboBox *>(ui->tbwdata->cellWidget(k, 5));
        if (mutiComboBoxexist)
        {
            QString lineQStr = mutiComboBoxexist->lineEdit()->text();
            tmplist = lineQStr.split(";");
            for (auto it : tmplist)
            {
                tmplist2 << it;
            }
        }
    }
    tmpcurrlist = currList;
    for (auto it : tmplist2)
    {
        for (auto it2 : tmpcurrlist)
        {
            if (it2 == it)
            {
                tmpcurrlist.removeAll(it);
            }
        }
    }
    for (int k = 0; k < ui->tbwdata->rowCount(); k++)
    {
        if (qobject_cast<MutiComboBox *>(ui->tbwdata->cellWidget(k, 5)))
        {
            if (ui->tbwdata->currentRow() == k)
            {
                continue;
            }
            else
            {
                {
                    MutiComboBox *mutiComboBox = new MutiComboBox;
                    m_mAmmoModeData.clear();
                    map<QString, int> tmpmap;
                    QStringList mAmmoModeDataStr = m_pCurrPlatAmmoVec[k].first.split(";");
                    for (int i = 0; i < mAmmoModeDataStr.size(); i++)
                    {
                        if (mAmmoModeDataStr[i].isEmpty())
                        {
                            continue;
                        }
                        tmpmap[mAmmoModeDataStr[i]] = i;
                    }
                    for (int j = 0; j < tmpcurrlist.size(); j++)
                    {
                        tmpmap[tmpcurrlist[j]] = j + mAmmoModeDataStr.size();
                    }
                    int index = 0;
                    QString strShowData;
                    for (auto it : tmpmap)
                    {
                        m_mAmmoModeData[index] = it.first;
                        for (int j = 0; j < mAmmoModeDataStr.size(); j++)
                        {
                            if (!mAmmoModeDataStr[j].isEmpty() && (m_mAmmoModeData[index] == mAmmoModeDataStr[j]))
                            {
                                strShowData += QString::number(index) + ";";
                            }
                        }
                        index++;
                    }
                    MutiComboBox *mutiComboBoxexist = qobject_cast<MutiComboBox *>(ui->tbwdata->cellWidget(k, 5));

                    mutiComboBox->setModelData(m_mAmmoModeData);
                    m_mRowPriAmmoModeData[k] = m_mAmmoModeData;
                    mutiComboBox->setCheckedItems(strShowData);
                    ui->tbwdata->setCellWidget(k, 5, mutiComboBox);
                }
            }

        }
    }
    on_createcombo_changed();
}

void CDlgMissionAllocation::on_indicator_hidePopuped2()
{
    m_pCurrPlatAmmoVec.clear();
    for (int i = 0; i < ui->tbwdata->rowCount(); i++)
    {
        MutiComboBox *mutiComboBox = qobject_cast<MutiComboBox *>(ui->tbwdata->cellWidget(i, 7));
        MutiComboBox *mutiComboBox2 = qobject_cast<MutiComboBox *>(ui->tbwdata->cellWidget(i, 5));
        QString secmount, primount;
        if (mutiComboBox != nullptr)
        {
            secmount = mutiComboBox->lineEdit()->text();
        }
        if (mutiComboBox2 != nullptr)
        {
            primount = mutiComboBox2->lineEdit()->text();
        }
        m_pCurrPlatAmmoVec.push_back(make_pair(primount, secmount));
    }

    QStringList tmplist, tmplist2, tmpcurrlist;

    for (int k = 0; k < ui->tbwdata->rowCount(); k++)
    {
        MutiComboBox *mutiComboBoxexist = qobject_cast<MutiComboBox *>(ui->tbwdata->cellWidget(k, 7));
        if (mutiComboBoxexist)
        {
            QString lineQStr = mutiComboBoxexist->lineEdit()->text();
            tmplist = lineQStr.split(";");
            for (auto it : tmplist)
            {
                tmplist2 << it;
            }
        }
    }
    tmpcurrlist = currList2;
    for (auto it : tmplist2)
    {
        for (auto it2 : tmpcurrlist)
        {
            if (it2 == it)
            {
                tmpcurrlist.removeAll(it);
            }
        }
    }
    for (int k = 0; k < ui->tbwdata->rowCount(); k++)
    {
        if (qobject_cast<MutiComboBox *>(ui->tbwdata->cellWidget(k, 7)))
        {
            if (ui->tbwdata->currentRow() == k)
            {
                continue;
            }
            else
            {
                {
                    MutiComboBox *mutiComboBox = new MutiComboBox;
                    m_mAmmoModeData.clear();
                    map<QString, int> tmpmap;
                    QStringList mAmmoModeDataStr = m_pCurrPlatAmmoVec[k].second.split(";");
                    for (int i = 0; i < mAmmoModeDataStr.size(); i++)
                    {
                        if (mAmmoModeDataStr[i].isEmpty())
                        {
                            continue;
                        }
                        tmpmap[mAmmoModeDataStr[i]] = i;
                    }
                    for (int j = 0; j < tmpcurrlist.size(); j++)
                    {
                        tmpmap[tmpcurrlist[j]] = j + mAmmoModeDataStr.size();
                    }
                    int index = 0;
                    QString strShowData;
                    for (auto it : tmpmap)
                    {
                        m_mAmmoModeData[index] = it.first;
                        for (int j = 0; j < mAmmoModeDataStr.size(); j++)
                        {
                            if (!mAmmoModeDataStr[j].isEmpty() && (m_mAmmoModeData[index] == mAmmoModeDataStr[j]))
                            {
                                strShowData += QString::number(index) + ";";
                            }
                        }
                        index++;
                    }
                    MutiComboBox *mutiComboBoxexist = qobject_cast<MutiComboBox *>(ui->tbwdata->cellWidget(k, 7));

                    mutiComboBox->setModelData(m_mAmmoModeData);
                    m_mRowSedAmmoModeData[k] = m_mAmmoModeData;
                    mutiComboBox->setCheckedItems(strShowData);
                    ui->tbwdata->setCellWidget(k, 7, mutiComboBox);
                }
            }

        }
    }
    on_createcombo_changed();
}

void CDlgMissionAllocation::ConnectHidePopupedDJ()
{
    int tabrow = ui->tbwdata->currentRow();
    QWidget *widgetCom0 = ui->tbwdata->cellWidget(tabrow, 5);
    MutiComboBox *mutiComboBoxexit = qobject_cast<MutiComboBox *>(widgetCom0);
    if (mutiComboBoxexit != nullptr)
    {
        const QString &lineQStr = mutiComboBoxexit->lineEdit()->text();
        const QString &strShowData = mutiComboBoxexit->getSelectedItemDatas();
        int num = 0;
        if (!strShowData.isEmpty())
        {
            num = strShowData.split(";").size();
        }
        QTableWidgetItem *item = ui->tbwdata->item(tabrow, 4);
        int currnum = item->text().section("_", 1, item->text().size()).toInt();
        if (num > currnum)
        {
            LyMessageBox::information(NULL, tr("tip"), tr("The ammo limit was exceeded"));
            mutiComboBoxexit->setCheckedItems("");
        }
    }
    int RowCount = ui->tbwdata->rowCount();
    for (int i = 0; i < RowCount; i++)
    {
        QWidget *widgetCom0 = ui->tbwdata->cellWidget(i, 5);
        MutiComboBox *mutiComboBox = qobject_cast<MutiComboBox *>(widgetCom0);

        disconnect(mutiComboBox, 0, 0, 0);
        connect(mutiComboBox, &MutiComboBox::hidePopuped, this, &CDlgMissionAllocation::on_indicator_hidePopupedDJ);
    }
}

void CDlgMissionAllocation::ConnectHidePopupedDJ2()
{
    int tabrow = ui->tbwdata->currentRow();
    QWidget *widgetCom0 = ui->tbwdata->cellWidget(tabrow, 7);
    MutiComboBox *mutiComboBox = qobject_cast<MutiComboBox *>(widgetCom0);
    if (mutiComboBox != nullptr)
    {
        const QString &lineQStr = mutiComboBox->lineEdit()->text();
        const QString &strShowData = mutiComboBox->getSelectedItemDatas();
        int num = strShowData.split(";").size();
        QTableWidgetItem *item = ui->tbwdata->item(tabrow, 6);
        int currnum = item->text().section("_", 1, item->text().size()).toInt();
        if (num > currnum)
        {
            LyMessageBox::information(NULL, tr("tip"), tr("The ammo limit was exceeded"));
            mutiComboBox->setCheckedItems("");

            return ;
        }
    }
    int RowCount = ui->tbwdata->rowCount();
    for (int i = 0; i < RowCount; i++)
    {
        QWidget *widgetCom0 = ui->tbwdata->cellWidget(i, 7);
        MutiComboBox *mutiComboBox = qobject_cast<MutiComboBox *>(widgetCom0);
        disconnect(mutiComboBox, 0, 0, 0);
        connect(mutiComboBox, &MutiComboBox::hidePopuped, this, &CDlgMissionAllocation::on_indicator_hidePopuped2);
    }
}

void CDlgMissionAllocation::DisConnectHidePopupedDJ()
{
    for (auto combo : NewComboVec)
    {
        disconnect(combo, 0, 0, 0);
    }
    for (int i = 0; i < ui->tbwdata->rowCount(); i++)
    {
        QWidget *widgetCom = ui->tbwdata->cellWidget(i, 5);
        MutiComboBox *mutiComboBox = qobject_cast<MutiComboBox *>(widgetCom);
        if (mutiComboBox != nullptr)
        {
            disconnect(mutiComboBox, 0, 0, 0);
        }
        QWidget *widgetCom02 = ui->tbwdata->cellWidget(i, 7);
        MutiComboBox *mutiComboBox2 = qobject_cast<MutiComboBox *>(widgetCom02);
        if (mutiComboBox2 != nullptr)
        {
            disconnect(mutiComboBox2, 0, 0, 0);
        }
    }
}

void CDlgMissionAllocation::SetPlatTypeSize()
{
    if (nullptr != ui)
    {
        for (int i = 0; i < ptpp.size(); i++)
        {
            QWidget *exist = ui->tbwdata->cellWidget(i, 1);
            QComboBox *comboBox = nullptr;

            if (exist && qobject_cast<QComboBox *>(exist))
            {
                comboBox = qobject_cast<QComboBox *>(exist);
            }
            if (comboBox == nullptr)
            {
                return;
            }
            comboBox->setFixedWidth(ui->tbwdata->columnWidth(1));
        }
    }
}

void CDlgMissionAllocation::ObtainAllData()
{
    int nActionID = m_pITaskManageService->GetActivatingActionID();
    m_vTaskVec = m_pITaskManageService->GetMissionsByAction(nActionID);
}

void CDlgMissionAllocation::CreateComboboxTaskID()
{
    ui->cbxTaskID->clear();
    for (auto data : m_vTaskVec)
    {
        ui->cbxTaskID->addItem(QString::fromStdString(data));
    }

    const std::string &ListMinssionName = LY_TaskManage::Get().GetDlgMissionList()->m_sMissionName;
    ui->cbxTaskID->setCurrentText(QString::fromStdString(ListMinssionName));
    const CNeubulaMission &MissionData = m_pITaskManageService->GetMission(ui->cbxTaskID->currentText().toStdString());
    ui->ledtTaskName->setText(QString::fromStdString(MissionData.GetName()));
    const std::string &str = ui->cbxTaskID->currentText().toStdString();
    ui->cbxTaskID->setEnabled(false);
}

void CDlgMissionAllocation::CreateTableWidget()
{
    QStringList header;
    ui->tbwMarkAmmoMatch->clear();
    header << tr("No.") << tr("Mark Number") << tr("Ammunition Number");
    ui->tbwMarkAmmoMatch->setColumnCount(header.size());
    ui->tbwMarkAmmoMatch->setRowCount(0);
    ui->tbwMarkAmmoMatch->clearContents();
    ui->tbwMarkAmmoMatch->setHorizontalHeaderLabels(header);
    ui->tbwMarkAmmoMatch->verticalHeader()->setHidden(true);
    ui->tbwMarkAmmoMatch->setSelectionBehavior(QTableWidget::SelectRows);
    ui->tbwMarkAmmoMatch->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tbwMarkAmmoMatch->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tbwMarkAmmoMatch->setAlternatingRowColors(true);
    ui->tbwMarkAmmoMatch->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->tbwMarkAmmoMatch->horizontalHeader()->setStretchLastSection(true);
}

void CDlgMissionAllocation::ObtainCurrentTextData()
{
    m_pAmmunitionManageService->LoadAmmoMarkDB();
    m_cConntionAmmoVec = m_pAmmunitionManageService->GetAmmunitionConnByTaskID(ui->cbxTaskID->currentText().toStdString());
    m_cAmmoConntionMarkVec = m_pAmmunitionManageService->GetTaskIDByInfo(ui->cbxTaskID->currentText().toStdString());
    CNeubulaConntionAmmoVec m_cConntionAmmoVecTemp;
    if (!m_cAmmoConntionMarkVec.empty())
    {
        for (auto temp : m_cConntionAmmoVec)
        {
            for (auto data : m_cAmmoConntionMarkVec)
            {
                if (data.GetAmmoMode() == temp.GetAmmoID())
                {
                    temp.SetIsMount(true);
                    break;
                }
            }
            m_cConntionAmmoVecTemp.push_back(temp);
        }
    }
    else
    {
        m_cConntionAmmoVecTemp = m_cConntionAmmoVec;
    }
    m_vAmmoModeVec.clear();
    for (auto data : m_cConntionAmmoVecTemp)
    {
        if (!data.GetIsMount())
        {
            m_vAmmoModeVec.push_back(data.GetAmmoID());
        }
    }
    m_taskMarkID = m_pITaskManageService->GetRelationMissionMark(ui->cbxTaskID->currentText().toStdString());
    m_vMarkAmmoModeMap.clear();
    for (auto data : m_taskMarkID)
    {
        std::vector<std::string> tempVec = m_pAmmunitionManageService->GetMarkIDByAmmoMode(m_cAmmoConntionMarkVec, data.GetMarkID());
        m_vMarkAmmoModeMap[data.GetMarkID()] = tempVec;
    }
}

void CDlgMissionAllocation::UpdatatbwMarkAmmoMatch()
{
    ui->tbwMarkAmmoMatch->setRowCount(m_taskMarkID.size());
    for (int i = 0; i < m_taskMarkID.size(); i++)
    {
        ui->tbwMarkAmmoMatch->setItem(i, 0, new QTableWidgetItem(QString::number(i + 1)));
        ui->tbwMarkAmmoMatch->setItem(i, 1, new QTableWidgetItem(tr(m_taskMarkID[i].GetMarkID().c_str())));
        MutiComboBox *mutiComboBox = new MutiComboBox;
        m_mAmmoModeDataDM.clear();
        int indiIndex = 0;
        QString strShowData;
        for (int j = 0; j < m_vMarkAmmoModeMap[m_taskMarkID[i].GetMarkID()].size(); j++)
        {
            m_mAmmoModeDataDM[indiIndex] = QString::fromStdString(m_vMarkAmmoModeMap[m_taskMarkID[i].GetMarkID()][j]);

            strShowData += QString::number(j) + ";";
            indiIndex += 1;
        }
        for (int j = 0; j < m_vAmmoModeVec.size(); j++)
        {
            m_mAmmoModeDataDM[indiIndex] = QString::fromStdString(m_vAmmoModeVec[j]);
            indiIndex += 1;
        }
        mutiComboBox->setModelData(m_mAmmoModeDataDM);
        mutiComboBox->setCheckedItems(strShowData);
        ui->tbwMarkAmmoMatch->setCellWidget(i, 2, mutiComboBox);
    }
    ConnectHidePopuped();
}

void CDlgMissionAllocation::ConnectHidePopuped()
{
    int RowCount = ui->tbwMarkAmmoMatch->rowCount();
    for (int i = 0; i < RowCount; i++)
    {
        QWidget *widgetCom0 = ui->tbwMarkAmmoMatch->cellWidget(i, 2);
        MutiComboBox *mutiComboBox = qobject_cast<MutiComboBox *>(widgetCom0);
        disconnect(mutiComboBox, 0, 0, 0);
        connect(mutiComboBox, &MutiComboBox::hidePopuped, this, &CDlgMissionAllocation::on_indicator_hidePopuped);
    }
}

void CDlgMissionAllocation::DisConnectHidePopuped()
{
    int RowCount = ui->tbwMarkAmmoMatch->rowCount();
    for (int i = 0; i < RowCount; i++)
    {
        QWidget *widgetCom0 = ui->tbwMarkAmmoMatch->cellWidget(i, 2);
        MutiComboBox *mutiComboBox = qobject_cast<MutiComboBox *>(widgetCom0);
        disconnect(mutiComboBox, 0, 0, 0);
    }
    ui->tbwMarkAmmoMatch->clear();
}

void CDlgMissionAllocation::SaveData()
{
    m_pAmmunitionManageService->RemoveAmmoConntionMark(m_taskMarkID[0].GetMissionID());
    CNeubulaAmmoConntionMark newAmmoMark;
    bool tempBool;
    for (auto data : m_taskMarkID)
    {
        std::vector<std::string> targetAmmo = m_vMarkAmmoModeMap[data.GetMarkID()];
        for (auto temp : targetAmmo)
        {
            newAmmoMark.SetAmmoMode(temp);
            newAmmoMark.SetMarkID(data.GetMarkID());
            newAmmoMark.SetTaskID(data.GetMissionID());
            tempBool = m_pAmmunitionManageService->AddAmmoConntionMark(newAmmoMark);
            if (!tempBool)
            {
                LyMessageBox::information(NULL, tr("tip"), tr("Save failed!"));
            }
        }
    }
    if (tempBool)
    {
        LyMessageBox::information(NULL, tr("tip"), tr("Save successfully!"));
    }
    m_pAmmunitionManageService->LoadAmmoMarkDB();
}

void CDlgMissionAllocation::showEvent(QShowEvent *event)
{
    Init();
}

void CDlgMissionAllocation::closeEvent(QCloseEvent *event)
{
    DisConnectHidePopuped();
    DisConnectHidePopupedDJ();
}

void CDlgMissionAllocation::resizeEvent(QResizeEvent *event)
{
    if (nullptr != ui)
    {
        for (int i = 0; i < ptpp.size(); i++)
        {
            QWidget *exist = ui->tbwdata->cellWidget(i, 1);
            QComboBox *comboBox = nullptr;

            if (exist && qobject_cast<QComboBox *>(exist))
            {
                comboBox = qobject_cast<QComboBox *>(exist);
            }
            if (comboBox == nullptr)
            {
                return;
            }
            comboBox->setFixedWidth(ui->tbwdata->columnWidth(1));
        }
    }
}

void CDlgMissionAllocation::on_pbEnsure_clicked()
{
    char pBuf[MAX_BUFFER_LEN] = { 0 };
    int nBufLen = 0;
    qnzkna::framework::MessageReferenceProps  props;
    std::string str = "1";
    props.put("UpdatePlatMateList", str);
    ITaskManagerActivator::Get().getIRegistryImpl()->messageNotify(new qnzkna::framework::MessageReference("MsgPlatMateMerge", pBuf, nBufLen, props));
}

void CDlgMissionAllocation::on_pbCancel_clicked()
{
    Init();
}

void CDlgMissionAllocation::checkIsRepetition(const QString &text)
{
    if (ui->tableWidget->currentRow() >= platType.getPlatNames_Selected().size() || ui->tableWidget->currentRow() < 0)
    {
        return;
    }
    if (text.toStdString() == platType.getPlatNames_Selected()[ui->tableWidget->currentRow()])
    {
        return;
    }
    auto missionWeaponTypeAndNames = missionWeaponTypeAndNames_usabled;
    if (missionWeaponTypeAndNames.empty())
    {
        return;
    }
    auto missionItor = missionWeaponTypeAndNames.find(platType.getMissionID());
    if (missionItor == missionWeaponTypeAndNames.end())
    {
        return;
    }
    auto weaponTypeItor = missionItor->second.find(platType.getWeaponType());
    if (weaponTypeItor == missionItor->second.end())
    {
        return;
    }

    auto textWeaponItor = std::find(weaponTypeItor->second.begin(), weaponTypeItor->second.end(), text.toStdString());

    if (textWeaponItor != weaponTypeItor->second.end() || text == " ")
    {
        vector<string> selNames = platType.getPlatNames_Selected();
        if (ui->tableWidget->currentRow() >= selNames.size())
        {
            return;
        }
        string oldName = selNames[ui->tableWidget->currentRow()];
        selNames[ui->tableWidget->currentRow()] = text.toStdString();
        platType.setPlatNames_Selected(selNames);
        updateMissionTypeOldToNewName(platType.getMissionID(), platType.getWeaponType(),
                                      platType.getPlatTypeName(), ui->tableWidget->currentRow(),
                                      oldName, text.toStdString());

    }
    else
    {

        vector<string> selNames = platType.getPlatNames_Selected();
        auto newNameItor = std::find(selNames.begin(), selNames.end(), text.toStdString());

        if (newNameItor != selNames.end())
        {
            string oldName = selNames[ui->tableWidget->currentRow()];
            int oldIndex = -1;
            for (int i = 0; i < selNames.size(); i++)
            {
                if (selNames[i] == text.toStdString())
                {
                    oldIndex = i;
                    break;
                }
            }
            if (oldIndex != -1)
            {
                selNames[oldIndex] = oldName;
                selNames[ui->tableWidget->currentRow()] = text.toStdString();
                QComboBox *tmpCell = (QComboBox *)(ui->tableWidget->cellWidget(oldIndex, 2));
                tmpCell->setCurrentText(QString::fromStdString(oldName));
                platType.setPlatNames_Selected(selNames);

            }
        }
        else
        {

        }
    }
    updateItemsColor();
}

void CDlgMissionAllocation::ctrlChanged(const QString &text)
{
    if (ui->tableWidget->currentRow() >= 0 && ui->tableWidget->currentRow() < ui->tableWidget->rowCount())
    {
        vector<string> ctrlers = platType.getControlers();
        ctrlers[ui->tableWidget->currentRow()] = text.toStdString();
        platType.setControlers(ctrlers);
        updateCtrler(platType.getMissionID(), platType.getPlatTypeName(),
                     ui->tableWidget->currentRow(), text.toStdString());
    }
}

void CDlgMissionAllocation::setCurMission(int curIndex)
{
    const QString &currentText = ui->comboBoxMissionName->itemText(curIndex);
    const QStringList &str = currentText.split('/');
    for (auto data : missionPlats)
    {
        missionPlatType newTemp = *data;
        if (newTemp.getMissionID() == str[0].toStdString() && newTemp.getPlatTypeName() == str[1].toStdString())
        {
            initMissionName(QString::fromStdString(data->getMissionName()));
            setPlatType(newTemp);
        }
    }

}

void CDlgMissionAllocation::on_cbxmode_currentTextChanged()
{

    const QString &taskID = ui->cbxmode->currentText();
    if (m_pMissionAllVec.empty())
    {
        return;
    }
    for (auto miss : m_pMissionAllVec)
    {
        if (taskID == miss.GetID().c_str())
        {
            ui->leTaskName->setText(miss.GetName().c_str());
        }
    }
    ui->tbwdata->clearContents();
    ui->tbwdata->setRowCount(0);
    m_pRelationMissionResourceCurrVec.clear();
    NewComboVec.clear();
    m_AmmoCurrVec.clear();
    ptpp.clear();
    m_pCurrPlatAmmoVec.clear();
    loadindex = 0;
    is_init = false;

    for (int i = 0; i < m_pRelationMissionResourceVec.size(); i++)
    {
        const QString &currmissionid = QString::fromStdString(m_pRelationMissionResourceVec[i].GetMissionID());
        if (taskID == currmissionid)
        {
            m_pRelationMissionResourceCurrVec.push_back(m_pRelationMissionResourceVec[i]);
        }
    }
    m_tmpread = m_pRelationMissionResourceCurrVec;

    if (!m_pRelationMissionResourceCurrVec.empty())
    {
        const vector<string> &tmp = ITaskManagerActivator::Get().getPTaskManageService()->readPlatFormMatch(m_pRelationMissionResourceCurrVec[0].GetResourceName(), taskID.toStdString());
        ptpp.insert(ptpp.end(), tmp.begin(), tmp.end());
        for (auto iter : tmp)
        {
            idname.push_back(make_pair(m_pRelationMissionResourceCurrVec[0].GetResourceName(), iter));
        }
        for (int i = 1; i < m_pRelationMissionResourceCurrVec.size(); i++)
        {
            const QString &tmpstr = tr2(m_pRelationMissionResourceCurrVec[i].GetResourceName().c_str());

            const vector<string> &tmp = ITaskManagerActivator::Get().getPTaskManageService()->readPlatFormMatch(m_pRelationMissionResourceCurrVec[i].GetResourceName(), taskID.toStdString());
            if (!tmp.empty() && !ptpp.empty())
            {
                if (tmp.back() != ptpp.back())
                {
                    for (int j = 0; j < tmp.size(); j++)
                    {
                        ptpp.push_back(tmp[j]);
                        idname.push_back(make_pair(m_pRelationMissionResourceCurrVec[i].GetResourceName(), tmp[j]));
                    }
                }
            }
        }
    }
    QStringList list;
    for (auto it : m_pRelationMissionResourceCurrVec)
    {
        list << QString::fromStdString(it.GetResourceName()) + QString::number(it.GetID());
    }

    int row = 0;
    for (int i = 0; i < ptpp.size(); i++)
    {
        ui->tbwdata->insertRow(row);
        const std::string &strID = ptpp[i];
        bool isGetWeaponComponentName = ITaskManagerActivator::Get().getPWeaponManageService()->GetWeaponComponentName(WeaponComponentName, strID);
        ui->tbwdata->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(WeaponComponentName)));

        QComboBox *combo = new QComboBox;
        combo->addItem("");
        for (int j = 0; j < m_pRelationMissionResourceCurrVec.size(); j++)
        {
            if (m_pRelationMissionResourceCurrVec[j].GetResourceName() == idname[i].first)
            {
                combo->addItem(list[j]);
            }
        }
        combo->setCurrentText("");
        m_AmmoCurrVec.push_back(make_pair(row, ""));
        NewComboVec.push_back(combo);
        ui->tbwdata->setCellWidget(row, 1, NewComboVec[row]);
        row++;
    }
    for (auto combo : NewComboVec)
    {
        connect(combo, QOverload<const QString &>::of(&QComboBox::currentTextChanged), this, &CDlgMissionAllocation::on_createcombo_changed);
    }

    const CNeubulaConntionAmmoVec &cNeubulaConntionAmmoVec = ITaskManagerActivator::Get().getPAmmunitionManageService()->GetAmmunitionConnByTaskID(taskID.toStdString());
    if (!cNeubulaConntionAmmoVec.empty())
    {
		QStringList tmplist = list;
        CRelationMissionResourceVec tmpread = m_tmpread;
        for (int i = 0; i < ptpp.size(); i++)
        {
            for (auto it : cNeubulaConntionAmmoVec)
            {
                if (it.GetPlatID() == ptpp[i])
                {
                    QWidget *exist = ui->tbwdata->cellWidget(i, 1);
                    QComboBox *comboBox = nullptr;

                    if (exist && qobject_cast<QComboBox *>(exist))
                    {
                        comboBox = qobject_cast<QComboBox *>(exist);
                    }
                    if (comboBox == nullptr)
                    {
                        return;
                    }
                    int dbunm = it.GetRelationID();
                    for (int j = tmpread.size() - 1; j >= 0; j--)
                    {
                        if (dbunm == tmpread[j].GetID())
                        {
							comboBox->setCurrentText(tmplist[j]);
							tmplist.erase(tmplist.begin() + j);
                            tmpread.erase(tmpread.begin() + j);
                            break;
                        }
                    }
                    break;
                }
            }
        }
    }
}

void CDlgMissionAllocation::on_btnconfirm_clicked()
{
    const QString &rwbh = ui->cbxmode->currentText();
    CNeubulaConntionAmmoVec cNeubulaConntionAmmoVec = ITaskManagerActivator::Get().getPAmmunitionManageService()->GetAmmunitionConnByTaskID(rwbh.toStdString());
    CNeubulaConntionAmmoVec dbAmmoVec = cNeubulaConntionAmmoVec;
    cNeubulaConntionAmmoVec.clear();
    int rowcount = ui->tbwdata->rowCount();
    for (int i = 0; i < rowcount; i++)
    {
        QWidget *widgetCom0 = ui->tbwdata->cellWidget(i, 5);
        MutiComboBox *mutiComboBox = qobject_cast<MutiComboBox *>(widgetCom0);
        if (mutiComboBox != nullptr)
        {
            const string &ptbh = ptpp[i];
            const QString &dyxh = ui->tbwdata->item(i, 4)->text().section("_", 0, 0);
            const QStringList &dybhs = mutiComboBox->lineEdit()->text().split(";");
            for (auto it : dybhs)
            {
                if (!(it.isEmpty()))
                {
                    CNeubulaConntionAmmo tmp;
                    tmp.SetPlatID(ptbh);
                    tmp.SetType(dyxh.toStdString());
                    tmp.SetAmmoID(it.toStdString());
                    tmp.SetLoadMethod(0);
                    tmp.SetTaskID(rwbh.toStdString());
                    const QString &tmpstr = NewComboVec[i]->currentText();
                    int numb = tmpstr.section(tr2("Òí"), 1, tmpstr.size()).toInt();
                    tmp.SetRelationID(numb);
                    cNeubulaConntionAmmoVec.push_back(tmp);
                }
            }
        }
        QWidget *widgetCom02 = ui->tbwdata->cellWidget(i, 7);
        MutiComboBox *mutiComboBox2 = qobject_cast<MutiComboBox *>(widgetCom02);
        if (mutiComboBox2 != nullptr)
        {
            const string &ptbh = ptpp[i];
            const QString &dyxh = ui->tbwdata->item(i, 6)->text().section("_", 0, 0);
            const QStringList &dybhs = mutiComboBox2->lineEdit()->text().split(";");
            for (auto it : dybhs)
            {
                if (!(it.isEmpty()))
                {
                    CNeubulaConntionAmmo tmp;
                    tmp.SetPlatID(ptbh);
                    tmp.SetType(dyxh.toStdString());
                    tmp.SetAmmoID(it.toStdString());
                    tmp.SetLoadMethod(1);
                    tmp.SetTaskID(rwbh.toStdString());
                    const QString &tmpstr = NewComboVec[i]->currentText();
                    int numb = tmpstr.section(tr2("Òí"), 1, tmpstr.size()).toInt();
                    tmp.SetRelationID(numb);
                    cNeubulaConntionAmmoVec.push_back(tmp);
                }
            }
        }
    }
    if (cNeubulaConntionAmmoVec.size() == 0)
    {
        LyMessageBox::information(NULL, tr("tip"), tr("Lack of ammo mounts"));
    }
    bool bSucc = ITaskManagerActivator::Get().getPAmmunitionManageService()->AddAmmunitionConnDB(cNeubulaConntionAmmoVec);
    CNeubulaAmmunitionInfo data;
    for (auto it : cNeubulaConntionAmmoVec)
    {
        data = ITaskManagerActivator::Get().getPAmmunitionManageService()->GetAmmoByID(it.GetAmmoID());
        if (!data.GetisVirtual())
        {
            data.SetisLoaded(true);
            bool bSucc1 = ITaskManagerActivator::Get().getPAmmunitionManageService()->EditAmmo(data);
        }
    }
    for (auto it : dbAmmoVec)
    {
        int fl = 1;
        for (int j = 0; j < cNeubulaConntionAmmoVec.size(); j++)
        {
            if (cNeubulaConntionAmmoVec[j].GetAmmoID() == it.GetAmmoID())
            {
                fl = 0;
                break;
            }

        }
        if (fl)
        {
            data = ITaskManagerActivator::Get().getPAmmunitionManageService()->GetAmmoByID(it.GetAmmoID());
            if (!data.GetisVirtual())
            {
                data.SetisLoaded(false);
                bool bSucc2 = ITaskManagerActivator::Get().getPAmmunitionManageService()->EditAmmo(data);
            }
        }
    }
    if (bSucc)
    {
        if (QMessageBox::Ok == LyMessageBox::information(nullptr, tr("tip"), tr("Save successfully"), QMessageBox::Ok))
        {
            DisConnectHidePopupedDJ();
        }
    }
    else
    {
        LyMessageBox::information(NULL, tr("tip"), tr("Save failed"));
    }
    char pBuf[MAX_BUFFER_LEN] = { 0 };
    int nBufLen = 0;
    qnzkna::framework::MessageReferenceProps  props;
    std::string str = "1";
    props.put("UpdatePlatAmmoList", str);
    ITaskManagerActivator::Get().getIRegistryImpl()->messageNotify(new qnzkna::framework::MessageReference("MsgPlatAmmoID", pBuf, nBufLen, props));
    ITaskManagerActivator::Get().getPAmmunitionManageService()->LoadAmmoPlatDB();
    Init();
}

void CDlgMissionAllocation::on_btnexit_clicked()
{
    DisConnectHidePopupedDJ();
    Init();
}

void CDlgMissionAllocation::on_createcombo_changed()
{
    int tabrow = ui->tbwdata->currentRow();
    if (tabrow == -1)
    {
        for (int j = 0; j < NewComboVec.size(); j++)
        {
            QString str = NewComboVec[j]->currentText();
            if (!str.isEmpty())
            {
                tabrow = j;
            }
        }
    }
    QWidget *exist = ui->tbwdata->cellWidget(tabrow, 1);
    QComboBox *comboBox = nullptr;

    if (exist && qobject_cast<QComboBox *>(exist))
    {
        comboBox = qobject_cast<QComboBox *>(exist);
    }
    if (comboBox == nullptr)
    {
        return;
    }
    QString newText = "";
    if (comboBox->currentText() != nullptr)
    {
        newText = comboBox->currentText();
    }
    QString lastText = "";
    for (auto ammo : m_AmmoCurrVec)
    {
        if (ammo.first == tabrow)
        {
            lastText = ammo.second;
            m_AmmoCurrVec[tabrow].second = newText;
            break;
        }
    }

    int rowcount = ui->tbwdata->rowCount();
    vector<int>spiltidname;
    if (idname.size() > 1)
    {
        string lastidname = idname[0].first;
        for (int i = 1; i < idname.size(); i++)
        {
            if (idname[i].first != lastidname)
            {
                spiltidname.push_back(i - 1);
                lastidname = idname[i].first;
            }
        }
    }
    spiltidname.push_back(idname.size() - 1);

    int start = 0;
    for (auto it : spiltidname)
    {
        if (tabrow > it)
        {
            continue;
        }
        else
        {
            for (int i = start; i <= it; i++)
            {
                if (i == tabrow)
                {
                    continue;
                }
                QComboBox *combo = nullptr;
                QWidget *existwid = ui->tbwdata->cellWidget(i, 1);
                if (existwid && qobject_cast<QComboBox *>(existwid))
                {
                    combo = qobject_cast<QComboBox *>(existwid);
                }
                if (combo == nullptr)
                {
                    continue;
                }
                if (lastText != "")
                {
                    int retNum = combo->findText(lastText);
                    if (retNum == -1)
                    {
                        combo->addItem(lastText);
                    }
                }
                if (newText != "")
                {
                    int retNum = combo->findText(newText);
                    if (retNum != -1)
                    {
                        combo->removeItem(retNum);
                    }
                }
            }
            break;
        }
    }

    CNeubulaConntionAmmoVec cNeubulaConntionAmmoVec = ITaskManagerActivator::Get().getPAmmunitionManageService()->GetAmmunitionConnByTaskID(ui->cbxmode->currentText().toStdString());

    for (auto it : m_pRelationMissionResourceCurrVec)
    {
        int currid = newText.section(tr2("Òí"), 1, newText.size()).toInt();
        if (currid == it.GetID())
        {
            ui->tbwdata->setItem(tabrow, 2, new QTableWidgetItem(QString::fromStdString(it.GetPrimaryPoc())));
            ui->tbwdata->setItem(tabrow, 3, new QTableWidgetItem(QString::fromStdString(it.GetSecondPod())));
            if (it.GetPriMountNum() != 0)
            {
                const QString &PriMountType = QString::fromStdString(it.GetPriMountType());
                int PriMountNum = it.GetPriMountNum();
                ui->tbwdata->setItem(tabrow, 4, new QTableWidgetItem(PriMountType + "_" + QString::number(PriMountNum)));

                QStringList list, tmplist, tmplist2;
                for (auto info : m_pAllAmmoVec)
                {
                    const QString &strAmmo = PriMountType;
                    QString ammoType = "";
                    QString ammoModel = "";
                    for (auto ammo : m_pAmmoType)
                    {
                        QString strname = QString::fromStdString(ammo.GetLoadName());
                        if (strAmmo == strname)
                        {
                            ammoType = QString::number(ammo.GetLoadType());
                            ammoModel = strname;
                            break;
                        }
                    }
                    if (ammoType == QString::fromStdString(info.GetAmmoType()) && tr2(ammoModel.toStdString().c_str()) == tr2(info.GetAmmoMode().c_str()))
                    {
                        for (auto it : cNeubulaConntionAmmoVec)
                        {
                            if (currid == it.GetRelationID() && it.GetAmmoID() == info.GetID())
                            {
                                list << info.GetID().c_str();
                                break;
                            }
                        }
                        if (list.indexOf(info.GetID().c_str()) == -1)
                        {
                            if (!info.GetisVirtual() && !info.GetisLoaded() || info.GetisVirtual())
                            {
                                int fl = 1;
                                for (auto it : cNeubulaConntionAmmoVec)
                                {
                                    if (it.GetAmmoID() == info.GetID())
                                    {
                                        fl = 0;
                                        break;
                                    }
                                }
                                if (fl)
                                {
                                    list << info.GetID().c_str();
                                }
                            }
                        }
                    }
                }
                currList = list;

                for (int k = 0; k < rowcount; k++)
                {
                    QWidget *widgetCom0 = ui->tbwdata->cellWidget(k, 5);
                    MutiComboBox *mutiComboBoxexist = qobject_cast<MutiComboBox *>(widgetCom0);
                    if (mutiComboBoxexist)
                    {
                        const QString &lineQStr = mutiComboBoxexist->lineEdit()->text();
                        tmplist = lineQStr.split(";");
                        for (auto it : tmplist)
                        {
                            tmplist2 << it;
                        }
                    }
                }
                for (auto it : tmplist2)
                {
                    for (auto it2 : list)
                    {
                        if (it2 == it)
                        {
                            list.removeAll(it);
                        }
                    }
                }
                QMap<int, QString> tmpAmmpModeData;
                for (int k = 0; k < currList.size(); k++)
                {
                    tmpAmmpModeData[k] = currList[k];
                }
                MutiComboBox *mutiComboBox = new MutiComboBox;
                QString strShowData;
                m_mAmmoModeData.clear();
                for (int k = 0; k < list.size(); k++)
                {
                    m_mAmmoModeData.insert(k, list[k]);
                }
                if (tabrow == ui->tbwdata->currentRow())
                {
                    if (!m_mRowPriAmmoModeData[tabrow].empty())
                    {
                        mutiComboBox->setModelData(m_mRowPriAmmoModeData[tabrow]);
                    }
                    else
                    {
                        mutiComboBox->setModelData(tmpAmmpModeData);
                        m_mRowPriAmmoModeData[tabrow] = m_mAmmoModeData;
                    }
                }
                else
                {
                    mutiComboBox->setModelData(m_mAmmoModeData);
                    m_mRowPriAmmoModeData[tabrow] = m_mAmmoModeData;
                }
                if (!is_init)
                {
                    for (int k = 0; k < cNeubulaConntionAmmoVec.size(); k++)
                    {
                        if (cNeubulaConntionAmmoVec[k].GetRelationID() == currid && cNeubulaConntionAmmoVec[k].GetLoadMethod() == 0)
                        {
                            int n = m_mRowPriAmmoModeData[tabrow].key(QString::fromStdString(cNeubulaConntionAmmoVec[k].GetAmmoID()));
                            strShowData += QString::number(n) + ";";
                        }
                    }
                }
                if (m_pCurrPlatAmmoVec.size() > tabrow)
                {
                    const QStringList &mAmmoModeDataStr = m_pCurrPlatAmmoVec[tabrow].first.split(";");
                    if (mAmmoModeDataStr[0].isEmpty())
                    {
                        strShowData = "";
                    }
                    for (int j = 0; j < m_mRowPriAmmoModeData[tabrow].size(); j++)
                    {
                        for (int k = 0; k < mAmmoModeDataStr.size(); k++)
                        {
                            if (!mAmmoModeDataStr[k].isEmpty() && (m_mRowPriAmmoModeData[tabrow][j] == mAmmoModeDataStr[k]))

                            {
                                strShowData += QString::number(j) + ";";
                            }
                        }
                    }
                }
                mutiComboBox->setCheckedItems(strShowData);
                ui->tbwdata->setCellWidget(tabrow, 5, mutiComboBox);
                ConnectHidePopupedDJ();
            }
            else
            {
                ui->tbwdata->setItem(tabrow, 4, new QTableWidgetItem());
                ui->tbwdata->setItem(tabrow, 5, new QTableWidgetItem());
            }
            if (it.GetSecMountNum() != 0)
            {
                const QString &SecMountType = QString::fromStdString(it.GetSecMountType());
                int SecMountNum = it.GetSecMountNum();

                ui->tbwdata->setItem(tabrow, 6, new QTableWidgetItem(QString::fromStdString(it.GetSecMountType()) +
                                                                     "_" + QString::number(it.GetSecMountNum())));

                QStringList list, tmplist, tmplist2;
                for (auto info : m_pAllAmmoVec)
                {
                    const QString &strAmmo = SecMountType;
                    QString ammoType = "";
                    QString ammoModel = "";
                    for (auto ammo : m_pAmmoType)
                    {
                        QString strname = QString::fromStdString(ammo.GetLoadName());
                        if (strAmmo == strname)
                        {
                            ammoType = QString::number(ammo.GetLoadType());
                            ammoModel = strname;
                            break;
                        }
                    }
                    if (ammoType == QString::fromStdString(info.GetAmmoType()) && tr2(ammoModel.toStdString().c_str()) == tr2(info.GetAmmoMode().c_str()))
                    {

                        if (!info.GetisVirtual())
                        {

                            if (info.GetisLoaded())
                            {
                                for (auto it : cNeubulaConntionAmmoVec)
                                {
                                    if (currid == it.GetRelationID() && it.GetAmmoID() == info.GetID())
                                    {
                                        list << info.GetID().c_str();
                                        break;
                                    }
                                }
                                continue;
                            }
                        }

                        else
                        {
                            for (auto it : cNeubulaConntionAmmoVec)
                            {

                                if (currid == it.GetRelationID() && it.GetAmmoID() == info.GetID())
                                {
                                    list << info.GetID().c_str();
                                    break;
                                }
                            }
                        }
                        if (list.indexOf(info.GetID().c_str()) == -1)
                        {
                            int fl = 1;
                            for (auto it : cNeubulaConntionAmmoVec)
                            {
                                if (it.GetAmmoID() == info.GetID())
                                {
                                    fl = 0;
                                    break;
                                }
                            }
                            if (fl)
                            {
                                list << info.GetID().c_str();
                            }
                        }
                    }
                }
                currList2 = list;

                for (int k = 0; k < rowcount; k++)
                {
                    QWidget *widgetCom0 = ui->tbwdata->cellWidget(k, 7);
                    MutiComboBox *mutiComboBoxexist = qobject_cast<MutiComboBox *>(widgetCom0);
                    if (mutiComboBoxexist)
                    {
                        const QString &lineQStr = mutiComboBoxexist->lineEdit()->text();
                        tmplist = lineQStr.split(";");
                        for (auto it : tmplist)
                        {
                            tmplist2 << it;
                        }
                    }
                }
                for (auto it : tmplist2)
                {
                    for (auto it2 : list)
                    {
                        if (it2 == it)
                        {
                            list.removeAll(it);
                        }
                    }
                }
                QMap<int, QString> tmpAmmpModeData;
                for (int k = 0; k < currList2.size(); k++)
                {
                    tmpAmmpModeData[k] = currList2[k];
                }
                MutiComboBox *mutiComboBox = new MutiComboBox;
                QString strShowData;
                m_mAmmoModeData.clear();
                for (int k = 0; k < list.size(); k++)
                {
                    m_mAmmoModeData.insert(k, list[k]);
                }

                if (tabrow == ui->tbwdata->currentRow())
                {
                    if (!m_mRowSedAmmoModeData[tabrow].empty())
                    {
                        mutiComboBox->setModelData(m_mRowSedAmmoModeData[tabrow]);
                    }
                    else
                    {
                        mutiComboBox->setModelData(tmpAmmpModeData);
                        m_mRowSedAmmoModeData[tabrow] = tmpAmmpModeData;
                    }
                }
                else
                {
                    mutiComboBox->setModelData(m_mAmmoModeData);
                    m_mRowSedAmmoModeData[tabrow] = m_mAmmoModeData;
                }

                if (!is_init)
                {
                    for (int k = 0; k < cNeubulaConntionAmmoVec.size(); k++)
                    {
                        if (cNeubulaConntionAmmoVec[k].GetRelationID() == currid && cNeubulaConntionAmmoVec[k].GetLoadMethod() == 1)
                        {
                            int n = m_mRowSedAmmoModeData[tabrow].key(QString::fromStdString(cNeubulaConntionAmmoVec[k].GetAmmoID()));
                            strShowData += QString::number(n) + ";";
                        }
                    }
                }
                if (m_pCurrPlatAmmoVec.size() > tabrow)
                {
                    const QStringList &mAmmoModeDataStr = m_pCurrPlatAmmoVec[tabrow].second.split(";");
                    if (mAmmoModeDataStr[0].isEmpty())
                    {
                        strShowData = "";
                    }
                    for (int j = 0; j < m_mRowSedAmmoModeData[tabrow].size(); j++)
                    {
                        for (int k = 0; k < mAmmoModeDataStr.size(); k++)
                        {
                            if (!mAmmoModeDataStr[k].isEmpty() && (m_mRowSedAmmoModeData[tabrow][j] == mAmmoModeDataStr[k]))

                            {
                                strShowData += QString::number(j) + ";";
                            }
                        }
                    }
                }
                mutiComboBox->setCheckedItems(strShowData);

                ui->tbwdata->setCellWidget(tabrow, 7, mutiComboBox);
                ConnectHidePopupedDJ2();
            }
            else
            {
                ui->tbwdata->setItem(tabrow, 6, new QTableWidgetItem());
                ui->tbwdata->setItem(tabrow, 7, new QTableWidgetItem());
            }

            break;
        }
        else if (currid == 0)
        {
            for (int k = 2; k <= 7; k++)
            {
                if (k == 5 || k == 7)
                {
                    QWidget *exist = ui->tbwdata->cellWidget(tabrow, k);
                    if (exist && qobject_cast<QComboBox *>(exist))
                    {
                        ui->tbwdata->removeCellWidget(tabrow, k);
                    }
                    delete exist;
                }
                else
                {
                    ui->tbwdata->setItem(tabrow, k, new QTableWidgetItem());
                }

            }
        }
    }
    loadindex++;
    if (cNeubulaConntionAmmoVec.size() > 1)
    {
        vector<string>dbplatnum;
        dbplatnum.push_back(cNeubulaConntionAmmoVec[0].GetPlatID());
        for (int j = 1; j < cNeubulaConntionAmmoVec.size(); j++)
        {
            if (dbplatnum.back() != cNeubulaConntionAmmoVec[j].GetPlatID())
            {
                dbplatnum.push_back(cNeubulaConntionAmmoVec[j].GetPlatID());
            }
        }
        if (loadindex == dbplatnum.size())
        {
            is_init = true;
        }
    }
    else
    {
        is_init = true;
    }
}

void CDlgMissionAllocation::on_pbtnOK_Clicked()
{
    SaveData();
    char pBuf[MAX_BUFFER_LEN] = { 0 };
    int nBufLen = 0;
    qnzkna::framework::MessageReferenceProps  props;
    std::string str = "1";
    props.put("UpdateInfo", str);
    ITaskManagerActivator::Get().getIRegistryImpl()->messageNotify(new qnzkna::framework::MessageReference("MsgMarkAmmoID", pBuf, nBufLen, props));
}

void CDlgMissionAllocation::on_pbtnCancel_Clicked()
{
    DisConnectHidePopuped();
    Init();
}

void CDlgMissionAllocation::on_currentIndexChangedTaskID(const QString &text)
{
    IsHidePopuped = true;
    ui->cbxTaskID->setCurrentText(text);
    const CNeubulaMission &MissionData = m_pITaskManageService->GetMission(text.toStdString());
    ui->ledtTaskName->setText(QString::fromStdString(MissionData.GetName()));
    ObtainCurrentTextData();
    UpdatatbwMarkAmmoMatch();
    IsHidePopuped = false;
}

void CDlgMissionAllocation::on_indicator_hidePopuped()
{
    if (!IsHidePopuped)
    {
        int row = ui->tbwMarkAmmoMatch->currentRow();
        if (row != -1)
        {
            QWidget *widgetCom0 = ui->tbwMarkAmmoMatch->cellWidget(row, 2);
            MutiComboBox *mutiComboBox = qobject_cast<MutiComboBox *>(widgetCom0);
            QLineEdit *lineEd = mutiComboBox->lineEdit();
            const QString &lineQStr = lineEd->text();

            std::vector<std::string> strAmmoMarkID;
            const std::string &targetID = ui->tbwMarkAmmoMatch->item(row, 1)->text().toStdString();
            if (!lineQStr.isEmpty())
            {
                m_indicators.clear();
                m_indicators = lineQStr.split(';');
                for (int i = 0; i < m_indicators.size(); i++)
                {
                    strAmmoMarkID.push_back(m_indicators[i].toStdString());
                }
            }
            m_vMarkAmmoModeMap[targetID] = strAmmoMarkID;
            std::vector<std::string> AllMarkAmmoID;
            for (auto data : m_taskMarkID)
            {
                const std::vector<std::string> &MarkAmmoModeVec = m_vMarkAmmoModeMap[data.GetMarkID()];
                for (auto temp : MarkAmmoModeVec)
                {
                    AllMarkAmmoID.push_back(temp);
                }
            }
            CNeubulaConntionAmmoVec m_cConntionAmmoVecTemp;
            for (auto temp : m_cConntionAmmoVec)
            {
                for (auto data : AllMarkAmmoID)
                {
                    if (data == temp.GetAmmoID())
                    {
                        temp.SetIsMount(true);
                        break;
                    }
                }
                m_cConntionAmmoVecTemp.push_back(temp);
            }
            m_vAmmoModeVec.clear();
            for (auto data : m_cConntionAmmoVecTemp)
            {
                if (!data.GetIsMount())
                {
                    m_vAmmoModeVec.push_back(data.GetAmmoID());
                }
            }
            UpdatatbwMarkAmmoMatch();
        }
    }
}

void CDlgMissionAllocation::on_btnComplete_clicked()
{
    DisConnectHidePopuped();
    DisConnectHidePopupedDJ();
    close();
}

void CDlgMissionAllocation::on_btnCancel_1_clicked()
{
    close();
}
