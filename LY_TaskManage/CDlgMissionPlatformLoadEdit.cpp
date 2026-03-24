#include "CDlgMissionPlatformLoadEdit.h"
#include "ui_CDlgMissionPlatformLoadEdit.h"
#include "QCString.h"
#include "ITaskManagerActivator.h"
#include "LyMessageBox.h"
#include "lY_TaskManage.h"

CDlgMissionPlatformLoadEdit::CDlgMissionPlatformLoadEdit(QWidget *parent) :
    LyDialog(tr("Task Payload Editing"), parent),
    ui(new Ui::CDlgMissionPlatformLoadEdit)
{
    ui->setupUi(centralWidget());
    this->setResizable(true);
    this->resize(560, 700);

    m_pITaskManageService = ITaskManagerActivator::Get().getPTaskManageService();
    m_pIMarkManageService = ITaskManagerActivator::Get().getPIMarkManageService();

    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(on_pushButton_clicked()));
    connect(ui->pushButton_2, SIGNAL(clicked()), this, SLOT(on_pushButton_2_clicked()));

    connect(ui->comboBox_2, SIGNAL(currentIndexChanged(const QString)), this, SLOT(on_comboBox_2_currentIndexChanged(const QString)));
    connect(ui->comboBox_12, SIGNAL(currentIndexChanged(const QString)), this, SLOT(on_comboBox_12_currentIndexChanged(const QString)));
    connect(ui->comboBox_9, SIGNAL(currentIndexChanged(const QString)), this, SLOT(on_comboBox_9_currentIndexChanged(const QString)));
    connect(ui->comboBox_14, SIGNAL(currentIndexChanged(const QString)), this, SLOT(on_comboBox_14_currentIndexChanged(const QString)));
    connect(ui->comboBox_3, SIGNAL(currentIndexChanged(const QString)), this, SLOT(on_comboBox_3_currentIndexChanged(const QString)));

    InitData();

}

CDlgMissionPlatformLoadEdit::~CDlgMissionPlatformLoadEdit()
{
    delete ui;
}

void CDlgMissionPlatformLoadEdit::InitData()
{
    QStringList list01, list02, list03;
    list01 << tr("-") << tr("Destroy") << tr("Damage") << tr("Suppression") << tr("Interference") << tr("Turn off") << tr("Surveillance") << tr("Block") << tr("Search and Discovery") << tr("Reconnaissance Mark Elements") << tr("Protection");
    list02 << tr("Not Specified") << tr("Aim and Launch") << tr("Fixed Point Throw") << tr("Lock on Lauch") << tr("Autonomous Aim Lauch") << tr("Autonomous Locking Lauch") << tr("Autonomous Vertex Throwing") << tr("other");
    list03 << tr("Ammunition") << tr("Pod") << tr("Communication Station");

    ui->comboBox_11->addItems(list01);
    ui->comboBox_13->addItems(list01);
    ui->comboBox_10->addItems(list02);
    ui->comboBox_14->addItems(list03);

    ui->textEdit->setText(tr(""));
    ui->lineEdit->setText(tr(""));
    ui->lineEdit_2->setText(tr(""));
    ui->lineEdit_3->setText(tr(""));
    ui->lineEdit_4->setText(tr(""));
    ui->lineEdit_5->setText(tr(""));
    ui->lineEdit_8->setText(tr(""));
    ui->lineEdit_9->setText(tr(""));

    ui->lineEdit_11->setText(tr(""));
    ui->lineEdit_12->setText(tr(""));

    ui->lineEdit_15->setText(tr(""));
    ui->lineEdit_14->setText(tr(""));

    m_DataType = 0;

    ui->lineEdit_5->setEnabled(false);
    ui->lineEdit_4->setEnabled(false);
    ui->lineEdit_8->setEnabled(false);

    m_vecNodeInfo = LY_TaskManage::Get().GetDlgMissionEdit()->GetNodeInfoVec();
}

