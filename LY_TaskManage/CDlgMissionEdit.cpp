#include "CDlgMissionEdit.h"
#include "ui_CDlgMissionEdit.h"
#include "QCString.h"
#include "LyMessageBox.h"
#include "ITaskManagerActivator.h"
#include "lY_TaskManage.h"
#include "QRegExpExample.h"
#include <QDateTime>

CDlgMissionEdit::CDlgMissionEdit(QWidget *parent) :
    LyDialog(tr("Task Editing"), parent),
    ui(new Ui::CDlgMissionEdit)
{
    ui->setupUi(centralWidget());
    this->setResizable(true);
    this->resize(770, 1060);

    m_pITaskManageService = ITaskManagerActivator::Get().getPTaskManageService();
    m_pIMarkManageService = ITaskManagerActivator::Get().getPIMarkManageService();
    m_pIConfigProjectControlService = ITaskManagerActivator::Get().getPIConfigProjectControlService();
    m_pIWeaponManageService = ITaskManagerActivator::Get().getPWeaponManageService();
    m_pICommandUnitManageService = ITaskManagerActivator::Get().getPICommandUnitManageService();

    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(on_pushButton_clicked()));
    connect(ui->pushButton_2, SIGNAL(clicked()), this, SLOT(on_pushButton_2_clicked()));
    connect(ui->pushButton_3, SIGNAL(clicked()), this, SLOT(on_pushButton_3_clicked()));
    connect(ui->pushButton_4, SIGNAL(clicked()), this, SLOT(on_pushButton_4_clicked()));
    connect(ui->pushButton_5, SIGNAL(clicked()), this, SLOT(on_pushButton_5_clicked()));
    connect(ui->pushButton_6, &QPushButton::clicked, this, &CDlgMissionEdit::on_pushButton_6_clicked);
    connect(ui->pushButton_7, SIGNAL(clicked()), this, SLOT(on_pushButton_7_clicked()));
    connect(ui->pushButton_8, SIGNAL(clicked()), this, SLOT(on_pushButton_8_clicked()));
    connect(ui->pushButton_9, SIGNAL(clicked()), this, SLOT(on_pushButton_9_clicked()));
    connect(ui->comboBox_2, SIGNAL(currentIndexChanged(const int)), this, SLOT(on_comboBox_2_currentIndexChanged(const int)));

    connect(ui->tableWidget_2, SIGNAL(itemDoubleClicked(QTableWidgetItem *)), this, SLOT(on_tableWidget_2_itemDoubleClicked(QTableWidgetItem *)));
    connect(ui->tableWidget, SIGNAL(itemDoubleClicked(QTableWidgetItem *)), this, SLOT(on_tableWidget_itemDoubleClicked(QTableWidgetItem *)));
    connect(ui->tableWidget_4, SIGNAL(itemDoubleClicked(QTableWidgetItem *)), this, SLOT(on_tableWidget_4_itemDoubleClicked(QTableWidgetItem *)));
    connect(ui->tableWidget_3, SIGNAL(itemDoubleClicked(QTableWidgetItem *)), this, SLOT(on_tableWidget_3_itemDoubleClicked(QTableWidgetItem *)));
    connect(ui->tbwAlternateAmmo, SIGNAL(itemDoubleClicked(QTableWidgetItem *)), this, SLOT(on_tbwAlternateAmmo_itemDoubleClicked(QTableWidgetItem *)));
    connect(ui->tbwSelectedAmmo, SIGNAL(itemDoubleClicked(QTableWidgetItem *)), this, SLOT(on_tbwSelectedAmmo_itemDoubleClicked(QTableWidgetItem *)));
    connect(ui->tableWidget, &QTableWidget::customContextMenuRequested, this, &CDlgMissionEdit::on_treeWidget_customContextMenuRequested);
    connect(this, &CDlgMissionEdit::MsgOnlyMark, this, &CDlgMissionEdit::sendOnlySlots);
    connect(this, &CDlgMissionEdit::DlgMissionEditShow, this, &CDlgMissionEdit::SetMStMission, Qt::QueuedConnection);
    connect(ui->dateTimeEdit, &QDateTimeEdit::dateTimeChanged, this, &CDlgMissionEdit::on_Start_DataTimeChanged);
    connect(ui->dateTimeEdit_3, &QDateTimeEdit::dateTimeChanged, this, &CDlgMissionEdit::on_End_DataTimeChanged);

    InitData();
    InitNodeInfoVecNew();
    CreateTableHead();
    CreatetbwSelectedAmmoMenu();
    AddQRegExp();
}

CDlgMissionEdit::~CDlgMissionEdit()
{
    delete ui;
}

void CDlgMissionEdit::InitData()
{
    QStringList list01, list02;
    list01 << tr("Unassigned") << tr("Strike") << tr("Reconnoitre") << tr("Protection") << tr("Surveillance") << tr("Rescue") << tr("Transport") << tr("Single Pawn");
    list02 << tr("-") << tr("Destroy") << tr("Damage") << tr("Suppression") << tr("Interference") << tr("Turn off") << tr("Surveillance") << tr("Block") << tr("Search and Discovery") << tr("Reconnaissance Mark Elements") << tr("Protection");

    ui->comboBox_2->addItems(list01);

    ui->textEdit->setText(tr("-"));

    ui->lineEdit->setText(tr(""));
    ui->lineEdit_2->setText(tr(""));
    ui->lineEdit_3->setText(tr(""));

    ui->lineEdit_15->setText(tr(""));
    ui->lineEdit_19->setText(tr(""));

    QValidator *edit_idator = new QRegExpValidator(regLineEdit1);
    ui->lineEdit_2->setValidator(new QRegExpValidator(edit_idator));
    ui->lineEdit_3->setValidator(new QRegExpValidator(edit_idator));
    ui->lineEdit_15->setValidator(new QRegExpValidator(edit_idator));
    ui->lineEdit_19->setValidator(new QRegExpValidator(edit_idator));

    ui->lineEdit_10->setValidator(new QRegExpValidator(regAltitude));
    ui->lineEdit_9->setValidator(new QRegExpValidator(regLatitude));
    ui->lineEdit_7->setValidator(new QRegExpValidator(regLongitude));

    ui->dateTimeEdit->setDateTime(QDateTime::currentDateTime());
    ui->dateTimeEdit_2->setDateTime(QDateTime::currentDateTime());
    ui->dateTimeEdit_3->setDateTime(QDateTime::currentDateTime());
    ui->dateTimeEdit_4->setDateTime(QDateTime::currentDateTime());
    ui->dateTimeEdit_5->setDateTime(QDateTime::currentDateTime());

    m_DataType = 0;

    if (m_pITaskManageService == nullptr)
    {
        return;
    }

}

void CDlgMissionEdit::InitWindowCtrl(int nTabIndex)
{
    ui->textEdit->setText(tr("-"));

    ui->lineEdit->setText(tr(""));
    ui->lineEdit_2->setText(tr(""));
    ui->lineEdit_3->setText(tr(""));

    ui->lineEdit_15->setText(tr(""));
    ui->lineEdit_19->setText(tr(""));

    ui->dateTimeEdit->setDateTime(QDateTime::currentDateTime());
    ui->dateTimeEdit_2->setDateTime(QDateTime::currentDateTime());
    ui->dateTimeEdit_3->setDateTime(QDateTime::currentDateTime());
    QDateTime tempMin = ui->dateTimeEdit->dateTime();
    QDateTime tempMax = ui->dateTimeEdit_3->dateTime();
    ui->dateTimeEdit_2->setMinimumDateTime(tempMin);
    ui->dateTimeEdit_2->setMaximumDateTime(tempMax);
    ui->dateTimeEdit_3->setMinimumDateTime(tempMin);

    ui->comboBox_2->setCurrentIndex(nTabIndex);

    ui->lineEdit_7->setText(tr("0"));
    ui->lineEdit_9->setText(tr("0"));
    ui->lineEdit_10->setText(tr("0"));

    m_stMission.ClearMarkList();
    m_stMission.ClearPlatResourcesList();
    m_CRelationMissionAmmoVec.clear();

    InitTimeEdit();
}

void CDlgMissionEdit::SetWindowInfo(CNeubulaMission &stInfo)
{
    m_stMission = stInfo;
    if (m_pITaskManageService)
    {
        m_CRelationMissionAmmoVec = m_pITaskManageService->GetMissionIDByMissionAmmo(m_stMission.GetID());
    }

    ui->lineEdit->setText(QString::fromStdString(stInfo.GetID()));
    ui->lineEdit_2->setText(QString::fromStdString(stInfo.GetName()));
    ui->lineEdit_3->setText(QString::fromStdString(stInfo.GetBelong()));
    ui->comboBox_2->setCurrentIndex(stInfo.GetTypeENum(stInfo.GetType()));

    ui->lineEdit_15->setText(QString::fromStdString(stInfo.GetrDetectDemand()));
    ui->lineEdit_19->setText(QString::fromStdString(stInfo.GetValidTime()));

    QString strTime;
    strTime = QString::fromStdString(stInfo.GetStartTime().c_str());
    QDateTime dataTime = QDateTime::fromString(strTime, "yyyy/M/d HH:mm");
    ui->dateTimeEdit->setDateTime(dataTime);

    strTime = QString::fromStdString(stInfo.GetAttackTime().c_str());
    dataTime = QDateTime::fromString(strTime, "yyyy/M/d HH:mm");
    ui->dateTimeEdit_2->setDateTime(dataTime);

    strTime = QString::fromStdString(stInfo.GetFinishTime().c_str());
    dataTime = QDateTime::fromString(strTime, "yyyy/M/d HH:mm");
    ui->dateTimeEdit_3->setDateTime(dataTime);

    ui->textEdit->setText(QString::fromStdString(stInfo.GetRemark()));
    ui->lineEdit_7->setText(QString::number(stInfo.GetStartLon(), 'f', 7));
    ui->lineEdit_9->setText(QString::number(stInfo.GetStartLat(), 'f', 7));
    ui->lineEdit_10->setText(QString::number(stInfo.GetStartAlt(), 'f', 2));
    m_CRelationMissionAmmoVec = stInfo.GetMissionAmmoVec();

    if (stInfo.GetTypeENum(stInfo.GetType()) == 7)
    {
        ui->comboBox->setCurrentIndex(stInfo.GetDBMissionType());
        ui->lineEdit_4->setText(QString::fromStdString(stInfo.GetrDetectDemand()));
        ui->lineEdit_5->setText(QString::fromStdString(stInfo.GetDBSource()));

        QString strTime;
        strTime = QString::fromStdString(stInfo.GetStartTime().c_str());
        QDateTime dataTime = QDateTime::fromString(strTime, "yyyy/M/d HH:mm");
        ui->dateTimeEdit_4->setDateTime(dataTime);
        strTime = QString::fromStdString(stInfo.GetFinishTime().c_str());
        dataTime = QDateTime::fromString(strTime, "yyyy/M/d HH:mm");
        ui->dateTimeEdit_5->setDateTime(dataTime);
    }
}

