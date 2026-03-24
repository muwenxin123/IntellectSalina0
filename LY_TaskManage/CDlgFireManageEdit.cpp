#include "CDlgFireManageEdit.h"
#include "ui_CDlgFireManageEdit.h"
#include "QCString.h"
#include "lY_TaskManage.h"
#include <QCheckBox>

CDlgFireManageEdit::CDlgFireManageEdit(QWidget *parent) :
    LyDialog(tr("Fire Control Unit Setup"), parent),
    ui(new Ui::CDlgFireManageEdit)
{
    ui->setupUi(centralWidget());
    this->setResizable(true);
    this->resize(800, 760);

    m_pITaskManageService = ITaskManagerActivator::Get().getPTaskManageService();
    if (m_pITaskManageService == nullptr)
    {
        return;
    }

    m_pIMarkManageService = ITaskManagerActivator::Get().getPIMarkManageService();
    if (m_pIMarkManageService == nullptr)
    {
        return;
    }

    ConnectControl();
    InitChart();
}

CDlgFireManageEdit::~CDlgFireManageEdit()
{
    delete ui;
}

void CDlgFireManageEdit::on_comboBox_currentIndexChanged(int index)
{
    SetPointID(index + 1);
    UpdateListData();
    UpdateChart();

}

void CDlgFireManageEdit::on_pushButton_clicked()
{
    QVariant  qv = ui->comboBox->itemData(ui->comboBox->currentIndex());
    int nPointNo = qv.toInt();
    int nPointCount = ui->listWidget->count();

    m_pITaskManageService->RmoveCodeSet_Load(m_strTaskID, m_strPlatformID, m_strLoadID, nPointNo);

    for (int i = 0; i < nPointCount; i++)
    {
        QListWidgetItem *pItem = ui->listWidget->item(i);
        int nCodeID = pItem->data(Qt::UserRole).toInt();
        int nCodePara = nCodeID % 10;
        nCodeID = nCodeID / 10;

        CCodeSet_Load_DB stCodeSet;
        int nID = m_pITaskManageService->GetCodeSet_Load_MaxID() + 1;
        stCodeSet.SetID(nID);
        stCodeSet.SetActionID(m_pITaskManageService->GetActivatingActionID());
        stCodeSet.SetTaskID(m_strTaskID);
        stCodeSet.SetLineID(QString::number(m_LineID).toStdString());
        stCodeSet.SetPlatformID(m_strPlatformID);
        stCodeSet.SetLoadID(m_strLoadID);
        stCodeSet.SetPointID(nPointNo);
        CNeubulaMissionLinePoint stPoint = m_pITaskManageService->GetMissionLinePoint(m_LineID, nPointNo);
        stCodeSet.SetLon(stPoint.dLon);
        stCodeSet.SetLat(stPoint.dLat);
        stCodeSet.SetAlt(stPoint.dAlt);
        stCodeSet.SetSuite(i + 1);
        stCodeSet.SetCodeID(nCodeID);

        CodeSetLoad_CommonCommand st1;
        UpdateCodeSetLoadCommonCommand(st1, nCodeID, nCodePara);
        stCodeSet.SetParameter(std::string((char *)&st1, sizeof(st1)));

        m_pITaskManageService->SetCodeSet_Load(stCodeSet);

    }
    LY_TaskManage::Get().UpdateCodeSetFire();

    this->hide();
}

void CDlgFireManageEdit::on_pushButton_2_clicked()
{
    this->hide();
}

void CDlgFireManageEdit::showEvent(QShowEvent *event)
{
    InitControl();
    CalculateMark();
    UpdateLinePointCombobox();
    UpdateListData();

    UpdateChart();
}

void CDlgFireManageEdit::closeEvent(QCloseEvent *event)
{

}

void CDlgFireManageEdit::InitControl()
{
    ui->comboBox_3->setCurrentIndex(0);
    ui->comboBox_4->setCurrentIndex(0);
    ui->comboBox_5->setCurrentIndex(0);
    ui->label_7->setText("");
}

void CDlgFireManageEdit::UpdateLinePointCombobox()
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
    }

    int nIndex01 = ui->comboBox->findText(QString::number(m_PointID));
    ui->comboBox->setCurrentIndex(nIndex01);
    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(on_comboBox_currentIndexChanged(int)));
}

