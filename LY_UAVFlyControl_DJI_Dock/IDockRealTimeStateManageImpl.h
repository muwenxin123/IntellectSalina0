#ifndef IDOCKREALTIMESTATEMANAGEIMPL_H
#define IDOCKREALTIMESTATEMANAGEIMPL_H

#include <string>

#include <QMutex>

#include "DataManage/IDockRealTimeStateManageService.h"

namespace qnzkna
{
namespace DockRealTimeStateManage
{

#define LOCAL_TIMER_ID      1        // 本地定时器ID
#define LOCAL_TIMER_ELCASE  30       // 10000 // 本地定时器间隔

class IDockRealTimeStateManageImpl : public IDockRealTimeStateManageService
{
public:
    typedef std::map<std::string, unsigned long long int>    DockRealTimeStateIntervalList;
    typedef DockRealTimeStateIntervalList::iterator          DockRealTimeStateIntervalListItr;
    typedef DockRealTimeStateIntervalList::const_iterator    DockRealTimeStateIntervalListConstItr;

public:
    // IDockRealTimeStateManageImpl();
    // virtual ~IDockRealTimeStateManageImpl();

    static IDockRealTimeStateManageImpl &Get();

    void OnTimerCallbackFun(unsigned int TimerID = LOCAL_TIMER_ID);
    void SetDockRealTimeState(const std::string &strDockID, const std::string &strProtocolType, const DockTelemetryInformation &state);
    bool IsDockStateTimeOut(const std::string &strDockID);
    void RemoveDockRealTimeState(const std::string &strDockID);
    void RemoveDockRealTimeStateList();


    // 设置当前关注的DockID
    virtual void SetCurFocusDock(const std::string &strDockID) override;

    // 获取当前关注DockID
    virtual std::string GetCurFocusDock() override;

    // 设置Dock状态超时时间(单位：ms 负值为永远不超时)
    virtual void SetDockRealStateTimout(const unsigned long long int nMillSeconds) override;

    // 获取Dock超时时间(ms 负值为永不超时)
    virtual unsigned long long int GetDockRealStateTimout() override;

    // 获取状态列表中第一个状态数据
    virtual bool GetDefaultDockRealTimeState(std::string &strDockID, DockRealTimeTelemetryInformation_t &teleInfomation) override;
    virtual bool GetDefaultDockRealTimeState(std::string &strDockID, std::string &strProtocolType, DockTelemetryInformation &teleInfomation) override;

    // 获取Dock实时状态
    virtual bool GetDockRealTimeState(const std::string &strDockID, DockRealTimeTelemetryInformation_t &teleInfomation) override;

    // 获取Dock所有的实时状态
    virtual bool GetDockRealTimeStateList(LPNebulaDockRealTimeStateMap pStateList) override;
    void SetDockRealTimeStateDockMode(const std::string &strDockID, const std::string &strDockMode) override;

private:
    IDockRealTimeStateManageImpl();
    virtual ~IDockRealTimeStateManageImpl();

private:
    IDockRealTimeStateManageImpl(const IDockRealTimeStateManageImpl &) = delete;
    IDockRealTimeStateManageImpl(const IDockRealTimeStateManageImpl &&) = delete;
    IDockRealTimeStateManageImpl &operator=(const IDockRealTimeStateManageImpl &) = delete;
    IDockRealTimeStateManageImpl &operator=(const IDockRealTimeStateManageImpl &&) = delete;

private:
    mutable QMutex                   m_mutex;                     // 临界区对象
    unsigned long long int           m_nTimerOut;                 // 超时时间
    unsigned long long int           m_nTimerEclipse;             // 超时时间间隔
    std::string                      m_strCurFouseWeaponId;       // 当前关注点的装备
    NebulaDockRealTimeStateMap       m_mapDockRealTimeState;      // Dock实时状态
    DockRealTimeStateIntervalList    m_mapRealTimeStateInterval;  // 实时状态间隔时间
};

}
}

#endif // IDOCKREALTIMESTATEMANAGEIMPL_H
