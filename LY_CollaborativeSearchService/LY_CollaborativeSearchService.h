#ifndef LY_COLLABORATIVESEARCHSERVICE_H
#define LY_COLLABORATIVESEARCHSERVICE_H

#include "lY_CollaborativeSearchService_global.h"
#include <QCString.h>
#include <QLine>
#include <vector>
#include "MissionLoadDialog.h"
#include "GetHeight.h"
#include "QKeyEvent"

#include "ICollaborativeSearchActivator.h"
#include "uav_dispose_route_dialog.h"
#include "SystemEventRecording.h"
#include "UAVAreaChooseDialog.h"

class QPainter;
class QMouseEvent;
class QTabWidget;
class IProj;

class QListView;
class QTreeView;
class PopForm;
class QWidget;

class RoutePlanningDialog;
class RoutePlanningDisplayDialog;
class UAV_DisPose_Route_Dialog;

class LY_COLLABORATIVESEARCHSERVICESHARED_EXPORT LY_CollaborativeSearchService
{
public:
    LY_CollaborativeSearchService();
    inline static LY_CollaborativeSearchService &Get();

	void Startup();
	void Shutdown();
	void Reset();

    bool DrawGraph(QPainter &painter, IProj *pProj);
    bool DrawDynamic(QPainter &painter, IProj *pProj);
    bool OnMouseButtonDown(QMouseEvent *e);
    bool OnMouseDoubleClick(QMouseEvent *e);
	void mouseReleaseEvent(QMouseEvent *e);
	void mouseMoveEvent(QMouseEvent *e);
    bool OnCommand(int nID, void *lParam = nullptr);
    bool OnCreate(int wParam, QWidget *pTab);
    bool OnTimer(int wParam);
	bool OnNetRecv(int nLength, char *lpData);
    inline void SetCoordinateTransformation(IProj *pProj){ m_pProj = pProj;};
	inline IProj * GetProj() { return m_pProj; };
    inline const QCString GetDataDir() const { return m_strDataDir; };
    inline const QCString GetConfDir() const { return m_strConfDir; };
    inline void SetDataDir(const char *lpszDir) { m_strDataDir = lpszDir; };
    inline void SetConfDir(const char *lpszDir) { m_strConfDir = lpszDir; };

	void KeyPress(QKeyEvent *e);
	void KeyRelease(QKeyEvent*e);

	void OnEarthMessage(int wParam, void *lParam);

public:
    RoutePlanningDialog *m_RoutePlanningDialog = nullptr;
    RoutePlanningDisplayDialog *m_routePlanningDisplayDialog = nullptr;
    UAV_DisPose_Route_Dialog* m_uav_dispose_route_dialog = nullptr;
	MissionLoadDialog* m_mission_load_dialog = nullptr;
	UAVAreaChooseDialog* m_UAVAreaChooseDialog = nullptr;
    SystemEventRecording* m_SystemEventRecording = nullptr;
	static CGetHeight s_CGetHeight;

	unsigned int						m_nColor[25];

private:
	bool mouseLeftIsDown = false;
	bool mouseIsMove = false;
	bool key_CtrlDown = false;

private:
    QListView	*m_pList;
    QTreeView	*m_pTree;
    PopForm		*m_pPopForm;
    IProj       *m_pProj;
    QWidget		*m_pWnd;
    QCString	m_strDataDir;
    QCString	m_strConfDir;
};

inline LY_CollaborativeSearchService &LY_CollaborativeSearchService::Get()
{
    static LY_CollaborativeSearchService sLY_CollaborativeSearchService;
    return sLY_CollaborativeSearchService;
}

#endif
