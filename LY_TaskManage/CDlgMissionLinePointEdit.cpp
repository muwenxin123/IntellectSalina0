#include "CDlgMissionLinePointEdit.h"
#include "ui_CDlgMissionLinePointEdit.h"
#include "QCString.h"
#include "CoordinateTranfer.h"
#include "ITaskManagerActivator.h"
#include "lY_TaskManage.h"
#include <QMenu>

#define INIT_HEIGHT						150

CDlgMissionLinePointEdit::CDlgMissionLinePointEdit(QWidget *parent) :
    LyDialog(tr("Route Editing"), parent),
    ui(new Ui::CDlgMissionLinePointEdit)
{
    ui->setupUi(centralWidget());
    this->setResizable(true);
    this->resize(1200, 800);

    m_pITaskManageService = ITaskManagerActivator::Get().getPTaskManageService();
    if (m_pITaskManageService == nullptr)
    {
        return;
    }

    InitWindow();
    CreateTableMenu();
    TabWidgetConnet();
}

CDlgMissionLinePointEdit::~CDlgMissionLinePointEdit()
{
    delete ui;
}

void CDlgMissionLinePointEdit::InitWindow()
{
    ui->lineEdit->setText("");
    ui->lineEdit_2->setText("");

    ui->label_20->setText("");
    ui->spinBox_5->setValue(0);
    ui->spinBox_6->setValue(0);
    ui->spinBox_7->setValue(0);
    ui->spinBox_8->setValue(0);

    ui->comboBox_8->insertItem(0, tr("Investigetion"));
    ui->comboBox_8->setItemData(1, QString::number(1));
    ui->comboBox_8->insertItem(1, tr("Strike"));
    ui->comboBox_8->setItemData(2, QString::number(2));

    ui->comboBox_9->insertItem(0, tr("XY-54"));
    ui->comboBox_9->setItemData(0, QString::number(1));
    ui->comboBox_9->insertItem(1, tr("XY"));
    ui->comboBox_9->setItemData(1, QString::number(2));

    ui->comboBox_5->insertItem(0, tr("Independent Control"));
    ui->comboBox_5->setItemData(0, QString::number(1));
    ui->comboBox_5->insertItem(1, tr("High Priority"));
    ui->comboBox_5->setItemData(1, QString::number(2));
    ui->comboBox_5->insertItem(2, tr("Diagonal Line Control"));
    ui->comboBox_5->setItemData(2, QString::number(3));

    ui->progressBar_2->hide();
    ui->label_22->hide();
    ui->label_23->hide();
    ui->label_24->hide();
    ui->label_25->hide();
    ui->label_26->hide();
    ui->label_27->hide();
}

void CDlgMissionLinePointEdit::SetWindowData(CNeubulaMissionLine stline)
{
    m_MissionLine = stline;
}

void CDlgMissionLinePointEdit::SetWindowStates(int nType)
{
    m_nType = nType;
}

void CDlgMissionLinePointEdit::TabWidgetConnet()
{
    connect(ui->tableWidget, SIGNAL(customContextMenuRequested(const QPoint)), this, SLOT(on_tableWidget_customContextMenuRequested(const QPoint)));

    connect(ui->pushButton_11, SIGNAL(clicked()), this, SLOT(on_pushButton_11_clicked()));
    connect(ui->pushButton_12, SIGNAL(clicked()), this, SLOT(on_pushButton_12_clicked()));
    connect(ui->pushButton_13, SIGNAL(clicked()), this, SLOT(on_pushButton_13_clicked()));
    connect(ui->pushButton_14, SIGNAL(clicked()), this, SLOT(on_pushButton_14_clicked()));
    connect(ui->pushButton_15, SIGNAL(clicked()), this, SLOT(on_pushButton_15_clicked()));
    connect(ui->pushButton_9, SIGNAL(clicked()), this, SLOT(on_pushButton_9_clicked()));
    connect(ui->pushButton_10, SIGNAL(clicked()), this, SLOT(on_pushButton_10_clicked()));
    connect(ui->pushButton_17, SIGNAL(clicked()), this, SLOT(on_pushButton_17_clicked()));
    connect(ui->pushButton_18, SIGNAL(clicked()), this, SLOT(on_pushButton_18_clicked()));

    connect(ui->comboBox_9, SIGNAL(currentIndexChanged(int)), this, SLOT(on_comboBox_9_currentIndexChanged(int)));
}