void CDlgMissionEdit::SetMStMission()
{

    auto pMsg03 = ITaskManagerActivator::Get().TaskDataEdit_pMsg03;
    CNeubulaMission tempMission;
    tempMission.SetID(pMsg03.TaskID);
    int type = pMsg03.taskType;
    tempMission.SetType((ENeubulaMissionType)type);
    tempMission.SetDetectDemand(pMsg03.cRequire);
    tempMission.SetStartTime(QDateTime::fromSecsSinceEpoch(pMsg03.StartTime).toString("yyyy/MM/dd HH:mm").toStdString());
    tempMission.SetFinishTime(QDateTime::fromSecsSinceEpoch(pMsg03.FinishTime).toString("yyyy/MM/dd HH:mm").toStdString());
    tempMission.SetRemark(pMsg03.cMark);

    MarkIDVec targetVec;
    for (int i = 0; i < 5; i++)
    {
        if (tr2(pMsg03.stMarkInfo[i].cMarkID) != "")
        {
            targetVec.push_back(pMsg03.stMarkInfo[i].cMarkID);
        }
    }
    tempMission.SetMarkIDList(targetVec);

    SetWindowInfo(tempMission);
    UpdateMarkSelect();

    this->show();
}

void CDlgMissionEdit::ShowTop()
{

}

void CDlgMissionEdit::SetDataType(int nType)
{
    m_DataType = nType;
}

void CDlgMissionEdit::InitTimeEdit()
{

    int nID = m_pITaskManageService->GetActivatingActionID();
    CNeubulaAction  stAction = m_pITaskManageService->GetNebulaAction(nID);

    QString strTime;
    strTime = QString::fromStdString(stAction.GetStartTime().c_str());
    QDateTime dataTime = QDateTime::fromString(strTime, "yyyy/M/d HH:mm");
    ui->dateTimeEdit->setDateTime(dataTime);

    strTime = QString::fromStdString(stAction.GetFinishTime().c_str());
    dataTime = QDateTime::fromString(strTime, "yyyy/M/d HH:mm");
    ui->dateTimeEdit_3->setDateTime(dataTime);

}

void CDlgMissionEdit::InitNodeInfoVec()
{
    m_vecNodeInfo.clear();
    std::string strWeaponSystemId = "11000";
    qnzkna::WeaponManage::WeaponSystem weaponSystem;
    bool bGetWeaponSys = ITaskManagerActivator::Get().getPWeaponManageService()->GetWeaponSystem(&weaponSystem, strWeaponSystemId);
    if (!bGetWeaponSys)
    {
        return;
    }
    qnzkna::WeaponManage::WeaponComponentMap *pWeaponCompoentMap = weaponSystem.GetWeaponComponentMapPtr();
    if (!pWeaponCompoentMap)
    {
        return;
    }
    qnzkna::WeaponManage::WeaponComponentMapItr itrMap = pWeaponCompoentMap->begin();
    int nCurIndex = 0;
    for (; itrMap != pWeaponCompoentMap->end(); itrMap++)
    {
        qnzkna::WeaponManage::WeaponComponent_Type weaponComponentType = itrMap->second.getPWeaponModel()->getComponentType();
        qnzkna::WeaponManage::WeaponComponent_Model weaponCompontModel = itrMap->second.getPWeaponModel()->getComponentModel();
        if (qnzkna::WeaponManage::WeaponComponent_Type::WeaponComponent_Type_RotorUAV == weaponComponentType)
        {
            std::string strUAVId = itrMap->second.getComponentID();
            std::string strComponentName = itrMap->second.getComponentName();
            std::string strProtocolType = itrMap->second.getProtocolType();
            qnzkna::NodeManage::CNodeRelationInfo *pNodeInfo = new qnzkna::NodeManage::CNodeRelationInfo(NODE_TYPE_WEAPONCOMPONENT, strUAVId, strComponentName, strProtocolType);
            if (pNodeInfo)
            {
                m_vecNodeInfo.push_back(pNodeInfo);
                nCurIndex++;
            }
        }
    }
}

void CDlgMissionEdit::InitNodeInfoVecNew()
{
    if (!m_pIConfigProjectControlService || !m_pIWeaponManageService || !m_pICommandUnitManageService)
    {
        return;
    }
    m_vecNodeInfo.clear();

    const qnzkna::SystemConfig::SystemSeatInfo &systemSeatInfo = m_pIConfigProjectControlService->systemSeatInfo();
    qnzkna::NodeManage::NODE_TYPE_UINT nNodeType = systemSeatInfo.SystemNodeType();
    std::string strNodeId = systemSeatInfo.SystemNodeId();

    if ("" == strNodeId)
    {
        return ;
    }

    StringSet weaponSysList;
    bool bGetWeaponSys = m_pICommandUnitManageService->GetCommandUnitWeaponList(strNodeId, &weaponSysList);
    if (!bGetWeaponSys)
    {
        return;
    }

    for (const auto &itrWeaponSys : weaponSysList)
    {
        std::string strWeaponSysId = itrWeaponSys;
        qnzkna::WeaponManage::WeaponSystem weaponSys;
        bool bGetSys = m_pIWeaponManageService->GetWeaponSystem(&weaponSys, strWeaponSysId);
        if (bGetSys)
        {
            if (weaponSys.getSystemType() == qnzkna::WeaponManage::WeaponSystem_Type_UAV)
            {
                for (const auto &itr : weaponSys.getMapWeaponComponent())
                {
                    qnzkna::WeaponManage::WeaponComponent_Type eComponentType = itr.second.getPWeaponModel()->getComponentType();
                    if (qnzkna::WeaponManage::WeaponComponent_Type_RotorUAV == eComponentType ||
                            qnzkna::WeaponManage::WeaponComponent_Type_FixedWingUAV == eComponentType ||
                            qnzkna::WeaponManage::WeaponComponent_Type_FlappingWingUAV == eComponentType ||
                            qnzkna::WeaponManage::WeaponComponent_Type_PrarafoilUAV == eComponentType ||
                            qnzkna::WeaponManage::WeaponComponent_Type_SpinWingUAV == eComponentType ||
                            qnzkna::WeaponManage::WeaponComponent_Type_HelicopterUAV == eComponentType)
                    {
                        std::string strUAVId = itr.second.getComponentID();
                        std::string strComponentName = itr.second.getComponentName();
                        std::string strProtocolType = itr.second.getProtocolType();
                        qnzkna::NodeManage::CNodeRelationInfo *pNodeInfo = new qnzkna::NodeManage::CNodeRelationInfo(NODE_TYPE_WEAPONCOMPONENT, strUAVId, strComponentName, strProtocolType);
                        if (pNodeInfo)
                        {
                            m_vecNodeInfo.push_back(pNodeInfo);
                        }
                    }

                }
            }
        }
    }
}
qnzkna::NodeManage::CNodeRelationInfoPtrVector CDlgMissionEdit::GetNodeInfoVec()
{
    return m_vecNodeInfo;
}

int CDlgMissionEdit::GetPickPointStates()
{
    return m_PickPointStates;
}

double CDlgMissionEdit::GetStartLon()
{
    return ui->lineEdit_7->text().toDouble();
}

double CDlgMissionEdit::GetStartLat()
{
    return ui->lineEdit_9->text().toDouble();
}

void CDlgMissionEdit::SetPostion(double &dLon, double &dLat, double &dHeight)
{
    ui->lineEdit_7->setText(QString::number(dLon, 'f', 7));
    ui->lineEdit_9->setText(QString::number(dLat, 'f', 7));
    ui->lineEdit_10->setText(QString::number(dHeight, 'f', 7));
}

void CDlgMissionEdit::sendOnlySlots()
{
    this->SetDataType(1);
    ui->textEdit->setText(tr("-"));

    ui->lineEdit->setText(tr(""));
    ui->lineEdit_2->setText(tr(""));
    ui->lineEdit_3->setText(tr(""));
    ui->lineEdit_15->setText(tr(""));
    ui->lineEdit_19->setText(tr(""));
    ui->dateTimeEdit->setDateTime(QDateTime::currentDateTime());
    ui->dateTimeEdit_2->setDateTime(QDateTime::currentDateTime());
    ui->dateTimeEdit_3->setDateTime(QDateTime::currentDateTime());

    ui->lineEdit_7->setText(QString::number(startPoint.longitude()));
    ui->lineEdit_9->setText(QString::number(startPoint.latitude()));
    ui->lineEdit_10->setText(QString::number(startPoint.altitude()));

    InitTimeEdit();
    if (this->isVisible())
    {
        this->hide();
    }
    this->show();
    this->raise();
}

