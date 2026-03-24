#include "CDlgActionEdit.h"
#include "ui_CDlgActionEdit.h"
#include "QCString.h"
#include "ITaskManagerActivator.h"
#include "lY_TaskManage.h"
#include "QDateTime"
#include "QDateTimeEdit"
#include "QRegExpExample.h"

CDlgActionEdit::CDlgActionEdit(QWidget *parent) :
    LyDialog(tr("Create Action"), parent),
    ui(new Ui::CDlgActionEdit)
{
    ui->setupUi(centralWidget());
    this->setResizable(true);
    this->resize(1260, 1080);
    ui->tbwArea->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->tbwMark->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->tbwPlatResource->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->tbwSensorResource->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->tbwAmmoResource->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->tbwOtherResource->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(ui->pbtnCancel, &QPushButton::clicked, this, &CDlgActionEdit::on_pbtnCancel_clicked);
    connect(ui->pbtnOK, &QPushButton::clicked, this, &CDlgActionEdit::on_pbtnOK_clicked);
    connect(ui->tbwArea, &QTableWidget::itemDoubleClicked, this, &CDlgActionEdit::on_tbwArea_itemDoubleClicked);
    connect(ui->tbwArea_2, &QTableWidget::itemDoubleClicked, this, &CDlgActionEdit::on_tbwArea_2_itemDoubleClicked);

    connect(ui->tbwMark, &QTableWidget::itemDoubleClicked, this, &CDlgActionEdit::on_tbwMark_itemDoubleClicked);
    connect(ui->tbwMark_2, &QTableWidget::itemDoubleClicked, this, &CDlgActionEdit::on_tbwMark_2_itemDoubleClicked);

    connect(ui->tbwPlatResource, &QTableWidget::itemDoubleClicked, this, &CDlgActionEdit::on_tbwPlatResource_itemDoubleClicked);
    connect(ui->tbwPlatResource_2, &QTableWidget::itemDoubleClicked, this, &CDlgActionEdit::on_tbwPlatResource_2_itemDoubleClicked);

    connect(ui->tbwSensorResource, &QTableWidget::itemDoubleClicked, this, &CDlgActionEdit::on_tbwSensorResource_itemDoubleClicked);
    connect(ui->tbwSensorResource_2, &QTableWidget::itemDoubleClicked, this, &CDlgActionEdit::on_tbwSensorResource_2_itemDoubleClicked);

    connect(ui->tbwAmmoResource, &QTableWidget::itemDoubleClicked, this, &CDlgActionEdit::on_tbwAmmoResource_itemDoubleClicked);
    connect(ui->tbwAmmoResource_2, &QTableWidget::itemDoubleClicked, this, &CDlgActionEdit::on_tbwAmmoResource_2_itemDoubleClicked);

    connect(ui->tbwOtherResource, &QTableWidget::itemDoubleClicked, this, &CDlgActionEdit::on_tbwOtherResource_itemDoubleClicked);
    connect(ui->tbwOtherResource_2, &QTableWidget::itemDoubleClicked, this, &CDlgActionEdit::on_tbwOtherResource_2_itemDoubleClicked);

    connect(ui->pbtnLeft_0, &QPushButton::clicked, this, &CDlgActionEdit::on_pbtnLeft_0_Clicked);
    connect(ui->pbtnRight_0, &QPushButton::clicked, this, &CDlgActionEdit::on_pbtnRight_0_Clicked);

    connect(ui->pbtnLeft_1, &QPushButton::clicked, this, &CDlgActionEdit::on_pbtnLeft_1_Clicked);
    connect(ui->pbtnRight_1, &QPushButton::clicked, this, &CDlgActionEdit::on_pbtnRight_1_Clicked);

    connect(ui->pbtnLeft_2, &QPushButton::clicked, this, &CDlgActionEdit::on_pbtnLeft_2_Clicked);
    connect(ui->pbtnRight_2, &QPushButton::clicked, this, &CDlgActionEdit::on_pbtnRight_2_Clicked);

    connect(ui->pbtnLeft_3, &QPushButton::clicked, this, &CDlgActionEdit::on_pbtnLeft_3_Clicked);
    connect(ui->pbtnRight_3, &QPushButton::clicked, this, &CDlgActionEdit::on_pbtnRight_3_Clicked);

    connect(ui->pbtnLeft_4, &QPushButton::clicked, this, &CDlgActionEdit::on_pbtnLeft_4_Clicked);
    connect(ui->pbtnRight_4, &QPushButton::clicked, this, &CDlgActionEdit::on_pbtnRight_4_Clicked);

    connect(ui->pbtnLeft_5, &QPushButton::clicked, this, &CDlgActionEdit::on_pbtnLeft_5_Clicked);
    connect(ui->pbtnRight_5, &QPushButton::clicked, this, &CDlgActionEdit::on_pbtnRight_5_Clicked);

    connect(ui->btnAddMark, &QPushButton::clicked, this, &CDlgActionEdit::on_btnAddMark_clicked);

    connect(ui->pbtnAddTask, &QPushButton::clicked, this, &CDlgActionEdit::on_btnAddTask_clicked);

    ui->lineEdit_ActionName->setValidator(new QRegExpValidator(regNumStrUnderLine));
    ui->lineEdit_ActionCode->setValidator(new QRegExpValidator(regNumStrUnderLine));
}

CDlgActionEdit::~CDlgActionEdit()
{
    delete ui;
}

void CDlgActionEdit::Init()
{

    ActionID = ITaskManagerActivator::Get().getPTaskManageService()->GetActivatingActionID();
    ITaskManagerActivator::Get().getPTaskManageService()->ReLoadRelationMarkDB();
    ActionInfo = ITaskManagerActivator::Get().getPTaskManageService()->GetNebulaAction(ActionID);
    m_MarkIDVec = ActionInfo.GetMarkIDList();

    CNeubulaActionVec = ITaskManagerActivator::Get().getPTaskManageService()->GetAllNebulaAction();
    MaxActionID = GetMaxActionID();

    targetInfoVec = GetMarkList();

    airspaceInfoVec.clear();
    airspaceInfoVec = GetActionAreaInfoList();

    platResourceVec = GetPlafFromInfoList();

    SensorResourceVec = GetSensorInfoList();

    AmmoResourceVec = GetAmmoInfiList();

    ui->lineEdit_ActionID->setText(QString::number(MaxActionID + 1));
    ActionID = ui->lineEdit_ActionID->text().toInt();

    ui->ledtVisibility->setText("100");
    ui->lineEdit_ActionName->clear();
    ui->lineEdit_ActionCode->clear();
    ui->textEdit_ActionNote->clear();
    ui->textEdit_ActionMark->clear();
    ui->cbxActiveState->setCheckState(Qt::Unchecked);

    QString str = ActionInfo.GetInfo().c_str();
    QStringList strInfo = str.split(";");
    if (strInfo.size() > 0)
    {
        ui->cbxSeason->setCurrentText(strInfo[0]);
    }
    if (strInfo.size() > 1)
    {
        ui->cbxDayNight->setCurrentText(strInfo[1]);
    }
    if (strInfo.size() > 2)
    {
        ui->ledtVisibility->setText(strInfo[2]);
    }
    if (strInfo.size() > 3)
    {
        ui->cbxWeather->setCurrentText(strInfo[3]);
    }
    if (strInfo.size() > 4)
    {
        QStringList wind = strInfo[4].split("-");
        if (wind.size() > 0)
        {
            ui->cbxWindType->setCurrentText(wind[0]);
        }
        if (wind.size() > 1)
        {
            ui->cbxWindLevel->setCurrentText(wind[1]);
        }
    }
    QString startTime = QString::fromStdString(ActionInfo.GetStartTime().c_str());
    QDateTime dataTimeBegin = QDateTime::fromString(startTime, "yyyy/M/d HH:mm");
    ui->dateTimeEdit_Begin->setDateTime(dataTimeBegin);
    QString finishTime = QString::fromStdString(ActionInfo.GetFinishTime().c_str());

    QDateTime dataTimeEnd = QDateTime::fromString(finishTime, "yyyy/M/d HH:mm");
    ui->dateTimeEdit_End->setDateTime(dataTimeEnd);

    ActionStartTime = ui->dateTimeEdit_Begin->text().toStdString();
    ActionEndTime = ui->dateTimeEdit_End->text().toStdString();
    Season = ui->cbxSeason->currentText().toStdString();
    DayorNight = ui->cbxDayNight->currentText().toStdString();
    Weather = ui->cbxWeather->currentText().toStdString();
    WindType = ui->cbxWindType->currentText().toStdString();
    WindLevel = ui->cbxWindLevel->currentText().toStdString();
    Visibility = ui->ledtVisibility->text().toStdString();

    selectedPlatVec.clear();
    selectedSensorVec.clear();
    selectedAmmoVec.clear();
    selectedOtherVec.clear();
    selectedAreaVec.clear();
    selectedMarkVec.clear();

    CreateAllTable();

    Update_tbwArea();
    UpdateSeleAreaTab();
    Update_tbwMark();
    UpdateMark_2();

    Update_tbwPlatResource();
    Update_tbwSensorResource();
    Update_tbwAmmoResource();
    Update_tbwOtherResource();

    connect(ui->lineEdit_ActionID, &QLineEdit::editingFinished, this, &CDlgActionEdit::GetTextFromLedtID);
    connect(ui->lineEdit_ActionName, &QLineEdit::editingFinished, this, &CDlgActionEdit::GetTextFromLedtName);
    connect(ui->lineEdit_ActionCode, &QLineEdit::editingFinished, this, &CDlgActionEdit::GetTextFromLedtDH);
    connect(ui->textEdit_ActionMark, &QTextEdit::textChanged, this, &CDlgActionEdit::GetTextFromLedtAim);
    connect(ui->textEdit_ActionNote, &QTextEdit::textChanged, this, &CDlgActionEdit::GetTextFromLedtRemark);
    connect(ui->ledtVisibility, &QLineEdit::editingFinished, this, &CDlgActionEdit::GetTextFromLedtVisibility);

    connect(ui->cbxSeason, &QComboBox::currentTextChanged, this, &CDlgActionEdit::GetTextFromcbxSeason);
    connect(ui->cbxDayNight, &QComboBox::currentTextChanged, this, &CDlgActionEdit::GetTextFromcbxDayNight);
    connect(ui->cbxWeather, &QComboBox::currentTextChanged, this, &CDlgActionEdit::GetTextFromcbxWeather);
    connect(ui->cbxWindType, &QComboBox::currentTextChanged, this, &CDlgActionEdit::GetTextFromcbxWindType);
    connect(ui->cbxWindLevel, &QComboBox::currentTextChanged, this, &CDlgActionEdit::GetTextFromcbxWindLevel);

    connect(ui->dateTimeEdit_Begin, &QDateTimeEdit::dateTimeChanged, this, &CDlgActionEdit::GetTextFromDateTimeStart);
    connect(ui->dateTimeEdit_End, &QDateTimeEdit::dateTimeChanged, this, &CDlgActionEdit::GetTextFromDateTimeEnd);

    connect(ui->cbxActiveState, &QCheckBox::stateChanged, this, &CDlgActionEdit::GetStateFromCbxActive);
}

void CDlgActionEdit::SplitPlatLoadByType()
{
    if (!AllLoadInfoVec.empty())
    {
        for (auto uavload : AllLoadInfoVec)
        {
            if (uavload.eType == EMissionPlatformLoadType_DC)
            {
                LoadPodVec.push_back(uavload);
            }
            else if (uavload.eType == EMissionPlatformLoadType_DY)
            {
                LoadAmmoVec.push_back(uavload);
            }
            else if (uavload.eType == EMissionPlatformLoadType_TX)
            {
                LoadCommVec.push_back(uavload);
            }
        }
    }
}

int CDlgActionEdit::GetMaxActionID()
{
    int tempInt = 0;

    if (!CNeubulaActionVec.empty())
    {
        for (auto maction : CNeubulaActionVec)
        {
            if (tempInt <= maction.GetID())
            {
                tempInt = maction.GetID();
            }
        }
    }
    return tempInt;
}

std::vector<SystemMarkInfo> CDlgActionEdit::GetTaegetInfoByIDList()
{
    std::vector<SystemMarkInfo>tmpVec;
    if (!m_MarkIDVec.empty())
    {
        for (auto tar_id : m_MarkIDVec)
        {
            SystemMarkInfo tmpTar = ITaskManagerActivator::Get().getPIMarkManageService()->GetMark(tar_id);
            tmpVec.push_back(tmpTar);
        }
    }
    targetInfoVec = tmpVec;
    return tmpVec;
}

