#include "RoutePlanningDisplayDialog.h"
#include "Protocol/XygsJL/Bag_XygsJLDefines.h"
#include <fstream>
#include <sstream>
#include <QFile>
#include "LyMessageBox.h"
#include "QCString.h"

RoutePlanningDisplayDialog::RoutePlanningDisplayDialog(QWidget *parent)
	: LyDialog(tr("RoutePlanningPlayDialog"), parent),
	ui(new Ui::RoutePlanningDisplayDialog)
{
	ui->setupUi(centralWidget());
	this->setResizable(true);
	this->resize(640, 80);
	init();
	LoadIPIni();

}

RoutePlanningDisplayDialog::~RoutePlanningDisplayDialog()
{
}

void RoutePlanningDisplayDialog::init()
{
	m_vecCalculatePoint.clear();
	m_vecCalculateSumTime = 0;
	m_vecCalculateMaxNum = 0;

	ui->ledtRate->setText("1");
	ui->ledtTime->setText("-");

	m_nPlayTime = 0;
	b_isStart = false;

	m_PlayTimer = new QTimer(this);
	m_PlayTimer->setInterval(1000 / PLAY_HZ);
	connect(m_PlayTimer, &QTimer::timeout, this, &RoutePlanningDisplayDialog::on_timeOut);

	connect(ui->pbtnPlay, &QPushButton::clicked, this, &RoutePlanningDisplayDialog::on_pbtnPlay_clicked);
	connect(ui->pbtnStop, &QPushButton::clicked, this, &RoutePlanningDisplayDialog::on_pbtnStop_clicked);
	connect(ui->pbtnAccelerate, &QPushButton::clicked, this, &RoutePlanningDisplayDialog::on_pbtnAccelerate_clicked);
	connect(ui->pbtnDecelerate, &QPushButton::clicked, this, &RoutePlanningDisplayDialog::on_pbtnDecelerate_clicked);
}

void RoutePlanningDisplayDialog::setTimeShow()
{
	QString hh = "00";
	QString mm = "00";
	QString ss = "00";
	QString dd = "0";
	static int	dex_i = 0;
	static int	m_temp = 0;
	static int	m_m = 0;
	static int	h_h = 0;
	static int  d_d = 0;
	if (isStopClicked)
	{
		m_temp = 0;
		dex_i = 0;
		m_m = 0;
		h_h = 0;
		isStopClicked = false;
	}

	if (m_temp >= 60)
	{
		m_m++;
		if (m_m >= 60)
		{
			h_h++;
			if (h_h >= 24)
			{
				d_d++;
				h_h = h_h - 24;
			}
			m_m = m_m - 60;

		}

		m_temp = 0;
	}
	if (m_temp < 10)
	{
		QString tmp = "0" + QString::number(m_temp);
		ss = tmp;
	}
	else
	{
		ss = QString::number(m_temp);
	}

	if (m_m < 10)
	{
		QString tmp = "0" + QString::number(m_m);
		mm = tmp;
	}
	else
	{
		mm = QString::number(m_m);
	}

	if (h_h < 10)
	{
		QString tmp = "0" + QString::number(h_h);
		hh = tmp;
	}
	else
	{
		hh = QString::number(h_h);
	}
	m_temp++;
	dd = QString::number(d_d);
	QString tim = dd + tr("Day") + " " + hh + ":" + mm + ":" + ss;
	ui->ledtTime->setText(tim);
	dex_i++;
}

void RoutePlanningDisplayDialog::on_pbtnStop_clicked()
{
	if (m_PlayTimer->isActive())
	{
		m_PlayTimer->stop();
	}
	ui->pbtnPlay->setText(tr("Play"));
	m_nPlayTime = 0;
	isStopClicked = true;
	b_isStart = false;
	ui->ledtRate->setText(tr("1"));
	m_nPlaySpeed = 1;
	ui->ledtTime->setText("--");
}

void RoutePlanningDisplayDialog::on_pbtnAccelerate_clicked()
{
	m_nPlaySpeed = m_nPlaySpeed * 2;
	if (m_nPlaySpeed > 64)
	{
		m_nPlaySpeed = 64;
	}

	if (m_PlayTimer->isActive() == true)
	{
		m_PlayTimer->stop();
		m_PlayTimer->setInterval(1000 / PLAY_HZ / m_nPlaySpeed);
		m_PlayTimer->start();
	}
	ui->ledtRate->setText(QString::number(m_nPlaySpeed));
}

void RoutePlanningDisplayDialog::on_pbtnDecelerate_clicked()
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
	ui->ledtRate->setText(QString::number(m_nPlaySpeed));
}

