#include "RealPlay.h"

#if __DEBUG__LOGINFO__
#define logInfo(format, ...)
#define logInfoDebug() /##/
#else

#include <QDebug>

#define logInfo(format, ...)
#define logInfoDebug() qDebug()

#endif

#ifdef _DEBUG
#pragma comment(lib, "HCNetSDK.lib")
#pragma comment(lib, "PlayCtrl.lib")
#else
#pragma comment(lib, "HCNetSDK.lib")
#pragma comment(lib, "PlayCtrl.lib")
#endif

void CALLBACK DrawFunZoomInInside(LONG lRealHandle, HDC hDc, DWORD dwUser)
{
    if (nullptr != nullptr) {

    }
}

CRealPlay::CRealPlay(HWND hPlayWnd)
: m_hPlayWnd(hPlayWnd)
{
    m_DeviceIp = "192.168.1.2";
    m_nDevPort = 8000;
    m_csUser   = ("admin");
    m_csPWD    = ("12345");
    m_bIsLogin = false;

    m_bIsPlaying    = false;
    m_bIsRecording  = false;
    m_iCurChanIndex = -1;
    m_lPlayHandle   = -1;
    m_bIsOnCruise   = false;
    m_bTrackRun     = false;
}

bool CRealPlay::Login(const std::string& csDeviceIp, const std::string& csUser, const std::string& csPWD, unsigned int nDevPor)
{
    m_DeviceIp = csDeviceIp;
    m_nDevPort = nDevPor;
    m_csUser   = csUser;
    m_csPWD    = csPWD;

    m_struDeviceInfo.lLoginID = -1;
    m_bIsLogin = false;

    logInfoDebug() << __FILE__ << __LINE__ << csUser.c_str() << csPWD.c_str() << nDevPor << NET_DVR_GetLastError();
    if(!m_bIsLogin)
    {
        logInfoDebug() << __FILE__ << __LINE__;
        if(!DoLogin())
            return false;
        DoGetDeviceResoureCfg();
        m_bIsLogin = TRUE;
    }
    else
    {
        logInfoDebug() << __FILE__ << __LINE__ << NET_DVR_GetLastError();
        if(m_bIsPlaying|| m_bIsRecording)
        {
            return false;
        }
        NET_DVR_Logout_V30(m_struDeviceInfo.lLoginID);
        m_struDeviceInfo.lLoginID = -1;
        m_bIsLogin = false;
    }

    logInfoDebug() << __FILE__ << __LINE__ << m_struDeviceInfo.lLoginID << NET_DVR_GetLastError();

    return m_bIsLogin;
}

bool CRealPlay::DoLogin()
{
    NET_DVR_DEVICEINFO_V30 DeviceInfoTmp;
    memset(&DeviceInfoTmp,0,sizeof(NET_DVR_DEVICEINFO_V30));

    logInfoDebug() << __FILE__ << __LINE__ << m_DeviceIp.c_str() << m_csUser.c_str() << m_csPWD.c_str() << m_nDevPort << NET_DVR_GetLastError();

    char DeviceIp[256]= {};
    strcpy(DeviceIp, m_DeviceIp.c_str());
    char csUser[256]= {};
    strcpy(csUser, m_csUser.c_str());
    char csPWD[256]= {};
    strcpy(csPWD, m_csPWD.c_str());
    WORD DevPort = m_nDevPort;

    LONG lLoginID = NET_DVR_Login_V30(DeviceIp, DevPort, csUser, csPWD, &DeviceInfoTmp);

    if(lLoginID == -1){
        unsigned long pErrorNo = NET_DVR_GetLastError();
        long errorNo = pErrorNo;
        logInfoDebug() << __FILE__ << __LINE__ << m_DeviceIp.c_str() << m_csUser.c_str() << m_csPWD.c_str() << m_nDevPort << pErrorNo << NET_DVR_GetErrorMsg(&errorNo);
        return false;
    }

    logInfoDebug() << __FILE__ << __LINE__ << m_DeviceIp.c_str() << m_csUser.c_str() << m_csPWD.c_str() << m_nDevPort << NET_DVR_GetLastError();
    m_struDeviceInfo.lLoginID       = lLoginID;
    m_struDeviceInfo.iDeviceChanNum = DeviceInfoTmp.byChanNum;
    m_struDeviceInfo.iIPChanNum     = DeviceInfoTmp.byIPChanNum;
    m_struDeviceInfo.iStartChan     = DeviceInfoTmp.byStartChan;
    m_struDeviceInfo.iIPStartChan   = DeviceInfoTmp.byStartDChan;
    return true;
}

