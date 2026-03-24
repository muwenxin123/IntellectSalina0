#include "CDlgEditLine.h"
#include "ui_CDlgEditLine.h"
#include "QCString.h"
#include "ITaskManagerActivator.h"
#include "LyMessageBox.h"
#include "CDlgEditGroupp.h"
#include "lY_TaskManage.h"
#include <QSettings>

#define INIT_HEIGHT						150

#define UPLOAD_START					0
#define UPLOAD_END						1
#define UPLOAD_WAIT						2

#define DOWN_TIME_OUT					5

CDlgEditLine::CDlgEditLine(QWidget *parent) :
    LyDialog(tr("Route Upload"), parent),
    ui(new Ui::CDlgEditLine)
{
    ui->setupUi(centralWidget());
    this->setResizable(true);
    this->resize(1400, 800);

    int colorIndex = 0;
    m_nColor[0] = 0xffff00;
    m_nColor[colorIndex++] = 0xffff00;
    m_nColor[colorIndex++] = 0xc0c0c0;
    m_nColor[colorIndex++] = 0x808080;
    m_nColor[colorIndex++] = 0x000000;

    m_nColor[colorIndex++] = 0x800000;
    m_nColor[colorIndex++] = 0x808000;
    m_nColor[colorIndex++] = 0x00ff00;
    m_nColor[colorIndex++] = 0x008000;
    m_nColor[colorIndex++] = 0x00ffff;
    m_nColor[colorIndex++] = 0x008080;
    m_nColor[colorIndex++] = 0x0000ff;
    m_nColor[colorIndex++] = 0x000080;
    m_nColor[colorIndex++] = 0xff00ff;
    m_nColor[colorIndex++] = 0x800080;
    m_nColor[colorIndex++] = 0xffa500;
    m_nColor[colorIndex++] = 0x404040;
    m_nColor[colorIndex++] = 0xc000c0;
    m_nColor[colorIndex++] = 0x00ffff;
    m_nColor[colorIndex++] = 0xaa5500;
    m_nColor[colorIndex] = 0xffffff;
    m_nColorNumber = colorIndex;

    connect(this, &CDlgEditLine::UpdateUAVParameterInfoSig, this, &CDlgEditLine::UpdateUAVParameterInfo);
    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(on_pushButton_clicked()));
    connect(ui->pushButton_7, SIGNAL(clicked()), this, SLOT(on_pushButton_7_clicked()));
    connect(ui->pushButton_2, SIGNAL(clicked()), this, SLOT(on_pushButton_2_clicked()));
    connect(ui->pushButton_3, SIGNAL(clicked()), this, SLOT(on_pushButton_3_clicked()));
    connect(ui->pushButton_4, SIGNAL(clicked()), this, SLOT(on_pushButton_4_clicked()));
    connect(ui->pushButton_5, SIGNAL(clicked()), this, SLOT(on_pushButton_5_clicked()));
    connect(ui->pushButton_6, SIGNAL(clicked()), this, SLOT(on_pushButton_6_clicked()));
    connect(ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(on_tabWidget_currentChanged(int)));
    connect(ui->radioButton, SIGNAL(clicked()), this, SLOT(on_radioButton_clicked()));
    connect(ui->radioButton_2, SIGNAL(clicked()), this, SLOT(on_radioButton_2_clicked()));
    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(on_comboBox_currentIndexChanged(int)));
    connect(ui->comboBox_3, SIGNAL(currentIndexChanged(QString)), this, SLOT(on_comboBox_3_currentIndexChanged(QString)));
    connect(ui->pushButton_9, SIGNAL(clicked()), this, SLOT(on_pushButton_9_clicked()));

    connect(ui->pushButton_8, SIGNAL(clicked()), this, SLOT(on_pushButton_8_clicked()));
    connect(ui->pushButton_10, SIGNAL(clicked()), this, SLOT(on_pushButton_10_clicked()));
    connect(ui->pushButton_11, SIGNAL(clicked()), this, SLOT(on_pushButton_11_clicked()));

    QString serviceTipsInfo("");
    m_pITaskManageService = ITaskManagerActivator::Get().getPTaskManageService();
    if (m_pITaskManageService == nullptr)
    {
        serviceTipsInfo += ("TaskManageService is null!\n");
    }

    m_pIWeaponManageService = ITaskManagerActivator::Get().getPWeaponManageService();
    if (m_pIWeaponManageService == nullptr)
    {
        serviceTipsInfo += ("WeaponManageService is null!\n");
    }

    m_pIConfigProjectControlService = ITaskManagerActivator::Get().getPIConfigProjectControlService();
    if (m_pIConfigProjectControlService == nullptr)
    {
        serviceTipsInfo += ("ConfigProjectControlService is null!\n");
    }

    m_pIMarkManageService = ITaskManagerActivator::Get().getPIMarkManageService();
    if (m_pIMarkManageService == nullptr)
    {
        serviceTipsInfo += ("MarkManageService is null!\n");
    }

    if (!serviceTipsInfo.isEmpty())
    {
        ui->label_5->setText(serviceTipsInfo);
        return;
    }

    ui->tabWidget->clear();
    ui->label_2->setText("");
    ui->label_3->setText("");
    ui->label_5->setText("");
    ui->label_7->setText("");
    ui->label_9->setText("");
    ui->label_11->setText("");

    ui->progressBar->setRange(0, 100);
    ui->progressBar->setValue(0);
    ui->progressBar->hide();

    ui->label_6->hide();
    ui->label_7->hide();
    ui->label_8->hide();
    ui->label_9->hide();
    ui->label_10->hide();
    ui->label_11->hide();

    InitWndData();
    localWeaponNameBindToId();

}

CDlgEditLine::~CDlgEditLine()
{
    delete ui;
}

void CDlgEditLine::changeEvent(QEvent *event)
{
    LyDialog::changeEvent(event);
    switch (event->type())
    {
    case QEvent::LanguageChange:
    {
        ui->retranslateUi(this);
        ui->label_2->setText("");
        ui->label_3->setText("");
        ui->label_5->setText("");
        ui->label_7->setText("");
        ui->label_9->setText("");
        ui->label_11->setText("");
        ui->label_14->setText("");
    }
    break;
    default:
        break;
    }
}

void CDlgEditLine::InitWndData()
{

    m_Ctrl_Table_Tabnum = - 1;
    m_strProtocolType = "";

    m_Last_Tab_No = -1;
    m_bLiseEditStates = PickNormal;
    m_Hitrow = 0;
    m_Hitcol = 0;
    m_StrEditForList = "";
    m_UpdatePointInfoList.clear();

    m_TaskId = -1;
    m_GroupID = -1;

    ui->radioButton->setChecked(true);
    ui->radioButton_2->setChecked(false);

    ui->comboBox_2->insertItem(0, tr("Independent"));
    ui->comboBox_2->insertItem(1, tr("HeightFirst"));
    ui->comboBox_2->insertItem(2, tr("Incline"));

    ui->label_3->setText("--");
    ui->label_14->setText("--");

    CreateTableMenu();

    m_RecvXYUAVParameterInfo = false;

    m_UpLoadTimer = new QTimer(this);
    m_UpLoadTimer->setInterval(2000);
    connect(m_UpLoadTimer, &QTimer::timeout, this, &CDlgEditLine::on_timeOut);

    m_DownLoadTimer = new QTimer(this);
    m_DownLoadTimer->setInterval(1000);
    connect(m_DownLoadTimer, &QTimer::timeout, this, &CDlgEditLine::on_DowntimeOut);

    m_UpLoadTimer54 = new QTimer(this);
    m_UpLoadTimer54->setInterval(2000);
    connect(m_UpLoadTimer54, &QTimer::timeout, this, &CDlgEditLine::on_timeOut54);

    m_DownLoadTimer54 = new QTimer(this);
    m_DownLoadTimer54->setInterval(1000);
    connect(m_DownLoadTimer54, &QTimer::timeout, this, &CDlgEditLine::on_DowntimeOut54);

    m_DownLoadMaxCount     = 0;
    m_DownLoadTimeCount    = 0;
    m_DownLoadTryTimeCount = 0;

    m_DownLoadMaxCount54     = 0;
    m_DownLoadTimeCount54    = 0;
    m_DownLoadTryTimeCount54 = 0;
}

WndRouteLineInfoList CDlgEditLine::GetWndRoutlineInfoList()
{
    return m_stWndRouteLineInfoList.m_vecRoutePointInfo;
}

const std::string CDlgEditLine::GetCurWeaponComponentProtocolType(int Culsel)
{
    WndRouteLineInfoList::iterator routeLineInfolist = m_stWndRouteLineInfoList.m_vecRoutePointInfo.begin();
    for (; routeLineInfolist != m_stWndRouteLineInfoList.m_vecRoutePointInfo.end(); routeLineInfolist++)
    {
        if ((*routeLineInfolist)->nID == Culsel)
        {
            break;
        }
    }
    if (routeLineInfolist == m_stWndRouteLineInfoList.m_vecRoutePointInfo.end())
    {
        return "";
    }

    std::string strMarkID = (*routeLineInfolist)->strWeaponID;
    qnzkna::WeaponManage::WeaponComponent pWeaponModel;
    if (!m_pIWeaponManageService->GetWeaponComponent(&pWeaponModel, strMarkID))
    {
        return "";
    }

    return pWeaponModel.getProtocolType();
}

void CDlgEditLine::UpdateTrackInfoToListControl(int Culsel)
{
    m_stWndRouteLineInfoList.lock();
    WndRouteLineInfoList::iterator iter_list = m_stWndRouteLineInfoList.m_vecRoutePointInfo.begin();
    for (; iter_list != m_stWndRouteLineInfoList.m_vecRoutePointInfo.end(); iter_list++)
    {
        if ((*iter_list)->nID == Culsel)
        {

            UpdateXYTrackInfoToListControl_54(Culsel, &iter_list);

            LY_TaskManage::Get().Draw3DLinesWeaponID((*iter_list)->strWeaponID);
        }
    }
    m_stWndRouteLineInfoList.unlock();
}

