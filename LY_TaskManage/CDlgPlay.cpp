#include "CDlgPlay.h"
#include "ui_CDlgPlay.h"
#include "ITaskManagerActivator.h"
#include "QCString.h"
#include <fstream>
#include <sstream>
#include <QDateTime>
#include <QFile>
#include <QTimer>
#include <QSettings>
#include "LyMessageBox.h"
#include "lY_TaskManage.h"
#include "CDlgPlaySpeedControl.h"
#include "GLES3/gl3.h"

#include <QtMath>
#include "CDlgForecastTendency.h"
#include "DataManage/SystemInfo/SystemInternalInterface.h"
#include "lY_TaskManageInterface.h"
#define RULEUAVSAFETYDIS_UPORDOWN 5
#define UAVMOMENTPOSITION_SAFETY_DIS 30
CDlgPlay::CDlgPlay(QWidget *parent) :
	LyDialog(tr("Analog Playback"), parent),
	ui(new Ui::CDlgPlay)
{
	ui->setupUi(centralWidget());
	this->setResizable(true);
	this->resize(1200, 800);

	ui->progressBar->setValue(0);

	m_PlayTimer = new QTimer(this);
	m_PlayTimer->setInterval(1000 / PLAY_HZ);
	connect(m_PlayTimer, &QTimer::timeout, this, &CDlgPlay::on_timeOut);

	m_LinesTimer = new QTimer(this);
	m_LinesTimer->setInterval(1000 / PLAY_HZ);
	m_LinesTimer->start();
	connect(m_LinesTimer, &QTimer::timeout, this, &CDlgPlay::on_LinesTimeOut);

	connect(ui->BT_LoadData, SIGNAL(clicked()), this, SLOT(on_BT_LoadData_clicked()));
	connect(ui->BT_Play, SIGNAL(clicked()), this, SLOT(on_BT_Play_clicked()));
	connect(ui->BT_Stop, SIGNAL(clicked()), this, SLOT(on_BT_Stop_clicked()));
	connect(ui->BT_decline, SIGNAL(clicked()), this, SLOT(on_BT_decline_clicked()));
	connect(ui->BT_Accelereate, SIGNAL(clicked()), this, SLOT(on_BT_Accelereate_clicked()));
	connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(on_pushButton_clicked()));
	connect(ui->pushButton_2, SIGNAL(clicked()), this, SLOT(on_pushButton_2_clicked()));
	connect(ui->pushButton_3, SIGNAL(clicked()), this, SLOT(on_pushButton_3_clicked()));
	connect(ui->pushButton_4, SIGNAL(clicked()), this, SLOT(on_pushButton_4_clicked()));
	connect(ui->tableWidget, SIGNAL(cellClicked(int, int)), this, SLOT(on_tableWidget_cellClicked(int, int)));
	connect(this, &CDlgPlay::DLGPlayDeviationVoyage, this, &CDlgPlay::DealDLGPlayDeviationVoyage, Qt::QueuedConnection);
	connect(ui->checkBoxLines, &QCheckBox::stateChanged, this, &CDlgPlay::setSelectShowLine);
	connect(this, &CDlgPlay::DLGPlayVoyageEditHeight, this, &CDlgPlay::DealDLGPlayVoyageEditHeight, Qt::QueuedConnection);

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

	m_pIUAVRealTimeStateManageService = ITaskManagerActivator::Get().getPIUAVRealTimeStateManageService();
	if (m_pIUAVRealTimeStateManageService == nullptr)
	{
		return;
	}
	m_pIZoneManageService = ITaskManagerActivator::Get().getIZoneManageService();
	if (m_pIZoneManageService == nullptr)
	{
		return;
	}

	m_nPlaySpeed = 1;
	m_nPlayTime = 0;

	m_nTaskID = 0;
	m_nGroupID = 0;
	m_vecNodeInfo.clear();
	m_LoadDataCal = false;

	ui->label_4->setText("--");
	ui->label_3->setVisible(false);
	ui->label_4->setVisible(false);

	m_bPlayed = false;

	LoadIPIni();
	CreateListHeading();
	localWeaponNameBindToId();

	QTimer* timer = new QTimer(this);
	connect(timer, &QTimer::timeout, [this]()
	{
		if (m_pIUAVRealTimeStateManageService == nullptr) {
			return;
		}
		qnzkna::UAVRealTimeStateManage::NebulaUAVRealTimeStateMap nebulaUAVRealTimeStateMap;
		if (!m_pIUAVRealTimeStateManageService->GetUAVRealTimeStateList(&nebulaUAVRealTimeStateMap))
			return;

		std::vector<std::pair<QString, QGeoCoordinate>>	LocatonCoverUAV;
		for (auto UAVRealTime : nebulaUAVRealTimeStateMap)
		{
			qnzkna::UAVRealTimeStateManage::RealTimeTelemetryInformation_t TelemetryInfo = UAVRealTime.second.m_telemetryInfo;
			LocatonCoverUAV.push_back(std::pair<QString, QGeoCoordinate>(UAVRealTime.first.c_str(), QGeoCoordinate(TelemetryInfo.telemetryInfo.position.lat, TelemetryInfo.telemetryInfo.position.lng, TelemetryInfo.telemetryInfo.gpsHeigth)));
		}
		if (LocatonCoverUAV.size() <= 1)
			return;

		bool bStatus = true;
		if (LY_TaskManage::Get().GetForecastTendency()->isVisible())
			bStatus = false;
		for (auto uavlist : m_mapHorizHeight.values())
		{
			if (int(abs(uavlist)) > 0)
				bStatus = false;
		}
		if (!m_mapAdjustUAVTelemetry.isEmpty())
			bStatus = false;
		if (!m_VecAdjustUAVTelemetrySpeed.isEmpty())
			bStatus = false;

		if (bStatus)
		{
			m_UAVStatus.nSubductionEnd = 0;
			m_UAVStatus.status = -1;
			m_UAVStatus.distane = -1;
			m_UAVStatus.nQuadrate = 0;
		}

		for (int it = 0; it < LocatonCoverUAV.size(); it++)
		{
			for (int UAV = it + 1; UAV < LocatonCoverUAV.size(); ++UAV)
			{
				QGeoCoordinate UAV_3D_1 = LocatonCoverUAV[it].second;
				QGeoCoordinate UAV_3D_2 = LocatonCoverUAV[UAV].second;
				double disUAV = UAV_3D_1.distanceTo(UAV_3D_2);

				if (UAV_3D_1.distanceTo(UAV_3D_2) < m_BaseRuleStu.nUAVSafeDis)
				{

					if(m_UAVStatus.nSubductionEnd == 0)
						ForecastRouteTendency_FlyRule(LocatonCoverUAV[it].first, LocatonCoverUAV[UAV].first, nebulaUAVRealTimeStateMap);
				}
				else
				{
					if (m_VecAdjustUAVTelemetrySpeed.find(LocatonCoverUAV[it].first) != m_VecAdjustUAVTelemetrySpeed.end()
						&& m_VecAdjustUAVTelemetrySpeed[LocatonCoverUAV[it].first] == LocatonCoverUAV[UAV].first)
					{

						for (auto& uavcon : m_PlatSpeedCol)
						{
							if (uavcon.UAVName.c_str() != LocatonCoverUAV[UAV].first)
								continue;

							if (n_WaitTimer < m_BaseRuleStu.nWaitTime)
							{
								n_WaitTimer = n_WaitTimer + 1 * m_nPlaySpeed;
								break;
							}
							uavcon.speed = 1;
							m_VecAdjustUAVTelemetrySpeed.remove(LocatonCoverUAV[it].first);

							QDateTime curdt = QDateTime::currentDateTime();
							QString strcurdt = curdt.toString("yyyy-MM-dd hh:mm:ss");
							auto lParam = new EventInfo(strcurdt, tr2("系统事件"), tr2("四旋翼无人机: %1 恢复状态").arg(m_SystemSysNameAndID[uavcon.UAVName].c_str()));
							LY_TaskManageInterface::Get().SendMsgToAll(IInterface::MSG_Command, 900200, lParam);
							n_WaitTimer = 0;
						}
					}
					if(m_mapHorizHeight.size() <=0)
						continue;

					bool isSucc = true;

					for (auto paisr : m_mapHorizHeight)
					{
						if (std::abs(paisr) < (m_BaseRuleStu.nAltDiff / 2))
							isSucc = false;
					}
					if (!isSucc)
					{
						m_UAVStatus.status == 0;
						continue;
					}
					if (m_UAVStatus.nSubductionEnd == 1 && m_UAVStatus.nQuadrate == 2)
					{
						if (m_UAVStatus.distane < 0)
						{
							m_UAVStatus.distane = UAV_3D_1.distanceTo(UAV_3D_2);
							return;
						}

						if (m_mapAdjustUAVTelemetry.find(LocatonCoverUAV[it].first) != m_mapAdjustUAVTelemetry.end()
							&& m_mapAdjustUAVTelemetry[LocatonCoverUAV[it].first] == LocatonCoverUAV[UAV].first)
						{
							if (UAV_3D_1.distanceTo(UAV_3D_2) - m_UAVStatus.distane > 1)
							{
								m_mapAdjustUAVTelemetry.remove(LocatonCoverUAV[it].first);
								m_UAVStatus.status = 2;

								QDateTime curdt = QDateTime::currentDateTime();
								QString strcurdt = curdt.toString("yyyy-MM-dd hh:mm:ss");
								auto lParam = new EventInfo(strcurdt, tr2("系统事件"), tr2("四旋翼无人机: %1, %2 解除碰撞告警").arg(LocatonCoverUAV[it].first, LocatonCoverUAV[UAV].first));
								LY_TaskManageInterface::Get().SendMsgToAll(IInterface::MSG_Command, 900200, lParam);
							}
							else
								m_UAVStatus.status = 1;
						}
					}
				}
			}
		}

		m_mapLocatonCoverUAV.clear();
		for (auto& UAVRealTime : nebulaUAVRealTimeStateMap)
		{
			qnzkna::UAVRealTimeStateManage::RealTimeTelemetryInformation_t TelemetryInfo = UAVRealTime.second.m_telemetryInfo;
			m_mapLocatonCoverUAV.push_back(std::pair<QString, QGeoCoordinate>(UAVRealTime.first.c_str(), QGeoCoordinate(TelemetryInfo.telemetryInfo.position.lat, TelemetryInfo.telemetryInfo.position.lng, TelemetryInfo.telemetryInfo.gpsHeigth)));
		}
	});
	timer->start(1000);
}

