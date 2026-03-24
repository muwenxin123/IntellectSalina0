#include "uav_dispose_route_dialog.h"
#include "ui_uav_dispose_route_dialog.h"
#include "LY_CollaborativeSearchService.h"
#include "RoutePlanningDialog.h"

#include <QDatetime>

#include "LY_CollaborativeSearchServiceInterface.h"

#include <LyMessageBox.h>
#include "lib_ly_ccip/ly_ccip.h"
#include "track.h"
#include "GLES3/gl3.h"

#define SAFETY_DIS  20

UAV_DisPose_Route_Dialog::UAV_DisPose_Route_Dialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::UAV_DisPose_Route_Dialog)
{
	ui->setupUi(this);

	if (!isDispose_targetAndUAVS)
	{
		m_strDataDir = "../data/";
		m_strIconPatch = "Temp_Line/marker48X48_3D.png";
		m_strIconPatch_tou = "Temp_Line/toudandian.png";
		m_strIconPatch_fashe = "Temp_Line/fashedian.png";
		init();
		int indexComboBox_target_2 = ui->comboBox_target_2->currentIndex();
		int indexComboBox_target_3 = ui->comboBox_target_3->currentIndex();
		ui->comboBox_target_2->setCurrentIndex(0);
		ui->comboBox_target_3->setCurrentIndex(0);
		ui->comboBox_target_2->setCurrentIndex(indexComboBox_target_2);
		ui->comboBox_target_3->setCurrentIndex(indexComboBox_target_3);
	}
	else
	{
		init_forDisPose_TargetAndUAVS();
		connect(ui->pushButton_UAV, &QPushButton::clicked, this, &UAV_DisPose_Route_Dialog::slot_selectUAVPos_A2);
		connect(ui->pushButton_UAV_2, &QPushButton::clicked, this, &UAV_DisPose_Route_Dialog::slot_selectUAVPos_A3);
		connect(ui->pushButton, &QPushButton::clicked, this, &UAV_DisPose_Route_Dialog::slot_route_Self);
	}

}

UAV_DisPose_Route_Dialog::~UAV_DisPose_Route_Dialog()
{
	delete ui;
}

void UAV_DisPose_Route_Dialog::Update(qnzkna::TaskManage::INebulaTaskManageService * sub)
{
	if (!isDispose_targetAndUAVS)
	{
		init_targets();
		init_uav_online();
	}
}

void UAV_DisPose_Route_Dialog::init()
{

	routInfo_forA2A3[0].dispose_way = DISPOSE_THROW;
	routInfo_forA2A3[1].dispose_way = DISPOSE_FIRE;

	routInfo_forA2A3[0].disOrHei = ui->lineEdit->text().toInt();
	routInfo_forA2A3[1].disOrHei = ui->lineEdit_2->text().toInt();

	ui->pushButton_UAV->setCheckable(true);
	ui->pushButton_UAV_2->setCheckable(true);
	ui->pushButton_target_1->setCheckable(true);
	ui->pushButton_target_2->setCheckable(true);

	if (ui->checkBox->isChecked())
	{
		selectedRouteUAV[0] = 1;
	}
	if (ui->checkBox_2->isChecked())
	{
		selectedRouteUAV[1] = 1;
	}

	fireInfo.clear();
	int roucount = ui->tableWidget->rowCount();
	for (int i = 0; i < roucount; i++)
	{
		QTableWidgetItem* tempItem = ui->tableWidget->item(i, 0);
		if (tempItem->text().toInt() != 0)
		{
			QTableWidgetItem* tempItem1 = ui->tableWidget->item(i, 1);
			fireInfo[tempItem->text().toInt()] = tempItem1->text().toInt();
		}
	}

	init_targets();
	if (!isDispose_targetAndUAVS)
	{
		init_uav_online();
	}

	connect(ui->pushButton_UAV, &QPushButton::clicked, this, &UAV_DisPose_Route_Dialog::slot_selectUAVPos_A2);
	connect(ui->pushButton_UAV_2, &QPushButton::clicked, this, &UAV_DisPose_Route_Dialog::slot_selectUAVPos_A3);
	connect(ui->pushButton_target_1, &QPushButton::clicked, this, &UAV_DisPose_Route_Dialog::slot_selectMarkPos_A2);
	connect(ui->pushButton_target_2, &QPushButton::clicked, this, &UAV_DisPose_Route_Dialog::slot_selectMarkPos_A3);

	connect(ui->comboBox_target_2, &QComboBox::currentTextChanged, this, &UAV_DisPose_Route_Dialog::slot_selectMark_A2);
	connect(ui->comboBox_target_3, &QComboBox::currentTextChanged, this, &UAV_DisPose_Route_Dialog::slot_selectMark_A3);

	connect(ui->checkBox, &QCheckBox::stateChanged, this, &UAV_DisPose_Route_Dialog::slot_isRouteUAV_A2);
	connect(ui->checkBox_2, &QCheckBox::stateChanged, this, &UAV_DisPose_Route_Dialog::slot_isRouteUAV_A3);

	connect(ui->lineEdit_w, &QLineEdit::editingFinished, this, &UAV_DisPose_Route_Dialog::slot_putin_windDir);
	connect(ui->lineEdit_w_2, &QLineEdit::editingFinished, this, &UAV_DisPose_Route_Dialog::slot_putin_windDir1);

	connect(ui->pushButton_route, &QPushButton::clicked, this, &UAV_DisPose_Route_Dialog::slot_double_route);

	connect(ui->pushButton, &QPushButton::clicked, this, &UAV_DisPose_Route_Dialog::slot_route_Self);
	connect(ui->pushButton_up, &QPushButton::clicked, this, &UAV_DisPose_Route_Dialog::slot_routPath_up);

	qnzkna::TaskManage::INebulaTaskManageService* pTaskService = ICollaborativeSearchActivator::Get().getPTaskManageService();
	if (nullptr == pTaskService) {
		return;
	}

	ActivedActionID = ICollaborativeSearchActivator::Get().getPTaskManageService()->GetActivatingActionID();
	ICollaborativeSearchActivator::Get().getPTaskManageService()->ReLoadRelationMarkDB();
	currAction = ICollaborativeSearchActivator::Get().getPTaskManageService()->GetNebulaAction(ActivedActionID);
	QString currActionName = QString::fromStdString(currAction.GetName());

	auto activatingMissionVec = ICollaborativeSearchActivator::Get().getPTaskManageService()->GetActivatingMissions();
	if (!activatingMissionVec.empty())
	{
		for (auto ms : activatingMissionVec)
		{
			std::string nam1 = ms.GetName();
			QString tempQName = QString::fromStdString(nam1);
			if (tempQName.indexOf(tr2("罐")) != -1)
			{
				fireMissionIDS[0] = ms.GetID();
				fireMissions[0] = ms;
			}
			else if (tempQName.indexOf(tr2("弹")) != -1)
			{
				fireMissionIDS[1] = ms.GetID();
				fireMissions[1] = ms;
			}
			if (ms.GetType() == ENeubulaMissionType_Attack)
			{
				actionMissionVec.push_back(ms);
			}
		}

	}

}

void UAV_DisPose_Route_Dialog::init_targets()
{
	targetInfoVec.clear();
	ui->comboBox_target_2->clear();
	ui->comboBox_target_3->clear();

	ui->comboBox_target_2->addItem(" ");
	ui->comboBox_target_2->setCurrentIndex(0);
	ui->comboBox_target_3->addItem(" ");
	ui->comboBox_target_3->setCurrentIndex(0);

	if (ICollaborativeSearchActivator::Get().getPTaskManageService() == nullptr ||
		ICollaborativeSearchActivator::Get().getPIMarkManageService() == nullptr)
	{
		return;
	}

	int ActivedActionID = ICollaborativeSearchActivator::Get().getPTaskManageService()->GetActivatingActionID();

	std::vector<QString> targetIDs = ICollaborativeSearchActivator::Get().getPIMarkManageService()->GetMarkIDListFromDB_ActivatingAction();

	for (auto targetID : targetIDs)
	{
		auto targetTemp = ICollaborativeSearchActivator::Get().getPIMarkManageService()->GetMark(targetID.toStdString());
		if (targetTemp.GetID() != "" && targetTemp.GetType() == ESystemMarkType_Fire)
		{
			targetInfoVec.push_back(targetTemp);
			ui->comboBox_target_2->addItem(QString::fromStdString(targetTemp.GetName()));
			ui->comboBox_target_3->addItem(QString::fromStdString(targetTemp.GetName()));
		}
	}

	if (selectMarkNames[0] != " ")
	{
		if (ui->comboBox_target_2->findText(selectMarkNames[0]) != -1)
		{
			ui->comboBox_target_2->setCurrentIndex(ui->comboBox_target_2->findText(selectMarkNames[0]));
		}
		else
		{
			if (targetInfoVec.empty())
			{
				selectMarkNames[0] = " ";
				ui->comboBox_target_2->setCurrentIndex(0);
			}
			else
			{
				selectMarkNames[0] = QString::fromStdString(targetInfoVec[0].GetName());
				ui->comboBox_target_2->setCurrentIndex(1);
			}

		}
	}
	else
	{
		if (targetInfoVec.empty())
		{
			selectMarkNames[0] = " ";
			ui->comboBox_target_2->setCurrentIndex(0);
		}
		else
		{
			selectMarkNames[0] = QString::fromStdString(targetInfoVec[0].GetName());
			ui->comboBox_target_2->setCurrentIndex(1);
		}
	}

	if (selectMarkNames[1] != " ")
	{
		if (ui->comboBox_target_3->findText(selectMarkNames[1]) != -1)
		{
			ui->comboBox_target_3->setCurrentIndex(ui->comboBox_target_3->findText(selectMarkNames[1]));
		}
		else
		{
			if (targetInfoVec.empty())
			{
				selectMarkNames[1] = " ";
				ui->comboBox_target_3->setCurrentIndex(0);
			}
			else
			{
				selectMarkNames[1] = QString::fromStdString(targetInfoVec[0].GetName());
				ui->comboBox_target_3->setCurrentIndex(1);
			}

		}
	}
	else
	{
		if (targetInfoVec.empty())
		{
			selectMarkNames[1] = " ";
			ui->comboBox_target_3->setCurrentIndex(0);
		}
		else
		{
			selectMarkNames[1] = QString::fromStdString(targetInfoVec[0].GetName());
			ui->comboBox_target_3->setCurrentIndex(1);
		}
	}

}

void UAV_DisPose_Route_Dialog::init_uav_online()
{

	qnzkna::UAVRealTimeStateManage::IUAVRealTimeStateManageService* pIUAVRealTimeStateManageService = ICollaborativeSearchActivator::Get().getPIUAVRealTimeStateManageService();
	if (pIUAVRealTimeStateManageService == nullptr)
		return;

	qnzkna::UAVRealTimeStateManage::NebulaUAVRealTimeStateMap nebulaUAVRealTimeStateMap;
	if (pIUAVRealTimeStateManageService->GetUAVRealTimeStateList(&nebulaUAVRealTimeStateMap))
	{
		for (const auto& nebulaUAVRealTimeState : nebulaUAVRealTimeStateMap)
		{
			unsigned long long FoucsUAVID = std::stoull(nebulaUAVRealTimeState.first);

			if (FoucsUAVID == std::stoull(UAVIDs[0]))
			{
				routInfo_forA2A3[0].uavPos = casic_vector3d( nebulaUAVRealTimeState.second.m_telemetryInfo.telemetryInfo.position.lng,
					nebulaUAVRealTimeState.second.m_telemetryInfo.telemetryInfo.position.lat, nebulaUAVRealTimeState.second.m_telemetryInfo.telemetryInfo.gpsHeigth);
			}
			else if (FoucsUAVID == std::stoull(UAVIDs[1]))
			{
				routInfo_forA2A3[1].uavPos = casic_vector3d(nebulaUAVRealTimeState.second.m_telemetryInfo.telemetryInfo.position.lng,
					nebulaUAVRealTimeState.second.m_telemetryInfo.telemetryInfo.position.lat, nebulaUAVRealTimeState.second.m_telemetryInfo.telemetryInfo.gpsHeigth);
			}

		}
	}

}

