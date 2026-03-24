#include "CDlgSensorManageEdit.h"
#include "ui_CDlgSensorManageEdit.h"
#include "QCString.h"
#include "ITaskManagerActivator.h"
#include "DataManage/NebulaTask/NebulaMissionAutoCommand.h"
#include "lY_TaskManage.h"

using namespace qnzkna::NebulaMissionManage::NebulaMissionAutoCommandManage;

CDlgSensorManageEdit::CDlgSensorManageEdit(QWidget *parent) :
    LyDialog(tr("Sensor Settings"), parent),
    ui(new Ui::CDlgSensorManageEdit)
{
    ui->setupUi(centralWidget());
    this->setResizable(true);
    this->resize(800, 600);

    m_pITaskManageService = ITaskManagerActivator::Get().getPTaskManageService();
    if (m_pITaskManageService == nullptr)
    {
        return;
    }

    ConnectControl();
}

CDlgSensorManageEdit::~CDlgSensorManageEdit()
{
    delete ui;
}

void CDlgSensorManageEdit::on_checkBox_Load_stateChanged(int arg1)
{
    QCheckBox *pCheckbox = (QCheckBox *)sender();
    int nCodeID = m_pQCheckBoxMap.find(pCheckbox).value();
    if (arg1 == 2)
    {
        qnzkna::NebulaMissionManage::NebulaMissionAutoCommandManage::CodeSetLoad_Parameter_CodeID ECoidID = (qnzkna::NebulaMissionManage::NebulaMissionAutoCommandManage::CodeSetLoad_Parameter_CodeID)nCodeID;
        QString str = qnzkna::NebulaMissionManage::NebulaMissionAutoCommandManage::CodeSetLoad_Parameter_CodeID_To_String(ECoidID);
        QListWidgetItem *item = new QListWidgetItem;
        item->setData(Qt::UserRole, QString::number(nCodeID));
        item->setText(str);
        ui->listWidget->addItem(item);
    }
    else if (arg1 == 0)
    {
        for (int i = 0; i < ui->listWidget->count(); i++)
        {
            QListWidgetItem *pItem = ui->listWidget->item(i);
            if (pItem->data(Qt::UserRole).toInt() == nCodeID)
            {
                QListWidgetItem *p = ui->listWidget->takeItem(i);
                delete p;
                break;
            }
        }
    }
}

