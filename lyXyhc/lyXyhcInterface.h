#ifndef LYXYHCInterface_H
#define LYXYHCInterface_H

#include <IInterface.h>

class LyXyhcInterface : public IInterface
{
public:

    static LyXyhcInterface &Get();

    virtual void Startup();

    virtual void Shutdown();

    virtual void Reset();

    virtual bool DrawGraph(QPainter &painter, IProj *pProj);
    virtual bool DrawDynamic(QPainter &painter, IProj *pProj);

    virtual bool OnMessage(unsigned int uiMessage, int wParam, void *lParam);
    virtual bool OnMessageBus(unsigned int uiMessage, int wParam, void *lParam)
    {
        return false;
    };
    virtual bool OnNetRecv(int nLength, char *lpData);
    virtual bool OnServiceBus(unsigned int uiMessage, int wParam, void *lParam)
    {
        return false;
    };
};

#endif
