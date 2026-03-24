#include <QPoint>
#include <QMouseEvent>
#include <map3d/IEarth.h>
#include <map3d/EarthEvent.h>
#include <osgEarth/MapNode>

#include "LY_AgriDataAnalysisPredict.h"
#include "LY_AgriDataAnalysisPredictInterface.h"

#include "IProj.h"
#include "IFunctionInterface.h"
#include "lyUI/LyDockManager.h"

#include "IAgriDataAnalysisPredictActivator.h"

Q_DECLARE_METATYPE(qnzkna::framework::IRegistry *)

qnzkna::framework::IRegistry           *LY_AgriDataAnalysisPredict::s_IRegistryImpl = nullptr;
qnzkna::AgriDataAnalysisPredictService::IAgriDataAnalysisPredictServiceImpl LY_AgriDataAnalysisPredict::s_IAgriDataAnalysisPredictServiceImpl;

LY_AgriDataAnalysisPredict::LY_AgriDataAnalysisPredict()
{
    IAgriDataAnalysisPredictActivator::Get();
}

void LY_AgriDataAnalysisPredict::Startup()
{
    IFunctionInterface &ifi = LY_AgriDataAnalysisPredictInterface::Get().GetFunctionInterface();
    QVariant vt = ifi.Invoke("Ly.IRegistryImpl", 0);
    if (vt.canConvert<qnzkna::framework::IRegistry *>())
    {
        qnzkna::framework::IRegistry *pIRegistry = vt.value<qnzkna::framework::IRegistry *>();
        if (pIRegistry != nullptr)
        {
            IAgriDataAnalysisPredictActivator::Get().start(pIRegistry);
			s_IRegistryImpl = pIRegistry;
        }
    }

	if (s_IRegistryImpl != nullptr)
	{
		s_IRegistryImpl->registerService(qnzkna::framework::ServiceReference("Ly.IAgriDataAnalysisPredictService", &(s_IAgriDataAnalysisPredictServiceImpl)));
	}
}

void LY_AgriDataAnalysisPredict::Shutdown()
{
    IFunctionInterface &ifi = LY_AgriDataAnalysisPredictInterface::Get().GetFunctionInterface();
    QVariant vt = ifi.Invoke("Ly.IRegistryImpl", 0);
    if (vt.canConvert<qnzkna::framework::IRegistry *>())
    {
        qnzkna::framework::IRegistry *pIRegistry = vt.value<qnzkna::framework::IRegistry *>();
        if (pIRegistry != nullptr)
        {
            IAgriDataAnalysisPredictActivator::Get().stop(pIRegistry);
        }
    }

	if (s_IRegistryImpl != nullptr)
	{
		s_IRegistryImpl->unregisterService(qnzkna::framework::ServiceReference("Ly.IAgriDataAnalysisPredictService", &(s_IAgriDataAnalysisPredictServiceImpl)));
	}
}

void LY_AgriDataAnalysisPredict::Reset()
{

}

qnzkna::ZoneManage::CZoneBase LY_AgriDataAnalysisPredict::GetActiveFieldZone()
{
	if (m_pAgriPestAndDiseaseAnalysisWidget != nullptr)
	{
		return m_pAgriPestAndDiseaseAnalysisWidget->GetActiveFieldZone();
	}
	return qnzkna::ZoneManage::CZoneBase();
}

void LY_AgriDataAnalysisPredict::ReCreateDBHandler()
{
	// 重建数据库数据（请求线程使用）
	if (m_pAgriDataAnalysisDBHandler)
	{
		delete m_pAgriDataAnalysisDBHandler;
		m_pAgriDataAnalysisDBHandler = nullptr;
	}

	m_pAgriDataAnalysisDBHandler = new AgriDataAnalysisDBHandler();
}

bool LY_AgriDataAnalysisPredict::DrawGraph(QPainter &painter, IProj *pProj)
{
    return false;
}

bool LY_AgriDataAnalysisPredict::DrawDynamic(QPainter &painter, IProj *pProj)
{
	if (m_pAgriPestAndDiseaseAnalysisWidget != nullptr)
	{
		m_pAgriPestAndDiseaseAnalysisWidget->DrawDynamic(painter, pProj);
	}

    return true;
}

