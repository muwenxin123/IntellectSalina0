#ifndef CDLGPLAY_H
#define CDLGPLAY_H

#include <QWidget>
#include "LyDialog.h"
#include "DataManage/INebulaTaskManageService.h"
#include "DataManage/Common/TypeDefine_Node.h"
#include "DataManage/NebulaTask/NebulaMissionLine.h"
#include "DataManage/IUAVRealTimeStateManageService.h"
#include "CommonStruct.h"
#include "CDlgEditGroupp.h"
#include <QGeoCoordinate>
#include "UAVProtocol.h"
#include "vector3.h"
#include "IProj.h"
#include "map3d/IEarth.h"
#include <map3d/IGraphics.h>
#include "CreateShape.h"
#include "DataManage/IZoneManageService.h"

#include <QVector3D>
#include "geometric.h"
#define PLAY_HZ		2

typedef std::vector<telemetryInformation>	TelemetryPointVec, *LpTelemetryPointVec;
typedef TelemetryPointVec::iterator			TelemetryPointVecItr;

typedef struct _CCalculateLine
{
    std::string				strWeaponName;
    std::string				strWeaponID;
    std::string				strStartTime;
    TelemetryPointVec		stTelemetryPointVec;

    _CCalculateLine()
    {
        strWeaponName = "";
        strWeaponID = "";
    }

} CCalculateLine;

typedef struct SpeedControl
{
	double speed;
	double amount;
	std::string UAVName;
	SpeedControl()
	{
		speed = 0;
		amount = 0;
		UAVName = "";
	}
};

typedef struct UAVStatus
{
	int nSubductionEnd = 0;
	int status = -1;
	int nQuadrate = 0;
	double distane = -1;

};

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
} UAVServerIP;

typedef std::vector<UAVServerIP>	UAVServerIPVec, *LPUAVServerIPVec;
typedef UAVServerIPVec::iterator	UAVServerIPVecItr;

typedef std::map<std::string, int>					PlatAndLineIDMap;
typedef std::map<std::string, PlatAndLineIDMap>		MissionIDAndPlatMap;
typedef std::map<std::string, bool>					PlatPlayingMap;

namespace Ui
{
class CDlgPlay;
}

class CDlgPlay : public LyDialog
{
    Q_OBJECT

public:
    explicit CDlgPlay(QWidget *parent = nullptr);
    ~CDlgPlay();

	bool												m_bDeviation = false;
	int													n_WaitTimer = 0;
	double												dLongitude = 0;
	double												dLatitude = 0;
	std::vector<SpeedControl>							m_PlatSpeedCol;
	std::map<std::string, double>						m_SpeedColTime;
	CCalculatePointVec									m_vecCalculatePoint;
	std::vector<std::string>							m_VecUAVName;
	QMap<QString, QString>								m_mapAdjustUAVTelemetry;
	QMap<QString,QString>								m_VecAdjustUAVTelemetrySpeed;
	std::map<std::string, std::string>					m_SystemSysNameAndID;
	qnzkna::ZoneManage::CZoneBaseRule					m_BaseRuleStu;
	UAVStatus											m_UAVStatus;

private:
    Ui::CDlgPlay *ui;

private:
	bool                                                    m_bIsShowLines = false;
	bool													m_draw3DFirst = true;
    bool                                                    m_LoadDataCal;
    bool													m_bPlayed;
    unsigned int											m_vecCalculateMaxNum;
    unsigned int                                            m_nTaskID;
    unsigned int                                            m_nGroupID;
	int														m_nPlaySpeed;
    qint64													m_CurrentTime;
    double													m_nPlayTime;

    STWndRouteLineInfoList									m_stWndRouteLineInfoList;
    CNebulaRoutePoint										m_NebulaRoutePoint;
    CNebulaRouteLine::RouteLineIDList						m_Routelinelist;
    CNebulaRoutePoint::RoutePointIDList						m_RoutePointIDlist;
    qnzkna::NodeManage::CNodeRelationInfoPtrVector          m_vecNodeInfo;
    qnzkna::TaskManage::INebulaTaskManageService							*m_pITaskManageService = nullptr;
    qnzkna::WeaponManage::IWeaponManageService								*m_pIWeaponManageService = nullptr;
    qnzkna::UAVRealTimeStateManage::IUAVRealTimeStateManageService			*m_pIUAVRealTimeStateManageService = nullptr;
	qnzkna::ZoneManage::IZoneManageService*									m_pIZoneManageService = nullptr;

