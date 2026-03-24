#ifndef LY_SYSTEMMAINTENANCESERVICEInterface_H
#define LY_SYSTEMMAINTENANCESERVICEInterface_H

#include <IInterface.h>

class LY_SystemMaintenanceServiceInterface : public IInterface
{
public:

    static LY_SystemMaintenanceServiceInterface &Get();

    virtual bool DrawGraph(QPainter &painter, IProj *pProj);
    virtual bool DrawDynamic(QPainter &painter, IProj *pProj);
    virtual void Startup();
    virtual void Shutdown();
    virtual void Reset();
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
