#ifndef ZoneManagerInterface_H
#define ZoneManagerInterface_H

#include <IInterface.h>

class QWidget;
class ZoneManagerInterface : public IInterface
{
public:

    static ZoneManagerInterface &Get();
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
