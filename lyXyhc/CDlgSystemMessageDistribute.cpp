#include "CDlgSystemMessageDistribute.h"
#include "QCString.h"
#include "IXyhcActivator.h"
#include "lyXyhc.h"
#include "TrackMgr.h"
#include "netheader.h"
#include "Protocol/XygsSystemControl/SystemControl.h"
#include "lyXyhcInterface.h"
#include <QMenu>

#define SEAT_TIMEOUT 20

CDlgSystemMessageDistribute::CDlgSystemMessageDistribute(QWidget *parent)
    : QWidget(parent),
      ui(new Ui::CDlgSystemMessageDistribute)
{

    ui->setupUi(this);
    Init();

    initTableTitle();

    ui->treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    CreateTableMenu();
    TabWidgetConnet();

    getSystemOnlineInfo();

    selectMarkDia = new SelectMarkDialog;
    connect(selectMarkDia->getEnsure(), &QPushButton::clicked, this, &CDlgSystemMessageDistribute::updateSystemMarks);
    connect(LyXyhc::Get().getTrackMgrPtr(), &CTrackMgr::sendMarkID, this, &CDlgSystemMessageDistribute::recvMarkID);

    ui->treeWidget->show();
}

CDlgSystemMessageDistribute::~CDlgSystemMessageDistribute()
{
}

void CDlgSystemMessageDistribute::Init()
{
    QSettings ini("../data/conf/ly.ini", QSettings::IniFormat);
    ini.beginGroup("Database");
    QString strDbName = ini.value("DbName", "LY").toString();
    QString strDbIP = ini.value("SvrIP", "192.168.1.115").toString();
    QString strDbUser = ini.value("User", "root").toString();
    QString strDbPass = ini.value("Password", "123456").toString();
    QString strDbDriver = ini.value("Driver", "QMYSQL").toString();

    g_db.init(strDbIP.toLatin1().data(), strDbUser.toLatin1().data(), strDbPass.toLatin1().data(),
              strDbName.toLatin1().data());

    if (timer == nullptr)
    {
        timer = new QTimer(this);
    }
    timer = new QTimer(this);

    timer->start(10000);
}

void CDlgSystemMessageDistribute::getDatabaseUser()
{
    if (updateIndex > 0)
    {
        updateIndex = 0;
        g_vCurrUsers.clear();
        const QDateTime &now = QDateTime::currentDateTime();
        QDbQuery qry = g_db.exec("SELECT id,role,lastTime,ip,name,passwd,lastSeat,lastID FROM user");
        while (!qry.eof())
        {
            _User user;
            qry >> user.id >> user.role >> user.lastTime >> user.ip >> user.name >> user.passwd >> user.lastSeat >> user.lastID;
            if (now.date() == user.lastTime.date())
            {
                g_vCurrUsers.push_back(user);
            }
        }
    }
    else
    {
        updateIndex++;
    }
}

void CDlgSystemMessageDistribute::TabWidgetConnet()
{
    connect(ui->treeWidget, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(customContextMenuRequested(const QPoint &)));
}

void CDlgSystemMessageDistribute::CreateTableMenu()
{
    m_TableMenu = new QMenu(this);
    m_MenuAdd = m_TableMenu->addAction(tr("Add"));
    m_MenuRemove = m_TableMenu->addAction(tr("Remove"));

    connect(m_MenuAdd, SIGNAL(triggered()), this, SLOT(on_Menu_Add()));
    connect(m_MenuRemove, SIGNAL(triggered()), this, SLOT(on_Menu_Remove()));
}

void CDlgSystemMessageDistribute::getSystemOnlineInfo()
{
    QTimer *timer = new QTimer(this);
    int m_msec = 0;

}

void CDlgSystemMessageDistribute::localWeaponNameBindToId()
{
    qnzkna::WeaponManage::WeaponSystemMap					 weaponSystemMap;
    IXyhcActivator::Get().getPWeaponManageService()->GetWeaponSystemMap(weaponSystemMap);
    for (auto sysItr : weaponSystemMap)
    {
        auto weaponComPonentMap = sysItr.second.GetWeaponComponentMapPtr();
        for (auto weapon : *weaponComPonentMap)
        {
            SystemSysNameAndID[weapon.second.getComponentName()] = weapon.second.getComponentID();
            SystemSysIDAndName[weapon.second.getComponentID()] = weapon.second.getComponentName();
        }
    }
}

