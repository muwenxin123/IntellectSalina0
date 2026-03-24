#ifndef LY_AGRILAYERCONTROLINTERFACE_H_
#define LY_AGRILAYERCONTROLINTERFACE_H_
 
#include <IInterface.h>

class LY_AgriLayerControlInterface : public IInterface
{
public:

	static LY_AgriLayerControlInterface& Get();

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
#endif  // LY_AgriLayerControlInterface_H