CDlgPlay::~CDlgPlay()
{
	m_LinesTimer->stop();
    delete ui;
}

void CDlgPlay::ClearWndRouteLineInfoList()
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

void CDlgPlay::UpdateRouteLineInfoList(unsigned int nTaskID, unsigned int nGroupID, qnzkna::NodeManage::CNodeRelationInfoPtrVector	&vecNodeInfo)
{
    m_nTaskID     = nTaskID;
    m_nGroupID    = nGroupID;
    m_vecNodeInfo = vecNodeInfo;
    return ;
}

void CDlgPlay::LoadDataRouteLineInfoList(unsigned int nTaskID, unsigned int nGroupID, qnzkna::NodeManage::CNodeRelationInfoPtrVector	&vecNodeInfo)
{
    ClearWndRouteLineInfoList();
    m_Routelinelist.clear();

    if (m_bPlayed == false)
    {
        return;
    }

    QString str;
    int count = 0;

    m_pITaskManageService->GetNebulaRouteLineIDList(nTaskID, nGroupID, &m_Routelinelist);
    int nLineNum;
    m_pITaskManageService->GetNebulaRouteLineNum(nTaskID, nGroupID, nLineNum);
    CNebulaRouteLine::RouteLineIDList::iterator iter_list = m_Routelinelist.begin();
    for (; iter_list != m_Routelinelist.end(); iter_list++)
    {
        int nLineID = *iter_list;
        std::string strWeaponID;
        m_pITaskManageService->GetNebulaWeaponID(nTaskID, nGroupID, nLineID, strWeaponID);
        qnzkna::NodeManage::CNodeRelationInfoPtrVector::iterator iter_info_list = vecNodeInfo.begin();
        for (; iter_info_list != vecNodeInfo.end(); iter_info_list++)
        {
            std::string s_NodeID;
            std::string strWeaponName;
            s_NodeID = (*iter_info_list)->GetNodeID();
            if (0 == strcmp(s_NodeID.c_str(), strWeaponID.c_str()))
            {
                strWeaponName = (*iter_info_list)->GetNodeName();
                m_RoutePointIDlist.clear();
                m_pITaskManageService->GetNebulaRoutePointIDList(nTaskID, nGroupID, nLineID, &m_RoutePointIDlist);
                int num;
                m_pITaskManageService->GetNebulaRoutePointNum(nTaskID, nGroupID, nLineID, num);

                WndRouteLineInfo *pPoint = new WndRouteLineInfo;
                pPoint->bFormState = false;
                pPoint->strID = strWeaponName;
                pPoint->strWeaponID = strWeaponID;
                pPoint->nLineID = nLineID;
                pPoint->nID = count + 1;
                CNebulaRoutePoint::RoutePointIDList::iterator iter_info_list = m_RoutePointIDlist.begin();
                for (; iter_info_list != m_RoutePointIDlist.end(); iter_info_list++)
                {
                    WndRoutePoint temWndRoutePoint;
                    temWndRoutePoint.nPointID = *iter_info_list;
                    int nPointID = *iter_info_list;
                    m_pITaskManageService->GetNebulaRoutePoint(nTaskID, nGroupID, nLineID, nPointID, &m_NebulaRoutePoint);
                    temWndRoutePoint.stNebulaRoutePoint = m_NebulaRoutePoint;
                    pPoint->vecNebulaRoutePoint.push_back(temWndRoutePoint);
                }
                m_stWndRouteLineInfoList.m_vecRoutePointInfo.push_back(pPoint);
                count++;
            }
        }
    }
    std::string strTaskName;
    m_pITaskManageService->GetNebulaTaskName(nTaskID, strTaskName);
    ui->label_4->setText(tr2(strTaskName.c_str()));

    CalculatePoint();
}

void CDlgPlay::CalculatePoint()
{
    qint64 currentTimeTemp;
    QString strTips = tr("In the calculation, the number of points:");
    QString strTipsF = "";

    m_vecCalculateMaxNum = 0;
    int nCalculatePoint = 1;
    m_vecCalculatePoint.clear();
    for (auto stLine : m_stWndRouteLineInfoList.m_vecRoutePointInfo)
    {
        if (m_StartTime == "")
        {
            m_StartTime = stLine->strStartTime;
        }
        CCalculateLine stCalculatePoint;
        stCalculatePoint.strWeaponID = stLine->strWeaponID;
        stCalculatePoint.strWeaponName = stLine->strID;
        stCalculatePoint.stTelemetryPointVec.clear();
        stCalculatePoint.strStartTime = stLine->strStartTime;

        QGeoCoordinate QPoint1, QPoint2;
        auto itrPoint = stLine->vecNebulaRoutePoint.begin();
        for (; itrPoint != stLine->vecNebulaRoutePoint.end(); itrPoint++)
        {
            if ((itrPoint + 1) != stLine->vecNebulaRoutePoint.end())
            {
                double dLongStart(0), dLatStart(0), dLongEnd(0), dLatEnd(0), dHeightEnd(0), dHeight(0);
                dLongStart = itrPoint->stNebulaRoutePoint.GetLongitude();
                dLatStart = itrPoint->stNebulaRoutePoint.GetLatitude();
                dLongEnd = (itrPoint + 1)->stNebulaRoutePoint.GetLongitude();
                dLatEnd = (itrPoint + 1)->stNebulaRoutePoint.GetLatitude();
                dHeightEnd = (itrPoint + 1)->stNebulaRoutePoint.GetHeight();
                dHeight = itrPoint->stNebulaRoutePoint.GetHeight();
                QPoint1.setLongitude(dLongStart);
                QPoint1.setLatitude(dLatStart);
                QPoint2.setLongitude(dLongEnd);
                QPoint2.setLatitude(dLatEnd);

                double localdistance = QPoint1.distanceTo(QPoint2);

                double speedPlay = (itrPoint->stNebulaRoutePoint.GetSpeed() * 1.0 / PLAY_HZ);
                if (speedPlay <= 0)
                {
                    speedPlay = 0.5;
                }

                double dbearing = QPoint1.azimuthTo(QPoint2);

                double dDisOnce = itrPoint->stNebulaRoutePoint.GetSpeed() * 1.0 / PLAY_HZ ;
                if (dDisOnce <= 0)
                {
                    dDisOnce = 0.5;
                }
                double dNewLong(0), dNewLat(0);

                telemetryInformation stTelemetryInformation;
                memset(&stTelemetryInformation, 0x00, sizeof(stTelemetryInformation));
                stTelemetryInformation.position.lng = dLongStart;
                stTelemetryInformation.position.lat = dLatStart;
                stTelemetryInformation.gpsHeigth = dHeight / 10.0;
                stTelemetryInformation.relativeOriginHeigth = stTelemetryInformation.gpsHeigth;
                stTelemetryInformation.relativeGroundHeigth = stTelemetryInformation.gpsHeigth;
                stTelemetryInformation.angleofNose = MakeSureAngel(dbearing);
                stTelemetryInformation.groundVelocity = itrPoint->stNebulaRoutePoint.GetSpeed();
                stCalculatePoint.stTelemetryPointVec.push_back(stTelemetryInformation);

                if (itrPoint->stNebulaRoutePoint.GetHoverTime() != 0)
                {
                    for (int i = 0; i < itrPoint->stNebulaRoutePoint.GetHoverTime()* PLAY_HZ - 1 ; i++)
                    {
                        stCalculatePoint.stTelemetryPointVec.push_back(stTelemetryInformation);
                    }
                }

                strTipsF = strTips + QString::fromStdString(stCalculatePoint.strWeaponName) + " "  + QString::number(itrPoint->nPointID) + " " + QString::number(nCalculatePoint);
                nCalculatePoint++;
                currentTimeTemp = QDateTime::currentSecsSinceEpoch();
                UpdateLabel(currentTimeTemp, strTipsF);

                double dlocaldistanceOnce = localdistance;
                double dLastDistance = localdistance;
                while (abs(localdistance - dDisOnce) > dDisOnce)
                {

                    QPoint1 = QPoint1.atDistanceAndAzimuth(dDisOnce, dbearing);

                    dLastDistance   = localdistance;
                    localdistance = QPoint1.distanceTo(QPoint2);

                    dbearing = QPoint1.azimuthTo(QPoint2);

                    telemetryInformation stTelemetryInformationTmp;
                    memset(&stTelemetryInformationTmp, 0x00, sizeof(stTelemetryInformationTmp));
                    stTelemetryInformationTmp.position.lng = QPoint1.longitude();
                    stTelemetryInformationTmp.position.lat = QPoint1.latitude();
                    stTelemetryInformationTmp.gpsHeigth = dHeight / 10.0;
                    stTelemetryInformationTmp.relativeOriginHeigth = stTelemetryInformation.gpsHeigth;
                    stTelemetryInformationTmp.relativeGroundHeigth = stTelemetryInformation.gpsHeigth;

                    if (dlocaldistanceOnce != 0)
                    {
                        stTelemetryInformationTmp.gpsHeigth = (dHeight + ((dlocaldistanceOnce - localdistance) * (dHeightEnd - dHeight)) / dlocaldistanceOnce) / 10.0;
                        stTelemetryInformationTmp.relativeOriginHeigth = stTelemetryInformation.gpsHeigth;
                        stTelemetryInformationTmp.relativeGroundHeigth = stTelemetryInformation.gpsHeigth;

                    }
                    stTelemetryInformationTmp.angleofNose = MakeSureAngel(dbearing);
                    stTelemetryInformationTmp.groundVelocity = itrPoint->stNebulaRoutePoint.GetSpeed();
                    stCalculatePoint.stTelemetryPointVec.push_back(stTelemetryInformationTmp);

                    strTipsF = strTips + QString::fromStdString(stCalculatePoint.strWeaponName) + " "  + QString::number(itrPoint->nPointID) + " " + QString::number(nCalculatePoint);
                    nCalculatePoint++;
                    currentTimeTemp = QDateTime::currentSecsSinceEpoch();
                    UpdateLabel(currentTimeTemp, strTipsF);
                    if (dLastDistance < localdistance)
                    {
                        break;
                    }
                }
            }
            else
            {

                double dLongStart(0), dLatStart(0), dHeight(0);
                dLongStart = itrPoint->stNebulaRoutePoint.GetLongitude();
                dLatStart = itrPoint->stNebulaRoutePoint.GetLatitude();
                dHeight = itrPoint->stNebulaRoutePoint.GetHeight();

                telemetryInformation stTelemetryInformation;
                memset(&stTelemetryInformation, 0x00, sizeof(telemetryInformation));
                stTelemetryInformation.position.lng = dLongStart;
                stTelemetryInformation.position.lat = dLatStart;
                stTelemetryInformation.gpsHeigth = dHeight;
                stTelemetryInformation.groundVelocity = itrPoint->stNebulaRoutePoint.GetSpeed();

                stCalculatePoint.stTelemetryPointVec.push_back(stTelemetryInformation);

                strTipsF = strTips + QString::fromStdString(stCalculatePoint.strWeaponName) + " "  + QString::number(itrPoint->nPointID) + " " + QString::number(nCalculatePoint);
                nCalculatePoint++;
                currentTimeTemp = QDateTime::currentSecsSinceEpoch();
                UpdateLabel(currentTimeTemp, strTipsF);

            }
        }
        if (stCalculatePoint.stTelemetryPointVec.size() >= m_vecCalculateMaxNum)
        {
            m_vecCalculateMaxNum = stCalculatePoint.stTelemetryPointVec.size();
        }
        m_vecCalculatePoint.push_back(stCalculatePoint);
    }
}

