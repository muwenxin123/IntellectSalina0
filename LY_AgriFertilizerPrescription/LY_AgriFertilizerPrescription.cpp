
//!!!!!!!!
//!!!!!!!!注意：将本文件编码从utf-8改为ansi才能够使用vc2015编译通过，gcc可能不用更改
//!!!!!!!!


#include "LY_AgriFertilizerPrescription.h"
#include <lycore.h>
#if QTCORE_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include "LY_AgriFertilizerPrescriptionInterface.h"
#include <CreateShape.h>
#include <ShapeClip.h>
#include <DrawShape.h>
#include <QCString.h>

//#include <QDb.h>
#include "LyDockManager.h"
#include "LyDockWidget.h"
#include "LyDockTabWidget.h"
#include "LY_AgriFertilizerPrescriptionInterface.h"

#include "LY_AgriFertilizerPrescriptionActivator.h"
#include "IFunctionInterface.h"



Q_DECLARE_METATYPE(qnzkna::framework::IRegistry *)

qnzkna::framework::IRegistry           *LY_AgriFertilizerPrescription::s_IRegistryImpl = nullptr;
qnzkna::AgriFertilizerPrescription::IAgriFertilizerPrescriptionImpl  LY_AgriFertilizerPrescription::s_AgriFertilizerPrescriptionImpl;

LY_AgriFertilizerPrescription:: LY_AgriFertilizerPrescription() : m_viewModel(nullptr)
{
	LY_AgriFertilizerPrescriptionActivator::Get();

}

LY_AgriFertilizerPrescription::~LY_AgriFertilizerPrescription()
{
	m_viewModel = nullptr;
}
void LY_AgriFertilizerPrescription::Reset()
{

}

void LY_AgriFertilizerPrescription::Startup()
{
    IFunctionInterface &ifi = LY_AgriFertilizerPrescriptionInterface::Get().GetFunctionInterface();
	QVariant vt = ifi.Invoke("Ly.IRegistryImpl", 0);
	if (vt.canConvert<qnzkna::framework::IRegistry *>())
	{
		qnzkna::framework::IRegistry *pIRegistry = vt.value<qnzkna::framework::IRegistry *>();
		if (pIRegistry != nullptr)
		{
			LY_AgriFertilizerPrescriptionActivator::Get().start(pIRegistry);
			s_IRegistryImpl = pIRegistry;
			
		}
	}
	if (s_IRegistryImpl != nullptr)
	{
		s_IRegistryImpl->registerService(qnzkna::framework::ServiceReference("Ly.IAgriFertilizerPrescriptionService", &(s_AgriFertilizerPrescriptionImpl)));
	}
}

void LY_AgriFertilizerPrescription::Shutdown()
{
    IFunctionInterface &ifi = LY_AgriFertilizerPrescriptionInterface::Get().GetFunctionInterface();
	QVariant vt = ifi.Invoke("Ly.IRegistryImpl", 0);
	if (vt.canConvert<qnzkna::framework::IRegistry *>())
	{
		qnzkna::framework::IRegistry *pIRegistry = vt.value<qnzkna::framework::IRegistry *>();
		if (pIRegistry != nullptr)
		{
			LY_AgriFertilizerPrescriptionActivator::Get().stop(pIRegistry);
		}
	}
	if (s_IRegistryImpl != nullptr)
	{
		s_IRegistryImpl->unregisterService(qnzkna::framework::ServiceReference("Ly.IAgriFertilizerPrescriptionService", &(s_AgriFertilizerPrescriptionImpl)));
	}
}

bool LY_AgriFertilizerPrescription::OnNetRecv(int nLength, char *lpData)
{
	return false;
}
bool LY_AgriFertilizerPrescription::DrawGraph(QPainter &painter, IProj *pProj)
{

    return false;
}

