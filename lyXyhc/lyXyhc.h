#ifndef LYXYHC_H
#define LYXYHC_H

#include "lyXyhc_global.h"
#include <QCString.h>
#include <QString>
#include "TrackMgr.h"
#include "UAVNavigationArmShotAutoAiming.h"
#include "UAVNavigationArmShotUpdateCalBombLine.h"
#include <QPoint>
#include <vector>
#include "Synergy/CDlgPlannedSynergy.h"
#include "Synergy/CDlgZoneSynergy.h"
#include "Synergy/CDlgTimeSynergy.h"
#include "Synergy/CDlgInfoSynergy.h"
#include "Synergy/CDlgScanSynergy01.h"
#include "Synergy/CDlgScanSynergy02.h"
#include "Synergy/CDlgFireSynergy01.h"
#include "Synergy/CDlgFireSynergy02.h"
#include <QObject>

#include "CDlgSystemMessageDistribute.h"

class QPainter;
class QMouseEvent;
class QTabWidget;
class IProj;
class Cmd;
class QTrack;
class QListWidget;
class QTreeView;
class PopForm;
class QWidget;
class QDb;
class CDlgSystemMessageDistribute;

class UAVCurrentShotAiming;

class LYXYHCSHARED_EXPORT LyXyhc
{
public:
    LyXyhc();
    inline static LyXyhc &Get();

    void Startup();
    void Shutdown();
    void Reset();

    bool DrawGraph(QPainter &painter, IProj *pProj);
    bool DrawDynamic(QPainter &painter, IProj *pProj);
    bool OnMouseButtonDown(QMouseEvent *e);
    bool OnMouseButtonUp(QMouseEvent *e);
    bool OnMouseMove(QMouseEvent *e);
    bool OnMouseDoubleClick(QMouseEvent *e);
    bool OnCommand(int nID);
    bool OnCreate(int wParam, QWidget *pWidget);
    bool OnTimer(int wParam);
    bool OnNetRecv(int nLength, char *lpData);
    inline void SetCoordinateTransformation(IProj *pProj)
    {
        m_pProj = pProj;
        m_TrackMgr.setPProj(m_pProj);
    }
    inline IProj *GetProj() const
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
    void OnEarthMessage(int wParam, void *lParam);
    void ContextMenu(const QPoint &pt);
    void UpdateContextMenuMark();
    CDlgPlannedSynergy *getDlgPlannedSynergy();

    CDlgZoneSynergy *getPDlgZoneSynergy() const;

    CDlgTimeSynergy *getPDlgTimeSynergy() const;

    CDlgSystemMessageDistribute *getPDlgSystemMessageDistribute() const;

public:
    typedef enum __MapMenuCommandOper
    {
        MapMenuCommandOper_MicrosMapMenuSwitch                  = 0,
        MapMenuCommandOper_DotLineTrack                         = 1,
        MapMenuCommandOper_GrowthTrack                          = 2,
        MapMenuCommandOper_ShortTrack                           = 3,
        MapMenuCommandOper_FiveDotTrack                         = 4,
        MapMenuCommandOper_FullTrack_NoFullTrack                = 5,
        MapMenuCommandOper_FullTrack_WithoutTime                = 6,
        MapMenuCommandOper_FullTrack_WithHalfTime               = 7,
        MapMenuCommandOper_FullTrack_WithFullTime               = 8,
        MapMenuCommandOper_TrackColor                           = 9,

        MapMenuCommandOper_FullTrack_ScoutView                  = 65535,
        MapMenuCommandOper_FullTrack_BallisticLine              = 65536,
        MapMenuCommandOper_FullTrack_BallisticInformation       = 65537,
        MapMenuCommandOper_FullTrack_ClearTrackLine             = 65538,

        MapMenuCommandOper_WarnInfo_WarnInfoTime                = 65539,
        MapMenuCommandOper_WarnInfo_WarnInfoThreat              = 65540,
        MapMenuCommandOper_WarnInfo_WarnInfoObstacleDist        = 65541,
        MapMenuCommandOper_WarnInfo_WarnInfoRemainEnergy        = 65542,
        MapMenuCommandOper_WarnInfo_WarnInfoRadio               = 65543,

        MapMenuCommandOper_AmmoRemain_AmmoRemain                = 65544,
        MapMenuCommandOper_LoadType_LoadType                    = 65545,

        MapMenuCommandOper_Report_Report						= 65546,

        MapMenuCommandOper_Synergy_PlannedSynergy               = 65549,
        MapMenuCommandOper_Synergy_ZoneSynergy              = 65550,
        MapMenuCommandOper_Synergy_TimeSynergy                  = 65551,
        MapMenuCommandOper_Synergy_InformationSupportSynergy    = 65552,

        MapMenuCommandOper_Synergy_ScanSynergy                  = 65553,
        MapMenuCommandOper_Synergy_LeadTransitionSynergy        = 65554,
        MapMenuCommandOper_Synergy_TrackingMarkSynergy        = 65555,
        MapMenuCommandOper_Synergy_GuidanceControlInfoSynergy   = 65556,
        MapMenuCommandOper_Synergy_Disapper		    = 65557,

    } MapMenuCommandOper;

