#include "LY_AgriMachineManager.h"
#include "LY_AgriMachineManagerInterface.h"

#include "IProj.h"
#include "IFunctionInterface.h"
#include "lyUI/LyDockManager.h"

#include "IAgriMachineManagerActivator.h"
#include "AgriMachineXMLCfgGenerater.h"


Q_DECLARE_METATYPE(qnzkna::framework::IRegistry *)

LY_AgriMachineManager::LY_AgriMachineManager()
{
    IAgriMachineManagerActivator::Get();
}

void LY_AgriMachineManager::Startup()
{
    IFunctionInterface &ifi = LY_AgriMachineManagerInterface::Get().GetFunctionInterface();
    QVariant vt = ifi.Invoke("Ly.IRegistryImpl", 0);
    if (vt.canConvert<qnzkna::framework::IRegistry *>())
    {
        qnzkna::framework::IRegistry *pIRegistry = vt.value<qnzkna::framework::IRegistry *>();
        if (pIRegistry != nullptr)
        {
            IAgriMachineManagerActivator::Get().start(pIRegistry);
        }
    }
}

void LY_AgriMachineManager::Shutdown()
{
    IFunctionInterface &ifi = LY_AgriMachineManagerInterface::Get().GetFunctionInterface();
    QVariant vt = ifi.Invoke("Ly.IRegistryImpl", 0);
    if (vt.canConvert<qnzkna::framework::IRegistry *>())
    {
        qnzkna::framework::IRegistry *pIRegistry = vt.value<qnzkna::framework::IRegistry *>();
        if (pIRegistry != nullptr)
        {
            IAgriMachineManagerActivator::Get().stop(pIRegistry);
        }
    }
}

void LY_AgriMachineManager::Reset()
{

}

bool LY_AgriMachineManager::DrawGraph(QPainter &painter, IProj *pProj)
{
    return false;
}

bool LY_AgriMachineManager::DrawDynamic(QPainter &painter, IProj *pProj)
{
    return false;
}

bool LY_AgriMachineManager::OnMouseButtonDown(QMouseEvent *e)
{
    bool bReturn = false;
    return bReturn;
}

bool LY_AgriMachineManager::OnMouseDoubleClick(QMouseEvent *e)
{
    return false;
}

bool LY_AgriMachineManager::OnCommand(int nID, void *lParam )
{
	switch (nID)
	{
	case 433381:
	{
		if (m_pAgriMachineManagerDialog)
		{
			if (m_pAgriMachineManagerDialog->isHidden())
			{
				m_pAgriMachineManagerDialog->show();
			}
		}
	}
	break;
	default:
	{
		if (m_pAgriMachineManagerDialog)
		{
			m_pAgriMachineManagerDialog->hide();
		}
	}
	break;
	}
	return false;
}

bool LY_AgriMachineManager::OnCreate(int wParam, QWidget *pWgt)
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
		if (m_pAgriMachineManagerDialog == nullptr)
		{
			QSize screenSize = qApp->property("ly_screen_xy").toSize();
			
			// 初始化数据库数据
			m_pAgriMachineDatabaseHandler = new AgriMachineDatabaseHandler(pWgt);

			// 初始化农机管理器
			m_pAgriMachineManager = new AgriMachineManager();

			// 初始化界面数据
			m_pAgriMachineManagerDialog = new AgriMachineManagerDialog(pWgt);

			m_pAgriMachineManagerDialog->hide();
			m_pAgriMachineManagerDialog->move(0, 60);
			m_pAgriMachineManagerDialog->resize(QSize(screenSize.width(), screenSize.height() - 60));

			// 生成配置文件
			AgriMachineXMLCfgGenerater xmlGenerater;
			xmlGenerater.generateManchineXmlCfg(AGRI_PATH_MACHINE_XML_CONFIG);
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

bool LY_AgriMachineManager::OnTimer(int wParam)
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

bool LY_AgriMachineManager::OnNetRecv(int nLength, char *lpData)
{
    return false;
}

void LY_AgriMachineManager::SetScreenCenter(const double dLon, const double dLat)
{
    if(m_pProj){
        QString str = "CenterAt(" + QString::number(dLon,'f', 8) + "," + QString::number(dLat,'f', 8) + ")";
        LY_AgriMachineManagerInterface::Get().Script(str.toLocal8Bit());
    }
}