void CDlgSensorManageEdit::InitLoadCheckBox()
{
    m_pQCheckBoxMap.clear();
    m_pQCheckBoxMap.insert(ui->checkBox_2, qnzkna::NebulaMissionManage::NebulaMissionAutoCommandManage::CodeSetLoad_Parameter_CodeID_ZC_CommonCommand_PayLoadStartWorking);
    m_pQCheckBoxMap.insert(ui->checkBox_3, qnzkna::NebulaMissionManage::NebulaMissionAutoCommandManage::CodeSetLoad_Parameter_CodeID_ZC_CommonCommand_PayLoadStopWorking);
    m_pQCheckBoxMap.insert(ui->checkBox_7, qnzkna::NebulaMissionManage::NebulaMissionAutoCommandManage::CodeSetLoad_Parameter_CodeID_ZC_CommonCommand_InfraredLightSet);
    m_pQCheckBoxMap.insert(ui->checkBox_8, qnzkna::NebulaMissionManage::NebulaMissionAutoCommandManage::CodeSetLoad_Parameter_CodeID_ZC_CommonCommand_StartStopRecodeVideo);
    m_pQCheckBoxMap.insert(ui->checkBox_9, qnzkna::NebulaMissionManage::NebulaMissionAutoCommandManage::CodeSetLoad_Parameter_CodeID_ZC_CommonCommand_StartStopContinueGrab);
    m_pQCheckBoxMap.insert(ui->checkBox_10, qnzkna::NebulaMissionManage::NebulaMissionAutoCommandManage::CodeSetLoad_Parameter_CodeID_ZC_CommonCommand_AutoFocus);
    m_pQCheckBoxMap.insert(ui->checkBox_11, qnzkna::NebulaMissionManage::NebulaMissionAutoCommandManage::CodeSetLoad_Parameter_CodeID_ZC_CommonCommand_SingleLaserTelemeter);
    m_pQCheckBoxMap.insert(ui->checkBox_12, qnzkna::NebulaMissionManage::NebulaMissionAutoCommandManage::CodeSetLoad_Parameter_CodeID_ZC_CommonCommand_LigthMultiple);
    m_pQCheckBoxMap.insert(ui->checkBox_13, qnzkna::NebulaMissionManage::NebulaMissionAutoCommandManage::CodeSetLoad_Parameter_CodeID_ZC_CommonCommand_GimbalCenter);
    m_pQCheckBoxMap.insert(ui->checkBox_14, qnzkna::NebulaMissionManage::NebulaMissionAutoCommandManage::CodeSetLoad_Parameter_CodeID_ZC_CommonCommand_GimbalLookDownVertical);
    m_pQCheckBoxMap.insert(ui->checkBox_15, qnzkna::NebulaMissionManage::NebulaMissionAutoCommandManage::CodeSetLoad_Parameter_CodeID_ZC_CommonCommand_GimbalScanParameterSet);
    m_pQCheckBoxMap.insert(ui->checkBox_16, qnzkna::NebulaMissionManage::NebulaMissionAutoCommandManage::CodeSetLoad_Parameter_CodeID_ZC_CommonCommand_GimbalRunScan);
    m_pQCheckBoxMap.insert(ui->checkBox_17, qnzkna::NebulaMissionManage::NebulaMissionAutoCommandManage::CodeSetLoad_Parameter_CodeID_ZC_CommonCommand_GimbalLock);
}

void CDlgSensorManageEdit::ConnectCheckBox()
{
    for (auto checkbox : m_pQCheckBoxMap.keys())
    {
        connect(checkbox, SIGNAL(stateChanged(int)), this, SLOT(on_checkBox_Load_stateChanged(int)));
    }
}

void CDlgSensorManageEdit::DisConnectCheckBox()
{
    for (auto checkbox : m_pQCheckBoxMap.keys())
    {
        checkbox->disconnect();
        checkbox->setChecked(false);
    }

    ui->lineEdit->setText("");
    ui->doubleSpinBox->setValue(100);
    ui->doubleSpinBox_2->setValue(5);
}

void CDlgSensorManageEdit::UpdateLinePointCombobox()
{
    ui->comboBox->disconnect();
    ui->comboBox->clear();

    if (m_LineID != 0)
    {
        CNeubulaMissionLine stLine = m_pITaskManageService->GetMissionLine(m_LineID);
        int nNo = 0;
        for (auto point : stLine.LinePointVec)
        {
            ui->comboBox->insertItem(nNo, QString::number(nNo + 1));
            ui->comboBox->setItemData(nNo, QString::number(nNo + 1));
            nNo++;
        }
        if (stLine.LinePointVec.size() == 0)
        {
            LyMessageBox::information(this, tr2("提示"), tr2("当前平台所匹配航线无航点信息！"));
            return;
        }
    }
    else
    {
        LyMessageBox::information(this, tr2("提示"), tr2("当前平台无航线信息！"));
        return;
    }

    int nIndex01 = ui->comboBox->findText(QString::number(m_PointID));
    ui->comboBox->setCurrentIndex(nIndex01);
    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(on_comboBox_currentIndexChanged(int)));
}

