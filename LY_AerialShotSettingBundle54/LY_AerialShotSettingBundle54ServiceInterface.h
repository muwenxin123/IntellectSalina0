#ifndef LY_AERIALSHOTSETTINGBUNDLE54SERVICEINTERFACE_H
#define LY_AERIALSHOTSETTINGBUNDLE54SERVICEINTERFACE_H

#include <IInterface.h>

class LY_AerialShotSettingBundle54ServiceInterface : public IInterface
{
public:
    static LY_AerialShotSettingBundle54ServiceInterface &Get();

    virtual void Startup();

    virtual void Shutdown();

    virtual void Reset();

    virtual bool OnNetRecv(int nLength, char *lpData);

    virtual bool DrawGraph(QPainter &painter, IProj *pProj);
    virtual bool DrawDynamic(QPainter &painter, IProj *pProj);
    virtual bool OnMessage(unsigned int uiMessage, int wParam, void *lParam);
    virtual bool OnMessageBus(unsigned int uiMessage, int wParam, void *lParam)
    {
        return false;
    }
    virtual bool OnServiceBus(unsigned int uiMessage, int wParam, void *lParam)
    {
        return false;
    }
};

#endif
