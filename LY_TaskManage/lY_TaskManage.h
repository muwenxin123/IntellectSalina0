#ifndef LY_TASKMANAGE_H
#define LY_TASKMANAGE_H

#include <QMetaType>
#include <QDir>
#include "ServiceFramework/IRegistry.h"
#include "ServiceFramework/IServiceInterfaceTracker.h"
#include "lY_TaskManage_global.h"
#include <QCString.h>
#include "CDlgActionCompendium.h"
#include "ITaskManagerActivator.h"
#include "CDlgOpenFile.h"
#include "CDlgEditGroupp.h"
#include "CDlgEditLine.h"
#include "CDlgPlay.h"
#include "CDlgMissionList.h"
#include "CDlgMissionEdit.h"
#include "CDlgMissionPlatformList.h"
#include "CDlgMissionPlatformEdit.h"
#include "CDlgMissionPlatformLoadList.h"
#include "CDlgMissionPlatformLoadEdit.h"
#include "map3d/IGraphics.h"
#include "map3d/IEarth.h"
#include "CDlgAutoPoint.h"
#include "CDlgMissionTree.h"
#include "CDlgActionList.h"
#include "CDlgActionEdit.h"
#include "CDlgCodeSet_Load_Tree.h"
#include "CDlgCodeSet_Load_Edit.h"
#include "QMenu"
#include "CDlgSensorManage.h"
#include "CDlgFireManage.h"
#include "CDlgCommunicateManage.h"
#include "CDlgMissionLineList.h"
#include "CDlgSensorManageEdit.h"
#include "QTimer"
#include "CDlgFireManageEdit.h"
#include "CDlgMissionLinePointEdit.h"
#include "CDlgTaskLineEdit.h"
#include "ITaskManagerActivator.h"
#include "CDlgMissionPlatformInfo.h"
#include "CDlgMarkSchemeList.h"
#include "CDlgMissionAllocation.h"
#include "CDlgMissionPlanningOverview.h"
#include "CDlgMarkMaintain.h"
#include "CDlgAddMaintainMark.h"
#include "WidgetLineEdit.h"
#include "CDlgTaskLineEdit.h"
#include "CDlgPlaySpeedControl.h"
#include "GetHeight.h"

typedef std::vector<IUserMesh1 *>	IUserMesh1Vec;
typedef std::vector<ILabel *>		ILabelVec;

struct UAV3D
{
    std::string		strID;
    ARGB			colorLine;
    IUserMesh1Vec	pMesh1Vec;
    ILabelVec		pLabelVec;
};

typedef std::map<std::string, UAV3D> UAV3DMap;

struct SensorLoadSSection
{
    int				nStartNo;
    int				nStopNo;
    SensorLoadSSection()
    {
        nStartNo = 0;
        nStopNo = 0;
    }
};
typedef std::vector<SensorLoadSSection> SensorLoadSSectionVec;
typedef std::map<int, SensorLoadSSection> ScanParaterMap;

struct SensorLoadStats
{
    SensorLoadSSectionVec		PowerVec;
    SensorLoadSSectionVec		ScanVec;
    ScanParaterMap				ScanParaMap;
};
typedef std::map<std::string, SensorLoadStats> SensorLoadStatsMap;

class QPainter;
class QMouseEvent;
class QTabWidget;
class IProj;

class CDlgActionResourcesEdit;
class CDlgTaskLineEdit;
class WidgetLineEdit;
class CDlgMissionAllocation;
class IResourceCarryingPlanDialog;
class ISAddCarryingPlanResDialog;
class CDlgForecastTendency;

class LY_TASKMANAGESHARED_EXPORT LY_TaskManage
{
public:
    LY_TaskManage();
    inline static LY_TaskManage &Get();

    void Startup();
    void Shutdown();
    void Reset();

    bool DrawGraph(QPainter &painter, IProj *pProj);
    bool DrawDynamic(QPainter &painter, IProj *pProj);
    bool OnMouseButtonDown(QMouseEvent *e);
    bool OnMouseButtonRelease(QMouseEvent *e);
    bool OnMouseMove(QMouseEvent *e);
    bool OnMouseDoubleClick(QMouseEvent *e);
    bool OnCommand(int nID);
    bool OnCreate(int wParam, QTabWidget *pTab);
    bool OnTimer(int wParam);
    inline void SetCoordinateTransformation(IProj *pProj)
    {
        m_pProj = pProj;
    };
	inline IProj* GetpProj()
	{
		return m_pProj;
	}
    inline const QCString GetDataDir() const
    {
        return m_strDataDir;
    };
    inline const QCString GetConfDir() const
    {
        return m_strConfDir;
    };
    inline void SetDataDir(const char *lpszDir)
    {
        m_strDataDir = lpszDir;
    };
    inline void SetConfDir(const char *lpszDir)
    {
        m_strConfDir = lpszDir;
    };