void UAV_DisPose_Route_Dialog::drawPoint(QPainter & painter, IProj * pProj)
{
	QPen pen(Qt::red, 1);
	painter.setPen(pen);
	if (isDispose_targetAndUAVS)
	{
		auto lines = disposeTargetAndUAVS_process.getRoutedLines();
		auto lineItor = lines.begin();
		while (lineItor != lines.end())
		{
			for (int pIndex = 0; pIndex < lineItor->second.size(); pIndex++)
			{
				QPoint DrawPoint = pProj->Jwd2Screen(lineItor->second[pIndex].x, lineItor->second[pIndex].y);
				painter.setPen(pen);
				painter.drawPoint(DrawPoint);
				painter.drawText(DrawPoint, QString::number(pIndex));
				if (pIndex < lineItor->second.size() - 1)
				{
					QPoint DrawPoint1 = pProj->Jwd2Screen(lineItor->second[pIndex + 1].x, lineItor->second[pIndex + 1].y);
					painter.drawLine(DrawPoint, DrawPoint1);
				}
			}
			lineItor++;
		}
	}
	else
	{
		for (int i = 0; i < 2; i++)
		{
			if (i == 1)
				pen.setColor(Qt::yellow);
			if (selectedRouteUAV[i] == 1)
			{
				for (int pIndex = 0; pIndex < routeLine_forA2A3[i].size(); pIndex++)
				{
					QPoint DrawPoint = pProj->Jwd2Screen(routeLine_forA2A3[i][pIndex].x, routeLine_forA2A3[i][pIndex].y);
					painter.setPen(pen);
					painter.drawPoint(DrawPoint);
					painter.drawText(DrawPoint, QString::number(pIndex));
					if (pIndex < routeLine_forA2A3[i].size() - 1)
					{
						QPoint DrawPoint1 = pProj->Jwd2Screen(routeLine_forA2A3[i][pIndex + 1].x, routeLine_forA2A3[i][pIndex + 1].y);
						painter.drawLine(DrawPoint, DrawPoint1);
					}
				}
			}
		}
	}

}

void UAV_DisPose_Route_Dialog::drawUAV_MarkPoisnts(QPainter & painter, IProj * pProj)
{
	QPen pen(Qt::red, 2);
	painter.setPen(pen);
	if (routInfo_forA2A3[0].targetPos.x != 0)
	{
		QPoint targetPoint = pProj->Jwd2Screen(routInfo_forA2A3[0].targetPos.x, routInfo_forA2A3[0].targetPos.y);
		painter.drawPoint(targetPoint);
		painter.drawText(targetPoint, "Mark_M2");
	}

	if (routInfo_forA2A3[1].targetPos.x != 0)
	{
		QPoint targetPoint = pProj->Jwd2Screen(routInfo_forA2A3[1].targetPos.x, routInfo_forA2A3[1].targetPos.y);
		painter.drawPoint(targetPoint);
		painter.drawText(targetPoint, "Mark_M3");
	}

	pen.setColor(Qt::green);
	painter.setPen(pen);
	if (routInfo_forA2A3[0].uavPos.x != 0)
	{
		QPoint targetPoint = pProj->Jwd2Screen(routInfo_forA2A3[0].uavPos.x, routInfo_forA2A3[0].uavPos.y);
		painter.drawPoint(targetPoint);
		painter.drawText(targetPoint, "UAV_M2");
	}

	if (routInfo_forA2A3[1].uavPos.x != 0)
	{
		QPoint targetPoint = pProj->Jwd2Screen(routInfo_forA2A3[1].uavPos.x, routInfo_forA2A3[1].uavPos.y);
		painter.drawPoint(targetPoint);
		painter.drawText(targetPoint, "UAV_M3");
	}

}

void UAV_DisPose_Route_Dialog::drawUAV_Fire_3Test(QPainter & painter, IProj * pProj)
{
	QPen pen(Qt::red, 2);
	painter.setPen(pen);
	for(int i = 0; i < testPoints.size(); i++)
	{
		QPoint targetPoint = pProj->Jwd2Screen(testPoints[i].x, testPoints[i].y);
		painter.drawPoint(targetPoint);
		painter.drawText(targetPoint, "testPos" +QString::number(i));
	}
}

bool UAV_DisPose_Route_Dialog::deal_TrackInfo(int nLength, char * lpData)
{
	if (nLength <= 0)
	{
		return false;
	}

	if (nLength != sizeof(TrackInfo))
	{
		return false;
	}

	TrackInfo *pD = (TrackInfo *)lpData;

	if (pD->header.unNodeID == 110303)
	{
		if (routeLine_forA2A3[1].size() > 0 && selectedRouteUAV[1] == 1 && Boom_fire_ForM3_throw_isShow)
		{
			casic_vector3d curPos(pD->cmn.Longitude, pD->cmn.Latitude, pD->cmn.Altitude);
			double heading = pD->cmn.Course;
			double speed = pD->cmn.Speed;
			if (Boom_fire_ForM3_throw == nullptr)
			{
				QColor fireZoneColor(0, 0, 255, 150);
				draw3DFireBoom_For_M3_throw(curPos, heading, 0, fireZoneColor, 4);
			}
			else
			{
				QColor fireZoneColor(0, 0, 255, 150);
				change3DFireBoom_For_M3_throw(curPos, heading, fireZoneColor);
			}

			if (discoverZone_for_M3.empty())
			{
				QColor fireZoneColor(0, 0, 255, 80);
				draw3DDiscoverZone_for_M3(curPos, heading, ANGLE_PITCH_FOR_M3 + pD->cmn.Pitch, pD->cmn.Roll, DIS_POD_FOR_M3, fireZoneColor);
			}
			else
			{
				QColor fireZoneColor(Qt::yellow);
				fireZoneColor.setAlpha(80);
				change3DDiscoverZone_For_M3(curPos, heading, ANGLE_PITCH_FOR_M3 + pD->cmn.Pitch, pD->cmn.Roll, fireZoneColor);
			}
		}
	}
	else if (pD->header.unNodeID == 110302)
	{
		if (routeLine_forA2A3[0].size() > 0 && selectedRouteUAV[0] == 1 && Boom_fire_ForM2_throw_isShow)
		{
			casic_vector3d curPos(pD->cmn.Longitude, pD->cmn.Latitude, pD->cmn.Altitude);
			double heading = pD->cmn.Course;
			double speed = pD->cmn.Speed;

			if (discoverZone_for_M2.empty())
			{
				QColor fireZoneColor(0, 0, 255, 80);
				draw3DDiscoverZone_for_M2(curPos, heading, ANGLE_PITCH_FOR_M2 + pD->cmn.Pitch, pD->cmn.Roll, DIS_POD_FOR_M2, fireZoneColor);
			}
			else
			{
				QColor fireZoneColor(Qt::yellow);
				fireZoneColor.setAlpha(80);
				change3DDiscoverZone_For_M2(curPos, heading, ANGLE_PITCH_FOR_M2 + pD->cmn.Pitch, pD->cmn.Roll, fireZoneColor);
			}
		}
	}
	return true;

}

void UAV_DisPose_Route_Dialog::init_forDisPose_TargetAndUAVS()
{
	bindSignToSlot();

	init_UAVLists_ForDispose();
	init_TargetList_forDispose();

	qnzkna::TaskManage::INebulaTaskManageService* pTaskService = ICollaborativeSearchActivator::Get().getPTaskManageService();
	if (nullptr == pTaskService) {
		return;
	}

	ActivedActionID = ICollaborativeSearchActivator::Get().getPTaskManageService()->GetActivatingActionID();
	ICollaborativeSearchActivator::Get().getPTaskManageService()->ReLoadRelationMarkDB();
	currAction = ICollaborativeSearchActivator::Get().getPTaskManageService()->GetNebulaAction(ActivedActionID);
	QString currActionName = QString::fromStdString(currAction.GetName());

	auto activatingMissionVec = ICollaborativeSearchActivator::Get().getPTaskManageService()->GetActivatingMissions();
	if (!activatingMissionVec.empty())
	{
		for (auto ms : activatingMissionVec)
		{
			std::string nam1 = ms.GetName();
			QString tempQName = QString::fromStdString(nam1);
			if (tempQName.indexOf(tr2("罐")) != -1)
			{
				fireMissionIDS[0] = ms.GetID();
				fireMissions[0] = ms;
			}
			else if (tempQName.indexOf(tr2("弹")) != -1)
			{
				fireMissionIDS[1] = ms.GetID();
				fireMissions[1] = ms;
			}
			if (ms.GetType() == ENeubulaMissionType_Attack)
			{
				actionMissionVec.push_back(ms);
			}
		}

	}
}

void UAV_DisPose_Route_Dialog::init_UAVLists_ForDispose()
{
	std::vector<std::string> UAVIDS_fire = disposeTargetAndUAVS_process.getUAVIDS_Fire();
	std::vector<std::string> UAVIDS_throw = disposeTargetAndUAVS_process.getUAVIDS_throw();

	for (auto uavID_fire : UAVIDS_fire)
	{
		ui->comboBox_UAV_fire->addItem(QString::fromStdString(uavID_fire));
	}
	for (auto uavID_throw : UAVIDS_throw)
	{
		ui->comboBox_UAV_thorw->addItem(QString::fromStdString(uavID_throw));
	}
}

void UAV_DisPose_Route_Dialog::init_TargetList_forDispose()
{
	CSystemMarkInfoVec MarksInfo = disposeTargetAndUAVS_process.getMarks_All();
	for (auto markInfo : MarksInfo)
	{
		MarkName2ID[markInfo.GetName()] = markInfo.GetID();
		ui->comboBox_target_2->addItem(QString::fromStdString(markInfo.GetName()));
		ui->comboBox_target_3->addItem(QString::fromStdString(markInfo.GetName()));
	}
}

void UAV_DisPose_Route_Dialog::setUAVOriPos(casic_vector3d pos)
{
	switch (selectPos_For)
	{
	case SELECTPOS_UAV_Fire:
	{
		std::string uavID = ui->comboBox_UAV_fire->currentText().toStdString();
		disposeTargetAndUAVS_process.setUAVStartPos_handle(uavID, pos);
	}
	break;
	case SELECTPOS_UAV_Throw:
	{
		std::string uavID = ui->comboBox_UAV_thorw->currentText().toStdString();
		disposeTargetAndUAVS_process.setUAVStartPos_handle(uavID, pos);
	}
	break;
	}
}

void UAV_DisPose_Route_Dialog::bindSignToSlot()
{
	connect(ui->comboBox_UAV_thorw, &QComboBox::currentTextChanged, this, &UAV_DisPose_Route_Dialog::slot_uavIDChange_throw);
	connect(ui->comboBox_UAV_fire, &QComboBox::currentTextChanged, this, &UAV_DisPose_Route_Dialog::slot_uavIDChange_fire);
	connect(ui->comboBox_target_2, &QComboBox::currentTextChanged, this, &UAV_DisPose_Route_Dialog::slot_targetIDChange_throw);
	connect(ui->comboBox_target_3, &QComboBox::currentTextChanged, this, &UAV_DisPose_Route_Dialog::slot_targetIDChange_fire);
	connect(ui->lineEdit, &QLineEdit::textChanged, this, &UAV_DisPose_Route_Dialog::slot_changedThrowHei);
	connect(ui->lineEdit_2, &QLineEdit::textChanged, this, &UAV_DisPose_Route_Dialog::slot_changedFireDis);
	connect(ui->checkBox, &QCheckBox::stateChanged, this, &UAV_DisPose_Route_Dialog::slot_changedIsRoute_throw);
	connect(ui->checkBox_2, &QCheckBox::stateChanged, this, &UAV_DisPose_Route_Dialog::slot_changedIsRoute_Fire);
	connect(ui->pushButton_route, &QPushButton::clicked, this, &UAV_DisPose_Route_Dialog::slot_route_auto);
	connect(ui->pushButton_up, &QPushButton::clicked, this, &UAV_DisPose_Route_Dialog::slot_route_up_forUAVS);
	connect(ui->comboBox_target_4, &QComboBox::currentTextChanged, this, &UAV_DisPose_Route_Dialog::slot_changeMode_forThrow);
	connect(ui->comboBox_target_5, &QComboBox::currentTextChanged, this, &UAV_DisPose_Route_Dialog::slot_changeMode_forFire);
	connect(ui->pushButton_areachoose, &QPushButton::clicked, this, &UAV_DisPose_Route_Dialog::slot_areachoose_forUAVS);

	connect(&disposeTargetAndUAVS_process, SIGNAL(disposeTargetAndUAVS_process.signal_addaMarkID(QString)), this,
		SLOT(UAV_DisPose_Route_Dialog::slot_addMarkID(QString)));
	connect(&disposeTargetAndUAVS_process, SIGNAL(disposeTargetAndUAVS_process.signal_delaMarkID(QString)), this,
		SLOT(UAV_DisPose_Route_Dialog::slot_delMarkID(QString)));
}

void UAV_DisPose_Route_Dialog::slot_uavIDChange_throw()
{
	std::string nowUAVIDStr = ui->comboBox_UAV_thorw->currentText().toStdString();
	auto nameIDItor = MarkName2ID.find(ui->comboBox_target_2->currentText().toStdString());
	if (nameIDItor != MarkName2ID.end())
	{
		std::string nowTargetIDStr = nameIDItor->second;
		disposeTargetAndUAVS_process.addOrUpdateUAVforMark_unique(nowTargetIDStr, nowUAVIDStr);
	}
}

