#include <QPoint>
#include <QMouseEvent>

#include "LY_AgriMultispectralSensor.h"
#include "LY_AgriMultispectralSensorInterface.h"

#include "IProj.h"
#include "IFunctionInterface.h"
#include "lyUI/LyDockManager.h"

#include "IAgriMultispectralSensorActivator.h"

Q_DECLARE_METATYPE(qnzkna::framework::IRegistry *)

LY_AgriMultispectralSensor::LY_AgriMultispectralSensor()
{
    IAgriMultispectralSensorActivator::Get();
}

void LY_AgriMultispectralSensor::Startup()
{
    IFunctionInterface &ifi = LY_AgriMultispectralSensorInterface::Get().GetFunctionInterface();
    QVariant vt = ifi.Invoke("Ly.IRegistryImpl", 0);
    if (vt.canConvert<qnzkna::framework::IRegistry *>())
    {
        qnzkna::framework::IRegistry *pIRegistry = vt.value<qnzkna::framework::IRegistry *>();
        if (pIRegistry != nullptr)
        {
            IAgriMultispectralSensorActivator::Get().start(pIRegistry);
        }
    }
}

void LY_AgriMultispectralSensor::Shutdown()
{
    IFunctionInterface &ifi = LY_AgriMultispectralSensorInterface::Get().GetFunctionInterface();
    QVariant vt = ifi.Invoke("Ly.IRegistryImpl", 0);
    if (vt.canConvert<qnzkna::framework::IRegistry *>())
    {
        qnzkna::framework::IRegistry *pIRegistry = vt.value<qnzkna::framework::IRegistry *>();
        if (pIRegistry != nullptr)
        {
            IAgriMultispectralSensorActivator::Get().stop(pIRegistry);
        }
    }
}

void LY_AgriMultispectralSensor::Reset()
{

}

bool LY_AgriMultispectralSensor::DrawGraph(QPainter &painter, IProj *pProj)
{
    return false;
}

bool LY_AgriMultispectralSensor::DrawDynamic(QPainter &painter, IProj *pProj)
{
	if (m_pAgriMultispectralAnalysis)
	{
		if (auto dialog = m_pAgriMultispectralAnalysis->getFieldIdentifyDialog())
		{
			dialog->drawDynamic(painter, pProj);
		}
	}

    return true;
}

bool LY_AgriMultispectralSensor::OnMouseButtonDown(QMouseEvent *e)
{
	bool bReturn = false;

	QPoint pt = e->pos();

	if (e->button() == Qt::LeftButton && m_pAgriMultispectralAnalysis)
	{
		auto dialog = m_pAgriMultispectralAnalysis->getFieldIdentifyDialog();
		if (dialog && dialog->isPositionPick())
		{
			double lon, lat;
			m_pProj->Screen2Jwd(pt, lon, lat);
			dialog->updateTargetPoint(QPointF(lon, lat));
		}
	}

	return bReturn;
}

bool LY_AgriMultispectralSensor::OnMouseDoubleClick(QMouseEvent *e)
{
    return false;
}

bool LY_AgriMultispectralSensor::OnCommand(int nID, void *lParam )
{
	switch (nID)
	{
	case 433391:
	{
		//if (m_pAgriMultispectralSensorWidget)
		//{
		//	if (m_pAgriMultispectralSensorWidget->isHidden())
		//	{
		//		m_pAgriMultispectralSensorWidget->show();
		//	}
		//}
		if (m_pAgriMultispectralAnalysis)
		{
			if (m_pAgriMultispectralAnalysis->isHidden())
			{
				m_pAgriMultispectralAnalysis->show();
			}
		}
	}
	break;
	default:
	{
		//if (m_pAgriMultispectralSensorWidget)
		//{
		//	m_pAgriMultispectralSensorWidget->hide();
		//}
		if (m_pAgriMultispectralAnalysis)
		{
			m_pAgriMultispectralAnalysis->hide();
		}
	}
		break;
	}
	return false;
}

bool LY_AgriMultispectralSensor::OnCreate(int wParam, QWidget *pWgt)
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
		m_pAgriMultispectralSensorDBHandler = new AgriMultispectralSensorDBHandler(pWgt);

		if (m_pAgriMultispectralAnalysis == nullptr) {
			m_pAgriMultispectralAnalysis = new AgriMultispectralAnalysis(pWgt);
			m_pAgriMultispectralAnalysis->hide();
			m_pAgriMultispectralAnalysis->move(0, 60);
			m_pAgriMultispectralAnalysis->resize(QSize(1920, 1020));
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

bool LY_AgriMultispectralSensor::OnTimer(int wParam)
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

bool LY_AgriMultispectralSensor::OnNetRecv(int nLength, char *lpData)
{
    return false;
}

bool LY_AgriMultispectralSensor::On2d3dChanged(int wParam)
{
	return false;
}

void LY_AgriMultispectralSensor::SetScreenCenter(const double dLon, const double dLat)
{
    if(m_pProj){
        QString str = "CenterAt(" + QString::number(dLon,'f', 8) + "," + QString::number(dLat,'f', 8) + ")";
        LY_AgriMultispectralSensorInterface::Get().Script(str.toLocal8Bit());
    }
}
