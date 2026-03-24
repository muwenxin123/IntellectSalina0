#ifndef LYUCISPLUGINFRAMEWORK_H
#define LYUCISPLUGINFRAMEWORK_H

#include "lyUcisPluginFrameWork_global.h"
#include <QCString.h>

#include <QMetaType>
#include "IRegistryImpl.h"

Q_DECLARE_METATYPE(qnzkna::framework::IRegistry *)

class QPainter;
class QMouseEvent;
class QTabWidget;
class IProj;

class LYUCISPLUGINFRAMEWORKSHARED_EXPORT LyUcisPluginFrameWork
{
public:
    LyUcisPluginFrameWork();
    inline static LyUcisPluginFrameWork &Get();

    void Startup();
    void Shutdown();
    void Reset();

    bool DrawGraph(QPainter &painter, IProj *pProj);
    bool DrawDynamic(QPainter &painter, IProj *pProj);
    bool OnMouseButtonDown(QMouseEvent *e);
    bool OnMouseDoubleClick(QMouseEvent *e);
    bool OnCommand(int nID);
    bool OnCreate(int wParam, QWidget *pTab);
    bool OnTimer(int wParam);
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
};

inline LyUcisPluginFrameWork &LyUcisPluginFrameWork::Get()
{
    static LyUcisPluginFrameWork sLyUcisPluginFrameWork;
    return sLyUcisPluginFrameWork;
}

#endif
