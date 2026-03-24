#include "CDlgEditGroupp.h"
#include "ui_CDlgEditGroup.h"
#include "QCString.h"
#include "ITaskManagerActivator.h"
#include "LyMessageBox.h"
#include <QPainter>

CDlgEditGroup::CDlgEditGroup(QWidget *parent) :
    LyDialog(tr("EditGroup"), parent),
    ui(new Ui::CDlgEditGroup)
{
    ui->setupUi(centralWidget());
    this->setResizable(true);
    this->resize(1200, 800);

    ui->label_6->setText("");

    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(on_pushButton_clicked()));
    connect(ui->pushButton_2, SIGNAL(clicked()), this, SLOT(on_pushButton_2_clicked()));
    connect(ui->pushButton_3, SIGNAL(clicked()), this, SLOT(on_pushButton_3_clicked()));
    connect(ui->pushButton_4, SIGNAL(clicked()), this, SLOT(on_pushButton_4_clicked()));
    connect(ui->pushButton_5, SIGNAL(clicked()), this, SLOT(on_pushButton_5_clicked()));

    connect(ui->comboBox_5, SIGNAL(currentIndexChanged(const QString)), this, SLOT(on_comboBox_5_currentIndexChanged(const QString)));
    connect(ui->comboBox_8, SIGNAL(currentIndexChanged(const QString)), this, SLOT(on_comboBox_8_currentIndexChanged(const QString)));
    connect(ui->comboBox_9, SIGNAL(currentIndexChanged(const QString)), this, SLOT(on_comboBox_9_currentIndexChanged(const QString)));
    connect(ui->comboBox_10, SIGNAL(currentIndexChanged(const QString)), this, SLOT(on_comboBox_10_currentIndexChanged(const QString)));

    connect(ui->radioButton, SIGNAL(clicked()), this, SLOT(on_radioButton_clicked()));
    connect(ui->radioButton_2, SIGNAL(clicked()), this, SLOT(on_radioButton_2_clicked()));

    m_pITaskManageService = ITaskManagerActivator::Get().getPTaskManageService();
    if (m_pITaskManageService == nullptr)
    {
        return;
    }

    m_pIWeaponManageService = ITaskManagerActivator::Get().getPWeaponManageService();
    if (m_pIWeaponManageService == nullptr)
    {
        return;
    }

    m_pIFormationManageService = ITaskManagerActivator::Get().getPFormationManageService();
    if (m_pIFormationManageService == nullptr)
    {
        return;
    }

    m_pIConfigProjectControlService = ITaskManagerActivator::Get().getPIConfigProjectControlService();

    m_pICommandUnitManageService = ITaskManagerActivator::Get().getPICommandUnitManageService();

    InitData();
    readFormsFromFile();
    InitUAVNodeInfo();

}

CDlgEditGroup::~CDlgEditGroup()
{
    delete ui;
}

void CDlgEditGroup::mousePressEvent(QMouseEvent *event)
{

}

void CDlgEditGroup::mouseReleaseEvent(QMouseEvent *event)
{

}

void CDlgEditGroup::mouseMoveEvent(QMouseEvent *event)
{

}

void CDlgEditGroup::on_pushButton_clicked()
{
    SetWindowStates(BTN_ADD);
    SetBtnStates(false);
}

void CDlgEditGroup::on_pushButton_2_clicked()
{
    SetWindowStates(BTN_EDIT);
    SetBtnStates(false);
    ui->comboBox->setEnabled(false);

    ui->comboBox_3->setEnabled(false);

}

void CDlgEditGroup::on_pushButton_3_clicked()
{
    SetWindowStates(BTN_NORMAL);
}

