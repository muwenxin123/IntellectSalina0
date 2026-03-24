#include "LY_ConfigBundleControlService.h"
#include "LY_ConfigBundleControlServiceInterface.h"
#include <IFunctionInterface.h>

#include <QDebug>

qnzkna::framework::IRegistry            *LY_ConfigBundleControlService::s_IRegistryImpl = nullptr;
qnzkna::SystemConfig::IConfigBundleControlImpl  LY_ConfigBundleControlService::s_IConfigBundleControlImpl;
qnzkna::SystemConfig::IConfigProjectControlImpl LY_ConfigBundleControlService::s_IConfigProjectControlImpl;

LY_ConfigBundleControlService::LY_ConfigBundleControlService()
{

}

void LY_ConfigBundleControlService::Startup()
{
    QStringList bundlePluginList =
    {
        "LyUcisAerialShotSettingBundleNEO",
    };

    std::list<std::pair<std::string, std::string>> bundlePluginPath;
    for (const auto &projectInfo : s_IConfigProjectControlImpl.vecProjectInfo())
    {
        if (s_IConfigProjectControlImpl.strCurProject() != projectInfo.strProjectName())
        {
            continue;
        }
        bundlePluginList.push_back(QString::fromStdString(s_IConfigProjectControlImpl.strCurProject()));
        for (const auto &projectBundle : projectInfo.ProjectBundleVector())
        {
            for (const auto &bundleInfo : s_IConfigBundleControlImpl.getVecBundleInfo())
            {
                if (bundleInfo.getBundleName() == projectBundle.m_strName)
                {
#ifdef _DEBUG
                    bundlePluginList.push_back(QString::fromStdString(bundleInfo.getLibNameDebug()));
                    bundlePluginPath.push_back(std::make_pair(bundleInfo.getLibNameDebug(), bundleInfo.getLibNameDebug()));
#else
                    bundlePluginList.push_back(QString::fromStdString(bundleInfo.getLibNameRelease()));
                    bundlePluginPath.push_back(std::make_pair(bundleInfo.getLibNameRelease(), bundleInfo.getLibNameRelease()));
#endif
                    break;
                }

            }
        }
        break;
    }
    qDebug() << __LINE__ << __FILE__ << bundlePluginList;

    IFunctionInterface &ifi = LY_ConfigBundleControlServiceInterface::Get().GetFunctionInterface();
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
        s_IRegistryImpl->registerService(qnzkna::framework::ServiceReference("Ly.IConfigBundleControlService",  &(s_IConfigBundleControlImpl)));
        s_IRegistryImpl->registerService(qnzkna::framework::ServiceReference("Ly.IConfigProjectControlService", &(s_IConfigProjectControlImpl)));
    }
}

void LY_ConfigBundleControlService::Shutdown()
{
    if (s_IRegistryImpl != nullptr)
    {
        s_IRegistryImpl->unregisterService(qnzkna::framework::ServiceReference("Ly.IConfigBundleControlService",  &(s_IConfigBundleControlImpl)));
        s_IRegistryImpl->unregisterService(qnzkna::framework::ServiceReference("Ly.IConfigProjectControlService", &(s_IConfigProjectControlImpl)));
    }
}

void LY_ConfigBundleControlService::Reset()
{

}

bool LY_ConfigBundleControlService::DrawGraph(QPainter &painter, IProj *pProj)
{
    return false;
}

bool LY_ConfigBundleControlService::DrawDynamic(QPainter &painter, IProj *pProj)
{
    return false;
}

bool LY_ConfigBundleControlService::OnMouseButtonDown(QMouseEvent *e)
{
    bool bReturn = false;
    return bReturn;
}

bool LY_ConfigBundleControlService::OnMouseDoubleClick(QMouseEvent *e)
{
    return false;
}

bool LY_ConfigBundleControlService::OnCommand(int nID)
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

bool LY_ConfigBundleControlService::OnCreate(int wParam, QTabWidget *pTab)
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
    case 4:
        break;
    }
    return false;
}

bool LY_ConfigBundleControlService::OnTimer(int wParam)
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