void CDlgSystemMessageDistribute::initTableTitle()
{
    QStringList headTitle;
    headTitle << tr("OrderNumber") << tr("MarkSystemType") << tr("MarkSystemName") << tr("MarkStationNumber") << tr("MarkBatchNumber") << tr("EquipmentName");

    ui->treeWidget->setColumnCount(headTitle.size());
    ui->treeWidget->setHeaderLabels(headTitle);
}

void CDlgSystemMessageDistribute::addTopItem_Widget(std::string SystemName)
{
    auto systemIDAndNameItor = SystemSysIDAndName.find(SystemName);
    if (systemIDAndNameItor != SystemSysIDAndName.end())
    {
        SystemName = systemIDAndNameItor->second;
    }

    int topItemNum = ui->treeWidget->topLevelItemCount();
    QTreeWidgetItem *newTopItem = new QTreeWidgetItem();
    newTopItem->setText(0, QString::number(topItemNum + 1));
    newTopItem->setText(1, tr("IndividualSoldier"));
    newTopItem->setText(2, QString::fromStdString(SystemName));
    ui->treeWidget->addTopLevelItem(newTopItem);

    applySystemSysDistribute();

}

void CDlgSystemMessageDistribute::delTopItem_Widget(std::string SystemName)
{

    auto systemIDandNameItor = SystemSysIDAndName.find(SystemName);
    if (systemIDandNameItor != SystemSysIDAndName.end())
    {
        SystemName = systemIDandNameItor->second;
    }
    int topItemCount = ui->treeWidget->topLevelItemCount();
    bool isDel = false;
    for (int i = 0; i < topItemCount; i++)
    {
        if (ui->treeWidget->topLevelItem(i)->text(2).toStdString() == SystemName)
        {
            ui->treeWidget->takeTopLevelItem(i);
            topItemCount--;
            isDel = true;
            continue;
        }
        if (isDel)
        {
            ui->treeWidget->topLevelItem(i)->setText(2, QString::number(i));
        }
    }

}

void CDlgSystemMessageDistribute::addChildItem(QTreeWidgetItem &topItem, std::pair<int, int> childInfo)
{
    if (std::find(targets.begin(), targets.end(), childInfo) == targets.end())
    {
        return;
    }

    int count = topItem.childCount();
    QTreeWidgetItem *newChildItem = new QTreeWidgetItem;
    newChildItem->setText(0, QString::number(count + 1));

    newChildItem->setText(3, QString::number(childInfo.first));
    newChildItem->setText(4, QString::number(childInfo.second));
    topItem.addChild(newChildItem);

    std::string itemName = topItem.text(2).toStdString();
    auto systemNameAndIDItor = SystemSysNameAndID.find(itemName);
    if (systemNameAndIDItor != SystemSysNameAndID.end())
    {
        itemName = systemNameAndIDItor->second;
    }

    auto systemItor = systemNeedMarks.find(itemName);
    if (systemItor != systemNeedMarks.end())
    {
        systemItor->second.push_back(childInfo);
    }
    else
    {
        std::vector<std::pair<int, int>> newVec;
        newVec.clear();
        newVec.push_back(childInfo);
        systemNeedMarks[itemName] = newVec;
    }
}

