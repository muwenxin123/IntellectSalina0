#include "CDlgMissionPlatformInfo.h"
#include "ui_CDlgMissionPlatformInfo.h"
#include "QCString.h"
#include "LyMessageBox.h"
#include "ITaskManagerActivator.h"
#include "lY_TaskManage.h"
CDlgMissionPlatformInfo::CDlgMissionPlatformInfo(QWidget *parent) :
    LyDialog(tr("Equipment Selection"), parent),
    ui(new Ui::CDlgMissionPlatformInfo)
{
    ui->setupUi(centralWidget());
    this->setResizable(true);
    this->raise();
    this->setWindowFlags(this->windowFlags()& Qt::WindowCloseButtonHint);
    ui->lineEditPlatType->setReadOnly(true);
    ui->lineEditPlatNum->setReadOnly(true);
    m_pITaskManageService = ITaskManagerActivator::Get().getPTaskManageService();
    connect(ui->pbtn_Ok, &QPushButton::clicked, this, &CDlgMissionPlatformInfo::on_pbtn_Ok_clicked);
    connect(ui->pbtn_Cancel, &QPushButton::clicked, this, &CDlgMissionPlatformInfo::on_pbtn_Cancel_clicked);
    connect(ui->cbxPodTypeMain, SIGNAL(activated(const QString &)), this, SLOT(on_currentIndexChangedMountMain(const QString &)));
    connect(ui->cbxPodTypeOrder, SIGNAL(activated(const QString &)), this, SLOT(on_currentIndexChangedMountOrder(const QString &)));
    connect(ui->cbxMountWQMain, SIGNAL(activated(const QString &)), this, SLOT(on_currentIndexChangedWQMain(const QString &)));
    connect(ui->cbxMountWQOrder, SIGNAL(activated(const QString &)), this, SLOT(on_currentIndexChangedWQOrder(const QString &)));
}

CDlgMissionPlatformInfo::~CDlgMissionPlatformInfo()
{
    delete ui;
}

void CDlgMissionPlatformInfo::Init(QString platType, int currentRow)
{
    this->currentRow = currentRow;
    if (m_pITaskManageService == nullptr)
    {
        return;
    }

    int nActivatingID = m_pITaskManageService->GetActivatingActionID();

    if (nActivatingID == -1)
    {
        return;
    }
    SensorResourceVec = m_pITaskManageService->GetRelationSensorResource(nActivatingID);
    AmmoResourceVec = m_pITaskManageService->GetRelationAmmoResource(nActivatingID);
    m_CRelationMissionAmmoVec = LY_TaskManage::Get().GetDlgMissionEdit()->m_CRelationMissionAmmoVec;
    ui->lineEditPlatType->setText(platType);
    ui->lineEditPlatNum->setText(QString::number(1));
    ui->cbxPodTypeMain->clear();
    ui->cbxPodTypeOrder->clear();
    ui->cbxMountWQMain->clear();
    ui->cbxMountWQOrder->clear();
    for (auto data : SensorResourceVec)
    {
        ui->cbxPodTypeMain->addItem(QString::fromStdString(data.GetResourceName()));
    }
    ui->cbxPodTypeMain->addItem(QString::fromStdString(""));

    for (auto data : SensorResourceVec)
    {
        if (ui->cbxPodTypeMain->currentText().toStdString() != data.GetResourceName())
        {
            ui->cbxPodTypeOrder->addItem(QString::fromStdString(data.GetResourceName().c_str()));
        }
    }
    ui->cbxPodTypeOrder->addItem(QString::fromStdString(""));
    ui->cbxPodTypeOrder->setCurrentText(QString::fromStdString(""));

    for (auto data : m_CRelationMissionAmmoVec)
    {
		const string &strAmmoName = data.GetAmmoName();
        ui->cbxMountWQMain->addItem(tr2(strAmmoName.c_str()));
    }
    ui->cbxMountWQMain->addItem(QString::fromStdString(""));

    for (auto data : m_CRelationMissionAmmoVec)
    {
        const string &strAmmoName = data.GetAmmoName();
        string MainAmmoName = tr3(ui->cbxMountWQMain->currentText());
        if (MainAmmoName != strAmmoName)
        {
            ui->cbxMountWQOrder->addItem(tr2(strAmmoName.c_str()));
        }
    }
    ui->cbxMountWQOrder->addItem(QString::fromStdString(""));
    ui->cbxMountWQOrder->setCurrentText(QString::fromStdString(""));
    QString strAmmoMain = ui->cbxMountWQMain->currentText();
    int WQCountNum = LY_TaskManage::Get().GetDlgMissionEdit()->CountcurrentWQNum(strAmmoMain);
    ui->spinBoxWQMainNum->setValue(0);
    for (auto data : m_CRelationMissionAmmoVec)
    {
        string MainAmmoName = tr3(ui->cbxMountWQMain->currentText());
		const std::string &strAmmoName = data.GetAmmoName();
        if (MainAmmoName == strAmmoName)
        {
            ui->spinBoxWQMainNum->setMaximum(data.GetAmmoNum() - WQCountNum);
        }
    }

    ui->spinBoxWQOrderNum->setValue(0);
    ui->spinBoxWQOrderNum->setMaximum(0);
}

