#include "CDlgCodeSet_Load_Tree.h"
#include "ui_CDlgCodeSet_Load_Tree.h"
#include "ITaskManagerActivator.h"
#include "QCString.h"
#include "lY_TaskManage.h"
#include "LyMessageBox.h"

CDlgCodeSet_Load_Tree::CDlgCodeSet_Load_Tree(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CDlgCodeSet_Load_Tree)
{
    ui->setupUi(this);
    m_pITaskManageService = ITaskManagerActivator::Get().getPTaskManageService();

    CreateTree();
    CreateTreeMenu();

    UpdateWindowData();
    UpdateTreeData();
}

CDlgCodeSet_Load_Tree::~CDlgCodeSet_Load_Tree()
{
    delete ui;
}

void CDlgCodeSet_Load_Tree::CreateTree()
{

    ui->treeWidget->setColumnCount(11);

    QStringList listheader;
    listheader << tr("No.") << tr(" Platform Name") << tr("Waypoint Number") << tr("Waypoint Longitude") << tr("Waypoint Latitude") << tr("Waypoint Aititude")
               << tr("Mount Device")  << tr("Instruction Execution Sequence") << tr("Instruction Content") << tr("Instruction Parameter") << tr("Instruction Parameter");
    ui->treeWidget->setHeaderLabels(listheader);

    for (int i = 0; i < listheader.size(); i++)
    {
        if (i == 0 || i == 8)
        {
            ui->treeWidget->header()->setSectionResizeMode(i, QHeaderView::ResizeToContents);
        }
        else
        {
            ui->treeWidget->header()->setSectionResizeMode(i, QHeaderView::Interactive);
        }
    }

    ui->treeWidget->header()->setDefaultAlignment(Qt::AlignCenter);

}

void CDlgCodeSet_Load_Tree::UpdateWindowData()
{
    if (m_pITaskManageService == nullptr)
    {
        return;
    }

    m_CodeSet_Load_DB_Vec.clear();
    m_CodeSet_Load_DB_Vec = m_pITaskManageService->GetAllCodeSet_Load();

    m_CodeSetLoadVec.clear();

    for (auto data : m_CodeSet_Load_DB_Vec)
    {
        CodeSetLoad_Parameter stParameter;
        stParameter.nNo = data.GetID();
        stParameter.strLoadID = data.GetLoadID();
        stParameter.nSuitNo = data.GetSuite();
        stParameter.nCodeID = data.GetCodeID();
        stParameter.strParameter = data.GetParameter();
        stParameter.strRemark = data.GetRemark();

        CodeSetLoad_Point stPoint;
        stPoint.nPointID = data.GetPointID();
        stPoint.dLon = data.GetLon();
        stPoint.dLat = data.GetLat();
        stPoint.dAlt = data.GetAlt();
        stPoint.ParameterVec.push_back(stParameter);

        CodeSetLoad stCodeSet;
        stCodeSet.nAcitonID = data.GetActionID();
        stCodeSet.strTaskID = data.GetTaskID();
        stCodeSet.strLineID = data.GetLineID();
        stCodeSet.strPlatformID = data.GetPlatformID();
        stCodeSet.PointParameterVec.push_back(stPoint);

        AddCodeSetLoad(stCodeSet);
    }
}