void CDlgFireManageEdit::InitLoadCombobox()
{
    for (int i = 0; i < 9; i++)
    {
        QString strText = QString(tr("%1 number ammunition")).arg(i + 1);

        ui->comboBox_3->insertItem(i, strText);
        ui->comboBox_3->setItemData(i, i + 1);

        ui->comboBox_4->insertItem(i, strText);
        ui->comboBox_4->setItemData(i, i + 1);

        ui->comboBox_5->insertItem(i, strText);
        ui->comboBox_5->setItemData(i, i + 1);
    }
}

void CDlgFireManageEdit::ConnectControl()
{

    InitLoadCombobox();

    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(on_comboBox_currentIndexChanged(int)));
    connect(ui->comboBox_3, SIGNAL(currentIndexChanged(int)), this, SLOT(on_comboBox_3_currentIndexChanged(int)));
    connect(ui->comboBox_4, SIGNAL(currentIndexChanged(int)), this, SLOT(on_comboBox_4_currentIndexChanged(int)));
    connect(ui->comboBox_5, SIGNAL(currentIndexChanged(int)), this, SLOT(on_comboBox_5_currentIndexChanged(int)));

    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(on_pushButton_clicked()));
    connect(ui->pushButton_2, SIGNAL(clicked()), this, SLOT(on_pushButton_2_clicked()));

    connect(ui->pushButton_3, SIGNAL(clicked()), this, SLOT(on_pushButton_3_clicked()));
    connect(ui->pushButton_4, SIGNAL(clicked()), this, SLOT(on_pushButton_4_clicked()));
    connect(ui->pushButton_5, SIGNAL(clicked()), this, SLOT(on_pushButton_5_clicked()));
    connect(ui->pushButton_7, SIGNAL(clicked()), this, SLOT(on_pushButton_7_clicked()));

    connect(ui->listWidget, SIGNAL(currentTextChanged(const QString &)), this, SLOT(on_listWidget_currentTextChanged(const QString &)));
}

void CDlgFireManageEdit::SetCodeSetData(const CCodeSet_Load_DB_Vec CodeSetVec)
{
    m_CodeSet_Load_DB_Vec = CodeSetVec;
}

void CDlgFireManageEdit::SetTaskID(const std::string strID)
{
    m_strTaskID = strID;
}

void CDlgFireManageEdit::SetPlatformID(const std::string strID)
{
    m_strPlatformID = strID;
}

void CDlgFireManageEdit::SetLoadID(const std::string strID)
{
    m_strLoadID = strID;
}

void CDlgFireManageEdit::SetLineID(const int nLineID)
{
    m_LineID = nLineID;
}

void CDlgFireManageEdit::SetPointID(const int nID)
{
    m_PointID = nID;

    m_CodeSet_Load_DB_Wnd_Vec.clear();
    for (auto CodeSet : m_CodeSet_Load_DB_Vec)
    {
        if (0 == strcmp(CodeSet.GetLoadID().c_str(), m_strLoadID.c_str()) &&
                0 == strcmp(CodeSet.GetPlatformID().c_str(), m_strPlatformID.c_str()) &&
                CodeSet.GetPointID() == m_PointID)
        {
            m_CodeSet_Load_DB_Wnd_Vec.push_back(CodeSet);
        }
    }
}

void CDlgFireManageEdit::SetMarkIDList(const QStringList strList)
{
    m_MarkList = strList;
}