void CDlgSensorManageEdit::InitLoadCombobox()
{
    ui->comboBox_3->insertItem(0, tr("Infrared"));
    ui->comboBox_3->setItemData(0, 0);
    ui->comboBox_3->insertItem(1, tr("Visible Light"));
    ui->comboBox_3->setItemData(1, 1);
    ui->comboBox_3->insertItem(2, tr("Infrared+Visible Light"));
    ui->comboBox_3->setItemData(2, 2);

    ui->comboBox_4->insertItem(0, tr("Stop"));
    ui->comboBox_4->insertItem(1, tr("Zoom+"));
    ui->comboBox_4->insertItem(2, tr("Zoom-"));
    ui->comboBox_4->insertItem(3, tr("Focus+"));
    ui->comboBox_4->insertItem(4, tr("Focus-"));
    ui->comboBox_4->setItemData(0, 0);
    ui->comboBox_4->setItemData(1, 1);
    ui->comboBox_4->setItemData(2, 2);
    ui->comboBox_4->setItemData(3, 3);
    ui->comboBox_4->setItemData(4, 4);
}

void CDlgSensorManageEdit::ConnectControl()
{
    InitLoadCheckBox();
    ConnectCheckBox();

    InitLoadCombobox();

    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(on_comboBox_currentIndexChanged(int)));
    connect(ui->comboBox_3, SIGNAL(currentIndexChanged(int)), this, SLOT(on_comboBox_3_currentIndexChanged(int)));
    connect(ui->comboBox_4, SIGNAL(currentIndexChanged(int)), this, SLOT(on_comboBox_4_currentIndexChanged(int)));

    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(on_pushButton_clicked()));
    connect(ui->pushButton_2, SIGNAL(clicked()), this, SLOT(on_pushButton_2_clicked()));

}

void CDlgSensorManageEdit::SetCodeSetData(const CCodeSet_Load_DB_Vec CodeSetVec)
{
    m_CodeSet_Load_DB_Vec = CodeSetVec;
}

void CDlgSensorManageEdit::SetTaskID(const std::string strID)
{
    m_strTaskID = strID;
}

void CDlgSensorManageEdit::SetPlatformID(const std::string strID)
{
    m_strPlatformID = strID;
}

void CDlgSensorManageEdit::SetLoadID(const std::string strID)
{
    m_strLoadID = strID;
}

void CDlgSensorManageEdit::SetLineID(const int nLineID)
{
    m_LineID = nLineID;
}

void CDlgSensorManageEdit::SetPointID(const int nID)
{
    m_PointID = nID;

    m_CodeSet_Load_DB_Wnd_Vec.clear();
    for (auto CodeSet : m_CodeSet_Load_DB_Vec)
    {
        if (0 == strcmp(CodeSet.GetPlatformID().c_str(), m_strPlatformID.c_str()) && CodeSet.GetPointID() == m_PointID)
        {
            m_CodeSet_Load_DB_Wnd_Vec.push_back(CodeSet);
        }
    }
}

void CDlgSensorManageEdit::SetMarkIDList(const QStringList strList)
{
    m_MarkList = strList;
}

void CDlgSensorManageEdit::UpdateListData()
{

    ui->listWidget->clear();
    if (m_pITaskManageService == nullptr || m_CodeSet_Load_DB_Vec.size() == 0)
    {
        return;
    }

    for (auto CodeSet : m_CodeSet_Load_DB_Wnd_Vec)
    {
        QString strCodeName;
        qnzkna::NebulaMissionManage::NebulaMissionAutoCommandManage::CodeSetLoad_Parameter_CodeID ePara = (qnzkna::NebulaMissionManage::NebulaMissionAutoCommandManage::CodeSetLoad_Parameter_CodeID)CodeSet.GetCodeID();
        strCodeName = qnzkna::NebulaMissionManage::NebulaMissionAutoCommandManage::CodeSetLoad_Parameter_CodeID_To_String(ePara);

        QListWidgetItem *item = new QListWidgetItem;
        item->setData(Qt::UserRole, QString::number(CodeSet.GetCodeID()));
        item->setText(strCodeName);
        ui->listWidget->addItem(item);
    }
}

