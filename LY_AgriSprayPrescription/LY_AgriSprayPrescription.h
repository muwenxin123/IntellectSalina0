
#ifndef LY_AgriSprayPrescription_H
#define LY_AgriSprayPrescription_H

#include "LY_AgriSprayPrescription_global.h"
#include <QCString.h>
#include <QMouseEvent>
#include "ly_agrispraypreshomewidget.h"
#include "ly_agrisprayprescommandwidget.h"

#include <map3d/IEarth.h>
#include <map3d/EarthEvent.h>
#include "ly_taskheaderwidget.h"
#include "ly_uavflylinemark.h"

class QPainter;
class IProj;

//#include "ly_sprayprescriptionviewmodel.h"

class LY_AGRISPRAYPRESCRIPTIONSHARED_EXPORT LY_AgriSprayPrescription
{ 

public:
    LY_AgriSprayPrescription();
    ~LY_AgriSprayPrescription();
    inline static LY_AgriSprayPrescription&Get();

	void Startup();
	void Shutdown();
	void Reset();

    bool DrawGraph(QPainter &painter, IProj *pProj);
    bool DrawDynamic(QPainter &painter, IProj *pProj);
    bool OnMouseButtonDown(QMouseEvent *e);
    bool OnMouseDoubleClick(QMouseEvent *e);
    bool OnCommand(int nID);
    bool OnCreate(int wParam, QWidget *pWidget);
    bool OnTimer(int wParam);
	bool OnNetRecv(int nLength, char *lpData);

	void OnEarthMessage(int wParam, void *lParam);

	inline const QCString GetDataDir() const { return m_strDataDir; };

	inline void SetDataDir(const char *lpszDir)
	{
		m_strDataDir = lpszDir;
	}
	inline void SetConfDir(const char *lpszDir)
	{
		m_strConfDir = lpszDir;
	}
	inline void SetCoordinateTransformation(IProj *pProj)
	{
		m_pProj = pProj;
	}


	LY_AgriSprayPresHomeWidget *GetSprayPresHomeWidget() const;
	LY_AgriSprayPresCommandWidget *GetSprayPresCommandWidget() const;
  //  LY_SprayPrescriptionViewModel *GetViewModel();
	LY_TaskHeaderWidget *GetTaskHeaderWidget() const;
	LY_UAVFlyLineMark *GetUavFlyLineMarkWidget() const;
private:
	IProj       *m_pProj;
	QCString	m_strDataDir;
	QCString	m_strConfDir;
	bool         m_IsShowReightWidget = true;

	LY_AgriSprayPresHomeWidget *m_sprayPrescriptionWidget;
	LY_AgriSprayPresCommandWidget *m_commandWidget;
   // LY_SprayPrescriptionViewModel *m_viewModel;
	LY_TaskHeaderWidget *m_taskHeaderWidget;
	LY_UAVFlyLineMark *m_uavFlyLineMarkWidget;
};

inline LY_AgriSprayPrescription &LY_AgriSprayPrescription::Get()
{
    static LY_AgriSprayPrescription sLY_MonitorMoudle;
    return sLY_MonitorMoudle;
}


#endif