    CTrackMgr &getTrackMgr()
    {
        return m_TrackMgr;
    }
    CTrackMgr *getTrackMgrPtr()
    {
        return &m_TrackMgr;
    }

    UAVNavigationArmShotAutoAiming *getPUAVNavigationArmShotAutoAiming() const
    {
        return m_pUAVNavigationArmShotAutoAiming;
    }

    UAVCurrentShotAiming *getPUAVCurrentShotAiming() const
    {
        return m_pUAVCurrentShotAiming;
    }

    CDlgInfoSynergy *getPDlgInfoSynergy() const;

    CDlgScanSynergy01 *getPDlgScanSynergy01() const;

    CDlgScanSynergy02 *getPDlgScanSynergy02() const;

    CDlgFireSynergy01 *getPDlgFireSynergy01() const;

    CDlgFireSynergy02 *getPDlgFireSynergy02() const;

    bool ExecSql(const QString &strSql);
    bool GetDb(QDb &db);
    const QString &GetPilotName(int nID) const;

public:
    QStringList m_slAttributeTitle;
    std::vector<QStringList> m_vslAttributeItem;
    std::vector<int> m_viPilot;
    std::vector<int>   m_DisapperID;

private:
    Cmd			                                *m_pCmd;
    IProj                                       *m_pProj;
    QWidget		                                *m_pWnd;
    QTabWidget		                            *m_pLayerControl;
    QTrack                                      *m_pTrack;
    QCString		                            m_strDbName;
    QCString		                            m_strDbIP;
    QCString		                            m_strDbUser;
    QCString		                            m_strDbPass;
    QCString		                            m_strDbDriver;
    QCString	                                m_strDataDir;
    QCString	                                m_strConfDir;
    Qt::MouseButton                             m_MouseButton;
    QPoint                                      m_ptPre;
    CTrackMgr	                                m_TrackMgr;
    UAVNavigationArmShotAutoAiming				*m_pUAVNavigationArmShotAutoAiming;
    UAVNavigationArmShotUpdateCalBombLine		*m_pUAVNavigationArmShotUpdateCalBombLine;
    CDlgPlannedSynergy							*m_pDlgPlannedSynergy;
    CDlgZoneSynergy						*m_pDlgZoneSynergy;
    CDlgTimeSynergy							*m_pDlgTimeSynergy;
    CDlgInfoSynergy							*m_pDlgInfoSynergy;
    CDlgScanSynergy01							*m_pDlgScanSynergy01;
    CDlgScanSynergy02							*m_pDlgScanSynergy02;
    CDlgFireSynergy01							*m_pDlgFireSynergy01;
    CDlgFireSynergy02							*m_pDlgFireSynergy02;
    UAVCurrentShotAiming						*m_pUAVCurrentShotAiming;
    CDlgSystemMessageDistribute				*m_pDlgSystemMessageDistribute;

};

inline LyXyhc &LyXyhc::Get()
{
    static LyXyhc sLyXyhc;
    return sLyXyhc;
}

#endif