bool LY_AgriDataAnalysisPredict::OnMouseButtonDown(QMouseEvent *e)
{
	bool bReturn = false;

	QPoint pt = e->pos();

	if (e->button() == Qt::LeftButton && m_pAgriPestAndDiseaseAnalysisWidget)
	{
		double lon = 0.0, lat = 0.0;

		m_pProj->Screen2Jwd(pt, lon, lat);

		updatePickTargetPoint(lon, lat, 0);
	}

	return bReturn;
}

bool LY_AgriDataAnalysisPredict::OnMouseDoubleClick(QMouseEvent *e)
{
    return false;
}

bool LY_AgriDataAnalysisPredict::OnCommand(int nID, void *lParam )
{
	switch (nID)
	{
	case 433385:
	{
		if (m_pAgriPestAndDiseaseAnalysisWidget)
		{
			if (m_pAgriPestAndDiseaseAnalysisWidget->isHidden())
			{
				m_pAgriPestAndDiseaseAnalysisWidget->show();

				// Tree更新当前激活农场
				m_pAgriPestAndDiseaseAnalysisWidget->UpdataCurActiveFarm();
			}
		}
	}
	break;
	default:
	{
		if (m_pAgriPestAndDiseaseAnalysisWidget)
		{
			m_pAgriPestAndDiseaseAnalysisWidget->hide();
		}
	}
		break;
	}
	return false;
}

bool LY_AgriDataAnalysisPredict::OnCreate(int wParam, QWidget *pWgt)
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
	case 1000:
	{
		// 初始化数据库数据
		m_pAgriDataAnalysisDBHandler = new AgriDataAnalysisDBHandler(pWgt);

		if (m_pAgriPestAndDiseaseAnalysisWidget == nullptr)
		{
			// 初始化界面数据
			m_pAgriPestAndDiseaseAnalysisWidget = new AgriPestAndDiseaseAnalysisWidget(pWgt);

			m_pAgriPestAndDiseaseAnalysisWidget->hide();
			m_pAgriPestAndDiseaseAnalysisWidget->move(0, 60);
			m_pAgriPestAndDiseaseAnalysisWidget->resize(QSize(1920, 1020));
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

bool LY_AgriDataAnalysisPredict::OnTimer(int wParam)
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

bool LY_AgriDataAnalysisPredict::OnNetRecv(int nLength, char *lpData)
{
    return false;
}

void LY_AgriDataAnalysisPredict::OnEarthMessage(int wParam, void *lParam)
{
	if (lParam == nullptr) return;

	if (wParam == IEarthEventHandler::Earth_LBUTTONDOWN)
	{
		if (pos3d *p3d = (pos3d *)lParam)
		{
			double dHeight = 0.0;
			if (const auto& iEarth = IEarth::GetEarth())
			{
				if (osgEarth::MapNode* mapNode = (osgEarth::MapNode*)(iEarth->GetPtr(IEarth::EPtrType_EarthMapNode)))
				{
					mapNode->getTerrain()->getHeight(mapNode->getMapSRS(), p3d->longitude, p3d->latitude, &dHeight);
				}
			}

			// 更新选取位置信息
			updatePickTargetPoint(p3d->longitude, p3d->latitude, dHeight);
		}
	}
}

bool LY_AgriDataAnalysisPredict::On2d3dChanged(int wParam)
{
	if (m_pAgriPestAndDiseaseAnalysisWidget != NULL)
	{
		m_pAgriPestAndDiseaseAnalysisWidget->On2d3dChanged(wParam);
	}
	return false;
}

void LY_AgriDataAnalysisPredict::SetScreenCenter(double dLon, double dLat)
{
    if(m_pProj){
        QString str = "CenterAt(" + QString::number(dLon,'f', 8) + "," + QString::number(dLat,'f', 8) + ")";
        LY_AgriDataAnalysisPredictInterface::Get().Script(str.toLocal8Bit());
    }
}

void LY_AgriDataAnalysisPredict::updatePickTargetPoint(double dLon, double dLat, float fAlt)
{
	auto dialog = m_pAgriPestAndDiseaseAnalysisWidget->getPestAndDiseaseDataCollectDialog();
	if (dialog && dialog->isPositionPick())
	{
		if (!dialog->isInitFieldZone())
		{
			// 获取当前激活区域(TODO：操作结束前禁止切换田块)
			auto zone = m_pAgriPestAndDiseaseAnalysisWidget->GetActiveFieldZone();
			dialog->InitActiveFieldZone(zone);
		}

		dialog->updateTargetPoint(dLon, dLat, fAlt);
	}
}
