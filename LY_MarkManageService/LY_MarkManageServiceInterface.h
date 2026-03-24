#ifndef LY_MARKMANAGESERVICEInterface_H
#define LY_MARKMANAGESERVICEInterface_H

#include <IInterface.h>
#include "Protocol/XygsTelegraph/TelegrpahHead.h"
#include "netheader.h"

class LY_MarkManageServiceInterface : public IInterface
{
public:

    static LY_MarkManageServiceInterface &Get();

    virtual void Startup();

    virtual void Shutdown();

    virtual void Reset();

    virtual bool DrawGraph(QPainter &painter, IProj *pProj);
    virtual bool DrawDynamic(QPainter &painter, IProj *pProj);

    virtual bool OnMessage(unsigned int uiMessage, int wParam, void *lParam);
    virtual bool OnMessageBus(unsigned int uiMessage, int wParam, void *lParam)
    {
        return true;
    };
    virtual bool OnServiceBus(unsigned int uiMessage, int wParam, void *lParam)
    {
        return true;
    };
    virtual bool OnNetRecv(int nLength, char *lpData);
};

#endif