void CDlgEditGroup::on_pushButton_4_clicked()
{
    if (m_EBtnStates == BTN_EDIT)
    {

        QString	str_groupID, str_groupName, str_taskID, str_taskName;
        str_taskID = ui->comboBox->currentText();
        str_taskName = ui->comboBox_2->currentText();
        str_groupID = ui->comboBox_3->currentText();
        str_groupName = ui->comboBox_4->currentText();
        unsigned int n_groupID = str_groupID.toInt();
        unsigned int n_TaskID = str_taskID.toInt();

        CNebulaRouteGroup stGroupInfo, stGroupInfoTmp;
        m_pITaskManageService->GetNebulaRouteGroup(m_TaskId, n_groupID, &stGroupInfo);
        if (m_UAVnumber == 0)
        {

            LyMessageBox::information(NULL, tr("Tips"), tr("Please select UAV"));
            return;
        }

        CNebulaRouteGroup::EFormMode eMode;
        if (ui->radioButton->isChecked())
        {
            eMode = CNebulaRouteGroup::EFormMode::EFormMode_Formation;
        }
        else
        {
            eMode = CNebulaRouteGroup::EFormMode::EFormMode_Free;
        }

        stGroupInfo.SetFormMode(eMode);

        if (ui->radioButton->isChecked())
        {
            QString str_FormName;
            int n_formid = -1;
            str_FormName = ui->comboBox_5->currentText();

            if (str_FormName == "")
            {

                LyMessageBox::information(NULL, tr("Tips"), tr("Please select form"));
                return;
            }

            std::string str_name = str_FormName.toStdString();
            long nLength = str_name.length();
            str_name = str_name.substr(0, nLength - 3);
            str_FormName = str_name.c_str();

            SFormList::iterator iter_list = m_vFormList.begin();
            for (; iter_list != m_vFormList.end(); iter_list++)
            {
                QString strtmp = QString::fromStdString(iter_list->m_strName);
                if (0 == QString::compare(strtmp, str_FormName))
                {
                    n_formid = iter_list->m_nID;
                    break;
                }
            }
            if (n_formid != -1)
            {
                stGroupInfo.SetFormationID(n_formid);
            }

            stGroupInfoTmp = stGroupInfo;
            stGroupInfoTmp.EmptyRouteLineList();
            m_VehicleShowInfoVec = ui->label_6->GetVehicleShowInfo();
            for (auto vecinfo : m_VehicleShowInfoVec)
            {
                if (vecinfo.Flag_In_Group == 1)
                {
                    if (vecinfo.SelectVehicleID == -1)
                    {

                        LyMessageBox::information(NULL, tr("Tips"), tr("Please select UAV seat."));
                        return;
                    }
                    else
                    {
                        qnzkna::NodeManage::CNodeRelationInfoPtrVector::iterator iter_info_list = m_vecNodeInfo.begin();
                        for (; iter_info_list != m_vecNodeInfo.end(); iter_info_list++)
                        {
                            std::string s_NodeName;
                            s_NodeName = (*iter_info_list)->GetNodeName();
                            if (0 == QString::compare(s_NodeName.c_str(), vecinfo.name))
                            {
                                break;
                            }
                        }
                        if (iter_info_list != m_vecNodeInfo.end())
                        {
                            std::string s_NodeID = (*iter_info_list)->GetNodeID();
                            CNebulaRouteLine stNebulaRouteLine;
                            bool bresult = stGroupInfo.GetRouteLine(s_NodeID.c_str(), &stNebulaRouteLine);
                            if (bresult == false)
                            {
                                stNebulaRouteLine.SetWeaponID(vecinfo.VehicleID.toLocal8Bit());
                            }
                            stNebulaRouteLine.SetFormOrdinalNum(vecinfo.SelectVehicleID + 1);
                            stGroupInfoTmp.SetRouteLine(stNebulaRouteLine);
                        }
                    }
                }
            }
        }
        else if (ui->radioButton_2->isChecked())
        {
            stGroupInfo.SetFormationID(-1);

        }
        else
        {

            LyMessageBox::information(NULL, tr("Tips"), tr("Plese select form mode"));
            return;
        }

        m_pITaskManageService->SetNebulaRouteGroup(n_TaskID, stGroupInfoTmp);

        LyMessageBox::information(NULL, tr("Tips"), tr("Edit Success!"));
    }

    SetWindowStates(BTN_NORMAL);
    ui->comboBox->setEnabled(true);

    ui->comboBox_3->setEnabled(true);

    SetBtnStates(true);

    UpdateWndData();
}

void CDlgEditGroup::on_pushButton_5_clicked()
{
    SetWindowStates(BTN_NORMAL);
    ui->comboBox->setEnabled(true);

    ui->comboBox_3->setEnabled(true);

    SetBtnStates(true);

}

void CDlgEditGroup::on_comboBox_3_currentIndexChanged(const QString &arg1)
{
    int nID = arg1.toInt();
    if (nID <= 0)
    {
        return;
    }
    UpdateSelectedGroup(nID);
}

void CDlgEditGroup::on_comboBox_4_currentIndexChanged(const QString &arg1)
{
    unsigned int nID;
    bool bresult = m_pITaskManageService->GetNebulaRouteGroupID(m_TaskId, arg1.toLocal8Bit(), nID);

    if (bresult == false)
    {
        return;
    }

    UpdateSelectedGroup(nID);
}

void CDlgEditGroup::on_comboBox_currentIndexChanged(const QString &arg1)
{
    int nID = arg1.toInt();
    if (nID > 0)
    {
        m_TaskId = nID;
        UpdateSelectedTask(nID);
    }
}

void CDlgEditGroup::on_comboBox_2_currentIndexChanged(const QString &arg1)
{
    unsigned int nID;
    m_pITaskManageService->GetNebulaTaskID(arg1.toLocal8Bit(), nID);
    if (nID > 0)
    {
        m_TaskId = nID;
        UpdateSelectedTask(nID);
    }
}