void CDlgCodeSet_Load_Tree::UpdateTreeData()
{
    ui->treeWidget->clear();

    if (m_pITaskManageService == nullptr)
    {
        return;
    }

    int nTaskID = 1;
    for (auto stCodeSet : m_CodeSetLoadVec)
    {
        QTreeWidgetItem *pRootItem = new QTreeWidgetItem(ui->treeWidget);
        CNebulaMissionPlatform stPlatform = m_pITaskManageService->GetMissionPlatform(stCodeSet.strPlatformID);
        CNeubulaMission stMission = m_pITaskManageService->GetMission(stCodeSet.strTaskID);
        QString strPlatformName = tr2(stPlatform.strMode) + "(" + QString::fromStdString(stMission.GetName()) + ")";
        pRootItem->setText(0, TransformNumberToCode(nTaskID));
        pRootItem->setText(1, strPlatformName);
        pRootItem->setTextAlignment(0, Qt::AlignCenter);
        pRootItem->setTextAlignment(1, Qt::AlignCenter);

        std::map<int, int> nPointIDMap;
        int nPointNo = 1;
        for (auto CodeSetPoint : stCodeSet.PointParameterVec)
        {
            auto itr = nPointIDMap.find(CodeSetPoint.nPointID);
            if (itr == nPointIDMap.end())
            {

                QTreeWidgetItem *pChildItemPoint = new QTreeWidgetItem(pRootItem);
                pChildItemPoint->setText(2, QString::number(CodeSetPoint.nPointID));
                pChildItemPoint->setText(3, QString::number(CodeSetPoint.dLon, 'f', 7));
                pChildItemPoint->setText(4, QString::number(CodeSetPoint.dLat, 'f', 7));
                pChildItemPoint->setText(5, QString::number(CodeSetPoint.dAlt, 'f', 2));

                QString strPointID = "(" + QString::number(nPointNo) + ")";
                pChildItemPoint->setText(0, strPointID);

                pChildItemPoint->setTextAlignment(0, Qt::AlignCenter);
                pChildItemPoint->setTextAlignment(2, Qt::AlignCenter);
                pChildItemPoint->setTextAlignment(3, Qt::AlignCenter);
                pChildItemPoint->setTextAlignment(4, Qt::AlignCenter);
                pChildItemPoint->setTextAlignment(5, Qt::AlignCenter);

                nPointIDMap.insert(std::make_pair(CodeSetPoint.nPointID, 0));

                for (auto CodeSetPointPara : CodeSetPoint.ParameterVec)
                {

                    QTreeWidgetItem *pChildItemPointParameter = new QTreeWidgetItem(pChildItemPoint);
                    CNebulaMissionPlatformLoad stLoad = m_pITaskManageService->GetMissionPlatformLoad(CodeSetPointPara.strLoadID);
                    pChildItemPointParameter->setText(0, QString::number(CodeSetPointPara.nNo));
                    pChildItemPointParameter->setText(6, tr2(stLoad.strMode));
                    pChildItemPointParameter->setText(7, QString::number(CodeSetPointPara.nSuitNo));
                    qnzkna::NebulaMissionManage::NebulaMissionAutoCommandManage::CodeSetLoad_Parameter_CodeID ePara = (qnzkna::NebulaMissionManage::NebulaMissionAutoCommandManage::CodeSetLoad_Parameter_CodeID)CodeSetPointPara.nCodeID;
                    QString strCodeName = qnzkna::NebulaMissionManage::NebulaMissionAutoCommandManage::CodeSetLoad_Parameter_CodeID_To_String(ePara);
                    pChildItemPointParameter->setText(8, strCodeName);
                    pChildItemPointParameter->setText(9, QString::fromStdString(CodeSetPointPara.strParameter));
                    pChildItemPointParameter->setText(10, QString::fromStdString(CodeSetPointPara.strRemark));

                    pChildItemPointParameter->setTextAlignment(0, Qt::AlignCenter);
                    pChildItemPointParameter->setTextAlignment(6, Qt::AlignCenter);
                    pChildItemPointParameter->setTextAlignment(7, Qt::AlignCenter);
                    pChildItemPointParameter->setTextAlignment(8, Qt::AlignCenter);
                    pChildItemPointParameter->setTextAlignment(9, Qt::AlignCenter);
                    pChildItemPointParameter->setTextAlignment(10, Qt::AlignCenter);

                }
            }
            nPointNo++;
        }
        nTaskID++;
    }

    ui->treeWidget->expandAll();
}