std::vector<SystemMarkInfo> CDlgActionEdit::GetMarkList()
{
    std::vector<SystemMarkInfo>tmp;
    tmp = ITaskManagerActivator::Get().getPIMarkManageService()->GetMarkList();
    return tmp;
}

std::vector<CZoneInfo> CDlgActionEdit::GetActionAreaInfoList()
{
    std::vector<CZoneInfo>  tmpVec = ITaskManagerActivator::Get().getPIMarkManageService()->GetZoneList();

    return tmpVec;
}

NebulaResourcesVec CDlgActionEdit::GetPlafFromInfoList()
{
    NebulaResourcesVec tmp;
    tmp = ITaskManagerActivator::Get().getPTaskManageService()->GetPlatFormResourceFmDB();
    return tmp;
}

NebulaResourcesVec CDlgActionEdit::GetSensorInfoList()
{
    NebulaResourcesVec tmp;
    tmp = ITaskManagerActivator::Get().getPTaskManageService()->GetSensorResourceFmDB();
    return tmp;
}

NebulaResourcesVec CDlgActionEdit::GetAmmoInfiList()
{
    NebulaResourcesVec tmp;
    tmp = ITaskManagerActivator::Get().getPTaskManageService()->GetAmmoResourceFmDB();
    return tmp;
}

bool CDlgActionEdit::IsMarkInRegion(SystemMarkInfo target)
{
    if (!targetInfoVec.empty())
    {
        QPointF pos(target.GetLon(), target.GetLat());
        if (selectedAreaVec.empty())
        {
            return false;
        }
        for (auto mArea : selectedAreaVec)
        {
            auto areaPoints = SetAreaToQPointF(mArea.GetPoints());
            if (is_point_in_zone(pos, areaPoints))
            {
                return true;
            }
        }
    }
    return false;
}

bool CDlgActionEdit::is_point_in_zone(const QPointF point, const std::vector<QPointF> ZonePoints)
{
    int count = ZonePoints.size();
    int nCross = 0;
    for (int i = 0; i < count; i++)
    {
        QPointF p1 = ZonePoints[i];
        QPointF p2 = ZonePoints[(i + 1) % count];

        if (p1.y() == p2.y())
        {
            continue;
        }

        if (point.y() < min(p1.y(), p2.y()))
        {
            continue;
        }

        if (point.y() >= max(p1.y(), p2.y()))
        {
            continue;
        }

        double x = (point.y() - p1.y()) * (p2.x() - p1.x()) / (p2.y() - p1.y()) + p1.x();

        if (x > point.x())
        {
            nCross++;
        }
    }

    return (nCross % 2 == 1);
}

std::vector<QPointF> CDlgActionEdit::SetAreaToQPointF(ZonePointVec pointVec)
{
    std::vector<QPointF> temp;
    if (pointVec.empty())
    {
        return temp;
    }

    for (auto point : pointVec)
    {
        temp.push_back(QPointF(point.dLon, point.dLat));
    }
    return temp;
}

void CDlgActionEdit::UpdateCurrAction(CNeubulaAction Action)
{
    int m_id = Action.GetID();
    ui->lineEdit_ActionID->setText(QString::number(m_id));
    ActionID = m_id;

    QString m_name = QString::fromStdString(Action.GetName());
    ui->lineEdit_ActionName->setText(m_name);

    QString m_code = QString::fromStdString(Action.GetCode());
    ui->lineEdit_ActionCode->setText(m_code);

    if (Action.GetActivateStates() == true)
    {
        ui->cbxActiveState->setCheckState(Qt::Checked);
    }

    const QString &m_aim = QString::fromStdString(Action.GetObject());
    ui->textEdit_ActionMark->setText(m_aim);

    QString m_remark = QString::fromStdString(Action.GetRemark());
    ui->textEdit_ActionNote->setText(m_remark);

    QString m_Area = QString::fromStdString(Action.GetAreaID());
    QStringList mlist = m_Area.split(';');
    for (auto area : mlist)
    {
        for (auto airInfo : airspaceInfoVec)
        {
            if (airInfo.GetID().c_str() == area)
            {
                selectedAreaVec.emplace_back(airInfo);
            }
        }
    }
    Update_tbwArea();
    UpdateSeleAreaTab();

    m_MarkIDVec = Action.GetMarkIDList();

    if (!selectedAreaVec.empty())
    {
        for (auto tar : m_MarkIDVec)
        {
            for (auto tarInfo : targetInfoVec)
            {
                if (tarInfo.GetID().c_str() == tar)
                {
                    selectedMarkVec.emplace_back(tarInfo);
                }
            }
        }
    }
    Update_tbwMark();
    UpdateMark_2();

    auto platVec = Action.GetPlatInfoVec();
    selectedPlatVec.clear();
    for (auto plat : platVec)
    {
        for (auto platr : platResourceVec)
        {
            if (platr.GetLoadName() == plat.first)
            {
                std::pair<NebulaResources, int>mpair = std::make_pair(platr, plat.second);
                selectedPlatVec.push_back(mpair);
                break;
            }
        }
    }
    selectPlatInfoVec = selectedPlatVec;
    Update_tbwPlatResource();

    auto sensorVec = Action.GetSensorInfoVec();
    selectedSensorVec.clear();
    for (auto sensor : sensorVec)
    {
        for (auto sensorR : SensorResourceVec)
        {
            if (sensorR.GetLoadName() == sensor.first)
            {
                std::pair<NebulaResources, int>mpair = std::make_pair(sensorR, sensor.second);
                selectedSensorVec.push_back(mpair);
                break;
            }
        }
    }
    selectSensorInfoVec = selectedSensorVec;
    Update_tbwSensorResource();

    auto ammoVec = Action.GetAmmoInfoVec();
    selectedAmmoVec.clear();
    for (auto ammo : ammoVec)
    {
        for (auto ammoR : AmmoResourceVec)
        {
            if (ammoR.GetLoadName() == ammo.first)
            {
                std::pair<NebulaResources, int>mpair = std::make_pair(ammoR, ammo.second);
                selectedAmmoVec.push_back(mpair);
                break;
            }
        }
    }
    selectAmmoInfoVec = selectedAmmoVec;
    Update_tbwAmmoResource();

    QString intelliStr = QString::fromStdString(Action.GetInfo());
    if (intelliStr.contains(';'))
    {
        QStringList tempIntelli = intelliStr.split(';');
        for (int i = 0; i < tempIntelli.size(); i++)
        {

        }

        if (tempIntelli.size() > 1)
        {
            for (int i = 0; i < ui->cbxSeason->count(); i++)
            {
                if (ui->cbxSeason->itemText(i) == tempIntelli[0])
                {
                    ui->cbxSeason->setCurrentIndex(i);
                    break;
                }
            }
        }

        if (tempIntelli.size() > 2)
        {
            for (int i = 0; i < ui->cbxDayNight->count(); i++)
            {
                if (ui->cbxDayNight->itemText(i) == tempIntelli[1])
                {
                    ui->cbxDayNight->setCurrentIndex(i);
                    break;
                }
            }
        }

        if (tempIntelli.size() > 3)
        {
            ui->ledtVisibility->setText(tempIntelli[2]);
        }

        if (tempIntelli.size() > 4)
        {
            for (int i = 0; i < ui->cbxWeather->count(); i++)
            {
                if (ui->cbxWeather->itemText(i) == tempIntelli[3])
                {
                    ui->cbxWeather->setCurrentIndex(i);
                    break;
                }
            }
        }

        if (tempIntelli.size() > 5)
        {
            if (tempIntelli[4].contains('-'))
            {
                QStringList tlst = tempIntelli[4].split('-');
                if (tlst.size() >= 2)
                {
                    for (int i = 0; i < ui->cbxWindType->count(); i++)
                    {
                        if (ui->cbxWindType->itemText(i) == tlst[0])
                        {
                            ui->cbxWindType->setCurrentIndex(i);
                            break;
                        }
                    }

                    for (int i = 0; i < ui->cbxWindLevel->count(); i++)
                    {
                        if (ui->cbxWindLevel->itemText(i) == tlst[1])
                        {
                            ui->cbxWindLevel->setCurrentIndex(i);
                            break;
                        }
                    }
                }
            }

        }
    }
    QDateTime cuu = ui->dateTimeEdit_Begin->dateTime();
    QString   cuustr = cuu.toString();
    QDateTime cuustrcuu = QDateTime::fromString(cuustr);

    QString m_start = QString::fromStdString(Action.GetStartTime());
    auto dateTimeBegin = QDateTime::fromString(m_start, "yyyy/M/d HH:mm");
    ui->dateTimeEdit_Begin->setDateTime(dateTimeBegin);

    QString m_end = QString::fromStdString(Action.GetFinishTime());
    QDateTime dataTimeEnd = QDateTime::fromString(m_end, "yyyy/M/d HH:mm");
    ui->dateTimeEdit_End->setDateTime(dataTimeEnd);

    GetTextFromLedtName();
    GetTextFromLedtID();
    GetTextFromLedtDH();
    GetTextFromLedtAim();
    GetTextFromLedtRemark();

    Update_tbwPlatResource();
    Update_tbwSensorResource();
    Update_tbwAmmoResource();
    Update_tbwOtherResource();

}

void CDlgActionEdit::closeEvent(QCloseEvent *e)
{
    close();
}