void CDlgEditGroup::on_treeWidget_itemChanged(QTreeWidgetItem *item, int column)
{
    QString itemText = item->text(0);
    if (itemText == "")
    {
        return;
    }

    qnzkna::NodeManage::CNodeRelationInfoPtrVector::iterator iter_info_list = m_vecNodeInfo.begin();
    for (; iter_info_list != m_vecNodeInfo.end(); iter_info_list++)
    {
        std::string s_NodeName;
        s_NodeName = (*iter_info_list)->GetNodeName();
        if (0 == strcmp(itemText.toLocal8Bit(), s_NodeName.c_str()))
        {
            break;
        }
    }
    if (iter_info_list == m_vecNodeInfo.end())
    {
        return;
    }

    std::string s_NodeID;
    s_NodeID = (*iter_info_list)->GetNodeID();

    std::vector<CNebulaTask *>::iterator iter_list = m_vecWndTaskInfo.begin();
    for (; iter_list != m_vecWndTaskInfo.end(); iter_list++)
    {
        if ((*iter_list)->GetTaskID() == m_TaskId)
        {
            break;
        }
    }

    if (iter_list == m_vecWndTaskInfo.end())
    {
        return;
    }

    if (Qt::Checked == item->checkState(0))
    {
        CNebulaRouteLine stNebulaRouteLine;
        stNebulaRouteLine.SetWeaponID(s_NodeID.c_str());
        (*iter_list)->AddNebulaRouteLine(m_GroupID, stNebulaRouteLine);
        m_UAVnumber++;
    }
    else  if (Qt::Unchecked == item->checkState(0))
    {
        (*iter_list)->RemoveNebulaRouteLine(m_GroupID, s_NodeID.c_str());
        m_UAVnumber--;
    }
    UpdateFormsFromFile();
    updateFormsToFormCombo(0);
    int nCurIndex = ui->comboBox_5->currentIndex();
    UpdateVehicleInfoShow();
    SForm *pForm = getFormByIdx(nCurIndex);
    m_nSelVehicleIcon = -1;
    if (pForm)
    {
        m_sForm = *pForm;
    }
    UpdateChildWndDate();
    ui->label_6->update();
}

void CDlgEditGroup::on_comboBox_5_currentIndexChanged(const QString &arg1)
{
    int nCurIndex = ui->comboBox_5->currentIndex();
    SForm *pForm = getFormByIdx(nCurIndex);
    if (!pForm)
    {
        return;
    }

    m_nSelVehicleIcon = -1;
    m_sForm = *pForm;
    UpdateChildWndDate();
    ui->label_6->update();

    QString strname = ui->comboBox_5->currentText();
    emit SetFormLabelTest(strname);
}

void CDlgEditGroup::on_comboBox_8_currentIndexChanged(const QString &arg1)
{

}

void CDlgEditGroup::on_comboBox_9_currentIndexChanged(const QString &arg1)
{
    int nCurIndex = ui->comboBox_9->currentIndex();
    m_fScale = (float)pow(2, nCurIndex) / 2;
    ui->label_6->SetScale(m_fScale);
    ui->label_6->update();

}

void CDlgEditGroup::on_comboBox_10_currentIndexChanged(const QString &arg1)
{

}

void CDlgEditGroup::on_radioButton_clicked()
{
    m_line_kind = 0;
    ui->label_6->SetLineKind(m_line_kind);
    ui->label_6->update();
}

void CDlgEditGroup::on_radioButton_2_clicked()
{
    m_line_kind = 1;
    ui->label_6->SetLineKind(m_line_kind);
    ui->label_6->update();
}

void CDlgEditGroup::on_radioButton_7_clicked()
{

}

void CDlgEditGroup::on_radioButton_8_clicked()
{

}

void CDlgEditGroup::InitData()
{
    ClearWndTaskList();
    ClearS_xygsJL_08();
    ClearS_xygsJL_09();

    SetBtnStates(true);

    m_btn_Course_OK_Flag = 0;

    m_pDlgEditLine = nullptr;

    m_fScale = 0.5f;
    m_line_kind = 0;

    ui->comboBox_8->insertItem(0, tr("Up"));
    ui->comboBox_8->insertItem(1, tr("Back"));

    ui->comboBox_9->insertItem(0, tr2("X1"));
    ui->comboBox_9->insertItem(1, tr2("X2"));
    ui->comboBox_9->insertItem(2, tr2("X4"));
    ui->comboBox_9->insertItem(3, tr2("X8"));

    ui->comboBox_10->insertItem(0, "5");
    ui->comboBox_10->insertItem(1, "10");
    ui->comboBox_10->insertItem(2, "20");
    ui->comboBox_10->insertItem(3, "50");
    ui->comboBox_10->insertItem(4, "100");
    ui->comboBox_10->insertItem(5, "200");
    ui->comboBox_10->setCurrentIndex(4);

    m_UAVnumber = 0;
    Last_UAVnumber = 0;
    m_eViewMode = VIEW_TOP;
    m_VehicleIcon[VIEW_TOP].load(":/images/UAV2_Cyan.png");
    m_VehicleIcon[VIEW_BACK].load(":/images/UAV2_Cyan_back_view.png");
    m_VehicleIcon[VIEW_TOP_SELECT].load(":/images/UAV2.png");
    memset(&s_VehicleShowInfo, 0x00, sizeof(VehicleShowInfo));
    SelectVehicleID = -1;

    ui->treeWidget->setHeaderLabel(tr2("UAVÁÐ±í"));
    ui->treeWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->treeWidget->setSelectionBehavior(QAbstractItemView::SelectItems);

    ui->radioButton->setEnabled(false);
    ui->radioButton_2->setEnabled(false);
    ui->radioButton->setChecked(true);
    ui->radioButton_2->setChecked(false);

    ui->radioButton_7->setChecked(true);
    ui->radioButton_8->setChecked(false);
}

void CDlgEditGroup::DrawWindows()
{
    QPainter p(this);

    QColor qcBack(11, 29, 47);

    QPen pen1(qcBack, 1, Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin);
    p.setPen(pen1);
    QBrush brushStar(qcBack, Qt::SolidPattern);
    p.setBrush(brushStar);
    p.drawRect(m_rectView);
}

