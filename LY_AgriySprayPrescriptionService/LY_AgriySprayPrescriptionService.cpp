#include "LY_AgriySprayPrescriptionService.h"
#include "LY_AgriySprayPrescriptionServiceInterface.h"
#include <IFunctionInterface.h>
#include "LY_AgriSprayPrescriptionActivator.h"


qnzkna::framework::IRegistry           *LY_AgriySprayPrescriptionService::s_IRegistryImpl = nullptr;
qnzkna::AgriSprayPrescription::ISprayPrescriptionImpl  LY_AgriySprayPrescriptionService::s_ISprayPrescriptionImpl;

LY_AgriySprayPrescriptionService::LY_AgriySprayPrescriptionService()
{
	LY_AgriSprayPrescriptionActivator::Get();
}

void LY_AgriySprayPrescriptionService::Startup()
{
    IFunctionInterface &ifi = LY_AgriySprayPrescriptionServiceInterface::Get().GetFunctionInterface();
    QVariant vt = ifi.Invoke("Ly.IRegistryImpl", 0);
    if (vt.canConvert<qnzkna::framework::IRegistry *>())
    {
        qnzkna::framework::IRegistry *pIRegistry = vt.value<qnzkna::framework::IRegistry *>();
        if (pIRegistry != nullptr)
        {
            s_IRegistryImpl = pIRegistry;
			LY_AgriSprayPrescriptionActivator::Get().start(pIRegistry);
        }
    }

    if (s_IRegistryImpl != nullptr)
    {
       s_IRegistryImpl->registerService(qnzkna::framework::ServiceReference("Ly.IAgriSprayPrescriptionService", &(s_ISprayPrescriptionImpl)));
    }
}

void LY_AgriySprayPrescriptionService::Shutdown()
{
	IFunctionInterface &ifi = LY_AgriySprayPrescriptionServiceInterface::Get().GetFunctionInterface();
	QVariant vt = ifi.Invoke("Ly.IRegistryImpl", 0);
	if (vt.canConvert<qnzkna::framework::IRegistry *>())
	{
		qnzkna::framework::IRegistry *pIRegistry = vt.value<qnzkna::framework::IRegistry *>();
		if (pIRegistry != nullptr)
		{
			LY_AgriSprayPrescriptionActivator::Get().stop(pIRegistry);
		}
	}
    if (s_IRegistryImpl != nullptr)
    {
        s_IRegistryImpl->unregisterService(qnzkna::framework::ServiceReference("Ly.IAgriSprayPrescriptionService", &(s_ISprayPrescriptionImpl)));
    }
}

void LY_AgriySprayPrescriptionService::Reset()
{

}

bool LY_AgriySprayPrescriptionService::DrawGraph(QPainter &painter, IProj *pProj)
{
    return false;
}

bool LY_AgriySprayPrescriptionService::DrawDynamic(QPainter &painter, IProj *pProj)
{
    return false;
}

bool LY_AgriySprayPrescriptionService::OnMouseButtonDown(QMouseEvent *e)
{
    bool bReturn = false;
    return bReturn;
}

bool LY_AgriySprayPrescriptionService::OnMouseDoubleClick(QMouseEvent *e)
{
    return false;
}

bool LY_AgriySprayPrescriptionService::OnCommand(int nID)
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

		if (m_AgriFertiliResultWidget)
		{
			m_AgriFertiliResultWidget->show();
		}

        break;
    }
    return false;
}