double CDlgPlay::DegreesToRadians(double angle)
{
    return angle / 180.0 * PI;
}

double CDlgPlay::RadiansToDegrees(double angle)
{
    return angle * 180.0 / PI;
}

double CDlgPlay::MakeSureAngel(double dangle)
{
    double dAmgeel = dangle;
    if (dAmgeel < 0)
    {
        dAmgeel += 360;
    }
    return dAmgeel;
}

void CDlgPlay::on_timeOut()
{
    std::vector<CCalculateLine>::const_iterator itrlie = m_vecCalculatePoint.cbegin();
    for (; itrlie != m_vecCalculatePoint.cend();++itrlie)
    {
        auto itrPlaying = m_PlatPlayingMap.find(itrlie->strWeaponID);
        if (itrPlaying == m_PlatPlayingMap.end())
            continue;

        if (itrPlaying->second == true)
			continue;

        qint64 llTime = CalculateTime(itrlie->strStartTime, m_StartTime) * PLAY_HZ;
        if (llTime - m_nPlayTime > 0)
            continue;

		SpeedControl Telemetry;
		for (auto data : m_PlatSpeedCol)
		{
			if(data.UAVName != itrlie->strWeaponID)
				continue;
			Telemetry = data;
			break;
		}

		double heightDiff = m_BaseRuleStu.nAltDiff / 2;

		double Accelereate = 1.0 / double(m_nPlaySpeed);
		double dHieht = m_mapUAVChangeHeight[itrlie->strWeaponID];
		bool bIsIterAdvance = false;
		if (m_UAVStatus.nSubductionEnd  == 1 && m_UAVStatus.status == 0)
		{
			if (m_mapHorizHeight.find(itrlie->strWeaponID) != m_mapHorizHeight.end() && m_mapHorizHeight[itrlie->strWeaponID] != dHieht)
			{
				if (dHieht == -heightDiff && m_mapHorizHeight[itrlie->strWeaponID] > dHieht)
				{
					m_mapHorizHeight[itrlie->strWeaponID] -= Accelereate;
					bIsIterAdvance = true;
				}
				else if (dHieht == heightDiff && m_mapHorizHeight[itrlie->strWeaponID] < dHieht)
				{
					m_mapHorizHeight[itrlie->strWeaponID] += Accelereate;
					bIsIterAdvance = true;
				}
			}
		}
		double nHorizHeight = m_mapHorizHeight[itrlie->strWeaponID];
		if (m_UAVStatus.status == 2)
		{
			if (std::abs(nHorizHeight) > Accelereate)
			{

				if (m_mapHorizHeight.find(itrlie->strWeaponID) != m_mapHorizHeight.end() && m_mapHorizHeight[itrlie->strWeaponID] < 0)
				{
					m_mapHorizHeight[itrlie->strWeaponID] += Accelereate;
					bIsIterAdvance = true;
				}
				else if (m_mapHorizHeight.find(itrlie->strWeaponID) != m_mapHorizHeight.end() && m_mapHorizHeight[itrlie->strWeaponID] > 0)
				{
					m_mapHorizHeight[itrlie->strWeaponID] -= Accelereate;
					bIsIterAdvance = true;
				}
			}
		}

		int nPointNo = m_SpeedColTime[itrlie->strWeaponID] - llTime;
		nPointNo = nPointNo + Telemetry.speed;
		if (bIsIterAdvance)
		{
			nPointNo = nPointNo - Telemetry.speed;
		}
		m_SpeedColTime[itrlie->strWeaponID] = nPointNo;
        if (nPointNo >= 0 && nPointNo < itrlie->stTelemetryPointVec.size())
        {

            const std::string &vehicleID = itrlie->strWeaponID;
            const telemetryInformation &telemetryInfo = itrlie->stTelemetryPointVec[nPointNo];

            xygsJL_20 bag;
            bag.flags = 20;
            bag.param = telemetryInfo;
#if 0

            if (ITaskManagerActivator::Get().getIRegistryImpl())
            {

                qnzkna::framework::MessageReferenceProps props;
                props.put("ChannelID", "ChannelID");
                props.put("ProtocolType", WEAPONSYSTEM_VEHICLE_PROTOCOLTYPE_XY_54);
                props.put("TargetID", vehicleID);
                ITaskManagerActivator::Get().getIRegistryImpl()->messageNotify(new qnzkna::framework::MessageReference("MsgUAVRealTimeState", (char *)&bag, sizeof(bag), props));
            }
#else

            TelemetryInformation_t stTelemetryInformation_t;
            memset(&stTelemetryInformation_t, 0x00, sizeof(TelemetryInformation_t));
            stTelemetryInformation_t.lat					= telemetryInfo.position.lat * 1e7 + dLatitude* 1e7;
            stTelemetryInformation_t.lng					= telemetryInfo.position.lng * 1e7 + dLongitude* 1e7;
			stTelemetryInformation_t.alt = telemetryInfo.gpsHeigth * 10 + m_mapHorizHeight[itrlie->strWeaponID];
            stTelemetryInformation_t.relativeOriginHeigth	= telemetryInfo.relativeOriginHeigth * 100;
            stTelemetryInformation_t.relativeGroundHeigth	= telemetryInfo.relativeGroundHeigth * 100;
            stTelemetryInformation_t.angleofNose			= telemetryInfo.angleofNose * 10;
            stTelemetryInformation_t.angleofRudder			= telemetryInfo.angleofNose * 10;
            stTelemetryInformation_t._rudder				= telemetryInfo.angleofNose * 10;
            stTelemetryInformation_t.targetVelocity			= telemetryInfo.groundVelocity * 100;
            stTelemetryInformation_t.groundVelocity			= telemetryInfo.groundVelocity * 100;

            for (auto IPItr : m_UAVServerIPVec)
            {
                std::string strName = IPItr.strName.toStdString();
                if (0 == strcmp(strName.c_str(), vehicleID.c_str()))
                {
                    QHostAddress stQHostAddress;
                    stQHostAddress.setAddress(IPItr.strIP);
                    m_UAVProtocol.SendCommandBytes(0x10, 0x10, reinterpret_cast<const uchar *>(&stTelemetryInformation_t), sizeof(stTelemetryInformation_t), stQHostAddress, IPItr.nPort, IPItr.nPort);
                    break;
                }
            }
#endif
        }

    }

	double MaxNum = 0;
	for (auto maxnum : m_SpeedColTime)
	{
		if (MaxNum < maxnum.second)
			MaxNum = maxnum.second;
	}

    if (m_vecCalculateMaxNum - MaxNum < 0.00001)
    {
        if (ui->checkBox->isChecked() == true)
        {
			m_SpeedColTime.clear();
        }
        else
        {
            m_nPlayTime = 0;
            m_nPlaySpeed = 1;
            m_PlayTimer->stop();
        }

    }
	m_nPlayTime = MaxNum;
    double nPor = m_nPlayTime * 100.0 / m_vecCalculateMaxNum;
    ui->progressBar->setValue(nPor);
    ui->progressBar->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    ui->progressBar->setFormat(QStringLiteral("%1%").arg(QString::number(nPor, 'f', 2)));
    ui->rate->setText(QString::number(m_nPlaySpeed));
    ui->lineEdit_time->setText(QString::number(m_nPlayTime * 1.0 / PLAY_HZ, 'f', 2));
}