SForm *CDlgEditGroup::getFormByIdx(int nIdx)
{
    if (nIdx > -1 && nIdx < m_vFormList.size())
    {
        return &m_vFormList[nIdx];
    }
    return NULL;
}

bool CDlgEditGroup::readFormsFromFile()
{
    if (!m_pIFormationManageService)
    {
        return false;
    }

    return m_pIFormationManageService->GetFormationList(m_vFormList_ALL);
}

bool CDlgEditGroup::UpdateFormsFromFile()
{
    m_vFormList.clear();
    if (m_UAVnumber > 1)
    {
        SFormList::iterator iter_list = m_vFormList_ALL.begin();
        for (; iter_list != m_vFormList_ALL.end(); iter_list++)
        {
            if (m_UAVnumber == iter_list->GetVehicleNum())
            {
                m_vFormList.push_back(*iter_list);
            }
        }
    }
    return true;
}

void CDlgEditGroup::InitUAVListInfo()
{

    ui->treeWidget->clear();
    ui->treeWidget->disconnect();

    std::vector<CNebulaTask *>::iterator iter_list = m_vecWndTaskInfo.begin();
    for (; iter_list != m_vecWndTaskInfo.end(); iter_list++)
    {
        if ((*iter_list)->GetTaskID() == m_TaskId)
        {
            break;
        }
    }
    if (iter_list == m_vecWndTaskInfo.end())
    {
        return;
    }

    NebulaTaskPlatformMap wndNebulaTaskPlatformMap = (*iter_list)->GetPlatformInfoList();

#if 0
    std::string strWeaponSystemId = "11000";
    qnzkna::WeaponManage::WeaponSystem weaponSystem;
    bool bGetWeaponSys = m_pIWeaponManageService->GetWeaponSystem(&weaponSystem, strWeaponSystemId);
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
            NebulaTaskPlatformMapItr itrmap = wndNebulaTaskPlatformMap.begin();
            for (; itrmap != wndNebulaTaskPlatformMap.end(); itrmap++)
            {
                if (0 == strcmp(strUAVId.c_str(), itrmap->second.GetPlatformID().c_str()))
                {
                    break;
                }
            }
            if (itrmap != wndNebulaTaskPlatformMap.end())
            {
                qnzkna::NodeManage::CNodeRelationInfo *pNodeInfo = new qnzkna::NodeManage::CNodeRelationInfo(NODE_TYPE_WEAPONCOMPONENT, strUAVId, strComponentName, strProtocolType);
                if (pNodeInfo)
                {
                    QTreeWidgetItem *group1 = new QTreeWidgetItem(ui->treeWidget);
                    group1->setText(0, QString::fromStdString(strComponentName));
                    group1->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
                    group1->setCheckState(0, Qt::Unchecked);

                    m_vecNodeInfo.push_back(pNodeInfo);
                    nCurIndex++;
                }
            }
        }
    }
#else
    for (auto stNode : m_vecNodeInfo)
    {
        std::string strUAVId = stNode->GetNodeID();
        std::string strComponentName = stNode->GetNodeName();
        std::string strProtocolType = stNode->GetNodeProtocolType();
        QTreeWidgetItem *group1 = new QTreeWidgetItem(ui->treeWidget);
        group1->setText(0, QString::fromStdString(strComponentName));
        group1->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        group1->setCheckState(0, Qt::Unchecked);
    }
#endif
}

void CDlgEditGroup::UpdateVehicleInfoShow()
{
    m_VehicleShowInfoVec.clear();

    int nDesSize = getIconDescSize();
    std::vector<CNebulaTask *>::iterator iter_list = m_vecWndTaskInfo.begin();
    for (; iter_list != m_vecWndTaskInfo.end(); iter_list++)
    {
        if ((*iter_list)->GetTaskID() == m_TaskId)
        {
            break;
        }
    }
    if (iter_list == m_vecWndTaskInfo.end())
    {
        return;
    }
    CNebulaRouteGroup wndNebulaRouteGroup;
    bool bresult = (*iter_list)->GetRouteGroup(m_GroupID, &wndNebulaRouteGroup);
    if (bresult == false)
    {
        return;
    }
    StringVector wndStringVector;
    wndNebulaRouteGroup.GetWeaponIDList(&wndStringVector);
    StringVectorItr itrIDlist = wndStringVector.begin();
    int ncount = 0;
    for (; itrIDlist != wndStringVector.end(); itrIDlist++)
    {
        std::string strLineID = *itrIDlist;
        CNebulaRouteLine wndNebulaRouteLine;
        wndNebulaRouteGroup.GetRouteLine(strLineID.c_str(), &wndNebulaRouteLine);
        qnzkna::NodeManage::CNodeRelationInfoPtrVector::iterator iter_info_list = m_vecNodeInfo.begin();
        for (; iter_info_list != m_vecNodeInfo.end(); iter_info_list++)
        {
            std::string s_NodeID;
            s_NodeID = (*iter_info_list)->GetNodeID();
            if (0 == strcmp((strLineID).c_str(), s_NodeID.c_str()))
            {
                break;
            }
        }
        if (iter_info_list != m_vecNodeInfo.end())
        {
            std::string s_NodeName, s_NodeID;
            VehicleShowInfo stVehicleShowInfo;
            s_NodeName = (*iter_info_list)->GetNodeName();
            s_NodeID = (*iter_info_list)->GetNodeID();
            stVehicleShowInfo.Flag_In_Group = true;
            stVehicleShowInfo.name = s_NodeName.c_str();
            stVehicleShowInfo.VehicleID = s_NodeID.c_str();
            stVehicleShowInfo.rect.setLeft(s_VehicleShowInfo.rect.left());
            stVehicleShowInfo.rect.setTop(s_VehicleShowInfo.rect.top() + nDesSize * (ncount) - 10);
            stVehicleShowInfo.rect.setRight(stVehicleShowInfo.rect.left() + nDesSize);
            stVehicleShowInfo.rect.setBottom(stVehicleShowInfo.rect.top() + nDesSize);

            stVehicleShowInfo.CountNo = ncount + 1;
            if (wndNebulaRouteGroup.GetFormMode() == CNebulaRouteGroup::EFormMode_Free)
            {
                stVehicleShowInfo.SelectVehicleID = -1;
            }
            else
            {
                stVehicleShowInfo.SelectVehicleID = wndNebulaRouteLine.GetFormOrdinalNum() - 1;
                stVehicleShowInfo.Flag = true;
            }
            m_VehicleShowInfoVec.push_back(stVehicleShowInfo);
            ncount++;
        }
    }
}