void CRealPlay::DoGetDeviceResoureCfg()
{
    NET_DVR_IPPARACFG_V40 IpAccessCfg;
    memset(&IpAccessCfg,0,sizeof(IpAccessCfg));
    DWORD  dwReturned;

    m_struDeviceInfo.bIPRet = NET_DVR_GetDVRConfig(m_struDeviceInfo.lLoginID,NET_DVR_GET_IPPARACFG_V40,0,&IpAccessCfg,sizeof(NET_DVR_IPPARACFG_V40),&dwReturned);

    int i;
    if(!m_struDeviceInfo.bIPRet)
    {
        for(i=0; i < MAX_ANALOG_CHANNUM; i++)
        {
            if (i < m_struDeviceInfo.iDeviceChanNum)
            {
                sprintf(m_struDeviceInfo.struChanInfo[i].chChanName,"camera%d",i+m_struDeviceInfo.iStartChan);
                m_struDeviceInfo.struChanInfo[i].iChanIndex=i+m_struDeviceInfo.iStartChan;
                m_struDeviceInfo.struChanInfo[i].bEnable = TRUE;

            }
            else
            {
                m_struDeviceInfo.struChanInfo[i].iChanIndex = -1;
                m_struDeviceInfo.struChanInfo[i].bEnable = FALSE;
                sprintf(m_struDeviceInfo.struChanInfo[i].chChanName, "");
            }
        }
    }
    else
    {
        for(i=0; i < MAX_ANALOG_CHANNUM; i++)
        {
            if (i < m_struDeviceInfo.iDeviceChanNum)
            {
                sprintf(m_struDeviceInfo.struChanInfo[i].chChanName,"camera%d",i+m_struDeviceInfo.iStartChan);
                m_struDeviceInfo.struChanInfo[i].iChanIndex=i+m_struDeviceInfo.iStartChan;
                if(IpAccessCfg.byAnalogChanEnable[i])
                {
                    m_struDeviceInfo.struChanInfo[i].bEnable = TRUE;
                }
                else
                {
                    m_struDeviceInfo.struChanInfo[i].bEnable = FALSE;
                }

            }
            else
            {
                m_struDeviceInfo.struChanInfo[i].iChanIndex = -1;
                m_struDeviceInfo.struChanInfo[i].bEnable = FALSE;
                sprintf(m_struDeviceInfo.struChanInfo[i].chChanName, "");
            }
        }

        for(i=0; i < MAX_IP_CHANNEL; i++)
        {
            if(IpAccessCfg.struStreamMode[i].uGetStream.struChanInfo.byEnable)
            {
                m_struDeviceInfo.struChanInfo[i+MAX_ANALOG_CHANNUM].bEnable = TRUE;
                m_struDeviceInfo.struChanInfo[i+MAX_ANALOG_CHANNUM].iChanIndex = i+IpAccessCfg.dwStartDChan;
                sprintf(m_struDeviceInfo.struChanInfo[i+MAX_ANALOG_CHANNUM].chChanName,"IP Camera %d",i+1);
            }
            else
            {
                m_struDeviceInfo.struChanInfo[i+MAX_ANALOG_CHANNUM].bEnable = FALSE;
                m_struDeviceInfo.struChanInfo[i+MAX_ANALOG_CHANNUM].iChanIndex = -1;
            }
        }

    }

}

void CRealPlay::DbPlayChannel(int ChanIndex)
{

    ChanIndex = ChanIndex + 33;
    logInfoDebug() << __FILE__ << __LINE__ << ChanIndex;
    if(!m_bIsPlaying)
    {
        StartPlay(ChanIndex);
    }
    else
    {
        StopPlay();
        StartPlay(ChanIndex);

    }
}

void CRealPlay::StartPlay(int iChanIndex)
{
    if (m_hPlayWnd == nullptr) {
        logInfoDebug() << __FILE__ << __LINE__;
        return;
    }
    m_iCurChanIndex = iChanIndex;
    NET_DVR_CLIENTINFO ClientInfo;
    ClientInfo.hPlayWnd     = m_hPlayWnd;
    ClientInfo.lChannel     = m_iCurChanIndex;
    ClientInfo.lLinkMode    = 0x00000001;
    ClientInfo.sMultiCastIP = nullptr;
    ClientInfo.byProtoType  = 0;

    m_lPlayHandle = NET_DVR_RealPlay_V30(m_struDeviceInfo.lLoginID,&ClientInfo,nullptr,nullptr,TRUE);
    if(-1 == m_lPlayHandle)
    {
        DWORD err = NET_DVR_GetLastError();

        logInfoDebug() << __FILE__ << __LINE__ << err;
        return;
    }

    m_bIsPlaying = TRUE;

    if (m_lPlayHandle >= 0)
    {
        logInfoDebug() << __FILE__ << __LINE__ << m_lPlayHandle;
        NET_DVR_RigisterDrawFun(m_lPlayHandle, DrawFunZoomInInside, 0);
    }
}

void CRealPlay::StopPlay()
{
    if(m_lPlayHandle != -1)
    {
        NET_DVR_StopRealPlay(m_lPlayHandle);
        m_lPlayHandle=-1;
        m_bIsPlaying = FALSE;
    }
}

int CRealPlay::GetCurChanIndex()
{
    return m_iCurChanIndex;
}

void CRealPlay::SetPlayWnd(HWND hPlayWnd) {
    m_hPlayWnd = hPlayWnd;
}

long CRealPlay::GetPlayHandle()
{
    return m_lPlayHandle;
}

void CRealPlay::OnClose()
{

    if (m_bIsLogin)
    {
        StopPlay();
        NET_DVR_Logout_V30(m_struDeviceInfo.lLoginID);
    }
}
