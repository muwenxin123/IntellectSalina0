#include "IDockRealTimeStateManageImpl.h"

#include <time.h>

using namespace qnzkna::DockRealTimeStateManage;

IDockRealTimeStateManageImpl::IDockRealTimeStateManageImpl()
    : IDockRealTimeStateManageService()
    , m_mutex()
    , m_nTimerOut(0)
    , m_nTimerEclipse(0)
    , m_strCurFouseWeaponId("")
{
    QMutexLocker locker(&m_mutex);
    m_mapDockRealTimeState.clear();
    m_mapRealTimeStateInterval.clear();
    locker.unlock();
}

IDockRealTimeStateManageImpl::~IDockRealTimeStateManageImpl()
{
    QMutexLocker locker(&m_mutex);
    m_mapDockRealTimeState.clear();
    m_mapRealTimeStateInterval.clear();
    locker.unlock();
}

IDockRealTimeStateManageImpl &IDockRealTimeStateManageImpl::Get()
{
    static IDockRealTimeStateManageImpl dockRealTimeStateManageImpl;
    return dockRealTimeStateManageImpl;
}

bool IDockRealTimeStateManageImpl::IsDockStateTimeOut(const std::string &strDockID)
{
    // 默认永不超时
    if (0 == m_nTimerEclipse)
    {
        return false;
    }

    // 查找
    QMutexLocker locker(&m_mutex);
    DockRealTimeStateIntervalListItr itrMap = m_mapRealTimeStateInterval.find(strDockID);
    if (itrMap != m_mapRealTimeStateInterval.end())
    {
        unsigned long long int nCurInternal = ::time(nullptr) - itrMap->second;
        if (nCurInternal >= m_nTimerEclipse)
        {
            m_mapRealTimeStateInterval.erase(itrMap);
            return true;
        }
    }
    return true;
}

void IDockRealTimeStateManageImpl::SetDockRealTimeState(const std::string &strDockID, const std::string &strProtocolType, const DockTelemetryInformation &state)
{
    QMutexLocker locker(&m_mutex);
    NebulaDockRealTimeStateMapItr itrMap = m_mapDockRealTimeState.find(strDockID);
    if (itrMap != m_mapDockRealTimeState.end())
    {
        itrMap->second.telemetryInfo = state;
        itrMap->second.isout         = false;
    }
    else
    {
        m_mapDockRealTimeState.insert(std::make_pair(strDockID, DockRealTimeTelemetryInformation_t{state, false, "", ""}));
    }

    DockRealTimeStateIntervalListItr itrMap_ = m_mapRealTimeStateInterval.find(strDockID);
    if (itrMap_ != m_mapRealTimeStateInterval.end())
    {
        itrMap_->second = ::time(nullptr);
    }
    else
    {
        m_mapRealTimeStateInterval.insert(std::make_pair(strDockID, ::time(nullptr)));
    }
}

void IDockRealTimeStateManageImpl::RemoveDockRealTimeState(const std::string &strDockID)
{
    QMutexLocker locker(&m_mutex);
    NebulaDockRealTimeStateMapItr itrMap = m_mapDockRealTimeState.find(strDockID);
    if (itrMap != m_mapDockRealTimeState.end())
    {
        m_mapDockRealTimeState.erase(itrMap);
    }

    DockRealTimeStateIntervalListItr itrMap_ = m_mapRealTimeStateInterval.find(strDockID);
    if (itrMap_ != m_mapRealTimeStateInterval.end())
    {
        m_mapRealTimeStateInterval.erase(itrMap_);
    }
}

void IDockRealTimeStateManageImpl::RemoveDockRealTimeStateList()
{
    QMutexLocker locker(&m_mutex);
    m_mapDockRealTimeState.clear();
    m_mapRealTimeStateInterval.clear();
}

void IDockRealTimeStateManageImpl::OnTimerCallbackFun(unsigned int TimerID)
{
    m_nTimerOut += 1;
    if (m_nTimerOut <= 30)
    {
        return ;
    }
    m_nTimerOut = 0;

    QMutexLocker locker(&m_mutex);
    if (LOCAL_TIMER_ID == TimerID)
    {
        DockRealTimeStateIntervalListItr itrList = m_mapRealTimeStateInterval.begin();
        for (; itrList != m_mapRealTimeStateInterval.end();)
        {
            long long int nDeltaValue = (::time(nullptr) - (long)itrList->second);
            if (nDeltaValue >= LOCAL_TIMER_ELCASE)
            {
                std::string strDockID = itrList->first;
                itrList = m_mapRealTimeStateInterval.erase(itrList);
                NebulaDockRealTimeStateMapItr itrState = m_mapDockRealTimeState.find(strDockID);
                if (itrState != m_mapDockRealTimeState.end())
                {
                    // itrState->second.m_telemetryInfo._status = 1;
                    itrState->second.isout = true;
                    // m_mapDockRealTimeState.erase(itrState);
                }
            }
            else
            {
                itrList ++;
            }
        }
    }
}

