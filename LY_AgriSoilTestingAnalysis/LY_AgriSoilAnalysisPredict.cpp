#include <QPoint>
#include <QMouseEvent>
#include <map3d/IEarth.h>
#include <map3d/EarthEvent.h>
#include <osgEarth/MapNode>

#include "LY_AgriSoilAnalysisPredict.h"
#include "LY_AgriSoilAnalysisPredictInterface.h"

#include "IProj.h"
#include "IFunctionInterface.h"
#include "lyUI/LyDockManager.h"

#include "CommFunc.h"
#include "IAgriSoilAnalysisPredictActivator.h"

Q_DECLARE_METATYPE(qnzkna::framework::IRegistry *)

qnzkna::framework::IRegistry           *LY_AgriSoilAnalysisPredict::s_IRegistryImpl = nullptr;
qnzkna::AgriSoilTestingAnalysisService::IAgriSoilTestingAnalysisServiceImpl LY_AgriSoilAnalysisPredict::s_IAgriSoilTestingAnalysisServiceImpl;

LY_AgriSoilAnalysisPredict::LY_AgriSoilAnalysisPredict()
{
    IAgriSoilAnalysisPredictActivator::Get();
}

void LY_AgriSoilAnalysisPredict::Startup()
{
    IFunctionInterface &ifi = LY_AgriSoilAnalysisPredictInterface::Get().GetFunctionInterface();
    QVariant vt = ifi.Invoke("Ly.IRegistryImpl", 0);
    if (vt.canConvert<qnzkna::framework::IRegistry *>())
    {
        qnzkna::framework::IRegistry *pIRegistry = vt.value<qnzkna::framework::IRegistry *>();
        if (pIRegistry != nullptr)
        {
            IAgriSoilAnalysisPredictActivator::Get().start(pIRegistry);
			s_IRegistryImpl = pIRegistry;
        }
    }

	if (s_IRegistryImpl != nullptr)
	{
		s_IRegistryImpl->registerService(qnzkna::framework::ServiceReference("Ly.IAgriSoilTestingAnalysisService", &(s_IAgriSoilTestingAnalysisServiceImpl)));
	}

	struFunInfo getAreaFuncInfo;
	getAreaFuncInfo.strName = "Anzkna.AgriSoilAnalysisPredict.GetAreaID";
	getAreaFuncInfo.pFunction = new GetAreaIDFunc();
	ifi.Register(getAreaFuncInfo);

	struFunInfo addEventFuncInfo;
	addEventFuncInfo.strName = "Anzkna.AgriSoilAnalysisPredict.AddEvent";
	addEventFuncInfo.pFunction = new AddSaltFieldEventFunc();
	ifi.Register(addEventFuncInfo);
}

void LY_AgriSoilAnalysisPredict::Shutdown()
{
    IFunctionInterface &ifi = LY_AgriSoilAnalysisPredictInterface::Get().GetFunctionInterface();
    QVariant vt = ifi.Invoke("Ly.IRegistryImpl", 0);
    if (vt.canConvert<qnzkna::framework::IRegistry *>())
    {
        qnzkna::framework::IRegistry *pIRegistry = vt.value<qnzkna::framework::IRegistry *>();
        if (pIRegistry != nullptr)
        {
            IAgriSoilAnalysisPredictActivator::Get().stop(pIRegistry);
        }
    }

	if (s_IRegistryImpl != nullptr)
	{
		s_IRegistryImpl->unregisterService(qnzkna::framework::ServiceReference("Ly.IAgriSoilTestingAnalysisService", &(s_IAgriSoilTestingAnalysisServiceImpl)));
	}

	if (nullptr != m_pSaltFieldEventAnalysisWgt)
	{
		m_pSaltFieldEventAnalysisWgt = nullptr;
	}
}

void LY_AgriSoilAnalysisPredict::Reset()
{

}

QString LY_AgriSoilAnalysisPredict::GetAreaID(double lon, double lat)
{
	QString identity;
	if (m_pSaltFieldEventAnalysisWgt != NULL)
	{
		identity = m_pSaltFieldEventAnalysisWgt->GetAreaID(lon, lat);
	}
	return identity;
}

bool LY_AgriSoilAnalysisPredict::AddEvent(SaltFieldEventInfo& info)
{
	bool isOk = false;
	if (m_pSaltFieldEventAnalysisWgt != NULL)
	{
		info.sourceFieldID = GetAreaID(info.lon, info.lat);
		if (!info.sourceFieldID.isEmpty())
		{
			info.name = m_pSaltFieldEventAnalysisWgt->GenerateName(info);
			m_pSaltFieldEventAnalysisWgt->AddEvent(info);
			isOk = true;
		}
	}
	return isOk;
}

qnzkna::ZoneManage::CZoneBase LY_AgriSoilAnalysisPredict::GetActiveFieldZone()
{
	//if (m_pAgriPestAndDiseaseAnalysisWidget != nullptr)
	//{
	//	return m_pAgriPestAndDiseaseAnalysisWidget->GetActiveFieldZone();
	//}

	if (m_pSaltFieldEventAnalysisWgt != NULL)
	{
		m_pSaltFieldEventAnalysisWgt->GetActiveFieldZone();
	}
	return qnzkna::ZoneManage::CZoneBase();
}

