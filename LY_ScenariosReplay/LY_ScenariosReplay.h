#ifndef LY_SCENARIOSREPLAY_H
#define LY_SCENARIOSREPLAY_H

#include "lY_ScenariosReplay_global.h"
#include <QCString.h>

class QPainter;
class QMouseEvent;
class QTabWidget;
class IProj;

class QListView;
class QTreeView;
class QWidget;

class ScenarioReplayDialog;
class IScenariosReplayThread;

class LY_SCENARIOSREPLAYSHARED_EXPORT LY_ScenariosReplay
{
public:
    LY_ScenariosReplay();
    inline static LY_ScenariosReplay &Get();

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
	ScenarioReplayDialog * m_pScenarioReplayDialog;
	IScenariosReplayThread* m_pIScenariosReplayThread;
	//LyDialog                     *m_pScenarioReplayWidgetDialog;

	int m_progressBarValue = 0;

private:
    QListView	*m_pList;
    QTreeView	*m_pTree;
    IProj       *m_pProj;
    QWidget		*m_pWnd;
    QCString	m_strDataDir;
    QCString	m_strConfDir;

};

inline LY_ScenariosReplay &LY_ScenariosReplay::Get()
{
    static LY_ScenariosReplay sLY_ScenariosReplay;
    return sLY_ScenariosReplay;
}

#endif