void CDlgSystemMessageDistribute::addChildItem(QTreeWidgetItem &topItem, unsigned long long childID)
{
    if (std::find(targetIDs.begin(), targetIDs.end(), childID) == targetIDs.end())
    {
        return;
    }

    std::string childIDStr = std::to_string(childID);
    auto systemIDAndNameItor = SystemSysIDAndName.find(childIDStr);
    if (systemIDAndNameItor != SystemSysIDAndName.end())
    {
        childIDStr = systemIDAndNameItor->second;
    }

    int count = topItem.childCount();
    QTreeWidgetItem *newChildItem = new QTreeWidgetItem;
    newChildItem->setText(0, QString::number(count + 1));
    newChildItem->setText(2, QString::fromStdString(childIDStr));
    topItem.addChild(newChildItem);

    std::string itemName = topItem.text(2).toStdString();
    auto systemNameAndIDItor = SystemSysNameAndID.find(itemName);
    if (systemNameAndIDItor != SystemSysNameAndID.end())
    {
        itemName = systemNameAndIDItor->second;
    }

    auto systemIDItor = systemNeedMarkIDs.find(itemName);
    if (systemIDItor != systemNeedMarkIDs.end())
    {
        systemIDItor->second.push_back(childID);
    }
    else
    {
        std::vector<unsigned long long> newVec;
        newVec.clear();
        newVec.push_back(childID);
        systemNeedMarkIDs[itemName] = newVec;
    }
}

void CDlgSystemMessageDistribute::addChildItems(QTreeWidgetItem &topItem, std::vector<std::pair<int, int>> childInfos)
{
    for (int i = 0; i < childInfos.size(); i++)
    {
        addChildItem(topItem, childInfos[i]);
    }
    std::string itemName = topItem.text(2).toStdString();
    auto systemNameAndIDItor = SystemSysNameAndID.find(itemName);
    if (systemNameAndIDItor != SystemSysNameAndID.end())
    {
        itemName = systemNameAndIDItor->second;
    }
    auto systemItor = systemNeedMarks.find(itemName);
    if (systemItor != systemNeedMarks.end())
    {
        sendMessage_distrubute(systemItor->first, systemItor->second);
    }
}

void CDlgSystemMessageDistribute::addChildItemsID(QTreeWidgetItem &topItem, std::vector<unsigned long long> childIDS)
{
    for (int i = 0; i < childIDS.size(); i++)
    {
        addChildItem(topItem, childIDS[i]);
    }
    std::string itemName = topItem.text(2).toStdString();
    auto systemNameAndIDItor = SystemSysNameAndID.find(itemName);
    if (systemNameAndIDItor != SystemSysNameAndID.end())
    {
        itemName = systemNameAndIDItor->second;
    }
    auto systemItor = systemNeedMarkIDs.find(itemName);
    if (systemItor != systemNeedMarkIDs.end())
    {
        sendMessageID_distrubute(systemItor->first, systemItor->second);
    }
}

void CDlgSystemMessageDistribute::removeChildItem(QTreeWidgetItem &topItem, std::pair<int, int> childInfo)
{
    std::string topName = topItem.text(2).toStdString();

    auto systemNameAndIDItor = SystemSysNameAndID.find(topName);
    if (systemNameAndIDItor != SystemSysNameAndID.end())
    {
        topName = systemNameAndIDItor->second;
    }
    auto systemItor = systemNeedMarks.find(topName);
    if (systemItor != systemNeedMarks.end())
    {
        auto childInfoItor = std::find(systemItor->second.begin(), systemItor->second.end(), childInfo);
        if (childInfoItor != systemItor->second.end())
        {
            childInfoItor = systemItor->second.erase(childInfoItor);
            sendMessage_distrubute(topItem.text(2).toStdString(), systemItor->second);
        }
    }

    int childCount = topItem.childCount();
    for (int i = 0; i < childCount; i++)
    {
        if (topItem.child(i)->text(2) == "")
        {
            std::pair<int, int> tmpChild(topItem.child(i)->text(3).toInt(), topItem.child(i)->text(4).toInt());
            if (tmpChild == childInfo)
            {
                topItem.removeChild(topItem.child(i));
                break;
            }
        }
    }
}

