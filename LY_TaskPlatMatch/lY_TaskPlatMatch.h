#ifndef LY_TASKPLATMATCH_H
#define LY_TASKPLATMATCH_H

#include "lY_TaskPlatMatch_global.h"
#include "lY_TaskPlatMatch.h"
#include "CDlgTaskPlatMatch.h"
#include <QCString.h>
#include "ServiceFramework/IRegistry.h"
#include "ServiceFramework/IServiceInterfaceTracker.h"
#include <IFunctionInterface.h>
#include "lY_TaskPlatMatchInterface.h"
#include "ITaskPlatMatchActivator.h"
#include "CDlgSetPlat.h"

class QPainter;
class QMouseEvent;
class QTabWidget;
class IProj;

class QWidget;
class CDlgTaskPlatMatch;
class CDlgSetPlat;

class LY_TASKPLATMATCHSHARED_EXPORT LY_TaskPlatMatch
{
public:
    LY_TaskPlatMatch();
    inline static LY_TaskPlatMatch &Get();

    void Startup();
    void Shutdown();
    void Reset();

    bool DrawGraph(QPainter &painter, IProj *pProj);
    bool DrawDynamic(QPainter &painter, IProj *pProj);
    bool OnMouseButtonDown(QMouseEvent *e);
    bool OnMouseDoubleClick(QMouseEvent *e);
    bool OnCommand(int nID);
    bool OnCreate(int wParam, QTabWidget *pTab);
    bool OnTimer(int wParam);
    inline void SetCoordinateTransformation(IProj *pProj)
    {
        m_pProj = pProj;
    };
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
    void showtestSql();

    CDlgTaskPlatMatch *gettaskPlatMatch();

public:
    bool        isPlatMateMerge = false;
private:
    IProj       *m_pProj;
    QWidget		*m_pWnd;
    QCString	m_strDataDir;
    QCString	m_strConfDir;

    CDlgTaskPlatMatch *m_pDlgTaskPlatMatch;

};

inline LY_TaskPlatMatch &LY_TaskPlatMatch::Get()
{
    static LY_TaskPlatMatch sLY_TaskPlatMatch;
    return sLY_TaskPlatMatch;
}

#endif