void UAV_DisPose_Route_Dialog::slot_uavIDChange_fire()
{
	std::string nowUAVIDStr = ui->comboBox_UAV_fire->currentText().toStdString();
	auto nameIDItor = MarkName2ID.find(ui->comboBox_target_3->currentText().toStdString());
	if (nameIDItor != MarkName2ID.end())
	{
		std::string nowTargetIDStr = nameIDItor->second;
		disposeTargetAndUAVS_process.addOrUpdateUAVforMark_unique(nowTargetIDStr, nowUAVIDStr);
	}
}

void UAV_DisPose_Route_Dialog::slot_targetIDChange_throw()
{
	std::string nowUAVIDStr = ui->comboBox_UAV_thorw->currentText().toStdString();
	auto nameIDItor = MarkName2ID.find(ui->comboBox_target_2->currentText().toStdString());
	if (nameIDItor != MarkName2ID.end())
	{
		std::string nowTargetIDStr = nameIDItor->second;
		disposeTargetAndUAVS_process.addOrUpdateUAVforMark_unique(nowTargetIDStr, nowUAVIDStr);
	}
}

void UAV_DisPose_Route_Dialog::slot_targetIDChange_fire()
{
	std::string nowUAVIDStr = ui->comboBox_UAV_fire->currentText().toStdString();
	auto nameIDItor = MarkName2ID.find(ui->comboBox_target_3->currentText().toStdString());
	if (nameIDItor != MarkName2ID.end())
	{
		std::string nowTargetIDStr = nameIDItor->second;
		disposeTargetAndUAVS_process.addOrUpdateUAVforMark_unique(nowTargetIDStr, nowUAVIDStr);
	}
}

void UAV_DisPose_Route_Dialog::slot_changedThrowHei()
{
	std::string uavID = ui->comboBox_UAV_thorw->currentText().toStdString();
	disposeTargetAndUAVS_process.changeUAV_throwHeiOrFireDis(uavID, ui->lineEdit->text().toInt());
}

void UAV_DisPose_Route_Dialog::slot_changedFireDis()
{
	std::string uavID = ui->comboBox_UAV_fire->currentText().toStdString();
	disposeTargetAndUAVS_process.changeUAV_throwHeiOrFireDis(uavID, ui->lineEdit_2->text().toInt());
}

void UAV_DisPose_Route_Dialog::slot_changedIsRoute_throw()
{
	std::string uavID = ui->comboBox_UAV_thorw->currentText().toStdString();
	if (ui->checkBox->isChecked())
	{
		std::string markId = MarkName2ID.find(ui->comboBox_target_2->currentText().toStdString())->second;
		disposeTargetAndUAVS_process.addOrUpdateUAVforMark_unique(markId, uavID);
	}
	else
	{
		disposeTargetAndUAVS_process.delUAVForAllMark(uavID);
	}
}

void UAV_DisPose_Route_Dialog::slot_changedIsRoute_Fire()
{
	std::string uavID = ui->comboBox_UAV_fire->currentText().toStdString();
	if (ui->checkBox_2->isChecked())
	{
		std::string markId = MarkName2ID.find(ui->comboBox_target_3->currentText().toStdString())->second;
		disposeTargetAndUAVS_process.addOrUpdateUAVforMark_unique(markId, uavID);
	}
	else
	{
		disposeTargetAndUAVS_process.delUAVForAllMark(uavID);
	}
}

void UAV_DisPose_Route_Dialog::slot_route_auto()
{
	slot_changedThrowHei();
	slot_changedFireDis();

	disposeTargetAndUAVS_process.setNoFlyZonePoints(LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->NoFlyZonePoints);
	disposeTargetAndUAVS_process.routeLines();
	auto lines = disposeTargetAndUAVS_process.getRoutedLines();
	draw3DPathLine_plan(lines,Qt::red, 3);

}

void UAV_DisPose_Route_Dialog::slot_route_up_forUAVS()
{
	disposeTargetAndUAVS_process.routeLinesUptoUAV();
}

void UAV_DisPose_Route_Dialog::slot_addMarkID(QString newMarkID)
{
	auto markService = ICollaborativeSearchActivator::Get().getPIMarkManageService();
	if (markService != NULL)
	{
		SystemMarkInfo newMarkInfo = markService->GetMark(newMarkID.toStdString());
		MarkName2ID[newMarkInfo.GetName()] = newMarkID.toStdString();
		ui->comboBox_target_2->addItem(QString::fromStdString(newMarkInfo.GetName()));
		ui->comboBox_target_3->addItem(QString::fromStdString(newMarkInfo.GetName()));
	}

}

void UAV_DisPose_Route_Dialog::slot_delMarkID(QString delMarkID)
{
	auto nameIdItor = MarkName2ID.begin();
	while (nameIdItor != MarkName2ID.end())
	{
		if (nameIdItor->second == delMarkID.toStdString())
		{
			int index = ui->comboBox_target_2->findText(QString::fromStdString(nameIdItor->first));
			if(index < ui->comboBox_target_2->count() && index >= 0)
				ui->comboBox_target_2->removeItem(index);
			index = ui->comboBox_target_3->findText(QString::fromStdString(nameIdItor->first));
			if(index < ui->comboBox_target_3->count() && index >= 0)
				ui->comboBox_target_3->removeItem(index);
		}
		nameIdItor++;
	}
}

void UAV_DisPose_Route_Dialog::slot_changeMode_forThrow()
{
	QString throwModeName = ui->comboBox_target_4->currentText();
	QString fireModeName = ui->comboBox_target_5->currentText();
	if (throwModeName == fireModeName)
	{
		if (throwModeName == QString::fromLocal8Bit("安全优先"))
			disposeTargetAndUAVS_process.setDispose_RouteFirst(DISPOSE_ROUTE::DISPOSE_ROUTE_FIRST_SAFE);
		else
			disposeTargetAndUAVS_process.setDispose_RouteFirst(DISPOSE_ROUTE::DISPOSE_ROUTE_FIRST_TIME);
	}
	else
	{
		ui->comboBox_target_5->setCurrentText(throwModeName);
	}
}

void UAV_DisPose_Route_Dialog::slot_changeMode_forFire()
{
	QString throwModeName = ui->comboBox_target_4->currentText();
	QString fireModeName = ui->comboBox_target_5->currentText();
	if (throwModeName == fireModeName)
	{
		if (throwModeName == QString::fromLocal8Bit("安全优先"))
			disposeTargetAndUAVS_process.setDispose_RouteFirst(DISPOSE_ROUTE::DISPOSE_ROUTE_FIRST_SAFE);
		else
			disposeTargetAndUAVS_process.setDispose_RouteFirst(DISPOSE_ROUTE::DISPOSE_ROUTE_FIRST_TIME);
	}
	else
	{
		ui->comboBox_target_4->setCurrentText(fireModeName);
	}
}

void UAV_DisPose_Route_Dialog::slot_areachoose_forUAVS()
{
	LY_CollaborativeSearchService::Get().m_UAVAreaChooseDialog->init();
	if (LY_CollaborativeSearchService::Get().m_UAVAreaChooseDialog->isHidden())
	{
		LY_CollaborativeSearchService::Get().m_UAVAreaChooseDialog->show();
	}

}

bool UAV_DisPose_Route_Dialog::CalculateTerrainVisibility(QGeoCoordinate npos, QGeoCoordinate tpos)
{
	double Distance = npos.distanceTo(tpos);

	double dir = npos.azimuthTo(tpos);

	QGeoCoordinate curr_npos = npos;
	QGeoCoordinate curr_tpos = tpos;

	double addDis = 10.0;

	double heiDis = npos.altitude() - tpos.altitude();
	double difDis = heiDis / (Distance / addDis);

	bool is_visibility = true;
	while (curr_npos.distanceTo(curr_tpos) > 10)
	{
		short int elevationTemp = LY_CollaborativeSearchService::Get().s_CGetHeight.
			GetHeight(curr_npos.longitude(), curr_npos.latitude());
		if (elevationTemp > heiDis + curr_tpos.altitude())
		{
			is_visibility = false;
			break;
		}
		curr_npos = curr_npos.atDistanceAndAzimuth(addDis, dir);
		heiDis -= difDis;
	}

	return is_visibility;
}

int UAV_DisPose_Route_Dialog::GetDayOfYear(int year, int month, int day)
{
	int DayOfYear;
	int FebDays;

	if ((year % 4 == 0 && year % 100 != 0) || (year % 400) == 0)
	{
		FebDays = 29;
	}
	else
	{
		FebDays = 28;
	}

	DayOfYear = day;
	if (month > 1) DayOfYear = DayOfYear + 31;
	if (month > 2) DayOfYear = DayOfYear + FebDays;
	if (month > 3) DayOfYear = DayOfYear + 31;
	if (month > 4) DayOfYear = DayOfYear + 30;
	if (month > 5) DayOfYear = DayOfYear + 31;
	if (month > 6) DayOfYear = DayOfYear + 30;
	if (month > 7) DayOfYear = DayOfYear + 31;
	if (month > 8) DayOfYear = DayOfYear + 31;
	if (month > 9) DayOfYear = DayOfYear + 30;
	if (month > 10) DayOfYear = DayOfYear + 31;
	if (month > 11) DayOfYear = DayOfYear + 30;

	return DayOfYear;
}

void UAV_DisPose_Route_Dialog::GetSolarAngle(int year, int month, int day, int hour, int min, double sec, double lon, double lat, int timeZone, int N, double & AzimuthAngle, double & ZenithAngle)
{
	double N0, sitar, ED, dLon, Et, Ho, viewAng;

	double dTimeAngle, gtdt, latitudeArc, latitude, HeightAngleArc;
	double AzimuthAngleArc, HANoon, TimeNoon, CosAzimuthAngle, HeightAngle;

	N0 = 79.6764 + 0.2422 * (year - 1985) - floor((year - 1985) / 4.0);

	sitar = 2 * PI *(N - N0) / 365.2422;

	ED = 0.3723 + 23.2567 * sin(sitar) + 0.1149 * sin(2 * sitar) - 0.1712 * sin(3 * sitar)
		- 0.758 * cos(sitar) + 0.3656 * cos(2 * sitar) + 0.0201 * cos(3 * sitar);

	ED = ED * PI / 180;

	dLon = 0.0;
	if (lon >= 0)
	{
		if (timeZone == -1)
		{
			dLon = lon - (floor((lon * 10 - 75) / 150) + 1) * 15.0;
		}
		else
		{
			dLon = timeZone *15.0 - lon;
		}
	}

	Et = 0.0028 - 1.9857 * sin(sitar) + 9.9059 * sin(2 * sitar) -
		7.0924 * cos(sitar) - 0.6882 * cos(2 * sitar);
	gtdt = hour + min / 60.0 + sec / 3600 + dLon / 15;
	gtdt = gtdt + Et / 60.0;
	dTimeAngle = 15.0 * (gtdt - 12);
	dTimeAngle = dTimeAngle * PI / 180;
	latitudeArc = lat * PI / 180;

	HeightAngleArc = asin(sin(latitudeArc)*sin(ED) + cos(latitudeArc)*cos(ED) *cos(dTimeAngle));
	CosAzimuthAngle = (sin(HeightAngleArc)*sin(latitudeArc) - sin(ED)) / cos(HeightAngleArc) / cos(latitudeArc);
	AzimuthAngleArc = acos(CosAzimuthAngle);
	HeightAngle = HeightAngleArc * 180 / PI;
	AzimuthAngle = AzimuthAngleArc * 180 / PI;

	if (dTimeAngle < 0)
	{
		AzimuthAngle = 180 - AzimuthAngle;
	}
	else
	{
		AzimuthAngle = 180 + AzimuthAngle;
	}
	ZenithAngle = 90 - HeightAngle;
}

double UAV_DisPose_Route_Dialog::calBen_Sun_Angle(casic_vector3d targetPos)
{
	QDateTime nowDateTime = QDateTime::currentDateTime();
	double AzimuthAngle, ZenithAngle;
	GetSolarAngle(nowDateTime.date().year(), nowDateTime.date().month(), nowDateTime.date().day(),
		nowDateTime.time().hour(), nowDateTime.time().minute(), nowDateTime.time().second(), targetPos.x, targetPos.y, 8,
		GetDayOfYear(nowDateTime.date().year(), nowDateTime.date().month(), nowDateTime.date().day()), AzimuthAngle, ZenithAngle);
	return AzimuthAngle;
}