void CDlgCodeSet_Load_Tree::CreateTreeMenu()
{
    m_TreeMenu = new QMenu(this);
    m_MenuAdd = m_TreeMenu->addAction(tr("Add"));
    m_MenuEdit = m_TreeMenu->addAction(tr("Edit"));
    m_MenuRemove = m_TreeMenu->addAction(tr("Delete"));
    m_MenuUpdate = m_TreeMenu->addAction(tr("Update"));

    connect(m_MenuAdd, SIGNAL(triggered()), this, SLOT(on_Menu_Add()));
    connect(m_MenuEdit, SIGNAL(triggered()), this, SLOT(on_Menu_Edit()));
    connect(m_MenuRemove, SIGNAL(triggered()), this, SLOT(on_Menu_Remove()));
    connect(m_MenuUpdate, SIGNAL(triggered()), this, SLOT(on_Menu_Update()));

}

void CDlgCodeSet_Load_Tree::AddCodeSetLoad(const CodeSetLoad stCodeSetLoad)
{
    if (m_CodeSetLoadVec.size() == 0)
    {
        m_CodeSetLoadVec.push_back(stCodeSetLoad);
        return;
    }

    auto itr = m_CodeSetLoadVec.begin();
    for (; itr != m_CodeSetLoadVec.end(); itr++)
    {
        if (0 == strcmp(itr->strTaskID.c_str(), stCodeSetLoad.strTaskID.c_str()) &&
                0 == strcmp(itr->strPlatformID.c_str(), stCodeSetLoad.strPlatformID.c_str()))
        {
            for (auto &CodeSetPoint : stCodeSetLoad.PointParameterVec)
            {
                AddCodeSetLoad_Point(itr->strTaskID, itr->strPlatformID, CodeSetPoint);
            }
            break;
        }
    }

    if (itr == m_CodeSetLoadVec.end())
    {
        m_CodeSetLoadVec.push_back(stCodeSetLoad);
    }
}

void CDlgCodeSet_Load_Tree::AddCodeSetLoad_Point(const std::string strTaskID, const std::string strPlatformID, const CodeSetLoad_Point stCodeSetLoad_Point)
{
    for (auto &CodeSet : m_CodeSetLoadVec)
    {
        if (0 == strcmp(CodeSet.strTaskID.c_str(), strTaskID.c_str()) &&
                0 == strcmp(CodeSet.strPlatformID.c_str(), strPlatformID.c_str()))
        {
            auto CodeSetPointItr = CodeSet.PointParameterVec.begin();
            for (; CodeSetPointItr != CodeSet.PointParameterVec.end(); CodeSetPointItr++)
            {
                if (stCodeSetLoad_Point.nPointID == CodeSetPointItr->nPointID)
                {
                    for (auto &CodeSetPointParameter : stCodeSetLoad_Point.ParameterVec)
                    {
                        AddCodeSetLoad_Parameter(CodeSet.strTaskID, CodeSet.strPlatformID, CodeSetPointItr->nPointID, CodeSetPointParameter);
                    }
                    break;
                }

            }
            if (CodeSetPointItr == CodeSet.PointParameterVec.end())
            {
                CodeSet.PointParameterVec.push_back(stCodeSetLoad_Point);
            }
        }
    }
}

void CDlgCodeSet_Load_Tree::AddCodeSetLoad_Parameter(const std::string strTaskID, const std::string strPlatformID, const int nPointID, const CodeSetLoad_Parameter stCodeSetLoad_Point)
{
    for (auto &CodeSet : m_CodeSetLoadVec)
    {
        if (0 == strcmp(CodeSet.strTaskID.c_str(), strTaskID.c_str()) &&
                0 == strcmp(CodeSet.strPlatformID.c_str(), strPlatformID.c_str()))
        {
            for (auto &CodeSetPoint : CodeSet.PointParameterVec)
            {
                if (nPointID == CodeSetPoint.nPointID)
                {
                    auto itr = CodeSetPoint.ParameterVec.begin();
                    for (; itr != CodeSetPoint.ParameterVec.end(); itr++)
                    {
                        if (itr->nNo == stCodeSetLoad_Point.nNo)
                        {
                            *itr = stCodeSetLoad_Point;
                            break;
                        }
                    }

                    if (itr == CodeSetPoint.ParameterVec.end())
                    {
                        CodeSetPoint.ParameterVec.push_back(stCodeSetLoad_Point);
                    }
                }
            }
        }
    }
}