void CDlgPlay::LoadIPIni()
{
    m_UAVServerIPVec.clear();
    std::string strPath = "../data/conf/PlaySendToServer.ini";
    std::ifstream inFile(strPath);
    std::string oneLine;
    std::stringstream ss;
    QStringList nameList, ipList;
    while (getline(inFile, oneLine))
    {
        std::string name, ip, port;
        ss = std::stringstream(oneLine);
        ss >> name >> ip >> port;

        UAVServerIP stUAVServerIPVec;
        stUAVServerIPVec.strIP = QString::fromStdString(ip);
        stUAVServerIPVec.strName = QString::fromStdString(name);
        stUAVServerIPVec.nPort = QString::fromStdString(port).toInt();
        m_UAVServerIPVec.push_back(stUAVServerIPVec);
    }
}

void CDlgPlay::SetPlayStates(bool bStates)
{
    m_bPlayed = bStates;
}

void CDlgPlay::CreateListHeading()
{
    QStringList headerlabels;
    headerlabels << tr(" Loading ") << tr(" No. ") << tr(" Route Number ") << tr(" Route Name ") << tr(" Task ID ") << tr(" Platform ");
    headerlabels << tr(" Route Type ") << tr(" Protocol Type ") << tr(" Destinations Number ") << tr(" Start Time ");

    ui->tableWidget->setColumnCount(headerlabels.size());
    ui->tableWidget->setHorizontalHeaderLabels(headerlabels);

    for (int i = 0; i < headerlabels.size(); i++)
    {
        if (i == 0 || i == 9)
        {
            ui->tableWidget->horizontalHeader()->setSectionResizeMode(i, QHeaderView::ResizeToContents);
        }
        else
        {
            ui->tableWidget->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Stretch);
        }
    }

    ui->tableWidget->verticalHeader()->setHidden(true);
    ui->tableWidget->setSelectionBehavior(QTableWidget::SelectRows);
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->setAlternatingRowColors(true);
    ui->tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
}

void CDlgPlay::UpdataWindowData()
{
    m_MissionLineVec.clear();
    if (m_pITaskManageService == nullptr)
    {
        return;
    }

    m_MissionLineVec = m_pITaskManageService->GetAllMissionLine();

    UpdateShowData();
    UpdateListData();
}

void CDlgPlay::UpdateShowData()
{
    m_MissionLineShowVec.clear();
    if (m_pITaskManageService == nullptr)
    {
        return;
    }

    auto m_Missions = m_pITaskManageService->GetActivatingMissions();

    for (int i = 0; i < m_Missions.size(); i++)
    {
		std::string missionId = m_Missions[i].GetID();
        auto newLines = m_pITaskManageService->GetMissionLinesByTask(missionId);
        for (int j = 0; j < newLines.size(); j++)
        {
            m_MissionLineShowVec.push_back(newLines[j]);
        }
    }
}