void CDlgFireManageEdit::UpdateListData()
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
        CodeSetLoad_CommonCommand st1 = *((CodeSetLoad_CommonCommand *)(CodeSet.GetParameter().c_str()));
        if (ePara == CodeSetLoad_Parameter_CodeID_DY_CommonCommand_Unlock_Insure)
        {
            UpdateListDataState(strCodeName, ePara, 1, st1.para.DY_CommonCommand_Unlock_Insure.bomb1);
            UpdateListDataState(strCodeName, ePara, 2, st1.para.DY_CommonCommand_Unlock_Insure.bomb2);
            UpdateListDataState(strCodeName, ePara, 3, st1.para.DY_CommonCommand_Unlock_Insure.bomb3);
            UpdateListDataState(strCodeName, ePara, 4, st1.para.DY_CommonCommand_Unlock_Insure.bomb4);
            UpdateListDataState(strCodeName, ePara, 5, st1.para.DY_CommonCommand_Unlock_Insure.bomb5);
            UpdateListDataState(strCodeName, ePara, 6, st1.para.DY_CommonCommand_Unlock_Insure.bomb6);
            UpdateListDataState(strCodeName, ePara, 7, st1.para.DY_CommonCommand_Unlock_Insure.bomb7);
            UpdateListDataState(strCodeName, ePara, 8, st1.para.DY_CommonCommand_Unlock_Insure.bomb8);
            UpdateListDataState(strCodeName, ePara, 9, st1.para.DY_CommonCommand_Unlock_Insure.bomb9);
        }
        else if (ePara == CodeSetLoad_Parameter_CodeID_DY_CommonCommand_Shot_Percussion)
        {
            UpdateListDataState(strCodeName, ePara, 1, st1.para.DY_CommonCommand_Shot_Percussion.bomb1);
            UpdateListDataState(strCodeName, ePara, 2, st1.para.DY_CommonCommand_Shot_Percussion.bomb2);
            UpdateListDataState(strCodeName, ePara, 3, st1.para.DY_CommonCommand_Shot_Percussion.bomb3);
            UpdateListDataState(strCodeName, ePara, 4, st1.para.DY_CommonCommand_Shot_Percussion.bomb4);
            UpdateListDataState(strCodeName, ePara, 5, st1.para.DY_CommonCommand_Shot_Percussion.bomb5);
            UpdateListDataState(strCodeName, ePara, 6, st1.para.DY_CommonCommand_Shot_Percussion.bomb6);
            UpdateListDataState(strCodeName, ePara, 7, st1.para.DY_CommonCommand_Shot_Percussion.bomb7);
            UpdateListDataState(strCodeName, ePara, 8, st1.para.DY_CommonCommand_Shot_Percussion.bomb8);
            UpdateListDataState(strCodeName, ePara, 9, st1.para.DY_CommonCommand_Shot_Percussion.bomb9);
        }
        else if (ePara == CodeSetLoad_Parameter_CodeID_DY_CommonCommand_Lock_Reset)
        {
            UpdateListDataState(strCodeName, ePara, 1, st1.para.DY_CommonCommand_Lock_Reset.bomb1);
            UpdateListDataState(strCodeName, ePara, 2, st1.para.DY_CommonCommand_Lock_Reset.bomb2);
            UpdateListDataState(strCodeName, ePara, 3, st1.para.DY_CommonCommand_Lock_Reset.bomb3);
            UpdateListDataState(strCodeName, ePara, 4, st1.para.DY_CommonCommand_Lock_Reset.bomb4);
            UpdateListDataState(strCodeName, ePara, 5, st1.para.DY_CommonCommand_Lock_Reset.bomb5);
            UpdateListDataState(strCodeName, ePara, 6, st1.para.DY_CommonCommand_Lock_Reset.bomb6);
            UpdateListDataState(strCodeName, ePara, 7, st1.para.DY_CommonCommand_Lock_Reset.bomb7);
            UpdateListDataState(strCodeName, ePara, 8, st1.para.DY_CommonCommand_Lock_Reset.bomb8);
            UpdateListDataState(strCodeName, ePara, 9, st1.para.DY_CommonCommand_Lock_Reset.bomb9);
        }
    }
}

void CDlgFireManageEdit::UpdateListDataState(const QString strCodeName, const int strCodeID, const unsigned int nNo, const unsigned int nStates)
{
    if (nStates != 1)
    {
        return;
    }

    QString strShow;
    QListWidgetItem *item = new QListWidgetItem;
    item->setData(Qt::UserRole, QString::number(strCodeID * 10 + nNo));
    strShow = strCodeName + tr("[") + QString::number(nNo) + tr("number ammunition]");
    item->setText(strShow);
    ui->listWidget->addItem(item);
}