int CDlgCodeSet_Load_Tree::GetSelectedTreeLevel()
{
    QTreeWidgetItem *ItemChild = ui->treeWidget->currentItem();
    int nLevel = 0;
    while (ItemChild != nullptr)
    {
        QString str = ItemChild->text(0);
        QTreeWidgetItem *ItemParent = ItemChild->parent();
        ItemChild = ItemParent;
        nLevel++;
    }

    return nLevel;
}

CCodeSet_Load_DB CDlgCodeSet_Load_Tree::getSelectedInfo(const int nLevel)
{
    CCodeSet_Load_DB stCodeSetLoad;

    QTreeWidgetItem *pItemSelected = ui->treeWidget->currentItem();
    if (pItemSelected == nullptr)
    {
        return stCodeSetLoad;
    }

    if (nLevel == 1)
    {
        for (auto stCodeSet : m_CodeSetLoadVec)
        {
            CNebulaMissionPlatform stPlatform = m_pITaskManageService->GetMissionPlatform(stCodeSet.strPlatformID);
            CNeubulaMission stMission = m_pITaskManageService->GetMission(stCodeSet.strTaskID);
            QString strPlatformName = tr2(stPlatform.strMode) + "(" + QString::fromStdString(stMission.GetName()) + ")";

            if (strPlatformName.compare(pItemSelected->text(1)) == 0)
            {
                stCodeSetLoad.SetTaskID(stCodeSet.strTaskID);
                stCodeSetLoad.SetPlatformID(stCodeSet.strPlatformID);
                break;
            }
        }
    }
    else if (nLevel == 2)
    {
        QTreeWidgetItem *pItemSelectedPar = pItemSelected->parent();
        if (pItemSelectedPar == nullptr)
        {
            return stCodeSetLoad;
        }

        for (auto stCodeSet : m_CodeSetLoadVec)
        {
            CNebulaMissionPlatform stPlatform = m_pITaskManageService->GetMissionPlatform(stCodeSet.strPlatformID);
            CNeubulaMission stMission = m_pITaskManageService->GetMission(stCodeSet.strTaskID);
            QString strPlatformName = tr2(stPlatform.strMode) + "(" + QString::fromStdString(stMission.GetName()) + ")";

            if (strPlatformName.compare(pItemSelectedPar->text(1)) == 0)
            {
                stCodeSetLoad.SetTaskID(stCodeSet.strTaskID);
                stCodeSetLoad.SetPlatformID(stCodeSet.strPlatformID);
                break;
            }
        }

        QString strPointID = pItemSelected->text(2);
        stCodeSetLoad.SetPointID(strPointID.toInt());
    }
    else if (nLevel == 3)
    {
        QString strCodeNo = pItemSelected->text(0);
        stCodeSetLoad = m_pITaskManageService->GetCodeSet_Load(strCodeNo.toInt());
    }
    return stCodeSetLoad;
}

QString CDlgCodeSet_Load_Tree::TransformNumberToCode(int nNo)
{
    QString strChinese = "";
    if (nNo == 10)
    {
        strChinese = tr("Ten ");
    }
    else if (nNo > 10)
    {
        int n1 = nNo / 10;
        int n2 = nNo - n1 * 10;
        if (n1 == 1)
        {
            strChinese = tr("Ten ") + TransformNumberToChinese(n2);
        }
        else
        {
            strChinese = TransformNumberToChinese(n1) + tr(" Ten ") + TransformNumberToChinese(n2);
        }
    }
    else if (nNo > 100)
    {
        strChinese = "";
    }
    else
    {
        strChinese = TransformNumberToChinese(nNo);
    }
    return strChinese;
}