    void setTroopName(const QString &name);
    QWidget *getPWnd() const;
    void DrawPointIcon(QPainter &painter, QPoint pt, bool Selected);
    void DrawCodeSetPointIcon(QPainter &painter, QPoint pt, bool Selected);
    void DrawMissionStartIcon(QPainter &painter, QPoint pt, QString strName);
    bool JudegePointCodeSet(const std::string strUAVID, const int nPointID);

    bool JudgePoint(QPoint pt, int &nLineID, int &nPoint);
    bool JudgePointMisson(QPoint pt, int &nLineID, int &nPoint);
    bool MatchLinePoint(QPoint mp, QPoint lp);

    bool MouseButtonDownOld(QMouseEvent *e);
    bool MouseButtonDownMission(QMouseEvent *e);

    bool MouseMoveOld(QMouseEvent *e);
    bool MouseMoveMission(QMouseEvent *e);
    bool MouseMoveMarkManage(QMouseEvent *e);

    void SetScreenCenter(double &dLon, double &dLat);

    void Draw3DLinesWeaponID(const std::string &strWeaponID);
    void Clear3DlinesWeaponID(const std::string &strWeaponID);

    void Draw3DLines();
    void Clear3Dlines();

    void SetPoint(double dLon, double dLat, double dHeigeth);

    void CreateMenu();
    void on_Menu_CodeSet();

    void UpdateCodeSetSensor();
    void UpdateCodeSetFire();

    void ArrangeSensorCodeSet(SensorLoadStatsMap &tmpLoadMap, CCodeSet_Load_DB_Vec &tmpCodeSetVec);
    void DrawSensorView(QPainter &painter, IProj *pProj);
    void DrawSensorViewEllipse(QPainter &painter, IProj *pProj, const QPoint StartPoint, const int nLength, const int nAreaAngel, const int nCurrentAngel, const QColor color, const int nOpenness);
    void DrawLineOld(QPainter &painter, IProj *pProj);
    void DrawMissionLine(QPainter &painter, IProj *pProj);

    static void on_ScanTimer();
    void DrawFireView(QPainter &painter, IProj *pProj);
    void DrawCommunicateView(QPainter &painter, IProj *pProj);
    void DrawCommunicateCoverView(QPainter &painter, IProj *pProj);
    void UpdateShapLineVec();

    void DrawMeasureIntervisibility(QPainter &painter, IProj *pProj);

    CDlgAutoPoint                  *GetDlgAutoPoint();
    CDlgActionEdit                 *GetCDlgActionEdit();
    CDlgActionList					*GetCDlgActionList();
    CDlgEditLine					*GetDlgEditLine();

    CDlgMissionList				*GetDlgMissionList();
    CDlgMissionEdit				*GetDlgMissionEdit();
    CDlgMissionPlatformList		*GetDlgMissionPlatformList();
    CDlgMissionPlatformEdit		*GetDlgMissionPlatformEdit();
    CDlgMissionPlatformLoadList	*GetDlgMissionPlatformLoadList();
    CDlgMissionPlatformLoadEdit	*GetDlgMissionPlatformLoadEdit();
    CDlgPlay						*GetDlgPlay();

    CDlgCodeSet_Load_Tree			*getDlgCodeSet_Load_Tree() const;
    CDlgCodeSet_Load_Edit			*getDlgCodeSet_Load_Edit() const;
    CDlgMissionLineList			*GetDlgMissionLineList() const;
    CDlgMissionLinePointEdit		*GetDlgMissionLinePointEdit() const;
    CDlgTaskLineEdit				*GetDlgTaskLineEdit() const;
    void							NewDlgTaskLineEdit();
    CDlgSensorManageEdit			*GetDlgSensorManageEdit() const;
    CDlgFireManageEdit				*GetDlgFireManageEdit() const;
    CDlgMissionTree                *GetDlgMissionTree() const;
    CDlgMissionPlatformInfo        *GetDlgMissionPlatfromInfo();
    CDlgMarkSchemeList           *GetDlgMarkSchemeList() const;
    CDlgMissionPlanningOverview    *GetDlgMissionPlanningOverview() const;
    CDlgMissionAllocation          *GetDlgMissionAllocation() const;
    CDlgAddMaintainMark			*GetDlgAddMaintainMark() const;
    CDlgMarkMaintain				*GetDlgMaintainMark() const;
    IResourceCarryingPlanDialog	*GetIResourceCarryingPlan()  const;
    ISAddCarryingPlanResDialog     *GetISAddCarryingPlanResDialog() const;
	CDlgPlaySpeedControl			*GetPlaySpeedControl() const;
	CDlgForecastTendency			*GetForecastTendency() const;
    inline QLabel					*GetZzTimeLable() const
    {
        return m_pZzTimeLabel;
    }
    IProj							*GetIProj() const;
    WidgetLineEdit                 *GetWidgetLineEdit()const;
	static CGetHeight s_CGetHeight;

private:
    IProj								*m_pProj;
    QWidget								*m_pWnd;
    QWidget								*m_pTroopLabel;
    QLabel								*m_pZzTimeLabel;
    QCString							m_strDataDir;
    QCString							m_strConfDir;