void UAV_DisPose_Route_Dialog::LineTransToZone(std::vector<casic_vector3d> linePoints, std::vector<LLQPoints>& noflyZones)
{
	int safeDis = 15;
	for (int i = 0; i < linePoints.size() - 1; i++)
	{
		QGeoCoordinate firstPos(linePoints[i].y, linePoints[i].x);
		QGeoCoordinate secPos(linePoints[i + 1].y, linePoints[i + 1].x);
		double dir = firstPos.azimuthTo(secPos);
		LLQPoints zonePoints;
		QGeoCoordinate point1 = firstPos.atDistanceAndAzimuth(safeDis, dir - 135);
		QGeoCoordinate point2 = firstPos.atDistanceAndAzimuth(safeDis, dir + 135);
		QGeoCoordinate point3 = secPos.atDistanceAndAzimuth(safeDis, dir - 45);
		QGeoCoordinate point4 = secPos.atDistanceAndAzimuth(safeDis, dir + 45);
		zonePoints.push_back(QPointF(point1.longitude(), point1.latitude()));
		zonePoints.push_back(QPointF(point2.longitude(), point2.latitude()));
		zonePoints.push_back(QPointF(point3.longitude(), point3.latitude()));
		zonePoints.push_back(QPointF(point4.longitude(), point4.latitude()));
		noflyZones.push_back(zonePoints);
	}
}

std::vector<casic_vector3d> UAV_DisPose_Route_Dialog::routeLine(dispose_pos_way uavRouteInfo, std::vector<casic_vector3d> noflyLine)
{
	std::vector<casic_vector3d> routePath;
	routePath.clear();

	if (uavRouteInfo.uavPos.x == 0 || uavRouteInfo.targetPos.x == 0)
		return routePath;

	if (uavRouteInfo.dispose_way == DISPOSE_THROW)
	{
		uavRouteInfo.disOrHei = ui->lineEdit->text().toInt();

		UAV_Dispose_Route_Base route_base;

		auto noflyZonePointVec = LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->NoFlyZonePoints;
		route_base.setNoFlyZonePoints(noflyZonePointVec);
		QGeoCoordinate geoMarkPos(uavRouteInfo.targetPos.y, uavRouteInfo.targetPos.x);
		short int elevationTemp = LY_CollaborativeSearchService::Get().s_CGetHeight.
			GetHeight(geoMarkPos.longitude(), geoMarkPos.latitude());
		uavRouteInfo.targetPos.z = elevationTemp + uavRouteInfo.disOrHei * 1.0;
		route_base.Dispose_route(uavRouteInfo.uavPos, uavRouteInfo.targetPos, SAFETY_DIS);

		std::vector<casic_vector3d> tmpPathPoints;
		routePath.push_back(uavRouteInfo.targetPos);
		tmpPathPoints = route_base.getSearchMapPoints();
		for (auto tempPoint : tmpPathPoints)
		{
			routePath.push_back(tempPoint);
		}
		routePath.push_back(uavRouteInfo.uavPos);

		draw3DDropBoomArea(uavRouteInfo.targetPos.y, uavRouteInfo.targetPos.x, uavRouteInfo.targetPos.z, 5);
	}

	else if (uavRouteInfo.dispose_way == DISPOSE_FIRE)
	{

		uavRouteInfo.disOrHei = ui->lineEdit_2->text().toInt();

		int fireHei = fireAddHei;

		float fireHei_Cal = 20.0;
		if (fireInfo.find(uavRouteInfo.disOrHei) != fireInfo.end())
		{
			fireHei_Cal = fireInfo.find(uavRouteInfo.disOrHei)->second;
		}

		QGeoCoordinate geoMarkPos(uavRouteInfo.targetPos.y, uavRouteInfo.targetPos.x);
		short int elevationTemp = LY_CollaborativeSearchService::Get().s_CGetHeight.
			GetHeight(geoMarkPos.longitude(), geoMarkPos.latitude());
		geoMarkPos.setAltitude(elevationTemp);
		QGeoCoordinate geoUAVPos(uavRouteInfo.uavPos.y, uavRouteInfo.uavPos.x);

		double ben_Angle = geoMarkPos.azimuthTo(geoUAVPos);

		QGeoCoordinate firePos;

		for (int i = 0; i < 180; i++)
		{

			firePos = geoMarkPos.atDistanceAndAzimuth(uavRouteInfo.disOrHei, ben_Angle + i * 1);
			firePos.setAltitude(geoMarkPos.altitude() + fireHei_Cal);
			if (CalculateTerrainVisibility(firePos, geoMarkPos) )
			{
				ben_Angle += i;
				break;
			}
			else
			{
				firePos = geoMarkPos.atDistanceAndAzimuth(uavRouteInfo.disOrHei, ben_Angle + i * -1);
				firePos.setAltitude(geoMarkPos.altitude() + fireHei_Cal);
				if (CalculateTerrainVisibility(firePos, geoMarkPos) )
				{
					ben_Angle += i * -1;
					break;
				}
			}
		}

		QGeoCoordinate firePos_up(firePos.latitude(), firePos.longitude(), firePos.altitude() + fireHei);

		QGeoCoordinate routeLastMarkPoint = firePos_up.atDistanceAndAzimuth(10, ben_Angle);
		routeLastMarkPoint.setAltitude(firePos_up.altitude());

		double an = firePos_up.azimuthTo(routeLastMarkPoint);

		UAV_Dispose_Route_Base route_base;

		std::vector<LLQPoints> NoflyzonPoints = LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->NoFlyZonePoints;
		LLQPoints targetNoflyPoints;
		for (int i = 1; i <= 4; i++)
		{
			QGeoCoordinate noFlyPoint = geoMarkPos.atDistanceAndAzimuth(20, i * 45.0);
			targetNoflyPoints.push_back(QPointF(noFlyPoint.longitude(), noFlyPoint.latitude()));
		}
		NoflyzonPoints.push_back(targetNoflyPoints);

		if (selectedRouteUAV[0] == 1 && selectedRouteUAV[1] == 1)
		{
			int pathPointCount = noflyLine.size();

			if (pathPointCount >= 1 &&  noflyLine[pathPointCount - 1].z == firePos_up.altitude())
			{

				LineTransToZone(noflyLine, NoflyzonPoints);
			}
		}

		route_base.setNoFlyZonePoints(NoflyzonPoints);

		std::vector<casic_vector3d> pathPoints;
		route_base.Dispose_route(uavRouteInfo.uavPos, casic_vector3d(routeLastMarkPoint.longitude(), routeLastMarkPoint.latitude(), routeLastMarkPoint.altitude()),
			SAFETY_DIS);

		pathPoints = route_base.getSearchMapPoints();

		routePath.push_back(casic_vector3d(firePos.longitude(), firePos.latitude(), firePos.altitude()));

		routePath.push_back(casic_vector3d(firePos_up.longitude(), firePos_up.latitude(), firePos_up.altitude()));

		routePath.push_back(casic_vector3d(routeLastMarkPoint.longitude(), routeLastMarkPoint.latitude(), routeLastMarkPoint.altitude()));

		for (auto pointTemp : pathPoints)
		{
			routePath.push_back(pointTemp);
		}
		routePath.push_back(uavRouteInfo.uavPos);

		QColor fireZoneColor(0, 0, 255, 150);
		draw3DFireBoom_For_M3(casic_vector3d(firePos.longitude(), firePos.latitude(), firePos.altitude()), fireZoneColor, 4);
		draw3DFireBoom_For_M3_throw(casic_vector3d(firePos.longitude(), firePos.latitude(), firePos.altitude()), ben_Angle + 180, 0, fireZoneColor, 4);
		fireZoneColor.setRgba(qRgba(0, 0, 255, 80));
		draw3DDiscoverZone_for_M3(casic_vector3d(firePos.longitude(), firePos.latitude(), firePos.altitude()), ben_Angle + 180, ANGLE_PITCH_FOR_M3, 0, DIS_POD_FOR_M3, fireZoneColor);
	}
	return routePath;
}

double UAV_DisPose_Route_Dialog::calSpeed_dan(double dis, double hei)
{
	double speed = 0;

	double time_throw = sqrtf(2 * hei / 9.8);
	speed = dis / time_throw;

	return speed;
}

void UAV_DisPose_Route_Dialog::slot_selectUAV()
{
}

void UAV_DisPose_Route_Dialog::slot_selectUAVPos()
{
}

void UAV_DisPose_Route_Dialog::slot_selectMark()
{
}

void UAV_DisPose_Route_Dialog::slot_selectMarkPos()
{
}

void UAV_DisPose_Route_Dialog::slot_selectDan()
{
}

void UAV_DisPose_Route_Dialog::slot_putin_windDir()
{
	if (ui->lineEdit_w->text() == "")
	{
		isUseWind = false;
	}
	else if (ui->lineEdit_w->text().toInt() >= 0)
	{
		isUseWind = true;
	}
	wind_dir = ui->lineEdit_w->text().toInt();
	wind_dir %= 360;
}

void UAV_DisPose_Route_Dialog::slot_putin_windDir1()
{
	if (ui->lineEdit_w_2->text() == "")
	{
		isUseWind = false;
	}
	else if (ui->lineEdit_w_2->text().toInt() >= 0)
	{
		isUseWind = true;
	}
	wind_dir = ui->lineEdit_w_2->text().toInt();
	wind_dir %= 360;
}

void UAV_DisPose_Route_Dialog::slot_enterWindDir()
{
}

void UAV_DisPose_Route_Dialog::slot_putin_disOrHei()
{

}

void UAV_DisPose_Route_Dialog::slot_route()
{
	searchPoints.clear();
	dispose_pos_way disposeInfo;
	disposeInfo.uavPos = LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->curpos;
	disposeInfo.targetPos = targetPos;
	disposeInfo.dispose_way = dispose_way;

	if (isUseWind)
	{
		disposeInfo.FavDir = ui->lineEdit_w->text().toInt();
	}
	else
	{
		disposeInfo.FavDir = -1;
	}

	if (disposeInfo.dispose_way == DISPOSE_THROW)
	{
		UAV_Dispose_Route_Base route_base;

		auto noflyZonePointVec = LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->NoFlyZonePoints;
		route_base.setNoFlyZonePoints(noflyZonePointVec);
		QGeoCoordinate geoMarkPos(disposeInfo.targetPos.y, disposeInfo.targetPos.x);
		short int elevationTemp = LY_CollaborativeSearchService::Get().s_CGetHeight.
			GetHeight(geoMarkPos.longitude(), geoMarkPos.latitude());
		targetPos.z = elevationTemp + disposeInfo.disOrHei * 1.0;
		route_base.Dispose_route(disposeInfo.uavPos, targetPos, SAFETY_DIS);

		std::vector<casic_vector3d> tmpPathPoints;
		searchPoints.push_back(targetPos);
		tmpPathPoints = route_base.getSearchMapPoints();
		for (auto tempPoint : tmpPathPoints)
		{
			searchPoints.push_back(tempPoint);
		}
		searchPoints.push_back(disposeInfo.uavPos);
	}

	else if (disposeInfo.dispose_way == DISPOSE_FIRE)
	{

		int fireHei = 40;

		double ben_Angle = 0.0;
		if (isUseWind)
		{
			ben_Angle = wind_dir;
		}
		else
		{
			ben_Angle = calBen_Sun_Angle(disposeInfo.targetPos);
		}

		QGeoCoordinate geoMarkPos(disposeInfo.targetPos.y, disposeInfo.targetPos.x);
		short int elevationTemp = LY_CollaborativeSearchService::Get().s_CGetHeight.
			GetHeight(geoMarkPos.longitude(), geoMarkPos.latitude());
		geoMarkPos.setAltitude(elevationTemp);

		QGeoCoordinate firePos;

		for (int i = 0; i < 180; i++)
		{
			firePos = geoMarkPos.atDistanceAndAzimuth(disposeInfo.disOrHei, ben_Angle + i * 1);
			firePos.setAltitude(geoMarkPos.altitude() + fireHei);
			if (CalculateTerrainVisibility(firePos, geoMarkPos))
			{
				ben_Angle += i;
				break;
			}
			else
			{
				firePos = geoMarkPos.atDistanceAndAzimuth(disposeInfo.disOrHei, ben_Angle + i * -1);
				firePos.setAltitude(geoMarkPos.altitude() + fireHei);
				if (CalculateTerrainVisibility(firePos, geoMarkPos))
				{
					ben_Angle += i * -1;
					break;
				}
			}
		}

		QGeoCoordinate routeLastMarkPoint = firePos.atDistanceAndAzimuth(10, ben_Angle);
		routeLastMarkPoint.setAltitude(firePos.altitude());

		UAV_Dispose_Route_Base route_base;

		std::vector<LLQPoints> NoflyzonPoints = LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->NoFlyZonePoints;
		LLQPoints targetNoflyPoints;
		for (int i = 1; i <= 4; i++)
		{
			QGeoCoordinate noFlyPoint = geoMarkPos.atDistanceAndAzimuth(20, i * 45.0);
			targetNoflyPoints.push_back(QPointF(noFlyPoint.longitude(), noFlyPoint.latitude()));
		}
		NoflyzonPoints.push_back(targetNoflyPoints);

		route_base.setNoFlyZonePoints(NoflyzonPoints);

		std::vector<casic_vector3d> pathPoints;
		route_base.Dispose_route(disposeInfo.uavPos, casic_vector3d(routeLastMarkPoint.longitude(), routeLastMarkPoint.latitude(), routeLastMarkPoint.altitude()),
			SAFETY_DIS);

		pathPoints = route_base.getSearchMapPoints();

		searchPoints.push_back(casic_vector3d(firePos.longitude(), firePos.latitude(), firePos.altitude()));

		searchPoints.push_back(casic_vector3d(routeLastMarkPoint.longitude(), routeLastMarkPoint.latitude(), routeLastMarkPoint.altitude()));
		for (auto pointTemp : pathPoints)
		{
			searchPoints.push_back(pointTemp);
		}
		searchPoints.push_back(disposeInfo.uavPos);

	}

}