void CDlgEditGroup::updateFormsToFormCombo(int nSel)
{
    ui->comboBox_5->disconnect();
    ui->comboBox_5->clear();

    char szBuf[128] = { 0 };
    QString strTmp;
    int nIndex = 0;
    SFormList::iterator iter_list = m_vFormList.begin();
    for (; iter_list != m_vFormList.end(); iter_list++)
    {
        sprintf(szBuf, "%s[%d]", iter_list->m_strName.c_str(), iter_list->GetVehicleNum());
        strTmp = QString(szBuf);
        ui->comboBox_5->insertItem(nIndex++, strTmp);
        if (m_FormationID == iter_list->m_nID)
        {
            nSel = nIndex - 1;
        }
    }

    if (nSel >= 0 && nSel < ui->comboBox_5->count())
    {
        ui->comboBox_5->setCurrentIndex(nSel);

        QString strname = ui->comboBox_5->currentText();;

        emit SetFormLabelTest(strname);
    }
    else
    {

        emit SetFormLabelTest("");
    }
    connect(ui->comboBox_5, SIGNAL(currentIndexChanged(const QString)), this, SLOT(on_comboBox_5_currentIndexChanged(const QString)));
}

void CDlgEditGroup::InitUAVNodeInfo()
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
        return;
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

void CDlgEditGroup::ClearUAVNodeInfo()
{
    qnzkna::NodeManage::CNodeRelationInfoPtrVectorItr itrVec = m_vecNodeInfo.begin();
    for (; itrVec != m_vecNodeInfo.end(); itrVec++)
    {
        qnzkna::NodeManage::CNodeRelationInfo *pNodeInfo = *itrVec;
        if (pNodeInfo)
        {
            delete pNodeInfo;
            pNodeInfo = NULL;
        }
    }
    m_vecNodeInfo.clear();
}

void CDlgEditGroup::ClearWndTaskList()
{
    std::vector<CNebulaTask *>::iterator iter_list = m_vecWndTaskInfo.begin();
    for (; iter_list != m_vecWndTaskInfo.end(); iter_list++)
    {
        CNebulaTask *temp_CNebulaTask = *iter_list;
        if (temp_CNebulaTask)
        {
            delete temp_CNebulaTask;
            temp_CNebulaTask = NULL;
        }
    }
    m_vecWndTaskInfo.clear();
}

void CDlgEditGroup::UpdateTrackInfoToWnd()
{
    m_TaskIdList.clear();
    ClearWndTaskList();
    m_pITaskManageService->GetNebulaTaskIDList(&m_TaskIdList);
    CNebulaTask::NebulaTaskIDList::iterator iter_list = m_TaskIdList.begin();
    for (; iter_list != m_TaskIdList.end(); iter_list++)
    {
        int nID = *iter_list;
        CNebulaTask *temp_CNebulaTask = new CNebulaTask;
        m_pITaskManageService->GetNebulaTask(nID, temp_CNebulaTask);
        m_vecWndTaskInfo.push_back(temp_CNebulaTask);
    }
}

void CDlgEditGroup::UpdateTrackTaskControl()
{
    ui->comboBox->clear();
    ui->comboBox_2->clear();
    ui->comboBox_3->clear();
    ui->comboBox_4->clear();
    m_TaskIdList.clear();

    m_pITaskManageService->GetNebulaTaskIDList(&m_TaskIdList);

    char szBuf[128] = { 0 };
    QString strTmp;
    std::string strTaskName;
    int nIndex = 0;
    CNebulaTask::NebulaTaskIDList::iterator iter_list = m_TaskIdList.begin();
    for (; iter_list != m_TaskIdList.end(); iter_list++)
    {
        int nID = *iter_list;
        strTmp = QString::number(nID);
        ui->comboBox->insertItem(nIndex, strTmp);
        m_pITaskManageService->GetNebulaTaskName(nID, strTaskName);
        ui->comboBox_2->insertItem(nIndex, tr2(strTaskName.c_str()));
        nIndex++;
    }
}

