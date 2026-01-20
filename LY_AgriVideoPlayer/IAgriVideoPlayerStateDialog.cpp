#include "IAgriVideoPlayerStateDialog.h"
#include "ui_IAgriVideoPlayerStateDialog.h"
#include "LY_AgriVideoPlayerActivator.h"
IAgriVideoPlayerStateDialog::IAgriVideoPlayerStateDialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::IAgriVideoPlayerStateDialog)
{
    ui->setupUi(this);
}

IAgriVideoPlayerStateDialog::~IAgriVideoPlayerStateDialog()
{
    delete ui;
}

void IAgriVideoPlayerStateDialog::Init(const int& index)
{
	qnzkna::UAVRealTimeStateManage::IUAVRealTimeStateManageService* pIUAVRealTimeStateManageService = LY_AgriVideoPlayerActivator::Get().getPIUAVRealTimeStateManageService();
	if (pIUAVRealTimeStateManageService == nullptr) {
		return;
	}
	qnzkna::UAVRealTimeStateManage::NebulaUAVRealTimeStateMap nebulaUAVRealTimeStateMap;
	qnzkna::UAVRealTimeStateManage::RealTimeTelemetryInformation_t stateInfo;
	std::string strFoucsUAVID = "", strProtocolType = "";
	if (pIUAVRealTimeStateManageService->GetUAVRealTimeStateList(&nebulaUAVRealTimeStateMap))
	{
		int nNum = 0;
		for (const auto state : nebulaUAVRealTimeStateMap)
		{
			if (nNum != index) {
				++nNum;
				continue;
			}
			telemetryInformation uav_information = state.second.m_telemetryInfo.telemetryInfo;
			ui->label_RCState->setText(QString::number(uav_information._status));
			ui->label_GPSState->setText(QString::number(uav_information.gpsLostTime));
			ui->label_moonNumber->setText(QString::number(uav_information.gpsNumber));
			ui->label_power->setText(QString::number(uav_information.powervoltage));
			ui->label_longtitude->setText(QString::number(uav_information.position.lng));
			ui->label_latitude->setText(QString::number(uav_information.position.lat));
			ui->label_altitude->setText(QString::number(uav_information.position.heigth));
			ui->label_speed->setText(QString::number(uav_information.groundVelocity));
			break;
		}	
	}
}