void UAV_DisPose_Route_Dialog::slot_SelectPos_State_Change()
{
	switch (selectPos_For)
	{
	case SELECTPOS_NO:
	{
		ui->pushButton_UAV->setChecked(false);
		ui->pushButton_UAV_2->setChecked(false);
		ui->pushButton_target_1->setChecked(false);
		ui->pushButton_target_2->setChecked(false);
	}break;
	case SELECTPOS_UAV_Throw:
	{
		ui->pushButton_UAV->setChecked(true);
		ui->pushButton_UAV_2->setChecked(false);
		ui->pushButton_target_1->setChecked(false);
		ui->pushButton_target_2->setChecked(false);
	}break;
	case SELECTPOS_UAV_Fire:
	{
		ui->pushButton_UAV->setChecked(false);
		ui->pushButton_UAV_2->setChecked(true);
		ui->pushButton_target_1->setChecked(false);
		ui->pushButton_target_2->setChecked(false);
	}break;
	case SELECTPOS_TAR_Throw:
	{
		ui->pushButton_UAV->setChecked(false);
		ui->pushButton_UAV_2->setChecked(false);
		ui->pushButton_target_1->setChecked(true);
		ui->pushButton_target_2->setChecked(false);
	}break;
	case SELECTPOS_TAR_Fire:
	{
		ui->pushButton_UAV->setChecked(false);
		ui->pushButton_UAV_2->setChecked(false);
		ui->pushButton_target_1->setChecked(false);
		ui->pushButton_target_2->setChecked(true);
	}break;
	}
}

void UAV_DisPose_Route_Dialog::slot_selectUAVPos_A2()
{
	if (selectPos_For != SELECTPOS_UAV_Throw)
	{
		selectPos_For = SELECTPOS_UAV_Throw;
	}
	else
	{
		selectPos_For = SELECTPOS_NO;
	}
	slot_SelectPos_State_Change();
}

void UAV_DisPose_Route_Dialog::slot_selectUAVPos_A3()
{
	if (selectPos_For != SELECTPOS_UAV_Fire)
	{
		selectPos_For = SELECTPOS_UAV_Fire;
	}
	else
	{
		selectPos_For = SELECTPOS_NO;
	}
	slot_SelectPos_State_Change();
}

void UAV_DisPose_Route_Dialog::slot_selectMark_A2()
{
	if (ui->comboBox_target_2->count() == 0)
		return;
	QString targetNowName = ui->comboBox_target_2->currentText();
	int targetNowIndex = ui->comboBox_target_2->currentIndex();
	selectMarkNames[0] = targetNowName;
	if (targetNowIndex > 0 && targetNowIndex <= targetInfoVec.size())
	{
		if (targetNowName == QString::fromStdString(targetInfoVec[targetNowIndex - 1].GetName()))
		{
			auto target_info = targetInfoVec[targetNowIndex - 1];
			short int elevationTemp = LY_CollaborativeSearchService::Get().s_CGetHeight.GetHeight(target_info.GetLon(), target_info.GetLat());
			if (elevationTemp < -100)
			{
				elevationTemp = target_info.GetHeight();
			}
			routInfo_forA2A3[0].targetPos = casic_vector3d(target_info.GetLon(), target_info.GetLat(), elevationTemp);
		}
	}
	if (targetNowIndex == 0)
	{
		routInfo_forA2A3[0].targetPos = casic_vector3d(0, 0, 0);
	}

    if(LY_CollaborativeSearchService::Get().m_SystemEventRecording){
        QDateTime curdt = QDateTime::currentDateTime();
        QString strcurdt = curdt.toString("yyyy-MM-dd hh:mm:ss");
        LY_CollaborativeSearchService::Get().m_SystemEventRecording->sltRecvEvent(EventInfo(strcurdt, tr2("操作事件"), tr2("M2 已选择目标")));
    }
}

void UAV_DisPose_Route_Dialog::slot_selectMark_A3()
{
	if (ui->comboBox_target_3->count() == 0)
		return;
	QString targetNowName = ui->comboBox_target_2->currentText();
	int targetNowIndex = ui->comboBox_target_2->currentIndex();
	selectMarkNames[1] = targetNowName;
	if (targetNowIndex > 0 && targetNowIndex <= targetInfoVec.size())
	{
		if (targetNowName == QString::fromStdString(targetInfoVec[targetNowIndex - 1].GetName()))
		{
			auto target_info = targetInfoVec[targetNowIndex - 1];
			short int elevationTemp = LY_CollaborativeSearchService::Get().s_CGetHeight.GetHeight(target_info.GetLon(), target_info.GetLat());
			if (elevationTemp < -100)
			{
				elevationTemp = target_info.GetHeight();
			}
			routInfo_forA2A3[1].targetPos = casic_vector3d(target_info.GetLon(), target_info.GetLat(), elevationTemp);
		}
	}
	if (targetNowIndex == 0)
	{
		routInfo_forA2A3[1].targetPos = casic_vector3d(0, 0, 0);
	}
    if(LY_CollaborativeSearchService::Get().m_SystemEventRecording){
	QDateTime curdt = QDateTime::currentDateTime();
	QString strcurdt = curdt.toString("yyyy-MM-dd hh:mm:ss");
	LY_CollaborativeSearchService::Get().m_SystemEventRecording->sltRecvEvent(EventInfo(strcurdt, tr2("操作事件"), tr2("M3 已选择目标")));
    }
}

void UAV_DisPose_Route_Dialog::slot_selectMarkPos_A2()
{
	if (selectPos_For != SELECTPOS_TAR_Throw)
	{
		selectPos_For = SELECTPOS_TAR_Throw;
	}
	else
	{
		selectPos_For = SELECTPOS_NO;
	}
	slot_SelectPos_State_Change();
}

void UAV_DisPose_Route_Dialog::slot_selectMarkPos_A3()
{
	if (selectPos_For != SELECTPOS_TAR_Fire)
	{
		selectPos_For = SELECTPOS_TAR_Fire;
	}
	else
	{
		selectPos_For = SELECTPOS_NO;
	}
	slot_SelectPos_State_Change();
}

void UAV_DisPose_Route_Dialog::slot_isRouteUAV_A2()
{
	QDateTime curdt = QDateTime::currentDateTime();
	QString strcurdt = curdt.toString("yyyy-MM-dd hh:mm:ss");

	if (ui->checkBox->isChecked())
	{
		selectedRouteUAV[0] = 1;
        if(LY_CollaborativeSearchService::Get().m_SystemEventRecording){
		LY_CollaborativeSearchService::Get().m_SystemEventRecording->sltRecvEvent(EventInfo(strcurdt, tr2("操作事件"), tr2("选择规划 M2 处置航线")));
	}
    }
	else
	{
		selectedRouteUAV[0] = 0;
        if(LY_CollaborativeSearchService::Get().m_SystemEventRecording){
		LY_CollaborativeSearchService::Get().m_SystemEventRecording->sltRecvEvent(EventInfo(strcurdt, tr2("操作事件"), tr2("取消规划 M2 处置航线")));
	}
    }

}

void UAV_DisPose_Route_Dialog::slot_isRouteUAV_A3()
{
	QDateTime curdt = QDateTime::currentDateTime();
	QString strcurdt = curdt.toString("yyyy-MM-dd hh:mm:ss");
	if (ui->checkBox_2->isChecked())
	{
		selectedRouteUAV[1] = 1;
        if(LY_CollaborativeSearchService::Get().m_SystemEventRecording){
		LY_CollaborativeSearchService::Get().m_SystemEventRecording->sltRecvEvent(EventInfo(strcurdt, tr2("操作事件"), tr2("选择规划 M3 处置航线")));
	}
    }
	else
	{
		selectedRouteUAV[1] = 0;
        if(LY_CollaborativeSearchService::Get().m_SystemEventRecording){
		LY_CollaborativeSearchService::Get().m_SystemEventRecording->sltRecvEvent(EventInfo(strcurdt, tr2("操作事件"), tr2("取消规划 M3 处置航线")));
	}
    }

}

void UAV_DisPose_Route_Dialog::slot_double_route()
{

	bool routeA2_A3 = false;
	if (selectedRouteUAV[0] == 1 && selectedRouteUAV[1] == 1)
		routeA2_A3 = true;

	QDateTime curdt = QDateTime::currentDateTime();
	QString strcurdt = curdt.toString("yyyy-MM-dd hh:mm:ss");

	if (selectedRouteUAV[0] == 1)
	{
		std::vector<casic_vector3d> routeLine_temp;
		routeLine_forA2A3[0] = routeLine(routInfo_forA2A3[0], routeLine_temp);
		draw3DPathLine(routeLine_forA2A3[0], Qt::red, 3);
        if(LY_CollaborativeSearchService::Get().m_SystemEventRecording){
		LY_CollaborativeSearchService::Get().m_SystemEventRecording->sltRecvEvent(EventInfo(strcurdt, tr2("操作事件"), tr2("M2 航线规划已完成")));
	}
    }

	if (selectedRouteUAV[1] == 1)
	{
		routeLine_forA2A3[1] = routeLine(routInfo_forA2A3[1], routeLine_forA2A3[0]);
		draw3DPathLine_M3(routeLine_forA2A3[1], Qt::yellow, 3);
        if(LY_CollaborativeSearchService::Get().m_SystemEventRecording){
		LY_CollaborativeSearchService::Get().m_SystemEventRecording->sltRecvEvent(EventInfo(strcurdt, tr2("操作事件"), tr2("M3 航线规划已完成")));
	}
    }
}

void UAV_DisPose_Route_Dialog::slot_route_Self()
{
	LY_CollaborativeSearchServiceInterface::Get().SendMsgToAll(IInterface::MSG_Command, 50009, nullptr);

}