void CDlgEditLine::UpdateXYTrackInfoToListControl(int Culsel, WndRouteLineInfoList::iterator *iter_list)
{

    int nCount = ui->tabWidget->count();
    if (Culsel > (nCount + 1))
    {
        return;
    }

    QWidget *p = ui->tabWidget->widget(Culsel - 1);
    if (p->layout()->count() <= 0)
    {
        return;
    }

    QLayoutItem *it = p->layout()->itemAt(0);
    if (it == nullptr)
    {
        return;
    }

    QTableWidget *pTabCtrl = qobject_cast<QTableWidget *>(it->widget());
    if (pTabCtrl == nullptr)
    {
        return;
    }

    bool enableComboBox = false;
    if (pTabCtrl->rowCount() > 0)
    {
        QComboBox *comboBoxTemp = qobject_cast<QComboBox *>(pTabCtrl->cellWidget(0, 8));
        if (comboBoxTemp != nullptr)
        {
            enableComboBox = comboBoxTemp->isEnabled();
        }
    }

    pTabCtrl->clearContents();
    pTabCtrl->setRowCount((*(*iter_list))->vecNebulaRoutePoint.size());
    pTabCtrl->disconnect(SIGNAL(cellChanged(int, int)));

    pTabCtrl->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    pTabCtrl->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    pTabCtrl->setEnabled(false);

    int m_No = 0;
    int tableWidgetRows = 0;
    double dOldLon = 0, dOldLat = 0;
    std::vector<WndRoutePoint>::iterator itr = (*(*iter_list))->vecNebulaRoutePoint.begin();
    for (; itr != (*(*iter_list))->vecNebulaRoutePoint.end(); itr++)
    {
        m_No = itr->nPointID - 1;
        if (tableWidgetRows == m_No)
        {
            double m_localdistance(0);

            double m_dLongitude = itr->stNebulaRoutePoint.GetLongitude();
            double m_dLatitude = itr->stNebulaRoutePoint.GetLatitude();
            int m_height = itr->stNebulaRoutePoint.GetHeight();
            int m_eHeightCtrl = itr->stNebulaRoutePoint.GetHeightCtrl();

            if (itr == (*(*iter_list))->vecNebulaRoutePoint.begin())
            {
                m_localdistance = 0;
                dOldLat = m_dLatitude;
                dOldLon = m_dLongitude;
            }
            else
            {
                m_localdistance = get_distance_from_lat_lon(dOldLat, dOldLon, m_dLatitude, m_dLongitude);
                dOldLat = m_dLatitude;
                dOldLon = m_dLongitude;
            }

            int m_eLandFlag = itr->stNebulaRoutePoint.GetLandFlag();
            int m_eTaskFlag = itr->stNebulaRoutePoint.GetTaskFlag();
            int m_nSpeed = itr->stNebulaRoutePoint.GetSpeed();
            int m_nHoverTime = itr->stNebulaRoutePoint.GetHoverTime();
            int nInvestigationSetting = itr->mInvestigationSetting;
            int nFirepowerSetting = itr->mFirepowerSetting;

            QStringList list08, list10, list11;
            list08 << tr("Independent") << tr("HeightFirst") << tr("Incline") << tr("Keep");
            list10 << tr("Lock") << tr("Scan") << tr("Follow") << tr("Ray");
            list11 << tr("Lock") << tr("Unlock") << tr("Shoot");

            QComboBox	*combobox08 = new QComboBox();
            QComboBox	*combobox10 = new QComboBox();
            QComboBox	*combobox11 = new QComboBox();
            combobox08->addItems(list08);
            combobox10->addItems(list10);
            combobox11->addItems(list11);

            QString No, Longitude, Latitude, height, eHeightCtrl, localdistance, LandFlag, TaskFlag, Speed, HoverTime, strInvestigationSetting, strFirepowerSetting;
            No = QString::number(m_No + 1);
            Longitude = QString::number(m_dLongitude, 'f', 7);
            Latitude = QString::number(m_dLatitude, 'f', 7);
            height = QString::number(m_height);
            switch (m_eHeightCtrl)
            {
            case CNebulaRoutePoint::EHeightCtrl::EHeightCtrl_Independent:
                eHeightCtrl = tr("Independent");
                combobox08->setCurrentIndex(0);
                break;
            case CNebulaRoutePoint::EHeightCtrl::EHeightCtrl_HightPreferred:
                eHeightCtrl = tr("HeightFirst");
                combobox08->setCurrentIndex(1);
                break;
            case CNebulaRoutePoint::EHeightCtrl::EHeightCtrl_SlashCtrl:
                eHeightCtrl = tr("Incline");
                combobox08->setCurrentIndex(2);
                break;
            case CNebulaRoutePoint::EHeightCtrl::EHeightCtrl_Keep:
                eHeightCtrl = tr("Keep");
                combobox08->setCurrentIndex(3);
                break;
            default:
                break;
            }
            localdistance = QString::number(m_localdistance, 'f', 3);
            switch (m_eLandFlag)
            {
            case CNebulaRoutePoint::ELandFlag::ELandFlag_Normal:
                LandFlag = tr("Normal");
                break;
            case CNebulaRoutePoint::ELandFlag::ELandFlag_Land:
                LandFlag = tr("Land");
                break;
            default:
                break;
            }
            switch (m_eTaskFlag)
            {
            case CNebulaRoutePoint::ETaskFlag::ETaskFlag_Normal:
                TaskFlag = tr("Normal");
                break;
			case CNebulaRoutePoint::ETaskFlag::ETaskFlag_TakePicture:
				TaskFlag = tr("Photo");
                break;
            default:
                break;
            }
            switch (nInvestigationSetting)
            {
            case InvestigationSetting::Investigat_Lock:
                strInvestigationSetting = tr("Lock");
                combobox10->setCurrentIndex(0);
                break;
            case InvestigationSetting::Investigat_Scan:
                strInvestigationSetting = tr("Scan");
                combobox10->setCurrentIndex(1);
                break;
            case InvestigationSetting::Investigat_Follow:
                strInvestigationSetting = tr("Follow");
                combobox10->setCurrentIndex(2);
                break;
            case InvestigationSetting::Investigat_Infrared:
                strInvestigationSetting = tr("Infrared");
                combobox10->setCurrentIndex(3);
                break;
            default:
                break;
            }
            switch (nFirepowerSetting)
            {
            case FirepowerSetting::Firepower_Lock:
                strFirepowerSetting = tr("Lock");
                combobox11->setCurrentIndex(0);
                break;
            case FirepowerSetting::Firepower_Unlock:
                strFirepowerSetting = tr("Unlock");
                combobox11->setCurrentIndex(1);
                break;
            case FirepowerSetting::Firepower_Fire:
                strFirepowerSetting = tr("Shoot");
                combobox11->setCurrentIndex(2);
                break;
            default:
                break;
            }

            Speed = QString::number(m_nSpeed);
            HoverTime = QString::number(m_nHoverTime);

            QTableWidgetItem *item0 = new QTableWidgetItem(No);
            QTableWidgetItem *item1 = new QTableWidgetItem(Longitude);
            QTableWidgetItem *item2 = new QTableWidgetItem(Latitude);
            QTableWidgetItem *item3 = new QTableWidgetItem(height);
            QTableWidgetItem *item4 = new QTableWidgetItem(localdistance);
            QTableWidgetItem *item5 = new QTableWidgetItem(LandFlag);
            QTableWidgetItem *item6 = new QTableWidgetItem(Speed);
            QTableWidgetItem *item7 = new QTableWidgetItem(HoverTime);

            QTableWidgetItem *item9 = new QTableWidgetItem(TaskFlag);

            pTabCtrl->setItem(tableWidgetRows, 0, item0);
            pTabCtrl->setItem(tableWidgetRows, 1, item1);
            pTabCtrl->setItem(tableWidgetRows, 2, item2);
            pTabCtrl->setItem(tableWidgetRows, 3, item3);
            pTabCtrl->setItem(tableWidgetRows, 4, item4);
            pTabCtrl->setItem(tableWidgetRows, 5, item5);
            pTabCtrl->setItem(tableWidgetRows, 6, item6);
            pTabCtrl->setItem(tableWidgetRows, 7, item7);

            pTabCtrl->setCellWidget(tableWidgetRows, 8, combobox08);

            pTabCtrl->setItem(tableWidgetRows, 9, item9);
            pTabCtrl->setCellWidget(tableWidgetRows, 10, combobox10);
            pTabCtrl->setCellWidget(tableWidgetRows, 11, combobox11);

            pTabCtrl->item(tableWidgetRows, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            pTabCtrl->item(tableWidgetRows, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            pTabCtrl->item(tableWidgetRows, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            pTabCtrl->item(tableWidgetRows, 3)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            pTabCtrl->item(tableWidgetRows, 4)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            pTabCtrl->item(tableWidgetRows, 5)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            pTabCtrl->item(tableWidgetRows, 6)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            pTabCtrl->item(tableWidgetRows, 7)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

            pTabCtrl->item(tableWidgetRows, 9)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

            connect(combobox08, SIGNAL(currentIndexChanged(int)), this, SLOT(on_combobox_08_currentIndexChanged(int)));
            connect(combobox10, SIGNAL(currentIndexChanged(int)), this, SLOT(on_combobox_10_currentIndexChanged(int)));
            connect(combobox11, SIGNAL(currentIndexChanged(int)), this, SLOT(on_combobox_11_currentIndexChanged(int)));

            combobox08->setEnabled(enableComboBox);
            combobox10->setEnabled(enableComboBox);
            combobox11->setEnabled(enableComboBox);
            item0->setFlags(Qt::NoItemFlags);
        }

        tableWidgetRows++;
    }

    pTabCtrl->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    pTabCtrl->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    pTabCtrl->setEnabled(true);

    connect(pTabCtrl, SIGNAL(cellChanged(int, int)), this, SLOT(on_tableWidget_cellChanged(int, int)));

}

void CDlgEditLine::UpdateXYTrackInfoToListControl_54(int Culsel, WndRouteLineInfoList::iterator *iter_list)
{
    int nCount = ui->tabWidget->count();
    if (Culsel > (nCount + 1))
    {
        return;
    }

    QWidget *p = ui->tabWidget->widget(Culsel - 1);
    if (p->layout()->count() <= 0)
    {
        return;
    }

    QLayoutItem *it = p->layout()->itemAt(0);
    QTableWidget *pTabCtrl = qobject_cast<QTableWidget *>(it->widget());

    bool enableComboBox = false;
    if (pTabCtrl->rowCount() > 0)
    {
        QComboBox *comboBoxTemp = qobject_cast<QComboBox *>(pTabCtrl->cellWidget(0, 8));
        if (comboBoxTemp != nullptr)
        {
            enableComboBox = comboBoxTemp->isEnabled();
        }
    }

    pTabCtrl->clearContents();
    pTabCtrl->setRowCount((*(*iter_list))->vecNebulaRoutePoint.size());
    pTabCtrl->disconnect(SIGNAL(cellChanged(int, int)));

    const QStringList list08 = { tr("Independent"), tr("HightPreferred"), tr("SlashCtrl"), tr("Keep") };
    const QStringList list10 = { tr("Whole"), tr("Single") };
    const QStringList list11 = { tr("Precipice"), tr("Tangent") };

    pTabCtrl->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    pTabCtrl->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    pTabCtrl->setEnabled(false);

    int m_No = 0;
    int tableWidgetRows = 0;
    double dOldLon = 0, dOldLat = 0;
    std::vector<WndRoutePoint>::iterator itr = (*(*iter_list))->vecNebulaRoutePoint.begin();
    for (; itr != (*(*iter_list))->vecNebulaRoutePoint.end(); itr++)
    {
        m_No = itr->nPointID - 1;
        if (tableWidgetRows == m_No)
        {
            double m_localdistance(0);
            double m_dLongitude = itr->stNebulaRoutePoint.GetLongitude();
            double m_dLatitude = itr->stNebulaRoutePoint.GetLatitude();
            int m_height = itr->stNebulaRoutePoint.GetHeight();
            int m_eHeightCtrl = itr->stNebulaRoutePoint.GetHeightCtrl();

            if (itr == (*(*iter_list))->vecNebulaRoutePoint.begin())
            {
                m_localdistance = 0;
                dOldLat = m_dLatitude;
                dOldLon = m_dLongitude;
            }
            else
            {
                m_localdistance = get_distance_from_lat_lon(dOldLat, dOldLon, m_dLatitude, m_dLongitude);
                dOldLat = m_dLatitude;
                dOldLon = m_dLongitude;
            }

            int m_eLandFlag = itr->stNebulaRoutePoint.GetLandFlag();
            int m_eTaskFlag = itr->stNebulaRoutePoint.GetTaskFlag();
            int m_nSpeed = itr->stNebulaRoutePoint.GetSpeed();
            int m_nHoverTime = itr->stNebulaRoutePoint.GetHoverTime();
            int m_nRadius = itr->stNebulaRoutePoint.GetRadius();
            int nLineProPerTy = itr->stNebulaRoutePoint.GetLineProPerTy();
            int nTurnMode = itr->stNebulaRoutePoint.GetTurnMode();

            QComboBox	*combobox08 = new QComboBox();
            QComboBox	*combobox10 = new QComboBox();
            QComboBox	*combobox11 = new QComboBox();
            combobox08->addItems(list08);
            combobox10->addItems(list10);
            combobox11->addItems(list11);

            QString No, Longitude, Latitude, height, localdistance, LandFlag, TaskFlag, Speed, HoverTime, strRadius;
            No = QString::number(m_No + 1);
            Longitude = QString::number(m_dLongitude, 'f', 7);
            Latitude = QString::number(m_dLatitude, 'f', 7);
            height = QString::number(m_height);
            switch (m_eHeightCtrl)
            {
            case CNebulaRoutePoint::EHeightCtrl::EHeightCtrl_Independent:
                combobox08->setCurrentIndex(0);
                break;
            case CNebulaRoutePoint::EHeightCtrl::EHeightCtrl_HightPreferred:
                combobox08->setCurrentIndex(1);
                break;
            case CNebulaRoutePoint::EHeightCtrl::EHeightCtrl_SlashCtrl:
                combobox08->setCurrentIndex(2);
                break;
            case CNebulaRoutePoint::EHeightCtrl::EHeightCtrl_Keep:
                combobox08->setCurrentIndex(3);
                break;
            default:
                break;
            }
            localdistance = QString::number(m_localdistance, 'f', 3);
            switch (m_eLandFlag)
            {
            case CNebulaRoutePoint::ELandFlag::ELandFlag_Normal:
                LandFlag = tr("Normal");
                break;
            case CNebulaRoutePoint::ELandFlag::ELandFlag_Land:
                LandFlag = tr("Land");
                break;
            default:
                break;
            }
            switch (m_eTaskFlag)
            {
            case CNebulaRoutePoint::ETaskFlag::ETaskFlag_Normal:
                TaskFlag = tr("Normal");
                break;
            case CNebulaRoutePoint::ETaskFlag::ETaskFlag_TakePicture:
                TaskFlag = tr("Photo");
                break;
            default:
                break;
            }
            switch (nLineProPerTy)
            {
            case CNebulaRoutePoint::ELineProPerTy::ELineProPerTy_Whole:
                combobox10->setCurrentIndex(0);
                break;
            case CNebulaRoutePoint::ELineProPerTy::ELineProPerTy_Alone:
                combobox10->setCurrentIndex(1);
                break;
            default:
                break;
            }
            switch (nTurnMode)
            {
            case CNebulaRoutePoint::ETurnMode::ETurnMode_Precipice:
                combobox11->setCurrentIndex(0);
                break;
            case CNebulaRoutePoint::ETurnMode::ETaskFlag_Tangent:
                combobox11->setCurrentIndex(1);
                break;
            default:
                break;
            }

            Speed = QString::number(m_nSpeed);
            HoverTime = QString::number(m_nHoverTime);
            strRadius = QString::number(m_nRadius);

            QTableWidgetItem *item0 = new QTableWidgetItem(No);
            QTableWidgetItem *item1 = new QTableWidgetItem(Longitude);
            QTableWidgetItem *item2 = new QTableWidgetItem(Latitude);
            QTableWidgetItem *item3 = new QTableWidgetItem(height);
            QTableWidgetItem *item4 = new QTableWidgetItem(localdistance);
            QTableWidgetItem *item5 = new QTableWidgetItem(LandFlag);
            QTableWidgetItem *item6 = new QTableWidgetItem(Speed);
            QTableWidgetItem *item7 = new QTableWidgetItem(HoverTime);

            QTableWidgetItem *item9 = new QTableWidgetItem(TaskFlag);
            QTableWidgetItem *item10 = new QTableWidgetItem(strRadius);

            pTabCtrl->setItem(tableWidgetRows, 0, item0);
            pTabCtrl->setItem(tableWidgetRows, 1, item1);
            pTabCtrl->setItem(tableWidgetRows, 2, item2);
            pTabCtrl->setItem(tableWidgetRows, 3, item3);
            pTabCtrl->setItem(tableWidgetRows, 4, item4);
            pTabCtrl->setItem(tableWidgetRows, 5, item5);
            pTabCtrl->setItem(tableWidgetRows, 6, item6);
            pTabCtrl->setItem(tableWidgetRows, 7, item7);

            pTabCtrl->setCellWidget(tableWidgetRows, 8, combobox08);

            pTabCtrl->setItem(tableWidgetRows, 9, item9);
            pTabCtrl->setItem(tableWidgetRows, 10, item10);
            pTabCtrl->setCellWidget(tableWidgetRows, 11, combobox10);
            pTabCtrl->setCellWidget(tableWidgetRows, 12, combobox11);

            pTabCtrl->item(tableWidgetRows, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            pTabCtrl->item(tableWidgetRows, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            pTabCtrl->item(tableWidgetRows, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            pTabCtrl->item(tableWidgetRows, 3)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            pTabCtrl->item(tableWidgetRows, 4)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            pTabCtrl->item(tableWidgetRows, 5)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            pTabCtrl->item(tableWidgetRows, 6)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            pTabCtrl->item(tableWidgetRows, 7)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

            pTabCtrl->item(tableWidgetRows, 9)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            pTabCtrl->item(tableWidgetRows, 10)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

            connect(combobox08, SIGNAL(currentIndexChanged(int)), this, SLOT(on_combobox_08_currentIndexChanged(int)));
            connect(combobox10, SIGNAL(currentIndexChanged(int)), this, SLOT(on_combobox_10_currentIndexChanged(int)));
            connect(combobox11, SIGNAL(currentIndexChanged(int)), this, SLOT(on_combobox_11_currentIndexChanged(int)));

            combobox08->setEnabled(enableComboBox);
            combobox10->setEnabled(enableComboBox);
            combobox11->setEnabled(enableComboBox);
            item0->setFlags(Qt::NoItemFlags);
        }

        tableWidgetRows++;
    }

    pTabCtrl->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    pTabCtrl->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    pTabCtrl->setEnabled(true);

    connect(pTabCtrl, SIGNAL(cellChanged(int, int)), this, SLOT(on_tableWidget_cellChanged(int, int)));
}

void CDlgEditLine::UpdateMavTrackInfoToListControl(WndRouteLineInfoList::iterator *iter_list)
{

}

void CDlgEditLine::UpdateTrackInfoToListHeading(int Culsel)
{
    if (m_Ctrl_Table_Tabnum == Culsel)
    {
        return;
    }
    m_Ctrl_Table_Tabnum = Culsel;

    int nCount = ui->tabWidget->count();
    if (Culsel > (nCount + 1))
    {
        return;
    }

    QWidget *p = ui->tabWidget->widget(Culsel - 1);
    if (p->layout()->count() > 0)
    {
        QLayoutItem *it = p->layout()->itemAt(0);
        QTableWidget *pTabCtrl = qobject_cast<QTableWidget *>(it->widget());
        QStringList headerlabels;
        int nListCount = 12;

        std::string strProtocolType = GetCurWeaponComponentProtocolType(Culsel);
        if (strProtocolType == "")
        {
            return;
        }

        m_strProtocolType = strProtocolType;

        if (strProtocolType == WEAPONSYSTEM_VEHICLE_PROTOCOLTYPE_XY)
        {
            headerlabels << tr("No.") << tr("Lon") << tr("Lat") << tr("Height") << tr("Dis") << tr("Land") << tr("Speed") << tr("HoverTime") << tr("HeightControl") << tr("States") << tr("Detect") << tr("Fire");
        }
        if (strProtocolType == WEAPONSYSTEM_VEHICLE_PROTOCOLTYPE_XY_54)
        {
            headerlabels << tr("No.") << tr("Lon") << tr("Lat") << tr("Height") << tr("Dis") << tr("Land") << tr("Speed") << tr("HoverTime") << tr("HeightControl") << tr("States") << tr("Radius") << tr("Property") << tr("Turn");
            nListCount = 13;
        }
        else if (strProtocolType == WEAPONSYSTEM_VEHICLE_PROTOCOLTYPE_MAVLINK)
        {
            headerlabels << tr("No.") << tr("Lon") << tr("Lat") << tr("Height") << tr("Dis") << tr("MAV_CMD") << tr("Param1") << tr("Param2") << tr("Param3") << tr("Param4") << tr("Detect") << tr("Fire") << tr("MAV_FRAME");
            nListCount = 13;
        }
        else if (strProtocolType == WEAPONSYSTEM_VEHICLE_PROTOCOLTYPE_DJI)
        {
            headerlabels << tr("No.") << tr("Lon") << tr("Lat") << tr("Height") << tr("Dis") << tr("Land") << tr("Speed") << tr("HoverTime") << tr("HeightControl") << tr("Point") << tr("Detect") << tr("Fire");
        }
        else
        {
        }

        pTabCtrl->setColumnCount(nListCount);
        pTabCtrl->setHorizontalHeaderLabels(headerlabels);
        for (int i = 0; i < nListCount; i++)
        {
            if (i == 1 || i == 2 || i == 4)
            {
                pTabCtrl->horizontalHeader()->setSectionResizeMode(i, QHeaderView::ResizeToContents);
            }
            else
            {
                pTabCtrl->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Stretch);
            }
        }

        pTabCtrl->verticalHeader()->setHidden(true);
        pTabCtrl->setSelectionBehavior(QTableWidget::SelectRows);
        pTabCtrl->setSelectionMode(QAbstractItemView::SingleSelection);
        pTabCtrl->setEditTriggers(QAbstractItemView::NoEditTriggers);
        pTabCtrl->setAlternatingRowColors(true);
        pTabCtrl->setContextMenuPolicy(Qt::CustomContextMenu);
    }
}

void CDlgEditLine::ClearWndRouteLineInfoList()
{
    WndRouteLineInfoList::iterator iter_list = m_stWndRouteLineInfoList.m_vecRoutePointInfo.begin();
    for (; iter_list != m_stWndRouteLineInfoList.m_vecRoutePointInfo.end(); iter_list++)
    {
        WndRouteLineInfo *pListInfo = *iter_list;
        if (pListInfo)
        {
            delete pListInfo;
            pListInfo = NULL;
        }
    }
    m_stWndRouteLineInfoList.m_vecRoutePointInfo.clear();
}

void CDlgEditLine::SendBagData(int nPointID, std::string &strWeaponName, std::string &strWeaponID, char *pBuf, int nBufLen)
{

    const qnzkna::SystemConfig::SystemSeatInfo &systemSeatInfo = m_pIConfigProjectControlService->systemSeatInfo();
    qnzkna::NodeManage::NODE_TYPE_UINT nNodeType = systemSeatInfo.SystemNodeType();
    std::string	strNodeId = systemSeatInfo.SystemNodeId();
    std::string strMarkID = strWeaponID;

    std::string strChId;
    if (!m_CXYWeaponManage.GetSendChannel(strMarkID, strChId))
    {
        return;
    }

    qnzkna::framework::MessageReferenceProps  props;
    props.put("SourceType", std::to_string(nNodeType));
    props.put("SourceID", strNodeId);
    props.put("TargetType", std::to_string(NODE_TYPE_WEAPONCOMPONENT));
    props.put("TargetID", strChId);
    props.put("WeaponID", strMarkID);
    props.put("ProtocolType", WEAPONSYSTEM_VEHICLE_PROTOCOLTYPE_XY);
    ITaskManagerActivator::Get().getIRegistryImpl()->messageNotify(new qnzkna::framework::MessageReference("MsgUAVFlyControlSend", pBuf, nBufLen, props));

    if (ui->label_5)
    {
        QString strTip = tr("%1 times upload to %2 point [%3]").arg(m_Upload_Send_No + 1).arg(QString::fromStdString(strWeaponName) + "(" + QString::fromStdString(strWeaponID) + ")").arg(nPointID);
        ui->label_5->setText(strTip);
    }

}

void CDlgEditLine::SendBagData54(int nPointID, std::string &strWeaponName, std::string &strWeaponID, char *pBuf, int nBufLen)
{
    const qnzkna::SystemConfig::SystemSeatInfo &systemSeatInfo = m_pIConfigProjectControlService->systemSeatInfo();
    qnzkna::NodeManage::NODE_TYPE_UINT nNodeType = systemSeatInfo.SystemNodeType();
    std::string	strNodeId = systemSeatInfo.SystemNodeId();
    std::string strMarkID = strWeaponID;

    std::string strChId;
    if (!m_CXYWeaponManage.GetSendChannel(strMarkID, strChId))
    {
        return;
    }

    qnzkna::framework::MessageReferenceProps  props;
    props.put("SourceType", std::to_string(nNodeType));
    props.put("SourceID", strNodeId);
    props.put("TargetType", std::to_string(NODE_TYPE_WEAPONCOMPONENT));
    props.put("TargetID", strChId);
    props.put("WeaponID", strMarkID);
    props.put("ProtocolType", WEAPONSYSTEM_VEHICLE_PROTOCOLTYPE_XY_54);
    ITaskManagerActivator::Get().getIRegistryImpl()->messageNotify(new qnzkna::framework::MessageReference("MsgUAVFlyControlSend", pBuf, nBufLen, props));

    if (ui->label_5)
    {
        QString strTip = tr("%1 times upload to %2 point [%3]").arg(m_Upload_Send_No + 1).arg(QString::fromStdString(strWeaponName) + "(" + QString::fromStdString(strWeaponID) + ")").arg(nPointID);
        ui->label_5->setText(strTip);
    }
}

void CDlgEditLine::ClearS_xygsJL_08()
{
    memset(&s_xygsJL_08, 0x00, sizeof(xygsJL_08));
    s_xygsJL_08.flags = 8;
}

void CDlgEditLine::ClearS_xygsJL_08_54()
{
    memset(&s_xygsJL_08_54, 0x00, sizeof(xygsJL_08));
    s_xygsJL_08_54.flags = 540708;
}

void CDlgEditLine::ClearS_xygsJL_09()
{
    memset(&s_xygsJL_09, 0x00, sizeof(xygsJL_09));
    s_xygsJL_09.flags = 9;
}

void CDlgEditLine::ClearS_xygsJL_09_54()
{
    memset(&s_xygsJL_09_54, 0x00, sizeof(xygsJL_V7_09));
    s_xygsJL_09_54.flags = 540709;

}

void CDlgEditLine::ClearS_xygsJL_17()
{
    memset(&s_xygsJL_17, 0x00, sizeof(xygsJL_17));
    s_xygsJL_17.flags = 17;
}

void CDlgEditLine::ClearS_xygsJL_14()
{
    memset(&s_xygsJL_14, 0x00, sizeof(xygsJL_14));
    s_xygsJL_14.flags = 14;
}

void CDlgEditLine::SetS_xygsJL_09(char *pBuf, int &nlen, int nPointID)
{
    ClearS_xygsJL_09();
    int num = ui->tabWidget->currentIndex();
    WndRouteLineInfoList::iterator iter_list = m_stWndRouteLineInfoList.m_vecRoutePointInfo.begin();
    if (num > m_stWndRouteLineInfoList.m_vecRoutePointInfo.size())
    {
        return;
    }
    iter_list += num;
    if (iter_list >= m_stWndRouteLineInfoList.m_vecRoutePointInfo.end())
    {
        return;
    }
    std::vector<WndRoutePoint>::iterator itr = (*iter_list)->vecNebulaRoutePoint.begin();
    itr += nPointID;
    if (itr >= (*iter_list)->vecNebulaRoutePoint.end())
    {
        return;
    }

    std::string strXYUAVID(""), strXYUAVGroupID(""), strChId("");

    s_xygsJL_09.groupIndex = strtol(strXYUAVGroupID.c_str(), NULL, 16);;
    s_xygsJL_09.planeIndex = strtol(strXYUAVID.c_str(), NULL, 16);
    s_xygsJL_09.param.temp1 = 0;
    s_xygsJL_09.param.position.lng = itr->stNebulaRoutePoint.GetLongitude();
    s_xygsJL_09.param.position.lat = itr->stNebulaRoutePoint.GetLatitude();
    s_xygsJL_09.param.position.heigth = itr->stNebulaRoutePoint.GetHeight();
    s_xygsJL_09.param.velocity = itr->stNebulaRoutePoint.GetSpeed();
    s_xygsJL_09.param.howerTime = itr->stNebulaRoutePoint.GetHoverTime();
    s_xygsJL_09.param.pointNumber = (*iter_list)->vecNebulaRoutePoint.size();
    m_Upload_PointNum = (*iter_list)->vecNebulaRoutePoint.size();
    s_xygsJL_09.param.pointSerialNo = nPointID + 1;

    int flag;
    unsigned char position;

    int m_eHeightCtrl = itr->stNebulaRoutePoint.GetHeightCtrl();
    int m_eLandFlag = itr->stNebulaRoutePoint.GetLandFlag();
    int m_eTaskFlag = itr->stNebulaRoutePoint.GetTaskFlag();
    switch (m_eHeightCtrl)
    {
    case CNebulaRoutePoint::EHeightCtrl::EHeightCtrl_Independent:
        flag = 0;
        position = 8;
        bit_set(&s_xygsJL_09.param.property, position, flag);
        flag = 0;
        position = 7;
        bit_set(&s_xygsJL_09.param.property, position, flag);
        break;
    case CNebulaRoutePoint::EHeightCtrl::EHeightCtrl_HightPreferred:
        flag = 1;
        position = 8;
        bit_set(&s_xygsJL_09.param.property, position, flag);
        flag = 0;
        position = 7;
        bit_set(&s_xygsJL_09.param.property, position, flag);
        break;
    case CNebulaRoutePoint::EHeightCtrl::EHeightCtrl_SlashCtrl:
        flag = 0;
        position = 8;
        bit_set(&s_xygsJL_09.param.property, position, flag);
        flag = 1;
        position = 7;
        bit_set(&s_xygsJL_09.param.property, position, flag);
        break;
    case CNebulaRoutePoint::EHeightCtrl::EHeightCtrl_Keep:
        flag = 1;
        position = 8;
        bit_set(&s_xygsJL_09.param.property, position, flag);
        flag = 1;
        position = 7;
        bit_set(&s_xygsJL_09.param.property, position, flag);
        break;
    default:
        break;
    }
    switch (m_eLandFlag)
    {
    case CNebulaRoutePoint::ELandFlag::ELandFlag_Normal:
        flag = 0;
        position = 2;
        bit_set(&s_xygsJL_09.param.property, position, flag);
        break;
    case CNebulaRoutePoint::ELandFlag::ELandFlag_Land:
        flag = 1;
        position = 2;
        bit_set(&s_xygsJL_09.param.property, position, flag);
    default:
        break;
    }
    switch (m_eTaskFlag)
    {
    case CNebulaRoutePoint::ETaskFlag::ETaskFlag_Normal:
        flag = 0;
        position = 1;
        bit_set(&s_xygsJL_09.param.property, position, flag);
        break;
    case CNebulaRoutePoint::ETaskFlag::ETaskFlag_TakePicture:
        flag = 1;
        position = 1;
        bit_set(&s_xygsJL_09.param.property, position, flag);
        break;
    default:
        break;
    }
    nlen = sizeof(s_xygsJL_09);
    memcpy(pBuf, &s_xygsJL_09, nlen);
    return;
}

void CDlgEditLine::SetS_xygsJL_09_54(char *pBuf, int &nlen, int nPointID)
{
    ClearS_xygsJL_09_54();
    int num = ui->tabWidget->currentIndex();
    WndRouteLineInfoList::iterator iter_list = m_stWndRouteLineInfoList.m_vecRoutePointInfo.begin();
    if (num > m_stWndRouteLineInfoList.m_vecRoutePointInfo.size())
    {
        return;
    }
    iter_list += num;
    if (iter_list >= m_stWndRouteLineInfoList.m_vecRoutePointInfo.end())
    {
        return;
    }
    std::vector<WndRoutePoint>::iterator itr = (*iter_list)->vecNebulaRoutePoint.begin();
    itr += nPointID;
    if (itr >= (*iter_list)->vecNebulaRoutePoint.end())
    {
        return;
    }

    std::string strXYUAVID, strXYUAVGroupID, strChId;

    s_xygsJL_09_54.groupIndex = strtol(strXYUAVGroupID.c_str(), NULL, 16);;
    s_xygsJL_09_54.planeIndex = strtol(strXYUAVID.c_str(), NULL, 16);
    s_xygsJL_09_54.param.group = 0;
    s_xygsJL_09_54.param.lng = itr->stNebulaRoutePoint.GetLongitude();
    s_xygsJL_09_54.param.lat = itr->stNebulaRoutePoint.GetLatitude();
    s_xygsJL_09_54.param.heigth = itr->stNebulaRoutePoint.GetHeight();
    s_xygsJL_09_54.param.speed = itr->stNebulaRoutePoint.GetSpeed();
    s_xygsJL_09_54.param.howerTime = itr->stNebulaRoutePoint.GetHoverTime();
    s_xygsJL_09_54.param.radius = itr->stNebulaRoutePoint.GetRadius();
    s_xygsJL_09_54.param.pointNumber = (*iter_list)->vecNebulaRoutePoint.size();
    m_Upload_PointNum = (*iter_list)->vecNebulaRoutePoint.size();
    s_xygsJL_09_54.param.pointSerialNo = nPointID + 1;
    int flag;
    unsigned char position;

    int m_eHeightCtrl = itr->stNebulaRoutePoint.GetHeightCtrl();
    int m_eLandFlag = itr->stNebulaRoutePoint.GetLandFlag();
    int m_eTaskFlag = itr->stNebulaRoutePoint.GetTaskFlag();
    int m_eLineProPerTy = itr->stNebulaRoutePoint.GetLineProPerTy();
    int m_eTurnMode = itr->stNebulaRoutePoint.GetTurnMode();
    switch (m_eHeightCtrl)
    {
    case CNebulaRoutePoint::EHeightCtrl::EHeightCtrl_Independent:
        flag = 0;
        position = 8;
        bit_set(&s_xygsJL_09_54.param.property1, position, flag);
        flag = 0;
        position = 7;
        bit_set(&s_xygsJL_09_54.param.property1, position, flag);
        break;
    case CNebulaRoutePoint::EHeightCtrl::EHeightCtrl_HightPreferred:
        flag = 1;
        position = 8;
        bit_set(&s_xygsJL_09_54.param.property1, position, flag);
        flag = 0;
        position = 7;
        bit_set(&s_xygsJL_09_54.param.property1, position, flag);
        break;
    case CNebulaRoutePoint::EHeightCtrl::EHeightCtrl_SlashCtrl:
        flag = 0;
        position = 8;
        bit_set(&s_xygsJL_09_54.param.property1, position, flag);
        flag = 1;
        position = 7;
        bit_set(&s_xygsJL_09_54.param.property1, position, flag);
        break;
    case CNebulaRoutePoint::EHeightCtrl::EHeightCtrl_Keep:
        flag = 1;
        position = 8;
        bit_set(&s_xygsJL_09_54.param.property1, position, flag);
        flag = 1;
        position = 7;
        bit_set(&s_xygsJL_09_54.param.property1, position, flag);
        break;
    default:
        break;
    }
    switch (m_eLandFlag)
    {
    case CNebulaRoutePoint::ELandFlag::ELandFlag_Normal:
        flag = 0;
        position = 2;
        bit_set(&s_xygsJL_09_54.param.property1, position, flag);
        break;
    case CNebulaRoutePoint::ELandFlag::ELandFlag_Land:
        flag = 1;
        position = 2;
        bit_set(&s_xygsJL_09_54.param.property1, position, flag);
    default:
        break;
    }
    switch (m_eTaskFlag)
    {
    case CNebulaRoutePoint::ETaskFlag::ETaskFlag_Normal:
        flag = 0;
        position = 1;
        bit_set(&s_xygsJL_09_54.param.property1, position, flag);
        break;
    case CNebulaRoutePoint::ETaskFlag::ETaskFlag_TakePicture:
        flag = 1;
        position = 1;
        bit_set(&s_xygsJL_09_54.param.property1, position, flag);
        break;
    default:
        break;
    }
    switch (m_eLineProPerTy)
    {
    case CNebulaRoutePoint::ELineProPerTy::ELineProPerTy_Whole:
        flag = 0;
        position = 1;
        bit_set(&s_xygsJL_09_54.param.property2, position, flag);
        break;
    case CNebulaRoutePoint::ELineProPerTy::ELineProPerTy_Alone:
        flag = 1;
        position = 1;
        bit_set(&s_xygsJL_09_54.param.property2, position, flag);
        break;
    default:
        break;
    }
    switch (m_eTurnMode)
    {
    case CNebulaRoutePoint::ETurnMode::ETurnMode_Precipice:
        flag = 0;
        position = 2;
        bit_set(&s_xygsJL_09_54.param.property2, position, flag);
        flag = 0;
        position = 3;
        bit_set(&s_xygsJL_09_54.param.property2, position, flag);
        break;
    case CNebulaRoutePoint::ETurnMode::ETaskFlag_Tangent:
        flag = 0;
        position = 2;
        bit_set(&s_xygsJL_09_54.param.property2, position, flag);
        flag = 1;
        position = 3;
        bit_set(&s_xygsJL_09_54.param.property2, position, flag);
        break;
    default:
        break;
    }

    nlen = sizeof(s_xygsJL_09_54);
    memcpy(pBuf, &s_xygsJL_09_54, nlen);
    return;
}

void CDlgEditLine::SetS_xygsJL_17(char *pBuf, int &nlen, int &nXYgroupID, int &nXYID)
{
    ClearS_xygsJL_17();
    s_xygsJL_17.groupIndex = nXYgroupID;
    s_xygsJL_17.planeIndex = nXYID;
    s_xygsJL_17.param.comandMain = 0xF1;
    s_xygsJL_17.param.comandSub = 0x21;
    s_xygsJL_17.param.comandExt = 00;

    nlen = sizeof(s_xygsJL_17);
    memcpy(pBuf, &s_xygsJL_17, nlen);
}

void CDlgEditLine::bit_set(unsigned char *pdata, unsigned char position, int flag)
{
    int i = 0;

    if (!pdata)
    {
        return;
    }
    if (position > 8 || position < 1 || (flag != 0 && flag != 1))
    {
        return;
    }
    if (flag != (*pdata >> (position - 1) & 1))
    {
        *pdata ^= 1 << (position - 1);
    }
}

void CDlgEditLine::DealUAVParameterInfo(const QString strWeaponId, const QByteArray &message)
{
    if (!m_RecvXYUAVParameterInfo)
    {
        return ;
    }

    if (message.size() <= 0)
    {
        return ;
    }

    emit UpdateUAVParameterInfoSig(strWeaponId, message);
}

void CDlgEditLine::UpdateUAVParameterInfo(const QString strWeaponId, const QByteArray message)
{
    if (!m_RecvXYUAVParameterInfo)
    {
        return ;
    }

    if (message.size() <= 0)
    {
        return ;
    }

    qnzkna::WeaponManage::WeaponComponent pWeaponModel;
    if (!m_pIWeaponManageService->GetWeaponComponent(&pWeaponModel, strWeaponId.toStdString()))
    {
        QString strTip = tr("%1 load point failed").arg(strWeaponId);
        ui->label_5->setText(strTip);
        return;
    }

    std::string strProtocolType = pWeaponModel.getProtocolType();
    if (strProtocolType == WEAPONSYSTEM_VEHICLE_PROTOCOLTYPE_XY)
    {
        HandleXYUAVParameterInfo(strWeaponId, message);
    }
    else if (strProtocolType == WEAPONSYSTEM_VEHICLE_PROTOCOLTYPE_MAVLINK)
    {

    }
    else if (strProtocolType == WEAPONSYSTEM_VEHICLE_PROTOCOLTYPE_DJI)
    {

    }
    else if (strProtocolType == WEAPONSYSTEM_VEHICLE_PROTOCOLTYPE_XY_54)
    {
        HandleXYUAVParameterInfo_54(strWeaponId, message);
    }
}

void CDlgEditLine::UpdateUAVInfoToWnd(xygsJL_09 s_xygsJL_09)
{
    CNebulaRoutePoint stNebulaRoutePoint;
    stNebulaRoutePoint.SetLongitude(s_xygsJL_09.param.position.lng);
    stNebulaRoutePoint.SetLatitude(s_xygsJL_09.param.position.lat);
    stNebulaRoutePoint.SetHeight(s_xygsJL_09.param.position.heigth);
    stNebulaRoutePoint.SetHoverTime(s_xygsJL_09.param.howerTime);
    stNebulaRoutePoint.SetSpeed(s_xygsJL_09.param.velocity);
    unsigned char charproperty = s_xygsJL_09.param.property;

    int  nproperty0 = (bool)((charproperty >> 0) & 1);

    int  nproperty1 = (bool)((charproperty >> 1) & 1);

    int  nproperty6 = (bool)((charproperty >> 6) & 1);

    int  nproperty7 = (bool)((charproperty >> 7) & 1);
    if (0 == nproperty0)
    {
        stNebulaRoutePoint.SetTaskFlag(CNebulaRoutePoint::ETaskFlag::ETaskFlag_Normal);
    }
    else
    {
        stNebulaRoutePoint.SetTaskFlag(CNebulaRoutePoint::ETaskFlag::ETaskFlag_TakePicture);
    }
    if (0 == nproperty1)
    {
        stNebulaRoutePoint.SetLandFlag(CNebulaRoutePoint::ELandFlag::ELandFlag_Normal);
    }
    else
    {
        stNebulaRoutePoint.SetLandFlag(CNebulaRoutePoint::ELandFlag::ELandFlag_Land);
    }

    if (0 == nproperty6 && 0 == nproperty7)
    {
        stNebulaRoutePoint.SetHeightCtrl(CNebulaRoutePoint::EHeightCtrl::EHeightCtrl_Independent);
    }
    else if (0 == nproperty6 && 1 == nproperty7)
    {
        stNebulaRoutePoint.SetHeightCtrl(CNebulaRoutePoint::EHeightCtrl::EHeightCtrl_HightPreferred);
    }
    else if (1 == nproperty6 && 0 == nproperty7)
    {
        stNebulaRoutePoint.SetHeightCtrl(CNebulaRoutePoint::EHeightCtrl::EHeightCtrl_SlashCtrl);
    }
    else
    {
        stNebulaRoutePoint.SetHeightCtrl(CNebulaRoutePoint::EHeightCtrl::EHeightCtrl_Keep);
    }

    int num = ui->tabWidget->currentIndex();
    if (num == -1)
    {
        return;
    }
    WndRouteLineInfoList::iterator iter_list = m_stWndRouteLineInfoList.m_vecRoutePointInfo.begin();
    iter_list += num;
    if (iter_list >= m_stWndRouteLineInfoList.m_vecRoutePointInfo.end())
    {
        return;
    }

    AddNebulaRoutePoint(num + 1, s_xygsJL_09.param.pointSerialNo, stNebulaRoutePoint);
}

void CDlgEditLine::UpdateUAVInfoToWnd54(xygsJL_V7_09 s_xygsJL_09_54)
{
    CNebulaRoutePoint stNebulaRoutePoint;
    stNebulaRoutePoint.SetLongitude(s_xygsJL_09_54.param.lng);
    stNebulaRoutePoint.SetLatitude(s_xygsJL_09_54.param.lat);
    stNebulaRoutePoint.SetHeight(s_xygsJL_09_54.param.heigth);
    stNebulaRoutePoint.SetHoverTime(s_xygsJL_09_54.param.howerTime);
    stNebulaRoutePoint.SetSpeed(s_xygsJL_09_54.param.speed);
    stNebulaRoutePoint.SetRadius(s_xygsJL_09_54.param.radius);
    unsigned char charproperty = s_xygsJL_09_54.param.property1;

    int  nproperty0 = (bool)((charproperty >> 0) & 1);

    int  nproperty1 = (bool)((charproperty >> 1) & 1);

    int  nproperty6 = (bool)((charproperty >> 6) & 1);

    int  nproperty7 = (bool)((charproperty >> 7) & 1);
    if (0 == nproperty0)
    {
        stNebulaRoutePoint.SetTaskFlag(CNebulaRoutePoint::ETaskFlag::ETaskFlag_Normal);
    }
    else
    {
        stNebulaRoutePoint.SetTaskFlag(CNebulaRoutePoint::ETaskFlag::ETaskFlag_TakePicture);
    }
    if (0 == nproperty1)
    {
        stNebulaRoutePoint.SetLandFlag(CNebulaRoutePoint::ELandFlag::ELandFlag_Normal);
    }
    else
    {
        stNebulaRoutePoint.SetLandFlag(CNebulaRoutePoint::ELandFlag::ELandFlag_Land);
    }
    if (0 == nproperty6 && 0 == nproperty7)
    {
        stNebulaRoutePoint.SetHeightCtrl(CNebulaRoutePoint::EHeightCtrl::EHeightCtrl_Independent);
    }
    else if (0 == nproperty6 && 1 == nproperty7)
    {
        stNebulaRoutePoint.SetHeightCtrl(CNebulaRoutePoint::EHeightCtrl::EHeightCtrl_HightPreferred);
    }
    else if (1 == nproperty6 && 0 == nproperty7)
    {
        stNebulaRoutePoint.SetHeightCtrl(CNebulaRoutePoint::EHeightCtrl::EHeightCtrl_SlashCtrl);
    }
    else
    {
        stNebulaRoutePoint.SetHeightCtrl(CNebulaRoutePoint::EHeightCtrl::EHeightCtrl_Keep);
    }

    unsigned char charproperty2 = s_xygsJL_09_54.param.property2;

    int  nproperty20 = (bool)((charproperty2 >> 0) & 1);

    int  nproperty21 = (bool)((charproperty2 >> 1) & 1);

    int  nproperty22 = (bool)((charproperty2 >> 2) & 1);

    if (nproperty20 == 1)
    {
        stNebulaRoutePoint.SetLineProPerTy(CNebulaRoutePoint::ELineProPerTy::ELineProPerTy_Alone);
    }
    else if (nproperty20 == 0)
    {
        stNebulaRoutePoint.SetLineProPerTy(CNebulaRoutePoint::ELineProPerTy::ELineProPerTy_Whole);
    }

    if (nproperty21 == 0 && nproperty22 == 0)
    {
        stNebulaRoutePoint.SetTurnMode(CNebulaRoutePoint::ETurnMode::ETurnMode_Precipice);
    }
    else if (nproperty21 == 0 && nproperty22 == 1)
    {
        stNebulaRoutePoint.SetTurnMode(CNebulaRoutePoint::ETurnMode::ETaskFlag_Tangent);
    }

    int num = ui->tabWidget->currentIndex();
    WndRouteLineInfoList::iterator iter_list = m_stWndRouteLineInfoList.m_vecRoutePointInfo.begin();
    iter_list += num;
    if (iter_list >= m_stWndRouteLineInfoList.m_vecRoutePointInfo.end())
    {
        return;
    }

    AddNebulaRoutePoint(num + 1, s_xygsJL_09_54.param.pointSerialNo, stNebulaRoutePoint);
}

bool CDlgEditLine::AddNebulaRoutePoint(int nLineID, int nPointID, CNebulaRoutePoint &stpoint)
{
    m_stWndRouteLineInfoList.lock();
    for (auto &line : m_stWndRouteLineInfoList.m_vecRoutePointInfo)
    {
        if (nLineID == line->nID)
        {

            {

                for (auto &spoint : line->vecNebulaRoutePoint)
                {
                    if (spoint.nPointID == nPointID)
                    {
                        spoint.stNebulaRoutePoint = stpoint;
                        m_stWndRouteLineInfoList.unlock();
                        return true;
                    }
                }

                {
                    std::vector<WndRoutePoint>::const_iterator vecNebulaRoutePointIter = line->vecNebulaRoutePoint.cbegin();
                    for (; vecNebulaRoutePointIter != line->vecNebulaRoutePoint.cend(); vecNebulaRoutePointIter++)
                    {
                        if (nPointID < vecNebulaRoutePointIter->nPointID)
                        {
                            WndRoutePoint stWndRoutePoint;
                            stWndRoutePoint.nPointID = nPointID;
                            stWndRoutePoint.stNebulaRoutePoint = stpoint;
                            line->vecNebulaRoutePoint.insert(vecNebulaRoutePointIter, stWndRoutePoint);
                            m_stWndRouteLineInfoList.unlock();
                            return true;
                        }
                    }

                    WndRoutePoint stWndRoutePoint;
                    stWndRoutePoint.nPointID           = nPointID;
                    stWndRoutePoint.stNebulaRoutePoint = stpoint;
                    line->vecNebulaRoutePoint.push_back(stWndRoutePoint);

                    m_stWndRouteLineInfoList.unlock();
                    return false;
                }
            }
        }
    }
    m_stWndRouteLineInfoList.unlock();
    return false;
}

void CDlgEditLine::HandleXYUAVParameterInfo(const QString strWeaponId, const QByteArray message)
{
    if (!m_RecvXYUAVParameterInfo)
    {
        return ;
    }

    if (message.size() <= 0)
    {
        return ;
    }

    const long nHead = *(reinterpret_cast <const long *>(message.constData()));

    if (8 == nHead)
    {
        StopAllTimer();
        ClearS_xygsJL_08();
        s_xygsJL_08 = *((xygsJL_08 *)message.constData());
        if ((s_xygsJL_08.param.lastPoint == m_Upload_Point_No + 2) && (s_xygsJL_08.param.pointNumber == m_Upload_PointNum))
        {

            QString strTip;
            if ((m_Upload_Point_No + 1) >= m_Upload_PointNum)
            {
                m_RecvXYUAVParameterInfo = false;
                StopAllTimer();
                m_UpLoadTimer->stop();
                ui->tabWidget->setEnabled(true);
                strTip = tr("Upload Success!");

                ui->label_9->setText(strTip);
                ui->label_11->hide();

                if (ui->label_5)
                {
                    strTip = tr("Point %1 upload success").arg(m_Upload_Point_No + 1);

                    ui->label_5->setText(strWeaponId + " " + strTip);
                }
                char pBufTmp[MAX_BUFFER_LEN] = { 0 };
                int nBufLen = 0;
                m_Upload_Point_No++;
                strTip = tr2("%1 of %2").arg(m_Upload_Point_No).arg(m_Upload_PointNum);
                ui->label_7->setText(strTip);

                int nProgress = m_Upload_Point_No * 100 / m_Upload_PointNum;
                ui->progressBar->setValue(nProgress);

                return;
            }

            if (ui->label_5)
            {
                strTip = tr("Point %1 upload success").arg(m_Upload_Point_No + 1);

                ui->label_5->setText(strWeaponId + " " + strTip);
            }
            char pBufTmp[MAX_BUFFER_LEN] = { 0 };
            int nBufLen = 0;
            m_Upload_Point_No++;
            strTip = tr2("%1 of %2").arg(m_Upload_Point_No).arg(m_Upload_PointNum);
            ui->label_7->setText(strTip);

            int nProgress = m_Upload_Point_No * 100 / m_Upload_PointNum;
            ui->progressBar->setValue(nProgress);

            m_Upload_Send_No = 0;
            int num = ui->tabWidget->currentIndex();
            WndRouteLineInfoList::iterator iter_list = m_stWndRouteLineInfoList.m_vecRoutePointInfo.begin();
            iter_list += num;
            if (iter_list >= m_stWndRouteLineInfoList.m_vecRoutePointInfo.end())
            {
                m_RecvXYUAVParameterInfo = true;
                m_UpLoadTimer->start();
                return;
            }

            SetS_xygsJL_09(pBufTmp, nBufLen, m_Upload_Point_No);

            SendBagData(m_Upload_Point_No + 1, (*iter_list)->strID, (*iter_list)->strWeaponID, pBufTmp, nBufLen);

            strTip = tr("Uploading");

            ui->label_9->setText(strTip);
            ui->label_11->setText(QString::number(m_Upload_Send_No));

        }

        if ((s_xygsJL_08.param.lastPoint == m_Upload_PointNum + 1)
                && (s_xygsJL_08.param.pointNumber == m_Upload_PointNum)

           )
        {
            m_RecvXYUAVParameterInfo = false;
            StopAllTimer();
            m_UpLoadTimer->stop();
            ui->tabWidget->setEnabled(true);
            QString strTip = tr("Upload Success!");

            ui->label_9->setText(strTip);
            ui->label_11->hide();
        }
        else
        {
            m_RecvXYUAVParameterInfo = true;
            m_UpLoadTimer->start();
        }
    }
    else if (9 == nHead)
    {
        if (1 == m_update_Flag)
        {
            return;
        }
        m_DownLoadTimeCount    = 0;
        m_DownLoadTryTimeCount = 0;
        ClearS_xygsJL_09();
        s_xygsJL_09 = *((xygsJL_09 *)message.constData());
        ClearS_xygsJL_08();
        s_xygsJL_08.groupIndex = s_xygsJL_09.groupIndex;
        s_xygsJL_08.planeIndex = s_xygsJL_09.planeIndex;
        s_xygsJL_08.param.lastPoint = s_xygsJL_09.param.pointSerialNo + 1;
        s_xygsJL_08.param.pointNumber = s_xygsJL_09.param.pointNumber;
        m_DownLoadMaxCount = s_xygsJL_09.param.pointNumber;
        int num = ui->tabWidget->currentIndex();
        if (num == -1)
        {
            return;
        }

        UpdateUAVInfoToWnd(s_xygsJL_09);

        QString strTip9 = tr("Downloading");
        ui->label_9->setText(strTip9);

        QString strTip = tr2("%1 of %2").arg(s_xygsJL_09.param.pointSerialNo).arg(m_DownLoadMaxCount);
        ui->label_7->setText(strTip);

        QString strTip5 = tr("Point %1 update complete").arg(QString::number(s_xygsJL_09.param.pointSerialNo));

        ui->label_5->setText(strWeaponId + " " + strTip5);

        ui->label_11->setText(QString::number(m_DownLoadTryTimeCount));

        WndRouteLineInfoList::iterator iter_list = m_stWndRouteLineInfoList.m_vecRoutePointInfo.begin();
        iter_list += num;
        if (iter_list >= m_stWndRouteLineInfoList.m_vecRoutePointInfo.end())
        {
            return;
        }

        bool downloadSucess = false;
        unsigned long long int vecNebulaRoutePointSize = 0;
        {
            m_stWndRouteLineInfoList.lock();
            int nLineID = (ui->tabWidget->currentIndex() + 1);
            for (auto &line : m_stWndRouteLineInfoList.m_vecRoutePointInfo)
            {
                if (nLineID == line->nID)
                {
                    vecNebulaRoutePointSize = line->vecNebulaRoutePoint.size();
                    downloadSucess = (vecNebulaRoutePointSize >= s_xygsJL_09.param.pointNumber);
                    break;
                }
            }

            m_stWndRouteLineInfoList.unlock();
        }

        if (vecNebulaRoutePointSize > 0)
        {
            int nProgress = vecNebulaRoutePointSize * 100 / m_DownLoadMaxCount;
            ui->progressBar->setValue(nProgress);
        }

        QString strTip7 = tr2("%1 of %2").arg(vecNebulaRoutePointSize).arg(m_DownLoadMaxCount);
        ui->label_7->setText(strTip7);

        if (downloadSucess)
        {
            m_RecvXYUAVParameterInfo = false;
            StopAllTimer();
            m_DownLoadTimer->stop();
            UpdateTrackInfoToListControl(num + 1);
            ui->tabWidget->setEnabled(true);
            QString strTip = tr("All points update complete.");
            ui->label_5->setText(strWeaponId + " " + strTip);
            QString strTip9 = tr("Download Success!");
            ui->label_9->setText(strTip9);
            ui->label_11->hide();
            m_update_Flag = 1;
            return;
        }

    }

}

void CDlgEditLine::HandleXYUAVParameterInfo_54(const QString strWeaponId, const QByteArray message)
{
    if (!m_RecvXYUAVParameterInfo)
    {
        return ;
    }

    if (message.size() <= 0)
    {
        return ;
    }

    const long nHead = *(reinterpret_cast <const long *>(message.constData()));

    if (540708 == nHead)
    {
        StopAllTimer();
        ClearS_xygsJL_08_54();
        s_xygsJL_08_54 = *((xygsJL_V7_08 *)message.constData());
        if ((s_xygsJL_08_54.param.lastPoint == m_Upload_Point_No + 2) && (s_xygsJL_08_54.param.pointNumber == m_Upload_PointNum))
        {

            QString strTip;
            if (ui->label_5)
            {
                strTip = tr("Point %1 upload success.").arg(m_Upload_Point_No + 1);

                ui->label_5->setText(strWeaponId + " " + strTip);
            }
            char pBufTmp[MAX_BUFFER_LEN] = { 0 };
            int nBufLen = 0;
            m_Upload_Point_No++;
            strTip = tr2("%1 of %2").arg(m_Upload_Point_No).arg(m_Upload_PointNum);
            ui->label_7->setText(strTip);

            int nProgress = m_Upload_Point_No * 100 / m_Upload_PointNum;
            ui->progressBar->setValue(nProgress);
            if ((m_Upload_Point_No + 1) > m_Upload_PointNum)
            {
                m_RecvXYUAVParameterInfo = false;
                StopAllTimer();
                m_UpLoadTimer54->stop();
                ui->tabWidget->setEnabled(true);
                strTip = tr("Upload Success");

                ui->label_9->setText(strTip);
                ui->label_11->hide();
                return;
            }
            m_Upload_Send_No = 0;
            int num = ui->tabWidget->currentIndex();
            if (num == -1)
            {
                m_RecvXYUAVParameterInfo = true;
                m_UpLoadTimer54->start();
                return;
            }
            WndRouteLineInfoList::iterator iter_list = m_stWndRouteLineInfoList.m_vecRoutePointInfo.begin();
            iter_list += num;
            if (iter_list >= m_stWndRouteLineInfoList.m_vecRoutePointInfo.end())
            {
                m_RecvXYUAVParameterInfo = true;
                m_UpLoadTimer54->start();
                return;
            }

            SetS_xygsJL_09_54(pBufTmp, nBufLen, m_Upload_Point_No);
            SendBagData54(m_Upload_Point_No + 1, (*iter_list)->strID, (*iter_list)->strWeaponID, pBufTmp, nBufLen);

            strTip = tr("Uploading");

            ui->label_9->setText(strTip);
            ui->label_11->setText(QString::number(m_Upload_Send_No));

        }
        if ((s_xygsJL_08.param.lastPoint == m_Upload_PointNum + 1)
                && (s_xygsJL_08.param.pointNumber == m_Upload_PointNum)

           )
        {
            m_RecvXYUAVParameterInfo = false;
            StopAllTimer();
            m_UpLoadTimer54->stop();
            ui->tabWidget->setEnabled(true);
            QString strTip = tr("Upload Success");

            ui->label_9->setText(strTip);
            ui->label_11->hide();
        }
        else
        {
            m_RecvXYUAVParameterInfo = true;
            m_UpLoadTimer54->start();
        }
    }
    else if (540709 == nHead)
    {
        if (1 == m_update_Flag)
        {
            return;
        }
        m_DownLoadTimeCount54    = 0;
        m_DownLoadTryTimeCount54 = 0;
        ClearS_xygsJL_09_54();
        s_xygsJL_09_54 = *((xygsJL_V7_09 *)message.constData());
        ClearS_xygsJL_08_54();
        s_xygsJL_08_54.groupIndex = s_xygsJL_09_54.groupIndex;
        s_xygsJL_08_54.planeIndex = s_xygsJL_09_54.planeIndex;
        s_xygsJL_08_54.param.lastPoint = s_xygsJL_09_54.param.pointSerialNo + 1;
        s_xygsJL_08_54.param.pointNumber = s_xygsJL_09_54.param.pointNumber;
        m_DownLoadMaxCount54 = s_xygsJL_09_54.param.pointNumber;

        int num = ui->tabWidget->currentIndex();
        if (num == -1)
        {
            return;
        }

        UpdateUAVInfoToWnd54(s_xygsJL_09_54);

        QString strTip9 = tr("Downloading");
        ui->label_9->setText(strTip9);

        QString strTip5 = tr("Point %1 update complete").arg(QString::number(s_xygsJL_09_54.param.pointSerialNo));
        ui->label_5->setText(strWeaponId + " " + strTip5);

        ui->label_11->setText(QString::number(m_DownLoadTryTimeCount54));

        bool downloadSucess = false;
        unsigned long long int vecNebulaRoutePointSize = 0;
        {
            m_stWndRouteLineInfoList.lock();
            int nLineID = (ui->tabWidget->currentIndex() + 1);
            for (auto &line : m_stWndRouteLineInfoList.m_vecRoutePointInfo)
            {
                if (nLineID == line->nID)
                {
                    vecNebulaRoutePointSize = line->vecNebulaRoutePoint.size();
                    downloadSucess = (vecNebulaRoutePointSize >= s_xygsJL_09_54.param.pointNumber);
                    break;
                }
            }

            m_stWndRouteLineInfoList.unlock();
        }

        if (vecNebulaRoutePointSize > 0)
        {
            int nProgress = vecNebulaRoutePointSize * 100 / m_DownLoadMaxCount54;
            ui->progressBar->setValue(nProgress);
        }

        QString strTip7 = tr2("%1 of %2").arg(vecNebulaRoutePointSize).arg(m_DownLoadMaxCount54);
        ui->label_7->setText(strTip7);

        if (downloadSucess)
        {
            m_RecvXYUAVParameterInfo = false;
            StopAllTimer();
            m_DownLoadTimer54->stop();
            UpdateTrackInfoToListControl(num + 1);
            m_update_Flag = 1;
            QString strTip = tr("All points update complete.");
            ui->label_5->setText(strWeaponId + " " + strTip);
            ui->tabWidget->setEnabled(true);
            QString strTip9 = tr("Download Success!");
            ui->label_9->setText(strTip9);
            ui->label_11->hide();

            return;
        }
    }

}

void CDlgEditLine::TabWidgetConnet(int Culsel)
{

    int nCount = ui->tabWidget->count();

    if (Culsel > (nCount + 1))
    {
        return;
    }

    for (int i = 0; i < nCount; i++)
    {
        QWidget *p = ui->tabWidget->widget(i);
        if (p->layout()->count() > 0)
        {
            QLayoutItem *it = p->layout()->itemAt(0);
            QTableWidget *pTabCtrl = qobject_cast<QTableWidget *>(it->widget());
            pTabCtrl->disconnect();
        }
    }

    QWidget *p1 = ui->tabWidget->widget(Culsel - 1);
    if (p1->layout()->count() > 0)
    {
        QLayoutItem *it = p1->layout()->itemAt(0);
        QTableWidget *pTabCtrl1 = qobject_cast<QTableWidget *>(it->widget());
        connect(pTabCtrl1, SIGNAL(customContextMenuRequested(const QPoint)), this, SLOT(on_tableWidget_customContextMenuRequested(const QPoint)));
    }
}

void CDlgEditLine::CreateTableMenu()
{
    m_TableMenu = new QMenu(this);
    Startuavroute	= m_TableMenu->addAction(tr("Start Edit"));
    Stopuavroute	= m_TableMenu->addAction(tr("Stop Edit"));
    Removepoint		= m_TableMenu->addAction(tr("Remove Point"));
    Insetpoint		= m_TableMenu->addAction(tr("Insert Point"));
    Upload			= m_TableMenu->addAction(tr("Upload Points"));
    Saveplanepoint	= m_TableMenu->addAction(tr("Load Points"));

    Removeallpoint	= m_TableMenu->addAction(tr("Remove All points"));

    SetFlayGoal = m_TableMenu->addAction(tr("Cutover to Point"));
    landCurrentVehiclePosition = m_TableMenu->addAction(tr("Update current pos"));

    connect(Startuavroute, SIGNAL(triggered()), this, SLOT(OnStartuavroute()));
    connect(Stopuavroute, SIGNAL(triggered()), this, SLOT(OnStopuavroute()));
    connect(Removepoint, SIGNAL(triggered()), this, SLOT(OnRemovepoint()));
    connect(Insetpoint, SIGNAL(triggered()), this, SLOT(OnInsetpoint()));

    connect(Upload, SIGNAL(triggered()), this, SLOT(OnUpload()));
    connect(Saveplanepoint, SIGNAL(triggered()), this, SLOT(OnSaveplanepoint()));

    connect(Removeallpoint, SIGNAL(triggered()), this, SLOT(OnRemoveallpoint()));
    connect(SetFlayGoal, SIGNAL(triggered()), this, SLOT(OnSetFlayGoal()));
    connect(landCurrentVehiclePosition, SIGNAL(triggered()), this, SLOT(OnLandCurrentVehiclePosition()));

}

EeditStates CDlgEditLine::GetLineEditStates()
{
    return m_bLiseEditStates;
}

bool CDlgEditLine::AddLinePoint(double dLon, double dLat)
{
    int nCul = ui->tabWidget->currentIndex();
    QString strTabName = ui->tabWidget->tabText(nCul);
    if (nCul < 0 || strTabName.isEmpty())
    {
        return false;
    }

    WndRouteLineInfoList::iterator routeLineInfolist = m_stWndRouteLineInfoList.m_vecRoutePointInfo.begin();
    if ((routeLineInfolist + nCul) >= m_stWndRouteLineInfoList.m_vecRoutePointInfo.end())
    {
        return false;
    }

    routeLineInfolist += nCul;

    if (strTabName.toStdString() != (*routeLineInfolist)->strID)
    {
        return false;
    }

    m_stWndRouteLineInfoList.lock();

    WndRoutePoint stPoint;
    stPoint.nPointID = (*routeLineInfolist)->vecNebulaRoutePoint.size() + 1;
    stPoint.stNebulaRoutePoint.SetLongitude(dLon);
    stPoint.stNebulaRoutePoint.SetLatitude(dLat);
    stPoint.stNebulaRoutePoint.SetHeight(INIT_HEIGHT);

    if ((*routeLineInfolist)->vecNebulaRoutePoint.size() > 0)
    {
        auto itr = (*routeLineInfolist)->vecNebulaRoutePoint.rbegin();
        double distance = get_distance_from_lat_lon(dLat, dLon, itr->stNebulaRoutePoint.GetLatitude(), itr->stNebulaRoutePoint.GetLongitude());
        stPoint.dLocalDistance = distance;
    }

    (*routeLineInfolist)->vecNebulaRoutePoint.push_back(stPoint);
    m_stWndRouteLineInfoList.unlock();

    UpdateTrackInfoToListControl(nCul + 1);
    return true;
}

bool CDlgEditLine::AddLinePointMouseDoubleClick(double dLon, double dLat, double dHeight)
{
    const int     nCul     = ui->tabWidget->currentIndex();
    const QString strTabName = ui->tabWidget->tabText(nCul);

    if (nCul < 0 || strTabName.isEmpty())
    {
        return false;
    }

    WndRouteLineInfoList::iterator routeLineInfolist = m_stWndRouteLineInfoList.m_vecRoutePointInfo.begin();
    if ((routeLineInfolist + nCul) >= m_stWndRouteLineInfoList.m_vecRoutePointInfo.end())
    {
        return false;
    }

    routeLineInfolist += nCul;

    int nIndex = strTabName.indexOf(":");
    if (strTabName.mid(nIndex).length() == 1)
    {
        return false;
    }
    QString strID = strTabName.mid(nIndex + 1);
    if (strID.toStdString() != (*routeLineInfolist)->strID)
    {
        return false;
    }

    m_stWndRouteLineInfoList.lock();

    WndRoutePoint stPoint;
    stPoint.nPointID = (*routeLineInfolist)->vecNebulaRoutePoint.size() + 1;
    stPoint.stNebulaRoutePoint.SetLongitude(dLon);
    stPoint.stNebulaRoutePoint.SetLatitude(dLat);
    if (dHeight == 0)
    {
        dHeight = INIT_HEIGHT;
    }
    stPoint.stNebulaRoutePoint.SetHeight(dHeight);

    if ((*routeLineInfolist)->vecNebulaRoutePoint.size() > 0)
    {
        auto itr = (*routeLineInfolist)->vecNebulaRoutePoint.rbegin();
        double distance = get_distance_from_lat_lon(dLat, dLon, itr->stNebulaRoutePoint.GetLatitude(), itr->stNebulaRoutePoint.GetLongitude());
        stPoint.dLocalDistance = distance;
    }

    (*routeLineInfolist)->vecNebulaRoutePoint.push_back(stPoint);
    m_stWndRouteLineInfoList.unlock();

    std::string strWeaponID("");
    {
        const int Culsel = nCul + 1;
        m_stWndRouteLineInfoList.lock();
        WndRouteLineInfoList::iterator iter_list = m_stWndRouteLineInfoList.m_vecRoutePointInfo.begin();
        for (; iter_list != m_stWndRouteLineInfoList.m_vecRoutePointInfo.end(); iter_list++)
        {
            if ((*iter_list)->nID == Culsel)
            {
                strWeaponID = (*iter_list)->strWeaponID;
                std::string strProtocolType = GetCurWeaponComponentProtocolType(Culsel);
                if (strProtocolType == "")
                {
                    continue;
                }

                if (strProtocolType == WEAPONSYSTEM_VEHICLE_PROTOCOLTYPE_XY)
                {
                    UpdateXYTrackInfoToListControl(Culsel, &iter_list);
                }
                else if (strProtocolType == WEAPONSYSTEM_VEHICLE_PROTOCOLTYPE_XY_54)
                {

                    {
                        int nCount = ui->tabWidget->count();
                        if (Culsel > (nCount + 1))
                        {
                            return false;
                        }

                        QWidget *p = ui->tabWidget->widget(Culsel - 1);
                        if (p->layout()->count() <= 0)
                        {
                            return false;
                        }

                        QLayoutItem *it = p->layout()->itemAt(0);
                        QTableWidget *pTabCtrl = qobject_cast<QTableWidget *>(it->widget());

                        pTabCtrl->setRowCount((*(iter_list))->vecNebulaRoutePoint.size());
                        pTabCtrl->disconnect(SIGNAL(cellChanged(int, int)));

                        const QStringList list08 = { tr("Independent"), tr("HightPreferred"), tr("SlashCtrl"), tr("Keep") };
                        const QStringList list10 = { tr("Whole"), tr("Single") };
                        const QStringList list11 = { tr("Precipice"), tr("Tangent") };

                        if ((*(iter_list))->vecNebulaRoutePoint.size() > 0 && pTabCtrl->rowCount() > 0)
                        {
                            std::vector<WndRoutePoint>::const_reference wndRoutePoint = (*(iter_list))->vecNebulaRoutePoint.back();
                            const int    m_No            = wndRoutePoint.nPointID - 1;
                            const double m_localdistance = stPoint.dLocalDistance;
                            const double m_dLongitude    = wndRoutePoint.stNebulaRoutePoint.GetLongitude();
                            const double m_dLatitude     = wndRoutePoint.stNebulaRoutePoint.GetLatitude();
                            const int    m_height        = wndRoutePoint.stNebulaRoutePoint.GetHeight();
                            const int    m_eHeightCtrl   = wndRoutePoint.stNebulaRoutePoint.GetHeightCtrl();

                            const int m_eLandFlag   = wndRoutePoint.stNebulaRoutePoint.GetLandFlag();
                            const int m_eTaskFlag   = wndRoutePoint.stNebulaRoutePoint.GetTaskFlag();
                            const int m_nSpeed      = wndRoutePoint.stNebulaRoutePoint.GetSpeed();
                            const int m_nHoverTime  = wndRoutePoint.stNebulaRoutePoint.GetHoverTime();
                            const int m_nRadius     = wndRoutePoint.stNebulaRoutePoint.GetRadius();
                            const int nLineProPerTy = wndRoutePoint.stNebulaRoutePoint.GetLineProPerTy();
                            const int nTurnMode     = wndRoutePoint.stNebulaRoutePoint.GetTurnMode();

                            QComboBox	*combobox08 = new QComboBox();
                            QComboBox	*combobox10 = new QComboBox();
                            QComboBox	*combobox11 = new QComboBox();
                            combobox08->addItems(list08);
                            combobox10->addItems(list10);
                            combobox11->addItems(list11);

                            QString No, Longitude, Latitude, height, localdistance, LandFlag, TaskFlag, Speed, HoverTime, strRadius;
                            No = QString::number(m_No + 1);
                            Longitude = QString::number(m_dLongitude, 'f', 7);
                            Latitude = QString::number(m_dLatitude, 'f', 7);
                            height = QString::number(m_height);
                            switch (m_eHeightCtrl)
                            {
                            case CNebulaRoutePoint::EHeightCtrl::EHeightCtrl_Independent:
                                combobox08->setCurrentIndex(0);
                                break;
                            case CNebulaRoutePoint::EHeightCtrl::EHeightCtrl_HightPreferred:
                                combobox08->setCurrentIndex(1);
                                break;
                            case CNebulaRoutePoint::EHeightCtrl::EHeightCtrl_SlashCtrl:
                                combobox08->setCurrentIndex(2);
                                break;
                            case CNebulaRoutePoint::EHeightCtrl::EHeightCtrl_Keep:
                                combobox08->setCurrentIndex(3);
                                break;
                            default:
                                break;
                            }
                            localdistance = QString::number(m_localdistance, 'f', 3);
                            switch (m_eLandFlag)
                            {
                            case CNebulaRoutePoint::ELandFlag::ELandFlag_Normal:
                                LandFlag = tr("Normal");
                                break;
                            case CNebulaRoutePoint::ELandFlag::ELandFlag_Land:
                                LandFlag = tr("Land");
                                break;
                            default:
                                break;
                            }
                            switch (m_eTaskFlag)
                            {
                            case CNebulaRoutePoint::ETaskFlag::ETaskFlag_Normal:
                                TaskFlag = tr("Normal");
                                break;
                            case CNebulaRoutePoint::ETaskFlag::ETaskFlag_TakePicture:
                                TaskFlag = tr("Photo");
                                break;
                            default:
                                break;
                            }
                            switch (nLineProPerTy)
                            {
                            case CNebulaRoutePoint::ELineProPerTy::ELineProPerTy_Whole:
                                combobox10->setCurrentIndex(0);
                                break;
                            case CNebulaRoutePoint::ELineProPerTy::ELineProPerTy_Alone:
                                combobox10->setCurrentIndex(1);
                                break;
                            default:
                                break;
                            }
                            switch (nTurnMode)
                            {
                            case CNebulaRoutePoint::ETurnMode::ETurnMode_Precipice:
                                combobox11->setCurrentIndex(0);
                                break;
                            case CNebulaRoutePoint::ETurnMode::ETaskFlag_Tangent:
                                combobox11->setCurrentIndex(1);
                                break;
                            default:
                                break;
                            }

                            Speed = QString::number(m_nSpeed);
                            HoverTime = QString::number(m_nHoverTime);
                            strRadius = QString::number(m_nRadius);

                            QTableWidgetItem *item0 = new QTableWidgetItem(No);
                            QTableWidgetItem *item1 = new QTableWidgetItem(Longitude);
                            QTableWidgetItem *item2 = new QTableWidgetItem(Latitude);
                            QTableWidgetItem *item3 = new QTableWidgetItem(height);
                            QTableWidgetItem *item4 = new QTableWidgetItem(localdistance);
                            QTableWidgetItem *item5 = new QTableWidgetItem(LandFlag);
                            QTableWidgetItem *item6 = new QTableWidgetItem(Speed);
                            QTableWidgetItem *item7 = new QTableWidgetItem(HoverTime);

                            QTableWidgetItem *item9 = new QTableWidgetItem(TaskFlag);
                            QTableWidgetItem *item10 = new QTableWidgetItem(strRadius);

                            const int tableWidgetRows = pTabCtrl->rowCount() - 1;
                            pTabCtrl->setItem(tableWidgetRows, 0, item0);
                            pTabCtrl->setItem(tableWidgetRows, 1, item1);
                            pTabCtrl->setItem(tableWidgetRows, 2, item2);
                            pTabCtrl->setItem(tableWidgetRows, 3, item3);
                            pTabCtrl->setItem(tableWidgetRows, 4, item4);
                            pTabCtrl->setItem(tableWidgetRows, 5, item5);
                            pTabCtrl->setItem(tableWidgetRows, 6, item6);
                            pTabCtrl->setItem(tableWidgetRows, 7, item7);

                            pTabCtrl->setCellWidget(tableWidgetRows, 8, combobox08);

                            pTabCtrl->setItem(tableWidgetRows, 9, item9);
                            pTabCtrl->setItem(tableWidgetRows, 10, item10);
                            pTabCtrl->setCellWidget(tableWidgetRows, 11, combobox10);
                            pTabCtrl->setCellWidget(tableWidgetRows, 12, combobox11);

                            pTabCtrl->item(tableWidgetRows, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
                            pTabCtrl->item(tableWidgetRows, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
                            pTabCtrl->item(tableWidgetRows, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
                            pTabCtrl->item(tableWidgetRows, 3)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
                            pTabCtrl->item(tableWidgetRows, 4)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
                            pTabCtrl->item(tableWidgetRows, 5)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
                            pTabCtrl->item(tableWidgetRows, 6)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
                            pTabCtrl->item(tableWidgetRows, 7)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

                            pTabCtrl->item(tableWidgetRows, 9)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
                            pTabCtrl->item(tableWidgetRows, 10)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

                            connect(combobox08, SIGNAL(currentIndexChanged(int)), this, SLOT(on_combobox_08_currentIndexChanged(int)));
                            connect(combobox10, SIGNAL(currentIndexChanged(int)), this, SLOT(on_combobox_10_currentIndexChanged(int)));
                            connect(combobox11, SIGNAL(currentIndexChanged(int)), this, SLOT(on_combobox_11_currentIndexChanged(int)));

                            item0->setFlags(Qt::NoItemFlags);
                        }

                        connect(pTabCtrl, SIGNAL(cellChanged(int, int)), this, SLOT(on_tableWidget_cellChanged(int, int)));
                    }
                }
                else if (strProtocolType == WEAPONSYSTEM_VEHICLE_PROTOCOLTYPE_MAVLINK)
                {
                    UpdateMavTrackInfoToListControl(&iter_list);
                }
                else if (strProtocolType == WEAPONSYSTEM_VEHICLE_PROTOCOLTYPE_DJI)
                {
                    UpdateXYTrackInfoToListControl(Culsel, &iter_list);
                }
                else
                {

                }

                UpdateLineLength((*iter_list)->vecNebulaRoutePoint);
            }
        }
        m_stWndRouteLineInfoList.unlock();
    }

    if (!strWeaponID.empty())
    {
        LY_TaskManage::Get().Draw3DLinesWeaponID(strWeaponID);
    }
    return true;
}

bool CDlgEditLine::InsertLinePoint(double dLon, double dLat, double dHeight)
{
    int nCul = ui->tabWidget->currentIndex();
    QString strTabName = ui->tabWidget->tabText(nCul);
    if (nCul < 0 || strTabName.isEmpty())
    {
        return false;
    }

    WndRouteLineInfoList::iterator routeLineInfolist = m_stWndRouteLineInfoList.m_vecRoutePointInfo.begin();
    if ((routeLineInfolist + nCul) >= m_stWndRouteLineInfoList.m_vecRoutePointInfo.end())
    {
        return false;
    }

    routeLineInfolist += nCul;

    if (strTabName.toStdString() != (*routeLineInfolist)->strID)
    {
        return false;
    }

    auto ItrP = (*routeLineInfolist)->vecNebulaRoutePoint.begin();
    if (m_Hitrow > (*routeLineInfolist)->vecNebulaRoutePoint.size())
    {
        return false;
    }

    ItrP += m_Hitrow;
    m_stWndRouteLineInfoList.lock();
    WndRoutePoint stPoint;
    stPoint.nPointID = m_Hitrow + 1;
    stPoint.stNebulaRoutePoint.SetLongitude(dLon);
    stPoint.stNebulaRoutePoint.SetLatitude(dLat);
    if (dHeight == 0)
    {
        dHeight = INIT_HEIGHT;
    }
    stPoint.stNebulaRoutePoint.SetHeight(dHeight);
    (*routeLineInfolist)->vecNebulaRoutePoint.insert(ItrP, stPoint);

    auto nPointID = 1;
    for (auto &LinePoint : (*routeLineInfolist)->vecNebulaRoutePoint)
    {
        LinePoint.nPointID = nPointID;
        nPointID++;
    }
    m_stWndRouteLineInfoList.unlock();

    UpdateTrackInfoToListControl(nCul + 1);

    m_bLiseEditStates = PickNormal;
    return true;
}

bool CDlgEditLine::UpdateLinePoint(int nLineID, int nPointID, double dLon, double dLat)
{
    int nCul = ui->tabWidget->currentIndex();

    if (nLineID != (nCul + 1))
    {
        return false;
    }

    WndRouteLineInfoList::iterator routeLineInfolist = m_stWndRouteLineInfoList.m_vecRoutePointInfo.begin();
    if ((routeLineInfolist + nCul) >= m_stWndRouteLineInfoList.m_vecRoutePointInfo.end())
    {
        return false;
    }

    routeLineInfolist += nCul;
    m_stWndRouteLineInfoList.lock();
    for (auto &itrPoint : (*routeLineInfolist)->vecNebulaRoutePoint)
    {
        if (itrPoint.nPointID == nPointID)
        {
            itrPoint.stNebulaRoutePoint.SetLongitude(dLon);
            itrPoint.stNebulaRoutePoint.SetLatitude(dLat);

            m_stWndRouteLineInfoList.unlock();
            UpdateTrackInfoToListControl(nCul + 1);
            return true;
        }
    }
    m_stWndRouteLineInfoList.unlock();
    return false;
}

bool CDlgEditLine::UpdateLinePointMouseMove(int nLineID, int nPointID, double dLon, double dLat)
{
    int nCul = ui->tabWidget->currentIndex();

    if (nLineID != (nCul + 1))
    {
        return false;
    }

    WndRouteLineInfoList::iterator routeLineInfolist = m_stWndRouteLineInfoList.m_vecRoutePointInfo.begin();
    if ((routeLineInfolist + nCul) >= m_stWndRouteLineInfoList.m_vecRoutePointInfo.end())
    {
        return false;
    }

    routeLineInfolist += nCul;
    m_stWndRouteLineInfoList.lock();
    std::vector<WndRoutePoint>::iterator itrPoint = (*routeLineInfolist)->vecNebulaRoutePoint.begin();
    double dOldLon, dOldLat;
    for (; itrPoint != (*routeLineInfolist)->vecNebulaRoutePoint.end(); itrPoint++)
    {
        if (itrPoint->nPointID == nPointID)
        {
            itrPoint->stNebulaRoutePoint.SetLongitude(dLon);
            itrPoint->stNebulaRoutePoint.SetLatitude(dLat);

            if (itrPoint != (*routeLineInfolist)->vecNebulaRoutePoint.begin())
            {
                itrPoint->dLocalDistance = get_distance_from_lat_lon(dOldLat, dOldLon, dLat, dLon);
            }

            if ((itrPoint + 1) != (*routeLineInfolist)->vecNebulaRoutePoint.end())
            {
                (itrPoint + 1)->dLocalDistance = get_distance_from_lat_lon(dLat, dLon, (itrPoint + 1)->stNebulaRoutePoint.GetLatitude(), (itrPoint + 1)->stNebulaRoutePoint.GetLongitude());
            }

            m_stWndRouteLineInfoList.unlock();

            {
                const int Culsel = nCul + 1;
                m_stWndRouteLineInfoList.lock();
                WndRouteLineInfoList::iterator iter_list = m_stWndRouteLineInfoList.m_vecRoutePointInfo.begin();
                for (; iter_list != m_stWndRouteLineInfoList.m_vecRoutePointInfo.end(); iter_list++)
                {
                    if ((*iter_list)->nID == Culsel)
                    {
                        std::string strProtocolType = GetCurWeaponComponentProtocolType(Culsel);
                        if (strProtocolType == "")
                        {
                            continue;
                        }

                        if (strProtocolType == WEAPONSYSTEM_VEHICLE_PROTOCOLTYPE_XY)
                        {
                            UpdateXYTrackInfoToListControl(Culsel, &iter_list);
                        }
                        else if (strProtocolType == WEAPONSYSTEM_VEHICLE_PROTOCOLTYPE_XY_54)
                        {

                            {
                                int nCount = ui->tabWidget->count();
                                if (Culsel > (nCount + 1))
                                {
                                    return false;
                                }

                                QWidget *p = ui->tabWidget->widget(Culsel - 1);
                                if (p->layout()->count() <= 0)
                                {
                                    return false;
                                }

                                QLayoutItem *it = p->layout()->itemAt(0);
                                QTableWidget *pTabCtrl = qobject_cast<QTableWidget *>(it->widget());

                                std::vector<WndRoutePoint>::iterator itr = (itrPoint);
                                if (itr != (*(iter_list))->vecNebulaRoutePoint.end())
                                {
                                    const int m_No = itr->nPointID - 1;
                                    const QString Longitude = QString::number(itr->stNebulaRoutePoint.GetLongitude(), 'f', 7);
                                    const QString Latitude = QString::number(itr->stNebulaRoutePoint.GetLatitude(), 'f', 7);
                                    const QString localdistance = QString::number(itr->dLocalDistance, 'f', 3);
                                    if (pTabCtrl->item(m_No, 1))
                                    {
                                        pTabCtrl->item(m_No, 1)->setText(Longitude);
                                        pTabCtrl->item(m_No, 2)->setText(Latitude);
                                        pTabCtrl->item(m_No, 4)->setText(localdistance);
                                    }
                                }

                                if ((itr + 1) != (*routeLineInfolist)->vecNebulaRoutePoint.end())
                                {
                                    const int m_No = (itr + 1)->nPointID - 1;
                                    const QString localdistance = QString::number((itr + 1)->dLocalDistance, 'f', 3);
                                    if (pTabCtrl->item(m_No, 4))
                                    {
                                        pTabCtrl->item(m_No, 4)->setText(localdistance);
                                    }
                                }
                            }
                        }
                        else if (strProtocolType == WEAPONSYSTEM_VEHICLE_PROTOCOLTYPE_MAVLINK)
                        {
                            UpdateMavTrackInfoToListControl(&iter_list);
                        }
                        else if (strProtocolType == WEAPONSYSTEM_VEHICLE_PROTOCOLTYPE_DJI)
                        {
                            UpdateXYTrackInfoToListControl(Culsel, &iter_list);
                        }
                        else
                        {

                        }

                        UpdateLineLength((*iter_list)->vecNebulaRoutePoint);
                    }
                }
                m_stWndRouteLineInfoList.unlock();
            }
            LY_TaskManage::Get().Draw3DLinesWeaponID((*routeLineInfolist)->strWeaponID);
            return true;
        }

        dOldLon = itrPoint->stNebulaRoutePoint.GetLongitude();
        dOldLat = itrPoint->stNebulaRoutePoint.GetLatitude();
    }
    m_stWndRouteLineInfoList.unlock();

    return false;
}

void CDlgEditLine::SetSelectedStates(int nLineID, int nPointID)
{
    int nCul = ui->tabWidget->currentIndex();

    if (nLineID != (nCul + 1))
    {
        ui->tabWidget->setCurrentIndex(nLineID - 1);
        nCul = nLineID - 1;
    }

    WndRouteLineInfoList::iterator routeLineInfolist = m_stWndRouteLineInfoList.m_vecRoutePointInfo.begin();
    if ((routeLineInfolist + nCul) >= m_stWndRouteLineInfoList.m_vecRoutePointInfo.end())
    {
        return ;
    }

    routeLineInfolist += nCul;
    m_stWndRouteLineInfoList.lock();
    for (auto &itrPoint : (*routeLineInfolist)->vecNebulaRoutePoint)
    {
        if (itrPoint.nPointID == nPointID)
        {
            itrPoint.m_bSelected = true;
            m_stWndRouteLineInfoList.unlock();
            return ;
        }
    }
    m_stWndRouteLineInfoList.unlock();
    return ;
}

void CDlgEditLine::ClearSelectedStates()
{
    m_stWndRouteLineInfoList.lock();
    WndRouteLineInfoList::iterator iter_list = m_stWndRouteLineInfoList.m_vecRoutePointInfo.begin();
    for (; iter_list != m_stWndRouteLineInfoList.m_vecRoutePointInfo.end(); iter_list++)
    {
        WndRouteLineInfo *pListInfo = *iter_list;
        for (auto &lp : pListInfo->vecNebulaRoutePoint)
        {
            lp.m_bSelected = false;
        }
    }
    m_stWndRouteLineInfoList.unlock();
}

void CDlgEditLine::UpdateFromTableCombobox(int nRow, int nCol, int nIndex)
{
    int nTabCul = ui->tabWidget->currentIndex();
    std::string strProtocolType = GetCurWeaponComponentProtocolType(nTabCul + 1);

    WndRouteLineInfoList::iterator routeLineInfolist = m_stWndRouteLineInfoList.m_vecRoutePointInfo.begin();
    if ((routeLineInfolist + nTabCul) >= m_stWndRouteLineInfoList.m_vecRoutePointInfo.end())
    {
        return;
    }

    routeLineInfolist += nTabCul;
    m_stWndRouteLineInfoList.lock();
    for (auto &itrPoint : (*routeLineInfolist)->vecNebulaRoutePoint)
    {
        if (itrPoint.nPointID == (nRow + 1))
        {
            if (strProtocolType == WEAPONSYSTEM_VEHICLE_PROTOCOLTYPE_XY_54)
            {
                switch (nCol)
                {
                case 8:
                {
                    if (nIndex == 0)
                    {
                        itrPoint.stNebulaRoutePoint.SetHeightCtrl(CNebulaRoutePoint::EHeightCtrl_Independent);
                    }
                    else if (nIndex == 1)
                    {
                        itrPoint.stNebulaRoutePoint.SetHeightCtrl(CNebulaRoutePoint::EHeightCtrl_HightPreferred);
                    }
                    else if (nIndex == 2)
                    {
                        itrPoint.stNebulaRoutePoint.SetHeightCtrl(CNebulaRoutePoint::EHeightCtrl_SlashCtrl);
                    }
                    else if (nIndex == 3)
                    {
                        itrPoint.stNebulaRoutePoint.SetHeightCtrl(CNebulaRoutePoint::EHeightCtrl_Keep);
                    }
                    break;
                }
                case 11:
                {
                    if (nIndex == 0)
                    {
                        itrPoint.stNebulaRoutePoint.SetLineProPerTy(CNebulaRoutePoint::ELineProPerTy::ELineProPerTy_Whole);
                    }
                    else if (nIndex == 1)
                    {
                        itrPoint.stNebulaRoutePoint.SetLineProPerTy(CNebulaRoutePoint::ELineProPerTy::ELineProPerTy_Alone);
                    }
                    break;
                }
                case 12:
                {
                    if (nIndex == 0)
                    {
                        itrPoint.stNebulaRoutePoint.SetTurnMode(CNebulaRoutePoint::ETurnMode::ETurnMode_Precipice);
                    }
                    else if (nIndex == 1)
                    {
                        itrPoint.stNebulaRoutePoint.SetTurnMode(CNebulaRoutePoint::ETurnMode::ETaskFlag_Tangent);
                    }
                    break;
                }
                default:
                    break;
                }
            }
            else if (strProtocolType == WEAPONSYSTEM_VEHICLE_PROTOCOLTYPE_XY)
            {
                switch (nCol)
                {
                case 8:
                {
                    if (nIndex == 0)
                    {
                        itrPoint.stNebulaRoutePoint.SetHeightCtrl(CNebulaRoutePoint::EHeightCtrl_Independent);
                    }
                    else if (nIndex == 1)
                    {
                        itrPoint.stNebulaRoutePoint.SetHeightCtrl(CNebulaRoutePoint::EHeightCtrl_HightPreferred);
                    }
                    else if (nIndex == 2)
                    {
                        itrPoint.stNebulaRoutePoint.SetHeightCtrl(CNebulaRoutePoint::EHeightCtrl_SlashCtrl);
                    }
                    else if (nIndex == 3)
                    {
                        itrPoint.stNebulaRoutePoint.SetHeightCtrl(CNebulaRoutePoint::EHeightCtrl_Keep);
                    }
                    break;
                }
                case 10:
                {
                    if (nIndex == 0)
                    {
                        itrPoint.mInvestigationSetting = Investigat_Lock;
                    }
                    else if (nIndex == 1)
                    {
                        itrPoint.mInvestigationSetting = Investigat_Scan;
                    }
                    else if (nIndex == 2)
                    {
                        itrPoint.mInvestigationSetting = Investigat_Follow;
                    }
                    else if (nIndex == 3)
                    {
                        itrPoint.mInvestigationSetting = Investigat_Infrared;
                    }
                    break;
                }
                case 11:
                {
                    if (nIndex == 0)
                    {
                        itrPoint.mFirepowerSetting = Firepower_Lock;
                    }
                    else if (nIndex == 1)
                    {
                        itrPoint.mFirepowerSetting = Firepower_Unlock;
                    }
                    else if (nIndex == 2)
                    {
                        itrPoint.mFirepowerSetting = Firepower_Fire;
                    }
                    break;
                }
                default:
                    break;
                }
            }
            break;
        }
    }
    m_stWndRouteLineInfoList.unlock();
}

void CDlgEditLine::UpdateFromTableText(int nRow, int nCol, QString str)
{
    int nTabCul = ui->tabWidget->currentIndex();
    std::string strProtocolType = GetCurWeaponComponentProtocolType(nTabCul + 1);

    WndRouteLineInfoList::iterator routeLineInfolist = m_stWndRouteLineInfoList.m_vecRoutePointInfo.begin();
    if ((routeLineInfolist + nTabCul) >= m_stWndRouteLineInfoList.m_vecRoutePointInfo.end())
    {
        return;
    }

    routeLineInfolist += nTabCul;

    std::string strWeaponID("");

    m_stWndRouteLineInfoList.lock();
    for (auto &itrPoint : (*routeLineInfolist)->vecNebulaRoutePoint)
    {
        if (itrPoint.nPointID == (nRow + 1))
        {

            if (nCol == 1)
            {
                strWeaponID = (*routeLineInfolist)->strWeaponID;
                itrPoint.stNebulaRoutePoint.SetLongitude(str.toDouble());
            }

            else if (nCol == 2)
            {
                strWeaponID = (*routeLineInfolist)->strWeaponID;

                itrPoint.stNebulaRoutePoint.SetLatitude(str.toDouble());
            }

            else if (nCol == 3)
            {
                strWeaponID = (*routeLineInfolist)->strWeaponID;
                itrPoint.stNebulaRoutePoint.SetHeight(str.toInt());
            }

            else if (nCol == 6)
            {
                itrPoint.stNebulaRoutePoint.SetSpeed(str.toInt());
            }

            else if (nCol == 7)
            {
                itrPoint.stNebulaRoutePoint.SetHoverTime(str.toInt());
            }
            if (strProtocolType == WEAPONSYSTEM_VEHICLE_PROTOCOLTYPE_XY_54 && nCol == 10)
            {
                itrPoint.stNebulaRoutePoint.SetRadius(str.toInt());
            }

            break;
        }
    }
    m_stWndRouteLineInfoList.unlock();

    if (!strWeaponID.empty())
    {
        LY_TaskManage::Get().Draw3DLinesWeaponID(strWeaponID);
    }
}

void CDlgEditLine::UpdateTableConnect(bool bStates)
{

    int nCount = ui->tabWidget->count();
    for (int i = 0; i < nCount; i++)
    {
        QWidget *p = ui->tabWidget->widget(i);
        if (p->layout()->count() > 0)
        {
            QLayoutItem *it = p->layout()->itemAt(0);
            QTableWidget *pTabCtrl = qobject_cast<QTableWidget *>(it->widget());
            if (bStates == true)
            {
                connect(pTabCtrl, SIGNAL(cellChanged(int, int)), this, SLOT(on_tableWidget_cellChanged(int, int)));
            }
            else
            {
                pTabCtrl->disconnect();
                TabWidgetConnet(ui->tabWidget->currentIndex() + 1);
            }
        }
    }
}

WndRouteLineInfo *CDlgEditLine::GetLineInfoByID(std::string strID)
{
    for (auto &line : m_stWndRouteLineInfoList.m_vecRoutePointInfo)
    {
        if (line->strWeaponID == strID)
        {
            return line;
        }
    }
    return nullptr;
}

std::string CDlgEditLine::GetLineMarkByID(std::string strID)
{
    for (auto &line : m_stWndRouteLineInfoList.m_vecRoutePointInfo)
    {
        if (line->strWeaponID == strID)
        {
            return line->strMarkID;
        }
    }
    return nullptr;
}

void CDlgEditLine::UpdateLineLength(std::vector<WndRoutePoint> &vecNebulaRoutePoint)
{
    double ddis = 0;
    for (auto itr : vecNebulaRoutePoint)
    {
        ddis += itr.dLocalDistance;
    }
    ui->label_14->setText(QString::number(ddis, 'f', 2));
}

void CDlgEditLine::TableWidgetDisconnectCellChanged(int nCul)
{
    int nCount = ui->tabWidget->count();
    if (nCul > (nCount + 1))
    {
        return;
    }

    QWidget *p = ui->tabWidget->widget(nCul - 1);
    if (p->layout()->count() <= 0)
    {
        return;
    }

    QLayoutItem *it = p->layout()->itemAt(0);
    QTableWidget *pTabCtrl = qobject_cast<QTableWidget *>(it->widget());
    pTabCtrl->disconnect(SIGNAL(cellChanged(int, int)));
}

void CDlgEditLine::TableWidgetConnectCellChanged(int nCul)
{
    int nCount = ui->tabWidget->count();
    if (nCul > (nCount + 1))
    {
        return;
    }

    QWidget *p = ui->tabWidget->widget(nCul - 1);
    if (p->layout()->count() <= 0)
    {
        return;
    }

    QLayoutItem *it = p->layout()->itemAt(0);
    QTableWidget *pTabCtrl = qobject_cast<QTableWidget *>(it->widget());

    connect(pTabCtrl, SIGNAL(cellChanged(int, int)), this, SLOT(on_tableWidget_cellChanged(int, int)));
}

void CDlgEditLine::UpdataDataFormServer()
{
    if (m_pITaskManageService == nullptr)
    {
        return;
    }

    m_MissionVec.clear();

    m_MissionVec = m_pITaskManageService->GetActivatingMissions();
}

void CDlgEditLine::UpdateMissionCombobox()
{
    ui->comboBox->disconnect();
    ui->comboBox->clear();

    int nNo = 0;
    for (auto itr : m_MissionVec)
    {
        ui->comboBox->insertItem(nNo, QString::fromStdString(itr.GetID()));
        nNo++;
    }
    connect(ui->comboBox, SIGNAL(currentIndexChanged(const QString)), this, SLOT(on_comboBox_currentIndexChanged(const QString)));

    bool findConfigSetting = false;
    if (nNo > 0)
    {
        QString strActivateID("");
        QString strActivateMissionID("");

        QSettings settings("../data/conf/TaskManageConfigSetting.ini", QSettings::IniFormat);
        settings.setIniCodec(QTextCodec::codecForName("utf-8"));
        settings.beginGroup("LY_TASKMANAGE_DLGEDITLINE_ACTIVAT_MISSION_SETTINGS");

        if (settings.contains("LY_TASKMANAGE_DLGEDITLINE_ACTIVATEID_SETTINGS") && settings.value("LY_TASKMANAGE_DLGEDITLINE_ACTIVATEID_SETTINGS").canConvert(QVariant::Type::String))
        {
            strActivateID    = settings.value("LY_TASKMANAGE_DLGEDITLINE_ACTIVATEID_SETTINGS", "").toString();
        }

        if (settings.contains("LY_TASKMANAGE_DLGEDITLINE_ACTIVATEMISSIONID_SETTINGS") && settings.value("LY_TASKMANAGE_DLGEDITLINE_ACTIVATEMISSIONID_SETTINGS").canConvert(QVariant::Type::String))
        {
            strActivateMissionID = settings.value("LY_TASKMANAGE_DLGEDITLINE_ACTIVATEMISSIONID_SETTINGS", "").toString();
        }

        settings.setValue("LY_TASKMANAGE_DLGEDITLINE_ACTIVATEID_SETTINGS", strActivateID);
        settings.setValue("LY_TASKMANAGE_DLGEDITLINE_ACTIVATEMISSIONID_SETTINGS", strActivateMissionID);
        settings.endGroup();

        QString strSelectedActivatingActionIDLocal = QString::number(m_pITaskManageService->GetSelectedActivatingActionIDLocal());
        if (strActivateID == strSelectedActivatingActionIDLocal)
        {
            int findTextIndex = ui->comboBox->findText(strActivateMissionID);
            if (findTextIndex > 0)
            {
                findConfigSetting = true;
                ui->comboBox->setCurrentIndex(findTextIndex);
            }
        }
    }

    if (!findConfigSetting)
    {
        ui->comboBox->setCurrentIndex(0);
    }
}

void CDlgEditLine::UpdateWindowData(const std::string strMissionID)
{
    if (m_pITaskManageService == nullptr)
    {
        return;
    }

    m_stWndRouteLineInfoList.lock();
    m_stWndRouteLineInfoList.m_vecRoutePointInfo.clear();

    CNeubulaMissionLineVec LineVec = m_pITaskManageService->GetMissionLinesByTask(strMissionID);

    if (LineVec.size() == 0)
    {
        m_stWndRouteLineInfoList.unlock();
        return;
    }

    std::vector<std::string> reStrs;
    reStrs = m_pITaskManageService->readPlatFormMatch(strMissionID);

    if (reStrs.size() == 0)
    {
        m_stWndRouteLineInfoList.unlock();
        return;
    }

    int nLineID(0), colorIndex(0);
    auto itrPlat = reStrs.begin();
    auto itrPlat2 = reStrs.begin();
    for (auto line : LineVec)
    {

        WndRouteLineInfo *pPoint = new WndRouteLineInfo;
        pPoint->bFormState = false;

        if (nLineID < reStrs.size())
        {

            itrPlat2 = itrPlat + nLineID;
        }
        else
        {
            continue;
        }

        const QString strIDName = tr2(line.strName);
        pPoint->strID = strIDName.toStdString();
        pPoint->strWeaponID = *itrPlat2;
        pPoint->nLineID = nLineID + 1;
        pPoint->nMissionLineID = line.nID;
        nLineID++;
        pPoint->nID = nLineID;

        auto itrUAV = m_SystemSysNameAndID.find(pPoint->strWeaponID);
        if (itrUAV != m_SystemSysNameAndID.end())
        {
            pPoint->strWeaponName = itrUAV->second;
        }

        if (colorIndex >= sizeof(m_nColor) / sizeof(m_nColor[0]))
        {
            colorIndex = 0;
        }

        pPoint->nColor = m_nColor[colorIndex];
        colorIndex++;

        int nPointNo = 1 ;
        double dLonTmp(0), dLatTmp(0);
        for (auto LinePoint : line.LinePointVec)
        {
            WndRoutePoint temWndRoutePoint;
            temWndRoutePoint.nPointID = nPointNo;

            CNebulaRoutePoint stNebulaRoutePoint;
            stNebulaRoutePoint.SetLongitude(LinePoint.dLon);
            stNebulaRoutePoint.SetLatitude(LinePoint.dLat);
            stNebulaRoutePoint.SetHeight(LinePoint.dAlt);
            stNebulaRoutePoint.SetHeightCtrl((CNebulaRoutePoint::EHeightCtrl)LinePoint.cHeightCtrl);
            stNebulaRoutePoint.SetLandFlag((CNebulaRoutePoint::ELandFlag)LinePoint.cLanding);
            stNebulaRoutePoint.SetSpeed(LinePoint.dSpeed);
            stNebulaRoutePoint.SetHoverTime(LinePoint.dHoverTime);
            stNebulaRoutePoint.SetRadius(LinePoint.DRadius);
            stNebulaRoutePoint.SetLineProPerTy((CNebulaRoutePoint::ELineProPerTy)LinePoint.cLineMode);
            stNebulaRoutePoint.SetTurnMode((CNebulaRoutePoint::ETurnMode)LinePoint.cTurnMode);

            if (nPointNo == 1)
            {
                temWndRoutePoint.dLocalDistance = 0;
                dLonTmp = LinePoint.dLon;
                dLatTmp = LinePoint.dLat;
            }
            else
            {
                double distance = get_distance_from_lat_lon(LinePoint.dLat, LinePoint.dLon, dLatTmp, dLonTmp);
                temWndRoutePoint.dLocalDistance = distance;
                dLonTmp = LinePoint.dLon;
                dLatTmp = LinePoint.dLat;
            }
            temWndRoutePoint.stNebulaRoutePoint = stNebulaRoutePoint;
            pPoint->vecNebulaRoutePoint.push_back(temWndRoutePoint);

            nPointNo++;
        }
        m_stWndRouteLineInfoList.m_vecRoutePointInfo.push_back(pPoint);
    }
    m_stWndRouteLineInfoList.unlock();
}

void CDlgEditLine::UpdateTabControl()
{
    int count = 0;
    m_Ctrl_Table_Tabnum = -1;
    ui->tabWidget->clear();
    WndRouteLineInfoList::iterator iter_list = m_stWndRouteLineInfoList.m_vecRoutePointInfo.begin();
    for (; iter_list != m_stWndRouteLineInfoList.m_vecRoutePointInfo.end(); iter_list++)
    {
        auto itrUAV = m_SystemSysNameAndID.find((*iter_list)->strWeaponID);
        if (itrUAV == m_SystemSysNameAndID.end())
        {
            continue;
        }
        QString strID = QString::fromStdString(itrUAV->second) + ":" + QString::fromStdString((*iter_list)->strID);
        count = (*iter_list)->nID;
        if (count > 0)
        {

            QWidget *tab = new QWidget();
            QHBoxLayout *horizontalLayout_2 = new QHBoxLayout(tab);

            QTableWidget *tableWidget_2 = new QTableWidget(tab);
            horizontalLayout_2->addWidget(tableWidget_2);
            ui->tabWidget->addTab(tab, strID);

            UpdateTrackInfoToListHeading(count);
            UpdateTrackInfoToListControl(count);
        }
    }
}

void CDlgEditLine::localWeaponNameBindToId()
{
    m_SystemSysNameAndID.clear();
    qnzkna::WeaponManage::WeaponSystemMap weaponSystemMap;
    m_pIWeaponManageService->GetWeaponSystemMap(weaponSystemMap);
    for (auto sysItr : weaponSystemMap)
    {
        if (sysItr.second.getSystemType() == qnzkna::WeaponManage::WeaponSystem_Type::WeaponSystem_Type_UAV)
        {
            auto weaponComPonentMap = sysItr.second.GetWeaponComponentMapPtr();
            for (auto weapon : *weaponComPonentMap)
            {
                m_SystemSysNameAndID.insert(std::make_pair(weapon.second.getComponentID(), weapon.second.getComponentName()));
            }
        }
    }
}

void CDlgEditLine::on_pushButton_clicked()
{
    int nCul = ui->tabWidget->currentIndex();

    if (m_stWndRouteLineInfoList.m_vecRoutePointInfo.size() == 0)
    {
        return;
    }

    WndRouteLineInfoList::iterator routeLineInfolist = m_stWndRouteLineInfoList.m_vecRoutePointInfo.begin();
    if ((routeLineInfolist + nCul) >= m_stWndRouteLineInfoList.m_vecRoutePointInfo.end())
    {
        return;
    }

    routeLineInfolist += nCul;
    int nHeight = ui->spinBox->text().toInt();
    if (nHeight < 0)
    {
        return;
    }

    m_stWndRouteLineInfoList.lock();
    for (auto &itrPoint : (*routeLineInfolist)->vecNebulaRoutePoint)
    {
        itrPoint.stNebulaRoutePoint.SetHeight(nHeight);
    }

    m_stWndRouteLineInfoList.unlock();
    UpdateTrackInfoToListControl(nCul + 1);
}

void CDlgEditLine::on_pushButton_7_clicked()
{
    if (m_stWndRouteLineInfoList.m_vecRoutePointInfo.size() == 0)
    {
        return;
    }

    int nCul = ui->tabWidget->currentIndex();

    WndRouteLineInfoList::iterator routeLineInfolist = m_stWndRouteLineInfoList.m_vecRoutePointInfo.begin();
    if ((routeLineInfolist + nCul) >= m_stWndRouteLineInfoList.m_vecRoutePointInfo.end())
    {
        return;
    }

    routeLineInfolist += nCul;
    int nSpeed = ui->spinBox_4->text().toInt();
    if (nSpeed < 0)
    {
        return;
    }

    m_stWndRouteLineInfoList.lock();
    for (auto &itrPoint : (*routeLineInfolist)->vecNebulaRoutePoint)
    {
        itrPoint.stNebulaRoutePoint.SetSpeed(nSpeed);
    }

    m_stWndRouteLineInfoList.unlock();
    UpdateTrackInfoToListControl(nCul + 1);
}

void CDlgEditLine::on_pushButton_2_clicked()
{
    if (m_stWndRouteLineInfoList.m_vecRoutePointInfo.size() == 0)
    {
        return;
    }

    int nCul = ui->tabWidget->currentIndex();
    WndRouteLineInfoList::iterator routeLineInfolist = m_stWndRouteLineInfoList.m_vecRoutePointInfo.begin();
    if ((routeLineInfolist + nCul) >= m_stWndRouteLineInfoList.m_vecRoutePointInfo.end())
    {
        return;
    }

    routeLineInfolist += nCul;
    int nTime = ui->spinBox_2->text().toInt();
    if (nTime < 0)
    {
        return;
    }

    m_stWndRouteLineInfoList.lock();
    for (auto &itrPoint : (*routeLineInfolist)->vecNebulaRoutePoint)
    {
        itrPoint.stNebulaRoutePoint.SetHoverTime(nTime);
    }

    m_stWndRouteLineInfoList.unlock();
    UpdateTrackInfoToListControl(nCul + 1);
}

void CDlgEditLine::on_pushButton_3_clicked()
{
    if (m_stWndRouteLineInfoList.m_vecRoutePointInfo.size() == 0)
    {
        return;
    }

    int nCul = ui->tabWidget->currentIndex();
    WndRouteLineInfoList::iterator routeLineInfolist = m_stWndRouteLineInfoList.m_vecRoutePointInfo.begin();
    if ((routeLineInfolist + nCul) >= m_stWndRouteLineInfoList.m_vecRoutePointInfo.end())
    {
        return;
    }

    routeLineInfolist += nCul;
    int nCulcombobox = ui->comboBox_2->currentIndex();
    m_stWndRouteLineInfoList.lock();
    for (auto &itrPoint : (*routeLineInfolist)->vecNebulaRoutePoint)
    {
        CNebulaRoutePoint::EHeightCtrl eCtrl;
        switch (nCulcombobox)
        {
        case 0:
        {
            eCtrl = CNebulaRoutePoint::EHeightCtrl::EHeightCtrl_Independent;
            break;
        }
        case 1:
        {
            eCtrl = CNebulaRoutePoint::EHeightCtrl::EHeightCtrl_HightPreferred;
            break;
        }
        case 2:
        {
            eCtrl = CNebulaRoutePoint::EHeightCtrl::EHeightCtrl_SlashCtrl;
            break;
        }
        case 3:
        {
            eCtrl = CNebulaRoutePoint::EHeightCtrl::EHeightCtrl_Keep;
            break;
        }
        default:
        {
            eCtrl = CNebulaRoutePoint::EHeightCtrl::EHeightCtrl_Independent;
            break;
        }
        }
        itrPoint.stNebulaRoutePoint.SetHeightCtrl(eCtrl);
    }

    m_stWndRouteLineInfoList.unlock();
    UpdateTrackInfoToListControl(nCul + 1);
}

void CDlgEditLine::on_pushButton_4_clicked()
{

}

void CDlgEditLine::on_pushButton_5_clicked()
{

    m_stWndRouteLineInfoList.lock();
    QString strTips = tr("Air Route");
    for (auto itrLine : m_stWndRouteLineInfoList.m_vecRoutePointInfo)
    {
        if (itrLine->nMissionLineID == 0)
        {
            continue;
        }
        CNeubulaMissionLine stLine = m_pITaskManageService->GetMissionLine(itrLine->nMissionLineID);
        stLine.LinePointVec.clear();

        int nNo = m_pITaskManageService->GetMissionLinePointMaxID();
        for (auto itrPoint : itrLine->vecNebulaRoutePoint)
        {
            CNeubulaMissionLinePoint stPoint;
            stPoint.nNo = nNo;
            stPoint.nLineID = stLine.nID;
            stPoint.dLon = itrPoint.stNebulaRoutePoint.GetLongitude();
            stPoint.dLat = itrPoint.stNebulaRoutePoint.GetLatitude();
            stPoint.dAlt = itrPoint.stNebulaRoutePoint.GetHeight();
            stPoint.dSpeed = itrPoint.stNebulaRoutePoint.GetSpeed();
            stPoint.dHoverTime = itrPoint.stNebulaRoutePoint.GetHoverTime();
            stPoint.cHeightCtrl = itrPoint.stNebulaRoutePoint.GetHeightCtrl();
            stPoint.DRadius = itrPoint.stNebulaRoutePoint.GetRadius();
            stPoint.cLineMode = itrPoint.stNebulaRoutePoint.GetLineProPerTy();
            stPoint.cTurnMode = itrPoint.stNebulaRoutePoint.GetTurnMode();
            stPoint.cLanding = itrPoint.stNebulaRoutePoint.GetLandFlag();

            stLine.LinePointVec.push_back(stPoint);
            nNo++;
        }

        bool result =  m_pITaskManageService->EditMissionLine(stLine);
        if (result == false)
        {
            strTips = tr("Air Route(") + QString::number(stLine.nID) + tr(")save failure.");
            LyMessageBox::information(LY_TaskManage::Get().getPWnd(), tr("tip"), strTips);
            m_stWndRouteLineInfoList.unlock();
            return;
        }
        else
        {
            strTips += + "(" + QString::number(stLine.nID) + ")";
        }
    }
    strTips += tr("save successfully.");
    LyMessageBox::information(LY_TaskManage::Get().getPWnd(), tr("tip"), strTips);
    m_stWndRouteLineInfoList.unlock();

}
void CDlgEditLine::on_pushButton_6_clicked()
{
    this->hide();
}

void CDlgEditLine::on_tabWidget_currentChanged(int index)
{

    WndRouteLineInfoList::iterator iter_list = m_stWndRouteLineInfoList.m_vecRoutePointInfo.begin();
    if (index > m_stWndRouteLineInfoList.m_vecRoutePointInfo.size())
    {
        return;
    }
    iter_list += index;
    if (iter_list >= m_stWndRouteLineInfoList.m_vecRoutePointInfo.end())
    {
        return;
    }

    TABITEMNum = index + 1;

    if ((*iter_list)->bLineShowStates == true)
    {
        ui->radioButton->setChecked(true);
        ui->radioButton_2->setChecked(false);
    }
    else
    {
        ui->radioButton->setChecked(false);
        ui->radioButton_2->setChecked(true);
    }
    QString strLineID = QStringLiteral("%1").arg(index + 1, 2, 10, QLatin1Char('0'));
    ui->label_3->setText(strLineID);

    QString tips = QString::fromStdString((*iter_list)->strID) + "(" + QString::fromStdString((*iter_list)->strWeaponID) + ")";
    ui->label_5->setText(tips);

    UpdateLineLength((*iter_list)->vecNebulaRoutePoint);

    TabWidgetConnet(index + 1);

    if (m_bLiseEditStates == true)
    {
        UpdateTableConnect(true);
    }

    UpdateMarkCombobox((*iter_list)->strMarkID);
}

void CDlgEditLine::on_tableWidget_customContextMenuRequested(const QPoint &pos)
{
    QWidget *p = ui->tabWidget->widget(0);
    if (p->layout()->count() <= 0)
    {
        return;
    }

    QLayoutItem *it = p->layout()->itemAt(0);
    QTableWidget *pTabCtrl = qobject_cast<QTableWidget *>(it->widget());

    m_TableMenu->exec(QCursor::pos());

    auto curItem = pTabCtrl->indexAt(pos);
    if (!curItem.isValid())
    {
    }
    else
    {

    }
}

void CDlgEditLine::OnStartuavroute()
{

    m_bLiseEditStates = PickLinePoint;

    int nCount = ui->tabWidget->count();
    for (int i = 0; i < nCount; i++)
    {
        std::string strProtocolType = GetCurWeaponComponentProtocolType(i + 1);
        QWidget *p = ui->tabWidget->widget(i);
        if (p->layout()->count() > 0)
        {
            QLayoutItem *it = p->layout()->itemAt(0);
            QTableWidget *pTabCtrl = qobject_cast<QTableWidget *>(it->widget());
            pTabCtrl->setSelectionBehavior(QTableWidget::SelectItems);
            pTabCtrl->setEditTriggers(QAbstractItemView::DoubleClicked);
            connect(pTabCtrl, SIGNAL(cellChanged(int, int)), this, SLOT(on_tableWidget_cellChanged(int, int)));

            int nRow = pTabCtrl->rowCount();
            for (int i = 0; i < nRow; i++)
            {
                if (strProtocolType == WEAPONSYSTEM_VEHICLE_PROTOCOLTYPE_XY)
                {
                    QWidget *wid08 = (QComboBox *)pTabCtrl->cellWidget(i, 8);
                    QWidget *wid10 = (QComboBox *)pTabCtrl->cellWidget(i, 10);
                    QWidget *wid11 = (QComboBox *)pTabCtrl->cellWidget(i, 11);

                    QComboBox *com08 = (QComboBox *)wid08;
                    QComboBox *com10 = (QComboBox *)wid10;
                    QComboBox *com11 = (QComboBox *)wid11;

                    com08->setEnabled(true);
                    com10->setEnabled(true);
                    com11->setEnabled(true);
                }
                else if (strProtocolType == WEAPONSYSTEM_VEHICLE_PROTOCOLTYPE_XY_54)
                {
                    QWidget *wid08 = (QComboBox *)pTabCtrl->cellWidget(i, 8);
                    QWidget *wid10 = (QComboBox *)pTabCtrl->cellWidget(i, 11);
                    QWidget *wid11 = (QComboBox *)pTabCtrl->cellWidget(i, 12);

                    QComboBox *com08 = (QComboBox *)wid08;
                    QComboBox *com10 = (QComboBox *)wid10;
                    QComboBox *com11 = (QComboBox *)wid11;

                    com08->setEnabled(true);
                    com10->setEnabled(true);
                    com11->setEnabled(true);
                }

            }
        }
    }

    ui->pushButton_5->setEnabled(false);
    ui->pushButton_6->setEnabled(false);
}

void CDlgEditLine::OnStopuavroute()
{

    m_bLiseEditStates = PickNormal;

    int nCount = ui->tabWidget->count();
    for (int i = 0; i < nCount; i++)
    {
        std::string strProtocolType = GetCurWeaponComponentProtocolType(i + 1);
        QWidget *p = ui->tabWidget->widget(i);
        if (p->layout()->count() > 0)
        {
            QLayoutItem *it = p->layout()->itemAt(0);
            QTableWidget *pTabCtrl = qobject_cast<QTableWidget *>(it->widget());
            pTabCtrl->setSelectionBehavior(QTableWidget::SelectRows);
            pTabCtrl->setEditTriggers(QAbstractItemView::NoEditTriggers);
            pTabCtrl->disconnect();
            TabWidgetConnet(ui->tabWidget->currentIndex() + 1);

            int nRow = pTabCtrl->rowCount();
            for (int i = 0; i < nRow; i++)
            {
                if (strProtocolType == WEAPONSYSTEM_VEHICLE_PROTOCOLTYPE_XY)
                {
                    QComboBox *com08 = (QComboBox *)pTabCtrl->cellWidget(i, 8);
                    QComboBox *com10 = (QComboBox *)pTabCtrl->cellWidget(i, 10);
                    QComboBox *com11 = (QComboBox *)pTabCtrl->cellWidget(i, 11);
                    com08->setEnabled(false);
                    com10->setEnabled(false);
                    com11->setEnabled(false);
                }
                else if (strProtocolType == WEAPONSYSTEM_VEHICLE_PROTOCOLTYPE_XY_54)
                {
                    QComboBox *com08 = (QComboBox *)pTabCtrl->cellWidget(i, 8);
                    QComboBox *com10 = (QComboBox *)pTabCtrl->cellWidget(i, 11);
                    QComboBox *com11 = (QComboBox *)pTabCtrl->cellWidget(i, 12);
                    com08->setEnabled(false);
                    com10->setEnabled(false);
                    com11->setEnabled(false);
                }
            }
        }
    }
    ui->pushButton_5->setEnabled(true);
    ui->pushButton_6->setEnabled(true);

}

void CDlgEditLine::OnRemovepoint()
{
    int nCul = ui->tabWidget->currentIndex();
    if (nCul < 0)
    {
        return ;
    }

    QWidget *p = ui->tabWidget->widget(nCul);
    if (p->layout()->count() > 0)
    {
        QLayoutItem *it = p->layout()->itemAt(0);
        QTableWidget *pTabCtrl = qobject_cast<QTableWidget *>(it->widget());
        m_Hitrow = pTabCtrl->currentRow();

        if (m_Hitrow == -1)
        {
            LyMessageBox::information(LY_TaskManage::Get().getPWnd(), tr("Tips"), tr("Please select one to remove"));

            return;
        }

        WndRouteLineInfoList::iterator iter_list = m_stWndRouteLineInfoList.m_vecRoutePointInfo.begin();
        iter_list += nCul;
        if (iter_list >= m_stWndRouteLineInfoList.m_vecRoutePointInfo.end())
        {
            return;
        }

        if (m_Hitrow > (*iter_list)->vecNebulaRoutePoint.size())
        {
            return;
        }

        m_stWndRouteLineInfoList.lock();
        std::vector<WndRoutePoint>::iterator itrP = (*iter_list)->vecNebulaRoutePoint.begin();
        int nNewNo = 1;
        for (; itrP != (*iter_list)->vecNebulaRoutePoint.end();)
        {
            if ((itrP->nPointID - 1) == m_Hitrow)
            {
                itrP = (*iter_list)->vecNebulaRoutePoint.erase(itrP);
                continue;
            }
            itrP->nPointID = nNewNo;
            nNewNo++;
            itrP++;
        }
        m_stWndRouteLineInfoList.unlock();

        UpdateTrackInfoToListControl(nCul + 1);
    }
}

void CDlgEditLine::OnInsetpoint()
{

    int nCul = ui->tabWidget->currentIndex();
    if (nCul < 0)
    {
        return ;
    }

    QWidget *p = ui->tabWidget->widget(nCul);
    if (p->layout()->count() > 0)
    {
        QLayoutItem *it = p->layout()->itemAt(0);
        QTableWidget *pTabCtrl = qobject_cast<QTableWidget *>(it->widget());
        m_Hitrow = pTabCtrl->currentRow();

        if (m_Hitrow == -1)
        {
            LyMessageBox::information(LY_TaskManage::Get().getPWnd(), tr("Tips"), tr("Pleae select where to insert"));
            return;
        }
        LyMessageBox::information(LY_TaskManage::Get().getPWnd(), tr("Tips"), tr("Please pick a point on map"));

        m_bLiseEditStates = PickSinglePoint;

    }
}

void CDlgEditLine::OnAutoqeneration()
{

}

void CDlgEditLine::OnUpload()
{
    StopAllTimer();
    m_RecvXYUAVParameterInfo = true;

    int result = 0;

    int num = ui->tabWidget->currentIndex();
    WndRouteLineInfoList::iterator iter_list = m_stWndRouteLineInfoList.m_vecRoutePointInfo.begin();
    iter_list += num;
    if (iter_list >= m_stWndRouteLineInfoList.m_vecRoutePointInfo.end())
    {
        return;
    }

    std::string    strMarkID = (*iter_list)->strWeaponID;
    qnzkna::WeaponManage::WeaponComponent pWeaponModel;
    if (!m_pIWeaponManageService->GetWeaponComponent(&pWeaponModel, strMarkID.c_str()))
    {
        QString strTip = tr("To %1 require line failed, not found device from device list.").arg(QString::fromStdString(strMarkID));
        ui->label_5->setText(strTip);
        return;
    }

    std::string strProtocolType = pWeaponModel.getProtocolType();

    if (strProtocolType == WEAPONSYSTEM_VEHICLE_PROTOCOLTYPE_XY || strProtocolType == WEAPONSYSTEM_VEHICLE_PROTOCOLTYPE_XY_54)
    {
        result = LyMessageBox::warning(LY_TaskManage::Get().getPWnd(), tr("Tips"), tr("Set last point landing?"), QMessageBox::Yes | QMessageBox::No);
    }

    if (QMessageBox::Yes == result)
    {
        WndRouteLineInfoList::iterator iter_list = m_stWndRouteLineInfoList.m_vecRoutePointInfo.begin();
        if (num > m_stWndRouteLineInfoList.m_vecRoutePointInfo.size())
        {
            return;
        }
        iter_list += num;
        if (iter_list >= m_stWndRouteLineInfoList.m_vecRoutePointInfo.end())
        {
            return;
        }
        std::vector<WndRoutePoint>::iterator itr = (*iter_list)->vecNebulaRoutePoint.begin();
        for (; itr != (*iter_list)->vecNebulaRoutePoint.end(); itr++)
        {
            if (itr + 1 == (*iter_list)->vecNebulaRoutePoint.end())
            {
                itr->stNebulaRoutePoint.SetLandFlag(CNebulaRoutePoint::ELandFlag_Land);
                itr->stNebulaRoutePoint.SetHeight(0);
            }
            else
            {
                itr->stNebulaRoutePoint.SetLandFlag(CNebulaRoutePoint::ELandFlag_Normal);
            }
        }
        UpdateTrackInfoToListControl(num + 1);
    }
    else
    {
        WndRouteLineInfoList::iterator iter_list = m_stWndRouteLineInfoList.m_vecRoutePointInfo.begin();
        if (num > m_stWndRouteLineInfoList.m_vecRoutePointInfo.size())
        {
            return;
        }
        iter_list += num;
        if (iter_list >= m_stWndRouteLineInfoList.m_vecRoutePointInfo.end())
        {
            return;
        }
        std::vector<WndRoutePoint>::iterator itr = (*iter_list)->vecNebulaRoutePoint.begin();
        for (; itr != (*iter_list)->vecNebulaRoutePoint.end(); itr++)
        {
            itr->stNebulaRoutePoint.SetLandFlag(CNebulaRoutePoint::ELandFlag_Normal);
        }
        UpdateTrackInfoToListControl(num + 1);
    }

    ui->label_2->setText("");
    ui->label_3->setText("");
    ui->label_5->setText("");
    ui->label_7->setText("");
    ui->label_9->setText("");
    ui->label_11->setText("");

    ui->label_9->setText("Uploading");

    ui->progressBar->show();
    ui->label_6->show();
    ui->label_7->show();
    ui->label_8->show();
    ui->label_9->show();
    ui->label_10->show();
    ui->label_11->show();
    ui->progressBar->setValue(0);
    ui->tabWidget->setEnabled(false);

    char pBuf[MAX_BUFFER_LEN] = { 0 };
    int nBufLen = 0;
    m_Upload_States = UPLOAD_START;
    m_Upload_Point_No = 0;
    m_Upload_Send_No = 0;

    if (strProtocolType == WEAPONSYSTEM_VEHICLE_PROTOCOLTYPE_XY)
    {
        SetS_xygsJL_09(pBuf, nBufLen, m_Upload_Point_No);
        SendBagData(m_Upload_Point_No + 1, (*iter_list)->strID, (*iter_list)->strWeaponID, pBuf, nBufLen);
        QString strTips1, strTips2, strTips3;

        strTips1 = tr("Uploading");
        strTips2 = tr2("%1 of %2").arg(QString::number(m_Upload_Point_No + 1)).arg(QString::number(m_Upload_PointNum));
        ui->label_7->setText(strTips2);
        ui->label_9->setText(strTips1);
        ui->label_11->setText(QString::number(m_Upload_Send_No));

        StopAllTimer();
        m_RecvXYUAVParameterInfo = true;
        m_DownLoadTimer->stop();
        m_UpLoadTimer->start();

    }
    if (strProtocolType == WEAPONSYSTEM_VEHICLE_PROTOCOLTYPE_XY_54)
    {
        SetS_xygsJL_09_54(pBuf, nBufLen, m_Upload_Point_No);
        SendBagData54(m_Upload_Point_No + 1, (*iter_list)->strID, (*iter_list)->strWeaponID, pBuf, nBufLen);
        QString strTips1, strTips2, strTips3;

        strTips1 = tr("Uploading");
        strTips2 = tr2("%1 of %2").arg(QString::number(m_Upload_Point_No + 1)).arg(QString::number(m_Upload_PointNum));
        ui->label_7->setText(strTips2);
        ui->label_9->setText(strTips1);
        ui->label_11->setText(QString::number(m_Upload_Send_No));

        StopAllTimer();
        m_RecvXYUAVParameterInfo = true;
        m_DownLoadTimer54->stop();
        m_UpLoadTimer54->start();

    }
    else if (strProtocolType == WEAPONSYSTEM_VEHICLE_PROTOCOLTYPE_MAVLINK)
    {

    }
    else if (strProtocolType == WEAPONSYSTEM_VEHICLE_PROTOCOLTYPE_DJI)
    {

    }
}

void CDlgEditLine::OnSaveplanepoint()
{
    StopAllTimer();

    ui->label_2->setText("");
    ui->label_3->setText("");
    ui->label_5->setText("");
    ui->label_7->setText("");
    ui->label_9->setText("");
    ui->label_11->setText("");

    ui->label_9->setText(tr("Downloading"));

    ui->progressBar->show();
    ui->label_6->show();
    ui->label_7->show();
    ui->label_8->show();
    ui->label_9->show();
    ui->label_10->show();
    ui->label_11->show();
    ui->progressBar->setValue(0);

    m_RecvXYUAVParameterInfo = true;

    int num = ui->tabWidget->currentIndex();
    WndRouteLineInfoList::iterator iter_list = m_stWndRouteLineInfoList.m_vecRoutePointInfo.begin();
    iter_list += num;
    if (iter_list >= m_stWndRouteLineInfoList.m_vecRoutePointInfo.end())
    {
        return;
    }

    const qnzkna::SystemConfig::SystemSeatInfo &systemSeatInfo = m_pIConfigProjectControlService->systemSeatInfo();
    qnzkna::NodeManage::NODE_TYPE_UINT nNodeType = systemSeatInfo.SystemNodeType();
    std::string	strNodeId = systemSeatInfo.SystemNodeId();
    std::string strMarkID = (*iter_list)->strWeaponID;
    std::string strXYUAVID, strXYUAVGroupID, strChId;
    if (!m_CXYWeaponManage.GetXYWeaponInfo(strMarkID, strXYUAVGroupID, strXYUAVID, strChId))
    {
        return;
    }

    int nXYUAVGroupID = strtol(strXYUAVGroupID.c_str(), NULL, 16);
    int nXYUAVID = strtol(strXYUAVID.c_str(), NULL, 16);
    qnzkna::WeaponManage::WeaponComponent pWeaponModel;
    if (!m_pIWeaponManageService->GetWeaponComponent(&pWeaponModel, strMarkID.c_str()))
    {
        QString strTip = tr("To %1 require line failed, not found device from device list.").arg(QString::fromStdString(strMarkID));
        ui->label_5->setText(strTip);
        return;
    }

    std::string strProtocolType = pWeaponModel.getProtocolType();
    if (strProtocolType == WEAPONSYSTEM_VEHICLE_PROTOCOLTYPE_XY)
    {
        ui->tabWidget->setEnabled(false);
        OnRemoveallpoint();

        char pBuf[MAX_BUFFER_LEN] = { 0 };
        int nBufLen = 0;
        SetS_xygsJL_17(pBuf, nBufLen, nXYUAVGroupID, nXYUAVID);

        qnzkna::framework::MessageReferenceProps  props;
        props.put("SourceType", std::to_string(nNodeType));
        props.put("SourceID", strNodeId);
        props.put("TargetType", std::to_string(NODE_TYPE_WEAPONCOMPONENT));
        props.put("TargetID", strChId);
        props.put("WeaponID", strMarkID);
        props.put("ProtocolType", WEAPONSYSTEM_VEHICLE_PROTOCOLTYPE_XY);
        ITaskManagerActivator::Get().getIRegistryImpl()->messageNotify(new qnzkna::framework::MessageReference("MsgUAVFlyControlSend", pBuf, nBufLen, props));
        StopAllTimer();
        m_UpLoadTimer->stop();
        m_DownLoadTimeCount    = 0;
        m_DownLoadMaxCount     = 0;
        m_DownLoadTryTimeCount = 0;
        m_RecvXYUAVParameterInfo = true;
        m_DownLoadTimer->start();
    }
    else if (strProtocolType == WEAPONSYSTEM_VEHICLE_PROTOCOLTYPE_XY_54)
    {
        ui->tabWidget->setEnabled(false);
        OnRemoveallpoint();

        char pBuf[MAX_BUFFER_LEN] = { 0 };
        int nBufLen = 0;
        SetS_xygsJL_17(pBuf, nBufLen, nXYUAVGroupID, nXYUAVID);

        qnzkna::framework::MessageReferenceProps  props;
        props.put("SourceType", std::to_string(nNodeType));
        props.put("SourceID", strNodeId);
        props.put("TargetType", std::to_string(NODE_TYPE_WEAPONCOMPONENT));
        props.put("TargetID", strChId);
        props.put("WeaponID", strMarkID);
        props.put("ProtocolType", WEAPONSYSTEM_VEHICLE_PROTOCOLTYPE_XY_54);
        ITaskManagerActivator::Get().getIRegistryImpl()->messageNotify(new qnzkna::framework::MessageReference("MsgUAVFlyControlSend", pBuf, nBufLen, props));
        StopAllTimer();
        m_UpLoadTimer54->stop();
        m_DownLoadTimeCount54    = 0;
        m_DownLoadMaxCount54     = 0;
        m_DownLoadTryTimeCount54 = 0;
        m_RecvXYUAVParameterInfo = true;
        m_DownLoadTimer54->start();
        QString strTip = tr("Start Loading");
        ui->label_5->setText(QString::fromStdString(strMarkID) + " " + strTip);

    }

    else if (strProtocolType == WEAPONSYSTEM_VEHICLE_PROTOCOLTYPE_DJI)
    {
        ui->tabWidget->setEnabled(false);
        OnRemoveallpoint();
    }

    m_update_Flag = 0;
    QString cstrTxt = tr("Require %1 to load points").arg(QString::fromStdString((*iter_list)->strID));
    ui->label_5->setText(QString::fromStdString(strMarkID) + " " + cstrTxt);
}

void CDlgEditLine::OnStraightpoint()
{
    StopAllTimer();
}

void CDlgEditLine::OnUpdateorigin()
{

    return ;
    int nCul = ui->tabWidget->currentIndex();
    QWidget *p = ui->tabWidget->widget(nCul);
    if (p->layout()->count() > 0)
    {
        QLayoutItem *it = p->layout()->itemAt(0);
        QTableWidget *pTabCtrl = qobject_cast<QTableWidget *>(it->widget());
        m_Hitrow = pTabCtrl->currentRow();

        if (m_Hitrow == -1)
        {
            LyMessageBox::information(LY_TaskManage::Get().getPWnd(), tr("Tips"), tr("Please select the point"));
            return;
        }

        WndRouteLineInfoList::iterator iter_list = m_stWndRouteLineInfoList.m_vecRoutePointInfo.begin();
        iter_list += nCul;
        if (iter_list >= m_stWndRouteLineInfoList.m_vecRoutePointInfo.end())
        {
            return;
        }

        if (m_Hitrow > (*iter_list)->vecNebulaRoutePoint.size())
        {
            return;
        }

        std::string strXYUAVID, strXYUAVGroupID, strChId;
        if (!m_CXYWeaponManage.GetXYWeaponInfo((*iter_list)->strWeaponID, strXYUAVGroupID, strXYUAVID, strChId))
        {
            return;
        }

        int nXYUAVGroupID = strtol(strXYUAVGroupID.c_str(), NULL, 16);
        int nXYUAVID = strtol(strXYUAVID.c_str(), NULL, 16);

        xygsJL_17 xygsJL_17_Temp;
        memset(&xygsJL_17_Temp, 0x00, sizeof(xygsJL_14));
        xygsJL_17_Temp.flags = 17;
        xygsJL_17_Temp.groupIndex = nXYUAVGroupID;
        xygsJL_17_Temp.planeIndex = nXYUAVID;
        xygsJL_17_Temp.param.comandMain = 0xF1;
        xygsJL_17_Temp.param.comandSub  = 0x28;
        xygsJL_17_Temp.param.comandExt  = 00;

        if (QMessageBox::Yes == LyMessageBox::warning(LY_TaskManage::Get().getPWnd(), tr2("提示"), tr2("确定更新原点%1吗？").arg(s_xygsJL_14.param.targetPoint), QMessageBox::Yes | QMessageBox::No))
        {

            char pBuf[MAX_BUFFER_LEN] = { 0 };
            int nBufLen = 0;
            nBufLen = sizeof(xygsJL_17_Temp);
            memcpy(pBuf, &xygsJL_17_Temp, nBufLen);

            const qnzkna::SystemConfig::SystemSeatInfo &systemSeatInfo = m_pIConfigProjectControlService->systemSeatInfo();
            qnzkna::NodeManage::NODE_TYPE_UINT nNodeType = systemSeatInfo.SystemNodeType();
            std::string	strNodeId = systemSeatInfo.SystemNodeId();
            std::string strMarkID = (*iter_list)->strWeaponID;

            qnzkna::framework::MessageReferenceProps  props;
            props.put("SourceType", std::to_string(nNodeType));
            props.put("SourceID", strNodeId);
            props.put("TargetType", std::to_string(NODE_TYPE_WEAPONCOMPONENT));
            props.put("TargetID", strChId);
            props.put("WeaponID", strMarkID);
            ITaskManagerActivator::Get().getIRegistryImpl()->messageNotify(new qnzkna::framework::MessageReference("MsgUAVFlyControlSend", pBuf, nBufLen, props));

            if (ui->label_5)
            {
                QString strTip = tr("Send Update origin point cmd to %1").arg(QString::fromStdString((*iter_list)->strID));
                ui->label_5->setText(QString::fromStdString(strMarkID) + " " + strTip);
            }
        }
    }

    StopAllTimer();
}

void CDlgEditLine::OnRemoveallpoint()
{
    StopAllTimer();

    int nCul = ui->tabWidget->currentIndex();
    WndRouteLineInfoList::iterator routeLineInfolist = m_stWndRouteLineInfoList.m_vecRoutePointInfo.begin();
    if ((routeLineInfolist + nCul) >= m_stWndRouteLineInfoList.m_vecRoutePointInfo.end())
    {
        return;
    }

    routeLineInfolist += nCul;
    m_stWndRouteLineInfoList.lock();
    (*routeLineInfolist)->vecNebulaRoutePoint.clear();
    m_stWndRouteLineInfoList.unlock();

    UpdateTrackInfoToListControl(nCul + 1);
}

void CDlgEditLine::OnSetFlayGoal()
{
    int nCul = ui->tabWidget->currentIndex();
    QWidget *p = ui->tabWidget->widget(nCul);
    if (p->layout()->count() > 0)
    {
        QLayoutItem *it = p->layout()->itemAt(0);
        QTableWidget *pTabCtrl = qobject_cast<QTableWidget *>(it->widget());
        m_Hitrow = pTabCtrl->currentRow();

        if (m_Hitrow == -1)
        {
            LyMessageBox::information(LY_TaskManage::Get().getPWnd(), tr("Tips"), tr("Please select the point"));
            return;
        }

        WndRouteLineInfoList::iterator iter_list = m_stWndRouteLineInfoList.m_vecRoutePointInfo.begin();
        iter_list += nCul;
        if (iter_list >= m_stWndRouteLineInfoList.m_vecRoutePointInfo.end())
        {
            return;
        }

        if (m_Hitrow > (*iter_list)->vecNebulaRoutePoint.size())
        {
            return;
        }

        std::string strXYUAVID, strXYUAVGroupID, strChId;
        if (!m_CXYWeaponManage.GetXYWeaponInfo((*iter_list)->strWeaponID, strXYUAVGroupID, strXYUAVID, strChId))
        {
            return;
        }

        int nXYUAVGroupID = strtol(strXYUAVGroupID.c_str(), NULL, 16);
        int nXYUAVID = strtol(strXYUAVID.c_str(), NULL, 16);

        ClearS_xygsJL_14();
        s_xygsJL_14.groupIndex = nXYUAVGroupID;
        s_xygsJL_14.planeIndex = nXYUAVID;
        s_xygsJL_14.param.targetPoint = m_Hitrow + 1;

        if (QMessageBox::Yes == LyMessageBox::warning(LY_TaskManage::Get().getPWnd(), tr2("提示"), tr2("确定工作航点切换到航点%1吗？").arg(s_xygsJL_14.param.targetPoint), QMessageBox::Yes | QMessageBox::No))
        {

            char pBuf[MAX_BUFFER_LEN] = { 0 };
            int nBufLen = 0;
            nBufLen = sizeof(s_xygsJL_14);
            memcpy(pBuf, &s_xygsJL_14, nBufLen);

            const qnzkna::SystemConfig::SystemSeatInfo &systemSeatInfo = m_pIConfigProjectControlService->systemSeatInfo();
            qnzkna::NodeManage::NODE_TYPE_UINT nNodeType = systemSeatInfo.SystemNodeType();
            std::string	strNodeId = systemSeatInfo.SystemNodeId();
            std::string strMarkID = (*iter_list)->strWeaponID;

            qnzkna::framework::MessageReferenceProps  props;
            props.put("SourceType", std::to_string(nNodeType));
            props.put("SourceID", strNodeId);
            props.put("TargetType", std::to_string(NODE_TYPE_WEAPONCOMPONENT));
            props.put("TargetID", strChId);
            props.put("WeaponID", strMarkID);
            ITaskManagerActivator::Get().getIRegistryImpl()->messageNotify(new qnzkna::framework::MessageReference("MsgUAVFlyControlSend", pBuf, nBufLen, props));

            if (ui->label_5)
            {
                QString strTip = tr("Send cutover cmd %2 to %1").arg(QString::fromStdString((*iter_list)->strID)).arg(m_Hitrow + 1);
                ui->label_5->setText(QString::fromStdString(strMarkID) + " " + strTip);
            }
        }
    }
}

void CDlgEditLine::OnLandCurrentVehiclePosition()
{

    m_stWndRouteLineInfoList.lock();
    int nCul = ui->tabWidget->currentIndex();
    QWidget *p = ui->tabWidget->widget(nCul);
    if (p && p->layout() && p->layout()->count() > 0)
    {
        QLayoutItem *it = p->layout()->itemAt(0);
        QTableWidget *pTabCtrl = qobject_cast<QTableWidget *>(it->widget());
        const int Hitrow = pTabCtrl->currentRow();

        if (Hitrow == -1 || Hitrow < 0)
        {
            m_stWndRouteLineInfoList.unlock();
            LyMessageBox::information(LY_TaskManage::Get().getPWnd(), tr("tip"), tr("Please select the waypoint you want to update"));
            return;
        }

        WndRouteLineInfoList::iterator iter_list = m_stWndRouteLineInfoList.m_vecRoutePointInfo.begin();
        iter_list += nCul;
        if (iter_list >= m_stWndRouteLineInfoList.m_vecRoutePointInfo.end())
        {
            m_stWndRouteLineInfoList.unlock();
            return;
        }

        if ((*iter_list)->nLineID != (nCul + 1))
        {
            m_stWndRouteLineInfoList.unlock();
            return;
        }

        if (Hitrow >= (*iter_list)->vecNebulaRoutePoint.size())
        {
            m_stWndRouteLineInfoList.unlock();
            return;
        }

        if ((*iter_list)->vecNebulaRoutePoint.at(Hitrow).nPointID != (Hitrow + 1))
        {
            m_stWndRouteLineInfoList.unlock();
            return;
        }

        qnzkna::UAVRealTimeStateManage::IUAVRealTimeStateManageService *pIUAVRealTimeStateManageService = ITaskManagerActivator::Get().getPIUAVRealTimeStateManageService();
        if (pIUAVRealTimeStateManageService == nullptr)
        {
            m_stWndRouteLineInfoList.unlock();
            return ;
        }

        qnzkna::UAVRealTimeStateManage::RealTimeTelemetryInformation_t teleInfomation;
        if (pIUAVRealTimeStateManageService->GetUAVRealTimeState((*iter_list)->strWeaponID, teleInfomation))
        {
            (*iter_list)->vecNebulaRoutePoint.at(Hitrow).stNebulaRoutePoint.SetLongitude(teleInfomation.telemetryInfo.position.lng);
            (*iter_list)->vecNebulaRoutePoint.at(Hitrow).stNebulaRoutePoint.SetLatitude(teleInfomation.telemetryInfo.position.lat);
            if (Hitrow == ((*iter_list)->vecNebulaRoutePoint.size() - 1))
            {
                (*iter_list)->vecNebulaRoutePoint.at(Hitrow).stNebulaRoutePoint.SetHeight(0);
                (*iter_list)->vecNebulaRoutePoint.at(Hitrow).stNebulaRoutePoint.SetLandFlag(CNebulaRoutePoint::ELandFlag_Land);
            }
            else
            {

                (*iter_list)->vecNebulaRoutePoint.at(Hitrow).stNebulaRoutePoint.SetLandFlag(CNebulaRoutePoint::ELandFlag_Normal);
            }

            m_stWndRouteLineInfoList.unlock();
            UpdateTrackInfoToListControl(nCul + 1);
            return ;
        }
    }

    m_stWndRouteLineInfoList.unlock();
}

void CDlgEditLine::OnEditCodeSet()
{
    CCodeSet_Load_DB stCodeSetDB;
    QString strUAVID = ui->tabWidget->tabText(ui->tabWidget->currentIndex());
    stCodeSetDB.SetPlatformID(strUAVID.toStdString());
    QString strTaskID = tr2("ZC001");
    stCodeSetDB.SetTaskID(strTaskID.toStdString());

    LY_TaskManage::Get().getDlgCodeSet_Load_Edit()->SetShowData(stCodeSetDB);

    if (LY_TaskManage::Get().getDlgCodeSet_Load_Edit()->isVisible())
    {
        LY_TaskManage::Get().getDlgCodeSet_Load_Edit()->hide();
    }
    LY_TaskManage::Get().getDlgCodeSet_Load_Edit()->show();
}

void CDlgEditLine::on_tableWidget_cellChanged(int row, int column)
{
    int nCul = ui->tabWidget->currentIndex();
    QWidget *p = ui->tabWidget->widget(nCul);
    if (p->layout()->count() > 0)
    {
        QLayoutItem *it = p->layout()->itemAt(0);
        QTableWidget *pTabCtrl = qobject_cast<QTableWidget *>(it->widget());
        QTableWidgetItem *pitem = pTabCtrl->item(row, column);
        QString str = pitem->text();
        UpdateFromTableText(row, column, str);
    }
}

void CDlgEditLine::on_combobox_08_currentIndexChanged(int index)
{
    int nCul = ui->tabWidget->currentIndex();
    QWidget *p = ui->tabWidget->widget(nCul);
    if (p->layout()->count() > 0)
    {
        QLayoutItem *it = p->layout()->itemAt(0);
        QTableWidget *pTabCtrl = qobject_cast<QTableWidget *>(it->widget());
        int nRow = pTabCtrl->currentRow();
        int nColum = pTabCtrl->currentColumn();
        UpdateFromTableCombobox(nRow, nColum, index);
    }
}

void CDlgEditLine::on_combobox_10_currentIndexChanged(int index)
{
    int nCul = ui->tabWidget->currentIndex();
    QWidget *p = ui->tabWidget->widget(nCul);
    if (p->layout()->count() > 0)
    {
        QLayoutItem *it = p->layout()->itemAt(0);
        QTableWidget *pTabCtrl = qobject_cast<QTableWidget *>(it->widget());
        int nRow = pTabCtrl->currentRow();
        int nColum = pTabCtrl->currentColumn();
        UpdateFromTableCombobox(nRow, nColum, index);
    }
}

void CDlgEditLine::on_combobox_11_currentIndexChanged(int index)
{
    int nCul = ui->tabWidget->currentIndex();
    QWidget *p = ui->tabWidget->widget(nCul);
    if (p->layout()->count() > 0)
    {
        QLayoutItem *it = p->layout()->itemAt(0);
        QTableWidget *pTabCtrl = qobject_cast<QTableWidget *>(it->widget());
        int nRow = pTabCtrl->currentRow();
        int nColum = pTabCtrl->currentColumn();
        UpdateFromTableCombobox(nRow, nColum, index);
    }
}

void CDlgEditLine::on_radioButton_clicked()
{
    if (ui == nullptr || ui->tabWidget == nullptr  || m_pITaskManageService == nullptr)
    {
        return;
    }

    if (m_stWndRouteLineInfoList.m_vecRoutePointInfo.size() == 0)
    {
        return;
    }

    int nCul = ui->tabWidget->currentIndex();
    if (nCul < 0)
    {
        return;
    }

    WndRouteLineInfoList::iterator routeLineInfolist = m_stWndRouteLineInfoList.m_vecRoutePointInfo.begin();
    if ((routeLineInfolist + nCul) >= m_stWndRouteLineInfoList.m_vecRoutePointInfo.end())
    {
        return;
    }

    routeLineInfolist += nCul;
    m_stWndRouteLineInfoList.lock();
    (*routeLineInfolist)->bLineShowStates = true;
    m_stWndRouteLineInfoList.unlock();

    lineShowStateAll(false);
}

void CDlgEditLine::on_radioButton_2_clicked()
{
    if (ui == nullptr || ui->tabWidget == nullptr || m_pITaskManageService == nullptr)
    {
        return;
    }

    if (m_stWndRouteLineInfoList.m_vecRoutePointInfo.size() == 0)
    {
        return;
    }

    const int nCul = ui->tabWidget->currentIndex();
    if (nCul < 0)
    {
        return;
    }

    WndRouteLineInfoList::iterator routeLineInfolist = m_stWndRouteLineInfoList.m_vecRoutePointInfo.begin();
    if ((routeLineInfolist + nCul) >= m_stWndRouteLineInfoList.m_vecRoutePointInfo.end())
    {
        return;
    }

    routeLineInfolist += nCul;
    m_stWndRouteLineInfoList.lock();
    (*routeLineInfolist)->bLineShowStates = false;
    m_stWndRouteLineInfoList.unlock();

    lineShowStateAll(false);
}

void CDlgEditLine::StopAllTimer()
{

    if (m_UpLoadTimer)
    {
        m_UpLoadTimer->stop();
    }

    if (m_DownLoadTimer)
    {
        m_DownLoadTimer->stop();
    }

    if (m_UpLoadTimer54)
    {
        m_UpLoadTimer54->stop();
    }

    if (m_DownLoadTimer54)
    {
        m_DownLoadTimer54->stop();
    }
}

void CDlgEditLine::on_timeOut()
{
    if (m_Upload_Send_No < 20)
    {
        char pBuf[MAX_BUFFER_LEN] = { 0 };
        int nBufLen = 0;
        m_Upload_Send_No++;
        int num = ui->tabWidget->currentIndex();
        WndRouteLineInfoList::iterator iter_list = m_stWndRouteLineInfoList.m_vecRoutePointInfo.begin();
        iter_list += num;
        if (iter_list >= m_stWndRouteLineInfoList.m_vecRoutePointInfo.end())
        {
            return;
        }
        QString str = tr("Uploading");
        QString str1 = QString::number(m_Upload_Send_No);
        ui->label_11->setText(str1);
        ui->label_9->setText(str);
        SetS_xygsJL_09(pBuf, nBufLen, m_Upload_Point_No);

        SendBagData(m_Upload_Point_No + 1, (*iter_list)->strID, (*iter_list)->strWeaponID, pBuf, nBufLen);

        if (m_Upload_Send_No > 3)
        {
            SendBagData(m_Upload_Point_No + 1, (*iter_list)->strID, (*iter_list)->strWeaponID, pBuf, nBufLen);
        }

        if (m_Upload_Send_No > 6)
        {
            SendBagData(m_Upload_Point_No + 1, (*iter_list)->strID, (*iter_list)->strWeaponID, pBuf, nBufLen);
            SendBagData(m_Upload_Point_No + 1, (*iter_list)->strID, (*iter_list)->strWeaponID, pBuf, nBufLen);
            SendBagData(m_Upload_Point_No + 1, (*iter_list)->strID, (*iter_list)->strWeaponID, pBuf, nBufLen);
        }

        if (m_Upload_Send_No > 9)
        {
            SendBagData(m_Upload_Point_No + 1, (*iter_list)->strID, (*iter_list)->strWeaponID, pBuf, nBufLen);
            SendBagData(m_Upload_Point_No + 1, (*iter_list)->strID, (*iter_list)->strWeaponID, pBuf, nBufLen);
            SendBagData(m_Upload_Point_No + 1, (*iter_list)->strID, (*iter_list)->strWeaponID, pBuf, nBufLen);
        }

        if (m_Upload_Send_No > 12)
        {
            SendBagData(m_Upload_Point_No + 1, (*iter_list)->strID, (*iter_list)->strWeaponID, pBuf, nBufLen);
            SendBagData(m_Upload_Point_No + 1, (*iter_list)->strID, (*iter_list)->strWeaponID, pBuf, nBufLen);
            SendBagData(m_Upload_Point_No + 1, (*iter_list)->strID, (*iter_list)->strWeaponID, pBuf, nBufLen);
        }
    }
    else
    {
        QString str = tr("Upload failed");
        QString str1 = QString::number(m_Upload_Send_No);
        ui->label_11->setText(str1);
        ui->label_9->setText(str);

        QString cstrTxt = tr("Point [%1] upload failed").arg(QString::number(m_Upload_Point_No + 1));
        ui->label_5->setText(cstrTxt);
        m_RecvXYUAVParameterInfo = false;
        StopAllTimer();
        m_UpLoadTimer->stop();

        ui->tabWidget->setEnabled(true);
    }
}

void CDlgEditLine::on_DowntimeOut()
{
    if (m_DownLoadTimeCount <= DOWN_TIME_OUT)
    {
        m_DownLoadTimeCount++;
        return;
    }

    int num = ui->tabWidget->currentIndex();
    if (num == -1)
    {
        return;
    }
    WndRouteLineInfoList::iterator iter_list = m_stWndRouteLineInfoList.m_vecRoutePointInfo.begin();
    iter_list += num;
    if (iter_list >= m_stWndRouteLineInfoList.m_vecRoutePointInfo.end())
    {
        return;
    }

    m_DownLoadTryTimeCount++;
    if (m_DownLoadTryTimeCount > 20)
    {
        m_RecvXYUAVParameterInfo = false;
        StopAllTimer();
        m_DownLoadTimer->stop();
        UpdateTrackInfoToListControl(num + 1);
        ui->tabWidget->setEnabled(true);
        if (m_DownLoadMaxCount > 0 && m_DownLoadMaxCount == (*iter_list)->vecNebulaRoutePoint.size())
        {
            ui->label_5->setText(tr("Load Success"));
        }
        else
        {
            ui->label_5->setText(tr("Load failed"));
            LyMessageBox::information(LY_TaskManage::Get().getPWnd(), tr("Tips"), tr("Load failed"));
        }
        return;
    }

    m_DownLoadTimeCount = 0;

    if (m_DownLoadMaxCount > 0 && m_DownLoadMaxCount == (*iter_list)->vecNebulaRoutePoint.size())
    {
        m_RecvXYUAVParameterInfo = false;
        StopAllTimer();
        m_DownLoadTimer->stop();
        UpdateTrackInfoToListControl(num + 1);
        ui->tabWidget->setEnabled(true);
        return;
    }

    int nPointNo = 1;
    for (auto itrPoint : (*iter_list)->vecNebulaRoutePoint)
    {
        if (itrPoint.nPointID != nPointNo)
        {
            ClearS_xygsJL_08();
            s_xygsJL_08.groupIndex        = s_xygsJL_09.groupIndex;
            s_xygsJL_08.planeIndex        = s_xygsJL_09.planeIndex;
            s_xygsJL_08.param.lastPoint   = nPointNo - 1;
            if (nPointNo > 2)
            {
                s_xygsJL_08.param.lastPoint   = nPointNo - 2;
            }
            s_xygsJL_08.param.pointNumber = m_DownLoadMaxCount;

            char pBuf[MAX_BUFFER_LEN] = { 0 };
            int nBufLen = sizeof(s_xygsJL_08);

            memcpy(pBuf, &s_xygsJL_08, nBufLen);

            const qnzkna::SystemConfig::SystemSeatInfo &systemSeatInfo = m_pIConfigProjectControlService->systemSeatInfo();
            qnzkna::NodeManage::NODE_TYPE_UINT nNodeType = systemSeatInfo.SystemNodeType();
            std::string	strNodeId = systemSeatInfo.SystemNodeId();
            std::string strMarkID = (*iter_list)->strWeaponID;

            std::string strChId;
            if (!m_CXYWeaponManage.GetSendChannel(strMarkID, strChId))
            {
                return;
            }

            qnzkna::framework::MessageReferenceProps  props;
            props.put("SourceType", std::to_string(nNodeType));
            props.put("SourceID", strNodeId);
            props.put("TargetType", std::to_string(NODE_TYPE_WEAPONCOMPONENT));
            props.put("TargetID", strChId);
            props.put("WeaponID", strMarkID);
            props.put("ProtocolType", WEAPONSYSTEM_VEHICLE_PROTOCOLTYPE_XY);
            ITaskManagerActivator::Get().getIRegistryImpl()->messageNotify(new qnzkna::framework::MessageReference("MsgUAVFlyControlSend", pBuf, nBufLen, props));

            if (m_DownLoadTryTimeCount > 3)
            {
                ITaskManagerActivator::Get().getIRegistryImpl()->messageNotify(new qnzkna::framework::MessageReference("MsgUAVFlyControlSend", pBuf, nBufLen, props));
            }
            if (m_DownLoadTryTimeCount > 6)
            {
                ITaskManagerActivator::Get().getIRegistryImpl()->messageNotify(new qnzkna::framework::MessageReference("MsgUAVFlyControlSend", pBuf, nBufLen, props));
            }
            if (m_DownLoadTryTimeCount > 9)
            {
                ITaskManagerActivator::Get().getIRegistryImpl()->messageNotify(new qnzkna::framework::MessageReference("MsgUAVFlyControlSend", pBuf, nBufLen, props));
            }
            if (m_DownLoadTryTimeCount > 12)
            {
                ITaskManagerActivator::Get().getIRegistryImpl()->messageNotify(new qnzkna::framework::MessageReference("MsgUAVFlyControlSend", pBuf, nBufLen, props));
            }
            if (ui->label_5)
            {
                QString strTip = tr("%1 times to %2 require point %3").arg(m_DownLoadTryTimeCount).arg(QString::fromStdString((*iter_list)->strID)).arg(nPointNo);
                ui->label_5->setText(QString::fromStdString(strMarkID) + " " + strTip);
                ui->label_11->setText(QString::number(m_DownLoadTryTimeCount));
            }
            return;
        }
        nPointNo++;
    }

    if ((*iter_list)->vecNebulaRoutePoint.size() == 0)
    {
        const qnzkna::SystemConfig::SystemSeatInfo &systemSeatInfo = m_pIConfigProjectControlService->systemSeatInfo();
        qnzkna::NodeManage::NODE_TYPE_UINT nNodeType = systemSeatInfo.SystemNodeType();
        std::string	strNodeId = systemSeatInfo.SystemNodeId();
        std::string strMarkID = (*iter_list)->strWeaponID;
        std::string strXYUAVID, strXYUAVGroupID, strChId;
        if (!m_CXYWeaponManage.GetXYWeaponInfo(strMarkID, strXYUAVGroupID, strXYUAVID, strChId))
        {
            return;
        }

        int nXYUAVGroupID = strtol(strXYUAVGroupID.c_str(), NULL, 16);
        int nXYUAVID = strtol(strXYUAVID.c_str(), NULL, 16);
        qnzkna::WeaponManage::WeaponComponent pWeaponModel;
        if (!m_pIWeaponManageService->GetWeaponComponent(&pWeaponModel, strMarkID.c_str()))
        {
            QString strTip = tr("To %1 require line failed, not found device from device list.").arg(QString::fromStdString(strMarkID));

            ui->label_5->setText(strTip);
            return;
        }

        char pBuf[MAX_BUFFER_LEN] = { 0 };
        int nBufLen = 0;
        SetS_xygsJL_17(pBuf, nBufLen, nXYUAVGroupID, nXYUAVID);

        qnzkna::framework::MessageReferenceProps  props;
        props.put("SourceType", std::to_string(nNodeType));
        props.put("SourceID", strNodeId);
        props.put("TargetType", std::to_string(NODE_TYPE_WEAPONCOMPONENT));
        props.put("TargetID", strChId);
        props.put("WeaponID", strMarkID);
        props.put("ProtocolType", WEAPONSYSTEM_VEHICLE_PROTOCOLTYPE_XY);
        ITaskManagerActivator::Get().getIRegistryImpl()->messageNotify(new qnzkna::framework::MessageReference("MsgUAVFlyControlSend", pBuf, nBufLen, props));
        m_UpLoadTimer->stop();
        m_DownLoadTimeCount    = 0;
        m_DownLoadMaxCount     = 0;

        QString cstrTxt = tr("Require %1 to load points %2 times").arg(QString::fromStdString((*iter_list)->strID)).arg(m_DownLoadTryTimeCount);
        ui->label_5->setText(QString::fromStdString(strMarkID) + " " + cstrTxt);
    }
}

void CDlgEditLine::on_timeOut54()
{
    if (m_Upload_Send_No < 20)
    {
        char pBuf[MAX_BUFFER_LEN] = { 0 };
        int nBufLen = 0;
        m_Upload_Send_No++;
        int num = ui->tabWidget->currentIndex();
        if (num == -1)
        {
            return;
        }
        WndRouteLineInfoList::iterator iter_list = m_stWndRouteLineInfoList.m_vecRoutePointInfo.begin();
        iter_list += num;
        if (iter_list >= m_stWndRouteLineInfoList.m_vecRoutePointInfo.end())
        {
            return;
        }
        QString str = tr("Uploading");

        QString str1 = QString::number(m_Upload_Send_No);
        ui->label_11->setText(str1);
        ui->label_9->setText(str);
        SetS_xygsJL_09_54(pBuf, nBufLen, m_Upload_Point_No);
        SendBagData54(m_Upload_Point_No + 1, (*iter_list)->strID, (*iter_list)->strWeaponID, pBuf, nBufLen);

        if (m_Upload_Send_No > 3)
        {
            SendBagData54(m_Upload_Point_No + 1, (*iter_list)->strID, (*iter_list)->strWeaponID, pBuf, nBufLen);
        }
        if (m_Upload_Send_No > 6)
        {
            SendBagData54(m_Upload_Point_No + 1, (*iter_list)->strID, (*iter_list)->strWeaponID, pBuf, nBufLen);
            SendBagData54(m_Upload_Point_No + 1, (*iter_list)->strID, (*iter_list)->strWeaponID, pBuf, nBufLen);
        }
        if (m_Upload_Send_No > 9)
        {
            SendBagData54(m_Upload_Point_No + 1, (*iter_list)->strID, (*iter_list)->strWeaponID, pBuf, nBufLen);
            SendBagData54(m_Upload_Point_No + 1, (*iter_list)->strID, (*iter_list)->strWeaponID, pBuf, nBufLen);
            SendBagData54(m_Upload_Point_No + 1, (*iter_list)->strID, (*iter_list)->strWeaponID, pBuf, nBufLen);
        }
        if (m_Upload_Send_No > 12)
        {
            SendBagData54(m_Upload_Point_No + 1, (*iter_list)->strID, (*iter_list)->strWeaponID, pBuf, nBufLen);
            SendBagData54(m_Upload_Point_No + 1, (*iter_list)->strID, (*iter_list)->strWeaponID, pBuf, nBufLen);
            SendBagData54(m_Upload_Point_No + 1, (*iter_list)->strID, (*iter_list)->strWeaponID, pBuf, nBufLen);
        }
    }
    else
    {
        QString str = tr("Upload failed");
        QString str1 = QString::number(m_Upload_Send_No);
        ui->label_11->setText(str1);
        ui->label_9->setText(str);

        QString cstrTxt = tr("Point [%1] upload failed").arg(QString::number(m_Upload_Point_No + 1));
        ui->label_5->setText(cstrTxt);
        m_RecvXYUAVParameterInfo = false;
        StopAllTimer();
        m_UpLoadTimer54->stop();

        ui->tabWidget->setEnabled(true);
    }
}

void CDlgEditLine::on_DowntimeOut54()
{
    if (m_DownLoadTimeCount54 <= DOWN_TIME_OUT)
    {
        m_DownLoadTimeCount54++;
        return;
    }

    int num = ui->tabWidget->currentIndex();
    if (num == -1)
    {
        return;
    }
    WndRouteLineInfoList::iterator iter_list = m_stWndRouteLineInfoList.m_vecRoutePointInfo.begin();
    iter_list += num;
    if (iter_list >= m_stWndRouteLineInfoList.m_vecRoutePointInfo.end())
    {
        return;
    }

    m_DownLoadTryTimeCount54++;
    if (m_DownLoadTryTimeCount54 > 20)
    {
        m_RecvXYUAVParameterInfo = false;
        StopAllTimer();
        m_DownLoadTimer54->stop();
        UpdateTrackInfoToListControl(num + 1);
        ui->tabWidget->setEnabled(true);
        if (m_DownLoadMaxCount54 > 0 && m_DownLoadMaxCount54 == (*iter_list)->vecNebulaRoutePoint.size())
        {
            ui->label_5->setText(tr("Load Success"));
        }
        else
        {
            ui->label_5->setText(tr("Load failed"));
            LyMessageBox::information(LY_TaskManage::Get().getPWnd(), tr("Tips"), tr("Load failed"));
        }
        return;
    }

    m_DownLoadTimeCount54 = 0;

    if (m_DownLoadMaxCount54 > 0 && m_DownLoadMaxCount54 == (*iter_list)->vecNebulaRoutePoint.size())
    {
        m_RecvXYUAVParameterInfo = false;
        StopAllTimer();
        m_DownLoadTimer54->stop();
        UpdateTrackInfoToListControl(num + 1);
        ui->tabWidget->setEnabled(true);
        return;
    }

    int nPointNo = 1;
    for (auto itrPoint : (*iter_list)->vecNebulaRoutePoint)
    {
        if (itrPoint.nPointID != nPointNo)
        {
            ClearS_xygsJL_08_54();
            s_xygsJL_08_54.groupIndex = s_xygsJL_09_54.groupIndex;
            s_xygsJL_08_54.planeIndex = s_xygsJL_09_54.planeIndex;
            s_xygsJL_08_54.param.lastPoint = nPointNo - 1;
            s_xygsJL_08_54.param.pointNumber = m_DownLoadMaxCount54;

            char pBuf[MAX_BUFFER_LEN] = { 0 };
            int nBufLen = 0;

            nBufLen = sizeof(s_xygsJL_08_54);
            memcpy(pBuf, &s_xygsJL_08_54, nBufLen);

            const qnzkna::SystemConfig::SystemSeatInfo &systemSeatInfo = m_pIConfigProjectControlService->systemSeatInfo();
            qnzkna::NodeManage::NODE_TYPE_UINT nNodeType = systemSeatInfo.SystemNodeType();
            std::string	strNodeId = systemSeatInfo.SystemNodeId();
            std::string strMarkID = (*iter_list)->strWeaponID;

            std::string strChId;
            if (!m_CXYWeaponManage.GetSendChannel(strMarkID, strChId))
            {
                return;
            }

            qnzkna::framework::MessageReferenceProps  props;
            props.put("SourceType", std::to_string(nNodeType));
            props.put("SourceID", strNodeId);
            props.put("TargetType", std::to_string(NODE_TYPE_WEAPONCOMPONENT));
            props.put("TargetID", strChId);
            props.put("WeaponID", strMarkID);
            props.put("ProtocolType", WEAPONSYSTEM_VEHICLE_PROTOCOLTYPE_XY_54);
            ITaskManagerActivator::Get().getIRegistryImpl()->messageNotify(new qnzkna::framework::MessageReference("MsgUAVFlyControlSend", pBuf, nBufLen, props));

            if (m_DownLoadTryTimeCount54 > 3)
            {
                ITaskManagerActivator::Get().getIRegistryImpl()->messageNotify(new qnzkna::framework::MessageReference("MsgUAVFlyControlSend", pBuf, nBufLen, props));
            }
            if (m_DownLoadTryTimeCount54 > 6)
            {
                ITaskManagerActivator::Get().getIRegistryImpl()->messageNotify(new qnzkna::framework::MessageReference("MsgUAVFlyControlSend", pBuf, nBufLen, props));
            }
            if (m_DownLoadTryTimeCount54 > 9)
            {
                ITaskManagerActivator::Get().getIRegistryImpl()->messageNotify(new qnzkna::framework::MessageReference("MsgUAVFlyControlSend", pBuf, nBufLen, props));
            }
            if (m_DownLoadTryTimeCount54 > 12)
            {
                ITaskManagerActivator::Get().getIRegistryImpl()->messageNotify(new qnzkna::framework::MessageReference("MsgUAVFlyControlSend", pBuf, nBufLen, props));
            }
            if (ui->label_5)
            {
                QString strTip = QString(tr("%1 times to %2 require point %3")).arg(m_DownLoadTryTimeCount54).arg(QString::fromStdString((*iter_list)->strID)).arg(nPointNo);

                ui->label_5->setText(QString::fromStdString(strMarkID) + " " + strTip);

                ui->label_11->setText(QString::number(m_DownLoadTryTimeCount54));
            }
            return;
        }
        nPointNo++;
    }

    if ((*iter_list)->vecNebulaRoutePoint.size() == 0)
    {
        const qnzkna::SystemConfig::SystemSeatInfo &systemSeatInfo = m_pIConfigProjectControlService->systemSeatInfo();
        qnzkna::NodeManage::NODE_TYPE_UINT nNodeType = systemSeatInfo.SystemNodeType();
        std::string	strNodeId = systemSeatInfo.SystemNodeId();
        std::string strMarkID = (*iter_list)->strWeaponID;
        std::string strXYUAVID, strXYUAVGroupID, strChId;
        if (!m_CXYWeaponManage.GetXYWeaponInfo(strMarkID, strXYUAVGroupID, strXYUAVID, strChId))
        {
            return;
        }

        int nXYUAVGroupID = strtol(strXYUAVGroupID.c_str(), NULL, 16);
        int nXYUAVID = strtol(strXYUAVID.c_str(), NULL, 16);
        qnzkna::WeaponManage::WeaponComponent pWeaponModel;
        if (!m_pIWeaponManageService->GetWeaponComponent(&pWeaponModel, strMarkID.c_str()))
        {
            QString strTip = tr("To %1 require line failed, not found device from device list.").arg(QString::fromStdString(strMarkID));

            ui->label_5->setText(QString::fromStdString(strMarkID) + " " + strTip);
            return;
        }

        char pBuf[MAX_BUFFER_LEN] = { 0 };
        int nBufLen = 0;
        SetS_xygsJL_17(pBuf, nBufLen, nXYUAVGroupID, nXYUAVID);

        qnzkna::framework::MessageReferenceProps  props;
        props.put("SourceType", std::to_string(nNodeType));
        props.put("SourceID", strNodeId);
        props.put("TargetType", std::to_string(NODE_TYPE_WEAPONCOMPONENT));
        props.put("TargetID", strChId);
        props.put("WeaponID", strMarkID);
        props.put("ProtocolType", WEAPONSYSTEM_VEHICLE_PROTOCOLTYPE_XY_54);
        ITaskManagerActivator::Get().getIRegistryImpl()->messageNotify(new qnzkna::framework::MessageReference("MsgUAVFlyControlSend", pBuf, nBufLen, props));
        m_UpLoadTimer54->stop();
        m_DownLoadTimeCount54    = 0;
        m_DownLoadMaxCount54     = 0;

        QString cstrTxt = QString(tr("Require %1 to load points %2 times")).arg(QString::fromStdString((*iter_list)->strID)).arg(m_DownLoadTryTimeCount54);
        ui->label_5->setText(QString::fromStdString(strMarkID) + " " + cstrTxt);
    }
}

void CDlgEditLine::SetDlgMissionList(CDlgMissionList *pDlgList)
{
    m_pDlgMissionList = pDlgList;
    if (m_pDlgMissionList)
    {
        connect(m_pDlgMissionList, &CDlgMissionList::ShowLine, this, &CDlgEditLine::ShowLines);
    }
}

void CDlgEditLine::ShowLines(QStringList &LineList, double &dLon, double &dLat, bool bAgainFlag)
{
    if (LineList.size() == 0)
    {
        return;
    }

    if (m_stWndRouteLineInfoList.m_vecRoutePointInfo.size() == 0)
    {
        return;
    }

    m_stWndRouteLineInfoList.lock();
    for (auto &stLine : m_stWndRouteLineInfoList.m_vecRoutePointInfo)
    {
        bool bShowFlag = false;
        for (auto showline : LineList)
        {
            if (showline.compare(QString::fromStdString(stLine->strID)) == 0)
            {
                bShowFlag = true;
                if (dLat == 0 && dLon == 0)
                {
                    dLon = (stLine->vecNebulaRoutePoint.begin())->stNebulaRoutePoint.GetLongitude();
                    dLat = (stLine->vecNebulaRoutePoint.begin())->stNebulaRoutePoint.GetLatitude();
                }
                break;
            }
        }

        if (bShowFlag == true)
        {
            if (bAgainFlag == true)
            {
                stLine->bLineShowStates = false;
            }
            else
            {
                stLine->bLineShowStates = true;
            }
        }
        else
        {
            stLine->bLineShowStates = false;
        }
    }

    m_stWndRouteLineInfoList.unlock();
}

void CDlgEditLine::UpdateMarkCombobox(std::string strMarkID)
{
    if (m_pIMarkManageService == nullptr)
    {
        return;
    }
    CSystemMarkInfoVec infoVec;

    if (m_pITaskManageService != nullptr)
    {
        int activatingActionID = -1;
        activatingActionID = m_pITaskManageService->GetActivatingActionID();
        CRelationActionMarkVec RelationActionMarkVec = m_pITaskManageService->GetRelationMark(activatingActionID);
        for (auto data : RelationActionMarkVec)
        {
            SystemMarkInfo stinfo = m_pIMarkManageService->GetMark(data.GetMarkID());
            if (stinfo.GetID() != "")
            {
                infoVec.push_back(stinfo);
            }
        }
    }

    if (infoVec.size() == 0)
    {
        infoVec = m_pIMarkManageService->GetMarkList();
    }

    ui->comboBox_3->disconnect();
    ui->comboBox_3->clear();

    QStringList strList;
    strList.push_back(tr2("-"));
    for (auto data : infoVec)
    {
        strList.push_back(QString::fromStdString(data.GetID()));
    }

    ui->comboBox_3->addItems(strList);

    QString strID1 = QString::fromStdString(strMarkID);

    int nIndex01 = ui->comboBox_3->findText(strID1);
    if (nIndex01 != -1)
    {
        ui->comboBox_3->setCurrentIndex(nIndex01);
    }
    connect(ui->comboBox_3, SIGNAL(currentIndexChanged(QString)), this, SLOT(on_comboBox_3_currentIndexChanged(QString)));
}

std::string CDlgEditLine::GetMarkIDString()
{
    return (ui->comboBox_3->currentText()).toStdString();
}

void CDlgEditLine::closeEvent(QCloseEvent *event)
{
    StopAllTimer();

    OnStopuavroute();

    m_RecvXYUAVParameterInfo = false;
}

void CDlgEditLine::showEvent(QShowEvent *event)
{
    ui->tabWidget->setEnabled(true);

    if (ui && ui->tabWidget)
    {
        m_stWndRouteLineInfoList.lock();
        m_stWndRouteLineInfoList.m_vecRoutePointInfo.clear();
        m_stWndRouteLineInfoList.unlock();

        ui->tabWidget->clear();
    }

    UpdataDataFormServer();
    UpdateMissionCombobox();
    QString strMissionID = ui->comboBox->currentText();
    if (strMissionID != "")
    {
        UpdateWindowData(strMissionID.toStdString());
        UpdateTabControl();
        lineShowStateAll(true);
    }
}

void CDlgEditLine::UpdateDataFromActivatingActionIDChange()
{
    StopAllTimer();

    OnStopuavroute();

    m_RecvXYUAVParameterInfo = false;

    if (ui && ui->tabWidget)
    {
        m_stWndRouteLineInfoList.lock();
        m_stWndRouteLineInfoList.m_vecRoutePointInfo.clear();
        m_stWndRouteLineInfoList.unlock();

        ui->tabWidget->clear();
    }

    UpdataDataFormServer();
    UpdateMissionCombobox();

}

void CDlgEditLine::on_comboBox_currentIndexChanged(const QString &arg1)
{
    UpdateWindowData(arg1.toStdString());

    lineShowStateAll(true);

    UpdateTabControl();

    if (m_pITaskManageService != nullptr)
    {
        QString strActivateID = QString::number(m_pITaskManageService->GetSelectedActivatingActionIDLocal());
        QString strActivateMissionID(arg1);

        QSettings settings("../data/conf/TaskManageConfigSetting.ini", QSettings::IniFormat);
        settings.setIniCodec(QTextCodec::codecForName("utf-8"));
        settings.beginGroup("LY_TASKMANAGE_DLGEDITLINE_ACTIVAT_MISSION_SETTINGS");
        settings.setValue("LY_TASKMANAGE_DLGEDITLINE_ACTIVATEID_SETTINGS", strActivateID);
        settings.setValue("LY_TASKMANAGE_DLGEDITLINE_ACTIVATEMISSIONID_SETTINGS", strActivateMissionID);
        settings.endGroup();
    }
}

void CDlgEditLine::on_pushButton_8_clicked()
{
    CDlgEditLine  *pDlgEditLine   = LY_TaskManage::Get().GetDlgEditLine();
    CDlgAutoPoint *pDlgAutoPoint  = LY_TaskManage::Get().GetDlgAutoPoint();
    if (pDlgEditLine && pDlgAutoPoint)
    {
        if (pDlgEditLine->GetLineEditStates() == PickLinePoint || pDlgEditLine->GetLineEditStates() == PickSinglePoint)
        {
            pDlgAutoPoint->SetEditLinePoint(true);
            pDlgAutoPoint->SetUpdatePositionAndMarkIDCal(true);
            pDlgAutoPoint->show();
        }
        else
        {
            pDlgAutoPoint->SetEditLinePoint(false);
            pDlgAutoPoint->SetUpdatePositionAndMarkIDCal(true);
            pDlgAutoPoint->show();
        }
    }
}

void CDlgEditLine::on_pushButton_10_clicked()
{
    if (ui == nullptr || ui->comboBox == nullptr || m_pITaskManageService == nullptr)
    {
        return ;
    }

    ui->radioButton->setChecked(true);
    ui->radioButton_2->setChecked(false);

    if (m_stWndRouteLineInfoList.m_vecRoutePointInfo.size() > 0)
    {
        WndRouteLineInfoList::iterator iter_list = m_stWndRouteLineInfoList.m_vecRoutePointInfo.begin();
        for (; iter_list != m_stWndRouteLineInfoList.m_vecRoutePointInfo.end(); iter_list++)
        {
            (*iter_list)->bLineShowStates = true;
        }
    }

    lineShowStateAll(false);
}

void CDlgEditLine::on_pushButton_11_clicked()
{
    if (ui == nullptr || ui->comboBox == nullptr || m_pITaskManageService == nullptr)
    {
        return ;
    }

    ui->radioButton->setChecked(false);
    ui->radioButton_2->setChecked(true);

    if (m_stWndRouteLineInfoList.m_vecRoutePointInfo.size() > 0)
    {
        WndRouteLineInfoList::iterator iter_list = m_stWndRouteLineInfoList.m_vecRoutePointInfo.begin();
        for (; iter_list != m_stWndRouteLineInfoList.m_vecRoutePointInfo.end(); iter_list++)
        {
            (*iter_list)->bLineShowStates = false;
        }
    }

    lineShowStateAll(false);
}

void CDlgEditLine::lineShowStateAll(bool valueFromSettings)
{
    if (ui == nullptr || ui->comboBox == nullptr || m_pITaskManageService == nullptr)
    {
        return ;
    }

    if (m_stWndRouteLineInfoList.m_vecRoutePointInfo.size() > 0)
    {
        const QString strActivateMissionID(ui->comboBox->currentText());
        if (!strActivateMissionID.isEmpty())
        {
            const QString strActivateID = QString::number(m_pITaskManageService->GetSelectedActivatingActionIDLocal());

            QStringList lineShowStates;

            QSettings settings("../data/conf/TaskManageConfigSetting.ini", QSettings::IniFormat);
            settings.setIniCodec(QTextCodec::codecForName("utf-8"));
            settings.beginGroup("LY_TASKMANAGE_DLGEDITLINE_ACTIVAT_MISSION_SETTINGS");
            if (settings.contains("LY_TASKMANAGE_DLGEDITLINE_ACTIVATEMISSIONID_LINESHOWSTATES_SETTINGS") && settings.value("LY_TASKMANAGE_DLGEDITLINE_ACTIVATEMISSIONID_LINESHOWSTATES_SETTINGS").canConvert(QVariant::Type::StringList))
            {
                lineShowStates = settings.value("LY_TASKMANAGE_DLGEDITLINE_ACTIVATEMISSIONID_LINESHOWSTATES_SETTINGS", "").toStringList();
            }
            settings.endGroup();

            QStringList lineShowStatesTemp;

            if (valueFromSettings)
            {
                WndRouteLineInfoList::iterator iter_list = m_stWndRouteLineInfoList.m_vecRoutePointInfo.begin();
                for (; iter_list != m_stWndRouteLineInfoList.m_vecRoutePointInfo.end(); iter_list++)
                {
                    int missionLineID = (*iter_list)->nMissionLineID;
                    int index = lineShowStates.indexOf(QRegExp(QString::number(missionLineID) + ":.*"));
                    if (index >= 0)
                    {
                        const QStringList lineShowStatesValue = lineShowStates.at(index).split(":");
                        if (lineShowStatesValue.size() == 2 && lineShowStatesValue.at(1) == QStringLiteral("0"))
                        {
                            (*iter_list)->bLineShowStates = false;
                        }
                    }
                    lineShowStatesTemp.append(QString::number(missionLineID) + ":" + QString::number((*iter_list)->bLineShowStates));
                }
            }
            else
            {
                WndRouteLineInfoList::iterator iter_list = m_stWndRouteLineInfoList.m_vecRoutePointInfo.begin();
                for (; iter_list != m_stWndRouteLineInfoList.m_vecRoutePointInfo.end(); iter_list++)
                {
                    int missionLineID = (*iter_list)->nMissionLineID;
                    lineShowStatesTemp.append(QString::number(missionLineID) + ":" + QString::number((*iter_list)->bLineShowStates));
                }
            }

            settings.beginGroup("LY_TASKMANAGE_DLGEDITLINE_ACTIVAT_MISSION_SETTINGS");
            settings.setValue("LY_TASKMANAGE_DLGEDITLINE_ACTIVATEID_SETTINGS",        strActivateID);
            settings.setValue("LY_TASKMANAGE_DLGEDITLINE_ACTIVATEMISSIONID_SETTINGS", strActivateMissionID);
            settings.setValue("LY_TASKMANAGE_DLGEDITLINE_ACTIVATEMISSIONID_LINESHOWSTATES_SETTINGS", lineShowStatesTemp);
            settings.endGroup();
        }
    }
}

void CDlgEditLine::on_pushButton_9_clicked()
{
    if (m_stWndRouteLineInfoList.m_vecRoutePointInfo.size() == 0)
    {
        return;
    }

    if (TABITEMNum == 0)
    {
        return;
    }

    if (TABITEMNum > m_stWndRouteLineInfoList.m_vecRoutePointInfo.size())
    {
        return;
    }

    auto itr = m_stWndRouteLineInfoList.m_vecRoutePointInfo.begin();
    itr += TABITEMNum - 1;

    if ((*itr)->vecNebulaRoutePoint.size() == 0)
    {
        return;
    }

    m_stWndRouteLineInfoList.lock();
    int nCul = ui->tabWidget->currentIndex();
    QWidget *p = ui->tabWidget->widget(nCul);
    if (p && p->layout() && p->layout()->count() > 0)
    {
        QLayoutItem *it = p->layout()->itemAt(0);
        QTableWidget *pTabCtrl = qobject_cast<QTableWidget *>(it->widget());
        const int Hitrow = pTabCtrl->currentRow();

        if (Hitrow == -1 || Hitrow < 0)
        {
            auto itrPoint = (*itr)->vecNebulaRoutePoint.begin();
            double dLong  = itrPoint->stNebulaRoutePoint.GetLongitude();
            double dLat   = itrPoint->stNebulaRoutePoint.GetLatitude();
            LY_TaskManage::Get().SetScreenCenter(dLong, dLat);

            m_stWndRouteLineInfoList.unlock();

            return;
        }

        WndRouteLineInfoList::iterator iter_list = m_stWndRouteLineInfoList.m_vecRoutePointInfo.begin();
        iter_list += nCul;
        if (iter_list >= m_stWndRouteLineInfoList.m_vecRoutePointInfo.end())
        {
            m_stWndRouteLineInfoList.unlock();
            return;
        }

        if ((*iter_list)->nLineID != (nCul + 1))
        {
            m_stWndRouteLineInfoList.unlock();
            return;
        }

        if (Hitrow >= (*iter_list)->vecNebulaRoutePoint.size())
        {
            m_stWndRouteLineInfoList.unlock();
            return;
        }

        if ((*iter_list)->vecNebulaRoutePoint.at(Hitrow).nPointID != (Hitrow + 1))
        {
            m_stWndRouteLineInfoList.unlock();
            return;
        }

        double dLong = (*iter_list)->vecNebulaRoutePoint.at(Hitrow).stNebulaRoutePoint.GetLongitude();
        double dLat  = (*iter_list)->vecNebulaRoutePoint.at(Hitrow).stNebulaRoutePoint.GetLatitude();
        LY_TaskManage::Get().SetScreenCenter(dLong, dLat);
    }

    m_stWndRouteLineInfoList.unlock();

}
