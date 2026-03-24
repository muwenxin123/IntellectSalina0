#include "CDlgMarkSchemeList.h"
#include "QCString.h"
#include "lY_TaskManage.h"
CDlgMarkSchemeList::CDlgMarkSchemeList(QWidget *parent) :
    LyDialog(tr("Choose Scheme"), parent),
    ui(new Ui::CDlgMarkSchemeList)
{
    this->setResizable(true);
    this->resize(800, 600);
    ui->setupUi(centralWidget());
    m_pITaskManageService = ITaskManagerActivator::Get().getPTaskManageService();
    if (m_pITaskManageService == nullptr)
    {
        return;
    }
    connect(ui->tbwMarkScheme, SIGNAL(itemDoubleClicked(QTableWidgetItem *)), this, SLOT(on_tbwMarkScheme_itemDoubleClicked(QTableWidgetItem *)));
    connect(ui->btnOk, SIGNAL(clicked()), this, SLOT(on_btnOk_Clicked()));
    connect(ui->btnCancel, SIGNAL(clicked()), this, SLOT(on_btnCancel_Clicked()));
}

CDlgMarkSchemeList::~CDlgMarkSchemeList()
{
}

void CDlgMarkSchemeList::Init()
{
    m_sMarkID = LY_TaskManage::Get().GetDlgMissionEdit()->m_sSelectMarkID;
    GetMarkSchemeData();
    CreateTableWidget();
}

void CDlgMarkSchemeList::GetMarkSchemeData()
{
    m_cNebulaSchemeVec = m_pITaskManageService->GetSchemeByMarkID(m_sMarkID);

    QStringList headerlabels;
    headerlabels << tr("No.") << tr("Scheme ID") << tr("Scheme Information");
    ui->tbwMarkScheme->clear();
    ui->tbwMarkScheme->setColumnCount(headerlabels.size());
    ui->tbwMarkScheme->setHorizontalHeaderLabels(headerlabels);
    ui->tbwMarkScheme->verticalHeader()->setHidden(true);
    ui->tbwMarkScheme->setSelectionBehavior(QTableWidget::SelectRows);
    ui->tbwMarkScheme->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tbwMarkScheme->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tbwMarkScheme->setAlternatingRowColors(true);
    ui->tbwMarkScheme->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->tbwMarkScheme->horizontalHeader()->setStretchLastSection(true);
}