void CDlgMissionPlatformInfo::showEvent(QShowEvent *event)
{

}

void CDlgMissionPlatformInfo::on_pbtn_Ok_clicked()
{
    this->close();
    platfrominfo.platType = ui->lineEditPlatType->text();
    platfrominfo.PodTypeMain = ui->cbxPodTypeMain->currentText();
    platfrominfo.PodTypeOrder = ui->cbxPodTypeOrder->currentText();
    platfrominfo.MountWQMain = ui->cbxMountWQMain->currentText();
    platfrominfo.MountWQMainNum = ui->spinBoxWQMainNum->text().toInt();
    platfrominfo.MountWQOrder = ui->cbxMountWQOrder->currentText();
    platfrominfo.MountWQOrderNum = ui->spinBoxWQOrderNum->text().toInt();
    LY_TaskManage::Get().GetDlgMissionEdit()->UpdateEquSelect(platfrominfo);
    LY_TaskManage::Get().GetDlgMissionEdit()->UpdateEquBackup(currentRow);
    LY_TaskManage::Get().GetDlgMissionEdit()->SortPlatType();
}

void CDlgMissionPlatformInfo::on_pbtn_Cancel_clicked()
{
    this->close();
}

void CDlgMissionPlatformInfo::on_currentIndexChangedMountMain(const QString &text)
{
    if (SensorResourceVec.size() == 0)
    {
        LyMessageBox::information(NULL, tr("tip"), tr("No pod type available for current action!"));
        return;
    }

    if (text != "")
    {
        const QString str = ui->cbxPodTypeOrder->currentText();
        ui->cbxPodTypeOrder->clear();
        for (auto data : SensorResourceVec)
        {
            ui->cbxPodTypeOrder->addItem(QString::fromStdString(data.GetResourceName()));
        }
        ui->cbxPodTypeOrder->addItem(QString::fromStdString(""));
        ui->cbxPodTypeOrder->setCurrentText(QString::fromStdString(""));
        int index = ui->cbxPodTypeOrder->findText(text);
        ui->cbxPodTypeOrder->removeItem(index);
        int index1 = ui->cbxPodTypeOrder->findText(str);
        ui->cbxPodTypeOrder->setCurrentIndex(index1);
    }
    else
    {
        ui->cbxPodTypeOrder->clear();
        for (auto data : SensorResourceVec)
        {
            ui->cbxPodTypeOrder->addItem(QString::fromStdString(data.GetResourceName()));
        }
        ui->cbxPodTypeOrder->addItem(QString::fromStdString(""));
        ui->cbxPodTypeOrder->setCurrentText(QString::fromStdString(""));
        ui->cbxPodTypeMain->clear();
        for (auto data : SensorResourceVec)
        {
            ui->cbxPodTypeMain->addItem(QString::fromStdString(data.GetResourceName()));
        }
        ui->cbxPodTypeMain->addItem(QString::fromStdString(""));
        ui->cbxPodTypeMain->setCurrentText(QString::fromStdString(""));
    }

}

