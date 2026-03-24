#ifndef LY_UAVFLYCONTROL_MAVLINK_H
#define LY_UAVFLYCONTROL_MAVLINK_H

#include <QMetaType>

#include "LY_UAVFlyControl_Mavlink_global.h"

#include <IProj.h>
#include <QCString.h>
typedef struct FireProt
{
	bool UpdateRoute = false;
	bool UpDownRroute = false;
}prot;

class QPainter;
class QMouseEvent;
class QTabWidget;
class IProj;
class LyWidget;
class LyDialog;

class UAVFlyManage_MavlinkMainDialog;

class UAVSelectDialog;
class UAVFlyManage_MavlinkDialog;
class UAVFlyManage_MavlinkTaskManageDialog;
class UAVFlyManage_MavlinkMessageIndicatorDialog;
class UAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialog;

class LY_UAVFLYCONTROLSHARED_MAVLINK_EXPORT LY_UAVFlyControl_Mavlink
{
public:
    LY_UAVFlyControl_Mavlink();
    inline static LY_UAVFlyControl_Mavlink &Get();

    void Startup();
    void Shutdown();
    void Reset();

    bool DrawGraph(QPainter &painter, IProj *pProj);
    bool DrawDynamic(QPainter &painter, IProj *pProj);
    bool OnMouseButtonDown(QMouseEvent *e);
    bool OnMouseButtonRelease(QMouseEvent *e);
    bool OnMouseDoubleClick(QMouseEvent *e);
    bool OnMouseMove(QMouseEvent *e);
    bool OnCommand(int nID, void *lParam);
    bool OnCreate(int wParam, QWidget *pTab);
    bool OnTimer(int wParam);
    bool OnNetRecv(int nLength, char *lpData);
    bool NetServerClientSend(const char * const msg, const unsigned int msglength, const QString &strComponentID);
	inline void SetFireProt(FireProt pProt)
	{
		m_fireProt = pProt;
	}
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

    void SetScreenCenter(const double &dLon, const double &dLat);

    QWidget *getPTab() const;

    const UAVSelectDialog *getPUAVSelectDialog() const;

    const UAVFlyManage_MavlinkDialog *getPUAVFlyManageDialog() const;

    UAVFlyManage_MavlinkMessageIndicatorDialog *getPUAVFlyManage_MavlinkMessageIndicatorDialog() const;
	UAVFlyManage_MavlinkTaskManageDialog*		getPUAVFlyManage_MavlinkTaskManageDialog() const;

	void setChangeUAVFlyManage_MavlinkTaskManageDialog(QString str);

private:
    IProj       *m_pProj;
    QCString	m_strDataDir;
    QCString	m_strConfDir;

	FireProt	m_fireProt;
private:
    QWidget*                                                    m_pTab;
    UAVFlyManage_MavlinkMainDialog*                             m_pUAVFlyManage_MavlinkMainDialog;

    UAVSelectDialog*                                            m_pUAVSelectDialog;
    UAVFlyManage_MavlinkDialog*                                 m_pUAVFlyManageDialog;
    LyDialog*                                                   m_pUAVFlyManage_MavlinkTaskManageLyWidgetDialog;
    UAVFlyManage_MavlinkTaskManageDialog*                       m_pUAVFlyManage_MavlinkTaskManageDialog;
    UAVFlyManage_MavlinkMessageIndicatorDialog*                 m_pUAVFlyManage_MavlinkMessageIndicatorDialog;
    LyWidget*                                                   m_pUAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialogLyWidgetDialog;
    UAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialog*    m_pUAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialog;


};

inline LY_UAVFlyControl_Mavlink &LY_UAVFlyControl_Mavlink::Get()
{
    static LY_UAVFlyControl_Mavlink sLY_UAVFlyControl_Mavlink;
    return sLY_UAVFlyControl_Mavlink;
}

#endif
