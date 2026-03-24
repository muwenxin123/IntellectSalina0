#include "ScenarioReplayDialog.h"
#include "ui_ScenarioReplayDialog.h"
#include "QCString.h"
#include "lY_ScenariosReplay.h"
#include "lY_ScenariosReplayInterface.h"
#include "QSettings"
#include "LyMessageBox.h"
#include<QDebug>
#include <QString>
#include <QDateEdit>

ScenarioReplayDialog::ScenarioReplayDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::ScenarioReplayDialog),
	videoThread_A1(nullptr),
	videoThread_A2(nullptr),
	videoThread_A3(nullptr),
	videoThread_A4(nullptr),
	videoThread_A5(nullptr),
	videoThread_A6(nullptr),
	videoThread_A7(nullptr),
	videoThread_A8(nullptr),
	videoThread_A9(nullptr),
	videoThread_A10(nullptr)
{
	ui->setupUi(this);

	viewModel = new ISReplayViewModel(this);
	viewModel->initTableWidget(ui->tableWidget);

	m_databaseName = "net20221123";
	std::vector<QString> items = viewModel->getDataList();

	for (auto data : items)
	{
		ui->cbxTablesList->addItem(data);
	}

	m_databaseName = ui->cbxTablesList->currentText();

	initMenu();
	InitUI();

	addSlotsAction();

	initThread(videoThread_A1);
	initThread(videoThread_A2);
	initThread(videoThread_A3);
	initThread(videoThread_A4);
	initThread(videoThread_A5);
	initThread(videoThread_A6);
	initThread(videoThread_A7);
	initThread(videoThread_A8);
	initThread(videoThread_A9);
	initThread(videoThread_A10);

}
void  ScenarioReplayDialog::addSlotsAction()
{

	connect(m_playTime, SIGNAL(timeout()), this, SLOT(setTimeShow()));
	connect(this, SIGNAL(progressBarValue(const unsigned int, const unsigned int, const unsigned int)), this, SLOT(setProgressBarValue(const unsigned int, const unsigned int, const unsigned int)));

	connect(ui->rdbtnSest, &QRadioButton::toggled, this, &ScenarioReplayDialog::onCheckBoxSelected);

	connect(m_RsHsing, &RangeSlider::lowerValueChanged, this, &ScenarioReplayDialog::LowerSliderChanged);
	connect(m_RsHsing, &RangeSlider::upperValueChanged, this, &ScenarioReplayDialog::UpperSliderChanged);

	connect(m_RsHsing, &RangeSlider::mouseReleaseBeginDate, this, &ScenarioReplayDialog::beginSliderTimeDate);
	connect(m_RsHsing, &RangeSlider::mouseReleaseEndDate, this, &ScenarioReplayDialog::endSliderTimeDate);

	connect(this, SIGNAL(progressBarValue(const unsigned int, const unsigned int, const unsigned int)), this, SLOT(setProgressBarValue(const unsigned int, const unsigned int, const unsigned int)));

	connect(ui->chxTimeStat, &QCheckBox::stateChanged, this, &ScenarioReplayDialog::SelectedCheckBoxState);
	connect(ui->chxSysControl, &QCheckBox::stateChanged, this, &ScenarioReplayDialog::SelectedCheckBoxState);
	connect(ui->chxSysRun, &QCheckBox::stateChanged, this, &ScenarioReplayDialog::SelectedCheckBoxState);
	connect(ui->chxTrackData, &QCheckBox::stateChanged, this, &ScenarioReplayDialog::SelectedCheckBoxState);
	connect(ui->chxPlat, &QCheckBox::stateChanged, this, &ScenarioReplayDialog::SelectedCheckBoxState);
	connect(ui->chxElectroPod, &QCheckBox::stateChanged, this, &ScenarioReplayDialog::SelectedCheckBoxState);
	connect(ui->chxFireControl, &QCheckBox::stateChanged, this, &ScenarioReplayDialog::SelectedCheckBoxState);
	connect(ui->chxinfraredAim, &QCheckBox::stateChanged, this, &ScenarioReplayDialog::SelectedCheckBoxState);
	connect(ui->chxRadio, &QCheckBox::stateChanged, this, &ScenarioReplayDialog::SelectedCheckBoxState);
	connect(ui->chxInterveneCMD, &QCheckBox::stateChanged, this, &ScenarioReplayDialog::SelectedCheckBoxState);
	connect(ui->chxcmdMsg, &QCheckBox::stateChanged, this, &ScenarioReplayDialog::SelectedCheckBoxState);
	connect(ui->chxFileConvey, &QCheckBox::stateChanged, this, &ScenarioReplayDialog::SelectedCheckBoxState);
	connect(ui->chxIntelligenceAssign, &QCheckBox::stateChanged, this, &ScenarioReplayDialog::SelectedCheckBoxState);
	connect(ui->chxSysUpdate, &QCheckBox::stateChanged, this, &ScenarioReplayDialog::SelectedCheckBoxState);
	connect(ui->chxPic, &QCheckBox::stateChanged, this, &ScenarioReplayDialog::SelectedCheckBoxState);
	connect(ui->chxVideo, &QCheckBox::stateChanged, this, &ScenarioReplayDialog::SelectedCheckBoxState);

	connect(ui->cbxSend, &QCheckBox::stateChanged, this, &ScenarioReplayDialog::SelectedSendBoxState);
	connect(ui->cbxRecv, &QCheckBox::stateChanged, this, &ScenarioReplayDialog::SelectedRecvBoxState);

	connect(ui->pbtnCustomRate, &QPushButton::clicked, this, &ScenarioReplayDialog::initMenu);
}
void ScenarioReplayDialog::Init()
{
	vector<LogDataInfo>   replayInfo = viewModel->m_ReplayDataInfoVec;

	if (replayInfo.empty())
		return;
	ui->tmEdtEnd->setDisplayFormat("hh:mm:ss");
	ui->tmEdtStart->setDisplayFormat("hh:mm:ss");
	QString strCurrent = ui->cbxTablesList->currentText();
	QString matchingNum = "";
	QRegExp numberRegex("\\d+");
	int pos = 0;
	while ((pos = numberRegex.indexIn(strCurrent, pos)) != -1)
	{
		matchingNum.append(numberRegex.cap(0));
		pos += numberRegex.matchedLength();
	}
	QDateTime dataTimeBegin = QDateTime::fromString(matchingNum, "yyyyMMdd");
	ui->dtEdtStart->setDateTime(dataTimeBegin);

	int replayTime = replayInfo[0].recordTime / 1000;
	int hours = replayTime / 3600;
	int minutes = (replayTime % 3600) / 60;
	int secs = replayTime % 60;
	QTime time(hours, minutes, secs);
	ui->tmEdtStart->setTime(time);
	int repEndtime = replayInfo.back().recordTime / 1000;
	hours = repEndtime / 3600;
	int addTimeData = 0;
	if (hours / 24)
	{
		addTimeData = hours / 24;
		hours -= 24 * addTimeData;
	}
	minutes = (repEndtime % 3600) / 60;
	secs = repEndtime % 60;

	bool bsucc = true;
	while (bsucc)
	{
		int nValue = 0;
		if (secs > 59)
		{
			minutes += secs / 60;
			continue;
		}
		else
			++nValue;
		if (minutes > 59)
		{
			hours += minutes / 60;
			continue;
		}
		else
			++nValue;
		if (hours > 23)
		{
			addTimeData += hours / 24;
			continue;
		}
		else
			++nValue;
		if (nValue >= 3)
			bsucc = false;
	}
	dataTimeBegin.addDays(addTimeData);

	ui->dtEdtEnd->setDateTime(dataTimeBegin);
	QTime time1(hours, minutes, secs);
	ui->tmEdtEnd->setTime(time1);

	m_RsHsing->SetMinimum(replayInfo[0].recordTime / 1000);
	m_RsHsing->SetMaximum(replayInfo.back().recordTime / 1000);
	nStartSeconds = replayInfo[0].recordTime / 1000;
	nEndSeconds = replayInfo.back().recordTime / 1000;

}

