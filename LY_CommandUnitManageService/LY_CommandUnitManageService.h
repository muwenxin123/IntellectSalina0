#ifndef LY_COMMANDUNITMANAGESERVICE_H
#define LY_COMMANDUNITMANAGESERVICE_H
#include <QMetaType>
#include "LY_CommandUnitManageService_global.h"
#include "ServiceFramework/IRegistry.h"
#include "ICommandUnitManageImpl.h"
#include <QCString.h>

class QPainter;
class QMouseEvent;
class QTabWidget;
class IProj;

class QListView;
class QTreeView;
class PopForm;
class QWidget;

Q_DECLARE_METATYPE(qnzkna::framework::IRegistry *)
class LY_COMMANDUNITMANAGESERVICESHARED_EXPORT LY_CommandUnitManageService
{
public:
    LY_CommandUnitManageService();
    inline static LY_CommandUnitManageService &Get();

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

private:
    static qnzkna::framework::IRegistry				*m_pIRegistryImpl;
    qnzkna::CCommandUnitManage::CCommandUnitServiceImpl s_commandunitServiceImpl;

private:
    QListView	*m_pList;
    QTreeView	*m_pTree;
    PopForm		*m_pPopForm;
    IProj       *m_pProj;
    QWidget		*m_pWnd;
    QCString	m_strDataDir;
    QCString	m_strConfDir;
};

inline LY_CommandUnitManageService &LY_CommandUnitManageService::Get()
{
    static LY_CommandUnitManageService sLY_CommandUnitManageService;
    return sLY_CommandUnitManageService;
}

#endif