void CDlgActionEdit::addMark(QString strName, QString strID)
{
    int rowcount = ui->tbwMark_2->rowCount();
    int colum = 2;

    QList<QString> columData;
    for (int i = 0; i < rowcount; i++)
    {
        QTableWidgetItem *item = ui->tbwMark_2->item(i, colum);
        if (item)
        {
            QString data = item->text();
            columData.append(data);
        }
    }
    if (columData.indexOf(strID) == -1)
    {
        ui->tbwMark_2->insertRow(rowcount);
        QTableWidgetItem *item1 = new QTableWidgetItem(QString::number(rowcount + 1));
        QTableWidgetItem *item2 = new QTableWidgetItem(strName);
        QTableWidgetItem *item3 = new QTableWidgetItem(strID);

        ui->tbwMark_2->setItem(rowcount, 0, item1);
        ui->tbwMark_2->setItem(rowcount, 1, item2);
        ui->tbwMark_2->setItem(rowcount, 2, item3);

        ui->tbwMark_2->item(rowcount, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->tbwMark_2->item(rowcount, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->tbwMark_2->item(rowcount, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        SystemMarkInfo tmpMark;
        std::string tar_id = strID.toStdString();
        for (auto tar : targetInfoVec)
        {
            if (tar.GetID() == tar_id)
            {
                tmpMark = tar;
                selectedMarkVec.push_back(tar);
                break;
            }
        }
    }
    QList<QString> columDataMark;
    for (int i = 0; i < ui->tbwMark->rowCount(); i++)
    {
        QTableWidgetItem *item = ui->tbwMark->item(i, colum);
        if (item)
        {
            QString data = item->text();
            columDataMark.append(data);
        }
    }
    if (columDataMark.indexOf(strID) != -1)
    {
        ui->tbwMark->removeRow(columDataMark.indexOf(strID));
        sorttbwMark();
    }
    sorttbwMark_2();
}

void CDlgActionEdit::sorttbwMark_2()
{
    ui->tbwMark_2->sortItems(2);
    int rowCount = ui->tbwMark_2->rowCount();
    for (int i = 0; i < rowCount; i++)
    {
        ui->tbwMark_2->setItem(i, 0, new QTableWidgetItem(QString::number(i + 1)));
        ui->tbwMark_2->item(i, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    }
}

void CDlgActionEdit::sorttbwMark()
{
    ui->tbwMark->sortItems(2);
    int rowCount = ui->tbwMark->rowCount();
    for (int i = 0; i < rowCount; i++)
    {
        ui->tbwMark->setItem(i, 0, new QTableWidgetItem(QString::number(i + 1)));
        ui->tbwMark->item(i, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    }
}

void CDlgActionEdit::ReadResourceNumber()
{

    for (int i = 0; i < ui->tbwPlatResource_2->rowCount(); i++)
    {
        std::string strName = ui->tbwPlatResource_2->item(i, 2)->text().toStdString();

        QWidget *existWidget = ui->tbwPlatResource_2->cellWidget(i, 3);
        QSpinBox *spinbox = nullptr;
        int platNum;
        if (existWidget && qobject_cast<QSpinBox *>(existWidget))
        {
            spinbox = qobject_cast<QSpinBox *>(existWidget);
        }
        else
        {
            spinbox->setValue(1);
        }
        platNum = spinbox->value();
        auto paltvec = selectedPlatVec.begin();
        for (paltvec; paltvec != selectedPlatVec.end(); paltvec++)
        {
            if ((*paltvec).first.GetLoadName() == strName)
            {
                (*paltvec).second = platNum;
                break;
            }
        }
    }

    for (int i = 0; i < ui->tbwAmmoResource_2->rowCount(); i++)
    {
        std::string strName = ui->tbwAmmoResource_2->item(i, 2)->text().toStdString();
        int AmmoNum;
        QWidget *existWidget = ui->tbwAmmoResource_2->cellWidget(i, 3);
        QSpinBox *spinbox = nullptr;
        if (existWidget && qobject_cast<QSpinBox *>(existWidget))
        {
            spinbox = qobject_cast<QSpinBox *>(existWidget);
        }
        else
        {
            spinbox->setValue(1);
        }
        AmmoNum = spinbox->value();
        auto ammoVec = selectedAmmoVec.begin();
        for (ammoVec; ammoVec != selectedAmmoVec.end(); ammoVec++)
        {
            if ((*ammoVec).first.GetLoadName() == strName)
            {
                (*ammoVec).second = AmmoNum;
                break;
            }
        }
    }

    for (int i = 0; i < ui->tbwSensorResource_2->rowCount(); i++)
    {
        std::string strName = ui->tbwSensorResource_2->item(i, 2)->text().toStdString();
        int sensorNum;
        QWidget *existWidget = ui->tbwSensorResource_2->cellWidget(i, 3);
        QSpinBox *spinbox = nullptr;
        if (existWidget && qobject_cast<QSpinBox *>(existWidget))
        {
            spinbox = qobject_cast<QSpinBox *>(existWidget);
        }
        else
        {
            spinbox->setValue(1);
        }
        sensorNum = spinbox->value();
        auto sensorVec = selectedSensorVec.begin();
        for (sensorVec; sensorVec != selectedSensorVec.end(); sensorVec++)
        {
            if ((*sensorVec).first.GetLoadName() == strName)
            {
                (*sensorVec).second = sensorNum;
                break;
            }
        }
    }
}
bool CDlgActionEdit::on_pbtnOK_clicked()
{
    if (selectedSensorVec.empty())
    {
        LyMessageBox::information(this, tr("tip"), tr("Please select sensor resources!"));
        return false;
    }
    if (ActionName.empty())
    {
        return false;
    }

    CNeubulaAction cNeuAction;
    cNeuAction.SetID(ActionID);
    cNeuAction.SetName(ActionName);
    cNeuAction.SetCode(ActionDH);
    cNeuAction.SetObject(ActionAim);
    std::string strArea = "";
    if (!selectedAreaVec.empty())
    {
        for (auto area : selectedAreaVec)
        {
            strArea += area.GetID();
            strArea += ";";
        }
    }
    if (strArea.length() >= 0)
    {
        strArea = strArea.substr(0, strArea.length() - 1);
    }
    cNeuAction.SetAreaID(strArea);

    std::string strInfo = Season + ";";
    strInfo += DayorNight;
    strInfo += ";";
    strInfo += Visibility;
    strInfo += ";";
    strInfo += Weather;
    strInfo += ";";
    strInfo += WindType;
    strInfo += "-";
    strInfo += WindLevel;
    cNeuAction.SetInfo(strInfo);
    cNeuAction.SetStartTime(ActionStartTime);
    cNeuAction.SetFinishTime(ActionEndTime);
    cNeuAction.SetRemark(ActionRemark);
    cNeuAction.SetActivateStates(isActive);

    double lon = ActionInfo.GetStartLon();
    double lat = ActionInfo.GetStartLat();
    double alt = ActionInfo.GetStartAlt();
    cNeuAction.SetStartPos(lon, lat, alt);

    std::vector < std::string > selectMarkID;
    for (auto id : selectedMarkVec)
    {
        selectMarkID.emplace_back(id.GetID());
    }
    cNeuAction.SetMarkIDList(selectMarkID);
    ITaskManagerActivator::Get().getPTaskManageService()->SetNebulaAction(cNeuAction);
    ITaskManagerActivator::Get().getPTaskManageService()->SetActivatingActionID(ActionID);

    std::vector<CRelationActionMark> V_ActionMark;
    MarkIDVec tarIDVec;
    if (!m_MarkIDVec.empty())
    {
        for (auto target : m_MarkIDVec)
        {
            tarIDVec.push_back(target);
        }
    }
    for (auto target : tarIDVec)
    {
        CRelationActionMark actionTar;
        actionTar.SetActionID(ActionID);
        actionTar.SetMarkID(target);
        V_ActionMark.emplace_back(actionTar);
    }
    ITaskManagerActivator::Get().getPTaskManageService()->RmoveRelationMark(V_ActionMark);

    ITaskManagerActivator::Get().getPTaskManageService()->AddRelationMark(selectMarkID);

    CRelationActionResource RelationActionVec;

    ReadResourceNumber();

    for (auto delect : selectPlatInfoVec)
    {
        std::string loadName = delect.first.GetLoadName();
        short int model = delect.first.GetLoadModel();
        int numSource = delect.second;
        bool being = false;
        for (auto plat : selectedPlatVec)
        {
            if (plat.first.GetLoadName() == loadName && plat.second == numSource)
            {
                being = true;
                break;
            }
        }
        if (!being)
        {
            CRelationActionResource RelationActionVec;
            int maxId = ITaskManagerActivator::Get().getPTaskManageService()->GetRelationPtMaxID();
            RelationActionVec.SetID(maxId);
            RelationActionVec.SetActionID(ActionID);
            RelationActionVec.SetResourceName(loadName);
            RelationActionVec.SetResourceNum(numSource);
            ITaskManagerActivator::Get().getPTaskManageService()->RmoveRelationPt(RelationActionVec);
        }
    }

    for (auto plat : selectedPlatVec)
    {
        int maxId = ITaskManagerActivator::Get().getPTaskManageService()->GetRelationPtMaxID();
        RelationActionVec.SetID(maxId);
        RelationActionVec.SetActionID(ActionID);
        RelationActionVec.SetResourceName(plat.first.GetLoadName());
        RelationActionVec.SetResourceNum(plat.second);
        ITaskManagerActivator::Get().getPTaskManageService()->AddRelationPt(RelationActionVec);
    }
    for (auto delect : selectSensorInfoVec)
    {
        std::string loadName = delect.first.GetLoadName();
        short int model = delect.first.GetLoadModel();
        int numSource = delect.second;
        bool being = false;
        for (auto sensor : selectedSensorVec)
        {
            if (sensor.first.GetLoadName() == loadName && sensor.second == numSource)
            {
                being = true;
                break;
            }
        }
        if (!being)
        {
            CRelationActionResource RelationActionVec;
            int maxId = ITaskManagerActivator::Get().getPTaskManageService()->GetRelationSensorMaxID();
            RelationActionVec.SetID(maxId);
            RelationActionVec.SetActionID(ActionID);
            RelationActionVec.SetResourceName(loadName);
            RelationActionVec.SetResourceNum(numSource);
            ITaskManagerActivator::Get().getPTaskManageService()->RmoveRelationSensor(RelationActionVec);
        }
    }

    for (auto target : selectedSensorVec)
    {
        int maxId = ITaskManagerActivator::Get().getPTaskManageService()->GetRelationSensorMaxID();
        RelationActionVec.SetID(maxId);
        RelationActionVec.SetActionID(ActionID);
        RelationActionVec.SetResourceName(target.first.GetLoadName());
        RelationActionVec.SetResourceNum(target.second);
        ITaskManagerActivator::Get().getPTaskManageService()->AddRelationSensor(RelationActionVec);
    }
    for (auto delect : selectAmmoInfoVec)
    {
        std::string loadName = delect.first.GetLoadName();
        short int model = delect.first.GetLoadModel();
        int numSource = delect.second;
        bool being = false;
        for (auto ammo : selectedAmmoVec)
        {
            if (ammo.first.GetLoadName() == loadName && ammo.second == numSource)
            {
                being = true;
                break;
            }
        }
        if (!being)
        {
            CRelationActionResource RelationActionVec;
            int maxId = ITaskManagerActivator::Get().getPTaskManageService()->GetRelationSensorMaxID();
            RelationActionVec.SetID(maxId);
            RelationActionVec.SetActionID(ActionID);
            RelationActionVec.SetResourceName(loadName);
            RelationActionVec.SetResourceNum(numSource);
            ITaskManagerActivator::Get().getPTaskManageService()->RmoveRelationAmmo(RelationActionVec);
        }
    }
    for (auto ammo : selectedAmmoVec)
    {
        int maxId = ITaskManagerActivator::Get().getPTaskManageService()->GetRelationAmmoMaxID();
        RelationActionVec.SetID(maxId);
        RelationActionVec.SetActionID(ActionID);
        RelationActionVec.SetResourceName(ammo.first.GetLoadName());
        RelationActionVec.SetResourceNum(ammo.second);
        ITaskManagerActivator::Get().getPTaskManageService()->AddRelationAmmo(RelationActionVec);
    }
    LY_TaskManage::Get().GetCDlgActionList()->UpdataWindowData();
    LY_TaskManage::Get().GetCDlgActionList()->UpdateListData();

    auto temVec = LY_TaskManage::Get().GetCDlgActionList()->GetActionVec();
    if (!isAddorEx)
    {
        LY_TaskManage::Get().GetCDlgActionList()->SetActive(temVec.size() - 1, 1);
    }
    close();
    return true;
}

void CDlgActionEdit::on_pbtnCancel_clicked()
{
    close();
}

void CDlgActionEdit::on_tbwArea_itemDoubleClicked(QTableWidgetItem *item)
{
    int mrow = item->row();
    std::string strArea = ui->tbwArea->item(mrow, 2)->text().toStdString();
    CZoneInfo airspace;
    for (int i = 0; i < airspaceInfoVec.size(); i++)
    {
        if (airspaceInfoVec[i].GetID() == strArea)
        {
            airspace = airspaceInfoVec[i];
            selectedAreaVec.push_back(airspaceInfoVec[i]);
            break;
        }
    }

    int rowcount = ui->tbwArea_2->rowCount();
    ui->tbwArea_2->insertRow(rowcount);
    QTableWidgetItem *item1 = new QTableWidgetItem(QString::number(rowcount + 1));
    QString strName = QString::fromStdString(airspace.GetName());
    QTableWidgetItem *item2 = new QTableWidgetItem(strName);
    QString strId = QString::fromStdString(airspace.GetID());
    QTableWidgetItem *item3 = new QTableWidgetItem(strId);

    ui->tbwArea_2->setItem(rowcount, 0, item1);
    ui->tbwArea_2->setItem(rowcount, 1, item2);
    ui->tbwArea_2->setItem(rowcount, 2, item3);
    ui->tbwArea_2->item(rowcount, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tbwArea_2->item(rowcount, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tbwArea_2->item(rowcount, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    Update_tbwMark();

    ui->tbwArea->removeRow(mrow);
}
void CDlgActionEdit::on_tbwArea_2_itemDoubleClicked(QTableWidgetItem *item)
{
    int mrow = item->row();
    std::string strId = ui->tbwArea_2->item(mrow, 2)->text().toStdString();
    CZoneInfo airSpace;
    for (int i = 0; i < selectedAreaVec.size(); i++)
    {
        if (selectedAreaVec[i].GetID() == strId)
        {
            airSpace = selectedAreaVec[i];
            selectedAreaVec.erase(selectedAreaVec.begin() + i);
            break;
        }
    }
    ui->tbwArea_2->removeRow(mrow);

    int rowNum = ui->tbwArea->rowCount();
    ui->tbwArea->insertRow(rowNum);
    QTableWidgetItem *item1 = new QTableWidgetItem(QString::number(rowNum + 1));
    QString strName = QString::fromStdString(airSpace.GetName());
    QTableWidgetItem *item2 = new QTableWidgetItem(strName);
    QString airspaceID = QString::fromStdString(airSpace.GetID());
    QTableWidgetItem *item3 = new QTableWidgetItem(airspaceID);

    ui->tbwArea->setItem(rowNum, 0, item1);
    ui->tbwArea->setItem(rowNum, 1, item2);
    ui->tbwArea->setItem(rowNum, 2, item3);
    ui->tbwArea->item(rowNum, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tbwArea->item(rowNum, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tbwArea->item(rowNum, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    Update_tbwMark();

    std::vector<std::string> strMarkName;
    for (auto target : targetInRegionVec)
    {
        strMarkName.push_back(target.GetName());
    }

    for (int i = 0; i < selectedMarkVec.size(); i++)
    {
        if (::find(strMarkName.begin(), strMarkName.end(), selectedMarkVec[i].GetName()) == strMarkName.end())
        {
            selectedMarkVec.erase(selectedMarkVec.begin() + i);
            --i;
        }
    }
    UpdateMark_2();
}
void CDlgActionEdit::UpdateMark_2()
{
    ui->tbwMark_2->clearContents();
    ui->tbwMark_2->setRowCount(selectedMarkVec.size());
    for (int i = 0; i < selectedMarkVec.size(); i++)
    {
        int rowcount = ui->tbwMark_2->rowCount();

        QTableWidgetItem *item1 = new QTableWidgetItem(QString::number(i + 1));
        QString strName = QString::fromStdString(selectedMarkVec[i].GetName());
        QTableWidgetItem *item2 = new QTableWidgetItem(strName);
        QString strId = QString::fromStdString(selectedMarkVec[i].GetID());
        QTableWidgetItem *item3 = new QTableWidgetItem(strId);

        ui->tbwMark_2->setItem(i, 0, item1);
        ui->tbwMark_2->setItem(i, 1, item2);
        ui->tbwMark_2->setItem(i, 2, item3);

        ui->tbwMark_2->item(i, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->tbwMark_2->item(i, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->tbwMark_2->item(i, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    }
}
void CDlgActionEdit::on_tbwMark_itemDoubleClicked(QTableWidgetItem *item)
{
    int mrow = item->row();
    SystemMarkInfo tmpMark;
    std::string tar_id = ui->tbwMark->item(mrow, 2)->text().toStdString();
    for (auto tar : targetInfoVec)
    {
        if (tar.GetID() == tar_id)
        {
            tmpMark = tar;
            selectedMarkVec.push_back(tar);
            break;
        }
    }

    int rowcount = ui->tbwMark_2->rowCount();
    ui->tbwMark_2->insertRow(rowcount);
    QTableWidgetItem *item1 = new QTableWidgetItem(QString::number(rowcount + 1));
    QString strName = QString::fromStdString(tmpMark.GetName());
    QTableWidgetItem *item2 = new QTableWidgetItem(strName);
    QString strId = QString::fromStdString(tmpMark.GetID());
    QTableWidgetItem *item3 = new QTableWidgetItem(strId);

    ui->tbwMark_2->setItem(rowcount, 0, item1);
    ui->tbwMark_2->setItem(rowcount, 1, item2);
    ui->tbwMark_2->setItem(rowcount, 2, item3);
    ui->tbwMark_2->item(rowcount, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tbwMark_2->item(rowcount, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tbwMark_2->item(rowcount, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    ui->tbwMark->removeRow(mrow);
    sorttbwMark();
    sorttbwMark_2();
}
void CDlgActionEdit::on_tbwMark_2_itemDoubleClicked(QTableWidgetItem *item)
{
    int mrow = item->row();

    SystemMarkInfo tmpMark;
    std::string tar_id = ui->tbwMark_2->item(mrow, 2)->text().toStdString();
    for (int i = 0; i < selectedMarkVec.size(); i++)
    {
        if (selectedMarkVec[i].GetID() == tar_id)
        {
            tmpMark = selectedMarkVec[i];
            selectedMarkVec.erase(selectedMarkVec.begin() + i);
            break;
        }
    }

    if (!IsMarkInRegion(tmpMark))
    {
        ui->tbwMark_2->removeRow(mrow);
        return;
    }
    int rowcount = ui->tbwMark->rowCount();
    ui->tbwMark->insertRow(rowcount);
    QTableWidgetItem *item1 = new QTableWidgetItem(QString::number(rowcount + 1));
    QString strName = QString::fromStdString(tmpMark.GetName());
    QTableWidgetItem *item2 = new QTableWidgetItem(strName);
    QString strId = QString::fromStdString(tmpMark.GetID());
    QTableWidgetItem *item3 = new QTableWidgetItem(strId);

    ui->tbwMark->setItem(rowcount, 0, item1);
    ui->tbwMark->setItem(rowcount, 1, item2);
    ui->tbwMark->setItem(rowcount, 2, item3);
    ui->tbwMark->item(rowcount, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tbwMark->item(rowcount, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tbwMark->item(rowcount, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    sorttbwMark();

    if (mrow == ui->tbwMark_2->rowCount() - 1)
    {
        ui->tbwMark_2->removeRow(mrow);
    }
    else
    {
        ui->tbwMark_2->removeRow(mrow);
        sorttbwMark_2();
    }
}
void CDlgActionEdit::CreateAllTable()
{
    ui->tbwArea->verticalHeader()->setHidden(true);
    ui->tbwArea->setSelectionBehavior(QTableWidget::SelectRows);
    ui->tbwArea->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tbwArea->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tbwArea->setAlternatingRowColors(true);
    ui->tbwArea->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->tbwArea->horizontalHeader()->setStretchLastSection(true);

    ui->tbwMark_2->verticalHeader()->setHidden(true);
    ui->tbwMark_2->setSelectionBehavior(QTableWidget::SelectRows);
    ui->tbwMark_2->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tbwMark_2->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tbwMark_2->setAlternatingRowColors(true);
    ui->tbwMark_2->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->tbwMark_2->horizontalHeader()->setStretchLastSection(true);

    ui->tbwArea_2->verticalHeader()->setHidden(true);
    ui->tbwArea_2->setSelectionBehavior(QTableWidget::SelectRows);
    ui->tbwArea_2->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tbwArea_2->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tbwArea_2->setAlternatingRowColors(true);
    ui->tbwArea_2->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->tbwArea_2->horizontalHeader()->setStretchLastSection(true);

    ui->tbwMark->verticalHeader()->setHidden(true);
    ui->tbwMark->setSelectionBehavior(QTableWidget::SelectRows);
    ui->tbwMark->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tbwMark->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tbwMark->setAlternatingRowColors(true);
    ui->tbwMark->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->tbwMark->horizontalHeader()->setStretchLastSection(true);

    ui->tbwPlatResource->verticalHeader()->setHidden(true);
    ui->tbwPlatResource->setSelectionBehavior(QTableWidget::SelectRows);
    ui->tbwPlatResource->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tbwPlatResource->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tbwPlatResource->setAlternatingRowColors(true);
    ui->tbwPlatResource->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->tbwPlatResource->horizontalHeader()->setStretchLastSection(true);

    ui->tbwPlatResource_2->verticalHeader()->setHidden(true);
    ui->tbwPlatResource_2->setSelectionBehavior(QTableWidget::SelectRows);
    ui->tbwPlatResource_2->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tbwPlatResource_2->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tbwPlatResource_2->setAlternatingRowColors(true);
    ui->tbwPlatResource_2->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->tbwPlatResource_2->horizontalHeader()->setStretchLastSection(true);

    ui->tbwSensorResource->verticalHeader()->setHidden(true);
    ui->tbwSensorResource->setSelectionBehavior(QTableWidget::SelectRows);
    ui->tbwSensorResource->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tbwSensorResource->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tbwSensorResource->setAlternatingRowColors(true);
    ui->tbwSensorResource->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->tbwSensorResource->horizontalHeader()->setStretchLastSection(true);

    ui->tbwSensorResource_2->verticalHeader()->setHidden(true);
    ui->tbwSensorResource_2->setSelectionBehavior(QTableWidget::SelectRows);
    ui->tbwSensorResource_2->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tbwSensorResource->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tbwSensorResource_2->setAlternatingRowColors(true);
    ui->tbwSensorResource_2->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->tbwSensorResource_2->horizontalHeader()->setStretchLastSection(true);

    ui->tbwAmmoResource->verticalHeader()->setHidden(true);
    ui->tbwAmmoResource->setSelectionBehavior(QTableWidget::SelectRows);
    ui->tbwAmmoResource->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tbwAmmoResource->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tbwAmmoResource->setAlternatingRowColors(true);
    ui->tbwAmmoResource->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->tbwAmmoResource->horizontalHeader()->setStretchLastSection(true);

    ui->tbwAmmoResource_2->verticalHeader()->setHidden(true);
    ui->tbwAmmoResource_2->setSelectionBehavior(QTableWidget::SelectRows);
    ui->tbwAmmoResource_2->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tbwAmmoResource->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tbwAmmoResource_2->setAlternatingRowColors(true);
    ui->tbwAmmoResource_2->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->tbwAmmoResource_2->horizontalHeader()->setStretchLastSection(true);

    ui->tbwOtherResource->verticalHeader()->setHidden(true);
    ui->tbwOtherResource->setSelectionBehavior(QTableWidget::SelectRows);
    ui->tbwOtherResource->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tbwOtherResource->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tbwOtherResource->setAlternatingRowColors(true);
    ui->tbwOtherResource->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->tbwOtherResource->horizontalHeader()->setStretchLastSection(true);

    ui->tbwOtherResource_2->verticalHeader()->setHidden(true);
    ui->tbwOtherResource_2->setSelectionBehavior(QTableWidget::SelectRows);
    ui->tbwOtherResource_2->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tbwOtherResource_2->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tbwOtherResource_2->setAlternatingRowColors(true);
    ui->tbwOtherResource_2->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->tbwOtherResource_2->horizontalHeader()->setStretchLastSection(true);
}

void CDlgActionEdit::on_btnAddMark_clicked()
{
    CDlgAddMark *addMark = new CDlgAddMark();
    addMark->setParent(this);
    addMark->show();

}

void CDlgActionEdit::on_btnAddTask_clicked()
{
    if (QMessageBox::Ok != LyMessageBox::information(this, tr("tip"), tr("Whether the current action is set?"), QMessageBox::Ok | QMessageBox::Cancel))
    {
        return;
    }

    if (!on_pbtnOK_clicked())
    {
        return;
    }

    LY_TaskManage::Get().GetDlgMissionEdit()->SetDataType(1);
    LY_TaskManage::Get().GetDlgMissionEdit()->InitWindowCtrl(0);
    if (LY_TaskManage::Get().GetDlgMissionEdit()->isVisible())
    {
        LY_TaskManage::Get().GetDlgMissionEdit()->hide();
    }
    LY_TaskManage::Get().GetDlgMissionEdit()->show();
    LY_TaskManage::Get().GetDlgMissionEdit()->raise();
}

void CDlgActionEdit::Update_tbwArea()
{
    if (!airspaceInfoVec.empty())
    {
        ui->tbwArea->clearContents();
        ui->tbwArea->setRowCount(0);
        std::vector<std::string> airspaceName;

        for (auto Area : selectedAreaVec)
        {
            airspaceName.push_back(Area.GetName());
        }

        for (auto mArea : airspaceInfoVec)
        {
            if (::find(airspaceName.begin(), airspaceName.end(), mArea.GetName()) != airspaceName.end())
            {
                continue;
            }
            int rowcount = ui->tbwArea->rowCount();
            ui->tbwArea->insertRow(rowcount);
            QTableWidgetItem *item0 = new QTableWidgetItem(QString::number(rowcount + 1));
            QString strName = QString::fromStdString(mArea.GetName());
            QTableWidgetItem *item2 = new QTableWidgetItem(strName);
            QString strId = QString::fromStdString(mArea.GetID());
            QTableWidgetItem *item3 = new QTableWidgetItem(strId);

            ui->tbwArea->setItem(rowcount, 0, item0);
            ui->tbwArea->setItem(rowcount, 1, item2);
            ui->tbwArea->setItem(rowcount, 2, item3);

            ui->tbwArea->item(rowcount, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            ui->tbwArea->item(rowcount, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            ui->tbwArea->item(rowcount, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        }
    }
}
void CDlgActionEdit::UpdateSeleAreaTab()
{
    ui->tbwArea_2->clearContents();
    ui->tbwArea_2->setRowCount(0);
    if (!selectedAreaVec.empty())
    {
        for (auto mArea : selectedAreaVec)
        {
            int rowcount = ui->tbwArea_2->rowCount();
            ui->tbwArea_2->insertRow(rowcount);
            QTableWidgetItem *item0 = new QTableWidgetItem(QString::number(rowcount + 1));
            QString strName = QString::fromStdString(mArea.GetName());
            QTableWidgetItem *item2 = new QTableWidgetItem(strName);
            QString strId = QString::fromStdString(mArea.GetID());
            QTableWidgetItem *item3 = new QTableWidgetItem(strId);

            ui->tbwArea_2->setItem(rowcount, 0, item0);
            ui->tbwArea_2->setItem(rowcount, 1, item2);
            ui->tbwArea_2->setItem(rowcount, 2, item3);

            ui->tbwArea_2->item(rowcount, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            ui->tbwArea_2->item(rowcount, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            ui->tbwArea_2->item(rowcount, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        }
    }
}
void CDlgActionEdit::Update_tbwMark()
{
    if (!targetInfoVec.empty())
    {
        ui->tbwMark->clearContents();
        ui->tbwMark->setRowCount(0);
        targetInRegionVec.clear();
        for (auto Tar : targetInfoVec)
        {
            if (IsMarkInRegion(Tar))
            {
                targetInRegionVec.push_back(Tar);
            }
        }
        std::vector<std::string> targetName;

        for (auto target : selectedMarkVec)
        {
            targetName.push_back(target.GetName());
        }
        for (auto target : targetInRegionVec)
        {
            if (::find(targetName.begin(), targetName.end(), target.GetName()) != targetName.end())
            {
                continue;
            }
            int rowcount = ui->tbwMark->rowCount();
            ui->tbwMark->insertRow(rowcount);
            QTableWidgetItem *item0 = new QTableWidgetItem(QString::number(rowcount + 1));
            QString strName = QString::fromStdString(target.GetName());
            QTableWidgetItem *item1 = new QTableWidgetItem(strName);
            QString strId = QString::fromStdString(target.GetID());
            QTableWidgetItem *item2 = new QTableWidgetItem(strId);

            ui->tbwMark->setItem(rowcount, 0, item0);
            ui->tbwMark->setItem(rowcount, 1, item1);
            ui->tbwMark->setItem(rowcount, 2, item2);

            ui->tbwMark->item(rowcount, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            ui->tbwMark->item(rowcount, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            ui->tbwMark->item(rowcount, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        }
    }
}
void CDlgActionEdit::Update_tbwPlatResource()
{
    if (!platResourceVec.empty())
    {
        ui->tbwPlatResource->clearContents();
        ui->tbwPlatResource->setRowCount(0);
        std::vector<std::string> platName;

        for (auto plat : selectedPlatVec)
        {
            platName.push_back(plat.first.GetLoadName());
        }

        for (auto item : platResourceVec)
        {
            if (::find(platName.begin(), platName.end(), item.GetLoadName()) != platName.end())
            {
                continue;
            }
            int rowcount = ui->tbwPlatResource->rowCount();
            ui->tbwPlatResource->insertRow(rowcount);
            QString mType = QString::number(item.GetLoadType());
            QTableWidgetItem *item0 = new QTableWidgetItem(mType);
            QString mModel = QString::number(item.GetLoadModel());
            QTableWidgetItem *item1 = new QTableWidgetItem(mModel);
            QString strName = QString::fromStdString(item.GetLoadName());
            QTableWidgetItem *item2 = new QTableWidgetItem(strName);

            ui->tbwPlatResource->setItem(rowcount, 0, item0);
            ui->tbwPlatResource->setItem(rowcount, 1, item1);
            ui->tbwPlatResource->setItem(rowcount, 2, item2);

            ui->tbwPlatResource->item(rowcount, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            ui->tbwPlatResource->item(rowcount, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            ui->tbwPlatResource->item(rowcount, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        }
    }
    ui->tbwPlatResource_2->clearContents();
    ui->tbwPlatResource_2->setRowCount(0);
    if (!selectedPlatVec.empty())
    {

        int index = 0;
        for (auto item : selectedPlatVec)
        {
            int rowcount = ui->tbwPlatResource_2->rowCount();
            ui->tbwPlatResource_2->insertRow(rowcount);
            QString mType = QString::number(item.first.GetLoadType());
            QTableWidgetItem *item0 = new QTableWidgetItem(mType);
            QString mModel = QString::number(item.first.GetLoadModel());
            QTableWidgetItem *item1 = new QTableWidgetItem(mModel);
            QString strName = QString::fromStdString(item.first.GetLoadName());
            QTableWidgetItem *item2 = new QTableWidgetItem(strName);
            QSpinBox *spinBox = new QSpinBox;
            spinBox->setValue(item.second);
            ui->tbwPlatResource_2->setItem(rowcount, 0, item0);
            ui->tbwPlatResource_2->setItem(rowcount, 1, item1);
            ui->tbwPlatResource_2->setItem(rowcount, 2, item2);
            ui->tbwPlatResource_2->setCellWidget(rowcount, 3, spinBox);
            ui->tbwPlatResource_2->item(rowcount, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            ui->tbwPlatResource_2->item(rowcount, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            ui->tbwPlatResource_2->item(rowcount, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

            ++index;
        }
    }
}

void CDlgActionEdit::Update_tbwSensorResource()
{

    if (!SensorResourceVec.empty())
    {
        ui->tbwSensorResource->clearContents();
        ui->tbwSensorResource->setRowCount(0);
        std::vector<std::string> sensorName;
        for (auto sensor : selectedSensorVec)
        {
            sensorName.push_back(sensor.first.GetLoadName());
        }

        for (auto item : SensorResourceVec)
        {
            if (::find(sensorName.begin(), sensorName.end(), item.GetLoadName()) != sensorName.end())
            {
                continue;
            }
            int rowcount = ui->tbwSensorResource->rowCount();
            ui->tbwSensorResource->insertRow(rowcount);
            QString mType = QString::number(item.GetLoadType());
            QTableWidgetItem *item0 = new QTableWidgetItem(mType);
            QString mModel = QString::number(item.GetLoadModel());
            QTableWidgetItem *item1 = new QTableWidgetItem(mModel);
            QString strName = QString::fromStdString(item.GetLoadName());
            QTableWidgetItem *item2 = new QTableWidgetItem(strName);
            ui->tbwSensorResource->setItem(rowcount, 0, item0);
            ui->tbwSensorResource->setItem(rowcount, 1, item1);
            ui->tbwSensorResource->setItem(rowcount, 2, item2);

            ui->tbwSensorResource->item(rowcount, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            ui->tbwSensorResource->item(rowcount, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            ui->tbwSensorResource->item(rowcount, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        }
    }
    ui->tbwSensorResource_2->clearContents();
    ui->tbwSensorResource_2->setRowCount(0);
    if (!selectedSensorVec.empty())
    {
        int index = 0;
        for (auto item : selectedSensorVec)
        {
            int rowcount = ui->tbwSensorResource_2->rowCount();
            ui->tbwSensorResource_2->insertRow(rowcount);
            QString mType = QString::number(item.first.GetLoadType());
            QTableWidgetItem *item0 = new QTableWidgetItem(mType);
            QString mModel = QString::number(item.first.GetLoadModel());
            QTableWidgetItem *item1 = new QTableWidgetItem(mModel);
            QString strName = QString::fromStdString(item.first.GetLoadName());
            QTableWidgetItem *item2 = new QTableWidgetItem(strName);
            QSpinBox *spinBox = new QSpinBox;
            spinBox->setValue(item.second);
            ui->tbwSensorResource_2->setItem(rowcount, 0, item0);
            ui->tbwSensorResource_2->setItem(rowcount, 1, item1);
            ui->tbwSensorResource_2->setItem(rowcount, 2, item2);
            ui->tbwSensorResource_2->setCellWidget(rowcount, 3, spinBox);

            ui->tbwSensorResource_2->item(rowcount, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            ui->tbwSensorResource_2->item(rowcount, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            ui->tbwSensorResource_2->item(rowcount, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            ++index;
        }
    }
}

void CDlgActionEdit::Update_tbwAmmoResource()
{
    if (!AmmoResourceVec.empty())
    {
        ui->tbwAmmoResource->clearContents();
        ui->tbwAmmoResource->setRowCount(0);
        std::vector<std::string> AmmoName;
        for (auto ammo : selectedAmmoVec)
        {
            AmmoName.push_back(ammo.first.GetLoadName());
        }

        for (auto item : AmmoResourceVec)
        {
            if (::find(AmmoName.begin(), AmmoName.end(), item.GetLoadName()) != AmmoName.end())
            {
                continue;
            }
            int rowcount = ui->tbwAmmoResource->rowCount();
            ui->tbwAmmoResource->insertRow(rowcount);
            QString mType = QString::number(item.GetLoadType());
            QTableWidgetItem *item0 = new QTableWidgetItem(mType);
            QString mModel = QString::number(item.GetLoadModel());
            QTableWidgetItem *item1 = new QTableWidgetItem(mModel);
            QString strName = QString::fromStdString(item.GetLoadName());
            QTableWidgetItem *item2 = new QTableWidgetItem(strName);

            ui->tbwAmmoResource->setItem(rowcount, 0, item0);
            ui->tbwAmmoResource->setItem(rowcount, 1, item1);
            ui->tbwAmmoResource->setItem(rowcount, 2, item2);

            ui->tbwAmmoResource->item(rowcount, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            ui->tbwAmmoResource->item(rowcount, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            ui->tbwAmmoResource->item(rowcount, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        }
    }
    ui->tbwAmmoResource_2->clearContents();
    ui->tbwAmmoResource_2->setRowCount(0);
    if (!selectedAmmoVec.empty())
    {
        int index = 0;
        for (auto item : selectedAmmoVec)
        {
            int rowcount = ui->tbwAmmoResource_2->rowCount();
            ui->tbwAmmoResource_2->insertRow(rowcount);
            QString mType = QString::number(item.first.GetLoadType());
            QTableWidgetItem *item0 = new QTableWidgetItem(mType);
            QString mModel = QString::number(item.first.GetLoadModel());
            QTableWidgetItem *item1 = new QTableWidgetItem(mModel);
            QString strName = QString::fromStdString(item.first.GetLoadName());
            QTableWidgetItem *item2 = new QTableWidgetItem(strName);
            QSpinBox *spinBox = new QSpinBox;
            spinBox->setValue(item.second);
            ui->tbwAmmoResource_2->setItem(rowcount, 0, item0);
            ui->tbwAmmoResource_2->setItem(rowcount, 1, item1);
            ui->tbwAmmoResource_2->setItem(rowcount, 2, item2);
            ui->tbwAmmoResource_2->setCellWidget(rowcount, 3, spinBox);

            ui->tbwAmmoResource_2->item(rowcount, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            ui->tbwAmmoResource_2->item(rowcount, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            ui->tbwAmmoResource_2->item(rowcount, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            ++index;
        }
    }
}

void CDlgActionEdit::Update_tbwOtherResource()
{
    if (!OtherResourceVec.empty())
    {
        ui->tbwOtherResource->clearContents();
        ui->tbwOtherResource->setRowCount(0);

        std::vector<std::string> otherName;
        for (auto other : selectedOtherVec)
        {
            otherName.push_back(other.first.GetLoadName());
        }

        for (auto item : OtherResourceVec)
        {
            if (::find(otherName.begin(), otherName.end(), item.GetLoadName()) != otherName.end())
            {
                continue;
            }
            int rowcount = ui->tbwOtherResource->rowCount();
            ui->tbwOtherResource->insertRow(rowcount);
            QString mType = QString::number(item.GetLoadType());
            QTableWidgetItem *item0 = new QTableWidgetItem(mType);
            QString mModel = QString::number(item.GetLoadModel());
            QTableWidgetItem *item1 = new QTableWidgetItem(mModel);
            QString strName = QString::fromStdString(item.GetLoadName());
            QTableWidgetItem *item2 = new QTableWidgetItem(strName);

            ui->tbwOtherResource->setItem(rowcount, 0, item0);
            ui->tbwOtherResource->setItem(rowcount, 1, item1);
            ui->tbwOtherResource->setItem(rowcount, 2, item2);

            ui->tbwOtherResource->item(rowcount, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            ui->tbwOtherResource->item(rowcount, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            ui->tbwOtherResource->item(rowcount, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        }
    }
    ui->tbwOtherResource_2->clearContents();
    ui->tbwOtherResource_2->setRowCount(0);
    if (!selectedOtherVec.empty())
    {
        int index = 0;
        for (auto item : selectedOtherVec)
        {
            int rowcount = ui->tbwOtherResource_2->rowCount();
            ui->tbwOtherResource_2->insertRow(rowcount);
            QString mType = QString::number(item.first.GetLoadType());
            QTableWidgetItem *item0 = new QTableWidgetItem(mType);
            QString mModel = QString::number(item.first.GetLoadModel());
            QTableWidgetItem *item1 = new QTableWidgetItem(mModel);
            QString strName = QString::fromStdString(item.first.GetLoadName());
            QTableWidgetItem *item2 = new QTableWidgetItem(strName);
            QSpinBox *spinBox = new QSpinBox;
            spinBox->setValue(item.second);
            ui->tbwOtherResource_2->setItem(rowcount, 0, item0);
            ui->tbwOtherResource_2->setItem(rowcount, 1, item1);
            ui->tbwOtherResource_2->setItem(rowcount, 2, item2);
            ui->tbwOtherResource_2->setCellWidget(rowcount, 3, spinBox);

            ui->tbwOtherResource_2->item(rowcount, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            ui->tbwOtherResource_2->item(rowcount, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            ui->tbwOtherResource_2->item(rowcount, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            ++index;
        }
    }
}

void CDlgActionEdit::on_tbwPlatResource_itemDoubleClicked(QTableWidgetItem *item)
{
    int mrow = item->row();
    std::string strplatID = ui->tbwPlatResource->item(mrow, 2)->text().toStdString();

    NebulaResources platResource;
    for (auto plat : platResourceVec)
    {
        if (plat.GetLoadName() == strplatID)
        {
            platResource = plat;
            selectedPlatVec.push_back(std::pair<NebulaResources, int>(plat, 1));
            break;
        }
    }

    int rowcount = ui->tbwPlatResource_2->rowCount();
    ui->tbwPlatResource_2->insertRow(rowcount);
    QTableWidgetItem *item1 = new QTableWidgetItem(QString::number(platResource.GetLoadType()));
    QTableWidgetItem *item2 = new QTableWidgetItem(QString::number(platResource.GetLoadModel()));
    QString strId = QString::fromStdString(platResource.GetLoadName());
    QTableWidgetItem *item3 = new QTableWidgetItem(strId);
    ui->tbwPlatResource_2->setItem(rowcount, 0, item1);
    ui->tbwPlatResource_2->setItem(rowcount, 1, item2);
    ui->tbwPlatResource_2->setItem(rowcount, 2, item3);

    QSpinBox *spinBox = new QSpinBox;
    spinBox->setValue(1);
    ui->tbwPlatResource_2->setCellWidget(rowcount, 3, spinBox);

    ui->tbwPlatResource_2->item(rowcount, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tbwPlatResource_2->item(rowcount, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tbwPlatResource_2->item(rowcount, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    ui->tbwPlatResource->removeRow(mrow);
}

void CDlgActionEdit::on_tbwPlatResource_2_itemDoubleClicked(QTableWidgetItem *item)
{
    int mrow = item->row();
    std::string strplatID = ui->tbwPlatResource_2->item(mrow, 2)->text().toStdString();
    NebulaResources platResource;
    for (int i = 0; i < selectedPlatVec.size(); i++)
    {
        if (selectedPlatVec[i].first.GetLoadName() == strplatID)
        {
            platResource = selectedPlatVec[i].first;
            selectedPlatVec.erase(selectedPlatVec.begin() + i);
            break;
        }
    }

    int rowcount = ui->tbwPlatResource->rowCount();
    ui->tbwPlatResource->insertRow(rowcount);
    QTableWidgetItem *item1 = new QTableWidgetItem(QString::number(platResource.GetLoadType()));
    QTableWidgetItem *item2 = new QTableWidgetItem(QString::number(platResource.GetLoadModel()));
    QString strId = QString::fromStdString(platResource.GetLoadName());
    QTableWidgetItem *item3 = new QTableWidgetItem(strId);
    ui->tbwPlatResource->setItem(rowcount, 0, item1);
    ui->tbwPlatResource->setItem(rowcount, 1, item2);
    ui->tbwPlatResource->setItem(rowcount, 2, item3);
    ui->tbwPlatResource->item(rowcount, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tbwPlatResource->item(rowcount, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tbwPlatResource->item(rowcount, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    ui->tbwPlatResource_2->removeRow(mrow);
}

void CDlgActionEdit::on_tbwSensorResource_itemDoubleClicked(QTableWidgetItem *item)
{
    int mrow = item->row();
    std::string strplatID = ui->tbwSensorResource->item(mrow, 2)->text().toStdString();
    NebulaResources sensorResource;
    for (auto sensor : SensorResourceVec)
    {
        if (sensor.GetLoadName() == strplatID)
        {
            sensorResource = sensor;
            selectedSensorVec.push_back(std::pair<NebulaResources, int>(sensor, 1));
            break;
        }
    }

    int rowcount = ui->tbwSensorResource_2->rowCount();
    ui->tbwSensorResource_2->insertRow(rowcount);
    QTableWidgetItem *item1 = new QTableWidgetItem(QString::number(sensorResource.GetLoadType()));
    QTableWidgetItem *item2 = new QTableWidgetItem(QString::number(sensorResource.GetLoadModel()));
    QString strId = QString::fromStdString(sensorResource.GetLoadName());
    QTableWidgetItem *item3 = new QTableWidgetItem(strId);
    ui->tbwSensorResource_2->setItem(rowcount, 0, item1);
    ui->tbwSensorResource_2->setItem(rowcount, 1, item2);
    ui->tbwSensorResource_2->setItem(rowcount, 2, item3);

    QSpinBox *spinBox = new QSpinBox;
    spinBox->setValue(1);
    ui->tbwSensorResource_2->setCellWidget(rowcount, 3, spinBox);
    ui->tbwSensorResource_2->item(rowcount, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tbwSensorResource_2->item(rowcount, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tbwSensorResource_2->item(rowcount, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tbwSensorResource->removeRow(mrow);
}
void CDlgActionEdit::on_tbwSensorResource_2_itemDoubleClicked(QTableWidgetItem *item)
{
    int mrow = item->row();
    std::string strsensorID = ui->tbwSensorResource_2->item(mrow, 2)->text().toStdString();
    NebulaResources sensorResource;
    for (int i = 0; i < selectedSensorVec.size(); i++)
    {
        if (selectedSensorVec[i].first.GetLoadName() == strsensorID)
        {
            sensorResource = selectedSensorVec[i].first;
            selectedSensorVec.erase(selectedSensorVec.begin() + i);
            break;
        }
    }
    int rowcount = ui->tbwSensorResource->rowCount();
    ui->tbwSensorResource->insertRow(rowcount);
    QTableWidgetItem *item1 = new QTableWidgetItem(QString::number(sensorResource.GetLoadType()));
    QTableWidgetItem *item2 = new QTableWidgetItem(QString::number(sensorResource.GetLoadModel()));
    QString strId = QString::fromStdString(sensorResource.GetLoadName());
    QTableWidgetItem *item3 = new QTableWidgetItem(strId);
    ui->tbwSensorResource->setItem(rowcount, 0, item1);
    ui->tbwSensorResource->setItem(rowcount, 1, item2);
    ui->tbwSensorResource->setItem(rowcount, 2, item3);
    ui->tbwSensorResource->item(rowcount, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tbwSensorResource->item(rowcount, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tbwSensorResource->item(rowcount, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    ui->tbwSensorResource_2->removeRow(mrow);
}
void CDlgActionEdit::on_tbwAmmoResource_itemDoubleClicked(QTableWidgetItem *item)
{
    int mrow = item->row();
    std::string strammoID = ui->tbwAmmoResource->item(mrow, 2)->text().toStdString();
    NebulaResources Resource;
    for (auto Ammo : AmmoResourceVec)
    {
        if (Ammo.GetLoadName() == strammoID)
        {
            Resource = Ammo;
            selectedAmmoVec.push_back(std::pair<NebulaResources, int>(Resource, 1));
            break;
        }
    }

    int rowcount = ui->tbwAmmoResource_2->rowCount();
    ui->tbwAmmoResource_2->insertRow(rowcount);
    QTableWidgetItem *item1 = new QTableWidgetItem(QString::number(Resource.GetLoadType()));
    QTableWidgetItem *item2 = new QTableWidgetItem(QString::number(Resource.GetLoadModel()));
    QString strId = QString::fromStdString(Resource.GetLoadName());
    QTableWidgetItem *item3 = new QTableWidgetItem(strId);

    ui->tbwAmmoResource_2->setItem(rowcount, 0, item1);
    ui->tbwAmmoResource_2->setItem(rowcount, 1, item2);
    ui->tbwAmmoResource_2->setItem(rowcount, 2, item3);
    QSpinBox *spinBox = new QSpinBox;
    spinBox->setValue(1);
    ui->tbwAmmoResource_2->setCellWidget(rowcount, 3, spinBox);
    ui->tbwAmmoResource_2->item(rowcount, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tbwAmmoResource_2->item(rowcount, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tbwAmmoResource_2->item(rowcount, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    ui->tbwAmmoResource->removeRow(mrow);
}
void CDlgActionEdit::on_tbwAmmoResource_2_itemDoubleClicked(QTableWidgetItem *item)
{
    int mrow = item->row();
    std::string strAmmoID = ui->tbwAmmoResource_2->item(mrow, 2)->text().toStdString();
    NebulaResources Resource;
    for (int i = 0; i < selectedAmmoVec.size(); i++)
    {
        if (selectedAmmoVec[i].first.GetLoadName() == strAmmoID)
        {
            Resource = selectedAmmoVec[i].first;
            selectedAmmoVec.erase(selectedAmmoVec.begin() + i);
            break;
        }
    }

    int rowcount = ui->tbwAmmoResource->rowCount();
    ui->tbwAmmoResource->insertRow(rowcount);
    QTableWidgetItem *item1 = new QTableWidgetItem(QString::number(Resource.GetLoadType()));
    QTableWidgetItem *item2 = new QTableWidgetItem(QString::number(Resource.GetLoadModel()));
    QString strId = QString::fromStdString(Resource.GetLoadName());
    QTableWidgetItem *item3 = new QTableWidgetItem(strId);
    ui->tbwAmmoResource->setItem(rowcount, 0, item1);
    ui->tbwAmmoResource->setItem(rowcount, 1, item2);
    ui->tbwAmmoResource->setItem(rowcount, 2, item3);
    ui->tbwAmmoResource->item(rowcount, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tbwAmmoResource->item(rowcount, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tbwAmmoResource->item(rowcount, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tbwAmmoResource_2->removeRow(mrow);
}
void CDlgActionEdit::on_tbwOtherResource_itemDoubleClicked(QTableWidgetItem *item)
{
    int mrow = item->row();
    std::string strotherID = ui->tbwOtherResource->item(mrow, 2)->text().toStdString();
    NebulaResources Resource;
    for (auto other : OtherResourceVec)
    {
        if (other.GetLoadName() == strotherID)
        {
            Resource = other;
            selectedOtherVec.push_back(std::pair<NebulaResources, int>(Resource, 1));
            break;
        }
    }

    int rowcount = ui->tbwOtherResource_2->rowCount();
    ui->tbwOtherResource_2->insertRow(rowcount);
    QTableWidgetItem *item1 = new QTableWidgetItem(QString::number(Resource.GetLoadType()));
    QTableWidgetItem *item2 = new QTableWidgetItem(QString::number(Resource.GetLoadModel()));
    QString strId = QString::fromStdString(Resource.GetLoadName());
    QTableWidgetItem *item3 = new QTableWidgetItem(strId);

    ui->tbwOtherResource_2->setItem(rowcount, 0, item1);
    ui->tbwOtherResource_2->setItem(rowcount, 1, item2);
    ui->tbwOtherResource_2->setItem(rowcount, 2, item3);
    QSpinBox *spinBox = new QSpinBox;
    spinBox->setValue(1);
    ui->tbwOtherResource_2->setCellWidget(rowcount, 3, spinBox);
    ui->tbwOtherResource_2->item(rowcount, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tbwOtherResource_2->item(rowcount, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tbwOtherResource_2->item(rowcount, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    ui->tbwOtherResource->removeRow(mrow);
}
void CDlgActionEdit::on_tbwOtherResource_2_itemDoubleClicked(QTableWidgetItem *item)
{
    int mrow = item->row();
    std::string strOtherID = ui->tbwOtherResource_2->item(mrow, 2)->text().toStdString();
    NebulaResources Resource;
    for (int i = 0; i < selectedOtherVec.size(); i++)
    {
        if (selectedOtherVec[i].first.GetLoadName() == strOtherID)
        {
            Resource = selectedOtherVec[i].first;
            selectedOtherVec.erase(selectedOtherVec.begin() + i);
            break;
        }
    }

    int rowcount = ui->tbwOtherResource->rowCount();
    ui->tbwOtherResource->insertRow(rowcount);
    QTableWidgetItem *item1 = new QTableWidgetItem(QString::number(Resource.GetLoadType()));
    QTableWidgetItem *item2 = new QTableWidgetItem(QString::number(Resource.GetLoadModel()));
    QString strId = QString::fromStdString(Resource.GetLoadName());
    QTableWidgetItem *item3 = new QTableWidgetItem(strId);
    QTableWidgetItem *item4 = new QTableWidgetItem(QString::number(1));
    item4->setFlags(item4->flags() & (~Qt::ItemIsEditable));
    ui->tbwOtherResource->setItem(rowcount, 0, item1);
    ui->tbwOtherResource->setItem(rowcount, 1, item2);
    ui->tbwOtherResource->setItem(rowcount, 2, item3);
    ui->tbwOtherResource->setItem(rowcount, 3, item4);
    ui->tbwOtherResource->item(rowcount, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tbwOtherResource->item(rowcount, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tbwOtherResource->item(rowcount, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tbwOtherResource->item(rowcount, 3)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tbwOtherResource_2->removeRow(mrow);
}
void CDlgActionEdit::on_pbtnLeft_0_Clicked()
{
    auto item = ui->tbwArea->currentItem();
    std::string strArea;
    int mrow;
    if (NULL == item)
    {
        mrow = ui->tbwArea->rowCount() - 1;
        if (mrow < 0)
        {
            return;
        }
        strArea = ui->tbwArea->item(mrow, 2)->text().toStdString();
    }
    else
    {
        mrow = item->row();
        strArea = ui->tbwArea->item(mrow, 2)->text().toStdString();
    }
    CZoneInfo airspace;
    for (int i = 0; i < airspaceInfoVec.size(); i++)
    {
        if (airspaceInfoVec[i].GetID() == strArea)
        {
            airspace = airspaceInfoVec[i];
            selectedAreaVec.push_back(airspaceInfoVec[i]);
            break;
        }
    }

    int rowcount = ui->tbwArea_2->rowCount();
    ui->tbwArea_2->insertRow(rowcount);
    QTableWidgetItem *item1 = new QTableWidgetItem(QString::number(rowcount + 1));
    QString strName = QString::fromStdString(airspace.GetName());
    QTableWidgetItem *item2 = new QTableWidgetItem(strName);
    QString strId = QString::fromStdString(airspace.GetID());
    QTableWidgetItem *item3 = new QTableWidgetItem(strId);

    ui->tbwArea_2->setItem(rowcount, 0, item1);
    ui->tbwArea_2->setItem(rowcount, 1, item2);
    ui->tbwArea_2->setItem(rowcount, 2, item3);
    ui->tbwArea_2->item(rowcount, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tbwArea_2->item(rowcount, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tbwArea_2->item(rowcount, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    Update_tbwMark();

    ui->tbwArea->removeRow(mrow);
}
void CDlgActionEdit::on_pbtnRight_0_Clicked()
{
    auto item = ui->tbwArea_2->currentItem();
    std::string strId;
    int mrow;
    if (NULL == item)
    {
        mrow = ui->tbwArea_2->rowCount() - 1;
        if (mrow < 0)
        {
            return;
        }
        strId = ui->tbwArea_2->item(mrow, 2)->text().toStdString();
    }
    else
    {
        mrow = item->row();
        strId = ui->tbwArea_2->item(mrow, 2)->text().toStdString();
    }

    CZoneInfo airSpace;
    for (int i = 0; i < selectedAreaVec.size(); i++)
    {
        if (selectedAreaVec[i].GetID() == strId)
        {
            airSpace = selectedAreaVec[i];
            selectedAreaVec.erase(selectedAreaVec.begin() + i);
            break;
        }
    }
    ui->tbwArea_2->removeRow(mrow);

    int rowNum = ui->tbwArea->rowCount();
    ui->tbwArea->insertRow(rowNum);
    QTableWidgetItem *item1 = new QTableWidgetItem(QString::number(rowNum + 1));
    QString strName = QString::fromStdString(airSpace.GetName());
    QTableWidgetItem *item2 = new QTableWidgetItem(strName);
    QString airspaceID = QString::fromStdString(airSpace.GetID());
    QTableWidgetItem *item3 = new QTableWidgetItem(airspaceID);

    ui->tbwArea->setItem(rowNum, 0, item1);
    ui->tbwArea->setItem(rowNum, 1, item2);
    ui->tbwArea->setItem(rowNum, 2, item3);
    ui->tbwArea->item(rowNum, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tbwArea->item(rowNum, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tbwArea->item(rowNum, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    Update_tbwMark();

    std::vector<std::string> strMarkName;
    for (auto target : targetInRegionVec)
    {
        strMarkName.push_back(target.GetName());
    }

    for (int i = 0; i < selectedMarkVec.size(); i++)
    {
        if (::find(strMarkName.begin(), strMarkName.end(), selectedMarkVec[i].GetName()) == strMarkName.end())
        {
            selectedMarkVec.erase(selectedMarkVec.begin() + i);
            --i;
        }
    }
    UpdateMark_2();
}
void CDlgActionEdit::on_pbtnRight_1_Clicked()
{
    auto item = ui->tbwPlatResource_2->currentItem();
    int mrow;
    std::string strplatID;
    if (NULL == item)
    {
        mrow = ui->tbwPlatResource_2->rowCount() - 1;
        if (mrow < 0)
        {
            return;
        }
        strplatID = ui->tbwPlatResource_2->item(mrow, 2)->text().toStdString();
    }
    else
    {
        mrow = item->row();
        strplatID = ui->tbwPlatResource_2->item(mrow, 2)->text().toStdString();
    }

    NebulaResources platResource;
    for (int i = 0; i < selectedPlatVec.size(); i++)
    {
        if (selectedPlatVec[i].first.GetLoadName() == strplatID)
        {
            platResource = selectedPlatVec[i].first;
            selectedPlatVec.erase(selectedPlatVec.begin() + i);
            break;
        }
    }

    int rowcount = ui->tbwPlatResource->rowCount();
    ui->tbwPlatResource->insertRow(rowcount);
    QTableWidgetItem *item1 = new QTableWidgetItem(QString::number(rowcount + 1));
    QTableWidgetItem *item2 = new QTableWidgetItem(QString::number(platResource.GetLoadModel()));
    QString strId = QString::fromStdString(platResource.GetLoadName());
    QTableWidgetItem *item3 = new QTableWidgetItem(strId);
    ui->tbwPlatResource->setItem(rowcount, 0, item1);
    ui->tbwPlatResource->setItem(rowcount, 1, item2);
    ui->tbwPlatResource->setItem(rowcount, 2, item3);
    ui->tbwPlatResource->item(rowcount, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tbwPlatResource->item(rowcount, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tbwPlatResource->item(rowcount, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    ui->tbwPlatResource_2->removeRow(mrow);
}
void CDlgActionEdit::on_pbtnLeft_1_Clicked()
{
    auto item = ui->tbwPlatResource->currentItem();
    int mrow;
    std::string strplatID;
    if (NULL == item)
    {
        mrow = ui->tbwPlatResource->rowCount() - 1;
        if (mrow < 0)
        {
            return;
        }
        strplatID = ui->tbwPlatResource->item(mrow, 2)->text().toStdString();
    }
    else
    {
        mrow = item->row();
        strplatID = ui->tbwPlatResource->item(mrow, 2)->text().toStdString();
    }

    NebulaResources platResource;
    for (auto plat : platResourceVec)
    {
        if (plat.GetLoadName() == strplatID)
        {
            platResource = plat;
            selectedPlatVec.push_back(std::pair<NebulaResources, int>(plat, 1));
            break;
        }
    }

    int rowcount = ui->tbwPlatResource_2->rowCount();
    ui->tbwPlatResource_2->insertRow(rowcount);
    QTableWidgetItem *item1 = new QTableWidgetItem(QString::number(rowcount + 1));
    QTableWidgetItem *item2 = new QTableWidgetItem(QString::number(platResource.GetLoadModel()));
    QString strId = QString::fromStdString(platResource.GetLoadName());
    QTableWidgetItem *item3 = new QTableWidgetItem(strId);
    ui->tbwPlatResource_2->setItem(rowcount, 0, item1);
    ui->tbwPlatResource_2->setItem(rowcount, 1, item2);
    ui->tbwPlatResource_2->setItem(rowcount, 2, item3);

    QSpinBox *spinBox = new QSpinBox;
    spinBox->setValue(1);
    ui->tbwPlatResource_2->setCellWidget(rowcount, 3, spinBox);

    ui->tbwPlatResource_2->item(rowcount, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tbwPlatResource_2->item(rowcount, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tbwPlatResource_2->item(rowcount, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    ui->tbwPlatResource->removeRow(mrow);
}
void CDlgActionEdit::on_pbtnLeft_2_Clicked()
{
    auto item = ui->tbwAmmoResource->currentItem();
    int mrow;
    std::string strammoID;
    if (NULL == item)
    {
        mrow = ui->tbwAmmoResource->rowCount() - 1;
        if (mrow < 0)
        {
            return;
        }
        strammoID = ui->tbwAmmoResource->item(mrow, 2)->text().toStdString();
    }
    else
    {
        mrow = item->row();
        strammoID = ui->tbwAmmoResource->item(mrow, 2)->text().toStdString();
    }
    NebulaResources Resource;
    for (auto Ammo : AmmoResourceVec)
    {
        if (Ammo.GetLoadName() == strammoID)
        {
            Resource = Ammo;
            selectedAmmoVec.push_back(std::pair<NebulaResources, int>(Resource, 1));
            break;
        }
    }

    int rowcount = ui->tbwAmmoResource_2->rowCount();
    ui->tbwAmmoResource_2->insertRow(rowcount);
    QTableWidgetItem *item1 = new QTableWidgetItem(QString::number(rowcount + 1));
    QTableWidgetItem *item2 = new QTableWidgetItem(QString::number(Resource.GetLoadModel()));
    QString strId = QString::fromStdString(Resource.GetLoadName());
    QTableWidgetItem *item3 = new QTableWidgetItem(strId);

    ui->tbwAmmoResource_2->setItem(rowcount, 0, item1);
    ui->tbwAmmoResource_2->setItem(rowcount, 1, item2);
    ui->tbwAmmoResource_2->setItem(rowcount, 2, item3);
    QSpinBox *spinBox = new QSpinBox;
    spinBox->setValue(1);
    ui->tbwAmmoResource_2->setCellWidget(rowcount, 3, spinBox);
    ui->tbwAmmoResource_2->item(rowcount, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tbwAmmoResource_2->item(rowcount, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tbwAmmoResource_2->item(rowcount, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    ui->tbwAmmoResource->removeRow(mrow);
}
void CDlgActionEdit::on_pbtnRight_2_Clicked()
{
    auto item = ui->tbwAmmoResource_2->currentItem();
    int mrow;
    std::string strAmmoID;
    if (NULL == item)
    {
        mrow = ui->tbwAmmoResource_2->rowCount() - 1;
        if (mrow < 0)
        {
            return;
        }
        strAmmoID = ui->tbwAmmoResource_2->item(mrow, 2)->text().toStdString();
    }
    else
    {
        mrow = item->row();
        strAmmoID = ui->tbwAmmoResource_2->item(mrow, 2)->text().toStdString();
    }

    NebulaResources Resource;
    for (int i = 0; i < selectedAmmoVec.size(); i++)
    {
        if (selectedAmmoVec[i].first.GetLoadName() == strAmmoID)
        {
            Resource = selectedAmmoVec[i].first;
            selectedAmmoVec.erase(selectedAmmoVec.begin() + i);
            break;
        }
    }

    int rowcount = ui->tbwAmmoResource->rowCount();
    ui->tbwAmmoResource->insertRow(rowcount);
    QTableWidgetItem *item1 = new QTableWidgetItem(QString::number(rowcount + 1));
    QTableWidgetItem *item2 = new QTableWidgetItem(QString::number(Resource.GetLoadModel()));
    QString strId = QString::fromStdString(Resource.GetLoadName());
    QTableWidgetItem *item3 = new QTableWidgetItem(strId);
    ui->tbwAmmoResource->setItem(rowcount, 0, item1);
    ui->tbwAmmoResource->setItem(rowcount, 1, item2);
    ui->tbwAmmoResource->setItem(rowcount, 2, item3);
    ui->tbwAmmoResource->item(rowcount, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tbwAmmoResource->item(rowcount, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tbwAmmoResource->item(rowcount, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tbwAmmoResource_2->removeRow(mrow);
}
void CDlgActionEdit::on_pbtnRight_3_Clicked()
{
    auto item = ui->tbwMark_2->currentItem();
    int mrow;
    std::string tar_id;
    if (NULL == item)
    {
        mrow = ui->tbwMark_2->rowCount() - 1;
        if (mrow < 0)
        {
            return;
        }
        tar_id = ui->tbwMark_2->item(mrow, 2)->text().toStdString();
    }
    else
    {
        mrow = item->row();
        tar_id = ui->tbwMark_2->item(mrow, 2)->text().toStdString();
    }

    SystemMarkInfo tmpMark;
    for (int i = 0; i < selectedMarkVec.size(); i++)
    {
        if (selectedMarkVec[i].GetID() == tar_id)
        {
            tmpMark = selectedMarkVec[i];
            selectedMarkVec.erase(selectedMarkVec.begin() + i);
            break;
        }
    }

    if (!IsMarkInRegion(tmpMark))
    {
        ui->tbwMark_2->removeRow(mrow);
        return;
    }
    int rowcount = ui->tbwMark->rowCount();
    ui->tbwMark->insertRow(rowcount);
    QTableWidgetItem *item1 = new QTableWidgetItem(QString::number(rowcount + 1));
    QString strName = QString::fromStdString(tmpMark.GetName());
    QTableWidgetItem *item2 = new QTableWidgetItem(strName);
    QString strId = QString::fromStdString(tmpMark.GetID());
    QTableWidgetItem *item3 = new QTableWidgetItem(strId);

    ui->tbwMark->setItem(rowcount, 0, item1);
    ui->tbwMark->setItem(rowcount, 1, item2);
    ui->tbwMark->setItem(rowcount, 2, item3);
    ui->tbwMark->item(rowcount, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tbwMark->item(rowcount, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tbwMark->item(rowcount, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    ui->tbwMark_2->removeRow(mrow);
    sorttbwMark();
    sorttbwMark_2();
}
void CDlgActionEdit::on_pbtnLeft_4_Clicked()
{
    auto item = ui->tbwSensorResource->currentItem();
    int mrow;
    std::string strplatID;
    if (NULL == item)
    {
        mrow = ui->tbwSensorResource->rowCount() - 1;
        if (mrow < 0)
        {
            return;
        }
        strplatID = ui->tbwSensorResource->item(mrow, 2)->text().toStdString();
    }
    else
    {
        mrow = item->row();
        strplatID = ui->tbwSensorResource->item(mrow, 2)->text().toStdString();
    }

    NebulaResources sensorResource;
    for (auto sensor : SensorResourceVec)
    {
        if (sensor.GetLoadName() == strplatID)
        {
            sensorResource = sensor;
            selectedSensorVec.push_back(std::pair<NebulaResources, int>(sensor, 1));
            break;
        }
    }

    int rowcount = ui->tbwSensorResource_2->rowCount();
    ui->tbwSensorResource_2->insertRow(rowcount);
    QTableWidgetItem *item1 = new QTableWidgetItem(QString::number(rowcount + 1));
    QTableWidgetItem *item2 = new QTableWidgetItem(QString::number(sensorResource.GetLoadModel()));
    QString strId = QString::fromStdString(sensorResource.GetLoadName());
    QTableWidgetItem *item3 = new QTableWidgetItem(strId);
    ui->tbwSensorResource_2->setItem(rowcount, 0, item1);
    ui->tbwSensorResource_2->setItem(rowcount, 1, item2);
    ui->tbwSensorResource_2->setItem(rowcount, 2, item3);

    QSpinBox *spinBox = new QSpinBox;
    spinBox->setValue(1);
    ui->tbwSensorResource_2->setCellWidget(rowcount, 3, spinBox);
    ui->tbwSensorResource_2->item(rowcount, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tbwSensorResource_2->item(rowcount, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tbwSensorResource_2->item(rowcount, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tbwSensorResource->removeRow(mrow);
}
void CDlgActionEdit::on_pbtnRight_4_Clicked()
{
    auto item = ui->tbwSensorResource_2->currentItem();
    int mrow;
    std::string strsensorID;
    if (NULL == item)
    {
        mrow = ui->tbwSensorResource_2->rowCount() - 1;
        if (mrow < 0)
        {
            return;
        }
        strsensorID = ui->tbwSensorResource_2->item(mrow, 2)->text().toStdString();
    }
    else
    {
        mrow = item->row();
        strsensorID = ui->tbwSensorResource_2->item(mrow, 2)->text().toStdString();
    }

    NebulaResources sensorResource;
    for (int i = 0; i < selectedSensorVec.size(); i++)
    {
        if (selectedSensorVec[i].first.GetLoadName() == strsensorID)
        {
            sensorResource = selectedSensorVec[i].first;
            selectedSensorVec.erase(selectedSensorVec.begin() + i);
            break;
        }
    }
    int rowcount = ui->tbwSensorResource->rowCount();
    ui->tbwSensorResource->insertRow(rowcount);
    QTableWidgetItem *item1 = new QTableWidgetItem(QString::number(rowcount + 1));
    QTableWidgetItem *item2 = new QTableWidgetItem(QString::number(sensorResource.GetLoadModel()));
    QString strId = QString::fromStdString(sensorResource.GetLoadName());
    QTableWidgetItem *item3 = new QTableWidgetItem(strId);
    ui->tbwSensorResource->setItem(rowcount, 0, item1);
    ui->tbwSensorResource->setItem(rowcount, 1, item2);
    ui->tbwSensorResource->setItem(rowcount, 2, item3);
    ui->tbwSensorResource->item(rowcount, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tbwSensorResource->item(rowcount, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tbwSensorResource->item(rowcount, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    ui->tbwSensorResource_2->removeRow(mrow);
}
void CDlgActionEdit::on_pbtnLeft_5_Clicked()
{
    auto item = ui->tbwOtherResource->currentItem();
    int mrow;
    std::string strotherID;
    if (NULL == item)
    {
        mrow = ui->tbwOtherResource->rowCount() - 1;
        if (mrow < 0)
        {
            return;
        }
        strotherID = ui->tbwOtherResource->item(mrow, 2)->text().toStdString();
    }
    else
    {
        mrow = item->row();
        strotherID = ui->tbwOtherResource->item(mrow, 2)->text().toStdString();
    }

    NebulaResources Resource;
    for (auto other : OtherResourceVec)
    {
        if (other.GetLoadName() == strotherID)
        {
            Resource = other;
            selectedOtherVec.push_back(std::pair<NebulaResources, int>(Resource, 1));
            break;
        }
    }

    int rowcount = ui->tbwOtherResource_2->rowCount();
    ui->tbwOtherResource_2->insertRow(rowcount);
    QTableWidgetItem *item1 = new QTableWidgetItem(QString::number(rowcount + 1));
    QTableWidgetItem *item2 = new QTableWidgetItem(QString::number(Resource.GetLoadModel()));
    QString strId = QString::fromStdString(Resource.GetLoadName());
    QTableWidgetItem *item3 = new QTableWidgetItem(strId);

    ui->tbwOtherResource_2->setItem(rowcount, 0, item1);
    ui->tbwOtherResource_2->setItem(rowcount, 1, item2);
    ui->tbwOtherResource_2->setItem(rowcount, 2, item3);
    QSpinBox *spinBox = new QSpinBox;
    spinBox->setValue(1);
    ui->tbwOtherResource_2->setCellWidget(rowcount, 3, spinBox);
    ui->tbwOtherResource_2->item(rowcount, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tbwOtherResource_2->item(rowcount, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tbwOtherResource_2->item(rowcount, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    ui->tbwOtherResource->removeRow(mrow);
}
void CDlgActionEdit::on_pbtnRight_5_Clicked()
{
    auto item = ui->tbwOtherResource_2->currentItem();
    int mrow;
    std::string strOtherID;
    if (NULL == item)
    {
        mrow = ui->tbwOtherResource_2->rowCount() - 1;
        if (mrow < 0)
        {
            return;
        }
        strOtherID = ui->tbwOtherResource_2->item(mrow, 2)->text().toStdString();
    }
    else
    {
        mrow = item->row();
        strOtherID = ui->tbwOtherResource_2->item(mrow, 2)->text().toStdString();
    }

    NebulaResources Resource;
    for (int i = 0; i < selectedOtherVec.size(); i++)
    {
        if (selectedOtherVec[i].first.GetLoadName() == strOtherID)
        {
            Resource = selectedOtherVec[i].first;
            selectedOtherVec.erase(selectedOtherVec.begin() + i);
            break;
        }
    }

    int rowcount = ui->tbwOtherResource->rowCount();
    ui->tbwOtherResource->insertRow(rowcount);
    QTableWidgetItem *item1 = new QTableWidgetItem(QString::number(rowcount + 1));
    QTableWidgetItem *item2 = new QTableWidgetItem(QString::number(Resource.GetLoadModel()));
    QString strId = QString::fromStdString(Resource.GetLoadName());
    QTableWidgetItem *item3 = new QTableWidgetItem(strId);
    QTableWidgetItem *item4 = new QTableWidgetItem(QString::number(1));
    item4->setFlags(item4->flags() & (~Qt::ItemIsEditable));
    ui->tbwOtherResource->setItem(rowcount, 0, item1);
    ui->tbwOtherResource->setItem(rowcount, 1, item2);
    ui->tbwOtherResource->setItem(rowcount, 2, item3);
    ui->tbwOtherResource->setItem(rowcount, 3, item4);
    ui->tbwOtherResource->item(rowcount, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tbwOtherResource->item(rowcount, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tbwOtherResource->item(rowcount, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tbwOtherResource->item(rowcount, 3)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tbwOtherResource_2->removeRow(mrow);
}
void CDlgActionEdit::on_pbtnLeft_3_Clicked()
{
    auto item = ui->tbwMark->currentItem();
    int mrow;
    std::string tar_id;
    if (NULL == item)
    {
        mrow = ui->tbwMark->rowCount() - 1;
        if (mrow < 0)
        {
            return;
        }
        tar_id = ui->tbwMark->item(mrow, 2)->text().toStdString();
    }
    else
    {
        mrow = item->row();
        tar_id = ui->tbwMark->item(mrow, 2)->text().toStdString();
    }

    SystemMarkInfo tmpMark;
    for (auto tar : targetInfoVec)
    {
        if (tar.GetID() == tar_id)
        {
            tmpMark = tar;
            selectedMarkVec.push_back(tar);
            break;
        }
    }

    int rowcount = ui->tbwMark_2->rowCount();
    ui->tbwMark_2->insertRow(rowcount);
    QTableWidgetItem *item1 = new QTableWidgetItem(QString::number(rowcount + 1));
    QString strName = QString::fromStdString(tmpMark.GetName());
    QTableWidgetItem *item2 = new QTableWidgetItem(strName);
    QString strId = QString::fromStdString(tmpMark.GetID());
    QTableWidgetItem *item3 = new QTableWidgetItem(strId);

    ui->tbwMark_2->setItem(rowcount, 0, item1);
    ui->tbwMark_2->setItem(rowcount, 1, item2);
    ui->tbwMark_2->setItem(rowcount, 2, item3);
    ui->tbwMark_2->item(rowcount, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tbwMark_2->item(rowcount, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tbwMark_2->item(rowcount, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    ui->tbwMark->removeRow(mrow);
    sorttbwMark();
    sorttbwMark_2();
}

void CDlgActionEdit::GetTextFromLedtID()
{
    ActionID = ui->lineEdit_ActionID->text().toInt();
}

void CDlgActionEdit::GetTextFromLedtName()
{
    ActionName = ui->lineEdit_ActionName->text().toStdString();
}

void CDlgActionEdit::GetTextFromLedtDH()
{
    ActionDH = ui->lineEdit_ActionCode->text().toStdString();
}

void CDlgActionEdit::GetTextFromLedtAim()
{
    ActionAim = ui->textEdit_ActionMark->toPlainText().toStdString();
}

void CDlgActionEdit::GetTextFromLedtRemark()
{
    ActionRemark = ui->textEdit_ActionNote->toPlainText().toStdString();
}

void CDlgActionEdit::GetTextFromLedtVisibility()
{
    Visibility = ui->ledtVisibility->text().toStdString();
}

void CDlgActionEdit::GetTextFromcbxSeason(const QString &text)
{
    Season = ui->cbxSeason->currentText().toStdString();
}

void CDlgActionEdit::GetTextFromcbxDayNight(const QString &text)
{
    DayorNight = ui->cbxDayNight->currentText().toStdString();
}

void CDlgActionEdit::GetTextFromcbxWeather(const QString &text)
{
    Weather = ui->cbxWeather->currentText().toStdString();
}

void CDlgActionEdit::GetTextFromcbxWindType(const QString &text)
{
    WindType = ui->cbxWindType->currentText().toStdString();
}

void CDlgActionEdit::GetTextFromcbxWindLevel(const QString &text)
{
    WindLevel = ui->cbxWindLevel->currentText().toStdString();
}

void CDlgActionEdit::GetTextFromDateTimeStart(const QDateTime &datetime)
{
    ActionStartTime = ui->dateTimeEdit_Begin->text().toStdString();
}

void CDlgActionEdit::GetTextFromDateTimeEnd(const QDateTime &datetime)
{
    ActionEndTime = ui->dateTimeEdit_End->text().toStdString();
}

void CDlgActionEdit::GetStateFromCbxActive(int state)
{
    if (ui->cbxActiveState->checkState() == Qt::Checked)
    {
        isActive = true;
    }
    else
    {
        isActive = false;
    }

}

