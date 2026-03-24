#include "WidgetLineEdit.h"

void QComboBox::wheelEvent(QWheelEvent *e)
{

}

WidgetLineEdit::WidgetLineEdit(QWidget *parent)
    : QWidget(parent),
      ui(new Ui::WidgetLineEdit)
{
    ui->setupUi(this);

    if (m_pITaskManageService == nullptr)
    {
        m_pITaskManageService = ITaskManagerActivator::Get().getPTaskManageService();
    }

    headerlabels_XY54 << tr("No.") << tr("Longitude") << tr("Latitude") << tr("Altitude") << tr("Distance of this Section") << tr("Course of Descent") << tr("Velocity")
                      << tr("Hovering Time") << tr("Height Control") << tr("Correct Waypoint") << tr("Radius") << tr("Attributes") << tr("Turn a Corner") << tr("Waypoint name") << tr("On time arrival time") << tr("Waypoint properties");

    initHeightCtr();

    connect(ui->lineEdit_LineName, SIGNAL(textChanged(QString)), this, SLOT(LineEditLineName_changed(QString)));
    connect(ui->pushButton_11, &QPushButton::clicked, this, &WidgetLineEdit::on_pB11_clicked);
    connect(ui->pushButton_12, &QPushButton::clicked, this, &WidgetLineEdit::on_pB12_clicked);
    connect(ui->pushButton_13, &QPushButton::clicked, this, &WidgetLineEdit::on_pB13_clicked);
    connect(ui->pushButton_14, &QPushButton::clicked, this, &WidgetLineEdit::on_pB14_clicked);
    connect(ui->pushButton_15, &QPushButton::clicked, this, &WidgetLineEdit::on_pB15_clicked);
    connect(ui->pushButtonIntoLine, &QPushButton::clicked, this, &WidgetLineEdit::on_LineName_Changed);
    connect(ui->pushButton_9, &QPushButton::clicked, this, &WidgetLineEdit::on_MarkSet_Clicked);

    m_pIMarkManageService = ITaskManagerActivator::Get().getPIMarkManageService();

    ui->label_4->hide();
    ui->lineEditPlatName->hide();
}

WidgetLineEdit::~WidgetLineEdit()
{
}

void WidgetLineEdit::setLineName(QString name)
{
    LineName = name;
    if (TaskLine.nID != 0)
    {
        strcpy(TaskLine.strName, name.toLocal8Bit().data());
    }
    ui->lineEdit_LineName->setText(LineName);
}

void WidgetLineEdit::setPlatName(const QString &platName)
{
    ui->lineEditPlatName->setText(platName);
}

void WidgetLineEdit::setPlatType(QString _platType)
{
    ui->lineEditPlatType->setText(_platType);
}

void WidgetLineEdit::setTaskLine(CNeubulaMissionLine linePoints)
{

    TaskLine = linePoints;
    setPlatName(QString::fromStdString(linePoints.nPlatId));

    setLineName(tr2(linePoints.strName));

    updataTableWidget();
}

int WidgetLineEdit::getLineID()
{
    return TaskLine.nID;
}

void WidgetLineEdit::updataTableWidget()
{
    initLineIntoItems();
    UpdateMarkCombobox();
    switch (1)
    {
    case 1:
    {
        UpdateListData_XY54();
        updateLineDis();
    }
    break;
    }

}

void WidgetLineEdit::UpdateListData_XY54()
{
    ui->tableWidget->clear();
    updateTableHeader_XY54();

    for (int i = 0; i < headerlabels_XY54.size(); i++)
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

    int m_No = 0;
    auto itr = TaskLine.LinePointVec.begin();
    for (; itr != TaskLine.LinePointVec.end(); itr++)
    {
        UpdatePoint_XY54(*itr, m_No);
        UpdateLoaclDistance(m_No, 4);
        m_No++;
    }
}

void WidgetLineEdit::updateTableHeader_XY54()
{
    ui->tableWidget->setColumnCount(headerlabels_XY54.size());
    ui->tableWidget->setHorizontalHeaderLabels(headerlabels_XY54);
    ui->tableWidget->verticalHeader()->setHidden(true);

    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);

    ui->tableWidget->setAlternatingRowColors(true);
    ui->tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(0);
}