void CDlgSystemMessageDistribute::removeChildItem(QTreeWidgetItem &topItem, unsigned long long childID)
{

    std::string topName = topItem.text(2).toStdString();
    auto systemNameAndIDItor = SystemSysNameAndID.find(topName);
    if (systemNameAndIDItor != SystemSysNameAndID.end())
    {
        topName = systemNameAndIDItor->second;
    }
    auto systemItor = systemNeedMarkIDs.find(topName);
    if (systemItor != systemNeedMarkIDs.end())
    {
        auto childInfoItor = std::find(systemItor->second.begin(), systemItor->second.end(), childID);
        if (childInfoItor != systemItor->second.end())
        {
            childInfoItor = systemItor->second.erase(childInfoItor);
            sendMessageID_distrubute(topItem.text(2).toStdString(), systemItor->second);
        }
    }

    int childCount = topItem.childCount();
    for (int i = 0; i < childCount; i++)
    {
        if (topItem.child(i)->text(2) != "")
        {
            unsigned long long tmpChild = topItem.child(i)->text(2).toULongLong();
            std::string tmpChildStr = topItem.child(i)->text(2).toStdString();
            auto systemNameAndIDItor2 = SystemSysNameAndID.find(tmpChildStr);
            if (systemNameAndIDItor2 != SystemSysNameAndID.end())
            {
                tmpChild = std::stoull(systemNameAndIDItor2->second);
            }
            if (tmpChild == childID)
            {
                topItem.removeChild(topItem.child(i));
                break;
            }
        }
    }
}

void CDlgSystemMessageDistribute::removeAllChildItems(QTreeWidgetItem &topItem)
{
    std::string topName = topItem.text(2).toStdString();

    auto systemNameAndIDItor = SystemSysNameAndID.find(topName);
    if (systemNameAndIDItor != SystemSysNameAndID.end())
    {
        topName = systemNameAndIDItor->second;
    }

    auto systemItor = systemNeedMarks.find(topName);
    if (systemItor != systemNeedMarks.end())
    {
        systemItor->second.clear();
    }

    auto systemIDItor = systemNeedMarkIDs.find(topName);
    if (systemIDItor != systemNeedMarkIDs.end())
    {
        systemIDItor->second.clear();
    }

    int count = topItem.childCount();
    for (int i = count - 1; i >= 0; i--)
    {
        topItem.removeChild(topItem.child(i));
    }

}

void CDlgSystemMessageDistribute::sendMessage_distrubute(std::string systemName, std::vector<std::pair<int, int>> targets)
{

    std::string MarkID = systemName;
    auto sysNameItor = SystemSysNameAndID.find(systemName);
    if (sysNameItor != SystemSysNameAndID.end())
    {
        MarkID = sysNameItor->second;
    }

    char sendBuf[sizeof(NetHeader) + sizeof(SystemRouteDistribute)] = { 0 };
    NetHeader *pHeader = (NetHeader *)sendBuf;
    SystemRouteDistribute *SRD = (SystemRouteDistribute *)(sendBuf + sizeof(NetHeader));
    pHeader->ucType = NETHEADER_UCTYPE_INFOMATION_DISTRUBUTE;

    pHeader->ucSubType = 0x01;
    pHeader->unLength = sizeof(NetHeader) + sizeof(SystemRouteDistribute);

    pHeader->unSeatType = 2;
    pHeader->unNodeType = 3;

    int tarsCount = targets.size() / 32;
    if (targets.size() % 32 > 0)
    {
        tarsCount += 1;
    }
    if (tarsCount == 0)
    {
        tarsCount = 1;
    }

    SRD->unMainType = XYGSSYSTEMCONTROL_SYSTEMCONTROL_UNMAINTYPE_SYSTEMROUTEDISTRIBUTE;
    std::string loginName = IXyhcActivator::Get().getPTaskManageService()->getLoginName();
    memcpy(SRD->Operator, loginName.data(), loginName.size());
    SRD->SysObjectType = 3;
    SRD->SysObjectID = std::stol(MarkID);
    SRD->unCmdNum = tarsCount;

    for (int i = 0; i < tarsCount; i++)
    {
        SRD->unCmdOrder = i + 1;
        SRD->unNum = targets.size();
        for (int index = i * 32; index < (i + 1) * 32 && index < targets.size(); index++)
        {
            SRD->unStation[index % 32] = targets[index].first;
            SRD->unBatch[index % 32] = targets[index].second;
        }

        LyXyhcInterface::Get().SendData(sendBuf, sizeof(sendBuf));
    }

}