void CDlgMissionEdit::CreateTableHead()
{

    QStringList headerlabels;
    headerlabels << tr("No.") << tr("Mark ID") << tr("Mark Name");
    ui->tableWidget_2->clear();
    ui->tableWidget_2->setColumnCount(headerlabels.size());
    ui->tableWidget_2->setHorizontalHeaderLabels(headerlabels);
    ui->tableWidget_2->verticalHeader()->setHidden(true);
    ui->tableWidget_2->setSelectionBehavior(QTableWidget::SelectRows);
    ui->tableWidget_2->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget_2->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget_2->setAlternatingRowColors(true);
    ui->tableWidget_2->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->tableWidget_2->horizontalHeader()->setStretchLastSection(true);

    headerlabels.clear();
    headerlabels << tr("No.") << tr("Mark ID") << tr("Mark Name") << tr("Damage Requirement");
    ui->tableWidget->clear();
    ui->tableWidget->setColumnCount(headerlabels.size());
    ui->tableWidget->setHorizontalHeaderLabels(headerlabels);
    ui->tableWidget->verticalHeader()->setHidden(true);
    ui->tableWidget->setSelectionBehavior(QTableWidget::SelectRows);
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->setAlternatingRowColors(true);
    ui->tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);

    QStringList headerlabels2;
    headerlabels2 << tr("No.") << tr("Platform Type") << tr("Platform Number");
    ui->tableWidget_4->clear();
    ui->tableWidget_4->setColumnCount(headerlabels2.size());
    ui->tableWidget_4->setHorizontalHeaderLabels(headerlabels2);
    ui->tableWidget_4->verticalHeader()->setHidden(true);
    ui->tableWidget_4->setSelectionBehavior(QTableWidget::SelectRows);
    ui->tableWidget_4->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget_4->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget_4->setAlternatingRowColors(true);
    ui->tableWidget_4->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->tableWidget_4->horizontalHeader()->setStretchLastSection(true);

    QStringList headerList;
    headerList << tr("No.") << tr("Platform Type") << tr("Platform Number") << tr("Main Pod Tyoe") << tr("Secondary Pod Tyoe") << tr("Primary Mount Types")
               << tr("Primary Mount Number") << tr("Secondary Mount Types") << tr("Secondary Mount Number");
    ui->tableWidget_3->clear();
    ui->tableWidget_3->setColumnCount(headerList.size());
    ui->tableWidget_3->setHorizontalHeaderLabels(headerList);
    ui->tableWidget_3->verticalHeader()->setHidden(true);
    ui->tableWidget_3->setSelectionBehavior(QTableWidget::SelectRows);
    ui->tableWidget_3->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget_3->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget_3->setAlternatingRowColors(true);
    ui->tableWidget_3->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->tableWidget_3->horizontalHeader()->setStretchLastSection(true);

    headerList.clear();
    headerList << tr("No.") << tr("Ammunition Name") << tr("Ammunition Number");
    ui->tbwAlternateAmmo->clear();
    ui->tbwAlternateAmmo->setColumnCount(headerList.size());
    ui->tbwAlternateAmmo->setHorizontalHeaderLabels(headerList);
    ui->tbwAlternateAmmo->verticalHeader()->setHidden(true);
    ui->tbwAlternateAmmo->setSelectionBehavior(QTableWidget::SelectRows);
    ui->tbwAlternateAmmo->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tbwAlternateAmmo->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tbwAlternateAmmo->setAlternatingRowColors(true);
    ui->tbwAlternateAmmo->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->tbwAlternateAmmo->horizontalHeader()->setStretchLastSection(true);

    ui->tbwSelectedAmmo->clear();
    ui->tbwSelectedAmmo->setColumnCount(headerList.size());
    ui->tbwSelectedAmmo->setHorizontalHeaderLabels(headerList);
    ui->tbwSelectedAmmo->verticalHeader()->setHidden(true);
    ui->tbwSelectedAmmo->setSelectionBehavior(QTableWidget::SelectRows);
    ui->tbwSelectedAmmo->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tbwSelectedAmmo->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tbwSelectedAmmo->setAlternatingRowColors(true);
    ui->tbwSelectedAmmo->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->tbwSelectedAmmo->horizontalHeader()->setStretchLastSection(true);
}

bool CDlgMissionEdit::JudeMissionMark(const std::string strID)
{
    for (auto tarID : m_stMission.GetMarkIDList())
    {
        if (0 == strcmp(tarID.c_str(), strID.c_str()))
        {
            return true;
        }
    }
    return false;
}

bool CDlgMissionEdit::JudeMissionPlat(const std::string strName, const int nNumTotal, int &nNumLave)
{
    CRelationMissionResourceVec Rmap = m_stMission.GetPlatInfoMap();
    auto itr = Rmap.begin();
    for (; itr != Rmap.end(); itr++)
    {
        if (itr == Rmap.end())
        {
            return false;
        }
        else
        {
            nNumLave = nNumTotal - itr->GetResourceNum();
            return true;
        }
    }
    return false;
}

