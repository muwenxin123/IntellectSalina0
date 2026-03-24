#include "CDlgMissionPlatformEdit.h"
#include "ui_CDlgMissionPlatformEdit.h"
#include "ITaskManagerActivator.h"
#include "QCString.h"
#include "LyMessageBox.h"
#include "lY_TaskManage.h"

CDlgMissionPlatformEdit::CDlgMissionPlatformEdit(QWidget *parent) :
    LyDialog(tr("Task Platform Editor"), parent),
    ui(new Ui::CDlgMissionPlatformEdit)
{
    ui->setupUi(centralWidget());
    this->setResizable(true);
    this->resize(460, 600);

    m_pITaskManageService = ITaskManagerActivator::Get().getPTaskManageService();
    m_pIMarkManageService = ITaskManagerActivator::Get().getPIMarkManageService();
    m_pIFormationManageService = ITaskManagerActivator::Get().getPFormationManageService();

    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(on_pushButton_clicked()));
    connect(ui->pushButton_2, SIGNAL(clicked()), this, SLOT(on_pushButton_2_clicked()));
    connect(ui->comboBox_2,  SIGNAL(currentIndexChanged(const QString)), this, SLOT(on_comboBox_2_currentIndexChanged(const QString)));
    connect(ui->comboBox_12, SIGNAL(currentIndexChanged(const QString)), this, SLOT(on_comboBox_12_currentIndexChanged(const QString)));
    connect(ui->comboBox_10, SIGNAL(currentIndexChanged(const QString)), this, SLOT(on_comboBox_10_currentIndexChanged(const QString)));
    connect(ui->comboBox_3, SIGNAL(currentIndexChanged(const QString)), this, SLOT(on_comboBox_3_currentIndexChanged(const QString)));

    InitData();
}

CDlgMissionPlatformEdit::~CDlgMissionPlatformEdit()
{
    delete ui;
}

void CDlgMissionPlatformEdit::InitData()
{
    QStringList list01, list02, list03;
    list01 << tr("-") << tr("Destroy") << tr("Damage") << tr("Suppression") << tr("Interference") << tr("Turn off") << tr("Surveillance") << tr("Block") << tr("Search and Discovery") << tr("Reconnaissance Mark Elements") << tr("Protection");
    list02 << tr("UAV") << tr("UV");
    list03 << tr("Unassigned") << tr("SJW01") << tr("SJW02") << tr("SJW03");

    ui->lineEdit_4->setEnabled(false);
    ui->lineEdit_8->setEnabled(false);

    ui->comboBox_11->addItems(list01);
    ui->comboBox_13->addItems(list01);
    ui->comboBox_4->addItems(list03);
    ui->comboBox_5->addItems(list02);
    ui->textEdit->setText(tr(""));

    ui->lineEdit->setText(tr(""));
    ui->lineEdit_2->setText(tr(""));
    ui->lineEdit_4->setText(tr(""));
    ui->lineEdit_8->setText(tr(""));

    ui->lineEdit_16->setText(tr(""));

    m_DataType = 0;

    readFormsFromFile();
}

void CDlgMissionPlatformEdit::InitWindowCtrl()
{
    ui->textEdit->setText(tr(""));

    ui->lineEdit->setText(tr(""));
    ui->lineEdit_2->setText(tr(""));
    ui->lineEdit_4->setText(tr(""));
    ui->lineEdit_8->setText(tr(""));

    ui->lineEdit_16->setText(tr(""));

    ui->comboBox_11->setCurrentIndex(0);
    ui->comboBox_13->setCurrentIndex(0);
    ui->comboBox_4->setCurrentIndex(0);
    ui->comboBox_5->setCurrentIndex(0);

    ui->lineEdit->setEnabled(true);

    InitTimeEdit();
}

void CDlgMissionPlatformEdit::SetWindowInfo(CNebulaMissionPlatform &stInfo)
{
    m_stMissionPlatform = stInfo;

    ui->lineEdit->setText(tr2(stInfo.strID));
    ui->lineEdit_2->setText(tr2(stInfo.strMode));

    ui->lineEdit_16->setText(tr2(stInfo.strSeat));
    ui->textEdit->setText(tr2(stInfo.strRemark));

    if (m_DataType == 2)
    {
        ui->lineEdit->setEnabled(false);

        int nIndex01 = ui->comboBox_4->findText(tr2(m_stMissionPlatform.strUser));
        if (nIndex01 != -1)
        {
            ui->comboBox_4->setCurrentIndex(nIndex01);
        }
        ui->comboBox_5->setCurrentIndex(stInfo.EType);

    }
}

void CDlgMissionPlatformEdit::ShowTop()
{

}

void CDlgMissionPlatformEdit::SetDataType(int nType)
{
    m_DataType = nType;
}

void CDlgMissionPlatformEdit::InitTimeEdit()
{

}