void CDlgMissionPlatformLoadEdit::InitWindowCtrl()
{
    ui->textEdit->setText(tr(""));
    ui->lineEdit->setText(tr(""));
    ui->lineEdit_2->setText(tr(""));
    ui->lineEdit_3->setText(tr(""));
    ui->lineEdit_4->setText(tr(""));
    ui->lineEdit_5->setText(tr(""));
    ui->lineEdit_8->setText(tr(""));
    ui->lineEdit_9->setText(tr(""));

    ui->lineEdit_11->setText(tr(""));
    ui->lineEdit_12->setText(tr(""));

    ui->lineEdit_15->setText(tr(""));
    ui->lineEdit_14->setText(tr(""));

    ui->comboBox_11->setCurrentIndex(0);
    ui->comboBox_13->setCurrentIndex(0);
    ui->comboBox_10->setCurrentIndex(0);
    ui->comboBox_14->setCurrentIndex(0);

    ui->lineEdit->setEnabled(true);

    InitTimeEdit();

}

void CDlgMissionPlatformLoadEdit::SetWindowInfo(CNebulaMissionPlatformLoad &stInfo)
{
    m_stMissionPlatformLoad = stInfo;

    ui->lineEdit->setText(tr2(stInfo.strID));
    ui->lineEdit_2->setText(tr2(stInfo.strMode));
	ui->lineEdit_3->setText(QString::number(stInfo.nNum));

    ui->lineEdit_9->setText(tr2(stInfo.strThrowTime));
	ui->comboBox_10->setCurrentIndex(stInfo.ePeltMode);

    ui->lineEdit_15->setText(tr2(stInfo.strWorkTime));
    ui->lineEdit_11->setText(tr2(stInfo.strScanRange));
    ui->lineEdit_12->setText(tr2(stInfo.strScanSpeed));
    ui->lineEdit_14->setText(tr2(stInfo.streOperate));
	ui->comboBox_14->setCurrentIndex(stInfo.eType);
    ui->textEdit->setText(tr2(stInfo.strRemark));
}

void CDlgMissionPlatformLoadEdit::ShowTop()
{

}

void CDlgMissionPlatformLoadEdit::SetDataType(int nType)
{
    m_DataType = nType;
}

void CDlgMissionPlatformLoadEdit::InitTimeEdit()
{

}

void CDlgMissionPlatformLoadEdit::UpdateMissionCombobox()
{
    if (m_pITaskManageService == nullptr)
    {
        return;
    }
    ui->comboBox_3->disconnect();

    CNeubulaMissionVec MissionVec;
    std::string strMode = m_stMissionPlatformLoad.strMode;
    MissionVec = m_pITaskManageService->GetMissionByLoadModel(strMode);

    ui->comboBox_3->clear();

    QStringList strList;
    strList.push_back(tr("-"));
    for (auto data : MissionVec)
    {
        strList.push_back(QString::fromStdString(data.GetID()));
    }
    ui->comboBox_3->addItems(strList);
    ui->comboBox_3->setCurrentIndex(0);

    if (m_DataType == 2)
    {
        QString strID1 = tr2(m_stMissionPlatformLoad.strMissionID);

        int nIndex01 = ui->comboBox_3->findText(strID1);

        if (nIndex01 != -1)
        {
            ui->comboBox_3->setCurrentIndex(nIndex01);
        }
    }
    connect(ui->comboBox_3, SIGNAL(currentIndexChanged(const QString)), this, SLOT(on_comboBox_3_currentIndexChanged(const QString)));
}

void CDlgMissionPlatformLoadEdit::SetTypeControl(bool bStates)
{

    ui->comboBox->setEnabled(bStates);
    ui->lineEdit_9->setEnabled(bStates);
    ui->comboBox_10->setEnabled(bStates);
    ui->lineEdit_15->setEnabled(!bStates);

    ui->comboBox_4->setEnabled(!bStates);
    ui->lineEdit_11->setEnabled(!bStates);
    ui->lineEdit_12->setEnabled(!bStates);
    ui->lineEdit_14->setEnabled(!bStates);
}