void CDlgPlay::UpdateListData()
{
    if (m_MissionLineShowVec.size() == 0)
    {
        return;
    }

    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(m_MissionLineShowVec.size());
    m_LineIDAndSystemSysID.clear();

    int nNo(0), intMissionLineNo(0);
    QString strMissionIDTemp = "";
    for (auto data : m_MissionLineShowVec)
    {
        QString strNo, strShow, strID, strName, strTask, strPlatform, strType, strProtocol, strPointCount, strTime;
        strNo = QString::number(nNo + 1);
        strID = "HX" + QString::number(data.nID);
        strName = tr2(data.strName);
        strShow = tr("");
        strTask = tr2(data.strMissionID);

        if (strMissionIDTemp == strTask)
        {
            intMissionLineNo++;
        }
        else
        {
            strMissionIDTemp = strTask;
            intMissionLineNo = 0;
        }
        std::string stdstrPlatform = LineMatchPlatform(data.strMissionID, data.nID, intMissionLineNo);

        strPlatform = GetPlatNameByID(stdstrPlatform);
        if (strPlatform == "")
        {
            strPlatform = "-";
        }

        m_LineIDAndSystemSysID.insert(std::make_pair(data.nID, stdstrPlatform));

        if (strTask == "")
        {
            strTask = "-";
        }

        strType = GetLineTypeStr(data.cType);
        strProtocol = GetLineProtocolStr(data.cProtocolType);
        strPointCount = QString::number(data.LinePointVec.size());
        CNeubulaMission stMission = m_pITaskManageService->GetMission(strTask.toStdString());
        strTime = QString::fromStdString(stMission.GetStartTime());

        TableWidgetAddCheckBox(ui->tableWidget, nNo, 0, NULL, Qt::Unchecked);

        QTableWidgetItem *item1 = new QTableWidgetItem(strNo);
        QTableWidgetItem *item2 = new QTableWidgetItem(strID);
        QTableWidgetItem *item3 = new QTableWidgetItem(strName);
        QTableWidgetItem *item4 = new QTableWidgetItem(strTask);
        QTableWidgetItem *item5 = new QTableWidgetItem(strPlatform);
        QTableWidgetItem *item6 = new QTableWidgetItem(strType);
        QTableWidgetItem *item7 = new QTableWidgetItem(strProtocol);
        QTableWidgetItem *item8 = new QTableWidgetItem(strPointCount);
        QTableWidgetItem *item9 = new QTableWidgetItem(strTime);

        ui->tableWidget->setItem(nNo, 1, item1);
        ui->tableWidget->setItem(nNo, 2, item2);
        ui->tableWidget->setItem(nNo, 3, item3);
        ui->tableWidget->setItem(nNo, 4, item4);
        ui->tableWidget->setItem(nNo, 5, item5);
        ui->tableWidget->setItem(nNo, 6, item6);
        ui->tableWidget->setItem(nNo, 7, item7);
        ui->tableWidget->setItem(nNo, 8, item8);
        ui->tableWidget->setItem(nNo, 9, item9);

        ui->tableWidget->item(nNo, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->tableWidget->item(nNo, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->tableWidget->item(nNo, 3)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->tableWidget->item(nNo, 4)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->tableWidget->item(nNo, 5)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->tableWidget->item(nNo, 6)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->tableWidget->item(nNo, 7)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->tableWidget->item(nNo, 8)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->tableWidget->item(nNo, 9)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

        nNo++;
    }
}

QString CDlgPlay::GetLineTypeStr(int nLineType)
{
    QString strF = "-";
    switch (nLineType)
    {
    case 1:
        strF = tr("Reconnaissance");
        break;
    case 2:
        strF = tr("Strike");
        break;
    default:
        break;
    }
    return strF;
}

QString CDlgPlay::GetLineProtocolStr(int nLinePro)
{
    QString strF = "-";
    switch (nLinePro)
    {
    case 1:
        strF = tr("XY54");
        break;
    case 2:
        strF = tr("XY");
        break;
    default:
        break;
    }
    return strF;
}

void CDlgPlay::TableWidgetAddCheckBox(QTableWidget *tableWidget, int x, int y, QString text, Qt::CheckState CheckState)
{
    QWidget *widget = new QWidget();
    QHBoxLayout *layout = new QHBoxLayout();
    QCheckBox *checkbox = new QCheckBox();

    checkbox->setText(text);
    checkbox->setCheckState(CheckState);
    layout->addWidget(checkbox, 0, Qt::AlignCenter);
    layout->setMargin(0);
    widget->setLayout(layout);
    tableWidget->setCellWidget(x, y, widget);
}

std::string CDlgPlay::LineMatchPlatform(const std::string &strMissionID, const int &nLineID, const int &nNo)
{
    std::string strPlatID = "";
	std::string missionid = QString::fromLocal8Bit(strMissionID.c_str()).toStdString();
    CNeubulaMissionLineVec LineVec = m_pITaskManageService->GetMissionLinesByTask(missionid);

    if (LineVec.size() == 0)
    {
        return strPlatID;
    }

    std::vector<std::string> reStrs;
    reStrs = m_pITaskManageService->readPlatFormMatch(strMissionID);

    if (reStrs.size() == 0)
    {
        return strPlatID;
    }

    auto itrPlat = reStrs.begin();
    auto itrPlat2 = reStrs.begin();
    for (auto line : LineVec)
    {
        if (line.nID != nLineID)
        {
            continue;
        }

        WndRouteLineInfo *pPoint = new WndRouteLineInfo;
        pPoint->bFormState = false;

        if (nNo < reStrs.size())
        {

            itrPlat2 = itrPlat + nNo;
        }
        else
        {
            continue;
        }

        return *itrPlat2;
    }
    return strPlatID;
}

void CDlgPlay::localWeaponNameBindToId()
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

std::string CDlgPlay::GetPlatByLineID(const int nLineID)
{
    std::string strPlatID = "";
    auto itr = m_LineIDAndSystemSysID.find(nLineID);
    if (itr != m_LineIDAndSystemSysID.end())
    {
        strPlatID = itr->second;
    }
    return strPlatID;
}

QString CDlgPlay::GetPlatNameByID(const std::string strPlatID)
{
    QString  strName = "";
    auto LineIte = m_SystemSysNameAndID.find(strPlatID);
    if (LineIte != m_SystemSysNameAndID.end())
    {
        strName = QString::fromStdString(LineIte->second);
    }
    return strName;
}

void CDlgPlay::UpdateLabel(qint64 CurrentTime, QString strTIps)
{
    if (m_CurrentTime == 0)
    {
        m_CurrentTime = CurrentTime;
        ui->label_6->setText(strTIps);
        ui->label_6->repaint();
    }

    if (abs(m_CurrentTime - CurrentTime) > 1)
    {
        m_CurrentTime = CurrentTime;
        ui->label_6->setText(strTIps);
        ui->label_6->repaint();
    }
}

bool SortTime(const WndRouteLineInfo *p1, const WndRouteLineInfo *p2)
{
    return (QDateTime::fromString(QString::fromStdString(p1->strStartTime), "yyyy/M/d h:mm") < QDateTime::fromString(QString::fromStdString(p2->strStartTime), "yyyy/M/d h:mm"));
}

void CDlgPlay::SortDataByStartTime()
{
    std::sort(m_stWndRouteLineInfoList.m_vecRoutePointInfo.begin(), m_stWndRouteLineInfoList.m_vecRoutePointInfo.end(), SortTime);
}

bool CDlgPlay::JudgeLinePlayTime()
{
    auto itrLine = m_vecCalculatePoint.begin();
    for (; itrLine != m_vecCalculatePoint.end(); itrLine++)
    {
        auto itrLineTemp = itrLine;
        while ((itrLineTemp + 1) != m_vecCalculatePoint.end())
        {
            if (itrLine->strWeaponID == (itrLineTemp + 1)->strWeaponID)
            {
                qint64 Timelast = CalculateTime(itrLine->strStartTime, (itrLineTemp + 1)->strStartTime);
                if (Timelast >= 43200)
                {
                    LyMessageBox::information(NULL, tr("tip"), tr("If the task execution interval exceeds 12 hours, confirm if you need to continue the simulation."));

                }
                if (Timelast * PLAY_HZ < itrLine->stTelemetryPointVec.size())
                {
                    QString strTips = tr("Platform(") + QString::fromStdString(itrLine->strWeaponName) + tr(")set time conflict, please try again.");
                    LyMessageBox::information(NULL, tr("tip"), strTips);
                    return false;
                }
            }
            itrLineTemp++;
        }
    }
    return true;
}

qint64 CDlgPlay::CalculateTime(const std::string &strTime01, const std::string &strTime02)
{
    return  QDateTime::fromString(QString::fromStdString(strTime01), "yyyy/M/d h:mm").toSecsSinceEpoch() -
            QDateTime::fromString(QString::fromStdString(strTime02), "yyyy/M/d h:mm").toSecsSinceEpoch();
}

void CDlgPlay::on_BT_LoadData_clicked()
{
    on_BT_Stop_clicked();

    m_bPlayed = true;

    m_LoadDataCal = true;
    m_SelectedLineMap.clear();

    if (ui->tableWidget->rowCount() == 0)
    {
        return;
    }

    std::string strMissionID = "";

    for (int i = 0; i < ui->tableWidget->rowCount() ; i++)
    {
        QWidget *pWidget = ui->tableWidget->cellWidget(i, 0);
        if (pWidget->layout()->count() > 0)
        {
            QLayoutItem *it = pWidget->layout()->itemAt(0);
            QCheckBox *pQCheckBox = qobject_cast<QCheckBox *>(it->widget());
            if (pQCheckBox != nullptr)
            {
                if (pQCheckBox->checkState() == Qt::Checked)
                {
                    if (i < m_MissionLineShowVec.size())
                    {
                        auto itr = m_MissionLineShowVec.begin() + i;

                        auto itrMissID = m_SelectedLineMap.find(itr->strMissionID);
                        if (itrMissID != m_SelectedLineMap.end())
                        {
                            auto itrMissionPlatID = itrMissID->second.find(GetPlatByLineID(itr->nID));
                            if (itrMissionPlatID != itrMissID->second.end())
                            {
                                QString strTips = tr("Task(") + tr2(itr->strMissionID) + tr(")equipment(") + GetPlatNameByID(GetPlatByLineID(itr->nID)) + tr(")is duplicate, please check the data.");
                                LyMessageBox::information(NULL, tr("tip"), strTips);
                                return;
                            }
                            else
                            {
                                itrMissID->second.insert(std::make_pair(GetPlatByLineID(itr->nID), itr->nID));

                            }
                        }
                        else
                        {
                            PlatAndLineIDMap PlatAndLineIDMaptemp;
                            PlatAndLineIDMaptemp.insert(std::make_pair(GetPlatByLineID(itr->nID), itr->nID));
                            m_SelectedLineMap.insert(std::make_pair(itr->strMissionID, PlatAndLineIDMaptemp));
                        }
                    }
                }
            }
        }
    }

    ClearWndRouteLineInfoList();
    m_CurrentTime = 0;
    m_StartTime = "";

    if (m_SelectedLineMap.size() == 0)
    {
        LyMessageBox::information(NULL, tr("tip"), tr("Please select data."));
        return;
    }

    int nCount = 0;
    for (auto MissionID : m_SelectedLineMap)
    {
        std::string strID = MissionID.first;
        CNeubulaMission stMission = m_pITaskManageService->GetMission(strID);
        for (auto LineData : MissionID.second)
        {
            CNeubulaMissionLine stLine = m_pITaskManageService->GetMissionLine(LineData.second);

            if (stLine.LinePointVec.size() == 0)
            {
                continue;
            }

            WndRouteLineInfo *pPoint = new WndRouteLineInfo;
            pPoint->bFormState = false;
            pPoint->strID = GetPlatNameByID(LineData.first).toStdString();
            pPoint->strWeaponID = LineData.first;
            pPoint->nLineID = nCount + 1;
            pPoint->nID = nCount + 1;
            if (stMission.GetStartTime() == "")
            {
                QString strTips = tr("Task(") + QString::fromStdString(strID) + tr(")start time is empty, please set.") ;
                LyMessageBox::information(NULL, tr("tip"), strTips);
                return;
            }
            pPoint->strStartTime = stMission.GetStartTime();

            int nPointNo = 1;
            double dLonTmp(0), dLatTmp(0);
            for (auto LinePoint : stLine.LinePointVec)
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
    }

    SortDataByStartTime();

    CalculatePoint();

    if (JudgeLinePlayTime())
    {
        ui->label_6->setText(tr("Finished calculation"));

        {
            unsigned int sumPoint = 0;
            auto itrLineSumPoint = m_vecCalculatePoint.rbegin();
            for (; itrLineSumPoint != m_vecCalculatePoint.rend(); itrLineSumPoint++)
            {
                sumPoint += itrLineSumPoint->stTelemetryPointVec.size();
            }
            ui->label_6->setText(QString::number(sumPoint) + tr(", Finished calculation"));
        }

        LyMessageBox::information(NULL, tr("tip"), tr("The data is imported."));

        if (m_vecCalculatePoint.size() == 0)
        {
            return;
        }

        auto itrLine = m_vecCalculatePoint.rbegin();
        std::string strLastStartTime = itrLine->strStartTime;

        int nPointMax = 0;
        for (; itrLine != m_vecCalculatePoint.rend(); itrLine++)
        {
            if (strLastStartTime != itrLine->strStartTime)
            {
                break;
            }
            if (nPointMax < itrLine->stTelemetryPointVec.size())
            {
                nPointMax = itrLine->stTelemetryPointVec.size();
            }
			int maxNum = CalculateTime(strLastStartTime, m_StartTime);
			if (m_vecCalculateMaxNum < maxNum)
				m_vecCalculateMaxNum = maxNum;
        }

    }
    else
    {
        ui->label_6->setText(tr("The computation failed."));
    }
	m_PlatSpeedCol.clear();
	for (auto data : m_vecCalculatePoint)
	{
		SpeedControl scp;
		scp.UAVName = data.strWeaponID;
		scp.speed = 1;
		scp.amount = 0;
		m_PlatSpeedCol.push_back(scp);
	}
	if (LY_TaskManage::Get().GetPlaySpeedControl() != nullptr)
	{
		if (LY_TaskManage::Get().GetPlaySpeedControl()->isVisible())
		{
			LY_TaskManage::Get().GetPlaySpeedControl()->close();
		}
		else
		{
			LY_TaskManage::Get().GetPlaySpeedControl()->Init(m_PlatSpeedCol);
			LY_TaskManage::Get().GetPlaySpeedControl()->show();
		}
	}
}

void CDlgPlay::on_BT_Play_clicked()
{
	m_route_lines.clear();
	m_returnPosList.clear();
    if (!m_LoadDataCal)
    {
        if (m_nTaskID > 0 && m_nTaskID > 0)
        {
            m_bPlayed = true;
            LoadDataRouteLineInfoList(m_nTaskID, m_nTaskID, m_vecNodeInfo);
            m_LoadDataCal = true;
        }
    }

    if (m_stWndRouteLineInfoList.m_vecRoutePointInfo.size()  == 0)
    {
        LyMessageBox::information(NULL, tr("Tip"), tr("The data is not imported, please import the data first."));
        return;
    }

    if (ui->BT_Play->text() == tr("Pause"))
    {
        ui->BT_Play->setText(tr("Play"));
        m_PlayTimer->stop();
    }
    else if (ui->BT_Play->text() == tr("Play"))
    {
		m_pIZoneManageService->GetZoneRule(m_BaseRuleStu);
        m_PlatPlayingMap.clear();
        for (auto itrlie : m_vecCalculatePoint)
        {
            qnzkna::UAVRealTimeStateManage::RealTimeTelemetryInformation_t stelemetryInformation;
            bool result = false;
            result = m_pIUAVRealTimeStateManageService->GetUAVRealTimeState(itrlie.strWeaponID, stelemetryInformation);
            if (result == false)
            {
                m_PlatPlayingMap.insert(std::make_pair(itrlie.strWeaponID, false));
            }
            else
            {
                if (stelemetryInformation.isout == false)
                {
                    m_PlatPlayingMap.insert(std::make_pair(itrlie.strWeaponID, true));
                    QString strTips = tr("UAV(") + QString::fromStdString(itrlie.strWeaponName) + tr(")has flight data, and the stabbing simulation will not send data.");
                    LyMessageBox::information(NULL, tr("tip"), strTips);
                }
                else
                {
                    m_PlatPlayingMap.insert(std::make_pair(itrlie.strWeaponID, false));
                }
            }
        }

        ui->BT_Play->setText(tr("Pause"));
        m_PlayTimer->setInterval(1000 / PLAY_HZ / m_nPlaySpeed);
        m_PlayTimer->start();
    }

}

void CDlgPlay::on_BT_Stop_clicked()
{
	m_route_lines.clear();
	m_returnPosList.clear();
    m_PlayTimer->stop();
    ui->BT_Play->setText(tr("Play"));
    m_nPlayTime = 0;

	if (m_draw3DFirst) {
		del3DPathLine();
		Path3DLines.clear();
	}
}

void CDlgPlay::on_BT_decline_clicked()
{
    m_nPlaySpeed = m_nPlaySpeed / 2;
    if (m_nPlaySpeed == 0)
    {
        m_nPlaySpeed = 1;
    }

    if (m_PlayTimer->isActive() == true)
    {
        m_PlayTimer->stop();
        m_PlayTimer->setInterval(1000 / PLAY_HZ / m_nPlaySpeed);
        m_PlayTimer->start();
    }
    else
    {
        ui->rate->setText(QString::number(m_nPlaySpeed));
    }
}

void CDlgPlay::on_BT_Accelereate_clicked()
{
    m_nPlaySpeed = m_nPlaySpeed * 2;
    if (m_nPlaySpeed > 32)
    {
        m_nPlaySpeed = 32;
    }

    if (m_PlayTimer->isActive() == true)
    {
        m_PlayTimer->stop();
        m_PlayTimer->setInterval(1000 / PLAY_HZ / m_nPlaySpeed);
        m_PlayTimer->start();
    }
    else
    {
        ui->rate->setText(QString::number(m_nPlaySpeed));
    }
}

void CDlgPlay::changeEvent(QEvent *event)
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

void CDlgPlay::showEvent(QShowEvent *event)
{
    UpdataWindowData();
    ui->label_6->setText("-");

    lineShowStateAll(true);
}

void CDlgPlay::closeEvent(QCloseEvent *e)
{
    lineShowStateAll(false);
}

void CDlgPlay::on_checkBox_stateChanged(int arg1)
{

}

void CDlgPlay::on_tableWidget_cellClicked(int row, int column)
{
    if (row == -1)
    {
        return;
    }

    if (row >= ui->tableWidget->rowCount())
    {
        return;
    }

    QWidget *pWidget = ui->tableWidget->cellWidget(row, 0);
    if (pWidget->layout()->count() > 0)
    {
        QLayoutItem *it = pWidget->layout()->itemAt(0);
        QCheckBox *pQCheckBox = qobject_cast<QCheckBox *>(it->widget());
        if (pQCheckBox != nullptr)
        {
            if (pQCheckBox->checkState() == Qt::Checked)
            {
                pQCheckBox->setCheckState(Qt::Unchecked);
            }
            else
            {
                pQCheckBox->setCheckState(Qt::Checked);
            }
        }
    }
}

void CDlgPlay::on_pushButton_4_clicked()
{
    if (ui == nullptr || ui->tableWidget == nullptr)
    {
        return ;
    }

    int selectedRow = ui->tableWidget->currentRow();
    if (selectedRow >= 0 && selectedRow < ui->tableWidget->rowCount() && ui->tableWidget->columnCount() > 5 && selectedRow < m_MissionLineShowVec.size())
    {
        const QString &strMissionID = ui->tableWidget->item(selectedRow, 4)->text();
        for (int i = 0; i < ui->tableWidget->rowCount(); i++)
        {
            if (strMissionID == ui->tableWidget->item(i, 4)->text())
            {
                QWidget *pWidget = ui->tableWidget->cellWidget(i, 0);
                if (pWidget && pWidget->layout()->count() > 0)
                {
                    QLayoutItem *it = pWidget->layout()->itemAt(0);
                    QCheckBox *pQCheckBox = qobject_cast<QCheckBox *>(it->widget());
                    if (pQCheckBox != nullptr)
                    {
                        if (pQCheckBox->checkState() != Qt::Checked)
                        {
                            pQCheckBox->setCheckState(Qt::Checked);
                        }
                    }
                }
            }
        }
    }

    lineShowStateAll(false);
}

void CDlgPlay::on_pushButton_3_clicked()
{
    if (ui == nullptr || ui->tableWidget == nullptr)
    {
        return ;
    }

    int selectedRow = ui->tableWidget->currentRow();
    if (selectedRow >= 0 && selectedRow < ui->tableWidget->rowCount() && ui->tableWidget->columnCount() > 5 && selectedRow < m_MissionLineShowVec.size())
    {
        const QString &strMissionID = ui->tableWidget->item(selectedRow, 4)->text();
        for (int i = 0; i < ui->tableWidget->rowCount(); i++)
        {
            if (strMissionID == ui->tableWidget->item(i, 4)->text())
            {
                QWidget *pWidget = ui->tableWidget->cellWidget(i, 0);
                if (pWidget && pWidget->layout()->count() > 0)
                {
                    QLayoutItem *it = pWidget->layout()->itemAt(0);
                    QCheckBox *pQCheckBox = qobject_cast<QCheckBox *>(it->widget());
                    if (pQCheckBox != nullptr)
                    {
                        if (pQCheckBox->checkState() != Qt::Unchecked)
                        {
                            pQCheckBox->setCheckState(Qt::Unchecked);
                        }
                    }
                }
            }
        }
    }

    lineShowStateAll(false);
}

void CDlgPlay::on_pushButton_clicked()
{
    for (int i = 0; i < ui->tableWidget->rowCount(); i++)
    {
        QWidget *pWidget = ui->tableWidget->cellWidget(i, 0);
        if (pWidget->layout()->count() > 0)
        {
            QLayoutItem *it = pWidget->layout()->itemAt(0);
            QCheckBox *pQCheckBox = qobject_cast<QCheckBox *>(it->widget());
            if (pQCheckBox != nullptr)
            {
                if (pQCheckBox->checkState() != Qt::Checked)
                {
                    pQCheckBox->setCheckState(Qt::Checked);
                }
            }
        }
    }

    lineShowStateAll(false);
}

void CDlgPlay::on_pushButton_2_clicked()
{
    for (int i = 0; i < ui->tableWidget->rowCount(); i++)
    {
        QWidget *pWidget = ui->tableWidget->cellWidget(i, 0);
        if (pWidget->layout()->count() > 0)
        {
            QLayoutItem *it = pWidget->layout()->itemAt(0);
            QCheckBox *pQCheckBox = qobject_cast<QCheckBox *>(it->widget());
            if (pQCheckBox != nullptr)
            {
                if (pQCheckBox->checkState() != Qt::Unchecked)
                {
                    pQCheckBox->setCheckState(Qt::Unchecked);
                }
            }
        }
    }

    lineShowStateAll(false);
}

void CDlgPlay::lineShowStateAll(bool valueFromSettings)
{
    if (ui == nullptr || ui->tableWidget == nullptr || m_pITaskManageService == nullptr)
    {
        return ;
    }

    if (0 < ui->tableWidget->rowCount() && ui->tableWidget->columnCount() > 5 && 0 < m_MissionLineShowVec.size())
    {
        const QString strActivateID = QString::number(m_pITaskManageService->GetSelectedActivatingActionIDLocal());

        QStringList lineShowStates;

        QSettings settings("../data/conf/PlayTaskConfigSetting.ini", QSettings::IniFormat);
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
            for (int i = 0; i < ui->tableWidget->rowCount(); i++)
            {
                QWidget *pWidget = ui->tableWidget->cellWidget(i, 0);
                if (pWidget && pWidget->layout()->count() > 0)
                {
                    QLayoutItem *it = pWidget->layout()->itemAt(0);
                    QCheckBox *pQCheckBox = qobject_cast<QCheckBox *>(it->widget());
                    if (pQCheckBox != nullptr)
                    {
                        QString strLineID = ui->tableWidget->item(i, 2)->text().remove("HX");
                        int index = lineShowStates.indexOf(QRegExp(strLineID + ":.*"));
                        if (index >= 0)
                        {
                            const QStringList lineShowStatesValue = lineShowStates.at(index).split(":");
                            if (lineShowStatesValue.size() == 2 && lineShowStatesValue.at(1) == QStringLiteral("0"))
                            {
                                pQCheckBox->setChecked(false);
                            }
                            else
                            {
                                pQCheckBox->setChecked(true);
                            }
                        }

                        lineShowStatesTemp.append(strLineID + ":" + QString::number((pQCheckBox->checkState()  == Qt::Checked)));
                    }
                }
            }
        }
        else
        {
            for (int i = 0; i < ui->tableWidget->rowCount(); i++)
            {
                QWidget *pWidget = ui->tableWidget->cellWidget(i, 0);
                if (pWidget && pWidget->layout()->count() > 0)
                {
                    QLayoutItem *it = pWidget->layout()->itemAt(0);
                    QCheckBox *pQCheckBox = qobject_cast<QCheckBox *>(it->widget());
                    if (pQCheckBox != nullptr)
                    {
                        QString strLineID    = ui->tableWidget->item(i, 2)->text().remove("HX");
                        lineShowStatesTemp.append(strLineID + ":" + QString::number((pQCheckBox->checkState() == Qt::Checked)));
                    }
                }
            }
        }

        settings.beginGroup("LY_TASKMANAGE_DLGEDITLINE_ACTIVAT_MISSION_SETTINGS");
        settings.setValue("LY_TASKMANAGE_DLGEDITLINE_ACTIVATEID_SETTINGS",        strActivateID);
        settings.setValue("LY_TASKMANAGE_DLGEDITLINE_ACTIVATEMISSIONID_LINESHOWSTATES_SETTINGS", lineShowStatesTemp);
        settings.endGroup();
    }
}

void CDlgPlay::DealDLGPlayDeviationVoyage()
{
	if (!LY_TaskManage::Get().GetPlaySpeedControl()->b_CheckRouteSafety || m_bDeviation)
		return;

	m_bDeviation = true;
	if (m_VecUAVName.size() > 1)
	{
		if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr2("提示"), tr2("%1,%2 之间的距离小于安全距离，\n是否切换为定点模式？").arg(m_VecUAVName[0].c_str(),m_VecUAVName[1].c_str()), QMessageBox::Yes | QMessageBox::No)) {
			on_BT_Play_clicked();
		}
		m_VecUAVName.clear();
	}
	else if(!m_VecUAVName.empty())
	{
		if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr2("提示"), tr2("检测到无人机%1位置与预设不符，\n是否切换为定点模式？").arg(m_VecUAVName[0].c_str()), QMessageBox::Yes | QMessageBox::No)) {
			on_BT_Play_clicked();
		}
		m_VecUAVName.clear();
	}
}