void CDlgSystemMessageDistribute::sendMessageID_distrubute(std::string systemName, std::vector<unsigned long long> targets)
{

    std::string MarkID = systemName;
    auto sysNameItor = SystemSysNameAndID.find(systemName);
    if (sysNameItor != SystemSysNameAndID.end())
    {
        MarkID = sysNameItor->second;
    }

    char sendBuf[sizeof(NetHeader) + sizeof(SystemRouteIDDistribute)] = { 0 };
    NetHeader *pHeader = (NetHeader *)sendBuf;
    SystemRouteIDDistribute *SRD = (SystemRouteIDDistribute *)(sendBuf + sizeof(NetHeader));
    pHeader->ucType = NETHEADER_UCTYPE_INFOMATION_DISTRUBUTE;
    pHeader->ucSubType = 0x01;
    pHeader->unLength = sizeof(NetHeader) + sizeof(SystemRouteIDDistribute);

    pHeader->unSeatType = 2;
    pHeader->unNodeType = 3;

    int tarsCount = targets.size() / 32;
    if (targets.size() % 32 > 0)
    {
        tarsCount += 1;
    }
    if (tarsCount == 0)
    {
        tarsCount = 1;
    }

    SRD->unMainType = XYGSSYSTEMCONTROL_SYSTEMCONTROL_UNMAINTYPE_SYSTEMROUTEDISTRIBUTEID;
    std::string loginName = IXyhcActivator::Get().getPTaskManageService()->getLoginName();
    memcpy(SRD->Operator, loginName.data(), loginName.size());
    SRD->SysObjectType = 3;
    SRD->SysObjectID = std::stol(MarkID);
    SRD->unCmdNum = tarsCount;

    for (int i = 0; i < tarsCount; i++)
    {
        SRD->unCmdOrder = i + 1;
        SRD->unNum = targets.size();
        for (int index = i * 32; index < (i + 1) * 32 && index < targets.size(); index++)
        {
            SRD->unMarkID[index % 32] = targets[index];
        }
        LyXyhcInterface::Get().SendData(sendBuf, sizeof(sendBuf));

    }
}