void CDlgMissionPlatformLoadEdit::showEvent(QShowEvent *event)
{
    UpdateMissionCombobox();

}

void CDlgMissionPlatformLoadEdit::closeEvent(QCloseEvent *event)
{

}

void CDlgMissionPlatformLoadEdit::on_pushButton_clicked()
{
    if (ui->lineEdit->text() == "")
    {
        LyMessageBox::information(NULL, tr("tip"), tr("The number cannot be empty, please reenter"));
        return;
    }
	CNeubulaMission stMission;
	CNebulaMissionPlatformLoad	stPlatformLoad;

    strncpy(stPlatformLoad.strID, ui->lineEdit->text().toLocal8Bit(), MAX_PLATFORM_LOAD_LEN_1);
    strncpy(stPlatformLoad.strMode, ui->lineEdit_2->text().toLocal8Bit(), MAX_PLATFORM_LOAD_LEN_1);
    strncpy(stPlatformLoad.strPlatformID, ui->comboBox_9->currentText().toLocal8Bit(), MAX_PLATFORM_LOAD_LEN_1);
    strncpy(stPlatformLoad.strPlatformMode, ui->lineEdit_5->text().toLocal8Bit(), MAX_PLATFORM_LOAD_LEN_1);
    strncpy(stPlatformLoad.strMissionID, ui->comboBox_3->currentText().toLocal8Bit(), MAX_PLATFORM_LOAD_LEN_1);
    strncpy(stPlatformLoad.strMarkID01, ui->comboBox_2->currentText().toLocal8Bit(), MAX_PLATFORM_LOAD_LEN_1);
    strncpy(stPlatformLoad.strMarkName01, ui->lineEdit_4->text().toLocal8Bit(), MAX_PLATFORM_LOAD_LEN_1);
    strncpy(stPlatformLoad.strMarkID02, ui->comboBox_12->currentText().toLocal8Bit(), MAX_PLATFORM_LOAD_LEN_1);
    strncpy(stPlatformLoad.strMarkName02, ui->lineEdit_8->text().toLocal8Bit(), MAX_PLATFORM_LOAD_LEN_1);
    strncpy(stPlatformLoad.strThrowPointName, ui->comboBox->currentText().toLocal8Bit(), MAX_PLATFORM_LOAD_LEN_1);
    strncpy(stPlatformLoad.strThrowTime, ui->lineEdit_9->text().toLocal8Bit(), MAX_PLATFORM_LOAD_LEN_1);
    strncpy(stPlatformLoad.strWorkPointName, ui->comboBox_4->currentText().toLocal8Bit(), MAX_PLATFORM_LOAD_LEN_1);
    strncpy(stPlatformLoad.strWorkTime, ui->lineEdit_15->text().toLocal8Bit(), MAX_PLATFORM_LOAD_LEN_1);
    strncpy(stPlatformLoad.strScanRange, ui->lineEdit_11->text().toLocal8Bit(), MAX_PLATFORM_LOAD_LEN_1);
    strncpy(stPlatformLoad.strScanSpeed, ui->lineEdit_12->text().toLocal8Bit(), MAX_PLATFORM_LOAD_LEN_1);
    strncpy(stPlatformLoad.streOperate, ui->lineEdit_14->text().toLocal8Bit(), MAX_PLATFORM_LOAD_LEN_1);
    strncpy(stPlatformLoad.strRemark, ui->textEdit->toPlainText().toLocal8Bit(), MAX_PLATFORM_LOAD_LEN_1);
	stPlatformLoad.nNum = ui->lineEdit_3->text().toInt();
    stPlatformLoad.EMarkHurm01 = stMission.GetHurtDegreeENum(ui->comboBox_11->currentIndex());
    stPlatformLoad.EMarkHurm02 = stMission.GetHurtDegreeENum(ui->comboBox_13->currentIndex());
	stPlatformLoad.ePeltMode = stPlatformLoad.GetPeltModeENum(ui->comboBox_10->currentIndex());
	stPlatformLoad.eType = stPlatformLoad.GetTypeENum(ui->comboBox_14->currentIndex());

    if (m_DataType == 1 && m_pITaskManageService != nullptr)
    {
        std::string strID = stPlatformLoad.strID;
        bool bresut = m_pITaskManageService->IsMissionPlatformLoadExisted(strID);
        if (bresut == true)
        {
            LyMessageBox::information(nullptr, tr("tip"), tr("ID duplicate, please modify"));
            return;
        }

        bresut = m_pITaskManageService->SetMissionPlatformLoad(stPlatformLoad);
        if (bresut == true)
        {

        }
        else
        {
            LyMessageBox::information(NULL, tr("tip"), tr("Add failed"));
        }
        m_DataType = 0;
    }

    else if (m_DataType == 2)
    {
        bool bresut = m_pITaskManageService->SetMissionPlatformLoad(stPlatformLoad);
        if (bresut == true)
        {

        }
        else
        {
            LyMessageBox::information(NULL, tr("tip"), tr("Add failed"));
        }
        m_DataType = 0;
    }
    ui->lineEdit->setEnabled(true);
    this->hide();

}