void CDlgMarkSchemeList::CreateTableWidget()
{
    ui->tbwMarkScheme->setRowCount(m_cNebulaSchemeVec.size());
    for (int i = 0; i < m_cNebulaSchemeVec.size(); i++)
    {
        ui->tbwMarkScheme->setItem(i, 0, new QTableWidgetItem(QString::number(i + 1)));
        ui->tbwMarkScheme->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(m_cNebulaSchemeVec[i].GetSchemeID().c_str())));
        auto SchemeSensor = ITaskManagerActivator::Get().getPTaskManageService()->GetSensorBySchemeID(m_cNebulaSchemeVec[i].GetSchemeID());
        auto SchemeAmmo = ITaskManagerActivator::Get().getPTaskManageService()->GetAmmoBySchemeID(m_cNebulaSchemeVec[i].GetSchemeID());
        int scType = m_cNebulaSchemeVec[i].GetSchemeType();
        QString SchemeInfoStr;
        if (scType == 0)
        {
            for (auto ammo : SchemeAmmo)
            {
                QString temp;
                temp = "(";
                temp.append(QString::fromStdString(ammo.GetAmmoType()));
                temp.append(",");
                temp.append(QString::fromStdString(ammo.GetShotPointID()));
                temp.append(",");
                temp.append(QString::fromStdString(ammo.GetShotTime()));
                temp.append(");");
                SchemeInfoStr.append(temp);
            }
            for (auto sensor : SchemeSensor)
            {
                QString temp;
                temp = "(";
                temp.append(QString::fromStdString(sensor.GetSensorType()));
                temp.append(",");
                temp.append(QString::fromStdString(sensor.GetPowerOnPointID()));
                temp.append(",");
                temp.append(QString::fromStdString(sensor.GetPowerOnTime()));
                temp.append(",");
                temp.append(QString::fromStdString(sensor.GetWorkPointID()));
                temp.append(",");
                temp.append(QString::fromStdString(sensor.GetWorkTime()));
                temp.append(",");
                temp.append(QString::fromStdString(sensor.GetPowerOffPointID()));
                temp.append(",");
                temp.append(QString::fromStdString(sensor.GetPowerOffTime()));
                temp.append(");");
                SchemeInfoStr.append(temp);
            }
        }
        else if (scType == 1)
        {
            for (auto ammo : SchemeAmmo)
            {
                QString temp;
                temp = "(";
                temp.append(QString::fromStdString(ammo.GetAmmoType()));
                temp.append(",");
                temp.append(QString::fromStdString(ammo.GetShotPointID()));
                temp.append(",");
                temp.append(QString::fromStdString(ammo.GetShotTime()));
                temp.append(");");
                SchemeInfoStr.append(temp);
            }
            for (auto sensor : SchemeSensor)
            {
                QString temp;
                temp = "(";
                temp.append(QString::fromStdString(sensor.GetSensorType()));
                temp.append(",");
                temp.append(QString::fromStdString(sensor.GetPowerOnPointID()));
                temp.append(",");
                temp.append(QString::fromStdString(sensor.GetPowerOnTime()));
                temp.append(",");
                temp.append(QString::fromStdString(sensor.GetWorkPointID()));
                temp.append(",");
                temp.append(QString::fromStdString(sensor.GetWorkTime()));
                temp.append(",");
                temp.append(QString::fromStdString(sensor.GetPowerOffPointID()));
                temp.append(",");
                temp.append(QString::fromStdString(sensor.GetPowerOffTime()));
                temp.append(");");
                SchemeInfoStr.append(temp);
            }
        }
        else
        {
            for (auto ammo : SchemeAmmo)
            {
                QString temp;
                temp = "(";
                temp.append(QString::fromStdString(ammo.GetAmmoType()));
                temp.append(",");
                temp.append(QString::fromStdString(ammo.GetShotPointID()));
                temp.append(",");
                temp.append(QString::fromStdString(ammo.GetShotTime()));
                temp.append(");");
                SchemeInfoStr.append(temp);
            }

            for (auto sensor : SchemeSensor)
            {
                QString temp;
                temp.append("(");
                temp.append(QString::fromStdString(sensor.GetSensorType()));
                temp.append(",");
                temp.append(QString::fromStdString(sensor.GetPowerOnPointID()));
                temp.append(",");
                temp.append(QString::fromStdString(sensor.GetPowerOnTime()));
                temp.append(",");
                temp.append(QString::fromStdString(sensor.GetWorkPointID()));
                temp.append(",");
                temp.append(QString::fromStdString(sensor.GetWorkTime()));
                temp.append(",");
                temp.append(QString::fromStdString(sensor.GetPowerOffPointID()));
                temp.append(",");
                temp.append(QString::fromStdString(sensor.GetPowerOffTime()));
                temp.append(");");
                SchemeInfoStr.append(temp);
            }
        }
        ui->tbwMarkScheme->setItem(i, 2, new QTableWidgetItem(SchemeInfoStr));
        ui->tbwMarkScheme->item(i, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->tbwMarkScheme->item(i, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->tbwMarkScheme->item(i, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    }
}

void CDlgMarkSchemeList::on_tbwMarkScheme_itemDoubleClicked(QTableWidgetItem *item)
{
    int row = ui->tbwMarkScheme->currentRow();
    CNebulaSchemeAmmoVec SchemeAmmoVec = m_pITaskManageService->GetAmmoBySchemeID(ui->tbwMarkScheme->item(row, 1)->text().toStdString());
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
    CRelationMissionAmmoVec tempVec2;
    tempVec2 = LY_TaskManage::Get().GetDlgMissionEdit()->GettbwSelectedAmmoData();
    for (auto data : tempVec2)
    {
        auto it = mAmmoNameNum.find(data.GetAmmoName());
        if (it != mAmmoNameNum.end())
        {
            mAmmoNameNum[data.GetAmmoName()] += data.GetAmmoNum();
        }
        else
        {
            mAmmoNameNum.insert(data.GetAmmoName(), data.GetAmmoNum());
        }
    }
    QMap<std::string, std::string> MarkAndSchemeID;
    std::string sSchemeID;
    MarkAndSchemeID = LY_TaskManage::Get().GetDlgMissionEdit()->m_mMarkAndSchemeID;
    auto it = MarkAndSchemeID.find(m_sMarkID);
    if (it != MarkAndSchemeID.end())
    {
        sSchemeID = MarkAndSchemeID[m_sMarkID];
        MarkAndSchemeID[m_sMarkID] = ui->tbwMarkScheme->item(row, 1)->text().toStdString();
    }
    else
    {
        MarkAndSchemeID.insert(m_sMarkID, ui->tbwMarkScheme->item(row, 1)->text().toStdString());
    }
    LY_TaskManage::Get().GetDlgMissionEdit()->m_mMarkAndSchemeID = MarkAndSchemeID;
    if (!sSchemeID.empty())
    {
        CNebulaSchemeAmmoVec SchemeAmmoVec = m_pITaskManageService->GetAmmoBySchemeID(sSchemeID);
        for (auto data : SchemeAmmoVec)
        {
            auto it = mAmmoNameNum.find(data.GetAmmoType());
            if (it != mAmmoNameNum.end())
            {
                mAmmoNameNum[data.GetAmmoType()] -= data.GetAmmoNum();
            }
            else
            {
                mAmmoNameNum.insert(data.GetAmmoType(), 1);
            }
        }
    }

    CRelationMissionAmmoVec tempVec1;
    for (auto it = mAmmoNameNum.begin(); it != mAmmoNameNum.end(); ++it)
    {
        if (it.value() > 0)
        {
            CRelationMissionAmmo missionAmmo;
            missionAmmo.SetAmmoName(it.key());
            missionAmmo.SetAmmoNum(it.value());
            tempVec1.push_back(missionAmmo);
        }
    }
    LY_TaskManage::Get().GetDlgMissionEdit()->m_CRelationMissionAmmoVec = tempVec1;
    LY_TaskManage::Get().GetDlgMissionEdit()->UpdateAmmoSelect();
    LY_TaskManage::Get().GetDlgMissionEdit()->UpdataAmmoBackup();
}

void CDlgMarkSchemeList::on_btnOk_Clicked()
{
    int row = ui->tbwMarkScheme->currentRow();
    CNebulaSchemeAmmoVec SchemeAmmoVec = m_pITaskManageService->GetAmmoBySchemeID(ui->tbwMarkScheme->item(row, 1)->text().toStdString());
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
    CRelationMissionAmmoVec tempVec2;
    tempVec2 = LY_TaskManage::Get().GetDlgMissionEdit()->GettbwSelectedAmmoData();
    for (auto data : tempVec2)
    {
        auto it = mAmmoNameNum.find(data.GetAmmoName());
        if (it != mAmmoNameNum.end())
        {
            mAmmoNameNum[data.GetAmmoName()] += data.GetAmmoNum();
        }
        else
        {
            mAmmoNameNum.insert(data.GetAmmoName(), data.GetAmmoNum());
        }
    }
    QMap<std::string, std::string> MarkAndSchemeID;
    std::string sSchemeID;
    MarkAndSchemeID = LY_TaskManage::Get().GetDlgMissionEdit()->m_mMarkAndSchemeID;
    auto it = MarkAndSchemeID.find(m_sMarkID);
    if (it != MarkAndSchemeID.end())
    {
        sSchemeID = MarkAndSchemeID[m_sMarkID];
        MarkAndSchemeID[m_sMarkID] = ui->tbwMarkScheme->item(row, 1)->text().toStdString();
    }
    else
    {
        MarkAndSchemeID.insert(m_sMarkID, ui->tbwMarkScheme->item(row, 1)->text().toStdString());
    }
    LY_TaskManage::Get().GetDlgMissionEdit()->m_mMarkAndSchemeID = MarkAndSchemeID;
    if (!sSchemeID.empty())
    {
        CNebulaSchemeAmmoVec SchemeAmmoVec = m_pITaskManageService->GetAmmoBySchemeID(sSchemeID);
        for (auto data : SchemeAmmoVec)
        {
            auto it = mAmmoNameNum.find(data.GetAmmoType());
            if (it != mAmmoNameNum.end())
            {
                mAmmoNameNum[data.GetAmmoType()] -= data.GetAmmoNum();
            }
            else
            {
                mAmmoNameNum.insert(data.GetAmmoType(), 1);
            }
        }
    }

    CRelationMissionAmmoVec tempVec1;
    for (auto it = mAmmoNameNum.begin(); it != mAmmoNameNum.end(); ++it)
    {
        if (it.value() > 0)
        {
            CRelationMissionAmmo missionAmmo;
            missionAmmo.SetAmmoName(it.key());
            missionAmmo.SetAmmoNum(it.value());
            tempVec1.push_back(missionAmmo);
        }
    }
    LY_TaskManage::Get().GetDlgMissionEdit()->m_CRelationMissionAmmoVec = tempVec1;
    LY_TaskManage::Get().GetDlgMissionEdit()->UpdateAmmoSelect();
    LY_TaskManage::Get().GetDlgMissionEdit()->UpdataAmmoBackup();
    this->close();
}

void CDlgMarkSchemeList::on_btnCancel_Clicked()
{
    this->close();
}

