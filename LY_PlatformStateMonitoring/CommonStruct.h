#pragma once

#include <QMutex>
#include <vector>
#include <map>
#include "DataManage/NebulaTask/NebulaRoutePoint.h"

typedef enum _EeditStates
{
    PickNormal = 0,
    PickSinglePoint,
    PickLinePoint
} EeditStates;

typedef enum _InvestigationSetting
{
    Investigat_Lock = 0,
    Investigat_Scan,
    Investigat_Follow,
    Investigat_Infrared,
} InvestigationSetting;

typedef enum _FirepowerSetting
{
    Firepower_Lock = 0,
    Firepower_Unlock,
    Firepower_Fire,
} FirepowerSetting;

typedef struct _WndUpdatePointInfo
{
    unsigned long	m_uPointID;
    unsigned long	m_uLineID;
    std::string		m_strWeaponName;
    std::string		m_strWeaponID;
    _WndUpdatePointInfo(const std::string &strWeaponID, const std::string &strWeaponName, unsigned long uLineID, unsigned long uPointID)
    {
        m_uPointID = uPointID;
        m_uLineID = uLineID;
        m_strWeaponName = strWeaponName;
        m_strWeaponID = strWeaponID;
    }

} WndUpdatePointInfo;

typedef struct _WndRoutePoint
{
    bool					m_bSelected;
    unsigned long			nPointID;
    double					dLocalDistance;
    CNebulaRoutePoint		stNebulaRoutePoint;
    InvestigationSetting	mInvestigationSetting;
    FirepowerSetting		mFirepowerSetting;
    _WndRoutePoint()
    {
        nPointID = -1;
        m_bSelected = false;
        dLocalDistance = 0;
        mInvestigationSetting = Investigat_Lock;
        mFirepowerSetting = Firepower_Lock;
    }

} WndRoutePoint, *LPWndRoutePoint;

typedef struct _WndRouteLineInfo
{
    bool			bFormState;
    bool			bLineShowStates;
    bool			bLiseEditStates;
    int				nMissionLineID;
    unsigned int    nColor;
    unsigned long	nID;
    unsigned long	nLineID;

    std::string		strID;
    std::string		strWeaponID;
    std::string		strWeaponName;
    std::string		strMarkID;
    std::string		strStartTime;
    std::vector<WndRoutePoint> vecNebulaRoutePoint;
    _WndRouteLineInfo()
    {
        bFormState      = 0;
        nID             = 0 ;
        nLineID         = 0;
        bLineShowStates = true;
        bLiseEditStates = false;
        nColor          = 0xFFFFFFFF;
        strID           = "";
        strWeaponID     = "";
        strWeaponName   = "";
        strMarkID		= "";
        nMissionLineID	= 0 ;
        strStartTime	= "";
        vecNebulaRoutePoint.clear();

    }

} WndRouteLineInfo, *LPWndRouteLineInfo;

typedef std::vector<WndRouteLineInfo *>	WndRouteLineInfoList, *LPWndRouteLineInfoList;
typedef WndRouteLineInfoList::iterator WndRouteLineInfoListItr;

typedef struct _STWndRouteLineInfoList
{
public:
    QMutex sMutex;
    WndRouteLineInfoList	m_vecRoutePointInfo;
    _STWndRouteLineInfoList() {	}
    ~_STWndRouteLineInfoList() { }
    void lock()
    {
        sMutex.lock();
    }

    void unlock()
    {
        sMutex.unlock();
    }

} STWndRouteLineInfoList;

