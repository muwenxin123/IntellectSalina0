#include "IScenariosReplayThread.h"

#include "LY_ScenariosReplay.h"
#include "QtNetwork\QHostAddress"
#include "LY_ScenariosReplayInterface.h"
#include "ScenarioReplayDialog.h"
#include "IScenariosReplayActivator.h"
#include <QSettings>

#define SCENARIO_PLAY_HZ   1.0

IScenariosReplayThread::IScenariosReplayThread(QObject *parent)
	: QThread(parent)
	, _speed(1)
	, _rate(1)
	, _stop(false)
	, _running(false)
	, _time(10)
{
	m_nPointNoMax = 0;
	initUdp();
}

IScenariosReplayThread::~IScenariosReplayThread()
{
	if (this) {
		_running = false;
		QThread::quit();
		QThread::wait(1);
	}
}

void IScenariosReplayThread::initUdp()
{
	QSettings iniFile("../data/conf/ly.ini", QSettings::IniFormat);
	iniFile.beginGroup("Multicast");
	QString strDbIP = iniFile.value("GroupIP", "225.1.1.1").toString();
	if (strDbIP.isEmpty())
	{
		udp_IP = "225.1.1.1";
	}
	else
	{
		udp_IP = strDbIP;
	}
	udp_Port = 1688;
	m_udpSocket = new QUdpSocket;
}

void IScenariosReplayThread::run()
{
	if (!m_ReplayDataInfoVec.empty())
	{
		int nNo(0);
		double dRunningTime(0);
		unsigned int needSleepTime = 0;
		_running = true;
		m_nPointNoMax = m_ReplayDataInfoVec.size();
		while (nNo < m_nPointNoMax )
		{
			if (!_running)
			{
				break;
			}
			if (_pause)
			{
				msleep(100);
				continue;
			}
			needSleepTime = 0;
			unsigned int neddSleepInit = 1000 / SCENARIO_PLAY_HZ;

			needSleepTime = neddSleepInit / (_speed *_rate);
			needSleepTime = (needSleepTime > 2000 || needSleepTime < 0) ? 2000 : needSleepTime;
			dRunningTime += needSleepTime*_rate;

			if (m_ReplayDataInfoVec.size() == 0)
			{
				return;
			}
			QByteArray udpsend = m_ReplayDataInfoVec[nNo].data;
			int nlength = udpsend.length();
			if (LY_ScenariosReplay::Get().m_pScenarioReplayDialog->isSeatReplay)
			{
				qnzkna::framework::IRegistry* pContext = IScenariosReplayActivator::Get().getIRegistryImpl();
				if (pContext != nullptr) {

					qnzkna::framework::MessageReferenceProps  props;
					std::string str = "1";
					pContext->messageNotify(new qnzkna::framework::MessageReference("MsgSeatScenariosReplay", udpsend, nlength, props));
				}
				m_udpSocket->writeDatagram(udpsend, udp_IP, udp_Port);
			}
			else
			{
				m_udpSocket->writeDatagram(udpsend, udp_IP, udp_Port);
			}

			m_percent = 100 * ((double)nNo / (double)m_nPointNoMax);
			if (nNo >= m_nPointNoMax - 1)
			{
				LY_ScenariosReplay::Get().m_progressBarValue = 100;
				LY_ScenariosReplay::Get().m_pScenarioReplayDialog->stopTimer();
				isFinished();
			}
			else
			{
				LY_ScenariosReplay::Get().m_progressBarValue = m_percent;
			}
			emit LY_ScenariosReplay::Get().m_pScenarioReplayDialog->progressBarValue(LY_ScenariosReplay::Get().m_progressBarValue, nNo, m_nPointNoMax);
			msleep(needSleepTime);
			nNo++;

		}
		_stop = true;
	}
}

void IScenariosReplayThread::startThread()
{
	QThread::start();
}

void IScenariosReplayThread::stopThread()
{
	_running = false;

	for (int i = 0; i < 30 && !_stop; i++)
	{
		QThread::msleep(_time);
	}
	_stop = false;
}

void IScenariosReplayThread::pauseReplay(bool pause)
{
	_pause = pause;
}

void IScenariosReplayThread::setSpeed(int speed)
{
	_speed = speed;
}

void IScenariosReplayThread::setRate(int rate)
{
	if (rate <= 0)
	{
		return;
	}
	_rate = rate;
}

bool IScenariosReplayThread::isFinished()
{
	return _stop;
}
