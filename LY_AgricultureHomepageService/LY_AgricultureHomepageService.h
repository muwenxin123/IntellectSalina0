#ifndef LY_AgricultureHomepageService_H
#define LY_AgricultureHomepageService_H

#include <QMetaType>
#include "ServiceFramework/IRegistry.h"
#include "ServiceFramework/IServiceInterfaceTracker.h"
#include "IAgricultureHomepageImpl.h"
#include "DataManage/AgriSevice/IAgricultureHomePageService.h"
#include "LY_AgricultureHomepageService_global.h"
#include <QCString.h>

class QPainter;
class QMouseEvent;
class QTabWidget;
class IProj;

Q_DECLARE_METATYPE(qnzkna::framework::IRegistry *)

class LY_AGRICULTUREHOMEPAGESHARED_EXPORT LY_AgricultureHomepageService
{
public:
    LY_AgricultureHomepageService();
    inline static LY_AgricultureHomepageService &Get();

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
	void OnEarthMessage(int wParam, void *lParam);

    inline void SetCoordinateTransformation(IProj *pProj)
    {
        m_pProj = pProj;
    }
    inline const QCString GetDataDir() const
    {
        return m_strDataDir;
    }
    inline const QCString GetConfDir() const
    {
        return m_strConfDir;
    }
    inline void SetDataDir(const char *lpszDir)
    {
        m_strDataDir = lpszDir;
    }
    inline void SetConfDir(const char *lpszDir)
    {
        m_strConfDir = lpszDir;
    }

private:
    IProj       *m_pProj;
    QCString	m_strDataDir;
    QCString	m_strConfDir;
	qint64		m_lastChangedTime = 0;

private:
    static qnzkna::framework::IRegistry             *s_IRegistryImpl;
    static qnzkna::IAgricultureHomePage::IAgricultureHomepageImpl   s_IAgricultureHomepageImpl;
};

inline LY_AgricultureHomepageService &LY_AgricultureHomepageService::Get()
{
    static LY_AgricultureHomepageService sLY_AgricultureHomepageService;
    return sLY_AgricultureHomepageService;
}

#endif
