
#ifndef LY_SENSORVEDIOSERVICE_H
#define LY_SENSORVEDIOSERVICE_H

#include "LY_SensorVedioService_global.h"
#include <QCString.h>

class QPainter;
class QMouseEvent;
class QTabWidget;
class IProj;

class QListView;
class QTreeView;
class PopForm;
class QWidget;
class SensorVedioTimer;
class SensorSocketSender;
class SensorSocketReceiver;

class LY_SENSORVEDIOSERVICE_EXPORT LY_SensorVedioService
{
public:
    LY_SensorVedioService();
    inline static LY_SensorVedioService&Get();

	void Startup();
	void Shutdown();

    bool DrawGraph(QPainter &painter, IProj *pProj);
    bool DrawDynamic(QPainter &painter, IProj *pProj);
    bool OnMouseButtonDown(QMouseEvent *e);
    bool OnMouseDoubleClick(QMouseEvent *e);
	bool OnMouseButtonUp(QMouseEvent *e);
    bool OnCommand(int nID);
    bool OnCreate(int wParam, QWidget *pWidget);
    bool OnTimer(int wParam);
    inline void SetCoordinateTransformation(IProj *pProj)
    {
        m_pProj = pProj;
    };
    inline const QCString GetDataDir() const { return m_strDataDir; };
    inline const QCString GetConfDir() const { return m_strConfDir; };
    inline void SetDataDir(const char *lpszDir)
    {
        m_strDataDir = lpszDir;
    };
    inline void SetConfDir(const char *lpszDir)
    {
        m_strConfDir = lpszDir;
    };

	bool OnNetRecv(int nLength, char *lpData);
	
	QString GetUUID();

	inline  SensorSocketSender* GetSender() { return m_pSender; };
	inline  SensorSocketReceiver* GetReceiver() { return m_pReceiver; };

public:

private:
	int m_timeOutCnt = 0;
    IProj       *m_pProj;
    QWidget		*m_pWnd;
    QCString	m_strDataDir;
    QCString	m_strConfDir;
	QString m_uuID;
	SensorVedioTimer* m_pSenderTimer = nullptr;
	SensorSocketSender* m_pSender = nullptr;
	SensorSocketReceiver* m_pReceiver = nullptr;
};

inline LY_SensorVedioService &LY_SensorVedioService::Get()
{
    static LY_SensorVedioService s_LY_SensorVedioService;
    return s_LY_SensorVedioService;
}


#endif // LY_SensorVedioService_H
