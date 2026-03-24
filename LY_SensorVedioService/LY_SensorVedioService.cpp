
//!!!!!!!!
//!!!!!!!!注意：将本文件编码从utf-8改为ansi才能够使用vc2015编译通过，gcc可能不用更改
//!!!!!!!!

#include "LY_SensorVedioService.h"
#include <lycore.h>
#if QTCORE_VERSION >= 0x050000
    #include <QtWidgets>
#else
    #include <QtGui>
#endif
#include "LY_SensorVedioServiceInterface.h"
#include <CreateShape.h>
#include <ShapeClip.h>
#include <DrawShape.h>
#include <QDb.h>
#include "LyDockManager.h"
#include "LyDockWidget.h"
#include "LyDockTabWidget.h"

#include <QThread>
#include <QDebug>
#include <QUuid>
#include <BufferQt.h>
#include <QApplication>
#include <RAGISInterface.h>
#include <Intervisibility.h>
#include <IFunctionInterface.h>
#include "netheader.h"
#include "ISensorVedioServiceImpl.h"
#include "SensorVedioTimer.h"
#include "SensorPlayThread.h"
#include "ServiceFramework/IRegistry.h"
#include "SensorSocketSender.h"
#include "SensorSocketReceiver.h"

#ifdef Q_OS_WIN
#include <windows.h>
#else defined(Q_OS_LINUX)
#include <unistd.h>
#endif // Q_OS_WIN

int getCoreCount()
{
#ifdef Q_OS_WIN
	SYSTEM_INFO sysinfo;
	GetSystemInfo(&sysinfo);
	return sysinfo.dwNumberOfProcessors;
#else defined(Q_OS_LINUX)
	return sysconf(_SC_NPROCESSORS_ONLN)
#endif // Q_OS_WIN
}
Q_DECLARE_METATYPE(qnzkna::framework::IRegistry *)
LY_SensorVedioService:: LY_SensorVedioService()
{
    m_pProj = NULL;
	m_uuID = QUuid::createUuid().toString();
}

void LY_SensorVedioService::Startup()
{
	IFunctionInterface &ifi = LY_SensorVedioServiceInterface::Get().GetFunctionInterface();
	QVariant vt = ifi.Invoke("Ly.IRegistryImpl", 0);
	if (vt.canConvert<qnzkna::framework::IRegistry *>())
	{
		qnzkna::framework::IRegistry *pIRegistry = vt.value<qnzkna::framework::IRegistry *>();
		if (pIRegistry != nullptr)
			{
			pIRegistry->registerService(qnzkna::framework::ServiceReference("Ly.ISensorVedioService", &(qnzkna::SensorVedio::ISensorVedioServiceImpl::Get())));
		}
	}
	QThreadPool::globalInstance()->setMaxThreadCount(qMin(4, getCoreCount() / 3));
}

void LY_SensorVedioService::Shutdown()
{
	if (nullptr != m_pSenderTimer)
	{
		delete m_pSenderTimer;
		m_pSenderTimer = nullptr;
	}

	if (nullptr != m_pSender)
	{
		delete m_pSender;
		m_pSender = nullptr;
	}

	if (nullptr != m_pReceiver)
	{
		delete m_pReceiver;
		m_pReceiver = nullptr;
	}

	IFunctionInterface &ifi = LY_SensorVedioServiceInterface::Get().GetFunctionInterface();
	QVariant vt = ifi.Invoke("Ly.IRegistryImpl", 0);
	if (vt.canConvert<qnzkna::framework::IRegistry*>())
	{
		qnzkna::framework::IRegistry* pIRegistry = vt.value<qnzkna::framework::IRegistry*>();
		if (pIRegistry != nullptr)
		{
			pIRegistry->unregisterService(qnzkna::framework::ServiceReference("Ly.ISensorVedioService", &(qnzkna::SensorVedio::ISensorVedioServiceImpl::Get())));
		}
	}
}

bool LY_SensorVedioService::DrawGraph(QPainter &painter, IProj *pProj)
{
    return false;
}

bool LY_SensorVedioService::DrawDynamic(QPainter &painter, IProj *pProj)
{
    return false;
}

bool LY_SensorVedioService::OnMouseButtonDown(QMouseEvent *e)
{
    bool bReturn = false;
	
    return bReturn;
}

bool LY_SensorVedioService::OnMouseDoubleClick(QMouseEvent *e)
{
    return false;
}

bool LY_SensorVedioService::OnMouseButtonUp(QMouseEvent *e)
{
	return false;
}

bool LY_SensorVedioService::OnCommand(int nID)
{
    switch (nID)
    {
    default:
        break;
    }
    return false;
}

bool LY_SensorVedioService::OnCreate(int wParam, QWidget *pWidget)
{
    switch (wParam)
    {
    case 1://左
        break;
    case 3:   //下
    {
    }
    break;
    case 0:
    {
        m_pWnd = pWidget;
#ifdef FULL_SMOOTH_IMAGE
		auto pPlayThread = new SensorPlayThread(m_pWnd);
		pPlayThread->connect(QApplication::instance(), &QCoreApplication::aboutToQuit,
			[pTmpPlayThread = pPlayThread]() {
			delete pTmpPlayThread);
		}
		);
#endif

		m_pSenderTimer = new SensorVedioTimer();

		m_pSender = new SensorSocketSender(m_pWnd);
		m_pSender->start();

		m_pReceiver = new SensorSocketReceiver(m_pWnd);
		m_pReceiver->start();
    }
    break;
    }
    return false;
}

bool LY_SensorVedioService::OnTimer(int wParam)
{
    switch (wParam)
    {
    case 1://200ms
        LY_SensorVedioServiceInterface::Get().Refresh(1);
		break;
    case 2://1s

        break;
    case 3://60s
        break;
    }
    return false;
}

bool LY_SensorVedioService::OnNetRecv(int nLength, char *lpData)
{
	return false;
}

QString LY_SensorVedioService::GetUUID()
{
	return m_uuID;
}