void CDlgSensorManageEdit::UpdateCheckBoxData()
{
    DisConnectCheckBox();

    for (auto checkbox : m_pQCheckBoxMap.keys())
    {
        for (auto codeset : m_CodeSet_Load_DB_Wnd_Vec)
        {
            if (m_pQCheckBoxMap[checkbox] == codeset.GetCodeID())
            {
                checkbox->setChecked(true);
                CodeSetLoad_CommonCommand st1 = *((CodeSetLoad_CommonCommand *)(codeset.GetParameter().c_str()));
                switch (m_pQCheckBoxMap[checkbox])
                {
                case CodeSetLoad_Parameter_CodeID::CodeSetLoad_Parameter_CodeID_ZC_CommonCommand_InfraredLightSet:
                    ui->comboBox_3->setCurrentIndex(st1.para.ZC_CommonCommand_InfraredLightSet.infraredLightSet);
                    break;
                case CodeSetLoad_Parameter_CodeID::CodeSetLoad_Parameter_CodeID_ZC_CommonCommand_StartStopContinueGrab:
                    ui->lineEdit->setText(QString::number(st1.para.ZC_CommonCommand_StartStopContinueGrab.interval));
                    break;
                case CodeSetLoad_Parameter_CodeID::CodeSetLoad_Parameter_CodeID_ZC_CommonCommand_LigthMultiple:
                    ui->comboBox_4->setCurrentIndex(st1.para.ZC_CommonCommand_LigthMultiple.ligthMultiple);
                    break;
                case CodeSetLoad_Parameter_CodeID::CodeSetLoad_Parameter_CodeID_ZC_CommonCommand_GimbalScanParameterSet:
                    ui->doubleSpinBox->setValue(st1.para.ZC_CommonCommand_GimbalScanParameterSet.scannAngle);
                    ui->doubleSpinBox_2->setValue(st1.para.ZC_CommonCommand_GimbalScanParameterSet.scannSpeed);
                    break;
                default:
                    break;
                }
            }
        }
    }

    ConnectCheckBox();
}

void CDlgSensorManageEdit::CalculateMark()
{
    CNebulaMissionRelation_PlatRouteVec MissionRelation_PlatRouteVec = m_pITaskManageService->GetMissionIDByPlatRoute(m_strTaskID);
    QString strMark;
    for (auto data : MissionRelation_PlatRouteVec)
    {
        if (data.GetPlatformID() == m_strPlatformID)
        {
            strMark = QString::fromStdString(data.GetMarkID());
            m_LineID = data.GetRouteID();
        }
        else
        {
            strMark = "";
            m_LineID = 0;
        }
    }
    ui->lineEdit_2->setText(strMark);

}

void CDlgSensorManageEdit::SavePodData()
{

}

void CDlgSensorManageEdit::on_comboBox_currentIndexChanged(int index)
{
    SetPointID(index + 1);
    UpdateListData();
    UpdateCheckBoxData();
}

void CDlgSensorManageEdit::on_comboBox_3_currentIndexChanged(int index)
{

}

void CDlgSensorManageEdit::on_comboBox_4_currentIndexChanged(int index)
{

}

void CDlgSensorManageEdit::showEvent(QShowEvent *event)
{
    CalculateMark();
    UpdateLinePointCombobox();
    UpdateListData();
    UpdateCheckBoxData();
}

void CDlgSensorManageEdit::closeEvent(QCloseEvent *event)
{

}