bool LY_AgriSoilAnalysisPredict::DrawGraph(QPainter &painter, IProj *pProj)
{
    return false;
}

bool LY_AgriSoilAnalysisPredict::DrawDynamic(QPainter &painter, IProj *pProj)
{
	//if (m_pAgriPestAndDiseaseAnalysisWidget != nullptr)
	//{
	//	m_pAgriPestAndDiseaseAnalysisWidget->DrawDynamic(painter, pProj);
	//}

	if (m_pSaltFieldEventAnalysisWgt != NULL)
	{
		m_pSaltFieldEventAnalysisWgt->DrawDynamic(painter, pProj);
	}

    return true;
}

bool LY_AgriSoilAnalysisPredict::OnMouseButtonDown(QMouseEvent *e)
{
	return false;
}

bool LY_AgriSoilAnalysisPredict::OnMouseDoubleClick(QMouseEvent *e)
{
    return false;
}

bool LY_AgriSoilAnalysisPredict::OnCommand(int nID, void *lParam )
{
	switch (nID)
	{
	case 433392:
	{
		//if (m_pAgriPestAndDiseaseAnalysisWidget)
		//{
		//	if (m_pAgriPestAndDiseaseAnalysisWidget->isHidden())
		//	{
		//		m_pAgriPestAndDiseaseAnalysisWidget->show();
		//
		//		// Tree更新当前激活农场
		//		m_pAgriPestAndDiseaseAnalysisWidget->UpdataCurActiveFarm();
		//	}
		//}
		if (m_pSaltFieldEventAnalysisWgt)
		{
			if (m_pSaltFieldEventAnalysisWgt->isHidden())
			{
				m_pSaltFieldEventAnalysisWgt->show();

				// Tree更新当前激活农场
				m_pSaltFieldEventAnalysisWgt->UpdataCurActiveFarm();
			}
		}
	}
	break;
	default:
	{
		//if (m_pAgriPestAndDiseaseAnalysisWidget)
		//{
		//	m_pAgriPestAndDiseaseAnalysisWidget->hide();
		//}
		if (m_pSaltFieldEventAnalysisWgt)
		{
			m_pSaltFieldEventAnalysisWgt->hide();
		}
	}
		break;
	}
	return false;
}

bool LY_AgriSoilAnalysisPredict::OnCreate(int wParam, QWidget *pWgt)
{
	QSize screenSize = qApp->property("ly_screen_xy").toSize();
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
	case 1000:
	{
		// 初始化数据库数据
		m_pAgriDataAnalysisDBHandler = new AgriSoilAnalysisDBHandler(pWgt);

		//if (m_pAgriPestAndDiseaseAnalysisWidget == nullptr)
		//{
		//	// 初始化界面数据
		//	m_pAgriPestAndDiseaseAnalysisWidget = new AgriSoilTestingAnalysisWidget(pWgt);
		//
		//	m_pAgriPestAndDiseaseAnalysisWidget->hide();
		//	m_pAgriPestAndDiseaseAnalysisWidget->move(0, 60);
		//	m_pAgriPestAndDiseaseAnalysisWidget->resize(QSize(1920, 1020));
		//}

		if (nullptr == m_pSaltFieldEventAnalysisWgt)
		{
			// 初始化界面数据
			m_pSaltFieldEventAnalysisWgt = new SaltFieldEventAnalysisWgt(pWgt);

			m_pSaltFieldEventAnalysisWgt->hide();
			m_pSaltFieldEventAnalysisWgt->move(0, 60);
			m_pSaltFieldEventAnalysisWgt->resize(QSize(screenSize.width(), screenSize.height() - 60));
		}
		if (nullptr == m_pISaltDetailedInformation)
		{
			m_pISaltDetailedInformation = new ISaltDetailedInformation();
			m_pISaltDetailedInformation->resize(QSize(600, 600));
			m_pISaltDetailedInformation->hide();

		}
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

bool LY_AgriSoilAnalysisPredict::OnTimer(int wParam)
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

bool LY_AgriSoilAnalysisPredict::OnNetRecv(int nLength, char *lpData)
{
    return false;
}

void LY_AgriSoilAnalysisPredict::OnEarthMessage(int wParam, void *lParam)
{
	return;
}

bool LY_AgriSoilAnalysisPredict::On2d3dChanged(int wParam)
{
	//if (m_pAgriPestAndDiseaseAnalysisWidget != NULL)
	//{
	//	m_pAgriPestAndDiseaseAnalysisWidget->On2d3dChanged(wParam);
	//}
	if (m_pSaltFieldEventAnalysisWgt != NULL)
	{
		m_pSaltFieldEventAnalysisWgt->On2d3dChanged(wParam);
	}
	return false;
}

void LY_AgriSoilAnalysisPredict::SetScreenCenter(double dLon, double dLat)
{
    if(m_pProj){
        QString str = "CenterAt(" + QString::number(dLon,'f', 8) + "," + QString::number(dLat,'f', 8) + ")";
        LY_AgriSoilAnalysisPredictInterface::Get().Script(str.toLocal8Bit());
    }
}
