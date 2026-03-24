#ifndef LY_AERIALSHOTSETTINGBUNDLE54SERVICE_H
#define LY_AERIALSHOTSETTINGBUNDLE54SERVICE_H

#include <QMetaType>

#include "LY_AerialShotSettingBundle54_global.h"
#include "IAerialShotSettingBundle54Dialog.h"

#include <QCString.h>

class QPainter;
class QMouseEvent;
class QTabWidget;
class IProj;

class LY_AERIALSHOTSETTINGBUNDLE54SHARED_EXPORT LY_AerialShotSettingBundle54Service
{
public:
    LY_AerialShotSettingBundle54Service();
    inline static LY_AerialShotSettingBundle54Service &Get();

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
    bool OnNetRecv(int nLength, char *lpData);
    bool NetServerClientSend(const AMapPod54Control &aMapPod54Control, const QString &strSensorID);
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

    IAerialShotSettingBundle54Dialog *getPIAerialShotSettingBundle54Dialog() const;

#ifdef LY_AERIALSHOTSETTINGBUNDLE54_SERVER_CLINET_RECV_SEND
    bool getNetServerClient() const
    {
        return m_NetServerClient;
    }

private:
    bool                              m_NetServerClient;
    unsigned int                      m_SeatID;
#endif

    QWidget *getPWnd() const
    {
        return m_pWnd;
    }

private:
    IProj       *m_pProj;
    QCString	m_strDataDir;
    QCString	m_strConfDir;
    QWidget		*m_pWnd;

private:
    IAerialShotSettingBundle54Dialog   *m_pIAerialShotSettingBundle54Dialog;
};

inline LY_AerialShotSettingBundle54Service &LY_AerialShotSettingBundle54Service::Get()
{
    static LY_AerialShotSettingBundle54Service sLY_AerialShotSettingBundle54Service;
    return sLY_AerialShotSettingBundle54Service;
}

#endif
