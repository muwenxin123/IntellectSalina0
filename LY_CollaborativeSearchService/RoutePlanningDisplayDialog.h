#pragma once

#include <QWidget>
#include "ui_RoutePlanningDisplayDialog.h"

#include "UAVProtocol.h"
#include "DataManage/INebulaTaskManageService.h"
#include "DataManage/Common/TypeDefine_Node.h"
#include <QTimer>
#include "Protocol/XygsJL/sharedFile.h"
#include "LyDialog.h"
#include "vector3.h"

typedef std::vector<telemetryInformation>	TelemetryPointVec, *LpTelemetryPointVec;
typedef TelemetryPointVec::iterator			TelemetryPointVecItr;

typedef struct _CCalculateLine
{
	std::string				strWeaponName;
	std::string				strWeaponID;
	TelemetryPointVec		stTelemetryPointVec;

	_CCalculateLine()
	{
		strWeaponName = "";
		strWeaponID = "";
	}

} CCalculateLine;

typedef std::vector<CCalculateLine>	CCalculatePointVec, *LPCCalculatePointVec;
typedef CCalculatePointVec::iterator	CCalculatePointVecItr;

typedef struct UAVServerIP
{
	QString				strName;
	QString             strIP;
	int                 nPort;
	UAVServerIP()
	{
		strName = "";
		strIP = "";
		nPort = 0;
	}
}UAVServerIP;

typedef std::vector<UAVServerIP>	UAVServerIPVec, *LPUAVServerIPVec;
typedef UAVServerIPVec::iterator	UAVServerIPVecItr;

#define PLAY_HZ		2
#define PI       3.14159265358979323846

const double _earth_r_a = 6378137.000;
const double _earth_r_b = 6356752.3141;
const double _earth_e = 0.006694380022;

class RoutePlanningDisplayDialog : public LyDialog
{
	Q_OBJECT

public:
	RoutePlanningDisplayDialog(QWidget *parent = Q_NULLPTR);
	~RoutePlanningDisplayDialog();

	void init();

	void setTimeShow();

signals:
	void updatePosition();

public slots:

	void on_pbtnPlay_clicked();

	void on_pbtnStop_clicked();

	void on_pbtnAccelerate_clicked();

	void on_pbtnDecelerate_clicked();

	void on_timeOut();

	void LoadIPIni();

public:

	CCalculatePointVec										m_vecCalculatePoint;
	unsigned int											m_vecCalculateSumTime;
	unsigned int											m_vecCalculateMaxNum;
	UAVServerIPVec											m_UAVServerIPVec;
	bool                                                    b_isStart = false;
	int                                                     m_nPlaySpeed = 1;

	QTimer													*m_PlayTimer;
	double													 m_nPlayTime;
	UAVProtocol		  m_UAVProtocol;
	unsigned int	  _rate = 1;
	bool			  isStopClicked = false;

	std::map<int,casic_vector3d>shadowCube;

private:
	Ui::RoutePlanningDisplayDialog *ui;
};
