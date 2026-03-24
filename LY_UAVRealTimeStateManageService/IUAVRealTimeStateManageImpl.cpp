#include "IUAVRealTimeStateManageImpl.h"

#include <time.h>

using namespace qnzkna::UAVRealTimeStateManage;

IUAVRealTimeStateManageImpl::IUAVRealTimeStateManageImpl()
    : m_mutex()
    , m_nTimerOut(0)
    , m_nTimerEclipse(0)
    , m_strCurFouseWeaponId("")
{
    QMutexLocker locker(&m_mutex);
    m_mapUAVRealTimeState.clear();
    m_mapRealTimeStateInterval.clear();
    locker.unlock();
}

IUAVRealTimeStateManageImpl::~IUAVRealTimeStateManageImpl()
{
    QMutexLocker locker(&m_mutex);
    m_mapUAVRealTimeState.clear();
    m_mapRealTimeStateInterval.clear();
    locker.unlock();
}

bool IUAVRealTimeStateManageImpl::IsUAVStateTimeOut(const std::string &strWeaponId)
{

    if (0 == m_nTimerEclipse)
    {
        return false;
    }

    QMutexLocker locker(&m_mutex);
    UAVRealTimeStateIntervalListItr itrMap = m_mapRealTimeStateInterval.find(strWeaponId);
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

void IUAVRealTimeStateManageImpl::SetUAVRealTimeState(const std::string &strWeaponId, const std::string &strProtocolType, const telemetryInformation &state)
{
    QMutexLocker locker(&m_mutex);
    NebulaUAVRealTimeStateMapItr itrMap = m_mapUAVRealTimeState.find(strWeaponId);
    if (itrMap != m_mapUAVRealTimeState.end())
    {
        itrMap->second.m_telemetryInfo.telemetryInfo = state;
        itrMap->second.m_telemetryInfo.isout         = false;
    }
    else
    {
        m_mapUAVRealTimeState.insert(std::make_pair(strWeaponId, TelemetryInformation(strProtocolType, {state, false})));
    }

    UAVRealTimeStateIntervalListItr itrMap_ = m_mapRealTimeStateInterval.find(strWeaponId);
    if (itrMap_ != m_mapRealTimeStateInterval.end())
    {
        itrMap_->second = ::time(nullptr);
    }
    else
    {
        m_mapRealTimeStateInterval.insert(std::make_pair(strWeaponId, ::time(nullptr)));
    }
}

void IUAVRealTimeStateManageImpl::RemoveUAVRealTimeState(const std::string &strWeaponId)
{
    QMutexLocker locker(&m_mutex);
    NebulaUAVRealTimeStateMapItr itrMap = m_mapUAVRealTimeState.find(strWeaponId);
    if (itrMap != m_mapUAVRealTimeState.end())
    {
        m_mapUAVRealTimeState.erase(itrMap);
    }

    UAVRealTimeStateIntervalListItr itrMap_ = m_mapRealTimeStateInterval.find(strWeaponId);
    if (itrMap_ != m_mapRealTimeStateInterval.end())
    {
        m_mapRealTimeStateInterval.erase(itrMap_);
    }
}

void IUAVRealTimeStateManageImpl::RemoveUAVRealTimeStateList()
{
    QMutexLocker locker(&m_mutex);
    m_mapUAVRealTimeState.clear();
    m_mapRealTimeStateInterval.clear();
}

void IUAVRealTimeStateManageImpl::OnTimerCallbackFun(unsigned int TimerID)
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
        UAVRealTimeStateIntervalListItr itrList = m_mapRealTimeStateInterval.begin();
        for (; itrList != m_mapRealTimeStateInterval.end();)
        {
            long long int nDeltaValue = (::time(nullptr) - (long)itrList->second);
            if (nDeltaValue >= LOCAL_TIMER_ELCASE)
            {
                std::string strWeaponId = itrList->first;
                itrList = m_mapRealTimeStateInterval.erase(itrList);
                NebulaUAVRealTimeStateMapItr itrState = m_mapUAVRealTimeState.find(strWeaponId);
                if (itrState != m_mapUAVRealTimeState.end())
                {

                    itrState->second.m_telemetryInfo.isout = true;

                }
            }
            else
            {
                itrList ++;
            }
        }
    }
}

void IUAVRealTimeStateManageImpl::SetCurFocusUAV(const std::string &strWeaponID)
{
    QMutexLocker locker(&m_mutex);
    m_strCurFouseWeaponId = strWeaponID;
}