void CDlgSystemMessageDistribute::getNetMessage_distrubute(int nLength, char *lpData)
{
    int len = sizeof(NetHeader) + sizeof(SystemRouteDistribute);
    len = sizeof(NetHeader) + sizeof(SystemRouteIDDistribute);
    if (nLength < sizeof(NetHeader) + sizeof(SystemRouteDistribute) || nLength < sizeof(NetHeader) + sizeof(SystemRouteIDDistribute))
    {
        return;
    }
    unsigned short *unMainType = (unsigned short *)(lpData + sizeof(NetHeader));
    if (*unMainType == XYGSSYSTEMCONTROL_SYSTEMCONTROL_UNMAINTYPE_SYSTEMROUTEDISTRIBUTE)
    {
        SystemRouteDistribute *srd = (SystemRouteDistribute *)(lpData + sizeof(NetHeader));
        int mesCount = srd->unCmdNum;
        int mesIndex = srd->unCmdOrder;
        int cmdCount = srd->unNum;

        auto systemOnlinesItor = systemOnlines.find(std::to_string(srd->SysObjectID));
        if (systemOnlinesItor == systemOnlines.end())
        {
            return;
        }

        std::vector<std::pair<int, int>> pairMess;
        pairMess.clear();
        if (mesIndex < mesCount)
        {
            for (int i = 0; i < 32; i++)
            {
                std::pair<int, int> tmpPair(srd->unStation[i], srd->unBatch[i]);
                pairMess.push_back(tmpPair);
            }
        }
        else
        {
            for (int i = 0; i < cmdCount % 32; i++)
            {
                std::pair<int, int> tmpPair(srd->unStation[i], srd->unBatch[i]);
                pairMess.push_back(tmpPair);
            }
        }

        auto topItemName = std::to_string(srd->SysObjectID);
        auto SystemSysIDAndNameItor = SystemSysIDAndName.find(std::to_string(srd->SysObjectID));
        if (SystemSysIDAndNameItor != SystemSysIDAndName.end())
        {
            topItemName = SystemSysIDAndNameItor->second;
        }
        int topItemCount = ui->treeWidget->topLevelItemCount();
        QTreeWidgetItem *topItem = NULL;
        for (int topItemIndex = 0; topItemIndex < topItemCount; topItemIndex++)
        {
            if (ui->treeWidget->topLevelItem(topItemIndex)->text(2) == QString::fromStdString(topItemName))
            {
                topItem = ui->treeWidget->topLevelItem(topItemIndex);
                break;
            }
        }

        auto systemNeedMarksItor = systemNeedMarks.find(std::to_string(srd->SysObjectID));
        if (systemNeedMarksItor != systemNeedMarks.end())
        {
            if (pairMess.size() == 0)
            {

                systemNeedMarksItor->second.clear();
                if (topItem)
                {
                    int count = topItem->childCount();
                    for (int i = count - 1; i >= 0; i--)
                    {
                        if (topItem->child(i)->text(2) == "")
                        {
                            topItem->removeChild(topItem->child(i));
                        }
                    }
                }
            }
            if (mesIndex == 1)
            {

                systemNeedMarksItor->second.clear();

                if (topItem)
                {
                    int count = topItem->childCount();
                    for (int i = count - 1; i >= 0; i--)
                    {
                        if (topItem->child(i)->text(2) == "")
                        {
                            topItem->removeChild(topItem->child(i));
                        }
                    }
                }

                for (int i = 0; i < pairMess.size(); i++)
                {
                    addChildItem(*topItem, pairMess[i]);
                }
            }
            else
            {

                for (int i = 0; i < pairMess.size(); i++)
                {
                    addChildItem(*topItem, pairMess[i]);
                }

            }
        }
        else if (systemNeedMarksItor == systemNeedMarks.end() && pairMess.size() > 0)
        {

            for (int i = 0; i < pairMess.size(); i++)
            {
                addChildItem(*topItem, pairMess[i]);
            }

        }
    }
    else if (*unMainType == XYGSSYSTEMCONTROL_SYSTEMCONTROL_UNMAINTYPE_SYSTEMROUTEDISTRIBUTEID)
    {
        SystemRouteIDDistribute *srd = (SystemRouteIDDistribute *)(lpData + sizeof(NetHeader));

        int mesCount = srd->unCmdNum;
        int mesIndex = srd->unCmdOrder;
        int cmdCount = srd->unNum;

        auto systemOnlinesItor = systemOnlines.find(std::to_string(srd->SysObjectID));
        if (systemOnlinesItor == systemOnlines.end())
        {
            return;
        }

        std::vector<unsigned long long> targetIDs;
        targetIDs.clear();
        if (mesIndex < mesCount)
        {
            for (int i = 0; i < 32; i++)
            {
                targetIDs.push_back(srd->unMarkID[i]);
            }
        }
        else
        {
            for (int i = 0; i < cmdCount % 32; i++)
            {
                targetIDs.push_back(srd->unMarkID[i]);
            }
        }

        auto topItemName = std::to_string(srd->SysObjectID);
        auto SystemSysIDAndNameItor = SystemSysIDAndName.find(std::to_string(srd->SysObjectID));
        if (SystemSysIDAndNameItor != SystemSysIDAndName.end())
        {
            topItemName = SystemSysIDAndNameItor->second;
        }
        int topItemCount = ui->treeWidget->topLevelItemCount();
        QTreeWidgetItem *topItem = NULL;
        for (int topItemIndex = 0; topItemIndex < topItemCount; topItemIndex++)
        {
            if (ui->treeWidget->topLevelItem(topItemIndex)->text(2) == QString::fromStdString(topItemName))
            {
                topItem = ui->treeWidget->topLevelItem(topItemIndex);
                break;
            }
        }

        auto systemNeedMarkIDsItor = systemNeedMarkIDs.find(std::to_string(srd->SysObjectID));
        if (systemNeedMarkIDsItor != systemNeedMarkIDs.end())
        {
            if (targetIDs.size() == 0)
            {

                systemNeedMarkIDsItor->second.clear();
                if (topItem)
                {
                    int count = topItem->childCount();
                    for (int i = count - 1; i >= 0; i--)
                    {
                        if (topItem->child(i)->text(2) != "")
                        {
                            topItem->removeChild(topItem->child(i));
                        }
                    }
                }
            }
            if (mesIndex == 1)
            {

                systemNeedMarkIDsItor->second.clear();

                if (topItem)
                {
                    int count = topItem->childCount();
                    for (int i = count - 1; i >= 0; i--)
                    {
                        if (topItem->child(i)->text(2) != "")
                        {
                            topItem->removeChild(topItem->child(i));
                        }
                    }
                }

                for (int i = 0; i < targetIDs.size(); i++)
                {
                    addChildItem(*topItem, targetIDs[i]);
                }
            }
            else
            {

                for (int i = 0; i < targetIDs.size(); i++)
                {
                    addChildItem(*topItem, targetIDs[i]);
                }
            }
        }
        else if (systemNeedMarkIDsItor == systemNeedMarkIDs.end() && targetIDs.size() > 0)
        {

            for (int i = 0; i < targetIDs.size(); i++)
            {
                addChildItem(*topItem, targetIDs[i]);
            }
        }
    }
}