void CDlgMissionPlatformInfo::on_currentIndexChangedMountOrder(const QString &text)
{
    if (SensorResourceVec.size() != 0 && ui->cbxPodTypeMain->currentText() == tr(""))
    {
        LyMessageBox::information(NULL, tr("tip"), tr("Please set up the main pod first!"));
        ui->cbxPodTypeOrder->clear();
        for (auto data : SensorResourceVec)
        {
            ui->cbxPodTypeOrder->addItem(QString::fromStdString(data.GetResourceName()));
        }
        ui->cbxPodTypeOrder->addItem(QString::fromStdString(""));
        ui->cbxPodTypeOrder->setCurrentText(QString::fromStdString(""));
        return;
    }
    if (SensorResourceVec.size() == 0)
    {
        LyMessageBox::information(NULL, tr("tip"), tr("No sub pod type available for current action!"));
        return;
    }
    if (SensorResourceVec.size() == 1)
    {
        LyMessageBox::information(NULL, tr("tip"), tr("No mount weapon type available for current action!"));
        return;
    }

    if (text != tr2(""))
    {
        QString str = ui->cbxPodTypeMain->currentText();
        ui->cbxPodTypeMain->clear();
        for (auto data : SensorResourceVec)
        {
            ui->cbxPodTypeMain->addItem(QString::fromStdString(data.GetResourceName()));
        }
        ui->cbxPodTypeMain->addItem(QString::fromStdString(""));
        int index = ui->cbxPodTypeMain->findText(text);
        ui->cbxPodTypeMain->removeItem(index);
        ui->cbxPodTypeMain->setCurrentText(str);
    }
}

void CDlgMissionPlatformInfo::on_currentIndexChangedWQMain(const QString &text)
{
    if (m_CRelationMissionAmmoVec.size() == 0)
    {
        LyMessageBox::information(NULL, tr("tip"), tr("No mount weapon type available for current action!"));
        return;
    }
    if (text != tr2(""))
    {
        const QString str = ui->cbxMountWQOrder->currentText();
        ui->cbxMountWQOrder->clear();
        for (auto data : m_CRelationMissionAmmoVec)
        {
			const std::string &AmmoName = data.GetAmmoName();
            ui->cbxMountWQOrder->addItem(tr2(AmmoName.c_str()));
        }
        ui->cbxMountWQOrder->addItem(QString::fromStdString(""));
        ui->cbxMountWQOrder->setCurrentText(QString::fromStdString(""));
        int index = ui->cbxMountWQOrder->findText(text);
        ui->cbxMountWQOrder->removeItem(index);
        int index1 = ui->cbxMountWQOrder->findText(str);
        ui->cbxMountWQOrder->setCurrentIndex(index1);

        int MountMainNum = 0;
        for (auto data : m_CRelationMissionAmmoVec)
        {
            string strAmmoName = tr3(text);
            const std::string &AmmoName = data.GetAmmoName();
            if (AmmoName == strAmmoName)
            {
                MountMainNum = data.GetAmmoNum();
            }
        }
        int WQCountNum = LY_TaskManage::Get().GetDlgMissionEdit()->CountcurrentWQNum(text);
        ui->spinBoxWQMainNum->setValue(0);
        ui->spinBoxWQOrderNum->setValue(0);
        ui->spinBoxWQMainNum->setMaximum(MountMainNum - WQCountNum);
    }
    else
    {
        ui->cbxMountWQOrder->clear();
        for (auto data : m_CRelationMissionAmmoVec)
        {
            const std::string &AmmoName = data.GetAmmoName();
            ui->cbxMountWQOrder->addItem(tr2(AmmoName.c_str()));
        }
        ui->cbxMountWQOrder->addItem(QString::fromStdString(""));
        ui->cbxMountWQOrder->setCurrentText(QString::fromStdString(""));
        ui->cbxMountWQMain->clear();
        for (auto data : m_CRelationMissionAmmoVec)
        {
            const std::string &AmmoName = data.GetAmmoName();
            ui->cbxMountWQMain->addItem(tr2(AmmoName.c_str()));
        }
        ui->cbxMountWQMain->addItem(QString::fromStdString(""));
        ui->cbxMountWQMain->setCurrentText(QString::fromStdString(""));

        int MountMainNum = 0;
        int WQCountNum = LY_TaskManage::Get().GetDlgMissionEdit()->CountcurrentWQNum(text);
        ui->spinBoxWQMainNum->setValue(0);
        ui->spinBoxWQOrderNum->setValue(0);
        ui->spinBoxWQMainNum->setMaximum(MountMainNum - WQCountNum);
    }
}