void WidgetLineEdit::UpdatePoint_XY54(CNeubulaMissionLinePoint &stPoint, const int nNo)
{
    disconnect(ui->tableWidget, SIGNAL(itemChanged(QTableWidgetItem *)), this, SLOT(on_itemChanged(QTableWidgetItem *)));
    ui->tableWidget->setRowCount(nNo + 1);

    double dLongitude = stPoint.dLon;
    double dLatitude = stPoint.dLat;
    int nheight = stPoint.dAlt;
    int nHeightCtrl = stPoint.cHeightCtrl;

    int nPointType = stPoint.cType;
    QString cName = tr2(stPoint.cName);

    const QStringList list08 = { tr("Independent Control"), tr("High Priority"), tr("Diagonal Line Control"), tr("Retain") };
    const QStringList list10 = { tr("the Whole Route"), tr("A Single Route") };
    const QStringList list11 = { tr("Hovering Turn"), tr("Inside Cut Turn") };
    const QStringList list14 = { tr("Releasing point"), tr("Disc suspension point"), tr("Aiming point"), tr("Search point"), tr("Lauch point"), tr("Power on point"), tr("Return point") };

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
    QComboBox  *combobox14 = new QComboBox();
    combobox08->addItems(list08);
    combobox10->addItems(list10);
    combobox11->addItems(list11);
    combobox14->addItems(list14);

    QString No, Longitude, Latitude, height,  LandFlag, TaskFlag, Speed, HoverTime, strRadius, strName;
    No = QString::number(nNo + 1);
    Longitude = QString::number(dLongitude, 'f', 7);
    Latitude = QString::number(dLatitude, 'f', 7);
    height = QString::number(nheight);
    if (cName.isEmpty())
    {
        strName = tr("No") + QString::number(nNo + 1) + tr("Waypoint");
        strncpy(stPoint.cName, strName.toLocal8Bit(), 24);
    }
    else
    {
        strName = cName;
    }
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
    switch (nPointType)
    {
    case CNebulaRoutePoint::EPointType::EPointType_Adopt:
        combobox14->setCurrentIndex(0);
        break;
    case CNebulaRoutePoint::EPointType::EPointType_Hovering:
        combobox14->setCurrentIndex(1);
        break;
    case CNebulaRoutePoint::EPointType::EPointType_Aim:
        combobox14->setCurrentIndex(2);
        break;
    case CNebulaRoutePoint::EPointType::EPointType_Grope:
        combobox14->setCurrentIndex(3);
        break;
    case CNebulaRoutePoint::EPointType::EPointType_Transmit:
        combobox14->setCurrentIndex(4);
        break;
    case CNebulaRoutePoint::EPointType::EPointType_PowerOn:
        combobox14->setCurrentIndex(5);
        break;
    case CNebulaRoutePoint::EPointType::EPointType_Return:
        combobox14->setCurrentIndex(6);
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
    QTableWidgetItem *item13 = new QTableWidgetItem(strName);

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
    ui->tableWidget->setItem(nNo, 13, item13);
    ui->tableWidget->setCellWidget(nNo, 15, combobox14);

    ui->tableWidget->item(nNo, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    ui->tableWidget->item(nNo, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tableWidget->item(nNo, 3)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    ui->tableWidget->item(nNo, 5)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tableWidget->item(nNo, 6)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tableWidget->item(nNo, 7)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    ui->tableWidget->item(nNo, 9)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tableWidget->item(nNo, 10)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tableWidget->item(nNo, 13)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    connect(combobox08, &QComboBox::currentTextChanged, [&](QString nowStr)
    {
        QStringList list08 = { tr("Independent Control"), tr("High Priority"), tr("Diagonal Line Control"), tr("Retain") };
        int nowIndex = list08.indexOf(nowStr);
        stPoint.cHeightCtrl = nowIndex;
    });
    connect(combobox10, &QComboBox::currentTextChanged, [&](QString nowStr)
    {
        QStringList list10 = { tr("the Whole Route"), tr("A Single Route") };
        int nowIndex = list10.indexOf(nowStr);
        stPoint.cLineMode = nowIndex;
    });
    connect(combobox11, &QComboBox::currentTextChanged, [&](QString nowStr)
    {
        QStringList list11 = { tr("Hovering Turn"), tr("Inside Cut Turn") };
        int nowIndex = list11.indexOf(nowStr);
        stPoint.cTurnMode = nowIndex;
    });
    connect(combobox14, &QComboBox::currentTextChanged, [&](QString nowStr)
    {

        QStringList list14 = { tr("Releasing point"), tr("Disc suspension point"), tr("Aiming point"), tr("Search point"), tr("Lauch point"), tr("Power on point"), tr("Return point") };
        int nowIndex = list14.indexOf(nowStr);
        stPoint.cType = nowIndex;
    });
    connect(ui->tableWidget, SIGNAL(itemChanged(QTableWidgetItem *)), this, SLOT(on_itemChanged(QTableWidgetItem *)));
}

void WidgetLineEdit::UpdateLoaclDistance(const int nRow, const int nColum)
{
    QString localdistance = tr("0");
    QString sReachTime = tr2("T");
    double dlocaldistance = 0;
    if (nRow == 0)
    {
        QTableWidgetItem *item4 = new QTableWidgetItem(localdistance);
        ui->tableWidget->setItem(nRow, nColum, item4);
        ui->tableWidget->item(nRow, nColum)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        QTableWidgetItem *item14 = new QTableWidgetItem(sReachTime);
        ui->tableWidget->setItem(nRow, 14, item14);
        ui->tableWidget->item(nRow, 14)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    }
    else
    {
        if (nRow < TaskLine.LinePointVec.size())
        {
            auto itr = TaskLine.LinePointVec.begin();
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
            double dTempDis = ui->tableWidget->item(nRow, nColum)->text().toDouble();
            double dTempSpeed = ui->tableWidget->item(nRow, 6)->text().toDouble();
            double dHoverTime = ui->tableWidget->item(nRow - 1, 7)->text().toDouble();
            QString dTempReachTime = ui->tableWidget->item(nRow - 1, 14)->text();
            QStringList list = dTempReachTime.split("+");
            double dReachTime;
            if (list.size() == 1)
            {
                dReachTime = dTempDis / dTempSpeed + dHoverTime + 0;
            }
            else
            {
                dReachTime = dTempDis / dTempSpeed + dHoverTime + list[1].toDouble();
            }
            sReachTime = tr2("T+") + QString::number(dReachTime, 'f', 3);
            QTableWidgetItem *item14 = new QTableWidgetItem(sReachTime);
            ui->tableWidget->setItem(nRow, 14, item14);
            ui->tableWidget->item(nRow, 14)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        }
    }
}

void WidgetLineEdit::updateLineDis()
{
    ui->label_20->setText("");
    ui->label_6->setText("");

    double dDis = 0;
    for (int i = 0; i < ui->tableWidget->rowCount(); i++)
    {
        QTableWidgetItem *pItem = ui->tableWidget->item(i, 4);
        if (pItem != nullptr)
        {
            QString strNo = pItem->text();
            dDis += strNo.toDouble();
        }

        ui->label_20->setText(QString::number(dDis, 'f', 3));
    }

    ui->label_6->setText(QString::number(ui->tableWidget->rowCount()));
}

void WidgetLineEdit::initHeightCtr()
{
    ui->comboBox_5->clear();
    QStringList header;
    header.clear();
    header << tr("Independent Control") << tr("High Priority") << tr("Diagonal Line Control") << tr("Retain");
    ui->comboBox_5->addItems(header);
    ui->comboBox_5->setCurrentIndex(0);
}

void WidgetLineEdit::initLineIntoItems()
{
    if (m_pITaskManageService == NULL)
    {
        return;
    }
    std::vector<std::string> routeNames = m_pITaskManageService->getRouteLineNames();
    ui->comboBoxIntoLine->clear();
    for (int i = 0; i < routeNames.size(); i++)
    {
        ui->comboBoxIntoLine->addItem(QString::fromStdString(routeNames[i]));
    }
}

void WidgetLineEdit::delPointN(int N)
{
    if (N >= TaskLine.LinePointVec.size())
    {
        return;
    }
    CNeubulaMissionLinePointVec::iterator pointItor = TaskLine.LinePointVec.begin() + N;
    if (pointItor != TaskLine.LinePointVec.end())
    {
        pointItor = TaskLine.LinePointVec.erase(pointItor);
    }
    while (pointItor != TaskLine.LinePointVec.end())
    {
        pointItor->nNo--;
        pointItor++;
    }

    if (ui->tableWidget->rowCount() <= N)
    {
        return;
    }

    ui->tableWidget->removeRow(N);
    for (int index = N; index < ui->tableWidget->rowCount(); index++)
    {
        ui->tableWidget->item(index, 0)->setText(QString::number(index + 1));
    }

    LY_TaskManage::Get().GetDlgMissionLineList()->RemoveLinePoint(TaskLine.nID, N);

}

void WidgetLineEdit::clearPoints()
{
    ui->tableWidget->clear();
    TaskLine.LinePointVec.clear();
    updataTableWidget();
    updateLineDis();
    LY_TaskManage::Get().GetDlgMissionLineList()->ClearLinePoints(TaskLine.nID);

}

void WidgetLineEdit::addPointN(int N, double lon, double lat, double alt)
{
    CNeubulaMissionLinePoint newPoint;
    if (!TaskLine.LinePointVec.empty())
    {

        if (N == 0)
        {
            newPoint = TaskLine.LinePointVec[0];
        }
        else if (N <= TaskLine.LinePointVec.size())
        {
            newPoint = TaskLine.LinePointVec[N - 1];
        }
    }
    newPoint.nNo++;
    newPoint.nLineID = TaskLine.nID;
    newPoint.dLon = lon;
    newPoint.dLat = lat;
    newPoint.dAlt = alt;

    auto newPointItor = TaskLine.LinePointVec.begin() + N;
    if (newPointItor >= TaskLine.LinePointVec.end())
    {
        TaskLine.LinePointVec.push_back(newPoint);
    }
    else
    {
        newPointItor = TaskLine.LinePointVec.insert(newPointItor, newPoint);
        newPointItor++;
        while (newPointItor != TaskLine.LinePointVec.end())
        {
            newPointItor->nNo++;
            newPointItor++;
        }
    }
    LY_TaskManage::Get().GetDlgMissionLineList()->insertLinePointN(TaskLine.nID, tr2(TaskLine.strName), newPoint, N, tr2(TaskLine.strMissionID));

    tableWidgetInsertPoint(newPoint, N);
    UpdateLoaclDistance(N, 4);
    updateLineDis();
}

void WidgetLineEdit::updateMissionLineID(int _nID)
{
    TaskLine.nID = _nID;
    for (int i = 0; i < TaskLine.LinePointVec.size(); i++)
    {
        TaskLine.LinePointVec[i].nLineID = TaskLine.nID;
    }
}

void WidgetLineEdit::updateLinePointLonLat(int pointID, double Lon, double Lat)
{
    for (int i = 0; i < TaskLine.LinePointVec.size(); i++)
    {
        if (TaskLine.LinePointVec[i].nNo == pointID)
        {
            TaskLine.LinePointVec[i].dLon = Lon;
            TaskLine.LinePointVec[i].dLat = Lat;

            ui->tableWidget->item(i, 1)->setText(QString::number(Lon, 'f', 7));
            ui->tableWidget->item(i, 2)->setText(QString::number(Lat, 'f', 7));

            UpdateLoaclDistance(i, 4);
            if (i < TaskLine.LinePointVec.size() - 1)
            {
                UpdateLoaclDistance(i + 1, 4);
            }

            updateLineDis();

        }
    }
}

void WidgetLineEdit::tableWidgetInsertPoint(CNeubulaMissionLinePoint &stPoint, int nNo)
{
    disconnect(ui->tableWidget, SIGNAL(itemChanged(QTableWidgetItem *)), this, SLOT(on_itemChanged(QTableWidgetItem *)));

    ui->tableWidget->insertRow(nNo);

    nNo = nNo > ui->tableWidget->rowCount() ? ui->tableWidget->rowCount() : nNo;

    double dLongitude = stPoint.dLon;
    double dLatitude = stPoint.dLat;
    int nheight = stPoint.dAlt;
    int nHeightCtrl = stPoint.cHeightCtrl;

    int nPointType = stPoint.cType;
    QString cName = tr2(stPoint.cName);

    const QStringList list08 = { tr("Independent Control"), tr("High Priority"), tr("Diagonal Line Control"), tr("Retain") };
    const QStringList list10 = { tr("the Whole Route"), tr("A Single Route") };
    const QStringList list11 = { tr("Hovering Turn"), tr("Inside Cut Turn") };
    const QStringList list14 = { tr("Releasing point"), tr("Disc suspension point"), tr("Aiming point"), tr("Search point"), tr("Lauch point"), tr("Power on point"), tr("Return point") };
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
    QComboBox  *combobox14 = new QComboBox();
    combobox08->addItems(list08);
    combobox10->addItems(list10);
    combobox11->addItems(list11);
    combobox14->addItems(list14);

    QString No, Longitude, Latitude, height,  LandFlag, TaskFlag, Speed, HoverTime, strRadius, strName;
    No = QString::number(nNo + 1);
    Longitude = QString::number(dLongitude, 'f', 7);
    Latitude = QString::number(dLatitude, 'f', 7);
    height = QString::number(nheight);
    if (cName.isEmpty())
    {
        strName = tr("No") + QString::number(nNo + 1) + tr("Waypoint");
        strncpy(stPoint.cName, strName.toStdString().c_str(), 24);
    }
    else
    {
        strName = cName;
    }
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
    switch (nPointType)
    {
    case CNebulaRoutePoint::EPointType::EPointType_Adopt:
        combobox14->setCurrentIndex(0);
        break;
    case CNebulaRoutePoint::EPointType::EPointType_Hovering:
        combobox14->setCurrentIndex(1);
        break;
    case CNebulaRoutePoint::EPointType::EPointType_Aim:
        combobox14->setCurrentIndex(2);
        break;
    case CNebulaRoutePoint::EPointType::EPointType_Grope:
        combobox14->setCurrentIndex(3);
        break;
    case CNebulaRoutePoint::EPointType::EPointType_Transmit:
        combobox14->setCurrentIndex(4);
        break;
    case CNebulaRoutePoint::EPointType::EPointType_PowerOn:
        combobox14->setCurrentIndex(5);
        break;
    case CNebulaRoutePoint::EPointType::EPointType_Return:
        combobox14->setCurrentIndex(6);
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
    QTableWidgetItem *item13 = new QTableWidgetItem(strName);

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
    ui->tableWidget->setItem(nNo, 13, item13);
    ui->tableWidget->setCellWidget(nNo, 15, combobox14);

    ui->tableWidget->item(nNo, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tableWidget->item(nNo, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tableWidget->item(nNo, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tableWidget->item(nNo, 3)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    ui->tableWidget->item(nNo, 5)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tableWidget->item(nNo, 6)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tableWidget->item(nNo, 7)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    ui->tableWidget->item(nNo, 9)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tableWidget->item(nNo, 10)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tableWidget->item(nNo, 13)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    connect(combobox08, &QComboBox::currentTextChanged, this, &WidgetLineEdit::on_heigheCtrl_changed);
    connect(combobox10, &QComboBox::currentTextChanged, this, &WidgetLineEdit::on_character_changed);
    connect(combobox11, &QComboBox::currentTextChanged, this, &WidgetLineEdit::on_trans_changed);
    connect(combobox14, &QComboBox::currentTextChanged, [&](QString nowStr)
    {
        QStringList list14 = { tr("Releasing point"), tr("Disc suspension point"), tr("Aiming point"), tr("Search point"), tr("Lauch point"), tr("Power on point"), tr("Return point") };
        int nowIndex = list14.indexOf(nowStr);
        stPoint.cType = nowIndex;
    });

    for (int index = nNo; index < ui->tableWidget->rowCount(); index++)
    {
        ui->tableWidget->item(index, 0)->setText(QString::number(index + 1));
    }
    connect(ui->tableWidget, SIGNAL(itemChanged(QTableWidgetItem *)), this, SLOT(on_itemChanged(QTableWidgetItem *)));
}

bool WidgetLineEdit::GetShowInfoByLineID(const int nLineID, QString &strTaskID, QString &strPlatformID)
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

void WidgetLineEdit::UpdateMarkCombobox()
{
    if (m_pIMarkManageService == nullptr)
    {
        return;
    }
    CSystemMarkInfoVec infoVec;

    if (m_pITaskManageService != nullptr)
    {
        int activatingActionID = -1;
        activatingActionID = m_pITaskManageService->GetActivatingActionID();
        CRelationActionMarkVec RelationActionMarkVec = m_pITaskManageService->GetRelationMark(activatingActionID);
        for (auto data : RelationActionMarkVec)
        {
            SystemMarkInfo stinfo = m_pIMarkManageService->GetMark(data.GetMarkID());
            if (stinfo.GetID() != "")
            {
                infoVec.push_back(stinfo);
            }
        }
    }

    if (infoVec.size() == 0)
    {
        infoVec = m_pIMarkManageService->GetMarkList();
    }

    ui->comboBox_4->disconnect();
    ui->comboBox_4->clear();

    QStringList strList;
    strList.push_back(tr("-"));
    for (auto data : infoVec)
    {
        strList.push_back(QString::fromStdString(data.GetID()));
    }

    ui->comboBox_4->addItems(strList);

}

std::string WidgetLineEdit::GetMarkIDString()
{
    return (ui->comboBox_4->currentText()).toStdString();
}

std::string WidgetLineEdit::GetPlatTypeString()
{
    return (ui->lineEditPlatType->text()).toStdString();
}

void WidgetLineEdit::setHoveringTime(int _index, int _time)
{
    int rowCount = ui->tableWidget->rowCount();
    int colum = headerlabels_XY54.indexOf(tr("Hovering Time"));
    if (_index < rowCount && _index >= 0)
    {
        auto tmpItem = ui->tableWidget->item(_index, colum);
        tmpItem->setText(QString::number(_time));
    }
}

void WidgetLineEdit::LineEditLineName_changed(QString newName)
{
    LineName = newName;
    strcpy(TaskLine.strName, LineName.toLocal8Bit().data());
    if (LY_TaskManage::Get().GetDlgMissionLineList() != NULL)
    {
        LY_TaskManage::Get().GetDlgMissionLineList()->updateLineName(TaskLine.nID, LineName);
    }

}

void WidgetLineEdit::on_pB11_clicked()
{
    QString boxText = ui->spinBox_5->text();

    int rowCount = ui->tableWidget->rowCount();

    int colum = headerlabels_XY54.indexOf(tr("Altitude"));

    for (int i = 0; i < rowCount; i++)
    {
        auto tmpItem = ui->tableWidget->item(i, colum);
        tmpItem->setText(boxText);
    }
    double Boxheight = boxText.toDouble();

    int pointCount = TaskLine.LinePointVec.size();
    for (int i = 0; i < pointCount; i++)
    {
        TaskLine.LinePointVec[i].dAlt = Boxheight;
    }

}

void WidgetLineEdit::on_pB12_clicked()
{
    QString boxText = ui->spinBox_6->text();

    int rowCount = ui->tableWidget->rowCount();
    int colum = headerlabels_XY54.indexOf(tr("Velocity"));
    for (int i = 0; i < rowCount; i++)
    {
        auto tmpItem = ui->tableWidget->item(i, colum);
        tmpItem->setText(boxText);
    }
    double Boxheight = boxText.toDouble();

    int pointCount = TaskLine.LinePointVec.size();
    for (int i = 0; i < pointCount; i++)
    {
        TaskLine.LinePointVec[i].dSpeed = Boxheight;
    }
}

void WidgetLineEdit::on_pB13_clicked()
{
    QString boxText = ui->spinBox_7->text();

    int rowCount = ui->tableWidget->rowCount();
    int colum = headerlabels_XY54.indexOf(tr("Hovering Time"));
    for (int i = 0; i < rowCount; i++)
    {
        auto tmpItem = ui->tableWidget->item(i, colum);
        tmpItem->setText(boxText);
    }
    double Boxheight = boxText.toDouble();

    int pointCount = TaskLine.LinePointVec.size();
    for (int i = 0; i < pointCount; i++)
    {
        TaskLine.LinePointVec[i].dHoverTime = Boxheight;
    }
}

void WidgetLineEdit::on_pB14_clicked()
{
    int curIndex = ui->comboBox_5->currentIndex();
    int rowCount = ui->tableWidget->rowCount();
    int colum = headerlabels_XY54.indexOf(tr("Height Control"));
    for (int i = 0; i < rowCount; i++)
    {
        auto tmpItem = (QComboBox *)(ui->tableWidget->cellWidget(i, colum));
        tmpItem->setCurrentIndex(curIndex);
    }

    int pointCount = TaskLine.LinePointVec.size();
    for (int i = 0; i < pointCount; i++)
    {
        TaskLine.LinePointVec[i].cHeightCtrl = curIndex;
    }
}

void WidgetLineEdit::on_pB15_clicked()
{
}

void WidgetLineEdit::on_LineName_Changed()
{
    QString lineName = ui->comboBoxIntoLine->currentText();
    std::vector<QVector3D> points = m_pITaskManageService->getRouteLinePointsForName(lineName.toStdString());

    clearPoints();
    for (int count = 0; count < points.size(); count++)
    {
        addPointN(count, points[count].y(), points[count].z());
    }

}

void WidgetLineEdit::on_heigheCtrl_changed()
{
    auto selectedItems = ui->tableWidget->selectedItems();
    if (!selectedItems.empty())
    {
        int selectedRow = selectedItems.at(0)->row();
        int colum = headerlabels_XY54.indexOf(tr("Height Control"));

        auto tmpItem = (QComboBox *)(ui->tableWidget->cellWidget(selectedRow, colum));
        int comBoxIndex = tmpItem->currentIndex();
        TaskLine.LinePointVec[selectedRow].cHeightCtrl = comBoxIndex;
    }
}

void WidgetLineEdit::on_character_changed()
{
    auto selectedItems = ui->tableWidget->selectedItems();
    if (!selectedItems.empty())
    {
        int selectedRow = selectedItems.at(0)->row();
        int colum = headerlabels_XY54.indexOf(tr("Attributes"));

        auto tmpItem = (QComboBox *)(ui->tableWidget->cellWidget(selectedRow, colum));
        int comBoxIndex = tmpItem->currentIndex();
        TaskLine.LinePointVec[selectedRow].cLineMode = comBoxIndex;
    }
}

void WidgetLineEdit::on_trans_changed()
{
    auto selectedItems = ui->tableWidget->selectedItems();
    if (!selectedItems.empty())
    {
        int selectedRow = selectedItems.at(0)->row();
        int colum = headerlabels_XY54.indexOf(tr("Turn a Corner"));

        auto tmpItem = (QComboBox *)(ui->tableWidget->cellWidget(selectedRow, colum));
        int comBoxIndex = tmpItem->currentIndex();
        TaskLine.LinePointVec[selectedRow].cTurnMode = comBoxIndex;
    }
}

void WidgetLineEdit::on_itemChanged(QTableWidgetItem *_item)
{
    int selRow = _item->row();
    int selCol = _item->column();
    QString selText = _item->text();

    switch (selCol)
    {
    case 1:
    {
        TaskLine.LinePointVec[selRow].dLon = selText.toDouble();
        LY_TaskManage::Get().GetDlgMissionLineList()->UpdateLinePointMouseMove(TaskLine.nID, TaskLine.LinePointVec[selRow].nNo,
                                                                               TaskLine.LinePointVec[selRow].dLon, TaskLine.LinePointVec[selRow].dLat);
        updataTableWidget();
    }
    break;
    case 2:
    {
        TaskLine.LinePointVec[selRow].dLat = selText.toDouble();
        LY_TaskManage::Get().GetDlgMissionLineList()->UpdateLinePointMouseMove(TaskLine.nID, TaskLine.LinePointVec[selRow].nNo,
                                                                               TaskLine.LinePointVec[selRow].dLon, TaskLine.LinePointVec[selRow].dLat);
        updataTableWidget();
    }
    break;
    case 3:
        TaskLine.LinePointVec[selRow].dAlt = selText.toDouble();
        break;
    case 6:
        TaskLine.LinePointVec[selRow].dSpeed = selText.toDouble();
        updataTableWidget();
        break;
    case 7:
        TaskLine.LinePointVec[selRow].dHoverTime = selText.toDouble();
        updataTableWidget();
        break;
    case 10:
        TaskLine.LinePointVec[selRow].DRadius = selText.toDouble();
        break;
    case 13:
        strncpy(TaskLine.LinePointVec[selRow].cName, selText.toLocal8Bit(), 24);

        break;
    case 14:
        strncpy(TaskLine.LinePointVec[selRow].dReachTime, selText.toLocal8Bit(), 24);

        break;

    }
}

void WidgetLineEdit::on_MarkSet_Clicked()
{

    CDlgAutoPoint *pDlgAutoPoint = LY_TaskManage::Get().GetDlgAutoPoint();
    if ( pDlgAutoPoint)
    {
        pDlgAutoPoint->SetEditLinePoint(true);
        pDlgAutoPoint->SetUpdatePositionAndMarkIDCal(true);
        pDlgAutoPoint->show();
    }
}