void CDlgEditGroup::UpdateTrackRouteGroupControl()
{
    ui->comboBox_3->clear();

    ui->comboBox_4->clear();

    emit ClearLineGroupCombobox();

    QString str = ui->comboBox->currentText();;
    if (str == "")
    {
        return;
    }
    m_TaskId = str.toInt();

    char szBuf[128] = { 0 };
    QString strTmp;
    std::string strTaskName;
    int nIndex = 0;
    m_pITaskManageService->GetNebulaRouteGroupIDList(m_TaskId, &m_RouteGroupList);
    m_pITaskManageService->GetNebulaRouteGroupNum(m_TaskId, m_GroupNum);
    CNebulaRouteGroup::RouteGroupIDList::iterator iter_list = m_RouteGroupList.begin();
    for (; iter_list != m_RouteGroupList.end(); iter_list++)
    {
        int nID = *iter_list;
        ui->comboBox_3->insertItem(nIndex, QString::number(nID));
        m_pITaskManageService->GetNebulaRouteGroupName(m_TaskId, nID, strTaskName);
        m_pITaskManageService->GetNebulaRouteGroupFormMode(m_TaskId, nID, eMode, m_FormationID);
        if (eMode == CNebulaRouteGroup::EFormMode::EFormMode_Formation)
        {
            ui->radioButton->setChecked(true);
            ui->radioButton_2->setChecked(false);
        }
        else
        {
            ui->radioButton->setChecked(false);
            ui->radioButton_2->setChecked(true);
        }
        SetGroupFormationID(nID, eMode, m_FormationID);
        ui->comboBox_4->insertItem(nIndex, tr2(strTaskName.c_str()));
        emit UpdateLineGroupComboboxItem(nIndex, tr2(strTaskName.c_str()));
        nIndex++;
    }
    ui->comboBox_3->setCurrentIndex(0);
    ui->comboBox_4->setCurrentIndex(0);
    emit SetLineGroupComboboxCul(0);

}

void CDlgEditGroup::UpdateGroupFormationID()
{
    QString str = ui->comboBox->currentText();
    if (str == "")
    {
        return;
    }
    m_TaskId = str.toInt();

    char szBuf[128] = { 0 };
    QString strTmp;
    std::string strTaskName;
    int nIndex = 0;
    m_pITaskManageService->GetNebulaRouteGroupIDList(m_TaskId, &m_RouteGroupList);
    m_pITaskManageService->GetNebulaRouteGroupNum(m_TaskId, m_GroupNum);
    CNebulaRouteGroup::RouteGroupIDList::iterator iter_list = m_RouteGroupList.begin();
    for (; iter_list != m_RouteGroupList.end(); iter_list++)
    {
        int nID = *iter_list;
        m_pITaskManageService->GetNebulaRouteGroupName(m_TaskId, nID, strTaskName);
        m_pITaskManageService->GetNebulaRouteGroupFormMode(m_TaskId, nID, eMode, m_FormationID);
        SetGroupFormationID(nID, eMode, m_FormationID);
    }

}

void CDlgEditGroup::SetGroupFormationID(int GroupID, int eMode, int m_FormationID)
{
    WndGroupInfo ngroup;
    ngroup.NebulaRouteGroup = GroupID;
    ngroup.FormMode = eMode;
    ngroup.FormationID = m_FormationID;
    auto itr = m_vecWndGroupInfoList.begin();
    for (; itr != m_vecWndGroupInfoList.end(); itr++)
    {
        if (GroupID == itr->NebulaRouteGroup)
        {
            itr->FormMode = eMode;
            itr->FormationID = m_FormationID;
            break;
        }
    }
    if (itr == m_vecWndGroupInfoList.end())
    {
        m_vecWndGroupInfoList.push_back(ngroup);
    }
}

void CDlgEditGroup::UpdateGroupFormationToRadioCtrl()
{
    QString	str = ui->comboBox_3->currentText();

    if (str == "")
    {
        return;
    }

    WndGroupInfoList::iterator iter_list = m_vecWndGroupInfoList.begin();
    for (; iter_list != m_vecWndGroupInfoList.end(); iter_list++)
    {
        QString strgroupnum = QString::number(iter_list->NebulaRouteGroup);
        if (QString::compare(str, strgroupnum) == 0)
        {
            if (iter_list->FormMode == 0)
            {
                ui->radioButton->setChecked(true);
                ui->radioButton_2->setChecked(false);
                m_FormationID = iter_list->FormationID;
            }
            else
            {
                ui->radioButton->setChecked(false);
                ui->radioButton_2->setChecked(true);
            }
            break;
        }
    }
}

void CDlgEditGroup::UpdatePaintView()
{
    if (ui->comboBox_5->count() > 0)
    {
        int nCurIndex = ui->comboBox_5->currentIndex();

        UpdateVehicleInfoShow();

        SForm *pForm = getFormByIdx(nCurIndex);
        m_nSelVehicleIcon = -1;
        if (pForm)
        {
            m_sForm = *pForm;
        }

        UpdateChildWndDate();
    }
    else
    {
        ui->label_6->SetUAVNumber(0);
    }
}

void CDlgEditGroup::ClearWndGroupInfoList()
{
    m_vecWndGroupInfoList.clear();
}