void CDlgFireManageEdit::CalculateMark()
{
    m_MarkList.clear();
    m_pQCheckBoxMarkMap.clear();

    QLayoutItem *child;
    while ((child = ui->horizontalLayout_12->takeAt(0)) != 0)
    {
        delete child->widget();
        delete child;
    }

    CNebulaAcitonRelationVec RelationVec = m_pITaskManageService->GetActivatingShowData();

    for (auto action : RelationVec)
    {
        for (auto Task : action.ReTaskVec)
        {
            if (0 == strcmp(Task.strTaskID.c_str(), m_strTaskID.c_str()))
            {
                for (auto Platform : Task.PlatFormVec)
                {
                    if (0 == strcmp(Platform.strPlatformID.c_str(), m_strPlatformID.c_str()))
                    {
                        m_LineID = atoi(Platform.strLineID.c_str());

                        int nNo = 0;
                        for (auto load : Platform.strLoadIDVec)
                        {
                            if (0 == strcmp(load.c_str(), m_strLoadID.c_str()))
                            {
                                auto itr = Platform.strMarkIDvec.begin();
                                if (nNo < Platform.strMarkIDvec.size())
                                {
                                    itr += nNo;
                                    m_MarkList.push_back(QString::fromStdString(*itr));
                                    QCheckBox *pcheckbox = new QCheckBox;
                                    pcheckbox->setText(QString::fromStdString(*itr));
                                    pcheckbox->setChecked(true);
                                    ui->horizontalLayout_12->addWidget(pcheckbox);

                                    m_pQCheckBoxMarkMap.insert(pcheckbox, QString::fromStdString(*itr));
                                }
                            }
                            nNo++;
                        }
                        break;
                    }
                }
                break;
            }
        }
    }
}

void CDlgFireManageEdit::InitChart()
{
    m_axisX = new QValueAxis();
    m_axisY = new QValueAxis();
    m_axisX->setTitleText(tr("Distance"));
    m_axisY->setTitleText(tr("Altitude"));
    m_axisX->setMin(0);
    m_axisY->setMin(0);
    m_axisX->setMax(m_AXIS_MAX_X);
    m_axisY->setMax(m_AXIS_MAX_Y);

    m_chart = new QChart();
    m_chart->addAxis(m_axisX, Qt::AlignBottom);
    m_chart->addAxis(m_axisY, Qt::AlignLeft);

    m_lineSeries = new QLineSeries();
    m_lineSeries->setPointsVisible(false);
    m_lineSeries->setName(tr("Curve of Trajectory"));
    m_chart->addSeries(m_lineSeries);
    m_lineSeries->attachAxis(m_axisX);
    m_lineSeries->attachAxis(m_axisY);

    m_chart->setTheme(QChart::ChartThemeBlueCerulean);

    m_chart->setAnimationOptions(QChart::SeriesAnimations);
    ui->graphicsView->setChart(m_chart);
    ui->graphicsView->setRenderHint(QPainter::Antialiasing);
}

void CDlgFireManageEdit::ShowChartCCIP(std::list<qnzkna::ly_ccip::ly_ccip_80mm::ccipdata_80mm> &cciplist, double &dAlt)
{
    m_lineSeries->clear();
    for (auto ccip : cciplist)
    {
        double dx = ccip.x;
        double dy = dAlt + ccip.zd;
        m_lineSeries->append(QPointF(dx, dy));
        if (m_AXIS_MAX_X < dx)
        {
            m_AXIS_MAX_X = dx;
        }
        if (m_AXIS_MAX_Y < dy)
        {
            m_AXIS_MAX_Y = dy;
        }
    }
}

void CDlgFireManageEdit::ShowChartMark(QLineSeries *pSeries, double &dMarkDis)
{
    pSeries->clear();
    pSeries->append(QPointF(dMarkDis, 0));

    if (m_AXIS_MAX_X < dMarkDis)
    {
        m_AXIS_MAX_X = dMarkDis;
    }
}

void CDlgFireManageEdit::UpdateChart()
{
    if (m_pITaskManageService == nullptr ||
            m_pIMarkManageService == nullptr)
    {
        return;
    }

    m_AXIS_MAX_X = 0;
    m_AXIS_MAX_Y = 0;
    ClearMarkSerious();

    QVariant  qv = ui->comboBox->itemData(ui->comboBox->currentIndex());
    int nPointNo = qv.toInt();
    CNeubulaMissionLinePoint stPoint = m_pITaskManageService->GetMissionLinePoint(m_LineID, nPointNo);

    double nHeihtGps = 0;
    double nHeihtAboveDist = 1.0;
    double nHeihtAbove = 0;
    std::list<qnzkna::ly_ccip::ly_ccip_80mm::ccipdata_80mm> ccipdata_80mmlist;
    qnzkna::ly_ccip::ly_ccip_80mm::ccip_80mm_max_dist(stPoint.dLon, stPoint.dLat, stPoint.dAlt + nHeihtGps, 0, stPoint.dAlt, 0, ccipdata_80mmlist);
    ShowChartCCIP(ccipdata_80mmlist, stPoint.dAlt);

    for (auto itr : m_MarkList)
    {
        SystemMarkInfo stInfo = m_pIMarkManageService->GetMark(itr.toStdString());

        QGeoCoordinate QPoint1, QPoint2;
        QPoint1.setLongitude(stPoint.dLon);
        QPoint1.setLatitude(stPoint.dLat);
        QPoint2.setLongitude(stInfo.GetLon());
        QPoint2.setLatitude(stInfo.GetLat());
        double dDis = QPoint1.distanceTo(QPoint2);

        QLineSeries *pSeries = new QLineSeries();
        pSeries->setPointsVisible(true);
        QString strTitle = tr("Mark Position:") + QString::fromStdString(stInfo.GetName());
        pSeries->setName(strTitle);
        m_chart->addSeries(pSeries);
        pSeries->attachAxis(m_axisX);
        pSeries->attachAxis(m_axisY);
        m_pQLineSeriesVec.push_back(pSeries);

        ShowChartMark(pSeries, dDis);
    }

    m_chart->axisX()->setMax(m_AXIS_MAX_X + 10);
    m_chart->axisY()->setMax(m_AXIS_MAX_Y + 1);
}