void UAV_DisPose_Route_Dialog::slot_routPath_up()
{
    bool find_routPath_up = false;
	for (int i = 0; i < 2; i++)
	{

		std::vector<casic_vector3d> routLine = routeLine_forA2A3[i];
		std::string strMissionID = fireMissionIDS[i];
		CNeubulaMission fireMission = fireMissions[i];

		if (strMissionID == " " || fireMission.GetID() != strMissionID)
		{
			LyMessageBox::information(NULL, tr("Prompt"), tr("Please load the reconnaissance mission first"));
			continue;
		}

		currMissionPlatVec = fireMission.GetPlatInfoMap();
        vector<int> LineIDVec;
		CNeubulaMissionLineVec missionLineVec = ICollaborativeSearchActivator::Get().getPTaskManageService()->GetMissionLinesByTask(fireMission.GetID());

		for (auto line : missionLineVec)
		{
			int lineID = line.nID;

            LineIDVec.push_back(line.nID);
		}

		int index = 0;

		CNeubulaMissionLine mline;
		if (index < currMissionPlatVec.size())
		{

            QString missionID = (QString::fromStdString(fireMission.GetID()));
            strncpy(mline.strMissionID, missionID.toLocal8Bit().data(), MAX_LINE_LEN_1);

			mline.cType = 0;
			mline.cProtocolType = 0;
			mline.cShowFlag = 1;

            if(index < LineIDVec.size())
                mline.nID = LineIDVec[index];
            else{

                LyMessageBox::information(NULL, tr2("提示"), tr2("规划航线和配置无人机不匹配，请检查配置"));
                return ;
            }
            QString stName = ("HX");
			stName += QString::number(mline.nID);
			strncpy(mline.strName, stName.toLocal8Bit().toStdString().c_str(), MAX_LINE_LEN_1);
			int index_i = 0;
			if (routLine.empty())
				continue;

			std::vector<casic_vector3d> upLinePoints;
			upLinePoints.clear();
			int firePointIndex = 0;
			for (int index = routLine.size() - 1; index >= 0; index--)
			{
				upLinePoints.push_back(routLine[index]);
				firePointIndex++;
			}
			for (int j = 1; j < routLine.size(); j++)
			{
				upLinePoints.push_back(routLine[j]);
			}

			routLine.insert(routLine.begin(), routLine[0]);
			routLine.insert(routLine.begin(), routLine[0]);
			firePointIndex += 2;
			routLine.push_back(routLine[routLine.size() - 1]);

			int heiOrDis = routInfo_forA2A3[i].disOrHei;

			int hei = 0;
			int firePosHei = 0;
			if (i == 0)
			{
				hei = routInfo_forA2A3[i].disOrHei;
			}
			else
			{
				int dis = routInfo_forA2A3[i].disOrHei;
				if (fireInfo.find(dis) != fireInfo.end())
				{
					hei = fireInfo.find(dis)->second + fireAddHei;
					firePosHei = fireInfo.find(dis)->second;
				}
			}

			for (int indexP = 0; indexP < upLinePoints.size(); indexP++)
			{
				auto point = upLinePoints[indexP];
				CNeubulaMissionLinePoint mPoint;
				mPoint.nNo = index_i;
				mPoint.nLineID = mline.nID;
				mPoint.dLon = point.x;
				mPoint.dLat = point.y;
				mPoint.dAlt = hei;
				if (indexP == firePointIndex)
				{

                    mPoint.dHoverTime = 30;
					mPoint.cType = '5';
					if (i == 1)
					{
						mPoint.dAlt = firePosHei;
					}
				}
				mline.LinePointVec.push_back(mPoint);
				index_i++;
			}

            ICollaborativeSearchActivator::Get().getPTaskManageService()->EditMissionLine(mline);
		}
		++index;

		struct FireProt
		{
			bool UpdateRoute = true;
			bool UpDownRroute = true;
		}prot;

		void *lParam = (void *)(&prot);
		if (lParam == nullptr) {
			return;
		}

		QString missNameLike;

		if (i == 0)
		{
			missNameLike = tr2("罐");
		}
		else if(i == 1)
		{
			missNameLike = tr2("弹");
		}
		void * lParam1 = (void*)(&missNameLike);
		if (lParam1 == nullptr)
		{
			return;
		}

		QString strTask = QString::fromStdString(fireMissions[i].GetID());
		LY_CollaborativeSearchServiceInterface::Get().SendMsgToAll(IInterface::MSG_Command, 50011, (void*)(&strTask));
		LY_CollaborativeSearchServiceInterface::Get().SendMsgToAll(IInterface::MSG_MAP_IDENTIFY_MENU + 100, 50009, lParam);

        if(LY_CollaborativeSearchService::Get().m_SystemEventRecording){
		QDateTime curdt = QDateTime::currentDateTime();
		QString strcurdt = curdt.toString("yyyy-MM-dd hh:mm:ss");
		LY_CollaborativeSearchService::Get().m_SystemEventRecording->sltRecvEvent(EventInfo(strcurdt, tr2("操作事件"), tr2("处置任务规划-M") + QString::number(i + 2) + tr2(" 航线上传")));
        }

        find_routPath_up = true;
	}

    if(find_routPath_up){
        LY_CollaborativeSearchServiceInterface::Get().SendMsgToAll(IInterface::MSG_Command, 50009, nullptr);
    }

}

void UAV_DisPose_Route_Dialog::draw3DDropBoomArea(double dlatitude, double dlongitude, double dheight, double dradius)
{
	IEarth* pEarth = IEarth::GetEarth();
	if (pEarth == nullptr)
	{
		return;
	}
	for (int index = 0; index < dropBoomAreaVec.size(); index++)
	{
		pEarth->GetGraphics()->RemoveObject(dropBoomAreaVec[index]);
	}
	dropBoomAreaVec.clear();
	for (int index = 0; index < dropBoomAreaTextVec.size(); index++)
	{
		pEarth->GetGraphics()->RemoveObject(dropBoomAreaTextVec[index]);
	}
	dropBoomAreaTextVec.clear();

	QColor colorBombTrack(0xffff00);
	colorBombTrack.setAlpha(200);
	QColor colorTemp;
	colorTemp.setAlpha(60);
	colorTemp.setRgb(colorBombTrack.blue(), colorBombTrack.green(), colorBombTrack.red(), colorTemp.alpha());

	CCreateShape cs;
	SHPObject *pShp = cs.CreateCircle(0, 0, dradius, 1e6);
	if (pShp == nullptr)
		return;

	IUserMesh2 *pUserMesh = pEarth->GetGraphics()->CreateUserMesh2(pShp);

	pUserMesh->SetColor(colorTemp.rgba());
	pUserMesh->SetPosition(dlongitude, dlatitude, dheight);
	dropBoomAreaVec.push_back(pUserMesh);

	short int elevationTemp = LY_CollaborativeSearchService::Get().s_CGetHeight.
		GetHeight(dlongitude, dlatitude);

	double dZ = 0;
	double dM = abs(dheight - elevationTemp);
	SHPObject *pShp1 = cs.AddZM2SHPObject(true, pShp, &dZ, &dM);
	pUserMesh = pEarth->GetGraphics()->CreateUserMesh2(pShp1);

	pUserMesh->SetColor(colorTemp.rgba());
	pUserMesh->SetState(IUserMesh::LIGHTMODEL, 1);
	pUserMesh->SetState(IUserMesh::DEPTH, 1);

	pUserMesh->SetPosition(dlongitude, dlatitude, elevationTemp);
	dropBoomAreaVec.push_back(pUserMesh);

	pUserMesh = pEarth->GetGraphics()->CreateUserMesh2(pShp1);
	if (pUserMesh != nullptr)
	{

		pUserMesh->SetColor(colorTemp.rgba());
		pUserMesh->SetState(IUserMesh::POLYGONMODE, IUserMesh::USERMESH_LINE);
		pUserMesh->SetState(IUserMesh::LINEWIDTH, 1);
		pUserMesh->SetState(IUserMesh::LIGHTMODEL, 1);

		pUserMesh->SetPosition(dlongitude, dlatitude, elevationTemp);
		dropBoomAreaVec.push_back(pUserMesh);
	}

	colorTemp.setAlpha(200);
	IText* mshpText = pEarth->GetGraphics()->CreateText("投弹区", dlongitude, dlatitude, dheight, NULL, 22, colorTemp.rgba());
	if (mshpText != nullptr)
	{
		dropBoomAreaTextVec.push_back(mshpText);
	}
}

void UAV_DisPose_Route_Dialog::draw3DAreaOnSurface(double dlongitude, double dlatitude, double dheight, double dradius, IUserMesh2* pUserMesh)
{
	IEarth* pEarth = IEarth::GetEarth();
	if (pEarth == nullptr)
	{
		return;
	}

	QColor colorTemp;
	colorTemp.setAlpha(160);

	colorTemp.setRgb(255, 255, 255, colorTemp.alpha());

	CCreateShape cs;

	SHPObject *pShp = cs.CreateCircle(0, 0, dradius, 6000, true, nullptr, nullptr);
	if (pShp == nullptr)
		return;

	pUserMesh = pEarth->GetGraphics()->CreateUserMesh2(pShp);
	pUserMesh->SetPosition(dlongitude, dlatitude, dheight);

}

void UAV_DisPose_Route_Dialog::draw3DPathLine( std::vector<casic_vector3d>& PointsVec, QColor pathColor, int lineWidth)
{

	if (m_draw3DFirst) {
		del3DPathLine();
		Path3DLines.clear();
	}

	if (!Path3DLines_surface.empty())
	{
		IEarth *pEarth = IEarth::GetEarth();
		for (int index = 0; index < Path3DLines_surface.size(); index++)
		{
			pEarth->GetGraphics()->RemoveObject(Path3DLines_surface[index]);
		}
		Path3DLines_surface.clear();
	}
	if (!Path3DLines_label.empty())
	{
		IEarth *pEarth = IEarth::GetEarth();
		for (int index = 0; index < Path3DLines_label.size(); index++)
		{
			pEarth->GetGraphics()->RemoveObject(Path3DLines_label[index]);
		}
		Path3DLines_label.clear();
	}

	IEarth* pEarth = IEarth::GetEarth();
	if (pEarth == nullptr)
	{
		return;
	}

	CCreateShape cs;

		std::vector<casic_vector3d> points = PointsVec;
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
			pdHeight.push_back(points[pointIndex].z);

			short int elevationTemp = LY_CollaborativeSearchService::Get().s_CGetHeight.
				GetHeight(points[pointIndex].x, points[pointIndex].y);

			double pdLon1[2] = { points[pointIndex].x   , points[pointIndex].x };
			double pdLat1[2] = { points[pointIndex].y, points[pointIndex].y };
			double pdHeight1[2] = { pdHeight[pointIndex] * 1.0, elevationTemp * 1.0 };

			IUserMesh1	*pUM1 = nullptr;
			CCreateShape cs1;
			SHPObject *pShp1 = cs1.CreatePolyline(pdLon1, pdLat1, 2, 100, false, nullptr, pdHeight1);
			if (pShp1 != nullptr) {
				pUM1 = pEarth->GetGraphics()->CreateUserMesh1(pShp1);
				if (pUM1 != nullptr) {
					QColor col(255, 255, 255, 100);
					pUM1->SetColor(col.rgba());

					pUM1->SetState(IUserMesh::LINEWIDTH, 2);
					pUM1->SetState(IUserMesh::LINESTIPPLE, 4);
					Path3DLines.push_back(pUM1);
				}
			}

			IUserMesh2	*pUM2 = nullptr;
			SHPObject *pShp2 = cs1.CreateCircle(0, 0, 2, 1e6);
			if (pShp2 != nullptr) {
				pUM2 = pEarth->GetGraphics()->CreateUserMesh2(pShp2);
				if (pUM2 != nullptr) {
					QColor col(255, 255, 255, 160);
					pUM2->SetColor(col.rgba());
					pUM2->SetPosition(points[pointIndex].x, points[pointIndex].y, elevationTemp);
					Path3DLines_surface.push_back(pUM2);
				}
			}

			const QByteArray &ba = QUuid::createUuid().toRfc4122();
			QUuid uid = QUuid::fromRfc4122(QByteArray::fromRawData(ba, 16));
			QString strPathOld;
			if (pointIndex == 0)
			{
				strPathOld = QObject::tr(m_strDataDir) + m_strIconPatch_tou;
			}
			else
			{
				strPathOld = QObject::tr(m_strDataDir) + m_strIconPatch;
			}
			QFile currenFile(strPathOld);
			QString strPath = QObject::tr(m_strDataDir);
			strPath += QLatin1String("temp/");
			strPath += uid.toString(QUuid::WithoutBraces);
			strPath += QLatin1String(".png");
			currenFile.copy(strPath);
			std::string strImg = tr3(strPath).data();
			QString strLable;

			strLable = QString::number(points.size() - pointIndex);

			ILabel *pLabel = pEarth->GetGraphics()->CreateLabel(strLable.toLocal8Bit(), strImg.c_str(), points[pointIndex].x,
				points[pointIndex].y, points[pointIndex].z, 16, 16, 0);
			if (pLabel != nullptr) {
				pLabel->SetColor(0xffffffff, 0xffffffff, 0xffffffff);
				pLabel->SetFontSize(15);
				pLabel->SetTagState(true);
				pLabel->SetText(strLable.toLocal8Bit());
				Path3DLines_label.push_back(pLabel);

			}

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
				pUM->SetState(IUserMesh::LIGHT, 1);
				Path3DLines.push_back(pUM);
			}

			pUM = pEarth->GetGraphics()->CreateUserMesh1(pShp);
			if (pUM != nullptr)
			{
				pUM->SetColor(pathColor.rgb());
				pUM->SetState(IUserMesh::POLYGONMODE, IUserMesh::USERMESH_POINT);
				pUM->SetState(IUserMesh::POINT, lineWidth * 2);
				Path3DLines.push_back(pUM);
			}
		}
}