void CDlgMissionLinePointEdit::CreateTableMenu()
{
    m_TableMenu = new QMenu(this);
    QAction *Startuavroute = m_TableMenu->addAction(tr("Start Waypoint Design"));
    QAction *Stopuavroute = m_TableMenu->addAction(tr("Stop Waypoint Design"));
    QAction *Removepoint = m_TableMenu->addAction(tr("Delete Waypoints"));
    QAction *Insetpoint = m_TableMenu->addAction(tr("Insert Waypoints"));
    QAction *Removeallpoint = m_TableMenu->addAction(tr("Clear All Waypoints"));
    QAction *EditCodeSet = m_TableMenu->addAction(tr("Edit Instruction Set"));

    connect(Startuavroute, SIGNAL(triggered()), this, SLOT(OnStartuavroute()));
    connect(Stopuavroute, SIGNAL(triggered()), this, SLOT(OnStopuavroute()));
    connect(Removepoint, SIGNAL(triggered()), this, SLOT(OnRemovepoint()));
    connect(Insetpoint, SIGNAL(triggered()), this, SLOT(OnInsetpoint()));
    connect(Removeallpoint, SIGNAL(triggered()), this, SLOT(OnRemoveallpoint()));
    connect(EditCodeSet, SIGNAL(triggered()), this, SLOT(OnEditCodeSet()));
}

void CDlgMissionLinePointEdit::CreateListHeading()
{
    ui->tableWidget->clear();

    QStringList headerlabels;
    switch (m_nProtocolType)
    {
    case 1:
        headerlabels << tr("No.") << tr("Longitude") << tr("Latitude") << tr("Aititude") << tr("Distance of this Section") << tr("Course of Descent") << tr("Velocity") << tr("Hovering Time") << tr("Height Control") << tr("Correct Waypoint") << tr("Radius") << tr("Attributes") << tr("Turn a Corner");
        break;
    case 2:
        headerlabels << tr("No.") << tr("Longitude") << tr("Latitude") << tr("Aititude") << tr("Distance of this Section") << tr("Course of Descent") << tr("Velocity") << tr("Hovering Time") << tr("Height Control") << tr("Correct Waypoint") << tr("Reconnaissance Setup") << tr("Fire Setup");
        break;
    default:
        break;
    }

    ui->tableWidget->setColumnCount(headerlabels.size());
    ui->tableWidget->setHorizontalHeaderLabels(headerlabels);
    for (int i = 0; i < headerlabels.size(); i++)
    {
        if (i == 1 || i == 2 || i == 3 || i == 4)
        {
            ui->tableWidget->horizontalHeader()->setSectionResizeMode(i, QHeaderView::ResizeToContents);
        }
        else
        {
            ui->tableWidget->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Stretch);
        }
    }

    ui->tableWidget->verticalHeader()->setHidden(true);
    ui->tableWidget->setSelectionBehavior(QTableWidget::SelectRows);
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->setAlternatingRowColors(true);
    ui->tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
}

void CDlgMissionLinePointEdit::UpdateListData()
{
    if (m_nProtocolType == 1)
    {
        UpdateListData_XY54();
    }
    UpdateLineDistance();
}

void CDlgMissionLinePointEdit::UpdateListData_XY54()
{
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(0);

    int m_No = 0;
    auto itr = m_MissionLine.LinePointVec.begin();
    for (; itr != m_MissionLine.LinePointVec.end(); itr++)
    {
        UpdatePoint_XY54(*itr, m_No);
        UpdateLoaclDistance(m_No, 4);
        m_No++;
    }
}