void CDlgFireManageEdit::UpdateCodeSetLoadCommonCommand(CodeSetLoad_CommonCommand &stCode, int &nCoidID, int &nCodePara)
{
    if (nCoidID == CodeSetLoad_Parameter_CodeID_DY_CommonCommand_Unlock_Insure)
    {
        stCode.nCodeID = CodeSetLoad_Parameter_CodeID_DY_CommonCommand_Unlock_Insure;
        switch (nCodePara)
        {
        case 1:
            stCode.para.DY_CommonCommand_Unlock_Insure.bomb1 = 1;
            break;
        case 2:
            stCode.para.DY_CommonCommand_Unlock_Insure.bomb2 = 1;
            break;
        case 3:
            stCode.para.DY_CommonCommand_Unlock_Insure.bomb3 = 1;
            break;
        case 4:
            stCode.para.DY_CommonCommand_Unlock_Insure.bomb4 = 1;
            break;
        case 5:
            stCode.para.DY_CommonCommand_Unlock_Insure.bomb5 = 1;
            break;
        case 6:
            stCode.para.DY_CommonCommand_Unlock_Insure.bomb6 = 1;
            break;
        case 7:
            stCode.para.DY_CommonCommand_Unlock_Insure.bomb7 = 1;
            break;
        case 8:
            stCode.para.DY_CommonCommand_Unlock_Insure.bomb8 = 1;
            break;
        case 9:
            stCode.para.DY_CommonCommand_Unlock_Insure.bomb9 = 1;
            break;
        default:
            break;
        }
    }
    else if (nCoidID == CodeSetLoad_Parameter_CodeID_DY_CommonCommand_Shot_Percussion)
    {
        stCode.nCodeID = CodeSetLoad_Parameter_CodeID_DY_CommonCommand_Shot_Percussion;
        switch (nCodePara)
        {
        case 1:
            stCode.para.DY_CommonCommand_Shot_Percussion.bomb1 = 1;
            break;
        case 2:
            stCode.para.DY_CommonCommand_Shot_Percussion.bomb2 = 1;
            break;
        case 3:
            stCode.para.DY_CommonCommand_Shot_Percussion.bomb3 = 1;
            break;
        case 4:
            stCode.para.DY_CommonCommand_Shot_Percussion.bomb4 = 1;
            break;
        case 5:
            stCode.para.DY_CommonCommand_Shot_Percussion.bomb5 = 1;
            break;
        case 6:
            stCode.para.DY_CommonCommand_Shot_Percussion.bomb6 = 1;
            break;
        case 7:
            stCode.para.DY_CommonCommand_Shot_Percussion.bomb7 = 1;
            break;
        case 8:
            stCode.para.DY_CommonCommand_Shot_Percussion.bomb8 = 1;
            break;
        case 9:
            stCode.para.DY_CommonCommand_Shot_Percussion.bomb9 = 1;
            break;
        default:
            break;
        }
    }
    else if (nCoidID == CodeSetLoad_Parameter_CodeID_DY_CommonCommand_Lock_Reset)
    {
        stCode.nCodeID = CodeSetLoad_Parameter_CodeID_DY_CommonCommand_Lock_Reset;
        switch (nCodePara)
        {
        case 1:
            stCode.para.DY_CommonCommand_Lock_Reset.bomb1 = 1;
            break;
        case 2:
            stCode.para.DY_CommonCommand_Lock_Reset.bomb2 = 1;
            break;
        case 3:
            stCode.para.DY_CommonCommand_Lock_Reset.bomb3 = 1;
            break;
        case 4:
            stCode.para.DY_CommonCommand_Lock_Reset.bomb4 = 1;
            break;
        case 5:
            stCode.para.DY_CommonCommand_Lock_Reset.bomb5 = 1;
            break;
        case 6:
            stCode.para.DY_CommonCommand_Lock_Reset.bomb6 = 1;
            break;
        case 7:
            stCode.para.DY_CommonCommand_Lock_Reset.bomb7 = 1;
            break;
        case 8:
            stCode.para.DY_CommonCommand_Lock_Reset.bomb8 = 1;
            break;
        case 9:
            stCode.para.DY_CommonCommand_Lock_Reset.bomb9 = 1;
            break;
        default:
            break;
        }
    }
}