QString CDlgCodeSet_Load_Tree::TransformNumberToChinese(int nNo)
{
    QString str = "";
    switch (nNo)
    {
    case 1:
        str = tr("One");
        break;
    case 2:
        str = tr("Two");
        break;
    case 3:
        str = tr("Three");
        break;
    case 4:
        str = tr("Four");
        break;
    case 5:
        str = tr("Five");
        break;
    case 6:
        str = tr("Six");
        break;
    case 7:
        str = tr("Seven");
        break;
    case 8:
        str = tr("Eight");
        break;
    case 9:
        str = tr("Nine");
        break;
    default:
        str = "";
        break;
    }
    return str;
}

CodeSetLoadVec CDlgCodeSet_Load_Tree::GetSortOutData()
{
    return m_CodeSetLoadVec;
}

void CDlgCodeSet_Load_Tree::on_treeWidget_customContextMenuRequested(const QPoint &pos)
{
    m_TreeMenu->exec(QCursor::pos());
}

void CDlgCodeSet_Load_Tree::on_Menu_Add()
{
    int nLevel = GetSelectedTreeLevel();
    CCodeSet_Load_DB stCodeSetDB = getSelectedInfo(nLevel);
    LY_TaskManage::Get().getDlgCodeSet_Load_Edit()->SetShowData(stCodeSetDB);

    if (LY_TaskManage::Get().getDlgCodeSet_Load_Edit()->isVisible())
    {
        LY_TaskManage::Get().getDlgCodeSet_Load_Edit()->hide();
    }
    LY_TaskManage::Get().getDlgCodeSet_Load_Edit()->show();
}

void CDlgCodeSet_Load_Tree::on_Menu_Edit()
{
    int nLevel = GetSelectedTreeLevel();
    CCodeSet_Load_DB stCodeSetDB = getSelectedInfo(nLevel);
    if (stCodeSetDB.GetID() == 0)
    {
        LyMessageBox::information(NULL, tr("tip"), tr("Please select the command you want to edit!"));
        return;
    }

    LY_TaskManage::Get().getDlgCodeSet_Load_Edit()->SetShowData(stCodeSetDB);

    if (LY_TaskManage::Get().getDlgCodeSet_Load_Edit()->isVisible())
    {
        LY_TaskManage::Get().getDlgCodeSet_Load_Edit()->hide();
    }
    LY_TaskManage::Get().getDlgCodeSet_Load_Edit()->show();

}

void CDlgCodeSet_Load_Tree::on_Menu_Remove()
{
    int nLevel = GetSelectedTreeLevel();
    CCodeSet_Load_DB stCodeSetDB = getSelectedInfo(nLevel);
    if (stCodeSetDB.GetID() == 0)
    {
        LyMessageBox::information(NULL, tr("tip"), tr("Please select the command you want to delete!"));
        return;
    }

    QString strTmp = tr("Please confirm whether to delete the serial number: ") + QString::number(stCodeSetDB.GetID()) + tr("?");
    if (QMessageBox::StandardButton::Yes == LyMessageBox::question(this, tr("tip"), strTmp, QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel))
    {
        bool bresut = m_pITaskManageService->RmoveCodeSet_Load(stCodeSetDB.GetID());
        if (bresut == true)
        {
        }
        else
        {
            LyMessageBox::information(NULL, tr("tip"), tr("Delete failed"));
            return;
        }
    }
    on_Menu_Update();
}

void CDlgCodeSet_Load_Tree::on_Menu_Update()
{
    UpdateWindowData();
    UpdateTreeData();
}