std::string IUAVRealTimeStateManageImpl::GetCurFocusUAV()
{
    QMutexLocker locker(&m_mutex);
    if ("" == m_strCurFouseWeaponId)
    {
        NebulaUAVRealTimeStateMapItr itrMap = m_mapUAVRealTimeState.begin();
        if (itrMap != m_mapUAVRealTimeState.end())
        {
            m_strCurFouseWeaponId = itrMap->first;
        }
    }
    else
    {
        NebulaUAVRealTimeStateMapItr itrMap = m_mapUAVRealTimeState.find(m_strCurFouseWeaponId);
        if (itrMap == m_mapUAVRealTimeState.end())
        {
            m_strCurFouseWeaponId = "";
        }
    }
    return m_strCurFouseWeaponId;
}

void IUAVRealTimeStateManageImpl::SetUAVRealStateTimout(const unsigned long long int nMillSeconds)
{
    m_nTimerEclipse = nMillSeconds;
}

unsigned long long int IUAVRealTimeStateManageImpl::GetUAVRealStateTimout()
{
    return m_nTimerEclipse;
}

bool IUAVRealTimeStateManageImpl::GetDefaultUAVRealTimeState(std::string &strWeaponId, RealTimeTelemetryInformation_t &teleInfomation)
{
    QMutexLocker locker(&m_mutex);
    if ("" == m_strCurFouseWeaponId)
    {
        NebulaUAVRealTimeStateMapItr itrMap = m_mapUAVRealTimeState.begin();
        if (itrMap != m_mapUAVRealTimeState.end())
        {
            strWeaponId = itrMap->first;
            teleInfomation = itrMap->second.m_telemetryInfo;
            return true;
        }
    }

    NebulaUAVRealTimeStateMapItr itrMap = m_mapUAVRealTimeState.find(m_strCurFouseWeaponId);
    if (itrMap != m_mapUAVRealTimeState.end())
    {
        strWeaponId = m_strCurFouseWeaponId;
        teleInfomation = itrMap->second.m_telemetryInfo;
        return true;
    }
    return false;
}

bool IUAVRealTimeStateManageImpl::GetDefaultUAVRealTimeState(std::string &strWeaponId, std::string &strProtocolType, RealTimeTelemetryInformation_t &teleInfomation)
{
    QMutexLocker locker(&m_mutex);
    if ("" == m_strCurFouseWeaponId)
    {
        NebulaUAVRealTimeStateMapItr itrMap = m_mapUAVRealTimeState.begin();
        if (itrMap != m_mapUAVRealTimeState.end())
        {
            strWeaponId       = itrMap->first;
            strProtocolType   = itrMap->second.m_strProtocolType;
            teleInfomation    = itrMap->second.m_telemetryInfo;
            return true;
        }
    }

    NebulaUAVRealTimeStateMapItr itrMap = m_mapUAVRealTimeState.find(m_strCurFouseWeaponId);
    if (itrMap != m_mapUAVRealTimeState.end())
    {
        strWeaponId       = m_strCurFouseWeaponId;
        strProtocolType   = itrMap->second.m_strProtocolType;
        teleInfomation    = itrMap->second.m_telemetryInfo;
        return true;
    }
    return false;
}

bool IUAVRealTimeStateManageImpl::GetUAVRealTimeState(const std::string &strWeaponId, RealTimeTelemetryInformation_t &teleInfomation)
{
    if ("" == strWeaponId)
    {
        return false;
    }

    QMutexLocker locker(&m_mutex);
    NebulaUAVRealTimeStateMapItr itrMap = m_mapUAVRealTimeState.find(strWeaponId);
    if (itrMap != m_mapUAVRealTimeState.end())
    {
        teleInfomation = itrMap->second.m_telemetryInfo;
        return true;
    }
    return false;
}

bool IUAVRealTimeStateManageImpl::GetUAVRealTimeStateList(LPNebulaUAVRealTimeStateMap pStateList)
{
    if (!pStateList)
    {
        return false;
    }
    QMutexLocker locker(&m_mutex);
    *pStateList = m_mapUAVRealTimeState;
    return true;
}

void IUAVRealTimeStateManageImpl::SetUAVRealTimeStateFlightMode(const std::string& strWeaponId, const std::string& strflightMode)
{
	if ("" == strWeaponId)
	{
	}

	QMutexLocker locker(&m_mutex);
	NebulaUAVRealTimeStateMapItr itrMap = m_mapUAVRealTimeState.find(strWeaponId);
	if (itrMap != m_mapUAVRealTimeState.end())
	{
		itrMap->second.m_telemetryInfo.strflightMode = strflightMode;
	}
}