void CDlgEditGroup::OuttoTree_Vehicle(string &strWeaponName, string &strWeaponID)
{
    QTreeWidgetItemIterator it(ui->treeWidget);
    while (*it)
    {
        QString strName = QString::fromStdString(strWeaponName);
        if (0 == QString::compare((*it)->text(0), strName))
        {
            (*it)->setCheckState(0, Qt::Checked);
            break;
        }
        ++it;
    }
}

void CDlgEditGroup::ClearS_xygsJL_08()
{

}

void CDlgEditGroup::ClearS_xygsJL_09()
{

}

void CDlgEditGroup::SetBtnStates(bool bstates)
{
    ui->pushButton->setEnabled(bstates);
    ui->pushButton_2->setEnabled(bstates);
    ui->pushButton_3->setEnabled(bstates);
    ui->pushButton_4->setEnabled(!bstates);
    ui->pushButton_5->setEnabled(!bstates);
    ui->comboBox_5->setEnabled(!bstates);
    ui->treeWidget->setEnabled(!bstates);
}

void CDlgEditGroup::UpdateWndData()
{
    ui->comboBox->disconnect();
    ui->comboBox_2->disconnect();
    ui->comboBox_3->disconnect();
    ui->comboBox_4->disconnect();

    UpdateTrackInfoToWnd();
    UpdateTrackTaskControl();

    ui->comboBox->setEnabled(true);
    ui->comboBox_2->setEnabled(true);
    ui->comboBox_3->setEnabled(true);
    ui->comboBox_4->setEnabled(true);
    ui->radioButton->setEnabled(true);
    ui->radioButton_2->setEnabled(true);

    int nSel = ui->comboBox->currentIndex();
    if (nSel == -1)
    {
        return;
    }

    QString str = ui->comboBox->currentText();
    if (ui->comboBox->count() > 0)
    {
        m_TaskId  = str.toInt();
        m_TaskId_Last = m_TaskId;
    }

    UpdateTrackRouteGroupControl();

    InitUAVListInfo();

    UpdateTrackRouteLineControl();

    UpdateFormsFromFile();
    UpdateGroupFormationToRadioCtrl();
    updateFormsToFormCombo(-1);
    UpdatePaintView();

    str = ui->comboBox_3->currentText();
    if (ui->comboBox_3->count() > 0)
    {
        m_GroupID_last = str.toInt();
    }

    m_line_kind = 0;

    emit UpdateLineDlgData(m_TaskId, m_GroupID);
    emit UpdateRouteLineInfoList(m_TaskId, m_GroupID, m_vecNodeInfo);

    repaint();

    connect(ui->comboBox, SIGNAL(currentIndexChanged(const QString)), this, SLOT(on_comboBox_currentIndexChanged(const QString)));
    connect(ui->comboBox_2, SIGNAL(currentIndexChanged(const QString)), this, SLOT(on_comboBox_2_currentIndexChanged(const QString)));
    connect(ui->comboBox_3, SIGNAL(currentIndexChanged(const QString)), this, SLOT(on_comboBox_3_currentIndexChanged(const QString)));
    connect(ui->comboBox_4, SIGNAL(currentIndexChanged(const QString)), this, SLOT(on_comboBox_4_currentIndexChanged(const QString)));

}

void CDlgEditGroup::UpdateTrackRouteLineControl()
{
    QString str;
    int count = 0;
    int nCurIndex = ui->comboBox_3->currentIndex();
    if (nCurIndex == -1)
    {
        m_UAVnumber = 0;
        return;
    }
    str = ui->comboBox_3->currentText();
    m_GroupID = str.toInt();
    m_RouteGroupList.clear();
    CNebulaRouteLine::RouteLineIDList m_Routelinelist;
    m_pITaskManageService->GetNebulaRouteLineIDList(m_TaskId, m_GroupID, &m_Routelinelist);
    int nLineNum;
    m_pITaskManageService->GetNebulaRouteLineNum(m_TaskId, m_GroupID, nLineNum);
    CNebulaRouteLine::RouteLineIDList::iterator iter_list = m_Routelinelist.begin();
    for (; iter_list != m_Routelinelist.end(); iter_list++)
    {
        std::string strWeaponID;
        m_pITaskManageService->GetNebulaWeaponID(m_TaskId, m_GroupID, *iter_list, strWeaponID);
        qnzkna::NodeManage::CNodeRelationInfoPtrVector::iterator iter_info_list = m_vecNodeInfo.begin();
        for (; iter_info_list != m_vecNodeInfo.end(); iter_info_list++)
        {
            std::string s_NodeID;
            std::string s_NodeName;
            s_NodeID = (*iter_info_list)->GetNodeID();
            if (0 == strcmp(s_NodeID.c_str(), strWeaponID.c_str()))
            {
                s_NodeName = (*iter_info_list)->GetNodeName();
                OuttoTree_Vehicle(s_NodeName, strWeaponID);
                count++;
            }
        }
    }
    m_UAVnumber = count;
    Last_UAVnumber = count;
    connect(ui->treeWidget, SIGNAL(itemChanged(QTreeWidgetItem *, int)), this, SLOT(on_treeWidget_itemChanged(QTreeWidgetItem *, int)));
    repaint();
}

void CDlgEditGroup::InitWindow()
{
    UpdateWndData();
}