void CDlgMissionEdit::UpdateMarkBackup()
{
    if (m_pITaskManageService == nullptr)
    {
        return;
    }

    if (m_pIMarkManageService == nullptr)
    {
        return;
    }

    int nActivatingID = m_pITaskManageService->GetActivatingActionID();

    if (nActivatingID == -1)
    {
        return;
    }

    CNeubulaAction stAciton = m_pITaskManageService->GetNebulaAction(nActivatingID);
    MarkIDVec IDVec = stAciton.GetMarkIDList();

    ui->tableWidget_2->clearContents();
    ui->tableWidget_2->setRowCount(0);

    int nNo = 0;
    for (auto MarkID : IDVec)
    {
        if (JudeMissionMark(MarkID) == true)
        {
            continue;
        }
        QString strNo, strID, strName;
        ui->tableWidget_2->setRowCount(nNo + 1);
        strNo = QString::number(nNo + 1);
        strID = QString::fromStdString(MarkID);
        SystemMarkInfo stMark = m_pIMarkManageService->GetMark(MarkID);
        strName = QString::fromStdString(stMark.GetName());

        QTableWidgetItem *item0 = new QTableWidgetItem(strNo);
        QTableWidgetItem *item1 = new QTableWidgetItem(strID);
        QTableWidgetItem *item2 = new QTableWidgetItem(strName);

        ui->tableWidget_2->setItem(nNo, 0, item0);
        ui->tableWidget_2->setItem(nNo, 1, item1);
        ui->tableWidget_2->setItem(nNo, 2, item2);

        ui->tableWidget_2->item(nNo, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->tableWidget_2->item(nNo, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->tableWidget_2->item(nNo, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

        nNo++;
    }
}

void CDlgMissionEdit::UpdateMarkSelect()
{
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(m_stMission.GetMarkIDList().size());
    MarkIDVec IDVec = m_stMission.GetMarkIDList();
    CRelationMissionMarkVec MarkDamageTemp = m_stMission.GetMarkDamageVec();
    int nNo = 0;
    for (auto MarkID : IDVec)
    {
        QString strNo, strID, strName;
        strNo = QString::number(nNo + 1);
        strID = QString::fromStdString(MarkID);
        SystemMarkInfo stMark = m_pIMarkManageService->GetMark(MarkID);
        strName = QString::fromStdString(stMark.GetName());

        QTableWidgetItem *item0 = new QTableWidgetItem(strNo);
        QTableWidgetItem *item1 = new QTableWidgetItem(strID);
        QTableWidgetItem *item2 = new QTableWidgetItem(strName);

        ui->tableWidget->setItem(nNo, 0, item0);
        ui->tableWidget->setItem(nNo, 1, item1);
        ui->tableWidget->setItem(nNo, 2, item2);

        ui->tableWidget->item(nNo, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->tableWidget->item(nNo, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->tableWidget->item(nNo, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        QComboBox *combobox = new QComboBox;
        for (int i = 0; i < 13; i++)
        {
            QString strMarkDamage = LY_TaskManage::Get().GetDlgMissionList()->GetHurtDegreeString(m_stMission.GetHurtDegreeENum(i));
            combobox->addItem(strMarkDamage);
            combobox->setItemData(i, QVariant(i), Qt::UserRole);
        }
        bool bDamage = false;
        for (auto data : MarkDamageTemp)
        {
            if (MarkID == data.GetMarkID())
            {
                QString strMarkDamage = LY_TaskManage::Get().GetDlgMissionList()->GetHurtDegreeString(m_stMission.GetHurtDegreeENum(data.GetMarkDamage()));
                combobox->setCurrentText(strMarkDamage);
                disconnect(combobox, 0, 0, 0);
                connect(combobox, SIGNAL(currentIndexChanged(int)), this, SLOT(on_combobox_currentIndexChanged(int)));
                ui->tableWidget->setCellWidget(nNo, 3, combobox);
                bDamage = true;
            }
        }
        if (!bDamage)
        {
            combobox->setCurrentText("-");
            ui->tableWidget->setCellWidget(nNo, 3, combobox);
        }
        nNo++;
    }
}

void CDlgMissionEdit::UpdateEquBackup()
{
    if (m_pITaskManageService == nullptr)
    {
        return;
    }

    int nActivatingID = m_pITaskManageService->GetActivatingActionID();

    if (nActivatingID == -1)
    {
        return;
    }

    CRelationActionResourceVec ResourceVec = m_pITaskManageService->GetRelationPtResource(nActivatingID);
    CRelationActionResourceVec SensorResourceVec = m_pITaskManageService->GetRelationSensorResource(nActivatingID);
    CRelationActionResourceVec AmmoResourceVec = m_pITaskManageService->GetRelationAmmoResource(nActivatingID);
    ui->tableWidget_4->clearContents();
    ui->tableWidget_4->setRowCount(ResourceVec.size());

    int row = ui->tableWidget_3->rowCount();
    int nNo = 0;
    for (auto data : ResourceVec)
    {
        int num = data.GetResourceNum();
        for (int i = 0; i < row; i++)
        {
            if (data.GetResourceName() == ui->tableWidget_3->item(i, 1)->text().toStdString())
            {
                num = num - 1;
            }
        }
        if (num <= 0)
        {
            ui->tableWidget_4->setItem(nNo, 0, new QTableWidgetItem(QString::number(nNo + 1)));
            ui->tableWidget_4->setItem(nNo, 1, new QTableWidgetItem(tr(data.GetResourceName().c_str())));
            ui->tableWidget_4->setItem(nNo, 2, new QTableWidgetItem(QString::number(0)));
            ui->tableWidget_4->item(nNo, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            ui->tableWidget_4->item(nNo, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            ui->tableWidget_4->item(nNo, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        }
        else
        {
            ui->tableWidget_4->setItem(nNo, 0, new QTableWidgetItem(QString::number(nNo + 1)));
            ui->tableWidget_4->setItem(nNo, 1, new QTableWidgetItem(tr(data.GetResourceName().c_str())));
            ui->tableWidget_4->setItem(nNo, 2, new QTableWidgetItem(QString::number(num)));
            ui->tableWidget_4->item(nNo, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            ui->tableWidget_4->item(nNo, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            ui->tableWidget_4->item(nNo, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        }

        nNo++;
    }

}
void CDlgMissionEdit::UpdateEquBackup(int currentRow)
{
    QTableWidgetItem *item = ui->tableWidget_4->item(currentRow, 2);
    ui->tableWidget_4->setItem(currentRow, 2, new QTableWidgetItem(QString::number(item->text().toInt() - 1)));
    ui->tableWidget_4->item(currentRow, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
}

void CDlgMissionEdit::UpdateEquBackup1()
{
    int nSelectedRow = ui->tableWidget_3->currentRow();
    QTableWidgetItem *item = ui->tableWidget_3->item(nSelectedRow, 1);
    int tableWidget_4RowCount = ui->tableWidget_4->rowCount();
    for (int i = 0; i < tableWidget_4RowCount; i++)
    {
        QTableWidgetItem *item1 = ui->tableWidget_4->item(i, 1);
        if (item->text() == item1->text())
        {
            QTableWidgetItem *item2 = ui->tableWidget_4->item(i, 2);
            ui->tableWidget_4->setItem(i, 2, new QTableWidgetItem(QString::number(item2->text().toInt() + 1)));
            ui->tableWidget_4->item(i, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        }
    }
}

void CDlgMissionEdit::UpdateEquSelect1()
{
    int nSelectedRow = ui->tableWidget_3->currentRow();
    ui->tableWidget_3->removeRow(nSelectedRow);
    if (nSelectedRow != ui->tableWidget_3->rowCount())
    {
        for (int i = 0; i < ui->tableWidget_3->rowCount(); i++)
        {
            ui->tableWidget_3->setItem(i, 0, new QTableWidgetItem(QString::number(i + 1)));
        }
    }
}

void CDlgMissionEdit::UpdataAmmoBackup()
{
    if (m_pITaskManageService == nullptr)
    {
        return;
    }
    int nActionID = m_pITaskManageService->GetActivatingActionID();
    m_CRelationActionResourceVec = m_pITaskManageService->GetRelationAmmoResource(nActionID);
    std::vector<std::string> MissionIDVec = m_pITaskManageService->GetMissionsByAction(nActionID);
    QMap<std::string, int> mAmmoNum;
    if (!MissionIDVec.empty())
    {
        for (auto data : MissionIDVec)
        {
            if (m_DataType == 2 && data == m_stMission.GetID())
            {
                continue;
            }
            CRelationMissionAmmoVec MissionAmmoVec = m_pITaskManageService->GetMissionIDByMissionAmmo(data);
            for (auto temp : MissionAmmoVec)
            {
                auto it = mAmmoNum.find(temp.GetAmmoName());
                if (it != mAmmoNum.end())
                {
                    mAmmoNum[temp.GetAmmoName()] += temp.GetAmmoNum();
                }
                else
                {
                    mAmmoNum.insert(temp.GetAmmoName(), temp.GetAmmoNum());
                }
            }
        }
    }
    if (m_CRelationActionResourceVec.empty())
    {
        return;
    };
    ui->tbwAlternateAmmo->setRowCount(m_CRelationActionResourceVec.size());
    int nRow = 0;
    for (auto &data : m_CRelationActionResourceVec)
    {
        ui->tbwAlternateAmmo->setItem(nRow, 0, new QTableWidgetItem(QString::number(nRow + 1)));
        ui->tbwAlternateAmmo->setItem(nRow, 1, new QTableWidgetItem(QString::fromStdString(data.GetResourceName())));
        auto it = mAmmoNum.find(data.GetResourceName());
        if (it != mAmmoNum.end())
        {
            int nNum = data.GetResourceNum() - mAmmoNum[data.GetResourceName()];
            data.SetResourceNum(nNum);
            for (auto temp : m_CRelationMissionAmmoVec)
            {
                if (data.GetResourceName() == temp.GetAmmoName())
                {
                    nNum -= temp.GetAmmoNum();
                }
            }
            if (nNum < 0)
            {
                LyMessageBox::information(this, tr("tip"), tr("Low on ammo for current operation!"));
                m_bIdAmmoEmpty = true;
                nNum = 0;
            }
            ui->tbwAlternateAmmo->setItem(nRow, 2, new QTableWidgetItem(QString::number(nNum)));
        }
        else
        {
            int nNum = data.GetResourceNum();
            for (auto temp : m_CRelationMissionAmmoVec)
            {
                if (data.GetResourceName() == temp.GetAmmoName())
                {
                    nNum -= temp.GetAmmoNum();
                }
            }
            if (nNum < 0)
            {
                LyMessageBox::information(this, tr("tip"), tr("Low on ammo for current operation!"));
                m_bIdAmmoEmpty = true;
                nNum = 0;
            }
            ui->tbwAlternateAmmo->setItem(nRow, 2, new QTableWidgetItem(QString::number(nNum)));
        }
        ui->tbwAlternateAmmo->item(nRow, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->tbwAlternateAmmo->item(nRow, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->tbwAlternateAmmo->item(nRow, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ++nRow;
    }
}

void CDlgMissionEdit::UpdateAmmoSelect()
{
    ui->tbwSelectedAmmo->setRowCount(m_CRelationMissionAmmoVec.size());
    int nRow = 0;
    for (auto data : m_CRelationMissionAmmoVec)
    {
        ui->tbwSelectedAmmo->setItem(nRow, 0, new QTableWidgetItem(QString::number(nRow + 1)));
        ui->tbwSelectedAmmo->setItem(nRow, 1, new QTableWidgetItem(QString::fromStdString(data.GetAmmoName())));
        int nMaxNum;
        for (auto temp : m_CRelationActionResourceVec)
        {
            if (data.GetAmmoName() == temp.GetResourceName())
            {
                nMaxNum = temp.GetResourceNum();
                break;
            }
        }
        QSpinBox *spinbox = new QSpinBox;
        spinbox->setValue(data.GetAmmoNum());
        spinbox->setMaximum(nMaxNum);
        disconnect(spinbox, 0, 0, 0);
        connect(spinbox, QOverload<int>::of(&QSpinBox::valueChanged), this, &CDlgMissionEdit::on_valueChanged);
        ui->tbwSelectedAmmo->setCellWidget(nRow, 2, spinbox);
        ui->tbwSelectedAmmo->item(nRow, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->tbwSelectedAmmo->item(nRow, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ++nRow;
    }
}

void CDlgMissionEdit::AddQRegExp()
{
    ui->lineEdit->setValidator(new QRegExpValidator(regNumStrUnderLine, this));
}

void CDlgMissionEdit::UpdateEquSelect()
{
    if (m_bIdAmmoEmpty)
    {
        ui->tableWidget_3->clearContents();
        ui->tableWidget_3->setRowCount(0);
        m_bIdAmmoEmpty = false;
    }
    else
    {
        ui->tableWidget_3->clearContents();
        ui->tableWidget_3->setRowCount(m_stMission.GetPlatInfoMap().size());

        int nNo = 0;
        for (auto Resource : m_stMission.GetPlatInfoMap())
        {
            QString strNo, strName, strCount;
            strNo = QString::number(nNo + 1);
            strName = QString::fromStdString(Resource.GetResourceName());
            strCount = QString::number(Resource.GetResourceNum());

            QTableWidgetItem *item0 = new QTableWidgetItem(strNo);
            QTableWidgetItem *item1 = new QTableWidgetItem(strName);
            QTableWidgetItem *item2 = new QTableWidgetItem(strCount);

            ui->tableWidget_3->setItem(nNo, 0, item0);
            ui->tableWidget_3->setItem(nNo, 1, item1);
            ui->tableWidget_3->setItem(nNo, 2, item2);

            ui->tableWidget_3->setItem(nNo, 3, new QTableWidgetItem(QString::fromStdString(Resource.GetPrimaryPoc())));
            ui->tableWidget_3->setItem(nNo, 4, new QTableWidgetItem(QString::fromStdString(Resource.GetSecondPod())));
            ui->tableWidget_3->setItem(nNo, 5, new QTableWidgetItem(QString::fromStdString(Resource.GetPriMountType())));
            ui->tableWidget_3->setItem(nNo, 6, new QTableWidgetItem(QString::number(Resource.GetPriMountNum())));
            ui->tableWidget_3->setItem(nNo, 7, new QTableWidgetItem(QString::fromStdString(Resource.GetSecMountType())));
            ui->tableWidget_3->setItem(nNo, 8, new QTableWidgetItem(QString::number(Resource.GetSecMountNum())));

            ui->tableWidget_3->item(nNo, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            ui->tableWidget_3->item(nNo, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            ui->tableWidget_3->item(nNo, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            ui->tableWidget_3->item(nNo, 3)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            ui->tableWidget_3->item(nNo, 4)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            ui->tableWidget_3->item(nNo, 5)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            ui->tableWidget_3->item(nNo, 6)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            ui->tableWidget_3->item(nNo, 7)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            ui->tableWidget_3->item(nNo, 8)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            nNo++;
        }
    }
}

void CDlgMissionEdit::UpdateEquSelect(PlatfromInfo platfrominfo)
{
    int newRow = ui->tableWidget_3->rowCount();
    ui->tableWidget_3->insertRow(newRow);
    ui->tableWidget_3->setItem(newRow, 0, new QTableWidgetItem(QString::number(newRow + 1)));
    ui->tableWidget_3->setItem(newRow, 1, new QTableWidgetItem(platfrominfo.platType));
    ui->tableWidget_3->setItem(newRow, 2, new QTableWidgetItem(QString::number(1)));
    ui->tableWidget_3->setItem(newRow, 3, new QTableWidgetItem(platfrominfo.PodTypeMain));
    ui->tableWidget_3->setItem(newRow, 4, new QTableWidgetItem(platfrominfo.PodTypeOrder));
    ui->tableWidget_3->setItem(newRow, 5, new QTableWidgetItem(platfrominfo.MountWQMain));
    ui->tableWidget_3->setItem(newRow, 6, new QTableWidgetItem(QString::number(platfrominfo.MountWQMainNum)));
    ui->tableWidget_3->setItem(newRow, 7, new QTableWidgetItem(platfrominfo.MountWQOrder));
    ui->tableWidget_3->setItem(newRow, 8, new QTableWidgetItem(QString::number(platfrominfo.MountWQOrderNum)));
    for (int i = 0; i < ui->tableWidget_3->columnCount(); i++)
    {
        ui->tableWidget_3->item(newRow, i)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    }
}

void CDlgMissionEdit::UpdatePlatData()
{
    int nActivatingID = m_pITaskManageService->GetActivatingActionID();

    if (nActivatingID == -1)
    {
        return;
    }
    if (m_DataType == 2)
    {
        std::vector<std::string> ExistResourceVec;
        ui->tableWidget_3->clearContents();
        CRelationActionResourceVec SensorResVec = m_pITaskManageService->GetRelationSensorResource(nActivatingID);
        QStringList sensorList;
        for (auto sensor : SensorResVec)
        {
            sensorList << QString::fromStdString(sensor.GetResourceName());
        }
        CRelationActionResourceVec AmmoResVec = m_pITaskManageService->GetRelationAmmoResource(nActivatingID);
        QStringList ammoList;
        for (auto ammo : AmmoResVec)
        {
            ammoList << QString::fromStdString(ammo.GetResourceName());
        }
        auto resourceList = m_stMission.GetPlatInfoMap();
        for (auto info : resourceList)
        {
            ExistResourceVec.push_back(info.GetResourceName());
        }
        int nNo = 0;
        ui->tableWidget_3->setRowCount(resourceList.size());
        for (auto list : resourceList)
        {
            QString strNo, strCount;
            strNo = QString::number(nNo + 1);
            std::string strName = list.GetResourceName();
            strCount = QString::number(1);

            QTableWidgetItem *item0 = new QTableWidgetItem(strNo);
            QTableWidgetItem *item1 = new QTableWidgetItem(strName.c_str());
            QTableWidgetItem *item2 = new QTableWidgetItem(strCount);
            ui->tableWidget_3->setItem(nNo, 0, item0);
            ui->tableWidget_3->setItem(nNo, 1, item1);
            ui->tableWidget_3->setItem(nNo, 2, item2);
            QComboBox *item3 = new QComboBox();
            item3->addItems(sensorList);
            ui->tableWidget_3->setCellWidget(nNo, 3, (QWidget *)item3);
            QComboBox *combo = new QComboBox;
            combo->addItems(sensorList);
            ui->tableWidget_3->setCellWidget(nNo, 4, (QWidget *)combo);
            QComboBox *combo4 = new QComboBox;
            combo4->addItems(ammoList);
            ui->tableWidget_3->setCellWidget(nNo, 5, (QWidget *)combo4);

            QSpinBox *spinBox = new QSpinBox;
            spinBox->setValue(1);
            ui->tableWidget_3->setCellWidget(nNo, 6, (QWidget *)spinBox);
            QComboBox *combo5 = new QComboBox;
            combo5->addItems(ammoList);
            ui->tableWidget_3->setCellWidget(nNo, 7, (QWidget *)combo5);
            QSpinBox *spinBox1 = new QSpinBox;
            ui->tableWidget_3->setCellWidget(nNo, 8, (QWidget *)spinBox1);

            ui->tableWidget_3->item(nNo, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            ui->tableWidget_3->item(nNo, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            ui->tableWidget_3->item(nNo, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            nNo++;
        }

        ui->tableWidget_4->clearContents();
        ui->tableWidget_4->setRowCount(0);
        int num = 0;
        CRelationActionResourceVec TaskPTList = m_pITaskManageService->GetRelationPtResource(nActivatingID);
        for (auto info : TaskPTList)
        {
            int count = std::count(ExistResourceVec.begin(), ExistResourceVec.end(), info.GetResourceName());
            int resNum = info.GetResourceNum() - count;
            if (resNum <= 0)
            {
                continue;
            }

            ui->tableWidget_4->insertRow(num);
            QString strNo, strName, strCount;
            strNo = QString::number(num + 1);
            strName = QString::fromStdString(info.GetResourceName());
            strCount = QString::number(resNum);

            QTableWidgetItem *item0 = new QTableWidgetItem(strNo);
            QTableWidgetItem *item1 = new QTableWidgetItem(strName);
            QTableWidgetItem *item2 = new QTableWidgetItem(strCount);
            ui->tableWidget_4->setItem(num, 0, item0);
            ui->tableWidget_4->setItem(num, 1, item1);
            ui->tableWidget_4->setItem(num, 2, item2);

            ui->tableWidget_4->item(num, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            ui->tableWidget_4->item(num, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            ui->tableWidget_4->item(num, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            ++num;
        }
    }
    else
    {
        ui->tableWidget_4->clearContents();
        CRelationActionResourceVec resourceList = m_pITaskManageService->GetRelationPtResource(nActivatingID);
        ui->tableWidget_4->setRowCount(resourceList.size());
        int nNo = 0;
        for (auto info : resourceList)
        {
            QString strNo, strName, strCount;
            strNo = QString::number(nNo + 1);
            strName = QString::fromStdString(info.GetResourceName());
            strCount = QString::number(info.GetResourceNum());

            QTableWidgetItem *item0 = new QTableWidgetItem(strNo);
            QTableWidgetItem *item1 = new QTableWidgetItem(strName);
            QTableWidgetItem *item2 = new QTableWidgetItem(strCount);
            ui->tableWidget_4->setItem(nNo, 0, item0);
            ui->tableWidget_4->setItem(nNo, 1, item1);
            ui->tableWidget_4->setItem(nNo, 2, item2);
            ui->tableWidget_4->item(nNo, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            ui->tableWidget_4->item(nNo, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            ui->tableWidget_4->item(nNo, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

            ++nNo;
        }
    }
}

void CDlgMissionEdit::showEvent(QShowEvent *event)
{
    m_mMarkAndSchemeID.clear();
    bool temp = LY_TaskManage::Get().GetDlgMissionList()->b_openTask;
    if (!temp)
    {
        UpdateMarkBackup();
        UpdateMarkSelect();
        UpdataAmmoBackup();
        UpdateAmmoSelect();
        UpdateEquSelect();
        UpdateEquBackup();
    }
    else
    {
        SchemeGenerateTask();
        UpdateMarkBackup();
        UpdateMarkSelect();
        UpdataAmmoBackup();
        UpdateAmmoSelect();
        UpdateEquSelect();
        UpdateEquBackup();
        ui->lineEdit_15->setText(tr(m_ShcemeDamageRequest.c_str()));
    }
}

void CDlgMissionEdit::closeEvent(QCloseEvent *event)
{
    m_RouteLineVec.clear();
    return;
}

void CDlgMissionEdit::SaveDataToSturct()
{
    int row = ui->tableWidget_3->rowCount();
    int ptMaxID = m_pITaskManageService->GetRelationTaskPtMaxID();
    m_stMission.ClearPlatResourcesList();
    for (int i = 0; i < row; i++)
    {
        QTableWidgetItem *pItem0 = ui->tableWidget_3->item(i, 0);
        int nMake = pItem0->text().toInt();
        QTableWidgetItem *pItem1 = ui->tableWidget_3->item(i, 1);
        std::string strName = pItem1->text().toStdString();
        QTableWidgetItem *pItem2 = ui->tableWidget_3->item(i, 2);
        int nNum = pItem2->text().toInt();

        QTableWidgetItem *pItem3 = ui->tableWidget_3->item(i, 3);
        std::string strPrimaryPod = pItem3->text().toStdString();

        QTableWidgetItem *pItem4 = ui->tableWidget_3->item(i, 4);
        std::string secondpod = pItem4->text().toStdString();

        QTableWidgetItem *pItem5 = ui->tableWidget_3->item(i, 5);
        std::string primaryAmmo = pItem5->text().toStdString();

        QTableWidgetItem *pItem6 = ui->tableWidget_3->item(i, 6);
        int spinNum = pItem6->text().toInt();

        QTableWidgetItem *pItem7 = ui->tableWidget_3->item(i, 7);
        std::string secondAmmo = pItem7->text().toStdString();

        QTableWidgetItem *pItem8 = ui->tableWidget_3->item(i, 8);
        int spinNum1 = pItem8->text().toInt();
        CRelationMissionResource Task;
        Task.SetID(ptMaxID);
        std::string strTaskID = ui->lineEdit->text().toStdString();
        Task.SetMissionID(strTaskID);
        Task.SetResourceName(strName);
        Task.SetResourceNum(1);
        Task.SetPrimaryPod(strPrimaryPod);
        Task.SetSecondPod(secondpod);
        Task.SetPriMountType(primaryAmmo);
        Task.SetPriMountNum(spinNum);
        Task.SetSecMountType(secondAmmo);
        Task.SetSecMountNum(spinNum1);
        m_stMission.AddPlatResources(Task);
        ++ptMaxID;
    }
    m_stMission.ClearMarkDamageVec();
    int tableWidgetRow = ui->tableWidget->rowCount();
    int MarkDamageMaxID = m_pITaskManageService->GetMaxRelationMissionMarkID();
    for (int i = 0; i < tableWidgetRow; i++)
    {
        QTableWidgetItem *pItem0 = ui->tableWidget->item(i, 1);
        std::string strMarkID = pItem0->text().toStdString();

        QWidget *widgetCom0 = ui->tableWidget->cellWidget(i, 3);
        QComboBox *comboBox0 = qobject_cast<QComboBox *>(widgetCom0);
        int index = comboBox0->currentIndex();
        QVariant  data = comboBox0->itemData(index);
        int strMarkDamage = data.toInt();
        CRelationMissionMark temp;
        temp.SetID(MarkDamageMaxID);
        std::string strTaskID = tr3(ui->lineEdit->text());
        temp.SetMissionID(strTaskID);
        temp.SetMarkID(strMarkID);
        temp.SetMarkDamage(strMarkDamage);
        m_stMission.AddMarkDamage(temp);
        ++MarkDamageMaxID;
    }

}

void CDlgMissionEdit::on_valueChanged(int value)
{
    int Row = ui->tbwSelectedAmmo->currentRow();
    string strAmmoName = tr3(ui->tbwSelectedAmmo->item(Row, 1)->text());
    for (auto &data : m_CRelationMissionAmmoVec)
    {
		const std::string &dataAmmoName = data.GetAmmoName();
        if (strAmmoName == dataAmmoName)
        {
            data.SetAmmoNum(value);
        }
    }
    UpdataAmmoBackup();
}

void CDlgMissionEdit::on_pushButton_clicked()
{
    if (ui->lineEdit->text() == "")
    {
        LyMessageBox::information(this, tr("tip"), tr("Mark code cannot be null, please retype"));
        return;
    }
    SaveDataToSturct();
    SaveMissionAmmo();

    std::map<string, int> DataNum;
    for (auto info : m_CRelationMissionAmmoVec)
    {
        std::string str = info.GetAmmoName();
        int num = info.GetAmmoNum();
        DataNum[str] = num;
    }

    for (auto info : m_stMission.GetPlatInfoMap())
    {
        std::string strname = "";
        if (info.GetPriMountType() == "")
        {
            continue;
        }
		const std::string &str = info.GetPriMountType();
        strname = str;
        int num = info.GetPriMountNum();
        int count = DataNum[strname];
        if (num > count)
        {
            LyMessageBox::information(this, tr("tip"), tr("The number of ammo is less than the number of platform mounts!"));
            return;
        }
    }

    CNeubulaMission stInfo;
    stInfo.SetID(ui->lineEdit->text().toStdString());
    stInfo.SetName(ui->lineEdit_2->text().toStdString());
    stInfo.SetBelong(ui->lineEdit_3->text().toStdString());
    stInfo.SetType(stInfo.GetTypeENum(ui->comboBox_2->currentIndex()));
    stInfo.SetDetectDemand(ui->lineEdit_15->text().toStdString());
    stInfo.SetValidTime(ui->lineEdit_19->text().toStdString());
    stInfo.SetStartTime(ui->dateTimeEdit->text().toStdString());
    stInfo.SetAttackTime(ui->dateTimeEdit_2->text().toStdString());
    stInfo.SetFinishTime(ui->dateTimeEdit_3->text().toStdString());
    stInfo.SetRemark(ui->textEdit->toPlainText().toStdString());
    stInfo.SetStartLon(ui->lineEdit_7->text().toDouble());
    stInfo.SetStartLat(ui->lineEdit_9->text().toDouble());
    stInfo.SetStartAlt(ui->lineEdit_10->text().toDouble());
    stInfo.SetMarkIDList(m_stMission.GetMarkIDList());
    stInfo.SetPlatInfoMap(m_stMission.GetPlatInfoMap());
    stInfo.SetMarkDamageVec(m_stMission.GetMarkDamageVec());
    stInfo.SetState(stInfo.GetStateENum(1));

    if (ui->comboBox_2->currentIndex() == 7)
    {
        stInfo.SetDBMissionType(ui->comboBox->currentIndex());
        stInfo.SetDetectDemand(ui->lineEdit_4->text().toStdString());
        stInfo.SetDBSource(ui->lineEdit_5->text().toStdString());
        stInfo.SetStartTime(ui->dateTimeEdit_4->text().toStdString());
        stInfo.SetFinishTime(ui->dateTimeEdit_5->text().toStdString());
    }

    if (m_DataType == 1 && m_pITaskManageService != nullptr)
    {

        bool bresut = m_pITaskManageService->IsExisted(stInfo.GetID());
        if (bresut == true)
        {
            LyMessageBox::information(this, tr("tip"), tr("ID duplicate, please modify"));
            return;
        }

        bresut = m_pITaskManageService->SetMission(stInfo);
        for (auto data : m_CRelationMissionAmmoVec)
        {
            bool bSaveMissionAmmo = m_pITaskManageService->AddMissionAmmo(data);
            if (!bSaveMissionAmmo)
            {
                LyMessageBox::information(this, tr("tip"), tr("Failed to add misson ammo!"));
                return;
            }
        }
        if (bresut == true)
        {

            CRelationActionTask stRelation;
            stRelation.SetID(m_pITaskManageService->GetRelationTaskMaxID());
            stRelation.SetActionID(m_pITaskManageService->GetActivatingActionID());
            stRelation.SetTaskID(stInfo.GetID());
            m_pITaskManageService->AddRelationTask(stRelation);

        }
        else
        {
            LyMessageBox::information(this, tr("tip"), tr("Add failed"));
            return;
        }

        m_DataType = 0;
    }

    else if (m_DataType == 2)
    {
        bool bresut = false;
        if (0 != strcmp(m_stMission.GetID().c_str(), stInfo.GetID().c_str()))
        {
            if (m_pITaskManageService->IsExisted(stInfo.GetID()))
            {
                LyMessageBox::information(this, tr("tip"), tr("Name duplicate, please reenter."));
                return;
            }
            bresut = m_pITaskManageService->SetMission(stInfo, m_stMission.GetID());
            bool bSaveMissionAmmo = m_pITaskManageService->RemoveMissionIDAmmo(stInfo.GetID());
            for (auto data : m_CRelationMissionAmmoVec)
            {
                bSaveMissionAmmo = m_pITaskManageService->AddMissionAmmo(data);
                if (!bSaveMissionAmmo)
                {
                    LyMessageBox::information(this, tr("tip"), tr("Failed to add misson ammo!"));
                    return;
                }
            }
        }
        else
        {
            bresut = m_pITaskManageService->SetMission(stInfo);
            bool bSaveMissionAmmo = m_pITaskManageService->RemoveMissionIDAmmo(stInfo.GetID());
            for (auto data : m_CRelationMissionAmmoVec)
            {
                bSaveMissionAmmo = m_pITaskManageService->AddMissionAmmo(data);
                if (!bSaveMissionAmmo)
                {
                    LyMessageBox::information(this, tr("tip"), tr("Failed to add misson ammo!"));
                    return;
                }
            }
        }
        if (!bresut)
        {
            LyMessageBox::information(this, tr("tip"), tr("Add failed"));
        }
        m_DataType = 0;
    }

    m_CRelationMissionAmmoVec.clear();
    m_pITaskManageService->LoadMissionAmmo();
    m_pITaskManageService->RmoveAllMission();
    m_pITaskManageService->LoadData();
    LY_TaskManage::Get().GetDlgMissionList()->UpdataWindowData();
    LY_TaskManage::Get().GetDlgMissionList()->UpdateListData();
    if(LY_TaskManage::Get().GetDlgMissionTree() != nullptr){
        LY_TaskManage::Get().GetDlgMissionTree()->Update(m_pITaskManageService);
    }
    m_pITaskManageService->LoadMissionLine();

    bool bl = LY_TaskManage::Get().GetDlgMissionList()->m_bIsSaveAs;
    if (bl)
    {
        LY_TaskManage::Get().GetDlgMissionList()->m_bIsSaveAs = false;
        m_MissionLineVec = LY_TaskManage::Get().GetDlgMissionList()->m_MissionLineVec;
        for (auto data : m_MissionLineVec)
        {
            m_RouteLineVec = m_pITaskManageService->getMissionLinePointsFornLineID(data.nID);
            if (!m_RouteLineVec.empty())
            {
                CNeubulaMissionLine MissionLine;
                int maxID = m_pITaskManageService->GetMissionLineMaxID();
                MissionLine.nID = maxID;
                std::string taskID = tr3(ui->lineEdit->text());
                QString qstrLineName = "HX" + QString::number(maxID);
                strncpy(MissionLine.strMissionID, taskID.c_str(), MAX_LINE_LEN_1);
                strncpy(MissionLine.strName, qstrLineName.toStdString().c_str(), MAX_LINE_LEN_1);
                MissionLine.cShowFlag = false;
                MissionLine.cProtocolType = 1;
                for (int i = 0; i < m_RouteLineVec.size(); i++)
                {
                    m_RouteLineVec[i].nLineID = maxID;
                }
                MissionLine.LinePointVec = m_RouteLineVec;
                m_pITaskManageService->AddMissionLine(MissionLine);
            }
            this->hide();
            m_RouteLineVec.clear();

            qnzkna::framework::MessageReferenceProps lineprops;
            int num = qrand() % 100;
            string strIn = "savesucc";
            strIn += to_string(num);
            lineprops.put("lineprops", strIn);
            ITaskManagerActivator::Get().getIRegistryImpl()->messageNotify(new qnzkna::framework::MessageReference("MsgSucceedSave", nullptr, 0, lineprops));
        }
    }
    else
    {
        if (!m_RouteLineVec.empty())
        {
            CNeubulaMissionLine MissionLine;
            int maxID = m_pITaskManageService->GetMissionLineMaxID();
            MissionLine.nID = maxID;
            std::string taskID = tr3(ui->lineEdit->text());
            strncpy(MissionLine.strMissionID, taskID.c_str(), MAX_LINE_LEN_1);
            strncpy(MissionLine.strName, strLineName.c_str(), MAX_LINE_LEN_1);
            MissionLine.cShowFlag = false;
            MissionLine.cProtocolType = 1;
            for (int i = 0; i < m_RouteLineVec.size(); i++)
            {
                m_RouteLineVec[i].nLineID = maxID;
            }
            MissionLine.LinePointVec = m_RouteLineVec;
            m_pITaskManageService->AddMissionLine(MissionLine);
        }
        this->hide();
        m_RouteLineVec.clear();

        qnzkna::framework::MessageReferenceProps lineprops;
        int num = qrand() % 100;
        string strIn = "savesucc";
        strIn += to_string(num);
        lineprops.put("lineprops", strIn);
        ITaskManagerActivator::Get().getIRegistryImpl()->messageNotify(new qnzkna::framework::MessageReference("MsgSucceedSave", nullptr, 0, lineprops));
    }
}

void CDlgMissionEdit::on_pushButton_2_clicked()
{
    m_RouteLineVec.clear();
    this->hide();
}

void CDlgMissionEdit::on_pushButton_3_clicked()
{
    QString strBTN = ui->pushButton_3->text();
    if (0 == QString::compare(strBTN, tr("Screen Fetch Point")))
    {
        ui->pushButton->setEnabled(false);
        ui->pushButton_2->setEnabled(false);
        ui->pushButton_3->setText(tr("End of Point"));
        m_PickPointStates = PICK_POINT_EDIT;
    }
    else if (0 == QString::compare(strBTN, tr("End of Point")))
    {
        ui->pushButton->setEnabled(true);
        ui->pushButton_3->setEnabled(true);
        ui->pushButton_3->setText(tr("Screen Fetch Point"));
        m_PickPointStates = PICK_POINT_NORMAL;
    }
}

void CDlgMissionEdit::on_pushButton_4_clicked()
{
    if (ui->tableWidget_2->rowCount() == 0)
    {
        LyMessageBox::information(NULL, tr("tip"), tr("Currently no target!"));
        return;
    }

    int nSelectedRow = ui->tableWidget_2->currentRow();
    if (nSelectedRow == -1)
    {
        LyMessageBox::information(NULL, tr("tip"), tr("Please select the target!"));
        return;
    }

    QTableWidgetItem *pItem = ui->tableWidget_2->item(nSelectedRow, 1);
    QString strMarkID = pItem->text();

    m_stMission.AddMark(strMarkID.toStdString());
    CRelationMissionMark temp;
    temp.SetMarkID(strMarkID.toStdString());
    temp.SetMarkDamage(0);
    m_stMission.AddMarkDamage(temp);

    UpdateMarkBackup();
    UpdateMarkSelect();
}

void CDlgMissionEdit::on_pushButton_5_clicked()
{
    if (ui->tableWidget->rowCount() == 0)
    {
        LyMessageBox::information(NULL, tr("tip"), tr("Currently no target!"));
        return;
    }

    int nSelectedRow = ui->tableWidget->currentRow();
    if (nSelectedRow == -1)
    {
        LyMessageBox::information(NULL, tr("tip"), tr("Please select the target!"));
        return;
    }

    QTableWidgetItem *pItem = ui->tableWidget->item(nSelectedRow, 1);
    QString strMarkID = pItem->text();

    m_stMission.RomoveMark(pItem->text().toStdString());
    m_stMission.RomoveMarkDamage(pItem->text().toStdString());
    CRelationMissionAmmoVec tempRelationMissionAmmoVec = GettbwSelectedAmmoData();
    std::string sSchemeID;
    auto it = m_mMarkAndSchemeID.find(pItem->text().toStdString());
    if (it != m_mMarkAndSchemeID.end())
    {
        sSchemeID = m_mMarkAndSchemeID[pItem->text().toStdString()];
        m_mMarkAndSchemeID.erase(it);
    }

    if (!sSchemeID.empty())
    {
        CNebulaSchemeAmmoVec SchemeAmmoVec = m_pITaskManageService->GetAmmoBySchemeID(sSchemeID);
        for (auto data : SchemeAmmoVec)
        {
            auto itr = tempRelationMissionAmmoVec.begin();
            for (; itr != tempRelationMissionAmmoVec.end(); itr++)
            {
                if (data.GetAmmoType() == itr->GetAmmoName())
                {
                    itr->SetAmmoNum(itr->GetAmmoNum() - data.GetAmmoNum());
                    if (itr->GetAmmoNum() <= 0)
                    {
                        tempRelationMissionAmmoVec.erase(itr);
                    }
                    break;
                }
            }
        }
    }
    m_CRelationMissionAmmoVec = tempRelationMissionAmmoVec;
    UpdateMarkBackup();
    UpdateMarkSelect();
    UpdateAmmoSelect();
    UpdataAmmoBackup();
}

void CDlgMissionEdit::on_pushButton_6_clicked()
{
    if (ui->tableWidget_4->rowCount() == 0)
    {
        LyMessageBox::information(NULL, tr("tip"), tr("Currently no platform!"));
        return;
    }
    int currentRow = ui->tableWidget_4->currentRow();
    if (currentRow == -1)
    {
        LyMessageBox::information(NULL, tr("tip"), tr("Please select the platform!"));
        return;
    }
    QTableWidgetItem *item = ui->tableWidget_4->item(currentRow, 1);
    if (item == nullptr)
    {
        return;
    }
    QString platType = item->text();

    AddPlatButton(platType, currentRow);
}

void CDlgMissionEdit::SortPlatType()
{
    ui->tableWidget_3->sortByColumn(1, Qt::AscendingOrder);
    for (int i = 0; i < ui->tableWidget_3->rowCount(); i++)
    {
        ui->tableWidget_3->setItem(i, 0, new QTableWidgetItem(QString::number(i + 1)));
    }
}
int CDlgMissionEdit::CountcurrentWQNum(QString text)
{
    int currentWQNum = 0;
    int RowCount = ui->tableWidget_3->rowCount();
    for (int i = 0; i < RowCount; i++)
    {
        if (ui->tableWidget_3->item(i, 5) != nullptr && ui->tableWidget_3->item(i, 5)->text() == text)
        {
            currentWQNum += ui->tableWidget_3->item(i, 6)->text().toInt();
        }
        if (ui->tableWidget_3->item(i, 7) != nullptr && ui->tableWidget_3->item(i, 7)->text() == text)
        {
            currentWQNum += ui->tableWidget_3->item(i, 8)->text().toInt();
        }
    }
    return currentWQNum;
}

QStringList CDlgMissionEdit::CreatTableHeadEquipInfo()
{
    QStringList headerlabels2;

    if (m_pITaskManageService == nullptr)
    {
        return headerlabels2;
    }

    int nActivatingID = m_pITaskManageService->GetActivatingActionID();

    if (nActivatingID == -1)
    {
        return headerlabels2;
    }
    headerlabels2 << tr("No.") << tr("Platform Type") << tr("Platform Number");
    CRelationActionResourceVec ResourceVec = m_pITaskManageService->GetRelationSensorResource(nActivatingID);
    if (ResourceVec.size() == 0 || ResourceVec.size() == 1)
    {
        headerlabels2 << tr("Pod Type");
        headerlabels2 << tr("Pod Number");
    }
    else
    {
        for (int i = 0; i < ResourceVec.size(); i++)
        {
            QString str = tr("Pod Type") + QString::number(i + 1);
            headerlabels2 << str;
            str += tr("number");
            headerlabels2 << str;
        }
    }
    ResourceVec = m_pITaskManageService->GetRelationAmmoResource(nActivatingID);
    if (ResourceVec.size() == 0 || ResourceVec.size() == 1)
    {
        headerlabels2 << tr("Weapon Type");
        headerlabels2 << tr("Weapon Number");
    }
    else
    {
        for (int i = 0; i < ResourceVec.size(); i++)
        {
            QString str = tr("Weapon Type") + QString::number(i + 1);
            headerlabels2 << str;
            str += tr("number");
            headerlabels2 << str;
        }
    }
    return headerlabels2;
}

void CDlgMissionEdit::AddPlatButton(QString platTyp, int currentRow)
{
    int nSelectedRow = ui->tableWidget_4->currentRow();
    QTableWidgetItem *item = ui->tableWidget_4->item(nSelectedRow, 2);
    if (item->text().toInt() == 0)
    {
        LyMessageBox::information(NULL, tr("tip"), tr("Insufficient number of selected platforms!"));
        return;
    }
    LY_TaskManage::Get().m_pDlgMissionPlatfromInfo->Init(platTyp, currentRow);
    LY_TaskManage::Get().m_pDlgMissionPlatfromInfo->show();
}

void CDlgMissionEdit::SchemeUpdateMark(const std::string strMarkID, const std::string strDamageRequest)
{
    m_ShcemeMarkID = strMarkID;
    m_ShcemeDamageRequest = strDamageRequest;
}

void CDlgMissionEdit::AcceptOnlyMarkInfo(const char *msgInfo, const std::string targetID, const std::string strlineName,
                                           QGeoCoordinate startpoint)
{
    CRelationMissionResource MissionInfo;
    memcpy(&MissionInfo, msgInfo, sizeof(CRelationMissionResource));
    CRelationMissionResourceVec platinfoVec;
    platinfoVec.push_back(MissionInfo);

    MarkIDVec targetVec;
    targetVec.push_back(targetID);
    m_stMission.SetMarkIDList(targetVec);
    m_stMission.SetPlatInfoMap(platinfoVec);
    if (LY_TaskManage::Get().GetDlgMissionEdit() == nullptr)
    {
        return;
    }
    startPoint = startpoint;
    strLineName = strlineName;

    CRelationMissionAmmo ammo;
    m_CRelationMissionAmmoVec.clear();
    ammo.SetAmmoName(platinfoVec[0].GetPriMountType());
    ammo.SetAmmoNum(platinfoVec[0].GetPriMountNum());
    m_CRelationMissionAmmoVec.push_back(ammo);

    emit MsgOnlyMark();
}

void CDlgMissionEdit::AcceptOnlyLineInfo(const char *msgInfo, const string &strsize)
{

    MissionLinePointVec LinepointVec;
    int num = atoi(strsize.c_str());
    const MissionLinePoint *mission = reinterpret_cast<const MissionLinePoint *>(msgInfo);
    int count = atoi(strsize.c_str());
    for (int i = 0; i < count; i++)
    {
        LinepointVec.push_back(mission[i]);
    }
    m_RouteLineVec.clear();
    for (auto line : LinepointVec)
    {
        CNeubulaMissionLinePoint info;
        info.nNo = line.nNo;
        info.dLon = line.dLon;
        info.dLat = line.dLat;
        info.dAlt = line.dAlt;
        info.dSpeed = line.dSpeed;
        info.dHoverTime = line.dHoverTime;
        int height = int(line.cHeightCtrl);
        if (height < 48 || height > 57)
        {
            height = 1;
        }
        else
        {
            height -= 48;
        }
        info.cHeightCtrl = height;
        info.DRadius = line.DRadius;
        int linemode = int(line.cLineMode);
        if (linemode < 48 || linemode > 57)
        {
            linemode = 1;
        }
        else
        {
            linemode -= 48;
        }
        info.cLineMode = linemode;
        int turnmode = int(line.cTurnMode);
        if (turnmode < 48 || turnmode > 57)
        {
            turnmode = 1;
        }
        else
        {
            turnmode -= 48;
        }
        info.cTurnMode = turnmode;
        m_RouteLineVec.push_back(info);
    }
}

void CDlgMissionEdit::SaveMissionAmmo()
{
    m_CRelationMissionAmmoVec.clear();
    int rowCount = ui->tbwSelectedAmmo->rowCount();
    for (int i = 0; i < rowCount; i++)
    {
        string strMissionID = tr3(ui->lineEdit->text());
        string strAmmoName = tr3(ui->tbwSelectedAmmo->item(i, 1)->text());
        QWidget *widgetCom0 = ui->tbwSelectedAmmo->cellWidget(i, 2);
        QSpinBox *spinBox0 = qobject_cast<QSpinBox *>(widgetCom0);
        int nAmmoNum = spinBox0->text().toInt();
        CRelationMissionAmmo temp;
        temp.SetAmmoName(strAmmoName);
        temp.SetMissionID(strMissionID);
        temp.SetAmmoNum(nAmmoNum);
        m_CRelationMissionAmmoVec.push_back(temp);
    }
}

void CDlgMissionEdit::CreatetbwSelectedAmmoMenu()
{
    m_mSelectedAmmoTableMenu = new QMenu(this);
    QAction *TaskMenu_select = m_mSelectedAmmoTableMenu->addAction(tr("Select Mark Solution"));
    connect(TaskMenu_select, SIGNAL(triggered()), this, SLOT(on_Menu_MarkSchemeID_select()));
}

CRelationMissionAmmoVec CDlgMissionEdit::GettbwSelectedAmmoData()
{
    CRelationMissionAmmoVec newVec;
    int rowCount = ui->tbwSelectedAmmo->rowCount();
    for (int i = 0; i < rowCount; i++)
    {
        QWidget *widgetCom0 = ui->tbwSelectedAmmo->cellWidget(i, 2);
        QSpinBox *spinBox0 = qobject_cast<QSpinBox *>(widgetCom0);
        int nAmmoNum = spinBox0->text().toInt();
        CRelationMissionAmmo temp;
        temp.SetAmmoName(ui->tbwSelectedAmmo->item(i, 1)->text().toStdString());
        temp.SetAmmoNum(nAmmoNum);
        newVec.push_back(temp);
    }
    return newVec;
}

void CDlgMissionEdit::SchemeGenerateTask()
{
    m_stMission.AddMark(m_ShcemeMarkID);
    CRelationMissionMark temp;
    temp.SetMarkID(m_ShcemeMarkID);
    temp.SetMarkDamage(0);
    m_stMission.AddMarkDamage(temp);
    CNebulaSchemeAmmoVec SchemeAmmoVec = m_pITaskManageService->GetAmmoBySchemeID(m_sSchemeID);
    QMap<std::string, int> mAmmoNameNum;
    for (auto data : SchemeAmmoVec)
    {
        auto it = mAmmoNameNum.find(data.GetAmmoType());
        if (it != mAmmoNameNum.end())
        {
            mAmmoNameNum[data.GetAmmoType()] += 1;
        }
        else
        {
            mAmmoNameNum.insert(data.GetAmmoType(), 1);
        }
    }
    CRelationMissionAmmoVec tempVec1;
    for (auto it = mAmmoNameNum.begin(); it != mAmmoNameNum.end(); ++it)
    {
        CRelationMissionAmmo missionAmmo;
        missionAmmo.SetAmmoName(it.key());
        missionAmmo.SetAmmoNum(it.value());
        tempVec1.push_back(missionAmmo);
    }
    m_CRelationMissionAmmoVec = tempVec1;
    m_mMarkAndSchemeID.insert(m_ShcemeMarkID, m_sSchemeID);
}

void CDlgMissionEdit::on_pushButton_7_clicked()
{
    if (ui->tableWidget_3->rowCount() == 0)
    {
        LyMessageBox::information(NULL, tr("tip"), tr("Currently no platform!"));
        return;
    }

    int nSelectedRow = ui->tableWidget_3->currentRow();
    if (nSelectedRow == -1)
    {
        LyMessageBox::information(NULL, tr("tip"), tr("Please select the platform!"));
        return;
    }

    UpdateEquBackup1();
    UpdateEquSelect1();
}

void CDlgMissionEdit::on_pushButton_8_clicked()
{
    if (ui->tbwAlternateAmmo->rowCount() == 0)
    {
        LyMessageBox::information(NULL, tr("tip"), tr("Currently no ammo!"));
        return;
    }

    int nSelectedRow = ui->tbwAlternateAmmo->currentRow();
    if (nSelectedRow == -1)
    {
        LyMessageBox::information(NULL, tr("tip"), tr("Please select the ammo!"));
        return;
    }
    QTableWidgetItem *pItemAmmoName = ui->tbwAlternateAmmo->item(nSelectedRow, 1);
    std::string strAmmoName = tr3(pItemAmmoName->text());
    for (auto data : m_CRelationMissionAmmoVec)
    {
		const std::string &AmmoName = data.GetAmmoName();
        if (strAmmoName == AmmoName)
        {
            LyMessageBox::information(NULL, tr("tip"), tr("The current ammo is selected!"));
            return;
        }
    }
    CRelationMissionAmmo newMissionAmmo;
    newMissionAmmo.SetAmmoName(pItemAmmoName->text().toStdString());
    newMissionAmmo.SetAmmoNum(0);
    m_CRelationMissionAmmoVec.push_back(newMissionAmmo);
    UpdateAmmoSelect();
}

void CDlgMissionEdit::on_pushButton_9_clicked()
{
    m_mMarkAndSchemeID.clear();
    if (ui->tbwSelectedAmmo->rowCount() == 0)
    {
        LyMessageBox::information(NULL, tr("tip"), tr("Currently no ammo!"));
        return;
    }

    int nSelectedRow = ui->tbwSelectedAmmo->currentRow();
    if (nSelectedRow == -1)
    {
        LyMessageBox::information(NULL, tr("tip"), tr("Please select the ammo!"));
        return;
    }

    QTableWidgetItem *pItemAmmoName = ui->tbwSelectedAmmo->item(nSelectedRow, 1);
    std::string strAmmoName = tr3(pItemAmmoName->text());
    auto itr = m_CRelationMissionAmmoVec.begin();
    for (; itr != m_CRelationMissionAmmoVec.end(); itr++)
    {
		const std::string &ammoname = itr->GetAmmoName();
        if (strAmmoName == ammoname)
        {
            m_CRelationMissionAmmoVec.erase(itr);
            break;
        }
    }
    UpdateAmmoSelect();
    UpdataAmmoBackup();
}

void CDlgMissionEdit::on_tableWidget_2_itemDoubleClicked(QTableWidgetItem *item)
{
    on_pushButton_4_clicked();
}

void CDlgMissionEdit::on_tableWidget_itemDoubleClicked(QTableWidgetItem *item)
{
    on_pushButton_5_clicked();
}

void CDlgMissionEdit::on_tableWidget_4_itemDoubleClicked(QTableWidgetItem *item)
{
    if (ui->tableWidget_4->rowCount() == 0)
    {
        LyMessageBox::information(NULL, tr("tip"), tr("Currently no platform!"));
        return;
    }
    int currentRow = ui->tableWidget_4->currentRow();
    item = ui->tableWidget_4->item(currentRow, 1);
    QString platType = item->text();
    if (currentRow == -1)
    {
        LyMessageBox::information(NULL, tr("tip"), tr("Please select the platform!"));
        return;
    }
    AddPlatButton(platType, currentRow);
}

void CDlgMissionEdit::on_tableWidget_3_itemDoubleClicked(QTableWidgetItem *item)
{
    on_pushButton_7_clicked();
}

void CDlgMissionEdit::on_tbwAlternateAmmo_itemDoubleClicked(QTableWidgetItem *item)
{
    on_pushButton_8_clicked();
}

void CDlgMissionEdit::on_tbwSelectedAmmo_itemDoubleClicked(QTableWidgetItem *item)
{
    on_pushButton_9_clicked();
}

void CDlgMissionEdit::on_comboBox_2_currentIndexChanged(int index)
{
    CNeubulaMission stInfo;
    ENeubulaMissionType eType = stInfo.GetTypeENum(ui->comboBox_2->currentIndex());
    QString strtips;
    bool danbingshow = false;
    ui->lineEdit_19->setEnabled(false);
    switch (eType)
    {
    case ENeubulaMissionType_Unknow:
        strtips = tr("Task Requirements:");
        break;
    case ENeubulaMissionType_Attack:
        strtips = tr("Strike Task Requirements:");
        break;
    case ENeubulaMissionType_Detect:
        strtips = tr("Reconnaissance Information Requirements:");
        ui->lineEdit_19->setEnabled(true);
        break;
    case ENeubulaMissionType_Protect:
        strtips = tr("Protection Task Requirements:");
        break;
    case ENeubulaMissionType_Overlook:
        strtips = tr("Listening Task Requirements:");
        break;
    case ENeubulaMissionType_Help:
        strtips = tr("Rescue Task Requirements:");
        break;
    case ENeubulaMissionType_Transport:
        strtips = tr("Transportation Task Requirements:");
        break;
    case ENeubulaMissionType_DB:
    {
        strtips = tr("Single Pawn Task Requirements:");
        danbingshow = true;
    }
    break;
    default:
        strtips = tr("Task Requirements");
        break;
    }

    ui->label_20->setText(strtips);
    ui->groupBox_3->setHidden(danbingshow);
    ui->groupBox_8->setHidden(danbingshow);
    ui->groupBox_7->setHidden(danbingshow);
    ui->groupBox->setHidden(danbingshow);
    ui->groupBox_5->setHidden(!danbingshow);

}

void CDlgMissionEdit::on_treeWidget_customContextMenuRequested(const QPoint &pos)
{
    m_mSelectedAmmoTableMenu->exec(QCursor::pos());
}

void CDlgMissionEdit::on_Menu_MarkSchemeID_select()
{

    int row = ui->tableWidget->currentRow();
    m_sSelectMarkID = ui->tableWidget->item(row, 1)->text().toStdString();
    if (LY_TaskManage::Get().GetDlgMarkSchemeList())
    {
        if (LY_TaskManage::Get().GetDlgMarkSchemeList()->isVisible())
        {
            LY_TaskManage::Get().GetDlgMarkSchemeList()->close();
        }
        else
        {
            LY_TaskManage::Get().GetDlgMarkSchemeList()->Init();

            LY_TaskManage::Get().GetDlgMarkSchemeList()->show();
        }
    }
}

void CDlgMissionEdit::on_combobox_currentIndexChanged(int value)
{
    int Row = ui->tableWidget->currentRow();
    QString strMarkID = ui->tableWidget->item(Row, 1)->text();
    CRelationMissionMarkVec temp = m_stMission.GetMarkDamageVec();
    for (auto &data : temp)
    {
        QString dataMarkID = QString::fromStdString(data.GetMarkID());
        if (strMarkID == dataMarkID)
        {
            data.SetMarkDamage(value);
        }
    }
    m_stMission.SetMarkDamageVec(temp);
    UpdateMarkSelect();
}

void CDlgMissionEdit::on_Start_DataTimeChanged()
{
    QDateTime tempMin = ui->dateTimeEdit->dateTime();
    QDateTime tempMax = ui->dateTimeEdit_3->dateTime();
    ui->dateTimeEdit_2->setMinimumDateTime(tempMin);
    ui->dateTimeEdit_2->setMaximumDateTime(tempMax);
    ui->dateTimeEdit_3->setMinimumDateTime(tempMin);

}

void CDlgMissionEdit::on_End_DataTimeChanged()
{
    QDateTime tempMin = ui->dateTimeEdit->dateTime();
    QDateTime tempMax = ui->dateTimeEdit_3->dateTime();
    ui->dateTimeEdit_2->setMinimumDateTime(tempMin);
    ui->dateTimeEdit_2->setMaximumDateTime(tempMax);

}
