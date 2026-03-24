#ifndef IUAVREALTIMESTATEMANAGEIMPL_H
#define IUAVREALTIMESTATEMANAGEIMPL_H

#include <string>

#include <QMutex>
#include <QXmlStreamReader>

#include "DataManage/IUAVRealTimeStateManageService.h"

namespace qnzkna
{
namespace UAVRealTimeStateManage
{

#define LOCAL_TIMER_ID      1
#define LOCAL_TIMER_ELCASE  30

class IUAVRealTimeStateManageImpl: public IUAVRealTimeStateManageService
{
public:
    typedef std::map<std::string, unsigned long long int>   UAVRealTimeStateIntervalList;
    typedef UAVRealTimeStateIntervalList::iterator          UAVRealTimeStateIntervalListItr;
    typedef UAVRealTimeStateIntervalList::const_iterator    UAVRealTimeStateIntervalListConstItr;

public:
    IUAVRealTimeStateManageImpl();

    virtual ~IUAVRealTimeStateManageImpl();

    void OnTimerCallbackFun(unsigned int TimerID = LOCAL_TIMER_ID);
    void SetUAVRealTimeState(const std::string &strWeaponId, const std::string &strProtocolType, const telemetryInformation &state);
    bool IsUAVStateTimeOut(const std::string &strWeaponId);
    void RemoveUAVRealTimeState(const std::string &strWeaponId);
    void RemoveUAVRealTimeStateList();

    virtual void SetCurFocusUAV(const std::string &strWeaponID);

    virtual std::string GetCurFocusUAV();

    virtual void SetUAVRealStateTimout(const unsigned long long int nMillSeconds);

    virtual unsigned long long int GetUAVRealStateTimout();

    virtual bool GetDefaultUAVRealTimeState(std::string &strWeaponId, RealTimeTelemetryInformation_t &teleInfomation);
    virtual bool GetDefaultUAVRealTimeState(std::string &strWeaponId, std::string &strProtocolType, RealTimeTelemetryInformation_t &teleInfomation);

    virtual bool GetUAVRealTimeState(const std::string &strWeaponId, RealTimeTelemetryInformation_t &teleInfomation);

    virtual bool GetUAVRealTimeStateList(LPNebulaUAVRealTimeStateMap pStateList);
	void SetUAVRealTimeStateFlightMode(const std::string& strWeaponId, const std::string& strflightMode);

private:
    mutable QMutex                  m_mutex;
    unsigned long long int          m_nTimerOut;
    unsigned long long int          m_nTimerEclipse;
    std::string                     m_strCurFouseWeaponId;
    NebulaUAVRealTimeStateMap       m_mapUAVRealTimeState;
    UAVRealTimeStateIntervalList    m_mapRealTimeStateInterval;
};

}
}

#endif