void CDlgMissionPlatformEdit::UpdateMarkCombobox()
{
    if (m_pIMarkManageService == nullptr)
    {
        return;
    }
    CNeubulaMission stMission;

    std::string strMissionID = m_stMissionPlatform.strMissionID;
    stMission = m_pITaskManageService->GetMission(strMissionID);

    ui->comboBox_2->clear();
    ui->comboBox_12->clear();

    QStringList strList;
    strList.push_back(tr("-"));

    ui->comboBox_2->addItems(strList);
    ui->comboBox_12->addItems(strList);

    ui->comboBox_2->setCurrentIndex(0);
    ui->comboBox_12->setCurrentIndex(0);

    if (m_DataType == 2)
    {
        QString strID1 = tr2(m_stMissionPlatform.strMarkID01);
        QString strID2 = tr2(m_stMissionPlatform.strMarkID02);

        int nIndex01 = ui->comboBox_2->findText(strID1);
        int nIndex02 = ui->comboBox_12->findText(strID2);

        if (nIndex01 != -1)
        {
            ui->comboBox_2->setCurrentIndex(nIndex01);
        }
        if (nIndex02 != -1)
        {
            ui->comboBox_12->setCurrentIndex(nIndex02);
        }

        CNeubulaMission stInfo;
        ui->comboBox_11->setCurrentIndex(stInfo.GetHurtDegreeENum(m_stMissionPlatform.EMarkHurm01));
        ui->comboBox_13->setCurrentIndex(stInfo.GetHurtDegreeENum(m_stMissionPlatform.EMarkHurm02));

    }
}

void CDlgMissionPlatformEdit::UpdateMissionCombobox()
{
    if (m_pITaskManageService == nullptr)
    {
        return;
    }
    ui->comboBox_3->disconnect();
    CNeubulaMissionVec MissionVec;
    std::string strMode = m_stMissionPlatform.strMode;
    MissionVec = m_pITaskManageService->GetMissionByModel(strMode);

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
        QString strID1 = tr2(m_stMissionPlatform.strMissionID);

        int nIndex01 = ui->comboBox_3->findText(strID1);

        if (nIndex01 != -1)
        {
            ui->comboBox_3->setCurrentIndex(nIndex01);
        }
    }
    connect(ui->comboBox_3, SIGNAL(currentIndexChanged(const QString)), this, SLOT(on_comboBox_3_currentIndexChanged(const QString)));
}

void CDlgMissionPlatformEdit::UpdateFormCombobox()
{
    ui->comboBox_10->setCurrentIndex(0);
    if (m_DataType == 2)
    {
        int nIndex01 = ui->comboBox_10->findText(tr2(m_stMissionPlatform.strForm));
        if (nIndex01 != -1)
        {
            ui->comboBox_10->setCurrentIndex(nIndex01);
        }

        int nIndex02 = ui->comboBox_8->findText(tr2(m_stMissionPlatform.strFormID));
        if (nIndex02 != -1)
        {
            ui->comboBox_8->setCurrentIndex(nIndex02);
        }
    }
}

bool CDlgMissionPlatformEdit::readFormsFromFile()
{
    if (!m_pIFormationManageService)
    {
        return false;
    }
    m_vFormList_ALL.clear();

    m_pIFormationManageService->GetFormationList(m_vFormList_ALL);

    QStringList strList;
    strList.push_back("-");
    for (auto form : m_vFormList_ALL)
    {
        strList.push_back(QString::fromStdString(form.m_strName));
    }
    ui->comboBox_10->addItems(strList);

    return true;
}

void CDlgMissionPlatformEdit::showEvent(QShowEvent *event)
{
    UpdateMissionCombobox();
    UpdateMarkCombobox();

    UpdateFormCombobox();

}

void CDlgMissionPlatformEdit::closeEvent(QCloseEvent *event)
{

}