void CDlgEditGroup::Update(qnzkna::TaskManage::INebulaTaskManageService *sub)
{
    if (sub != NULL)
    {
        UpdateWndData();
    }
}

void CDlgEditGroup::UpdateChildWndDate()
{
    ui->label_6->SetVehicleShowInfo(m_VehicleShowInfoVec);
    ui->label_6->SetUAVNumber(m_UAVnumber);
    ui->label_6->SetScale(m_fScale);
    ui->label_6->SetUAVForm(m_sForm);
}

void CDlgEditGroup::SetWindowStates(E_BTN_STATES eStates)
{
    m_EBtnStates = eStates;
    ui->label_6->SetWndStates(eStates);
}

void CDlgEditGroup::UpdateGroupFromLine(int nGroupID)
{
    UpdateSelectedGroup(nGroupID);
}

void CDlgEditGroup::SetDlgEditLine(CDlgEditLine *pDlg)
{
    if (m_pDlgEditLine == nullptr && pDlg != nullptr)
    {
        m_pDlgEditLine = pDlg;
        connect(m_pDlgEditLine, &CDlgEditLine::UpdateSelectGroup, this, &CDlgEditGroup::UpdateGroupFromLine);
    }
}

void CDlgEditGroup::UpdateSelectedGroup(int nGroupID)
{
    ui->comboBox_3->disconnect();
    ui->comboBox_4->disconnect();

    m_TaskId = ui->comboBox->currentText().toInt();

    if (m_TaskId <= 0 || nGroupID <= 0)
    {
        return;
    }
    m_GroupID = nGroupID;

    int nCountGroupIDNum = ui->comboBox_3->count();
    for (int i = 0; i < nCountGroupIDNum; i++)
    {
        int nID = ui->comboBox_3->itemText(i).toInt();
        if (nID == nGroupID)
        {
            ui->comboBox_3->setCurrentIndex(i);
            break;
        }
    }

    std::string strName;
    m_pITaskManageService->GetNebulaRouteGroupName(m_TaskId, m_GroupID, strName);
    int nGroupNameNum = ui->comboBox_4->count();
    for (int i = 0; i < nGroupNameNum; i++)
    {
        QString strtmp = ui->comboBox_4->itemText(i);
        if (0 == strcmp(strtmp.toLocal8Bit(), strName.c_str()))
        {
            ui->comboBox_4->setCurrentIndex(i);
            emit SetLineGroupComboboxCul(i);
            break;
        }
    }

    InitUAVListInfo();
    UpdateTrackRouteLineControl();

    UpdateFormsFromFile();
    UpdateGroupFormationToRadioCtrl();
    updateFormsToFormCombo(-1);
    UpdatePaintView();
    m_GroupID_last = nGroupID;

    emit UpdateLineDlgData(m_TaskId, m_GroupID);
    emit UpdateRouteLineInfoList(m_TaskId, m_GroupID, m_vecNodeInfo);

    connect(ui->comboBox_3, SIGNAL(currentIndexChanged(const QString)), this, SLOT(on_comboBox_3_currentIndexChanged(const QString)));
    connect(ui->comboBox_4, SIGNAL(currentIndexChanged(const QString)), this, SLOT(on_comboBox_4_currentIndexChanged(const QString)));
}

void CDlgEditGroup::UpdateSelectedTask(int nTaskID)
{
    ClearWndGroupInfoList();

    ui->comboBox->disconnect();
    ui->comboBox_2->disconnect();
    ui->comboBox_3->disconnect();
    ui->comboBox_4->disconnect();

    m_TaskId = nTaskID;

    if (m_TaskId <= 0)
    {
        return;
    }

    int nIDNum = ui->comboBox->count();
    for (int i = 0; i < nIDNum; i++)
    {
        int nID = ui->comboBox->itemText(i).toInt();
        if (nID == nTaskID)
        {
            ui->comboBox->setCurrentIndex(i);
            break;
        }
    }

    std::string strName;
    m_pITaskManageService->GetNebulaTaskName(m_TaskId, strName);
    int nNameNum = ui->comboBox_2->count();
    for (int i = 0; i < nNameNum; i++)
    {
        QString strtmp = ui->comboBox_2->itemText(i);
        if (0 == strcmp(strtmp.toLocal8Bit(), strName.c_str()))
        {
            ui->comboBox_2->setCurrentIndex(i);
            break;
        }
    }

    UpdateTrackRouteGroupControl();

    int nGroupID = ui->comboBox_3->currentText().toInt();
    if (nGroupID > 0)
    {
        m_GroupID = nGroupID;
        UpdateSelectedGroup(nGroupID);
    }

    connect(ui->comboBox, SIGNAL(currentIndexChanged(const QString)), this, SLOT(on_comboBox_currentIndexChanged(const QString)));
    connect(ui->comboBox_2, SIGNAL(currentIndexChanged(const QString)), this, SLOT(on_comboBox_2_currentIndexChanged(const QString)));
}

void CDlgEditGroup::changeEvent(QEvent *event)
{
    LyDialog::changeEvent(event);
    switch (event->type())
    {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

qnzkna::NodeManage::CNodeRelationInfoPtrVector CDlgEditGroup::GetNodeInfoVec()
{
    return m_vecNodeInfo;
}
