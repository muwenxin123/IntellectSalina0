#ifndef LY_UAVNAVIGATIONARMSHOTALONE54_H
#define LY_UAVNAVIGATIONARMSHOTALONE54_H

#include <QMetaType>

#include "LY_UAVNavigationArmShotAlone54_global.h"
#include "IUAVNavigationArmShotAlone54Dialog.h"
#include <QCString.h>

#include "Protocol/XygsZC/Bag_XygsZCDefines.h"
#include <QTimer>
#include <QMap>
#include <QString>

class QPainter;
class QMouseEvent;
class QTabWidget;
class IProj;
class IUAVNavigationArmShotAlone54Dialog;
class IUAVNavigationArmShotAlone54_ALL_Dialog;
class IUAVNavigationArmShotAlone54_95_1_StatusSync;
class IUAVTargetTree;
class IUAVSuspendWgt;
class UAVArmShotAloneListDialog;

class LY_UAVNAVIGATIONARMSHOTALONE54SHARED_EXPORT LY_UAVNavigationArmShotAlone54
{
public:
    LY_UAVNavigationArmShotAlone54();
    inline static LY_UAVNavigationArmShotAlone54 &Get();

    void Startup();
    void Shutdown();
    void Reset();

    bool DrawGraph(QPainter &painter, IProj *pProj);
    bool DrawDynamic(QPainter &painter, IProj *pProj);
    bool OnMouseButtonDown(QMouseEvent *e);
    bool OnMouseDoubleClick(QMouseEvent *e);
    bool OnCommand(int nID, void *lParam);
    bool OnCreate(int wParam, QWidget *pTab);
    bool OnTimer(int wParam);
    bool OnNetRecv(int nLength, char *lpData);
    inline void SetCoordinateTransformation(IProj *pProj)
    {
        m_pProj = pProj;
    }

    inline IProj * GetCoordinateTransformation()
    {
        return m_pProj;
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

    const std::list<IUAVNavigationArmShotAlone54Dialog *> *getPIUAVNavigationArmShotAlone54Dialog() const;

    bool getUseUdpSocket_95_1_StatusSync() const;

    IUAVNavigationArmShotAlone54_95_1_StatusSync *getPIUAVNavigationArmShotAlone54_95_1_StatusSync() const;

    int sendMessage_95_1_StatusSync_control(const char * const pbuf, const int pbufSize);

    QWidget *getPWnd() const
    {
        return m_pWnd;
    }

    IUAVTargetTree *GetUAVTargetTree()
    {
        return m_IUAVTargetTree;
    }

	void onFocalZoomChanged(int mFocal);
public:
	//½¹¾à
	int currFocal = 1;
private:
    IProj       *m_pProj;
    QCString    m_strDataDir;
    QCString    m_strConfDir;
    QWidget*    m_p2DView;

    IUAVTargetTree *m_IUAVTargetTree = nullptr;

public:
    std::map<int, QVector<XygsZC_Mark_01>>           m_XygsZc_TargetMap;
    std::map<QString, QVector<QString>>                m_TargetBleadMap;

private:
    std::list<IUAVNavigationArmShotAlone54Dialog *>    m_pIUAVNavigationArmShotAlone54DialogList;
    IUAVNavigationArmShotAlone54_ALL_Dialog*           m_pIUAVNavigationArmShotAlone54_ALL_Dialog;
    IUAVNavigationArmShotAlone54_95_1_StatusSync      *m_pIUAVNavigationArmShotAlone54_95_1_StatusSync;
	UAVArmShotAloneListDialog*						   m_pIUAVArmSHotAloneListDialog;
    bool                                               m_UseUdpSocket_95_1_StatusSync;
    QWidget                                           *m_pWnd = nullptr;
};

inline LY_UAVNavigationArmShotAlone54 &LY_UAVNavigationArmShotAlone54::Get()
{
    static LY_UAVNavigationArmShotAlone54 sLY_UAVNavigationArmShotAlone54;
    return sLY_UAVNavigationArmShotAlone54;
}

#endif
