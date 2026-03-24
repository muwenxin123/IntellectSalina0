#ifndef __REAL_PLAY_H__
#define __REAL_PLAY_H__

#include <string>

#include "GeneralDef.h"

class CRealPlay
{
public:
    CRealPlay(HWND hPlayWnd = nullptr);

public:
    bool Login(const std::string& csDeviceIp, const std::string& csUser, const std::string& csPWD, unsigned int nDevPor = 8000);
    void SetPlayWnd(HWND hPlayWnd);
    long GetPlayHandle();
    int  GetCurChanIndex();
    void DbPlayChannel(int ChanIndex);
    void StartPlay(int iChanIndex);
    void StopPlay();
    bool DoLogin();
    void DoGetDeviceResoureCfg();

public:
    bool m_bTrackRun;
    bool m_bIsOnCruise;

    bool m_bIsLogin;
    bool m_bIsPlaying;
    bool m_bIsRecording;
    LOCAL_DEVICE_INFO m_struDeviceInfo;
    int  m_iCurChanIndex;
    long m_lPlayHandle;

private:
    unsigned int m_nDevPort;
    std::string  m_DeviceIp;
    std::string  m_csUser;
    std::string  m_csPWD;
    HWND         m_hPlayWnd;

protected:
    void OnClose();
};

#endif
