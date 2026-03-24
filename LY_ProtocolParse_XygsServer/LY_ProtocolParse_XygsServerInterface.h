#ifndef LY_PROTOCOLPARSE_XYGSSERVERINTERFACE_H
#define LY_PROTOCOLPARSE_XYGSSERVERINTERFACE_H

#include <IInterface.h>

class LY_ProtocolParse_XygsServerInterface : public IInterface
{
public:

    static LY_ProtocolParse_XygsServerInterface& Get();

    virtual void Startup();

    virtual void Shutdown();

    virtual void Reset();

    virtual bool OnNetRecv(int nLength, char *lpData);

    virtual bool DrawGraph(QPainter &painter, IProj *pProj);
    virtual bool DrawDynamic(QPainter &painter, IProj *pProj);
    virtual bool OnMessage(unsigned int uiMessage, int wParam, void *lParam);
    virtual bool OnMessageBus(unsigned int , int , void * )
    {
        return false;
    }
    virtual bool OnServiceBus(unsigned int , int , void * )
    {
        return false;
    }
};

#endif