void CDlgPlay::DealDLGPlayVoyageEditHeight()
{

}

Vector3D CDlgPlay::toCartesian(const QPointF& p)
{
	const double R = 6371000.0;
	double px = p.x() * M_PI / 180;
	double py = p.y() * M_PI / 180;
	double x = R*std::cos(py)*std::cos(px);
	double y = R*std::cos(py) * std::sin(px);
	double z = R*std::sin(py);
	return{ x,y,z };

}

Vector3D CDlgPlay::calculateVector(const QPointF& A, const QPointF& B)
{
	Vector3D cA = toCartesian(A);
	Vector3D cB = toCartesian(B);
	return{ cB.x - cA.x,cB.y - cA.y,cB.z - cA.z };
}

double CDlgPlay::calculateAngle(QPointF firstPoint1, QPointF firstPoint2, QPointF secondPoint1, QPointF secondPoint2)
{
	Vector3D ab = calculateVector(firstPoint1, firstPoint2);
	Vector3D cd = calculateVector(secondPoint1, secondPoint2);
	double dot = ab.angle(cd);
	double angle = dot * (180.0 / P);
	return angle;
}

double CDlgPlay::CalculatePointDistance(const QGeoCoordinate A, const QGeoCoordinate B)
{
	const double R = 6371000.0;
	double lat1Rad = A.latitude() * M_PI / 180.0;
	double lon1Rad = A.longitude() * M_PI / 180.0;
	double lat2Rad = B.latitude() * M_PI / 180.0;
	double lon2Rad = B.longitude() * M_PI / 180.0;
	double deltaLat = lat2Rad - lat1Rad;
	double deltaLon = lon2Rad - lon1Rad;
	double a = qSin(deltaLat / 2) * qSin(deltaLat / 2) + qCos(lat1Rad) * qCos(lat2Rad) * qSin(deltaLon / 2) * qSin(deltaLon / 2);
	double c = 2 * qAtan2(qSqrt(a), qSqrt(1 - a));
	return R * c;
}

