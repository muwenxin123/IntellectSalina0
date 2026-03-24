#include "LY_AgricultureHomepageService.h"
#include "LY_AgricultureHomepageServiceInterface.h"
#include <IFunctionInterface.h>
#include <QDateTime>

qnzkna::framework::IRegistry           *LY_AgricultureHomepageService::s_IRegistryImpl = nullptr;
 qnzkna::IAgricultureHomePage::IAgricultureHomepageImpl   LY_AgricultureHomepageService::s_IAgricultureHomepageImpl;


LY_AgricultureHomepageService::LY_AgricultureHomepageService()
{
}

void LY_AgricultureHomepageService::Startup()
{
    IFunctionInterface &ifi = LY_AgricultureHomepageServiceInterface::Get().GetFunctionInterface();
    QVariant vt = ifi.Invoke("Ly.IRegistryImpl", 0);
    if (vt.canConvert<qnzkna::framework::IRegistry *>())
    {
        qnzkna::framework::IRegistry *pIRegistry = vt.value<qnzkna::framework::IRegistry *>();
        if (pIRegistry != nullptr)
        {
            s_IRegistryImpl = pIRegistry;
        }
    }

    if (s_IRegistryImpl != nullptr)
    {
       s_IRegistryImpl->registerService(qnzkna::framework::ServiceReference("Ly.IAgricultureHomePageService", &(s_IAgricultureHomepageImpl)));
    }
}

void LY_AgricultureHomepageService::Shutdown()
{
    if (s_IRegistryImpl != nullptr)
    {
        s_IRegistryImpl->unregisterService(qnzkna::framework::ServiceReference("LY.IAgricultureHomePageService", &(s_IAgricultureHomepageImpl)));
    }
}

void LY_AgricultureHomepageService::Reset()
{

}

bool LY_AgricultureHomepageService::DrawGraph(QPainter &painter, IProj *pProj)
{
    return false;
}

bool LY_AgricultureHomepageService::DrawDynamic(QPainter &painter, IProj *pProj)
{
    return false;
}

bool LY_AgricultureHomepageService::OnMouseButtonDown(QMouseEvent *e)
{
    bool bReturn = false;
    return bReturn;
}

bool LY_AgricultureHomepageService::OnMouseDoubleClick(QMouseEvent *e)
{
    return false;
}

bool LY_AgricultureHomepageService::OnCommand(int nID)
{
    switch (nID)
    {
    case 43385:
    {

    }
    break;
    case 44777:
    {

    }
    break;
    case 44778:
    {

    }
    break;
    default:
        break;
    }
    return false;
}

bool LY_AgricultureHomepageService::OnCreate(int wParam, QTabWidget *pTab)
{
    switch (wParam)
    {
    case 0:
    {

    }
    break;
    case 1:
    {

    }
    break;
    case 2:
    {

    }
    break;
    case 3:
    {

    }
    break;
	case 1000:
	{
		s_IAgricultureHomepageImpl.OnCreate();
	}
	break;
    }
    return false;
}

bool LY_AgricultureHomepageService::OnTimer(int wParam)
{
    switch (wParam)
    {
    case 1:
        break;
    case 2:
        break;
    case 3:
        break;
    }
    return false;
}

void LY_AgricultureHomepageService::OnEarthMessage(int wParam, void *lParam)
{
	static bool isShow = false;

	pos3d *p3d = (pos3d *)lParam;
	switch (wParam)
	{
	case IEarthEventHandler::Earth_LDOUBLECLICK:
	case IEarthEventHandler::Earth_MOUSE_MOVE:
	case IEarthEventHandler::Earth_TRACK_CONTENT:
	case IEarthEventHandler::Earth_LBUTTONDOWN:
	break;
	case IEarthEventHandler::Earth_VIEWPORT_CHANGED:
	{
		qint64 now = QDateTime::currentMSecsSinceEpoch();
		if (now - m_lastChangedTime < 1000) {
			// 距离上次点击不到1秒，忽略
			return;
		}
		m_lastChangedTime = now;

		char* pD = (char*)LY_AgricultureHomepageServiceInterface::Get().Script("GetViewPort()");
		// 经度 维度 1000 方向 俯仰 视点高度
		QStringList list = QString(pD).split(',');
		if (6 == list.size()){
			double viewHeight = list[5].toDouble();
			if (0 < viewHeight && /*300.0*/ 700.0 >= viewHeight){
				if (isShow){
					s_IAgricultureHomepageImpl.showSensor(false);
					s_IAgricultureHomepageImpl.showSensorModel(true);
					isShow = false;
				}
			}else{
				if (!isShow){
					s_IAgricultureHomepageImpl.showSensor(true);
					s_IAgricultureHomepageImpl.showSensorModel(false);
					isShow = true;
				}
			}
		}
	}
	break;
	}
}