bool LY_AgriySprayPrescriptionService::OnCreate(int wParam, QWidget *pTab)
{
    switch (wParam)
    {
    case 0:
    {

    }
    break;
    case 1:
    {
		//p_DomainWidget = new LY_DomainWidget;

		//if (m_ShowVideoData == nullptr)
		//{
		//	m_ShowVideoData = new LY_ShowVideoData(pTab);
		//	m_ShowVideoData->hide();
		//}
		////任务信息窗口
		//if (m_ShowTaskDataInfo == nullptr)
		//{
		//	m_ShowTaskDataInfo = new LY_ShowTaskDataInfo(pTab);
		//	m_ShowTaskDataInfo->hide();
		//}
		////天气信息窗口
		//if (m_ShowWeatherDataInfo == nullptr)
		//{
		//	m_ShowWeatherDataInfo = new LY_ShowWeatherDataInfo(pTab);
		//	m_ShowWeatherDataInfo->hide();

		//}
		//if (m_commandWidget == nullptr)
		//{
		//	m_commandWidget = new LY_AgriSprayCommandWidget(pTab);
		//	m_commandWidget->hide();
		//	m_commandWidget->setFixedSize(QSize(500, 750));
		//	m_commandWidget->move(2500, -800);
		//}
		//if (m_AgriFertiliResultWidget == nullptr)
		//{
		//	m_AgriFertiliResultWidget = new LY_AgriFertiliResultWidget(pTab);
		//	m_AgriFertiliResultWidget->hide();
		//	m_AgriFertiliResultWidget->setFixedSize(QSize(500, 350));
		//	m_AgriFertiliResultWidget->move(2500, -500);
		//}
		//if (m_cameraWidget1 == nullptr)
		//{
		//	m_cameraWidget1 = new CameraWidget("1001");
		//	m_cameraWidget1->hide();
		//	m_cameraWidget1->setFixedSize(QSize(565, 315));
		//	m_cameraWidget1->move(3275, -2075);
		//}
		//if (m_cameraWidget2 == nullptr)
		//{
		//	m_cameraWidget2 = new CameraWidget("1002");
		//	m_cameraWidget2->hide();
		//	m_cameraWidget2->setFixedSize(QSize(285, 250));
		//	m_cameraWidget2->move(3275, -1745);
		//}
		//if (m_cameraWidget3 == nullptr)
		//{
		//	m_cameraWidget3 = new CameraWidget("1003");
		//	m_cameraWidget3->hide();
		//	m_cameraWidget3->setFixedSize(QSize(285, 250));
		//	m_cameraWidget3->move(3570, -1745);
		//}
	
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
    case 4:
        break;
    }
    return false;
}
LY_AgriSprayCommandWidget *LY_AgriySprayPrescriptionService::GetAgriSprayCommandWidget() 
{
	return m_commandWidget;
}
bool LY_AgriySprayPrescriptionService::OnTimer(int wParam)
{
    switch (wParam)
    {
    case 1:
	
		if (m_IsShowReightWidget)
		{
			//视频窗口
			if (m_ShowVideoData)
			{
				if (m_ShowVideoData->isHidden())
				{
					m_ShowVideoData->SetStyleSheetWidget();
					m_ShowVideoData->show();
				}
			}
			//任务窗口
			if (m_ShowTaskDataInfo)
			{
				if (m_ShowTaskDataInfo->isHidden())
				{
					m_ShowTaskDataInfo->SetStyleSheetWidget();
					m_ShowTaskDataInfo->show();
				}
			}
			//天气窗口
			if (m_ShowWeatherDataInfo)
			{
				if (m_ShowWeatherDataInfo->isHidden())
				{
					m_ShowWeatherDataInfo->SetStyleSheetWidget();
					m_ShowWeatherDataInfo->show();
				}
			}
		/*	if (m_cameraWidget1)
			{
				if (m_cameraWidget1->isHidden())
				{
					m_cameraWidget1->show();
					VideoInfoData info;
					info.url = "/flyLIne1.mp4";
					m_cameraWidget1->setVideoInfo(info);
					QTimer::singleShot(100, [&]() {

						m_cameraWidget1->pause(true);
					});

				}
			}
		if (m_cameraWidget2)
			{
				if (m_cameraWidget2->isHidden())
				{
					m_cameraWidget2->show();
					VideoInfoData info;
					info.url = "/flyLine4.mp4";
					m_cameraWidget2->setVideoInfo(info);

					QTimer::singleShot(100, [&]() {

						m_cameraWidget2->pause(true);
					});
				}
			}
			if (m_cameraWidget3)
			{
				if (m_cameraWidget3->isHidden())
				{
					m_cameraWidget3->show();
					VideoInfoData info;
					info.url = "/flyLine3.mp4";
					m_cameraWidget3->setVideoInfo(info);
				}
			}*/
			m_IsShowReightWidget = false;
		}
        break;
    case 2:
        break;
    case 3:
        break;
    }
    return false;
}

LY_ShowVideoData * LY_AgriySprayPrescriptionService::GetShowVideoData() const
{
	return m_ShowVideoData;
}

LY_ShowTaskDataInfo * LY_AgriySprayPrescriptionService::GetShowTaskDataInfo() const
{
	return m_ShowTaskDataInfo;
}

LY_ShowWeatherDataInfo * LY_AgriySprayPrescriptionService::GetShowWeatherDataInfo() const
{
	return m_ShowWeatherDataInfo;
}

LY_AgriFertiliResultWidget *LY_AgriySprayPrescriptionService::GetAgriFertiliResultWidget() const
{
	return m_AgriFertiliResultWidget;
}