void ScenarioReplayDialog::stopTimer()
{
	m_playTime->stop();
}

void ScenarioReplayDialog::setTimeShow()
{
	QString hh = "00";
	QString mm = "00";
	QString ss = "00";
	QString dd = "0";
	QString ms = "0";
	static int	dex_i = 0;
	static int	m_temp = 0;
	static int	m_m = 0;
	static int	s_s = 0;
	static int	h_h = 0;
	static int  d_d = 0;
	static int  m_s = 0;
	if (isStopClicked)
	{
		m_temp = 0;
		dex_i = 0;
		m_m = 0;
		h_h = 0;
		m_s = 0;
		isStopClicked = false;
	}

	if (tempPlayTime != QTime::currentTime().second())
	{
		s_s++;
		if (s_s >= 60)
		{
			m_m++;
			if (m_m >= 60)
			{
				h_h++;
				if (h_h >= 24) {
					d_d++;
					h_h = h_h - 24;
				}
				m_m = m_m - 60;
			}
			s_s = s_s - 60;
		}

		m_temp = 0;
	}

	if (m_temp < 10)
	{
		QString tmp = "0" + QString::number(m_temp);
		ms = tmp;
	}
	else {
		ms = QString::number(m_temp);
	}

	if (s_s < 10)
	{
		QString tmp = "0" + QString::number(s_s);
		ss = tmp;
	}
	else
	{
		ss = QString::number(s_s);
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
	tempPlayTime = QTime::currentTime().second();
	dd = QString::number(d_d);
	QString tim = dd + tr("Day") + " " + hh + ":" + mm + ":" + ss + ":" + ms;
	ui->ledtTime->setText(tim);
	dex_i++;
}

void ScenarioReplayDialog::setProgressBarValue(const unsigned int value, const unsigned int nNo, const unsigned int nMax)
{
	ui->pBar->setValue(value);
	QString mstr = QString::number(nNo);
	mstr += "/";
	mstr += QString::number(nMax);
	ui->pBar->setFormat(mstr);
}
void ScenarioReplayDialog::hiddenNormalChexkbox()
{
	ui->chxTimeStat->setHidden(true);
	ui->chxSysControl->setHidden(true);
	ui->chxSysRun->setHidden(true);
	ui->chxPlat->setHidden(true);
	ui->chxElectroPod->setHidden(true);
	ui->chxFireControl->setHidden(true);
	ui->chxinfraredAim->setHidden(true);
	ui->chxRadio->setHidden(true);
	ui->chxInterveneCMD->setHidden(true);
	ui->chxFileConvey->setHidden(true);
	ui->chxIntelligenceAssign->setHidden(true);
	ui->chxSysUpdate->setHidden(true);
}
void ScenarioReplayDialog::InitUI()
{

	m_playTime = new QTimer(this);
	m_playTime->setTimerType(Qt::PreciseTimer);

	ui->ledtRate->setText("1");
	ui->ledtTime->setEnabled(false);
	ui->ledtRate->setEnabled(false);

	ui->pBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	ui->pBar->setMinimum(0);
	ui->pBar->setMaximum(100);
	QString colorstr = QString("QProgressBar::chunk{background-color : rgb(255,0,0)}");
	ui->pBar->setStyleSheet(colorstr);
	QString mstr = QString::number(0);
	mstr += "/";
	mstr += QString::number(0);
	ui->pBar->setFormat(mstr);
	ui->cbxRecv->setChecked(true);
	ui->cbxSend->setChecked(true);
	ui->rdbtnSystem->setChecked(true);

	m_RsHsing = new RangeSlider(Qt::Horizontal, RangeSlider::Option::DoubleHandles, nullptr);
	ui->horLayoutRange->addWidget(m_RsHsing);
}

void ScenarioReplayDialog::initMenu()
{
	m_menu = new QMenu(this);
	m_menu->setWindowFlags(m_menu->windowFlags() | Qt::FramelessWindowHint);

	m_1stBtn = new QAction(tr2("1x"), this);
	m_2rdBtn = new QAction(tr2("2x"), this);
	m_5thBtn = new QAction(tr2("5x"), this);
	m_10thBtn = new QAction(tr2("10x"), this);

	m_menu->addAction(m_1stBtn);
	m_menu->addAction(m_2rdBtn);
	m_menu->addAction(m_5thBtn);
	m_menu->addAction(m_10thBtn);

	ui->pbtnCustomRate->setMenu(m_menu);

	ui->pbtnCustomRate->setStyleSheet("QPushButton::munu-indicator{image:none;}");

	connect(m_menu, &QMenu::triggered, this, &ScenarioReplayDialog::action_Triggerd);
}

void ScenarioReplayDialog::action_Triggerd(QAction* action)
{
	int nrate = ui->ledtRate->text().toInt();
	if (action == m_1stBtn)
	{
		nrate = 1;
	}
	else if (action == m_2rdBtn)
	{
		nrate = 2;
	}
	else if (action == m_5thBtn)
	{
		nrate = 5;
	}
	else if (action == m_10thBtn)
	{
		nrate = 10;
	}

	ui->ledtRate->setText(QString::number(nrate));
	LY_ScenariosReplay::Get().m_pIScenariosReplayThread->setRate(nrate);

	m_playTime->stop();
	if (isPlaying)
	{
		m_playTime->start(10 / nrate);
	}
}

void ScenarioReplayDialog::resetPlay()
{
	on_pbtnStop_clicked();
	isPlaying = false;
}

void ScenarioReplayDialog::on_pbtnPlay_clicked()
{

	if (viewModel->m_tempAllData.empty())
	{
		LyMessageBox::warning(this, tr2("警告"), tr2("请先导入数据"), QMessageBox::Ok);
		return;
	}

	vector<LogDataInfo>tempvec;
	set<LogDataInfo> set_ = viewModel->getFileVec();
	if (set_.empty())
	{
		LyMessageBox::warning(this, tr2("警告"), tr2("请选择筛选条件"), QMessageBox::Ok);
		return;
	}

	for (auto dataitor : viewModel->m_tempAllData)
	{
		auto itor = set_.find(dataitor);
		if (itor != set_.end())
		{
			tempvec.emplace_back(dataitor);
		}
	}
	viewModel->m_ReplayVec = tempvec;

	for (auto data : viewModel->m_ReplayVec)
	{
		if (data.ucType == 999)
		{
			isSlectedVideo = false;
			viewModel->sendPic(data);
			sureVideoData(set_);
		}
		else if (data.ucType == 160) {

			isSlectedVideo = false;
			viewModel->sendMessageInfo();
			sureVideoData(set_);
		}
		else if (data.ucType == 998) {
			videoType = 2;
			isSlectedVideo = true;
		}
		else if (data.ucType == 43) {
			videoType = 1;
			isSlectedVideo = true;

		}
		else {

			isSlectedVideo = false;
			sureVideoData(set_);
			LY_ScenariosReplay::Get().m_pIScenariosReplayThread->m_ReplayDataInfoVec = viewModel->m_ReplayVec;
			break;
		}

	}

	if (isStopPlaying) {
		isStopPlaying = !isStopPlaying;
	}
	if (LY_ScenariosReplay::Get().m_pIScenariosReplayThread->isFinished())
	{
		on_pbtnStop_clicked();
	}

	LY_ScenariosReplay::Get().m_pIScenariosReplayThread->startThread();
	LY_ScenariosReplay::Get().m_pIScenariosReplayThread->_pause = !LY_ScenariosReplay::Get().m_pIScenariosReplayThread->_pause;
	isPlaying = true;
	if (LY_ScenariosReplay::Get().m_pIScenariosReplayThread->_pause == false)
	{

		ui->pbtnPlay->setText(tr("Pause"));
		LY_ScenariosReplay::Get().m_pIScenariosReplayThread->pauseReplay(false);

		videoThread_A1->videoPathMap.clear();
		videoThread_A2->videoPathMap.clear();
		videoThread_A3->videoPathMap.clear();
		videoThread_A4->videoPathMap.clear();
		videoThread_A5->videoPathMap.clear();
		videoThread_A6->videoPathMap.clear();
		videoThread_A7->videoPathMap.clear();
		videoThread_A8->videoPathMap.clear();
		videoThread_A9->videoPathMap.clear();
		videoThread_A10->videoPathMap.clear();

		if (count(TypeSelectVec.begin(), TypeSelectVec.end(), 998)
			&& count(TypeSelectVec.begin(), TypeSelectVec.end(), 43)
			&& isSlectedVideo)
		{
			videoType = 3;
			videoThread_A1->setVideoPath(set_, 8000, 3);
			videoThread_A2->setVideoPath(set_, 8001, 3);
			videoThread_A3->setVideoPath(set_, 18000, 3);
			videoThread_A4->setVideoPath(set_, 18001, 3);
			videoThread_A5->setVideoPath(set_, 28000, 3);
			videoThread_A6->setVideoPath(set_, 28001, 3);
			videoThread_A7->setVideoPath(set_, 38000, 3);
			videoThread_A8->setVideoPath(set_, 38001, 3);
			videoThread_A9->setVideoPath(set_, 48000, 3);
			videoThread_A10->setVideoPath(set_, 48001, 3);

			videoThread_A1->start();
			videoThread_A2->start();
			videoThread_A3->start();
			videoThread_A4->start();
			videoThread_A5->start();
			videoThread_A6->start();
			videoThread_A7->start();
			videoThread_A8->start();
			videoThread_A9->start();
			videoThread_A10->start();

		}
		else
		{
			if (videoType == 2) {

				videoThread_A1->setVideoPath(set_, 8001, 2);
				videoThread_A2->setVideoPath(set_, 18001, 2);
				videoThread_A3->setVideoPath(set_, 28001, 2);
				videoThread_A4->setVideoPath(set_, 38001, 2);
				videoThread_A5->setVideoPath(set_, 48001, 2);
			}
			else if (videoType == 1) {

				videoThread_A1->setVideoPath(set_, 8000, 1);
				videoThread_A2->setVideoPath(set_, 18000, 1);
				videoThread_A3->setVideoPath(set_, 28000, 1);
				videoThread_A4->setVideoPath(set_, 38000, 1);
				videoThread_A5->setVideoPath(set_, 48000, 1);

			}
			if (isSlectedVideo)
			{
				videoThread_A1->start();
				videoThread_A2->start();
				videoThread_A3->start();
				videoThread_A4->start();
				videoThread_A5->start();
			}
		}
	}
	else
	{
		ui->pbtnPlay->setText(tr("Play"));
		LY_ScenariosReplay::Get().m_pIScenariosReplayThread->pauseReplay(true);
		if (isSlectedVideo && videoThread_A1 != nullptr)
		{
			videoThread_A1->pause();
			videoThread_A2->pause();
			videoThread_A3->pause();
			videoThread_A4->pause();
			videoThread_A5->pause();
			videoThread_A6->pause();
			videoThread_A7->pause();
			videoThread_A8->pause();
			videoThread_A9->pause();
			videoThread_A10->pause();
		}
	}
	if (m_playTime->isActive() == false)
	{
		int rate = ui->ledtRate->text().toInt();
		m_playTime->start(10 / rate);
	}
	else
	{
		m_playTime->stop();
	}

}
void ScenarioReplayDialog::sureVideoData(set<LogDataInfo> temp_set)
{
	for (auto subSet : temp_set)
	{
		if (subSet.ucType == 998 || subSet.ucType == 43)
		{
			isSlectedVideo = true;
			videoType = (subSet.ucType == 998) ? 2 : 1;
			break;
		}
	}
}
void ScenarioReplayDialog::on_pbtnPause_clicked()
{

}
void ScenarioReplayDialog::initThread(ISSendVideoThread  *&qthread) {
	if (qthread == nullptr) {
		qthread = new ISSendVideoThread;
	}
}
void ScenarioReplayDialog::deleteThread(ISSendVideoThread  *&qthread)
{

	if (qthread) {
		qthread->closeFile();
		qthread->stop();
		delete qthread;
		qthread = nullptr;
	}
	initThread(qthread);
}
void ScenarioReplayDialog::on_pbtnStop_clicked()
{

	videoThread_A1->pause();
	videoThread_A2->pause();
	videoThread_A3->pause();
	videoThread_A4->pause();
	videoThread_A5->pause();
	videoThread_A6->pause();
	videoThread_A7->pause();
	videoThread_A8->pause();
	videoThread_A9->pause();
	videoThread_A10->pause();

	isStopClicked = true;
	isStopPlaying = true;
	ui->pbtnPlay->setText(tr("Play"));
	LY_ScenariosReplay::Get().m_pIScenariosReplayThread->_pause = true;
	LY_ScenariosReplay::Get().m_pIScenariosReplayThread->stopThread();
	LY_ScenariosReplay::Get().m_pIScenariosReplayThread->_rate = 1;
	ui->ledtRate->setText(tr2("1"));
	ui->ledtTime->setText("--");
	if (m_playTime->isActive())
	{
		m_playTime->stop();
	}

}

void ScenarioReplayDialog::on_pbtnAccelerate_clicked()
{
	auto m_rate = ui->ledtRate->text();
	int nrate = m_rate.toInt();
	{
		if (nrate < 60)
		{
			nrate *= 2;
			ui->ledtRate->setText(QString::number(nrate));
			LY_ScenariosReplay::Get().m_pIScenariosReplayThread->setRate(nrate);
		}
		else
		{
			ui->ledtRate->setText(QString::number(nrate));
			LY_ScenariosReplay::Get().m_pIScenariosReplayThread->setRate(nrate);
		}
	}
	if (isStopClicked)
	{
		return;
	}
	m_playTime->stop();
	if (isPlaying)
	{
		m_playTime->start(10 / nrate);
	}
}

void ScenarioReplayDialog::on_pbtnDecelerate_clicked()
{
	auto m_rate = ui->ledtRate->text();
	int nrate = m_rate.toInt();
	{
		if (nrate > 1)
		{
			nrate /= 2;
			ui->ledtRate->setText(QString::number(nrate));
			LY_ScenariosReplay::Get().m_pIScenariosReplayThread->setRate(nrate);
		}
		else
		{
			ui->ledtRate->setText(QString::number(nrate));
			LY_ScenariosReplay::Get().m_pIScenariosReplayThread->setRate(1);
		}
	}
	if (isStopClicked)
	{
		return;
	}
	m_playTime->stop();
	if (isPlaying == true)
	{
		m_playTime->start(10 / nrate);
	}
}

void ScenarioReplayDialog::on_cbxTablesList_currentIndexChanged(int index)
{
	ui->pbtnLoadDatabase->setText(tr2("未导入"));
	m_databaseName = ui->cbxTablesList->currentText();

	viewModel->setCurrentData(ui->cbxTablesList->currentText());
}

void ScenarioReplayDialog::on_pbtnLoadDatabase_clicked()
{
	bool isLoad = viewModel->LoadReplayDataBase(m_databaseName);
	if (isLoad) ui->pbtnLoadDatabase->setText(tr2("已导入"));

	Init();
}
void ScenarioReplayDialog::onCheckBoxSelected(bool state)
{
	bool nstate = ui->rdbtnSest->isChecked();

	deleteThread(videoThread_A1);
	deleteThread(videoThread_A2);
	deleteThread(videoThread_A3);
	deleteThread(videoThread_A4);
	deleteThread(videoThread_A5);
	deleteThread(videoThread_A6);
	deleteThread(videoThread_A7);
	deleteThread(videoThread_A8);
	deleteThread(videoThread_A9);
	deleteThread(videoThread_A10);

	if (videoType == 3)
	{
		isSlectedVideo = true;
		videoThread_A1->setVideoPath(viewModel->getFileVec(), 8000, 3);
		videoThread_A2->setVideoPath(viewModel->getFileVec(), 8001, 3);
		videoThread_A3->setVideoPath(viewModel->getFileVec(), 18000, 3);
		videoThread_A4->setVideoPath(viewModel->getFileVec(), 18001, 3);
		videoThread_A5->setVideoPath(viewModel->getFileVec(), 28000, 3);
		videoThread_A6->setVideoPath(viewModel->getFileVec(), 28001, 3);
		videoThread_A7->setVideoPath(viewModel->getFileVec(), 38000, 3);
		videoThread_A8->setVideoPath(viewModel->getFileVec(), 38001, 3);
		videoThread_A9->setVideoPath(viewModel->getFileVec(), 48000, 3);
		videoThread_A10->setVideoPath(viewModel->getFileVec(), 48001, 3);
	}
	else if (videoType == 2) {

		isSlectedVideo = true;
		videoThread_A1->setVideoPath(viewModel->getFileVec(), 8001, 2);
		videoThread_A2->setVideoPath(viewModel->getFileVec(), 18001, 2);
		videoThread_A3->setVideoPath(viewModel->getFileVec(), 28001, 2);
		videoThread_A4->setVideoPath(viewModel->getFileVec(), 38001, 2);
		videoThread_A5->setVideoPath(viewModel->getFileVec(), 480001, 2);
	}
	else {

		isSlectedVideo = true;
		videoThread_A1->setVideoPath(viewModel->getFileVec(), 8000, 1);
		videoThread_A2->setVideoPath(viewModel->getFileVec(), 18000, 1);
		videoThread_A3->setVideoPath(viewModel->getFileVec(), 28000, 1);
		videoThread_A4->setVideoPath(viewModel->getFileVec(), 38000, 1);
		videoThread_A5->setVideoPath(viewModel->getFileVec(), 480000, 1);
	}

	if (nstate)
	{
		LY_ScenariosReplayInterface::Get().SendMsgToAll(IInterface::MSG_Command, 14001, nullptr);
		isSeatReplay = true;
	}
	else
	{
		LY_ScenariosReplayInterface::Get().SendMsgToAll(IInterface::MSG_Command, 14002, nullptr);
		isSeatReplay = false;

	}

}
void ScenarioReplayDialog::SelectedSendBoxState(bool state)
{
	viewModel->showSendDate(state);
}

void ScenarioReplayDialog::SelectedRecvBoxState(bool state)
{
	viewModel->showRecvDate(state);
}
void ScenarioReplayDialog::SelectedCheckBoxState(bool state)
{
	QCheckBox* chk = qobject_cast<QCheckBox*>(QObject::sender());
	if (chk != nullptr)
	{
		if (state == true)
		{
			if (chk == ui->chxTimeStat)
			{
				TypeSelectVec.push_back(0);
			}
			else if (chk == ui->chxSysControl)
			{
				TypeSelectVec.push_back(23);
			}
			else if (chk == ui->chxSysRun)
			{
				TypeSelectVec.push_back(24);
			}
			else if (chk == ui->chxTrackData)
			{
				TypeSelectVec.push_back(32);
			}
			else if (chk == ui->chxPlat)
			{
				TypeSelectVec.push_back(33);
			}
			else if (chk == ui->chxElectroPod)
			{
				TypeSelectVec.push_back(42);
			}
			else if (chk == ui->chxFireControl)
			{
				TypeSelectVec.push_back(37);
			}
			else if (chk == ui->chxinfraredAim)
			{
				TypeSelectVec.push_back(43);
			}
			else if (chk == ui->chxRadio)
			{
				TypeSelectVec.push_back(45);
			}
			else if (chk == ui->chxInterveneCMD)
			{
				TypeSelectVec.push_back(254);
			}
			else if (chk == ui->chxcmdMsg)
			{
				TypeSelectVec.push_back(160);
			}
			else if (chk == ui->chxFileConvey)
			{
				TypeSelectVec.push_back(161);
			}
			else if (chk == ui->chxIntelligenceAssign)
			{
				TypeSelectVec.push_back(255);
			}
			else if (chk == ui->chxSysUpdate)
			{
				TypeSelectVec.push_back(239);
			}
			else if (chk == ui->chxPic)
			{
				TypeSelectVec.push_back(999);
			}
			else if (chk == ui->chxVideo)
			{
				TypeSelectVec.push_back(998);

			}
		}
		else
		{
			if (chk == ui->chxTimeStat)
			{
				TypeSelectVec.erase(std::remove(TypeSelectVec.begin(), TypeSelectVec.end(), 0), TypeSelectVec.end());
			}
			else if (chk == ui->chxSysControl)
			{
				TypeSelectVec.erase(std::remove(TypeSelectVec.begin(), TypeSelectVec.end(), 23), TypeSelectVec.end());
			}
			else if (chk == ui->chxSysRun)
			{
				TypeSelectVec.erase(std::remove(TypeSelectVec.begin(), TypeSelectVec.end(), 24), TypeSelectVec.end());
			}
			else if (chk == ui->chxTrackData)
			{
				TypeSelectVec.erase(std::remove(TypeSelectVec.begin(), TypeSelectVec.end(), 32), TypeSelectVec.end());
			}
			else if (chk == ui->chxPlat)
			{
				TypeSelectVec.erase(std::remove(TypeSelectVec.begin(), TypeSelectVec.end(), 33), TypeSelectVec.end());
			}
			else if (chk == ui->chxElectroPod)
			{
				TypeSelectVec.erase(std::remove(TypeSelectVec.begin(), TypeSelectVec.end(), 42), TypeSelectVec.end());
			}
			else if (chk == ui->chxFireControl)
			{
				TypeSelectVec.erase(std::remove(TypeSelectVec.begin(), TypeSelectVec.end(), 37), TypeSelectVec.end());
			}
			else if (chk == ui->chxinfraredAim)
			{
				TypeSelectVec.erase(std::remove(TypeSelectVec.begin(), TypeSelectVec.end(), 43), TypeSelectVec.end());
			}
			else if (chk == ui->chxRadio)
			{
				TypeSelectVec.erase(std::remove(TypeSelectVec.begin(), TypeSelectVec.end(), 45), TypeSelectVec.end());
			}
			else if (chk == ui->chxInterveneCMD)
			{
				TypeSelectVec.erase(std::remove(TypeSelectVec.begin(), TypeSelectVec.end(), 254), TypeSelectVec.end());
			}
			else if (chk == ui->chxcmdMsg)
			{
				TypeSelectVec.erase(std::remove(TypeSelectVec.begin(), TypeSelectVec.end(), 160), TypeSelectVec.end());
			}
			else if (chk == ui->chxFileConvey)
			{
				TypeSelectVec.erase(std::remove(TypeSelectVec.begin(), TypeSelectVec.end(), 161), TypeSelectVec.end());
			}
			else if (chk == ui->chxIntelligenceAssign)
			{
				TypeSelectVec.erase(std::remove(TypeSelectVec.begin(), TypeSelectVec.end(), 255), TypeSelectVec.end());
			}
			else if (chk == ui->chxSysUpdate)
			{
				TypeSelectVec.erase(std::remove(TypeSelectVec.begin(), TypeSelectVec.end(), 239), TypeSelectVec.end());
			}
			else if (chk == ui->chxPic)
			{
				TypeSelectVec.erase(std::remove(TypeSelectVec.begin(), TypeSelectVec.end(), 999), TypeSelectVec.end());
			}
			else if (chk == ui->chxVideo)
			{
				TypeSelectVec.erase(std::remove(TypeSelectVec.begin(), TypeSelectVec.end(), 998), TypeSelectVec.end());

			}
		}
	}
	viewModel->fiterDateWithTypeVec(TypeSelectVec);
}

void ScenarioReplayDialog::beginSliderTimeDate(int aLowerValue)
{

	int value = 0;
	if (aLowerValue < tempBeginTimeAlowerTime && tempBeginTimeAlowerTime != 0)
	{
		value = nStartSeconds - aLowerValue;
	}
	else {
		value = aLowerValue + nStartSeconds;
	}

	viewModel->setBeginTime(value);
	tempBeginTimeAlowerTime = aLowerValue;
}

void ScenarioReplayDialog::endSliderTimeDate(int aLowerValue)
{
	viewModel->setEndTime(aLowerValue);
}
void ScenarioReplayDialog::LowerSliderChanged(int aLowerValue)
{
	aLowerValue = aLowerValue + nStartSeconds;

	int hours = aLowerValue / 3600;
	int minutes = (aLowerValue % 3600) / 60;
	int secs = aLowerValue % 60;

	int addTimeData = 0;
	if (hours / 24)
	{
		addTimeData = hours / 24;
		hours -= 24 * addTimeData;
	}

	bool bsucc = true;
	while (bsucc)
	{
		int nValue = 0;
		if (secs > 59)
		{
			minutes += secs / 60;
			continue;
		}
		else
			++nValue;
		if (minutes > 59)
		{
			hours += minutes / 60;
			continue;
		}
		else
			++nValue;
		if (hours > 23)
		{
			addTimeData += hours / 24;
			continue;
		}
		else
			++nValue;
		if (nValue >= 3)
			bsucc = false;
	}
	QTime time1(hours, minutes, secs);

	ui->tmEdtStart->setTime(time1);

	if (addTimeData)
	{
		QString matching = ui->dtEdtStart->text();
		QDateTime dataTimeBegin = QDateTime::fromString(matching, "yyyyMMdd");
		dataTimeBegin.addDays(addTimeData);
		ui->dtEdtStart->setDateTime(dataTimeBegin);
	}
}

void ScenarioReplayDialog::UpperSliderChanged(int aUpperValue)
{
	int hours = aUpperValue / 3600;
	int minutes = (aUpperValue % 3600) / 60;
	int secs = aUpperValue % 60;

	int addTimeData = 0;
	if (hours / 24)
	{
		addTimeData = hours / 24;
		hours -= 24 * addTimeData;
	}

	bool bsucc = true;
	while (bsucc)
	{
		int nValue = 0;

		if (secs > 59)
		{
			minutes += secs / 60;
			continue;
		}
		else
			++nValue;
		if (minutes > 59)
		{
			hours += minutes / 60;
			continue;
		}
		else
			++nValue;
		if (hours > 23)
		{
			addTimeData += hours / 24;
			continue;
		}
		else
			++nValue;
		if (nValue >= 3)
			bsucc = false;
	}

	QTime time1(hours, minutes, secs);
	qDebug() << "------------:" << hours << minutes << secs;

	ui->tmEdtEnd->setTime(time1);
	if (addTimeData)
	{
		QString matching = ui->dtEdtEnd->text();
		QDateTime dataTimeBegin = QDateTime::fromString(matching, "yyyyMMdd");
		dataTimeBegin.addDays(addTimeData);
		ui->dtEdtEnd->setDateTime(dataTimeBegin);
	}
}
ScenarioReplayDialog::~ScenarioReplayDialog()
{

	delete ui;
}