    CDlgEditLine						*m_pDlgEditLine = nullptr;
    CDlgPlay							*m_pDlgPlay = nullptr;
    CDlgMissionList						*m_pDlgMissionList = nullptr;
    CDlgMissionEdit						*m_pDlgMissionEdit = nullptr;
    CDlgMissionPlatformList				*m_pDlgMissionPlatformList = nullptr;
    CDlgMissionPlatformEdit				*m_pDlgMissionPlatformEdit = nullptr;
    CDlgMissionPlatformLoadList			*m_pDlgMissionPlatformLoadList = nullptr;
    CDlgMissionPlatformLoadEdit			*m_pDlgMissionPlatformLoadEdit = nullptr;
    CDlgActionList						*m_pDlgActionList = nullptr;
    CDlgActionEdit						*m_pDlgActionEdit = nullptr;
    CDlgAutoPoint						*m_pDlgAutoPoint = nullptr;
    CDlgMissionTree						*m_pDlgMissionTree = nullptr;
    CDlgCodeSet_Load_Tree				*m_pDlgCodeSet_Load_Tree = nullptr;
    CDlgCodeSet_Load_Edit				*m_pDlgCodeSet_Load_Edit = nullptr;
    CDlgSensorManageEdit				*m_pDlgSensorManageEdit = nullptr;
    CDlgFireManage						*m_pDlgFireManage = nullptr;
    CDlgFireManageEdit					*m_pDlgFireManageEdit = nullptr;
    CDlgCommunicateManage				*m_pDlgCommunicateManage = nullptr;
    CDlgMissionLineList					*m_pDlgMissionLineList = nullptr;
    CDlgMissionLinePointEdit			*m_pDlgMissionLinePointEdit = nullptr;
    CDlgTaskLineEdit					*m_pDlgTaskLineEdit = nullptr;
    CDlgMarkSchemeList                *m_pDlgMarkSchemeList = nullptr;
    CDlgMissionPlanningOverview			*m_pDlgMissionPlanningOverview = nullptr;
    CDlgMarkMaintain					*m_pDlgMarkMaintain = nullptr;
    CDlgAddMaintainMark				*m_pDlgAddMaintainMark = nullptr;
    IResourceCarryingPlanDialog			*m_pResourceCarryingPlanDialog = nullptr;
    ISAddCarryingPlanResDialog			*m_pISAddCarryingPlanResDialog = nullptr;
    WidgetLineEdit                      *m_pWidgetLineEdit = nullptr;
	CDlgPlaySpeedControl				*m_pDlgPlaySpeedControl = nullptr;
	CDlgForecastTendency				*m_pDlgForecastTendency = nullptr;

    unsigned int						m_nColor[25];
    int									m_nColorNumber;
    QPixmap								m_IconImage;
    QPixmap								m_IconImageSelected;
    QPixmap								m_CodeSetIconImage;
    QPixmap								m_MissionHomeIcon;
    bool								m_bMouseMove = false;
    bool								m_bMissionMouseMove = false;
    bool                                m_FlightTaskManageEditLineElevation = true;
    QString                             m_FlightTaskManageEditLineElevationDir;
    int									m_nLineID;
    int									m_nPointID;
    int									m_nMissionSelectLineID = 0;
    int									m_nMissionSelectPointID = 0;

    UAV3DMap							m_UAV3DMap;
    QString								m_strIconPatch;

    QMenu								*m_LineMenu;
    QAction								*m_MenuCodeSetEdit;

    QTimer								*m_ScanTimer;

    QMutex								m_MutexTimer;

    QMutex								m_MutexShapLine;

    SHPObject							*m_pShp[2] = {nullptr};
    std::vector<std::vector<SHPObject *>> m_pShpVec;
    ShapLineVec							m_ShapLineVec;

    bool								bflag = false;

public:
    int									m_nTimer = 0;
    CDlgActionResourcesEdit             *m_pCDlgActionResourcesEdit = nullptr;
    CDlgMissionPlatformInfo             *m_pDlgMissionPlatfromInfo = nullptr;
    CDlgMissionAllocation               *m_pDlgMissionAllocation = nullptr;
    CDlgSensorManage					*m_pDlgSensorManage = nullptr;
    bool                                bIntervisibility = false;
};

inline LY_TaskManage &LY_TaskManage::Get()
{
    static LY_TaskManage sLY_TaskManage;
    return sLY_TaskManage;
}

#endif