void CDlgPlay::ForecastRouteTendency_FlyRule(QString UAVName, QString UAVName1, qnzkna::UAVRealTimeStateManage::NebulaUAVRealTimeStateMap UAVRealTimeStateMap)
{
	std::vector<std::pair<QString, QGeoCoordinate>> MapUAVRealLocation;
	std::vector<Vector3D> UAV_3D_1;
	for (auto& UAVTimeState : UAVRealTimeStateMap)
	{
		qnzkna::UAVRealTimeStateManage::RealTimeTelemetryInformation_t telemetryInfoLocation = UAVTimeState.second.m_telemetryInfo;
		QGeoCoordinate UAVTimeGeoPoint = QGeoCoordinate(telemetryInfoLocation.telemetryInfo.position.lat, telemetryInfoLocation.telemetryInfo.position.lng, telemetryInfoLocation.telemetryInfo.gpsHeigth);
		QGeoCoordinate CoverGeoPoint;
		bool isExist = false;
		for (auto coverUAV : m_mapLocatonCoverUAV)
		{
			if(coverUAV.first != UAVTimeState.first.c_str())
				continue;
			CoverGeoPoint = coverUAV.second;
			if(CoverGeoPoint == UAVTimeGeoPoint)
				continue;
			isExist = true;
			break;
		}
		if (isExist)
		{
			UAV_3D_1.push_back(calculateVector(QPointF(CoverGeoPoint.longitude(), CoverGeoPoint.latitude()), QPointF(UAVTimeGeoPoint.longitude(), UAVTimeGeoPoint.latitude())));
		}
	}
	if (UAV_3D_1.size() < 1)
		return;
	double angle = -1;

		angle = UAV_3D_1[0].angle(UAV_3D_1[1]) * (180.0 / P);

	double heightDiff = m_BaseRuleStu.nAltDiff / 2;
	std::string tipe1 = m_SystemSysNameAndID[tr3(UAVName)];
	std::string tipe2 = m_SystemSysNameAndID[tr3(UAVName1)];
	if (angle > 140.0 && m_mapAdjustUAVTelemetry.find(UAVName) == m_mapAdjustUAVTelemetry.end())
	{
		m_mapAdjustUAVTelemetry[UAVName] = UAVName1;
		m_mapUAVChangeHeight[tr3(UAVName)] = heightDiff;
		m_mapUAVChangeHeight[tr3(UAVName1)] = -heightDiff;
		m_mapHorizHeight[tr3(UAVName)] = 0;
		m_mapHorizHeight[tr3(UAVName1)] = 0;

		if (LY_TaskManage::Get().GetForecastTendency() != nullptr)
		{
			std::vector<std::string> uavNameVec;
			uavNameVec.push_back(m_SystemSysNameAndID[tr3(UAVName)]);
			uavNameVec.push_back(m_SystemSysNameAndID[tr3(UAVName1)]);
			LY_TaskManage::Get().GetForecastTendency()->Init(uavNameVec, m_BaseRuleStu.nAltDiff);
			LY_TaskManage::Get().GetForecastTendency()->show();
			LY_TaskManage::Get().GetForecastTendency()->raise();

		};

		QDateTime curdt = QDateTime::currentDateTime();
		QString strcurdt = curdt.toString("yyyy-MM-dd hh:mm:ss");
		auto lParam = new EventInfo(strcurdt, tr2("告警事件"), tr2("四旋翼无人机: %1, %2 触发空中碰撞告警，\n冲突消减规则：平台上升 / 下降(高度差 %3米)后恢复航点飞行").arg(tipe1.c_str(), tipe2.c_str(), QString::number(m_BaseRuleStu.nAltDiff)));
		LY_TaskManageInterface::Get().SendMsgToAll(IInterface::MSG_Command, 900200, lParam);

		m_UAVStatus.nSubductionEnd = 1;
		m_UAVStatus.nQuadrate = 2;
		m_UAVStatus.status = 0;
	}
	else if (angle < 120 && angle > 0)
	{
		if (m_VecAdjustUAVTelemetrySpeed.find(tr3(UAVName)) == m_VecAdjustUAVTelemetrySpeed.end() && m_VecAdjustUAVTelemetrySpeed.find(tr3(UAVName1)) == m_VecAdjustUAVTelemetrySpeed.end())
		{

			QDateTime curdt = QDateTime::currentDateTime();
			QString strcurdt = curdt.toString("yyyy-MM-dd hh:mm:ss");
			auto lParam = new EventInfo(strcurdt, tr2("告警事件"), tr2("四旋翼无人机:%1,%2 触发空中碰撞告警，\n冲突消减规则：等待避让(安全距离%3米)").arg(tipe1.c_str(), tipe2.c_str(), QString::number(m_BaseRuleStu.nUAVSafeDis)));
			LY_TaskManageInterface::Get().SendMsgToAll(IInterface::MSG_Command, 900200, lParam);
			m_UAVStatus.nSubductionEnd = 1;
			if (LY_TaskManage::Get().GetForecastTendency() != nullptr)
			{
				std::vector<std::string> uavNameVec;
				uavNameVec.push_back(tr3(UAVName));
				uavNameVec.push_back(tr3(UAVName1));
				LY_TaskManage::Get().GetForecastTendency()->Init(uavNameVec);
				LY_TaskManage::Get().GetForecastTendency()->show();
				LY_TaskManage::Get().GetForecastTendency()->raise();
			};
		}
	}
}