bool LY_AgriFertilizerPrescription::DrawDynamic(QPainter &painter, IProj *pProj)
{
	if (m_fertilizerPresWidget)
	{

		m_fertilizerPresWidget->draw3DGridMap(painter, pProj);
		m_fertilizerPresWidget->draw2DGridMap(painter, pProj);
	}
	if (m_sprayPresWidget)
	{
		m_sprayPresWidget->zoneHighLight(painter, pProj);
		m_sprayPresWidget->draw2DPathLine(painter, pProj);
	}
	return false;
}

bool LY_AgriFertilizerPrescription::OnMouseButtonDown(QMouseEvent *e)
{

    return false;
}

bool LY_AgriFertilizerPrescription::OnMouseDoubleClick(QMouseEvent *e)
{
    return false;
}

bool LY_AgriFertilizerPrescription::OnCommand(int nID)
{
    switch (nID)
    {
    case 43385:{


	} break;
	case 433393: {
		if (m_fertilizerPresWidget)
		{
			m_fertilizerPresWidget->show();
			m_fertilizerPresWidget->updataCurActiveFarm();
		}
		if (m_sprayPresWidget)
		{
			m_sprayPresWidget->hide();
		}
	} break;
	case 433394:
	{
		if (m_sprayPresWidget)
		{
			m_sprayPresWidget->show();
			m_sprayPresWidget->updataCurActiveFarm();
		}
	}
	break;
	case 50009:
	{
// 		if (m_sprayPresWidget)
// 		{
// 			m_sprayPresWidget->show();
// 			m_sprayPresWidget->updataCurActiveFarm();
// 		}
	}
	break;
    default:
		if (m_fertilizerPresWidget)
		{
			m_fertilizerPresWidget->hide();
		}
		if (m_sprayPresWidget)
		{
			m_sprayPresWidget->hide();
		}
        break;
    }
    return false;
}

bool LY_AgriFertilizerPrescription::OnCreate(int wParam, QWidget *pWidget)
{

    switch (wParam)
    {
	case 1000:
	{
		if (m_viewModel == nullptr)
		{
			m_viewModel = new LY_AgriFertilizePescViewModel();
		}
		if (m_fertilizerPresWidget == nullptr)
		{
			m_fertilizerPresWidget = new LY_AgriFertilizerPresWidget();
			m_fertilizerPresWidget->hide();
			m_fertilizerPresWidget->move(0, 60);
			m_fertilizerPresWidget->resize(QSize(1920, 1020));
		}
		if (m_sprayPresWidget == nullptr)
		{
			m_sprayPresWidget = new LY_AgriSprayPresCommandWidget(pWidget);
			m_sprayPresWidget->hide();
			m_sprayPresWidget->move(0, 60);
			m_sprayPresWidget->resize(QSize(1920, 1020));
		}

	
	}
	break;

	case 0:
	{

	}
	break;
    case 1:   
    {
		if (p_DomainWidget == nullptr)
		{
			p_DomainWidget = new LY_DomainWidget();
		}
	

		
    }
        break;
	case 2:   //上
	{

	}
	break;
	case 3:   //下
	{

	}
	break;
	case 4:
		break;
	}
    return false;
}

bool LY_AgriFertilizerPrescription::OnTimer(int wParam)
{
    switch (wParam)
    {
    case 1://200ms
        LY_AgriFertilizerPrescriptionInterface::Get().Refresh(1);
        break;
    case 2://1s
        break;
    case 3://60s
        break;
    }
    return true;
}

LY_AgriFertilizerPresWidget * LY_AgriFertilizerPrescription::GetFertilizerPresWidget()
{
	return m_fertilizerPresWidget;
}
LY_AgriSprayPresCommandWidget * LY_AgriFertilizerPrescription::GetSprayPresCommandWidget()
{
	return m_sprayPresWidget;
}
LY_AgriFertilizePescViewModel *LY_AgriFertilizerPrescription::GetFertiliVIewModel()
{
	return m_viewModel;
}