void CDlgFireManageEdit::ClearMarkSerious()
{
    QLineSeries *p;
    for (auto itr : m_pQLineSeriesVec)
    {
        m_chart->removeSeries(itr);
        p = itr;
        if (p != nullptr)
        {
            delete p;
        }
    }

    m_pQLineSeriesVec.clear();
}

double CDlgFireManageEdit::Get80mmFireRange(double dLon, double dLat, double dAlt)
{
    std::list<qnzkna::ly_ccip::ly_ccip_80mm::ccipdata_80mm> ccipdata_80mmlist;
    qnzkna::ly_ccip::ly_ccip_80mm::ccip_80mm_max_dist(dLon, dLat, dAlt, 0, dAlt, 0, ccipdata_80mmlist);
    auto itr = ccipdata_80mmlist.rbegin();
    return itr->x;
}

void CDlgFireManageEdit::on_comboBox_3_currentIndexChanged(int index)
{

}

void CDlgFireManageEdit::on_comboBox_4_currentIndexChanged(int index)
{

}

void CDlgFireManageEdit::on_comboBox_5_currentIndexChanged(int index)
{

}

void CDlgFireManageEdit::on_pushButton_3_clicked()
{
    qnzkna::NebulaMissionManage::NebulaMissionAutoCommandManage::CodeSetLoad_Parameter_CodeID ePara = CodeSetLoad_Parameter_CodeID_DY_CommonCommand_Unlock_Insure;
    QString strCodeName = qnzkna::NebulaMissionManage::NebulaMissionAutoCommandManage::CodeSetLoad_Parameter_CodeID_To_String(ePara);
    QVariant qv = ui->comboBox_3->itemData(ui->comboBox_3->currentIndex());
    UpdateListDataState(strCodeName, ePara, qv.toInt(), 1);
}

void CDlgFireManageEdit::on_pushButton_5_clicked()
{
    qnzkna::NebulaMissionManage::NebulaMissionAutoCommandManage::CodeSetLoad_Parameter_CodeID ePara = CodeSetLoad_Parameter_CodeID_DY_CommonCommand_Shot_Percussion;
    QString strCodeName = qnzkna::NebulaMissionManage::NebulaMissionAutoCommandManage::CodeSetLoad_Parameter_CodeID_To_String(ePara);
    QVariant qv = ui->comboBox_3->itemData(ui->comboBox_4->currentIndex());
    UpdateListDataState(strCodeName, ePara, qv.toInt(), 1);
}

void CDlgFireManageEdit::on_pushButton_7_clicked()
{
    qnzkna::NebulaMissionManage::NebulaMissionAutoCommandManage::CodeSetLoad_Parameter_CodeID ePara = CodeSetLoad_Parameter_CodeID_DY_CommonCommand_Lock_Reset;
    QString strCodeName = qnzkna::NebulaMissionManage::NebulaMissionAutoCommandManage::CodeSetLoad_Parameter_CodeID_To_String(ePara);
    QVariant qv = ui->comboBox_3->itemData(ui->comboBox_5->currentIndex());
    UpdateListDataState(strCodeName, ePara, qv.toInt(), 1);
}

void CDlgFireManageEdit::on_pushButton_4_clicked()
{
    int nCul = ui->listWidget->currentRow();
    if (nCul != -1)
    {
        ui->listWidget->takeItem(nCul);
    }
}

void CDlgFireManageEdit::on_listWidget_currentTextChanged(const QString &currentText)
{
    ui->label_7->setText(currentText);
}
