#ifndef LY_COMMANDUNITMANAGESERVICEInterface_H
#define LY_COMMANDUNITMANAGESERVICEInterface_H

#include <IInterface.h>

class LY_CommandUnitManageServiceInterface : public IInterface
{
public:

    static LY_CommandUnitManageServiceInterface &Get();
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
};

#endif
