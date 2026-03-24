#include "CDlgCodeSet_Load_Edit_Child.h"
#include "ui_CDlgCodeSet_Load_Edit_Child.h"
#include "QCString.h"
#include "ITaskManagerActivator.h"

CDlgCodeSet_Load_Edit_Child::CDlgCodeSet_Load_Edit_Child(QWidget *parent) :
    LyDialog(tr("Planning Instruction Set Editing"), parent),
    ui(new Ui::CDlgCodeSet_Load_Edit_Child)
{
    ui->setupUi(centralWidget());
    this->setResizable(true);
    this->resize(400, 300);

    m_pITaskManageService = ITaskManagerActivator::Get().getPTaskManageService();

    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(on_pushButton_clicked()));
    connect(ui->pushButton_2, SIGNAL(clicked()), this, SLOT(on_pushButton_2_clicked()));

    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(on_comboBox_currentIndexChanged(int)));
}

CDlgCodeSet_Load_Edit_Child::~CDlgCodeSet_Load_Edit_Child()
{
    delete ui;
}

void CDlgCodeSet_Load_Edit_Child::SetEditData(const CCodeSet_Load_DB stData)
{
    m_CodeSet_Load_DB = stData;
}

CCodeSet_Load_DB CDlgCodeSet_Load_Edit_Child::GetEditData()
{
    return m_CodeSet_Load_DB;
}

void CDlgCodeSet_Load_Edit_Child::InitEditCtrl()
{
    ui->lineEdit->setText("");
    ui->lineEdit_3->setText("");
    ui->lineEdit_4->setText("");
}

void CDlgCodeSet_Load_Edit_Child::UpdateEditCtrl()
{
    if (m_CodeSet_Load_DB.GetID() == 0)
    {
        return;
    }

    ui->lineEdit->setText(QString::number(m_CodeSet_Load_DB.GetSuite()));
    ui->lineEdit_3->setText(QString::fromStdString(m_CodeSet_Load_DB.GetParameter()));
    ui->lineEdit_4->setText(QString::fromStdString(m_CodeSet_Load_DB.GetRemark()));
}

void CDlgCodeSet_Load_Edit_Child::UpdateLoadCombobox()
{
    if (m_pITaskManageService == nullptr)
    {
        return;
    }

    ui->comboBox->disconnect();

    ui->comboBox->clear();

    CNebulaMissionPlatformLoadVec MissionLoadVec;
    MissionLoadVec = m_pITaskManageService->GetAllMissionPlatformLoad();

    int nNo = 0 ;
    for (auto data : MissionLoadVec)
    {
        ui->comboBox->insertItem(nNo, tr2(data.strMode));
        ui->comboBox->setItemData(nNo, tr2(data.strID));
        nNo++;
    }

    if (m_CodeSet_Load_DB.GetID() == 0)
    {
        ui->comboBox->setCurrentIndex(0);
    }
    else
    {
        int nSel = ui->comboBox->findData(QString::fromStdString(m_CodeSet_Load_DB.GetLoadID()));
        if (nSel != -1)
        {
            ui->comboBox->setCurrentIndex(nSel);
        }
    }

    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(on_comboBox_currentIndexChanged(int)));
}

void CDlgCodeSet_Load_Edit_Child::InitCodeCombobox()
{
    ui->comboBox_2->clear();

    if (m_pITaskManageService == nullptr)
    {
        return;
    }

    QVariant  qv = ui->comboBox->itemData(ui->comboBox->currentIndex());
    QString strLoadID = qv.toString();

    CNebulaMissionPlatformLoad MissionLoad;
    MissionLoad = m_pITaskManageService->GetMissionPlatformLoad(strLoadID.toStdString());

    if (MissionLoad.eType == EMissionPlatformLoadType_DC)
    {
        int nNo = 0;
        for (auto cmd : qnzkna::NebulaMissionManage::NebulaMissionAutoCommandManage::CodeSetLoad_Parameter_CodeID_ZC_CommonCommandList)
        {
            QString str = qnzkna::NebulaMissionManage::NebulaMissionAutoCommandManage::CodeSetLoad_Parameter_CodeID_To_String(cmd);
            ui->comboBox_2->insertItem(nNo, str);
            ui->comboBox_2->setItemData(nNo, cmd);
            nNo++;
        }
    }
    else if (MissionLoad.eType == EMissionPlatformLoadType_DY)
    {
        int nNo = 0;
        for (auto cmd : qnzkna::NebulaMissionManage::NebulaMissionAutoCommandManage::CodeSetLoad_Parameter_CodeID_DY_CommonCommandList)
        {
            QString str = qnzkna::NebulaMissionManage::NebulaMissionAutoCommandManage::CodeSetLoad_Parameter_CodeID_To_String(cmd);
            ui->comboBox_2->insertItem(nNo, str);
            ui->comboBox_2->setItemData(nNo, cmd);
            nNo++;
        }
    }
}

void CDlgCodeSet_Load_Edit_Child::UpdateCodeCombobox()
{
    if (m_pITaskManageService == nullptr)
    {
        return;
    }
    if (m_CodeSet_Load_DB.GetID() == 0)
    {
        ui->comboBox_2->setCurrentIndex(0);

    }
    else
    {
        int nCodeID = m_CodeSet_Load_DB.GetCodeID();
        qnzkna::NebulaMissionManage::NebulaMissionAutoCommandManage::CodeSetLoad_Parameter_CodeID ePara = (qnzkna::NebulaMissionManage::NebulaMissionAutoCommandManage::CodeSetLoad_Parameter_CodeID)nCodeID;
        int nSel = ui->comboBox_2->findData(ePara);
        if (nSel != -1)
        {
            ui->comboBox_2->setCurrentIndex(nSel);
        }
    }
}

void CDlgCodeSet_Load_Edit_Child::showEvent(QShowEvent *event)
{
    InitEditCtrl();
    UpdateEditCtrl();
    UpdateLoadCombobox();
    InitCodeCombobox();
    UpdateCodeCombobox();
}

void CDlgCodeSet_Load_Edit_Child::on_pushButton_clicked()
{
    QVariant  qv = ui->comboBox->itemData(ui->comboBox->currentIndex());
    m_CodeSet_Load_DB.SetLoadID((qv.toString()).toStdString());
    m_CodeSet_Load_DB.SetSuite((ui->lineEdit->text()).toInt());
    qv = ui->comboBox_2->itemData(ui->comboBox_2->currentIndex());
    m_CodeSet_Load_DB.SetCodeID(qv.toString().toInt());
    m_CodeSet_Load_DB.SetParameter(ui->lineEdit_3->text().toStdString());
    m_CodeSet_Load_DB.SetRemark(ui->lineEdit_4->text().toStdString());

    emit PushOkButton();
    this->hide();
}

void CDlgCodeSet_Load_Edit_Child::on_pushButton_2_clicked()
{
    this->hide();
}

void CDlgCodeSet_Load_Edit_Child::on_comboBox_currentIndexChanged(int index)
{
    InitCodeCombobox();
}