void CDlgMissionPlatformInfo::on_currentIndexChangedWQOrder(const QString &text)
{
    if (m_CRelationMissionAmmoVec.size() != 0 && ui->cbxMountWQMain->currentText() == tr(""))
    {
        LyMessageBox::information(NULL, tr("tip"), tr("Please set up the main mount weapon first!"));
        ui->cbxMountWQOrder->clear();
        for (auto data : m_CRelationMissionAmmoVec)
        {
            const std::string &AmmoName = data.GetAmmoName();
            ui->cbxMountWQOrder->addItem(tr2(AmmoName.c_str()));
        }
        ui->cbxMountWQOrder->addItem(QString::fromStdString(""));
        ui->cbxMountWQOrder->setCurrentText(QString::fromStdString(""));
        return;
    }
    if (m_CRelationMissionAmmoVec.size() == 0)
    {
        LyMessageBox::information(NULL, tr("tip"), tr("No mount weapon type available for current action!"));
        return;
    }
    if (m_CRelationMissionAmmoVec.size() == 1)
    {
        LyMessageBox::information(NULL, tr("tip"), tr("No secondary mount weapon type available for current action!"));
        return;
    }
    if (text != tr2(""))
    {
        QString str = ui->cbxMountWQMain->currentText();
        ui->cbxMountWQMain->clear();
        for (auto data : m_CRelationMissionAmmoVec)
        {
            const std::string &AmmoName = data.GetAmmoName();
            ui->cbxMountWQMain->addItem(tr2(AmmoName.c_str()));
        }
        ui->cbxMountWQMain->addItem(QString::fromStdString(""));
        int index = ui->cbxMountWQMain->findText(text);
        ui->cbxMountWQMain->removeItem(index);
        ui->cbxMountWQMain->setCurrentText(str);
    }
    else
    {
        QString str = ui->cbxMountWQMain->currentText();
        ui->cbxMountWQMain->clear();
        for (auto data : m_CRelationMissionAmmoVec)
        {
            const std::string &AmmoName = data.GetAmmoName();
            ui->cbxMountWQMain->addItem(tr2(AmmoName.c_str()));
        }
        ui->cbxMountWQMain->addItem(QString::fromStdString(""));
        ui->cbxMountWQMain->setCurrentText(str);
    }
    int MountOrderNum = 0;
    string strAmmoName;
    if (text == "")
    {
        strAmmoName = "";
    }
    else
    {
        string str = tr3(text);
        strAmmoName = str;
    }

    for (auto data : m_CRelationMissionAmmoVec)
    {
        const std::string &AmmoName = data.GetAmmoName();
        if (AmmoName == strAmmoName)
        {
            MountOrderNum = data.GetAmmoNum();
        }
    }
    int WQCountNum = LY_TaskManage::Get().GetDlgMissionEdit()->CountcurrentWQNum(text);
    ui->spinBoxWQOrderNum->setValue(0);
    ui->spinBoxWQOrderNum->setMaximum(MountOrderNum - WQCountNum);
}