void CDlgSensorManageEdit::on_pushButton_clicked()
{
    QVariant  qv = ui->comboBox->itemData(ui->comboBox->currentIndex());
    int nPointNo = qv.toInt();
    int nPointCount = ui->listWidget->count();
    m_pITaskManageService->RmoveCodeSet_Load(m_strTaskID, m_strPlatformID, nPointNo);

    for (int i = 0; i < nPointCount; i++)
    {
        QListWidgetItem *pItem = ui->listWidget->item(i);
        int nCodeID = pItem->data(Qt::UserRole).toInt();

        CCodeSet_Load_DB stCodeSet;
        int nID = m_pITaskManageService->GetCodeSet_Load_MaxID() + 1;
        stCodeSet.SetID(nID);
        stCodeSet.SetActionID(m_pITaskManageService->GetActivatingActionID());
        stCodeSet.SetTaskID(m_strTaskID);
        stCodeSet.SetLineID(QString::number(m_LineID).toStdString());
        stCodeSet.SetPlatformID(m_strPlatformID);
        stCodeSet.SetLoadID("1");
        stCodeSet.SetPointID(nPointNo);
        CNeubulaMissionLinePoint stPoint = m_pITaskManageService->GetMissionLinePoint(m_LineID, nPointNo);
        stCodeSet.SetLon(stPoint.dLon);
        stCodeSet.SetLat(stPoint.dLat);
        stCodeSet.SetAlt(stPoint.dAlt);
        stCodeSet.SetSuite(i + 1);
        stCodeSet.SetCodeID(nCodeID);
        CodeSetLoad_CommonCommand st1;
        switch (nCodeID)
        {
        case CodeSetLoad_Parameter_CodeID::CodeSetLoad_Parameter_CodeID_ZC_CommonCommand_InfraredLightSet:
            st1.nCodeID = CodeSetLoad_Parameter_CodeID::CodeSetLoad_Parameter_CodeID_ZC_CommonCommand_InfraredLightSet;
            qv = ui->comboBox_3->itemData(ui->comboBox_3->currentIndex());
            st1.para.ZC_CommonCommand_InfraredLightSet.infraredLightSet = qv.toInt();
            stCodeSet.SetParameter(std::string((char *)&st1, sizeof(st1)));
            break;
        case CodeSetLoad_Parameter_CodeID::CodeSetLoad_Parameter_CodeID_ZC_CommonCommand_StartStopContinueGrab:
            st1.nCodeID = CodeSetLoad_Parameter_CodeID::CodeSetLoad_Parameter_CodeID_ZC_CommonCommand_StartStopContinueGrab;
            st1.para.ZC_CommonCommand_StartStopContinueGrab.interval = ui->lineEdit->text().toInt();
            stCodeSet.SetParameter(std::string((char *)&st1, sizeof(st1)));
            break;
        case CodeSetLoad_Parameter_CodeID::CodeSetLoad_Parameter_CodeID_ZC_CommonCommand_LigthMultiple:
            st1.nCodeID = CodeSetLoad_Parameter_CodeID::CodeSetLoad_Parameter_CodeID_ZC_CommonCommand_LigthMultiple;
            qv = ui->comboBox_4->itemData(ui->comboBox_4->currentIndex());
            st1.para.ZC_CommonCommand_LigthMultiple.ligthMultiple = qv.toInt();
            stCodeSet.SetParameter(std::string((char *)&st1, sizeof(st1)));
            break;
        case CodeSetLoad_Parameter_CodeID::CodeSetLoad_Parameter_CodeID_ZC_CommonCommand_GimbalScanParameterSet:
            st1.nCodeID = CodeSetLoad_Parameter_CodeID::CodeSetLoad_Parameter_CodeID_ZC_CommonCommand_GimbalScanParameterSet;
            st1.para.ZC_CommonCommand_GimbalScanParameterSet.scannAngle = ui->doubleSpinBox->value();
            st1.para.ZC_CommonCommand_GimbalScanParameterSet.scannSpeed = ui->doubleSpinBox_2->value();
            stCodeSet.SetParameter(std::string((char *)&st1, sizeof(st1)));
            break;
        default:
            break;
        }
        m_pITaskManageService->SetCodeSet_Load1(stCodeSet);

    }
    LY_TaskManage::Get().UpdateCodeSetSensor();
    LY_TaskManage::Get().m_pDlgSensorManage->UpdataListData(QString::fromStdString(m_strTaskID), QString::fromStdString(m_strPlatformID));
    this->hide();
}

void CDlgSensorManageEdit::on_pushButton_2_clicked()
{
    this->hide();
}