void CDlgMissionPlatformLoadEdit::on_pushButton_2_clicked()
{
    this->hide();
}

void CDlgMissionPlatformLoadEdit::on_comboBox_9_currentIndexChanged(const QString &arg1)
{
    if (arg1 == "-")
    {
        ui->lineEdit_5->setText("");
    }
    else
    {
        if (m_pITaskManageService == nullptr)
        {
            return;
        }
        CNebulaMissionPlatform stInfo;
        stInfo = m_pITaskManageService->GetMissionPlatform(arg1.toStdString());
        ui->lineEdit_5->setText(tr2(stInfo.strMode));
    }
}

void CDlgMissionPlatformLoadEdit::on_comboBox_2_currentIndexChanged(const QString &arg1)
{
    if (arg1 == "-")
    {
        ui->lineEdit_4->setText("");
    }
    else
    {
        if (m_pIMarkManageService == nullptr)
        {
            return;
        }
        SystemMarkInfo stInfo;
        stInfo = m_pIMarkManageService->GetMark(arg1.toStdString());
        ui->lineEdit_4->setText(QString::fromStdString(stInfo.GetName()));
    }
}

void CDlgMissionPlatformLoadEdit::on_comboBox_12_currentIndexChanged(const QString &arg1)
{
    if (arg1 == "-")
    {
        ui->lineEdit_8->setText("");
    }
    else
    {
        if (m_pIMarkManageService == nullptr)
        {
            return;
        }
        SystemMarkInfo stInfo;
        stInfo = m_pIMarkManageService->GetMark(arg1.toStdString());
        ui->lineEdit_8->setText(QString::fromStdString(stInfo.GetName()));
    }
}

void CDlgMissionPlatformLoadEdit::on_comboBox_14_currentIndexChanged(const QString &arg1)
{
    if (arg1.compare(tr("Ammunition")) == 0)
    {
        SetTypeControl(true);
    }
    else if (arg1.compare(tr("Pod")) == 0)
    {
        SetTypeControl(false);
    }
}

void CDlgMissionPlatformLoadEdit::on_comboBox_3_currentIndexChanged(const QString &arg1)
{
    if (arg1 == "-")
    {

        ui->comboBox_2->clear();
        ui->lineEdit_4->setText("");
        ui->comboBox_11->setCurrentIndex(0);

        ui->comboBox_12->clear();
        ui->lineEdit_8->setText("");
        ui->comboBox_13->setCurrentIndex(0);

        ui->comboBox_9->clear();
        ui->lineEdit_5->setText("");
    }
    else
    {
        if (m_pITaskManageService == nullptr)
        {
            return;
        }

        CNeubulaMission stMission = m_pITaskManageService->GetMission(arg1.toStdString());
        strncpy(m_stMissionPlatformLoad.strMissionID, arg1.toLocal8Bit(), MAX_PLATFORM_LOAD_LEN_1);

    }
}
