#ifndef LY_INTELLECTIMAGESHANGBO_H
#define LY_INTELLECTIMAGESHANGBO_H

#include "LY_IntellectImageShangBo_global.h"
#include <QCString.h>

class QPainter;
class QMouseEvent;
class QTabWidget;
class IProj;

class QListView;
class QTreeView;
class PopForm;
class QWidget;

class IntellectImageShangBoDialog;

class LY_INTELLECTIMAGESHANGBOSHARED_EXPORT LY_IntellectImageShangBo
{
public:
    LY_IntellectImageShangBo();
    inline static LY_IntellectImageShangBo &Get();

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
    inline void SetCoordinateTransformation(IProj *pProj){ m_pProj = pProj;};
    inline const QCString GetDataDir() const { return m_strDataDir; };
    inline const QCString GetConfDir() const { return m_strConfDir; };
    inline void SetDataDir(const char *lpszDir) { m_strDataDir = lpszDir; };
    inline void SetConfDir(const char *lpszDir) { m_strConfDir = lpszDir; };

public:

private:
    QListView	*m_pList;
    QTreeView	*m_pTree;
    PopForm		*m_pPopForm;
    IProj       *m_pProj;
    QWidget		*m_pWnd;
    QCString	m_strDataDir;
    QCString	m_strConfDir;

    IntellectImageShangBoDialog* m_pIntellectImageShangBoDialog;
};

inline LY_IntellectImageShangBo &LY_IntellectImageShangBo::Get()
{
    static LY_IntellectImageShangBo sLY_IntellectImageShangBo;
    return sLY_IntellectImageShangBo;
}

#endif