void RoutePlanningDisplayDialog::on_timeOut()
{
	for (auto itrlie : m_vecCalculatePoint)
	{
		auto itrPoint = itrlie.stTelemetryPointVec.begin();
		if (m_nPlayTime < itrlie.stTelemetryPointVec.size())
		{
			itrPoint += m_nPlayTime;

			const std::string& vehicleID = itrlie.strWeaponID;
			telemetryInformation telemetryInfo;
			telemetryInfo = *itrPoint;

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
				ITaskManagerActivator::Get().getIRegistryImpl()->messageNotify(new qnzkna::framework::MessageReference("MsgUAVRealTimeState", (char*)&bag, sizeof(bag), props));
			}
#else

			TelemetryInformation_t stTelemetryInformation_t;
			memset(&stTelemetryInformation_t, 0x00, sizeof(TelemetryInformation_t));
			stTelemetryInformation_t.lat = telemetryInfo.position.lat* 1e7;
			stTelemetryInformation_t.lng = telemetryInfo.position.lng* 1e7;
			stTelemetryInformation_t.alt = telemetryInfo.gpsHeigth * 10;
			stTelemetryInformation_t.angleofNose = telemetryInfo.angleofNose * 10;
			stTelemetryInformation_t.angleofRudder = telemetryInfo.angleofNose * 10;
			stTelemetryInformation_t._rudder = telemetryInfo.angleofNose * 10;
			stTelemetryInformation_t.targetVelocity = telemetryInfo.groundVelocity * 100;
			stTelemetryInformation_t.groundVelocity = telemetryInfo.groundVelocity * 100;

			for (auto IPItr : m_UAVServerIPVec)
			{
				std::string strName = IPItr.strName.toStdString();
				if (0 == strcmp(strName.c_str(), vehicleID.c_str()))
				{
					if (shadowCube.empty())
					{
						casic_vector3d mpos = casic_vector3d(stTelemetryInformation_t.lng* 1e-7,
							stTelemetryInformation_t.lat* 1e-7, 0);
						std::pair<int, casic_vector3d>npair = std::make_pair(std::atoi(vehicleID.c_str()), mpos);
						shadowCube.insert(npair);
					}
					else
					{
						bool isfind = false;
						for (auto itor = shadowCube.begin(); itor != shadowCube.end(); itor++)
						{
							if (itor->first == std::atoi(vehicleID.c_str()))
							{
								itor->second = casic_vector3d(stTelemetryInformation_t.lng* 1e-7,
									stTelemetryInformation_t.lat* 1e-7, 0);
								isfind = true;
								break;
							}
						}
						if (isfind == false)
						{
							casic_vector3d mpos = casic_vector3d(stTelemetryInformation_t.lng* 1e-7,
								stTelemetryInformation_t.lat* 1e-7, 0);
							std::pair<int, casic_vector3d>npair = std::make_pair(std::atoi(vehicleID.c_str()), mpos);
							shadowCube.insert(npair);
						}
					}
					QHostAddress stQHostAddress;
					stQHostAddress.setAddress(IPItr.strIP);
					m_UAVProtocol.SendCommandBytes(0x10, 0x10, reinterpret_cast<const uchar *>(&stTelemetryInformation_t), sizeof(stTelemetryInformation_t), stQHostAddress, IPItr.nPort, IPItr.nPort);
					emit updatePosition();
					break;
				}
			}
#endif
		}
	}
	m_nPlayTime++;

	setTimeShow();
	if (abs(m_nPlayTime - m_vecCalculateMaxNum) < 0.00001)
	{
		m_PlayTimer->stop();
	}
	double nPor = m_nPlayTime*100.0 / m_vecCalculateMaxNum;
	ui->progressBar->setValue(nPor);
	ui->progressBar->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	ui->progressBar->setFormat(QString("%1%").arg(QString::number(nPor, 'f', 2)));
}

void RoutePlanningDisplayDialog::LoadIPIni()
{
	m_UAVServerIPVec.clear();
	std::string strPath = "../data/PlaySendToServer.ini";
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

void RoutePlanningDisplayDialog::on_pbtnPlay_clicked()
{
	if (b_isStart)
	{
		ui->pbtnPlay->setText(tr("Play"));
		m_PlayTimer->stop();
		b_isStart = false;
	}
	else if (b_isStart == false)
	{
		ui->pbtnPlay->setText(tr("Pause"));

		m_PlayTimer->setInterval(1000 / PLAY_HZ / m_nPlaySpeed);
		m_PlayTimer->start();
		b_isStart = true;
	}
}