void CDlgMissionPlatformEdit::on_pushButton_clicked()
{
    if (ui->lineEdit->text() == "")
    {
        LyMessageBox::information(NULL, tr("tip"), tr("The platform number cannot be empty, please reenter"));
        return;
    }

    if (ui->lineEdit_2->text() == "")
    {
        LyMessageBox::information(NULL, tr("tip"), tr("The platform model cannot be empty, please reenter"));
        return;
    }

    CNeubulaMission stMission;
    CNebulaMissionPlatform	stPlatform;

    strncpy(stPlatform.strID, ui->lineEdit->text().toLocal8Bit(), MAX_PLATFORM_LEN_1);
    strncpy(stPlatform.strMode, ui->lineEdit_2->text().toLocal8Bit(), MAX_PLATFORM_LEN_1);
    strncpy(stPlatform.strGZ01, ui->comboBox_7->currentText().toLocal8Bit(), MAX_PLATFORM_LEN_1);
    strncpy(stPlatform.strGZ02, ui->comboBox_6->currentText().toLocal8Bit(), MAX_PLATFORM_LEN_1);
    strncpy(stPlatform.strForm, ui->comboBox_10->currentText().toLocal8Bit(), MAX_PLATFORM_LEN_1);
    strncpy(stPlatform.strUser, ui->comboBox_4->currentText().toLocal8Bit(), MAX_PLATFORM_LEN_1);
    strncpy(stPlatform.strSeat, ui->lineEdit_16->text().toLocal8Bit(), MAX_PLATFORM_LEN_1);
    strncpy(stPlatform.strFormID, ui->comboBox_8->currentText().toLocal8Bit(), MAX_PLATFORM_LEN_1);
    strncpy(stPlatform.strLineID, ui->comboBox->currentText().toLocal8Bit(), MAX_PLATFORM_LEN_1);
    strncpy(stPlatform.strRemark, ui->textEdit->toPlainText().toLocal8Bit(), MAX_PLATFORM_LEN_1);
    strncpy(stPlatform.strMissionID, ui->comboBox_3->currentText().toLocal8Bit(), MAX_PLATFORM_LEN_1);
    strncpy(stPlatform.strMarkID01, ui->comboBox_2->currentText().toLocal8Bit(), MAX_PLATFORM_LEN_1);
    strncpy(stPlatform.strMarkID02, ui->comboBox_12->currentText().toLocal8Bit(), MAX_PLATFORM_LEN_1);
    strncpy(stPlatform.strMarkName01, ui->lineEdit_4->text().toLocal8Bit(), MAX_PLATFORM_LEN_1);
    strncpy(stPlatform.strMarkName02, ui->lineEdit_8->text().toLocal8Bit(), MAX_PLATFORM_LEN_1);
    stPlatform.EMarkHurm01 = stMission.GetHurtDegreeENum(ui->comboBox_11->currentIndex());
    stPlatform.EMarkHurm02 = stMission.GetHurtDegreeENum(ui->comboBox_13->currentIndex());
	stPlatform.EType = stPlatform.GetPlatformTypeENum(ui->comboBox_5->currentIndex());

    if (m_DataType == 1 && m_pITaskManageService != nullptr)
    {
        std::string strID = stPlatform.strID;
        bool bresut = m_pITaskManageService->IsMissionPlatformExisted(strID);
        if (bresut == true)
        {
            LyMessageBox::information(nullptr, tr("tip"), tr("ID duplicate, please modify"));
            return;
        }

        bresut = m_pITaskManageService->SetMissionPlatform(stPlatform);
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
        bool bresut = m_pITaskManageService->SetMissionPlatform(stPlatform);
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

void CDlgMissionPlatformEdit::on_pushButton_2_clicked()
{
    this->hide();
}

void CDlgMissionPlatformEdit::on_comboBox_2_currentIndexChanged(const QString &arg1)
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

void CDlgMissionPlatformEdit::on_comboBox_12_currentIndexChanged(const QString &arg1)
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

void CDlgMissionPlatformEdit::on_comboBox_10_currentIndexChanged(const QString &arg1)
{
    if (arg1 == "-")
    {
        ui->comboBox_8->clear();
    }
    else
    {
        ui->comboBox_8->clear();
        for (auto stForm : m_vFormList_ALL)
        {
            std::string strtmp = arg1.toStdString();
            if (0 == strcmp(stForm.m_strName.c_str(), strtmp.c_str()))
            {
                int nSize = stForm.GetVehicleNum();

                QStringList strNo;
                for (int i = 0; i < nSize; i++)
                {
                    strNo.push_back(QString::number(i + 1));
                }

                ui->comboBox_8->addItems(strNo);
                break;
            }
        }
    }
}

void CDlgMissionPlatformEdit::on_comboBox_3_currentIndexChanged(const QString &arg1)
{
    if (arg1 == "-")
    {
        ui->comboBox_2->clear();
        ui->lineEdit_4->setText("");
        ui->comboBox_11->setCurrentIndex(0);

        ui->comboBox_12->clear();
        ui->lineEdit_8->setText("");
        ui->comboBox_13->setCurrentIndex(0);

        ui->comboBox_7->clear();
        ui->comboBox_6->clear();
        ui->comboBox_10->setCurrentIndex(0);
        ui->comboBox_8->setCurrentIndex(0);
        ui->comboBox->clear();
    }
    else
    {
        if (m_pITaskManageService == nullptr)
        {
            return;
        }

        CNeubulaMission stMission = m_pITaskManageService->GetMission(arg1.toStdString());
        strncpy(m_stMissionPlatform.strMissionID, arg1.toLocal8Bit(), MAX_PLATFORM_LEN_1);

        UpdateMarkCombobox();

        UpdateFormCombobox();
    }
}