void CDlgPlay::setSelectShowLine(bool state)
{
	if (ui->checkBoxLines->checkState() == Qt::Checked)
	{
		m_bIsShowLines = true;
	}
	else
	{
		m_bIsShowLines = false;
	}
}

void CDlgPlay::drawPoint(QPainter & painter, IProj * pProj)
{
	if (m_bIsShowLines)
	{
		QPen pen(Qt::black, 3);
		painter.setPen(pen);

		QMutexLocker locker(&m_qmutex);
		auto lines = m_route_lines;
		auto lineItor = lines.begin();
		while (lineItor != lines.end())
		{
			for (int pIndex = 0; pIndex < lineItor->second.size(); pIndex++)
			{
				QPointF DrawPoint = pProj->Jwd2Screen(lineItor->second[pIndex].x, lineItor->second[pIndex].y);
				painter.setPen(pen);

				if (pIndex < lineItor->second.size() - 1)
				{
					QPointF DrawPoint1 = pProj->Jwd2Screen(lineItor->second[pIndex + 1].x, lineItor->second[pIndex + 1].y);
					painter.drawLine(DrawPoint, DrawPoint1);
				}
			}
			lineItor++;
		}
	}
}

void CDlgPlay::draw3DPathLine(std::map<std::string, std::vector<casic_vector3d>> &PointsVec, QColor pathColor, int lineWidth )
{

	if (m_draw3DFirst) {
		del3DPathLine();
		Path3DLines.clear();
	}

	IEarth* pEarth = IEarth::GetEarth();
	if (pEarth == nullptr)
	{
		return;
	}

	CCreateShape cs;
	auto pointsItor = PointsVec.begin();
	while (pointsItor != PointsVec.end())
	{
		std::vector<casic_vector3d> points = pointsItor->second;
		std::vector<double> pdLon;
		pdLon.clear();
		std::vector<double> pdLat;
		pdLat.clear();
		std::vector<double> pdHeight;
		pdHeight.clear();

		for (int pointIndex = 0; pointIndex < points.size(); pointIndex++)
		{
			pdLon.push_back(points[pointIndex].x);
			pdLat.push_back(points[pointIndex].y);

			pdHeight.push_back(points[pointIndex].z*10);

		}
		IUserMesh1 *pUM = nullptr;
		SHPObject *pShp = cs.CreatePolyline(pdLon.data(), pdLat.data(), pdLon.size(),
			100, false, nullptr, pdHeight.data());
		if (pShp != nullptr)
		{
			pUM = pEarth->GetGraphics()->CreateUserMesh1(pShp);
			if (pUM != nullptr)
			{
				pUM->SetColor(pathColor.rgb());
				pUM->SetState(IUserMesh::POLYGONMODE, IUserMesh::USERMESH_LINE);
				pUM->SetState(IUserMesh::LINEWIDTH, lineWidth);

				Path3DLines_plat.push_back(pUM);
			}
		}
		pointsItor++;
	}
}

void CDlgPlay::del3DPathLine()
{
	IEarth *pEarth = IEarth::GetEarth();
	for (int index = 0; index < Path3DLines.size(); index++)
	{
		pEarth->GetGraphics()->RemoveObject(Path3DLines[index]);
	}
	for (int index = 0; index < Path3DLines_plat.size(); index++)
	{
		pEarth->GetGraphics()->RemoveObject(Path3DLines_plat[index]);
	}

	return;
}

void CDlgPlay::on_LinesTimeOut()
{
	qnzkna::UAVRealTimeStateManage::NebulaUAVRealTimeStateMap nebulaUAVRealTimeStateMap;
	if (m_pIUAVRealTimeStateManageService->GetUAVRealTimeStateList(&nebulaUAVRealTimeStateMap))
	{

		for (const auto& nebulaUAVRealTimeState : nebulaUAVRealTimeStateMap)
		{

			std::vector<casic_vector3d>     LinePosList;
			auto itr = m_route_lines.find(nebulaUAVRealTimeState.first);
			if (itr!=m_route_lines.end()&&!nebulaUAVRealTimeState.second.m_telemetryInfo.isout)
			{
				LinePosList = m_route_lines[nebulaUAVRealTimeState.first];

				QGeoCoordinate QPoint1, QPoint2;
				if (LinePosList.empty())
				{
					QPoint1.setLongitude(nebulaUAVRealTimeState.second.m_telemetryInfo.telemetryInfo.position.lng);
					QPoint1.setLatitude(nebulaUAVRealTimeState.second.m_telemetryInfo.telemetryInfo.position.lat);
					QPoint1.setAltitude(nebulaUAVRealTimeState.second.m_telemetryInfo.telemetryInfo.gpsHeigth);
				}
				else
				{
					casic_vector3d endpoint = LinePosList.back();
					QPoint1.setLongitude(endpoint.x);
					QPoint1.setLatitude(endpoint.y);
					QPoint1.setAltitude(endpoint.z);
				}
				QPoint2.setLongitude(nebulaUAVRealTimeState.second.m_telemetryInfo.telemetryInfo.position.lng);
				QPoint2.setLatitude(nebulaUAVRealTimeState.second.m_telemetryInfo.telemetryInfo.position.lat);
				QPoint2.setAltitude(nebulaUAVRealTimeState.second.m_telemetryInfo.telemetryInfo.gpsHeigth);
				double lastAngle = QPoint1.azimuthTo(QPoint2);
				double lastDis = QPoint1.distanceTo(QPoint2);
				auto itrdis = m_mUAVbeginDis.find(nebulaUAVRealTimeState.first);
				if (itrdis == m_mUAVbeginDis.end()||itrdis->second==0)
				{
					m_mUAVbeginDis[nebulaUAVRealTimeState.first] = lastDis;
				}
				auto itr = m_mUAVbeginAngle.find(nebulaUAVRealTimeState.first);
				if (itr!= m_mUAVbeginAngle.end())
				{
					double tempdis = itrdis->second;

						m_mUAVbeginDis[nebulaUAVRealTimeState.first] = lastDis;
						if ((lastAngle == itr->second && !LinePosList.empty()))
						{
							LinePosList.pop_back();
							LinePosList.push_back(casic_vector3d(nebulaUAVRealTimeState.second.m_telemetryInfo.telemetryInfo.position.lng, nebulaUAVRealTimeState.second.m_telemetryInfo.telemetryInfo.position.lat, nebulaUAVRealTimeState.second.m_telemetryInfo.telemetryInfo.gpsHeigth));
						}
						else
						{
							m_mUAVbeginAngle[itr->first] = lastAngle;
							LinePosList.push_back(casic_vector3d(nebulaUAVRealTimeState.second.m_telemetryInfo.telemetryInfo.position.lng, nebulaUAVRealTimeState.second.m_telemetryInfo.telemetryInfo.position.lat, nebulaUAVRealTimeState.second.m_telemetryInfo.telemetryInfo.gpsHeigth));
						}

				}
				else
				{
					m_mUAVbeginAngle[nebulaUAVRealTimeState.first] = lastAngle;
				}
			}
			m_route_lines[nebulaUAVRealTimeState.first] = LinePosList;
		}
	}
	if (m_bIsShowLines)
	{
		draw3DPathLine(m_route_lines, Qt::green, 4);
	}
	else
	{

		if (m_draw3DFirst) {
			del3DPathLine();
			Path3DLines.clear();
		}
	}
}