void CDlgMissionLinePointEdit::UpdatePoint_XY54(const CNeubulaMissionLinePoint stPoint, const int nNo)
{
    ui->tableWidget->setRowCount(nNo + 1);

    double dLongitude = stPoint.dLon;
    double dLatitude = stPoint.dLat;
    int nheight = stPoint.dAlt;
    int nHeightCtrl = stPoint.cHeightCtrl;

    const QStringList list08 = { tr("Independent Control"), tr("High Priority"), tr("Diagonal Line Control"), tr("Retain") };
    const QStringList list10 = { tr("the Whole Route"), tr("A Single Route") };
    const QStringList list11 = { tr("Hovering Turn"), tr("Inside Cut Turn") };

    int nLandFlag = stPoint.cLanding;
    int nTaskFlag = 0;
    double dSpeed = stPoint.dSpeed;
    int nHoverTime = stPoint.dHoverTime;
    int nRadius = stPoint.DRadius;
    int nLineProPerTy = stPoint.cLineMode;
    int nTurnMode = stPoint.cTurnMode;

    QComboBox	*combobox08 = new QComboBox();
    QComboBox	*combobox10 = new QComboBox();
    QComboBox	*combobox11 = new QComboBox();
    combobox08->addItems(list08);
    combobox10->addItems(list10);
    combobox11->addItems(list11);

    QString No, Longitude, Latitude, height,  LandFlag, TaskFlag, Speed, HoverTime, strRadius;
    No = QString::number(nNo + 1);
    Longitude = QString::number(dLongitude, 'f', 7);
    Latitude = QString::number(dLatitude, 'f', 7);
    height = QString::number(nheight);
    switch (nHeightCtrl)
    {
    case CNebulaRoutePoint::EHeightCtrl::EHeightCtrl_Independent:
        combobox08->setCurrentIndex(0);
        break;
    case CNebulaRoutePoint::EHeightCtrl::EHeightCtrl_HightPreferred:
        combobox08->setCurrentIndex(1);
        break;
    case CNebulaRoutePoint::EHeightCtrl::EHeightCtrl_SlashCtrl:
        combobox08->setCurrentIndex(2);
        break;
    case CNebulaRoutePoint::EHeightCtrl::EHeightCtrl_Keep:
        combobox08->setCurrentIndex(3);
        break;
    default:
        break;
    }

    switch (nLandFlag)
    {
    case CNebulaRoutePoint::ELandFlag::ELandFlag_Normal:
        LandFlag = tr("Normal");
        break;
    case CNebulaRoutePoint::ELandFlag::ELandFlag_Land:
        LandFlag = tr("Point of Descent");
        break;
    default:
        break;
    }
    switch (nTaskFlag)
    {
    case CNebulaRoutePoint::ETaskFlag::ETaskFlag_Normal:
        TaskFlag = tr("Normal");
        break;
    case CNebulaRoutePoint::ETaskFlag::ETaskFlag_TakePicture:
        TaskFlag = tr("Photograph");
        break;
    default:
        break;
    }
    switch (nLineProPerTy)
    {
    case CNebulaRoutePoint::ELineProPerTy::ELineProPerTy_Whole:
        combobox10->setCurrentIndex(0);
        break;
    case CNebulaRoutePoint::ELineProPerTy::ELineProPerTy_Alone:
        combobox10->setCurrentIndex(1);
        break;
    default:
        break;
    }
    switch (nTurnMode)
    {
    case CNebulaRoutePoint::ETurnMode::ETurnMode_Precipice:
        combobox11->setCurrentIndex(0);
        break;
    case CNebulaRoutePoint::ETurnMode::ETaskFlag_Tangent:
        combobox11->setCurrentIndex(1);
        break;
    default:
        break;
    }

    Speed = QString::number(dSpeed);
    HoverTime = QString::number(nHoverTime);
    strRadius = QString::number(nRadius);

    QTableWidgetItem *item0 = new QTableWidgetItem(No);
    QTableWidgetItem *item1 = new QTableWidgetItem(Longitude);
    QTableWidgetItem *item2 = new QTableWidgetItem(Latitude);
    QTableWidgetItem *item3 = new QTableWidgetItem(height);

    QTableWidgetItem *item5 = new QTableWidgetItem(LandFlag);
    QTableWidgetItem *item6 = new QTableWidgetItem(Speed);
    QTableWidgetItem *item7 = new QTableWidgetItem(HoverTime);
    QTableWidgetItem *item9 = new QTableWidgetItem(TaskFlag);
    QTableWidgetItem *item10 = new QTableWidgetItem(strRadius);

    ui->tableWidget->setItem(nNo, 0, item0);
    ui->tableWidget->setItem(nNo, 1, item1);
    ui->tableWidget->setItem(nNo, 2, item2);
    ui->tableWidget->setItem(nNo, 3, item3);

    ui->tableWidget->setItem(nNo, 5, item5);
    ui->tableWidget->setItem(nNo, 6, item6);
    ui->tableWidget->setItem(nNo, 7, item7);

    ui->tableWidget->setCellWidget(nNo, 8, combobox08);

    ui->tableWidget->setItem(nNo, 9, item9);
    ui->tableWidget->setItem(nNo, 10, item10);
    ui->tableWidget->setCellWidget(nNo, 11, combobox10);
    ui->tableWidget->setCellWidget(nNo, 12, combobox11);

    ui->tableWidget->item(nNo, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tableWidget->item(nNo, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tableWidget->item(nNo, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tableWidget->item(nNo, 3)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    ui->tableWidget->item(nNo, 5)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tableWidget->item(nNo, 6)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tableWidget->item(nNo, 7)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    ui->tableWidget->item(nNo, 9)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tableWidget->item(nNo, 10)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    connect(combobox08, SIGNAL(currentIndexChanged(int)), this, SLOT(on_combobox_08_currentIndexChanged(int)));
    connect(combobox10, SIGNAL(currentIndexChanged(int)), this, SLOT(on_combobox_10_currentIndexChanged(int)));
    connect(combobox11, SIGNAL(currentIndexChanged(int)), this, SLOT(on_combobox_11_currentIndexChanged(int)));
}

void CDlgMissionLinePointEdit::UpdateLoaclDistance(const int nRow, const int nColum)
{
    QString localdistance = tr("0");
    double dlocaldistance = 0;
    if (nRow ==  0)
    {
        QTableWidgetItem *item4 = new QTableWidgetItem(localdistance);
        ui->tableWidget->setItem(nRow, nColum, item4);
        ui->tableWidget->item(nRow, nColum)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    }
    else
    {
        if (nRow < m_MissionLine.LinePointVec.size())
        {
            auto itr = m_MissionLine.LinePointVec.begin();
            double dOldLon, dOldLat, dLon, dLat;
            dOldLon = (itr + nRow - 1)->dLon;
            dOldLat = (itr + nRow - 1)->dLat;
            dLon = (itr + nRow)->dLon;
            dLat = (itr + nRow)->dLat;

            dlocaldistance = get_distance_from_lat_lon(dOldLat, dOldLon, dLat, dLon);
            localdistance = QString::number(dlocaldistance, 'f', 3);
            QTableWidgetItem *item4 = new QTableWidgetItem(localdistance);
            ui->tableWidget->setItem(nRow, nColum, item4);
            ui->tableWidget->item(nRow, nColum)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        }
    }
}

int CDlgMissionLinePointEdit::GetMaxPointNo()
{
    int nMax = 0;
    for (auto point : m_MissionLine.LinePointVec)
    {
        if (nMax < point.nNo)
        {
            nMax = point.nNo;
        }
    }

    return nMax + 1;
}

bool CDlgMissionLinePointEdit::AddLinePointMouseDoubleClick(double dLon, double dLat, double dHeight)
{
    CNeubulaMissionLinePoint stPoint;
    stPoint.nNo = GetMaxPointNo();
    stPoint.nLineID = ui->lineEdit->text().toInt();
    stPoint.dLon = dLon;
    stPoint.dLat = dLat;
    if (dHeight == 0)
    {
        dHeight = INIT_HEIGHT;
    }
    stPoint.dAlt = dHeight;
    m_MissionLine.LinePointVec.push_back(stPoint);

    if (m_nProtocolType == 1)
    {
        UpdatePoint_XY54(stPoint, m_MissionLine.LinePointVec.size() - 1);
        UpdateLoaclDistance(m_MissionLine.LinePointVec.size() - 1, 4);
    }
    UpdateLineDistance();

    LY_TaskManage::Get().GetDlgMissionLineList()->AddLinePoint(m_MissionLine.nID, m_MissionLine.strName, stPoint, m_MissionLine.strMissionID);
    return true;
}

bool CDlgMissionLinePointEdit::UpdateLinePointMouseMove(int nLineID, int nPointID, double dLon, double dLat)
{
    UpdateListLinePoint(nPointID, dLon, dLat);
    return true;
}

void CDlgMissionLinePointEdit::UpdateListLinePoint(int nPointID, double dLon, double dLat)
{
    int nNo = 0;
    for (auto &point : m_MissionLine.LinePointVec)
    {
        if (point.nNo == nPointID)
        {
            point.dLon = dLon;
            point.dLat = dLat;

            QTableWidgetItem *pItemLon = ui->tableWidget->item(nNo, 1);
            QTableWidgetItem *pItemLat = ui->tableWidget->item(nNo, 2);

            pItemLon->setText(QString::number(dLon, 'f', 7));
            pItemLat->setText(QString::number(dLat, 'f', 7));

            UpdateLoaclDistance(nNo, 4);
            break;

        }
        nNo++;
    }
}

void CDlgMissionLinePointEdit::UpdateCtrlData()
{
    if (m_pITaskManageService == nullptr)
    {
        return;
    }

    ui->comboBox_4->clear();
    int nNo = 0;
    ui->comboBox_4->insertItem(nNo, tr("-"));
    ui->comboBox_4->setItemData(nNo, tr("0"));
    CNeubulaMissionVec MissionVec = m_pITaskManageService->GetAllMission();
    for (auto Mission : MissionVec)
    {
        nNo++;
        ui->comboBox_4->insertItem(nNo, QString::fromStdString(Mission.GetName()));
        ui->comboBox_4->setItemData(nNo, QString::fromStdString(Mission.GetID()));
    }

    ui->comboBox_7->clear();
    nNo = 0;
    ui->comboBox_7->insertItem(nNo, tr("-"));
    ui->comboBox_7->setItemData(nNo, tr("0"));
    CNebulaMissionPlatformVec PlatformVec = m_pITaskManageService->GetAllMissionPlatform();
    for (auto Platfrom : PlatformVec)
    {
        nNo++;
        ui->comboBox_7->insertItem(nNo, QString::fromStdString(Platfrom.strMode));
        ui->comboBox_7->setItemData(nNo, QString::fromStdString(Platfrom.strID));
    }

    if (m_nType == 1)
    {
        ui->lineEdit->setText("");
        ui->lineEdit_2->setText("");

        ui->comboBox_4->setCurrentIndex(0);
        ui->comboBox_7->setCurrentIndex(0);
        ui->comboBox_8->setCurrentIndex(0);
        ui->comboBox_9->setCurrentIndex(0);

        QVariant  qv = ui->comboBox_9->itemData(0);
        m_MissionLine.cProtocolType = qv.toInt();
    }
    else if (m_nType == 2)
    {
        ui->lineEdit->setText(QString::number(m_MissionLine.nID));
        ui->lineEdit_2->setText(tr2(m_MissionLine.strName));

        QString strMissionID, strPlatfromID;

        GetShowInfoByLineID(m_MissionLine.nID, strMissionID, strPlatfromID);

        QString strType = QString::number(m_MissionLine.cType);
        QString strProtocol = QString::number(m_MissionLine.cProtocolType);

        int nIndex01 = ui->comboBox_4->findData(strMissionID);
        int nIndex02 = ui->comboBox_7->findData(strPlatfromID);
        int nIndex03 = ui->comboBox_8->findData(strType);
        int nIndex04 = ui->comboBox_9->findData(strProtocol);

        if (nIndex01 != -1)
        {
            ui->comboBox_4->setCurrentIndex(nIndex01);
        }
        if (nIndex02 != -1)
        {
            ui->comboBox_7->setCurrentIndex(nIndex02);
        }
        if (nIndex03 != -1)
        {
            ui->comboBox_8->setCurrentIndex(nIndex03);
        }
        if (nIndex04 != -1)
        {
            ui->comboBox_9->setCurrentIndex(nIndex04);
        }
    }
}

void CDlgMissionLinePointEdit::UpdateLineDistance()
{
    ui->label_20->setText("");
    if (m_nProtocolType == 1)
    {
        double dDis = 0;
        for (int i = 0; i < ui->tableWidget->rowCount(); i++)
        {
            QTableWidgetItem *pItem = ui->tableWidget->item(i, 4);
            if (pItem != nullptr)
            {
                QString strNo = pItem->text();
                dDis += strNo.toDouble();
            }
        }
        ui->label_20->setText(QString::number(dDis, 'f', 3));
    }
}

bool CDlgMissionLinePointEdit::GetShowInfoByLineID(const int nLineID, QString &strTaskID, QString &strPlatformID)
{
    if (m_pITaskManageService == nullptr)
    {
        return false;
    }
    CNebulaAcitonRelationVec RelationVec = m_pITaskManageService->GetActivatingShowData();
    for (auto data : RelationVec)
    {
        for (auto dataTask : data.ReTaskVec)
        {
            for (auto dataPlatform : dataTask.PlatFormVec)
            {
                if (QString::fromStdString(dataPlatform.strLineID).toInt() == nLineID)
                {
                    strTaskID = QString::fromStdString(dataTask.strTaskID);
                    strPlatformID = QString::fromStdString(dataPlatform.strPlatformID);
                    return true;
                }
            }
        }
    }
    return false;
}

EeditStates CDlgMissionLinePointEdit::GetLineEditStates()
{
    return m_EeditStates;
}

bool CDlgMissionLinePointEdit::CheckLineEdit()
{
    if (ui->lineEdit->text() == "")
    {
        LyMessageBox::information(LY_TaskManage::Get().getPWnd(), tr("tip"), tr("The route ID cannot be empty."));
        return false;
    }
    if (ui->lineEdit_2->text() == "")
    {
        LyMessageBox::information(LY_TaskManage::Get().getPWnd(), tr("tip"), tr("The route name cannot be empty."));
        return false;
    }
    return true;
}

void CDlgMissionLinePointEdit::showEvent(QShowEvent *event)
{
    UpdateCtrlData();

    if (m_nProtocolType != m_MissionLine.cProtocolType)
    {
        m_nProtocolType = m_MissionLine.cProtocolType;
        CreateListHeading();
    }
    UpdateListData();
}

void CDlgMissionLinePointEdit::on_comboBox_9_currentIndexChanged(int index)
{
    QVariant  qv = ui->comboBox_9->itemData(index);
    m_MissionLine.cProtocolType = qv.toInt();
}

void CDlgMissionLinePointEdit::on_pushButton_11_clicked()
{

}

void CDlgMissionLinePointEdit::on_pushButton_12_clicked()
{

}

void CDlgMissionLinePointEdit::on_pushButton_13_clicked()
{

}

void CDlgMissionLinePointEdit::on_pushButton_14_clicked()
{

}

void CDlgMissionLinePointEdit::on_pushButton_15_clicked()
{

}

void CDlgMissionLinePointEdit::on_pushButton_9_clicked()
{
    QString strName = ui->lineEdit_2->text();
    if (strName.length() > 24)
    {
        LyMessageBox::information(LY_TaskManage::Get().getPWnd(), tr("tip"), tr("Route name is too long, please reenter."));
        return;
    }

    if (m_nType == 2)
    {

        strncpy(m_MissionLine.strName, strName.toLocal8Bit(), MAX_LINE_LEN_1);
        m_pITaskManageService->EditMissionLine(m_MissionLine);

        LY_TaskManage::Get().GetDlgMissionLineList()->UpdataWindowData();
    }
    else if (m_nType == 1)
    {
        QString strID = ui->lineEdit->text();
        int nID = strID.toInt();
        m_MissionLine.nID = nID;
        strncpy(m_MissionLine.strName, strName.toLocal8Bit(), MAX_LINE_LEN_1);
        m_MissionLine.cShowFlag = true;
        m_MissionLine.cProtocolType = ui->comboBox_9->itemData(ui->comboBox_9->currentIndex()).toInt();
        m_pITaskManageService->AddMissionLine(m_MissionLine);

        QString strTaskID = "";
        if (ui->comboBox_4->currentIndex() != 0)
        {
            strTaskID = ui->comboBox_4->itemData(ui->comboBox_4->currentIndex()).toString();
        }

        QString strPlatFormID = "";
        if (ui->comboBox_7->currentIndex() != 0)
        {
            strPlatFormID = ui->comboBox_7->itemData(ui->comboBox_7->currentIndex()).toString();
        }

        CRelationActionTask stRelation;
        stRelation.SetID(m_pITaskManageService->GetRelationTaskMaxID());
        stRelation.SetActionID(m_pITaskManageService->GetActivatingActionID());
        stRelation.SetTaskID(strTaskID.toStdString());
        stRelation.SetPlatfromID(strPlatFormID.toStdString());
        stRelation.SetLineID(strID.toStdString());
        m_pITaskManageService->AddRelationTask(stRelation);

        LY_TaskManage::Get().GetDlgMissionLineList()->UpdataWindowData();
    }

    this->hide();
}

void CDlgMissionLinePointEdit::on_pushButton_10_clicked()
{
    this->hide();
    LY_TaskManage::Get().GetDlgMissionLineList()->UpdataWindowData();
}

void CDlgMissionLinePointEdit::on_tableWidget_customContextMenuRequested(const QPoint &pos)
{
    m_TableMenu->exec(QCursor::pos());
}

void CDlgMissionLinePointEdit::OnStartuavroute()
{
    if (CheckLineEdit() == false)
    {
        return;
    }
    m_EeditStates = PickLinePoint;

    if (m_nType == 1)
    {
        m_MissionLine.nID = ui->lineEdit->text().toInt();
        strncpy(m_MissionLine.strName, ui->lineEdit_2->text().toLocal8Bit(), MAX_LINE_LEN_1);
    }
}

void CDlgMissionLinePointEdit::OnStopuavroute()
{
    m_EeditStates = PickNormal;
}

void CDlgMissionLinePointEdit::OnRemovepoint()
{
    int nHitrow = ui->tableWidget->currentRow();

    if (nHitrow == -1)
    {
        LyMessageBox::information(LY_TaskManage::Get().getPWnd(), tr("tip"), tr("Select the waypoint you want to delete"));
        return;
    }

    QString strTmp = tr("Whether to delete waypoints:") + QString::number(nHitrow + 1);
    if (QMessageBox::StandardButton::Yes != LyMessageBox::question(this, tr("tip"), strTmp, QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel))
    {
        return;
    }

    auto itr = m_MissionLine.LinePointVec.begin();
    itr += nHitrow;

    if (itr >= m_MissionLine.LinePointVec.end())
    {
        return;
    }

    int nPoint = itr->nNo;
    m_MissionLine.LinePointVec.erase(itr);

    LY_TaskManage::Get().GetDlgMissionLineList()->RemoveLinePoint(m_MissionLine.nID, nPoint);

    UpdateListData();
}

void CDlgMissionLinePointEdit::OnInsetpoint()
{

}

void CDlgMissionLinePointEdit::OnRemoveallpoint()
{
    QString strTmp = tr("Delete them all?");
    if (QMessageBox::StandardButton::Yes != LyMessageBox::question(this, tr("tip"), strTmp, QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel))
    {
        return;
    }

    m_MissionLine.LinePointVec.clear();
    LY_TaskManage::Get().GetDlgMissionLineList()->RemoveLinePoint(m_MissionLine.nID, 0);

    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(0);
}

void CDlgMissionLinePointEdit::OnEditCodeSet()
{

}

void CDlgMissionLinePointEdit::on_tableWidget_cellChanged(int row, int column)
{

}

void CDlgMissionLinePointEdit::on_pushButton_17_clicked()
{
    int nMax = m_pITaskManageService->GetMissionLineMaxID();
    ui->lineEdit->setText(QString::number(nMax));
    ui->lineEdit_2->setText(QString::number(nMax));
}

void CDlgMissionLinePointEdit::on_pushButton_18_clicked()
{

}
