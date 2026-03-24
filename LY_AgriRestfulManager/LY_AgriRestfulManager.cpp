#include <QPoint>
#include <QMouseEvent>
#include <map3d/IEarth.h>
#include <map3d/EarthEvent.h>
#include <osgEarth/MapNode>

#include "LY_AgriRestfulManager.h"
#include "LY_AgriRestfulManagerInterface.h"
#include "IProj.h"
#include "IFunctionInterface.h"
#include "lyUI/LyDockManager.h"
#include "IAgriRestfulManagerActivator.h"

#include "./controller/UAVPathExecController.h"
#include "./controller/UAVFertilizeController.h"
#include "./controller/UAVPatrolController.h"
#include "./controller/UAVSprayController.h"
#include "./controller/UAVStatusController.h"
#include "./controller/WeatherBasicController.h"
#include "./controller/WorkResultController.h"

#include "./AppComponent.h"
#include "oatpp/network/Server.hpp"
#include "handler/testHandler.h"

#include "task/ThreadPool.h"
//#include "task/TaskTest.h"

Q_DECLARE_METATYPE(qnzkna::framework::IRegistry *)


void run() {

	/* Register Components in scope of run() method */
	AppComponent components;

	/* Get router component */
	OATPP_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, router);

	/* Create MyController and add all of its endpoints to router */
	router->addController(std::make_shared<UAVPathExecController>());
	router->addController(std::make_shared<UAVFertilizeController>());
	router->addController(std::make_shared<UAVPatrolController>());
	router->addController(std::make_shared<UAVSparyDTO::UAVSprayController>());
	router->addController(std::make_shared<UAVStatusController>());
	router->addController(std::make_shared<WeatherBasicController>());
	router->addController(std::make_shared<WorkResultController>());

	/* Get connection handler component */
	OATPP_COMPONENT(std::shared_ptr<oatpp::network::ConnectionHandler>, connectionHandler);

	/* Get connection provider component */
	OATPP_COMPONENT(std::shared_ptr<oatpp::network::ServerConnectionProvider>, connectionProvider);

	/* Create server which takes provided TCP connections and passes them to HTTP connection handler */
	oatpp::network::Server server(connectionProvider, connectionHandler);

	/* Print info about server port */
	OATPP_LOGi("MyApp", "Server running on port {}", connectionProvider->getProperty("port").toString());

	/* Run server */
	server.run();
}

class LY_AgriRestfulManagerPrivate
{
public:
	LY_AgriRestfulManagerPrivate() {

		OEThreadPool::ThreadPoolConfig threadPoolConfig;
		threadPoolConfig.nMaxThreadsNum = 10;
		threadPoolConfig.nMinThreadsNum = 2;
		threadPoolConfig.dbTaskAddThreadRate = 3;
		threadPoolConfig.dbTaskSubThreadRate = 0.5;

		if (!p_ThreadPool)
		{
			p_ThreadPool = std::shared_ptr<OEThreadPool>(new OEThreadPool);
			p_ThreadPool->init(threadPoolConfig);
		}
	}
	~LY_AgriRestfulManagerPrivate()
	{
		if (p_ThreadPool)
		{
			p_ThreadPool->release();
		}
	}

	std::shared_ptr<OEThreadPool> p_ThreadPool = nullptr;
};

LY_AgriRestfulManager::LY_AgriRestfulManager()
	:_p(new LY_AgriRestfulManagerPrivate)
{
    IAgriRestfulManagerActivator::Get();
}

void LY_AgriRestfulManager::Startup()
{
    IFunctionInterface &ifi = LY_AgriRestfulManagerInterface::Get().GetFunctionInterface();
    QVariant vt = ifi.Invoke("Ly.IRegistryImpl", 0);
    if (vt.canConvert<qnzkna::framework::IRegistry *>())
    {
        qnzkna::framework::IRegistry *pIRegistry = vt.value<qnzkna::framework::IRegistry *>();
        if (pIRegistry != nullptr)
        {
            IAgriRestfulManagerActivator::Get().start(pIRegistry);
        }
    }
}

void LY_AgriRestfulManager::Shutdown()
{
    IFunctionInterface &ifi = LY_AgriRestfulManagerInterface::Get().GetFunctionInterface();
    QVariant vt = ifi.Invoke("Ly.IRegistryImpl", 0);
    if (vt.canConvert<qnzkna::framework::IRegistry *>())
    {
        qnzkna::framework::IRegistry *pIRegistry = vt.value<qnzkna::framework::IRegistry *>();
        if (pIRegistry != nullptr)
        {
            IAgriRestfulManagerActivator::Get().stop(pIRegistry);
        }
    }
}

void LY_AgriRestfulManager::Reset()
{

}

int LY_AgriRestfulManager::addTask(std::shared_ptr<ITask> taskptr, bool priority /*= false*/)
{
	if (_p && _p->p_ThreadPool){
		return _p->p_ThreadPool->addTask(taskptr, priority);
	}
	return -1;
}

std::shared_ptr<ITask> LY_AgriRestfulManager::isExistTaskProcess()
{
	if (_p && _p->p_ThreadPool) {
		return _p->p_ThreadPool->isExistTaskProcess();
	}
	return nullptr;
}

bool LY_AgriRestfulManager::DrawGraph(QPainter &painter, IProj *pProj)
{
    return false;
}

bool LY_AgriRestfulManager::DrawDynamic(QPainter &painter, IProj *pProj)
{
    return true;
}

bool LY_AgriRestfulManager::OnMouseButtonDown(QMouseEvent *e)
{
	return false;
}

bool LY_AgriRestfulManager::OnMouseDoubleClick(QMouseEvent *e)
{
    return false;
}

bool LY_AgriRestfulManager::OnCommand(int nID, void *lParam )
{
	switch (nID)
	{
	default:
	break;
	}
	return false;
}

bool LY_AgriRestfulManager::OnCreate(int wParam, QWidget *pWgt)
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
		std::thread restfukWorker([=]() {

			oatpp::Environment::init();

			run();

			// Print how much objects were created during app running, and what have left-probably leaked
			// Disable object counting for release builds using '-D OATPP_DISABLE_ENV_OBJECT_COUNTERS' flag for better performance
			std::cout << "\nEnvironment:\n";
			std::cout << "objectsCount = " << oatpp::Environment::getObjectsCount() << "\n";
			std::cout << "objectsCreated = " << oatpp::Environment::getObjectsCreated() << "\n\n";

			oatpp::Environment::destroy();
		});
		restfukWorker.detach();
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

bool LY_AgriRestfulManager::OnTimer(int wParam)
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

bool LY_AgriRestfulManager::OnNetRecv(int nLength, char *lpData)
{
    return false;
}

void LY_AgriRestfulManager::OnEarthMessage(int wParam, void *lParam)
{
	return;
}

bool LY_AgriRestfulManager::On2d3dChanged(int wParam)
{
	return false;
}

void LY_AgriRestfulManager::SetScreenCenter(double dLon, double dLat)
{
	return;
}