    QTimer													*m_PlayTimer;
    UAVProtocol												m_UAVProtocol;
	UAVServerIPVec											m_UAVServerIPVec;
    CNeubulaMissionLineVec									m_MissionLineVec;
    CNeubulaMissionLineVec									m_MissionLineShowVec;

    std::map<int, std::string>								m_LineIDAndSystemSysID;
    MissionIDAndPlatMap										m_SelectedLineMap;
    std::string												m_StartTime;
    PlatPlayingMap											m_PlatPlayingMap;
	std::map<std::string, std::vector<casic_vector3d>>		m_route_lines;
	std::vector<casic_vector3d>                             m_returnPosList;

	std::vector<IUserMesh1*> Path3DLines;
	std::vector<IRenderableObject*> Path3DLines_plat;
	QTimer													*m_LinesTimer;
	std::map<std::string, double>                            m_mUAVbeginAngle;
	std::map<std::string, double>                            m_mUAVbeginDis;
	QMutex													 m_qmutex;

	std::vector<std::pair<QString, QGeoCoordinate>>			m_mapLocatonCoverUAV;

	QMap<std::string, double>								m_mapUAVChangeHeight;
	QMap<std::string, double>								m_mapHorizHeight;

public:

    void ClearWndRouteLineInfoList();

    void UpdateRouteLineInfoList(unsigned int nTaskID, unsigned int nGroupID, qnzkna::NodeManage::CNodeRelationInfoPtrVector	&vecNodeInfo);
    void LoadDataRouteLineInfoList(unsigned int nTaskID, unsigned int nGroupID, qnzkna::NodeManage::CNodeRelationInfoPtrVector	&vecNodeInfo);

    void CalculatePoint();

    double DegreesToRadians(double angle);

    double RadiansToDegrees(double angle);

    double MakeSureAngel(double dangle);

    void on_timeOut();

    void LoadIPIni();

    void SetPlayStates(bool bStates);

    void CreateListHeading();

    void UpdataWindowData();
    void UpdateShowData();
    void UpdateListData();

    QString GetLineTypeStr(int nLineType);
    QString GetLineProtocolStr(int nLinePro);

    void TableWidgetAddCheckBox(QTableWidget *tableWidget, int x, int y, QString text, Qt::CheckState CheckState);

    std::string LineMatchPlatform(const std::string &strMissionID, const int &nLineID, const int &nNo);

    void localWeaponNameBindToId();

    std::string GetPlatByLineID(const int nLineID);
    QString	GetPlatNameByID(const std::string strPlatID);

    void UpdateLabel(qint64 CurrentTime, QString strTIps);

    void SortDataByStartTime();

    bool JudgeLinePlayTime();

    qint64 CalculateTime(const std::string &strTime01, const std::string &strTime02);

	void drawPoint(QPainter & painter, IProj * pProj);
	//»æÖÆÈýÎ¬º½Ïß
	void draw3DPathLine(std::map<std::string, std::vector<casic_vector3d>> &PointsVec, QColor pathColor, int lineWidth = 2);

	void del3DPathLine();

	void on_LinesTimeOut();

	void ForecastRouteTendency_FlyRule(QString UAVName, QString UAVName1, qnzkna::UAVRealTimeStateManage::NebulaUAVRealTimeStateMap UAVRealTimeStateMap);

	Vector3D toCartesian(const QPointF& p);
	Vector3D calculateVector(const QPointF& A, const QPointF& B);
	double calculateAngle(QPointF firstPoint1, QPointF firstPoint2, QPointF secondPoint1, QPointF secondPoint2);
	double CalculatePointDistance(const QGeoCoordinate A, const QGeoCoordinate B);

signals:
	void DLGPlayDeviationVoyage();
	void DLGPlayVoyageEditHeight();
private slots:

    void on_BT_LoadData_clicked();

    void on_BT_Play_clicked();

    void on_BT_Stop_clicked();

    void on_BT_decline_clicked();

    void on_BT_Accelereate_clicked();

    void on_checkBox_stateChanged(int arg1);

    void on_tableWidget_cellClicked(int row, int column);

    void on_pushButton_4_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void lineShowStateAll(bool valueFromSettings);

	void DealDLGPlayDeviationVoyage();

	void setSelectShowLine(bool state);
	void DealDLGPlayVoyageEditHeight();

protected:

    virtual void changeEvent(QEvent *event);
    virtual void showEvent(QShowEvent *event);
    virtual void closeEvent(QCloseEvent *e);

};

#endif
