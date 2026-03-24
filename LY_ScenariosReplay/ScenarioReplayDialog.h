#ifndef SCENARIOREPLAYDIALOG_H
#define SCENARIOREPLAYDIALOG_H

#include <QDialog>
#include "QtSql\QSqlDatabase"
#include "QtSql\QSqlQuery"
#include <QMutex>
#include <QMenu>
#include "IScenariosReplayThread.h"
#include <iostream>
#include <vector>
#include "RangeSlider.h"
#include "ISReplayViewModel.h"
#include "ISSendVideoThread.h"
#include <QTimer>
#include <QTime>

using namespace std;

namespace Ui {
class ScenarioReplayDialog;
}

class ScenarioReplayDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ScenarioReplayDialog(QWidget *parent = nullptr);
    ~ScenarioReplayDialog();

	void stopTimer();

	void resetPlay();

	void Init();

public:
	ISSendVideoThread                 *videoThread_A1;
	ISSendVideoThread                 *videoThread_A2;
	ISSendVideoThread                 *videoThread_A3;
	ISSendVideoThread                 *videoThread_A4;
	ISSendVideoThread                 *videoThread_A5;
	ISSendVideoThread                 *videoThread_A6;
	ISSendVideoThread                 *videoThread_A7;
	ISSendVideoThread                 *videoThread_A8;
	ISSendVideoThread                 *videoThread_A9;
	ISSendVideoThread                 *videoThread_A10;
	int videoType;

	ISReplayViewModel    *viewModel;
	QString                        m_databaseName;

	bool									isPlaying = false;
	bool									isStopPlaying = false;
	bool									isStopClicked = false;

	bool                                    isSeatReplay = false;

	QTimer									*m_playTime;

	RangeSlider*							m_RsHsing;
	QString									m_strStartTime;
	QString									m_strEndTime;
	int										nStartSeconds;
	int										nEndSeconds;
	QMenu									*m_menu;
	QAction									*m_1stBtn;
	QAction									*m_2rdBtn;
	QAction									*m_5thBtn;
	QAction									*m_10thBtn;

	std::map<QString, QByteArray>              m_ReplayData;

	std::vector<int> TypeSelectVec;

private:
	void addSlotsAction();
	void sureVideoData(set<LogDataInfo> temp_set);
	void deleteThread( ISSendVideoThread  *&qthread);
	void initThread(ISSendVideoThread  *&qthread);

signals:
	void progressBarValue(const unsigned int value, const unsigned int nNo, const unsigned int nMax);
	void transmitData(QString &str);

private slots:
    void on_pbtnPlay_clicked();

    void on_pbtnPause_clicked();

    void on_pbtnStop_clicked();

    void on_pbtnAccelerate_clicked();

    void on_pbtnDecelerate_clicked();

    void on_cbxTablesList_currentIndexChanged(int index);

    void on_pbtnLoadDatabase_clicked();

	void onCheckBoxSelected(bool state);

	void SelectedCheckBoxState(bool state);

	void SelectedSendBoxState(bool state);

	void SelectedRecvBoxState(bool state);

	void LowerSliderChanged(int aLowerValue);

	void UpperSliderChanged(int aUpperValue);

	void beginSliderTimeDate(int aLowerValue);

	void endSliderTimeDate(int aLowerValue);

	void hiddenNormalChexkbox();

public slots:
	void setTimeShow();

	void setProgressBarValue(const unsigned int value, const unsigned int nNo, const unsigned int nMax);

	void InitUI();

	void initMenu();

	void action_Triggerd(QAction* action);

private:

	bool  isSlectedVideo;
	int  tempPlayTime = QTime::currentTime().second();
    Ui::ScenarioReplayDialog *ui;

	int tempBeginTimeAlowerTime = 0;
	int tempendTimeAlowerTime;
};

#endif