void CDlgSystemMessageDistribute::applySystemSysDistribute()
{
    char sendBuf[sizeof(NetHeader) + sizeof(ApplySystemRouteDistribute)] = { 0 };
    NetHeader *pHeader = (NetHeader *)sendBuf;
    ApplySystemRouteDistribute *SRD = (ApplySystemRouteDistribute *)(sendBuf + sizeof(NetHeader));
    pHeader->ucType = NETHEADER_UCTYPE_INFOMATION_DISTRUBUTE;
    pHeader->ucSubType = 0x01;
    pHeader->unLength = sizeof(NetHeader) + sizeof(ApplySystemRouteDistribute);

    pHeader->unSeatType = 2;
    pHeader->unNodeType = 3;

    SRD->unMainType = 0x22;

    LyXyhcInterface::Get().SendData(sendBuf, sizeof(sendBuf));

}

std::map<std::string, std::string> CDlgSystemMessageDistribute::getSystemSysNameAndID()
{
    return SystemSysNameAndID;
}

std::map<std::string, std::string> CDlgSystemMessageDistribute::getSystemSysIDAndName()
{
    return SystemSysIDAndName;
}

void CDlgSystemMessageDistribute::customContextMenuRequested(const QPoint &pos)
{
    m_TableMenu->exec(QCursor::pos());
}

void CDlgSystemMessageDistribute::on_Menu_Add()
{
    showSelectMarkDia();
}

void CDlgSystemMessageDistribute::on_Menu_Remove()
{
    QTreeWidgetItem *item = ui->treeWidget->currentItem();
    if (item == NULL)
    {
        return;
    }
    QTreeWidgetItem *topItem = item->parent();
    if (topItem == NULL)
    {
        return;
    }

    std::string topName = topItem->text(2).toStdString();
    auto systemNameAndIDItor = SystemSysNameAndID.find(topName);
    if (systemNameAndIDItor != SystemSysNameAndID.end())
    {
        topName = systemNameAndIDItor->second;
    }

    if (item->text(2) != "")
    {
        unsigned long long child = item->text(2).toULongLong();
        std::string childStr = item->text(2).toStdString();
        auto systemNameAndIdItor2 = SystemSysNameAndID.find(childStr);
        if (systemNameAndIdItor2 != SystemSysNameAndID.end())
        {
            child = std::stoull(systemNameAndIdItor2->second);
        }
        removeChildItem(*topItem, child);
    }
    else
    {
        std::pair<int, int> child(item->text(3).toInt(), item->text(4).toInt());
        removeChildItem(*topItem, child);
    }
}