void UAV_DisPose_Route_Dialog::draw3DPathLine_M3(std::vector<casic_vector3d>& PointsVec, QColor pathColor, int lineWidth)
{
	IEarth* pEarth = IEarth::GetEarth();
	if (pEarth == nullptr)
	{
		return;
	}

	for (int i = 0; i < path3DLines_M3.size(); i++)
	{
		pEarth->GetGraphics()->RemoveObject(path3DLines_M3[i]);
	}
	path3DLines_M3.clear();

	for (int j = 0; j < Path3DLines_surface_M3.size(); j++)
	{
		pEarth->GetGraphics()->RemoveObject(Path3DLines_surface_M3[j]);
	}
	Path3DLines_surface_M3.clear();

	for (int j = 0; j < Path3DLines_label_M3.size(); j++)
	{
		pEarth->GetGraphics()->RemoveObject(Path3DLines_label_M3[j]);
	}
	Path3DLines_label_M3.clear();

	CCreateShape cs;

	std::vector<casic_vector3d> points = PointsVec;
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
		pdHeight.push_back(points[pointIndex].z);

		short int elevationTemp = LY_CollaborativeSearchService::Get().s_CGetHeight.
			GetHeight(points[pointIndex].x, points[pointIndex].y);

		double pdLon1[2] = { points[pointIndex].x   , points[pointIndex].x };
		double pdLat1[2] = { points[pointIndex].y, points[pointIndex].y };
		double pdHeight1[2] = { pdHeight[pointIndex] * 1.0, elevationTemp * 1.0 };

		IUserMesh1* pum1;
		CCreateShape cs1;
		SHPObject *pShp1 = cs1.CreatePolyline(pdLon1, pdLat1, 2, 100, false, nullptr, pdHeight1);
		if (pShp1 != nullptr) {
			pum1 = pEarth->GetGraphics()->CreateUserMesh1(pShp1);
			if (pum1 != nullptr) {
				QColor col(255, 255, 255, 100);
				pum1->SetColor(col.rgba());

				pum1->SetState(IUserMesh::LINEWIDTH, 2);
				pum1->SetState(IUserMesh::LINESTIPPLE, 6);
				path3DLines_M3.push_back(pum1);
			}
		}

		IUserMesh2	*pUM2 = nullptr;
		SHPObject *pShp2 = cs1.CreateCircle(0, 0, 2, 1e6);
		if (pShp2 != nullptr) {
			pUM2 = pEarth->GetGraphics()->CreateUserMesh2(pShp2);
			if (pUM2 != nullptr) {
				QColor col(255, 255, 255, 100);
				pUM2->SetColor(col.rgba());
				pUM2->SetPosition(points[pointIndex].x, points[pointIndex].y, elevationTemp);
				Path3DLines_surface.push_back(pUM2);
			}
		}

		const QByteArray &ba = QUuid::createUuid().toRfc4122();
		QUuid uid = QUuid::fromRfc4122(QByteArray::fromRawData(ba, 16));
		QString strPathOld;
		if (pointIndex == 0)
		{
			strPathOld = QObject::tr(m_strDataDir) + m_strIconPatch_fashe;
		}
		else
		{
			strPathOld = QObject::tr(m_strDataDir) + m_strIconPatch;
		}
		QFile currenFile(strPathOld);
		QString strPath = QObject::tr(m_strDataDir);
		strPath += QLatin1String("temp/");
		strPath += uid.toString(QUuid::WithoutBraces);
		strPath += QLatin1String(".png");
		currenFile.copy(strPath);
		std::string strImg = tr3(strPath).data();
		QString strLable;

		strLable = QString::number(points.size() - pointIndex);

		ILabel *pLabel = pEarth->GetGraphics()->CreateLabel(strLable.toLocal8Bit(), strImg.c_str(), points[pointIndex].x,
			points[pointIndex].y, points[pointIndex].z, 16, 16, 0);
		if (pLabel != nullptr) {
			pLabel->SetColor(0xffffffff, 0xffffffff, 0xffffffff);
			pLabel->SetFontSize(15);
			pLabel->SetTagState(true);
			pLabel->SetText(strLable.toLocal8Bit());
			Path3DLines_label_M3.push_back(pLabel);
		}

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
			path3DLines_M3.push_back(pUM);
		}
	}
}

void UAV_DisPose_Route_Dialog::draw3DPathLine_plan(std::map<std::string, std::vector<casic_vector3d>> &PointsVec, QColor pathColor, int lineWidth )
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

	QColor colorTemp[2];
	colorTemp[0] = QColor(Qt::green);
	colorTemp[1] = QColor(3,97,255,255);

	int indexColor = 0;
	auto pointsItor = PointsVec.begin();
	while (pointsItor != PointsVec.end())
	{

		if (indexColor > 1)
		{
			indexColor = 0;
		}
		QColor color(colorTemp[indexColor]);
		indexColor++;
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

			pdHeight.push_back(points[pointIndex].z);

			double pdLon1[2] = { points[pointIndex].x, points[pointIndex].x };
			double pdLat1[2] = { points[pointIndex].y, points[pointIndex].y };
			double pdHeight1[2] = { pdHeight[pointIndex] * 1.0, 0 };

			IUserMesh1	*pUM1 = nullptr;
			CCreateShape cs1;
			SHPObject *pShp1 = cs1.CreatePolyline(pdLon1, pdLat1, 2, 100, false, nullptr, pdHeight1);
			if (pShp1 != nullptr) {
				pUM1 = pEarth->GetGraphics()->CreateUserMesh1(pShp1);
				if (pUM1 != nullptr) {
					QColor col(255, 255, 255, 100);
					pUM1->SetColor(color.rgba());
					pUM1->SetState(IUserMesh::LINESTIPPLE, 4);
					pUM1->SetState(IUserMesh::LINEWIDTH, 2);
					pUM1->SetState(GL_DEPTH_TEST, 1);
					Path3DLines.push_back(pUM1);
				}
			}

			IUserMesh2	*pUM2 = nullptr;
			SHPObject *pShp2 = cs1.CreateCircle(0, 0, 2, 1e6);
			if (pShp2 != nullptr) {
				pUM2 = pEarth->GetGraphics()->CreateUserMesh2(pShp2);
				if (pUM2 != nullptr) {
					QColor col(255, 255, 255, 160);
					pUM2->SetColor(color.rgba());
					pUM2->SetState(GL_DEPTH_TEST, 1);
					pUM2->SetPosition(points[pointIndex].x, points[pointIndex].y, 0);
					Path3DLines_plat.push_back(pUM2);
				}
			}

		}
		IUserMesh1 *pUM = nullptr;
		SHPObject *pShp = cs.CreatePolyline(pdLon.data(), pdLat.data(), pdLon.size(),
			100, false, nullptr, pdHeight.data());
		if (pShp != nullptr)
		{
			pUM = pEarth->GetGraphics()->CreateUserMesh1(pShp);
			if (pUM != nullptr)
			{
				pUM->SetColor(color.rgb());
				pUM->SetState(IUserMesh::POLYGONMODE, IUserMesh::USERMESH_LINE);
				pUM->SetState(IUserMesh::LINEWIDTH, lineWidth);
				pUM->SetState(GL_DEPTH_TEST, 1);
				Path3DLines_plat.push_back(pUM);
			}
		}
		pointsItor++;
	}
}

void UAV_DisPose_Route_Dialog::del3DPathLine()
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

void UAV_DisPose_Route_Dialog::del3DstdIUserMesh1s(std::vector<IUserMesh1*>& mesh1s)
{
	IEarth *pEarth = IEarth::GetEarth();
	for (int index = 0; index < mesh1s.size(); index++)
	{
		pEarth->GetGraphics()->RemoveObject(mesh1s[index]);
	}
	mesh1s.clear();

	return;
}

void UAV_DisPose_Route_Dialog::del3DstdIUserMesh2s(std::vector<IUserMesh2*>& mesh2s)
{
	IEarth *pEarth = IEarth::GetEarth();
	for (int index = 0; index < mesh2s.size(); index++)
	{
		pEarth->GetGraphics()->RemoveObject(mesh2s[index]);
	}
	mesh2s.clear();

	return;
}

void UAV_DisPose_Route_Dialog::draw3DFireBoom_For_M3(casic_vector3d firePos, QColor zoneColor, int ZoneSize)
{
	IEarth* pEarth = IEarth::GetEarth();
	if (pEarth == nullptr)
	{
		return;
	}
	del3DstdIUserMesh1s(Boom_fire_ForM3);

	CCreateShape cs1;

	std::vector<double> pdLon;
	pdLon.clear();
	std::vector<double> pdLat;
	pdLat.clear();
	std::vector<double> pdHeight;
	pdHeight.clear();

	if (LY_CollaborativeSearchService::Get().m_RoutePlanningDialog != nullptr)
	{
		std::vector<casic_vector3d> edpoints = LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->getEndPoints(firePos, ZoneSize);
		for (auto area : edpoints)
		{
			pdLon.push_back(area.x);
			pdLat.push_back(area.y);

			pdHeight.push_back(firePos.z + ZoneSize / 2);
		}
	}

	IUserMesh1* pum;
	SHPObject *pShp = cs1.CreatePolygon(pdLon.data(), pdLat.data(), pdLon.size(),
		100, true, nullptr, pdHeight.data());
	if (pShp != nullptr)
	{
		pum = pEarth->GetGraphics()->CreateUserMesh1(pShp);
		if (pum != nullptr)
		{
			pum->SetColor(zoneColor.rgba());
			pum->SetState(IUserMesh::POLYGONMODE, IUserMesh::USERMESH_FILL);
			pum->SetState(IUserMesh::LINEWIDTH, 2);
			Boom_fire_ForM3.push_back(pum);
			double dZ = firePos.z - ZoneSize / 2, dM = ZoneSize;
			SHPObject *pShp1 = cs1.AddZM2SHPObject(true, pShp, &dZ, &dM);
			IUserMesh1 *pUserMesh = pEarth->GetGraphics()->CreateUserMesh1(pShp1);
			pUserMesh->SetColor(zoneColor.rgba());
			Boom_fire_ForM3.push_back(pUserMesh);

			pUserMesh = pEarth->GetGraphics()->CreateUserMesh1(pShp1);
			pUserMesh->SetColor(zoneColor.rgba());
			pUserMesh->SetState(IUserMesh::POLYGONMODE, IUserMesh::USERMESH_LINE);
			pUserMesh->SetState(IUserMesh::LINEWIDTH, 3);
			pUserMesh->SetState(IUserMesh::DEPTH, 1);
			Boom_fire_ForM3.push_back(pUserMesh);
		}
	}
}

void UAV_DisPose_Route_Dialog::draw3DFireBoom_For_M3_throw_Zone(casic_vector3d firePos, int fireDis, int fireHei, double angle, QColor zoneColor, int lineSize)
{
	IEarth* pEarth = IEarth::GetEarth();
	if (pEarth == nullptr)
	{
		return;
	}
	if (Boom_fire_ForM3_throw != nullptr)
	{
		pEarth->GetGraphics()->RemoveObject(Boom_fire_ForM3_throw);
	}

	CCreateShape cs;

	std::vector<double> pdLon;
	pdLon.clear();
	std::vector<double> pdLat;
	pdLat.clear();
	std::vector<double> pdHeight;
	pdHeight.clear();

}

void UAV_DisPose_Route_Dialog::draw3DFireBoom_For_M3_throw(casic_vector3d firePos, double heading, double speed, QColor zoneColor, int zoneSize)
{
	if (heading > 360)
		heading -= 360;

	IEarth *pEarth = IEarth::GetEarth();
	if (pEarth == nullptr)
	{
		return;
	}
	if (Boom_fire_ForM3_throw != nullptr)
	{
		pEarth->GetGraphics()->RemoveObject(Boom_fire_ForM3_throw);
	}
	Boom_fire_ForM3_throw = nullptr;

	if (Boom_fire_forM3_throw_onUAV != nullptr)
	{
		pEarth->GetGraphics()->RemoveObject(Boom_fire_forM3_throw_onUAV);
	}
	Boom_fire_forM3_throw_onUAV = nullptr;

	double speed_dan = 24.75;
	int fire_hei = 20;
	int dis = routInfo_forA2A3[1].disOrHei;
	auto fireDis_heiItor = fireInfo.find(dis);
	if (fireDis_heiItor != fireInfo.end())
	{
		double speed_temp = calSpeed_dan(dis, fireDis_heiItor->second);
		if (speed_temp > 0)
		{
			speed_dan = speed_temp;
			fire_hei = fireDis_heiItor->second;
		}
	}

	IUserMesh2* pum2 = CreateParabola(speed_dan + speed, fire_hei);
	Boom_fire_ForM3_throw = pum2;
	if (Boom_fire_ForM3_throw != nullptr)
	{
		Boom_fire_ForM3_throw->SetPosition(firePos.x, firePos.y,
			firePos.z);
		Boom_fire_ForM3_throw->SetYawPitchRoll(heading, 0, 0);
		Boom_fire_ForM3_throw->SetColor(zoneColor.rgba());
		Boom_fire_ForM3_throw->SetVisible(true);
	}
	IUserMesh2* pum2onUAV = CreateParabola(speed_dan + speed, fire_hei);
	Boom_fire_forM3_throw_onUAV = pum2onUAV;
	if (Boom_fire_forM3_throw_onUAV != nullptr)
	{
		Boom_fire_forM3_throw_onUAV->SetPosition(firePos.x, firePos.y,
			firePos.z);
		Boom_fire_forM3_throw_onUAV->SetYawPitchRoll(heading, 0, 0);
		Boom_fire_forM3_throw_onUAV->SetColor(zoneColor.rgba());
		Boom_fire_forM3_throw_onUAV->SetVisible(true);
	}

}

