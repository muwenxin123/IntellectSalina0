#ifndef LY_TASKMANAGE_H
#define LY_TASKMANAGE_H

#include <QMetaType>
#include <QDir>
#include "ServiceFramework/IRegistry.h"
#include "ServiceFramework/IServiceInterfaceTracker.h"
#include "lY_AgroTaskManage_global.h"
#include <QCString.h>
#include "map3d/IGraphics.h"
#include "map3d/IEarth.h"
#include "QMenu"
#include "QTimer"
#include "GetHeight.h"
#include "CDlgTaskSchedule.h"
#include "CDlgTaskEventRecord.h"
#include "CDlgAgroTaskManage.h"
#include "CDlgDebug.h"
#include "CDlgAgroTaskManageEdit.h"
#include "CDlgCropGrowStageWidget.h"
#include "CDlgAgroParameterShow.h"
#include "CDlgAgroSampleParaShow.h"
#include "CDlgAgroManureParaShow.h"
#include "CDlgScanParaShow.h"

class QPainter;
class QMouseEvent;
class QTabWidget;
class IProj;

class LY_AGROTASKMANAGESHARED_EXPORT LY_AgroTaskManage
{
public:
    LY_AgroTaskManage();
    inline static LY_AgroTaskManage &Get();

	void X_S(double x, double y, float *l, float *b);
	void S_X(float x, float y, double *xx, double *yy);
	void L_X(double l, double b, double *x, double *y);
	void X_L(double x, double y, double *l, double *b);
	void L_S(double x, double y, int *sx, int *sy);
	void S_L(int sx, int sy, double *x, double *y);

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
    inline IProj *GetpProj()
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

    static CGetHeight s_CGetHeight;

	CDlgTaskSchedule*						getPDlgTaskSchedule() const;
	CDlgTaskEventRecord*				getPDlgTaskEventRecord() const;
	CDlgAgroTaskManage*				getPDlgAgroTaskManage() const;
	CDlgAgroTaskManageEdit*		getPDlgAgroTaskManageEdit() const;
	CDlgCropGrowStageWidget*		getPDlgCropGrowStageWidget() const;
	CDlgAgroParameterShow*			getCDlgAgroParameterShow() const;
	CDlgAgroSampleParaShow*		getPDlgAgroSampleParaShow() const;
	CDlgAgroManureParaShow*		getPDlgAgroManureParaShow() const;
	CDlgScanParaShow*					getPCDlgScanParaShow() const;

private:

    IProj								*m_pProj;
    QWidget								*m_pWnd;
    QWidget								*m_pTroopLabel;
    QCString							m_strDataDir;
    QCString							m_strConfDir;

	CDlgTaskSchedule*								m_pDlgTaskSchedule;
	CDlgTaskEventRecord*						m_pDlgTaskEventRecord;
	CDlgAgroTaskManage*						m_pDlgAgroTaskManage;
	CDlgAgroTaskManageEdit*				m_pDlgAgroTaskManageEdit;
	CDlgCropGrowStageWidget*				m_pDlgCropGrowStageWidget;
	CDlgDebug*										m_pDlgDebug;
	CDlgAgroParameterShow*					m_pDlgAgroParameterShow;
	CDlgAgroSampleParaShow*				m_pDlgAgroSampleParaShow;
	CDlgAgroManureParaShow*				m_pDlgAgroManureParaShow;
	CDlgScanParaShow*							m_pCDlgScanParaShow;
};

inline LY_AgroTaskManage &LY_AgroTaskManage::Get()
{
    static LY_AgroTaskManage sLY_TaskManage;
    return sLY_TaskManage;
}


#endif // LY_TASKMANAGE_H