void CDlgSystemMessageDistribute::updateSystemMarks()
{
    QTreeWidgetItem *item = ui->treeWidget->currentItem();
    if (item == NULL)
    {
        return;
    }
    QTreeWidgetItem *topItem = item->parent();
    if (topItem == NULL)
    {
        topItem = item;
    }

    removeAllChildItems(*topItem);
    addChildItems(*topItem, selectMarkDia->getMarks());
    addChildItemsID(*topItem, selectMarkDia->getMarkIDs());
}

void CDlgSystemMessageDistribute::deleteTopItem(std::string systemName)
{
    delTopItem_Widget(systemName);
    auto systemOnlineItor = systemOnlines.find(systemName);
    if (systemOnlineItor != systemOnlines.end())
    {
        delete systemOnlineItor->second;
        systemOnlineItor = systemOnlines.erase(systemOnlineItor);
    }

    unsigned long long targetID = std::stoll(systemName);
    auto targetIDSItor = std::find(targetIDs.begin(), targetIDs.end(), targetID);
    if (targetIDSItor != targetIDs.end())
    {
        targetIDSItor = targetIDs.erase(targetIDSItor);
    }

    auto systemNeedMarkItor = systemNeedMarks.find(systemName);
    if (systemNeedMarkItor != systemNeedMarks.end())
    {
        systemNeedMarkItor->second.clear();

        sendMessage_distrubute(systemNeedMarkItor->first, systemNeedMarkItor->second);
        systemNeedMarkItor = systemNeedMarks.erase(systemNeedMarkItor);
    }

    auto systemNeedMarkIDItor = systemNeedMarkIDs.find(systemName);
    if (systemNeedMarkIDItor != systemNeedMarkIDs.end())
    {
        systemNeedMarkIDItor->second.clear();

        sendMessageID_distrubute(systemNeedMarkIDItor->first, systemNeedMarkIDItor->second);
        systemNeedMarkIDItor = systemNeedMarkIDs.erase(systemNeedMarkIDItor);
    }
}

void CDlgSystemMessageDistribute::recvMarkID(int targetID)
{
    const QString &strZ = QString::asprintf("%d", (targetID >> 8) & 0xFF);
    const QString &strP = QString::asprintf("%d", targetID & 0xFF);

    std::pair<int, int> tmpMarkID(strZ.toInt(), strP.toInt());
    auto targetItor = std::find(targets.begin(), targets.end(), tmpMarkID);
    if (targetItor == targets.end())
    {
        targets.push_back(tmpMarkID);
    }
}

void CDlgSystemMessageDistribute::showSelectMarkDia()
{
    QTreeWidgetItem *item = ui->treeWidget->currentItem();
    if (item == NULL)
    {
        return;
    }
    QTreeWidgetItem *topItem = item->parent();
    if (topItem == NULL)
    {
        topItem = item;
    }

    std::string topName = topItem->text(2).toStdString();
    auto systemNameAndIDItor = SystemSysNameAndID.find(topName);
    if (systemNameAndIDItor != SystemSysNameAndID.end())
    {
        topName = systemNameAndIDItor->second;
    }
    std::vector<std::pair<int, int>> selectedMarks;
    std::vector<unsigned long long> selectedMarkIDs;
    selectedMarks.clear();
    selectedMarkIDs.clear();

    auto systemItor = systemNeedMarks.find(topName);
    if (systemItor != systemNeedMarks.end())
    {
        selectedMarks = systemItor->second;
    }
    auto systemIDItor = systemNeedMarkIDs.find(topName);
    if (systemIDItor != systemNeedMarkIDs.end())
    {
        selectedMarkIDs = systemIDItor->second;
    }

    if (selectMarkDia == NULL)
    {
        selectMarkDia = new SelectMarkDialog;
    }
    selectMarkDia->initTableWidget(selectedMarks, selectedMarkIDs, targets,  targetIDs);
    selectMarkDia->setParent(this);
    selectMarkDia->setWindowFlags(selectMarkDia->windowFlags() | Qt::Dialog);
    selectMarkDia->show();

}