void UAV_DisPose_Route_Dialog::change3DFireBoom_For_M3_throw(casic_vector3d firePos, double heading, QColor zoneColor)
{
	if (heading > 360)
		heading -= 360;
	if (Boom_fire_forM3_throw_onUAV != nullptr)
	{
		Boom_fire_forM3_throw_onUAV->SetPosition(firePos.x, firePos.y,
			firePos.z);
		Boom_fire_forM3_throw_onUAV->SetYawPitchRoll(heading, 0, 0);
		Boom_fire_forM3_throw_onUAV->SetColor(zoneColor.rgba());
		Boom_fire_forM3_throw_onUAV->SetVisible(true);
	}
}

void UAV_DisPose_Route_Dialog::draw3DDiscoverZone_for_M3(casic_vector3d curPos, double heading, double fuyang, double pianzhuan, float dis_m, QColor zoneColor)
{
	if (heading > 360)
		heading -= 360;

	IEarth *pEarth = IEarth::GetEarth();
	if (pEarth == nullptr)
	{
		return;
	}

	del3DstdIUserMesh2s(discoverZone_for_M3);

	discoverZone_for_M3 = CreateDetectionZone(40, 20, dis_m);
	for(auto pum2 : discoverZone_for_M3)
	{
		pum2->SetPosition(curPos.x, curPos.y,
			curPos.z);
		pum2->SetYawPitchRoll(heading, fuyang, pianzhuan);
		pum2->SetColor(zoneColor.rgba());
		pum2->SetVisible(true);
	}

}

void UAV_DisPose_Route_Dialog::change3DDiscoverZone_For_M3(casic_vector3d curPos, double heading, double fuyang, double pianzhuan, QColor zoneColor)
{
	for (auto pum2 : discoverZone_for_M3)
	{
		pum2->SetPosition(curPos.x, curPos.y,
			curPos.z);
		pum2->SetYawPitchRoll(heading, fuyang, pianzhuan);
		pum2->SetColor(zoneColor.rgba());
		pum2->SetVisible(true);
	}
}

void UAV_DisPose_Route_Dialog::draw3DDiscoverZone_for_M2(casic_vector3d curPos, double heading, double fuyang, double pianzhuan, float dis_m, QColor zoneColor)
{
	if (heading > 360)
		heading -= 360;

	IEarth *pEarth = IEarth::GetEarth();
	if (pEarth == nullptr)
	{
		return;
	}

	del3DstdIUserMesh2s(discoverZone_for_M2);

	discoverZone_for_M2 = CreateDetectionZone(40, 20, dis_m);
	for (auto pum2 : discoverZone_for_M2)
	{
		pum2->SetPosition(curPos.x, curPos.y,
			curPos.z);
		pum2->SetYawPitchRoll(heading, fuyang, pianzhuan);
		pum2->SetColor(zoneColor.rgba());
		pum2->SetVisible(true);
	}
}

void UAV_DisPose_Route_Dialog::change3DDiscoverZone_For_M2(casic_vector3d curPos, double heading, double fuyang, double pianzhuan, QColor zoneColor)
{
	for (auto pum2 : discoverZone_for_M3)
	{
		pum2->SetPosition(curPos.x, curPos.y,
			curPos.z);
		pum2->SetYawPitchRoll(heading, fuyang, pianzhuan);
		pum2->SetColor(zoneColor.rgba());
		pum2->SetVisible(true);
	}
}

IUserMesh2 *UAV_DisPose_Route_Dialog::CreateParabola(float v, int h, float angle)
{
	IUserMesh2 *pUM = nullptr;
	IEarth *pEarth = IEarth::GetEarth();
	if (pEarth != nullptr)
	{
		pUM = pEarth->GetGraphics()->CreateUserMesh2();
	}

	angle = angle*PI / 180;
	double dVh = v*sin(angle) / 30;
	double dVx = v*cos(angle) / 30;
	double t = (dVh + sqrt(dVh*dVh + 2 * 9.8*h)) / 9.8;
	double dX = dVx * t * 30;
	int nSize = ceil(t * 30) ;
	int nN = 2 * nSize + 1;
	VERTEX *pVerter = (VERTEX*)malloc(nN * sizeof(VERTEX));
	unsigned short *pIndex0 = (unsigned short *)malloc(nN * sizeof(unsigned short));
	unsigned short *pIndex = (unsigned short *)malloc(nSize * sizeof(unsigned short));
	if (pVerter != nullptr)
	{
		int i;
		int nI1 = 0;
		int nI2 = nSize << 1;
		int nI3 = 0;
		for (i = 0; i < nSize; i++)
		{
			if ((nI1 % 10) == 9)
			{
				pIndex[nI3++] = nI1;
				pIndex[nI3++] = nI2;
			}
			float fS = dVx*i;
			pVerter[nI1].x = 0;
			pVerter[nI1].y = fS;
			pVerter[nI1].z = dVh*i + (-9.8)*i*i / 30 / 30 / 2;
			nI1++;
			pVerter[nI2].x = 0;
			pVerter[nI2].y = fS;
			pVerter[nI2].z = -h;
			nI2--;
		}
		pVerter[nI1].x = 0;
		pVerter[nI1].y = dX;
		pVerter[nI1].z = -h;
		nI1++;
		for (i = 0; i < nN; i++)
		{
			pIndex0[i] = i;
		}
		pUM->SetPoints(nN, pVerter);
		pUM->SetIndexs(nN, pIndex0, IUserMesh::USERMESH_POLYGON, false);
		pUM->SetIndexs(nI3, pIndex, IUserMesh::USERMESH_LINES);
		free(pIndex);
		free(pVerter);
		pUM->SetState(IUserMesh::POLYGONMODE, IUserMesh::USERMESH_LINE);
		pUM->SetState(IUserMesh::LINEWIDTH, 3);
	}
	return pUM;
}

vector<IUserMesh2 *> UAV_DisPose_Route_Dialog::CreateDetectionZone(float angle_l, float angle_w, float dis_m)
{
	vector<IUserMesh2*> reUserMesh2s;
	reUserMesh2s.clear();
	IUserMesh2 *pUM = nullptr;
	IEarth *pEarth = IEarth::GetEarth();
	if (pEarth != nullptr)
	{
		pUM = pEarth->GetGraphics()->CreateUserMesh2();
	}

	int angle_w_count = (int)angle_w;
	int angle_l_count = (int)(angle_l * 2);

	if (angle_w_count == 0)
		angle_w_count = 1;
	if (angle_l_count == 0)
		angle_l_count = 1;

	float angle_w_dis = angle_w / angle_w_count;
	float angle_l_dis = angle_l / angle_l_count;

	vector<unsigned short> pIndex_up;
	vector<unsigned short> pIndex_down;
	vector<unsigned short> pIndex_left;
	vector<unsigned short> pIndex_r;
	vector<vector<unsigned short>> pIndex_cir;
	pIndex_up.clear();
	pIndex_down.clear();
	pIndex_left.clear();
	pIndex_r.clear();
	pIndex_cir.clear();

	pIndex_up.push_back(0);
	pIndex_down.push_back(0);
	pIndex_left.push_back(0);
	pIndex_r.push_back(0);

	VERTEX *pVerter = (VERTEX*)malloc(((angle_l_count + 1) * (angle_w_count + 1) + 1) * sizeof(VERTEX));
	if (pVerter != nullptr)
	{
		int pVIndex = 0;
		float angle_w_cur = -(angle_w / 2);
		pVerter[pVIndex].x = 0;
		pVerter[pVIndex].y = 0;
		pVerter[pVIndex].z = 0;
		pVIndex++;
		for (int angle_w_index = 0; angle_w_index <= angle_w_count; angle_w_index++)
		{
			double angle_w_cur_pi = angle_w_cur*PI / 180;
			angle_w_cur += angle_w_dis;
			double z = dis_m * sin(angle_w_cur_pi);
			double dis_z = dis_m * cos(angle_w_cur_pi);
			float angle_l_cur = -(angle_l / 2);

			for (int angle_l_index = 0; angle_l_index <= angle_l_count; angle_l_index++)
			{
				double angle_l_cur_pi = angle_l_cur * PI / 180;
				angle_l_cur += angle_l_dis;
				double x = dis_z * sin(angle_l_cur_pi);
				double y = dis_z * cos(angle_l_cur_pi);

				pVerter[pVIndex].x = x;
				pVerter[pVIndex].y = y;
				pVerter[pVIndex].z = z;

				if (angle_w_index == 0)
				{
					pIndex_up.push_back(pVIndex);
				}
				else if (angle_w_index == angle_w_count)
				{
					pIndex_down.push_back(pVIndex);
				}

				if (angle_l_index == 0)
				{
					pIndex_left.push_back(pVIndex);
				}
				else if (angle_l_index == angle_l_count)
				{
					pIndex_r.push_back(pVIndex);
				}

				pVIndex++;
			}
		}

		int startIndex = 1;
		int index_added = angle_l_count + 1;
		for (int wIndex = 0; wIndex < angle_w_count; wIndex++)
		{
			int startWIndex = startIndex + wIndex * index_added;
			vector<unsigned short> tempPindex_cir;
			tempPindex_cir.clear();
			tempPindex_cir.push_back(startWIndex);
			tempPindex_cir.push_back(startWIndex + index_added);
			for (int lIndex = 1; lIndex < index_added; lIndex++)
			{
				tempPindex_cir.push_back(startWIndex + lIndex);
				tempPindex_cir.push_back(startWIndex + lIndex + index_added);
			}
			pIndex_cir.push_back(tempPindex_cir);
		}

		pUM->SetPoints(pVIndex, pVerter);
		pUM->SetIndexs(pIndex_up.size(), &pIndex_up[0], IUserMesh::USERMESH_POLYGON, false);
		pUM->SetIndexs(pIndex_down.size(), &pIndex_down[0], IUserMesh::USERMESH_POLYGON, false);
		pUM->SetIndexs(pIndex_left.size(), &pIndex_left[0], IUserMesh::USERMESH_POLYGON, false);
		pUM->SetIndexs(pIndex_r.size(), &pIndex_r[0], IUserMesh::USERMESH_POLYGON, false);
		for (int iindex = 0; iindex < pIndex_cir.size(); iindex++)
		{
			if (iindex != pIndex_cir.size() - 1)
			{
				pUM->SetIndexs(pIndex_cir[iindex].size(), &pIndex_cir[iindex][0], IUserMesh::USERMESH_QUAD_STRIP, false);
			}
			else
			{
				pUM->SetIndexs(pIndex_cir[iindex].size(), &pIndex_cir[iindex][0], IUserMesh::USERMESH_QUAD_STRIP);
			}
		}

		pUM->SetState(IUserMesh::POLYGONMODE, IUserMesh::USERMESH_FILL);
		pUM->SetState(IUserMesh::LINEWIDTH, 3);
		reUserMesh2s.push_back(pUM);

		pUM = pEarth->GetGraphics()->CreateUserMesh2();
		pUM->SetPoints(pVIndex, pVerter);
		pUM->SetIndexs(pIndex_up.size(), &pIndex_up[0], IUserMesh::USERMESH_POLYGON, false);
		pUM->SetIndexs(pIndex_down.size(), &pIndex_down[0], IUserMesh::USERMESH_POLYGON, false);
		pUM->SetIndexs(pIndex_left.size(), &pIndex_left[0], IUserMesh::USERMESH_POLYGON, false);
		pUM->SetIndexs(pIndex_r.size(), &pIndex_r[0], IUserMesh::USERMESH_POLYGON);
		free(pVerter);
		pUM->SetState(IUserMesh::POLYGONMODE, IUserMesh::USERMESH_LINE);
		pUM->SetState(IUserMesh::LINEWIDTH, 2);
		pUM->SetState(IUserMesh::LIGHT, 1);
		reUserMesh2s.push_back(pUM);

	}
	return reUserMesh2s;
}

void UAV_DisPose_Route_Dialog::clearAll()
{
	del3DPathLine();
	Path3DLines.clear();
	disposeTargetAndUAVS_process.getDispos_Uav_Route()->clearRoutedLine();
	routeLine_forA2A3->clear();
}
