#ifndef IPROTOCOLPARSEXYGSSERVERIMPL_XYHC_CONTROL_H
#define IPROTOCOLPARSEXYGSSERVERIMPL_XYHC_CONTROL_H

#include "IProtocolParseXygsServerImpl_Control.h"

namespace qnzkna {

namespace NetSeverClient {

class IProtocolParseXygsServerImpl_Xyhc_Control : public IProtocolParseXygsServerImpl_Control
{
public:
    virtual bool Pack_XygsServer(const qnzkna::framework::MessageReference* msg, qnzkna::framework::IRegistry* pContext) override;
    virtual bool UnPack_XygsServer(const qnzkna::framework::MessageReference* msg, qnzkna::framework::IRegistry* pContext) override;
    virtual bool NetHeader_XygsServer(const int nLength, const char * const pRecvBuf) override;
    virtual bool NetRecv_XygsServer(const int nLength, const char * const pRecvBuf) override;

private:
    bool                                               m_bRecordHeartbeats;
    bool                                               m_bControlCommandRecord;
    bool                                               m_bTelemetryLogRecord;
    static CLogRecordCVS                               s_LogCVS;
    static CLogRecordCVSCMD                            s_LogCVSCMDUp;
    static CLogRecordCVSCMD                            s_LogCVSCMDDown;

private:
    IProtocolParseXygsServerImpl_Xyhc_Control();
    virtual ~IProtocolParseXygsServerImpl_Xyhc_Control() override;
    IProtocolParseXygsServerImpl_Xyhc_Control(const IProtocolParseXygsServerImpl_Xyhc_Control&) = delete;
    IProtocolParseXygsServerImpl_Xyhc_Control(const IProtocolParseXygsServerImpl_Xyhc_Control&&) = delete;
    IProtocolParseXygsServerImpl_Xyhc_Control& operator=(const IProtocolParseXygsServerImpl_Xyhc_Control&) = delete;
    IProtocolParseXygsServerImpl_Xyhc_Control& operator=(const IProtocolParseXygsServerImpl_Xyhc_Control&&) = delete;

    template<class classname>
    friend class IProtocolParseXygsServerImpl_Control_Handle;
};

}

}

#endif