void IDockRealTimeStateManageImpl::SetCurFocusDock(const std::string &strDockID)
{
    QMutexLocker locker(&m_mutex);
    m_strCurFouseWeaponId = strDockID;
}

std::string IDockRealTimeStateManageImpl::GetCurFocusDock()
{
    QMutexLocker locker(&m_mutex);
    if ("" == m_strCurFouseWeaponId)
    {
        NebulaDockRealTimeStateMapItr itrMap = m_mapDockRealTimeState.begin();
        if (itrMap != m_mapDockRealTimeState.end())
        {
            m_strCurFouseWeaponId = itrMap->first;
        }
    }
    else
    {
        NebulaDockRealTimeStateMapItr itrMap = m_mapDockRealTimeState.find(m_strCurFouseWeaponId);
        if (itrMap == m_mapDockRealTimeState.end())
        {
            m_strCurFouseWeaponId = "";
        }
    }
    return m_strCurFouseWeaponId;
}

void IDockRealTimeStateManageImpl::SetDockRealStateTimout(const unsigned long long int nMillSeconds)
{
    m_nTimerEclipse = nMillSeconds;
}

unsigned long long int IDockRealTimeStateManageImpl::GetDockRealStateTimout()
{
    return m_nTimerEclipse;
}

bool IDockRealTimeStateManageImpl::GetDefaultDockRealTimeState(std::string &strDockID, DockRealTimeTelemetryInformation_t &teleInfomation)
{
    QMutexLocker locker(&m_mutex);
    if ("" == m_strCurFouseWeaponId)
    {
        NebulaDockRealTimeStateMapItr itrMap = m_mapDockRealTimeState.begin();
        if (itrMap != m_mapDockRealTimeState.end())
        {
            strDockID = itrMap->first;
            teleInfomation = itrMap->second;
            return true;
        }
    }

    NebulaDockRealTimeStateMapItr itrMap = m_mapDockRealTimeState.find(m_strCurFouseWeaponId);
    if (itrMap != m_mapDockRealTimeState.end())
    {
        strDockID = m_strCurFouseWeaponId;
        teleInfomation = itrMap->second;
        return true;
    }
    return false;
}

bool IDockRealTimeStateManageImpl::GetDefaultDockRealTimeState(std::string &strDockID, std::string &strProtocolType, DockTelemetryInformation &teleInfomation)
{
    QMutexLocker locker(&m_mutex);
    if ("" == m_strCurFouseWeaponId)
    {
        NebulaDockRealTimeStateMapItr itrMap = m_mapDockRealTimeState.begin();
        if (itrMap != m_mapDockRealTimeState.end())
        {
            strDockID       = itrMap->first;
            strProtocolType   = itrMap->second.strProtocolType;
            teleInfomation    = itrMap->second.telemetryInfo;
            return true;
        }
    }

    NebulaDockRealTimeStateMapItr itrMap = m_mapDockRealTimeState.find(m_strCurFouseWeaponId);
    if (itrMap != m_mapDockRealTimeState.end())
    {
        strDockID       = m_strCurFouseWeaponId;
        strProtocolType   = itrMap->second.strProtocolType;
        teleInfomation    = itrMap->second.telemetryInfo;
        return true;
    }
    return false;
}

bool IDockRealTimeStateManageImpl::GetDockRealTimeState(const std::string &strDockID, DockRealTimeTelemetryInformation_t &teleInfomation)
{
    if ("" == strDockID)
    {
        return false;
    }

    QMutexLocker locker(&m_mutex);
    NebulaDockRealTimeStateMapItr itrMap = m_mapDockRealTimeState.find(strDockID);
    if (itrMap != m_mapDockRealTimeState.end())
    {
        teleInfomation = itrMap->second;
        return true;
    }
    return false;
}

bool IDockRealTimeStateManageImpl::GetDockRealTimeStateList(LPNebulaDockRealTimeStateMap pStateList)
{
    //if (!pStateList)
    //{
    //    return false;
    //}
    QMutexLocker locker(&m_mutex);
    *pStateList = m_mapDockRealTimeState;
    return true;
}

// 设置Dock模式
void IDockRealTimeStateManageImpl::SetDockRealTimeStateDockMode(const std::string &strDockID, const std::string &strDockMode)
{
    if ("" == strDockID)
    {
    }

    QMutexLocker locker(&m_mutex);
    NebulaDockRealTimeStateMapItr itrMap = m_mapDockRealTimeState.find(strDockID);
    if (itrMap != m_mapDockRealTimeState.end())
    {
        itrMap->second.strDockMode = strDockMode;
    }
}
