#include "lY_TaskPlatMatch.h"
#include <lycore.h>
#include <QtWidgets>
#include "lY_TaskPlatMatchInterface.h"
#include "lyUI/LyDockManager.h"
#include "BiIntervisibility.h"

Q_DECLARE_METATYPE(qnzkna::framework::IRegistry *)

LY_TaskPlatMatch::LY_TaskPlatMatch()
{
    m_pProj = NULL;
}

void LY_TaskPlatMatch::Startup()
{
    IFunctionInterface &ifi = LY_TaskPlatMatchInterface::Get().GetFunctionInterface();
    QVariant vt = ifi.Invoke("Ly.IRegistryImpl", 0);
    if (vt.canConvert<qnzkna::framework::IRegistry *>())
    {
        qnzkna::framework::IRegistry *pIRegistry = vt.value<qnzkna::framework::IRegistry *>();
        if (pIRegistry != nullptr)
        {
            ITaskPlatMatchActivator::Get().start(pIRegistry);
        }
    }
}

void LY_TaskPlatMatch::Shutdown()
{
    IFunctionInterface &ifi = LY_TaskPlatMatchInterface::Get().GetFunctionInterface();
    QVariant vt = ifi.Invoke("Ly.IRegistryImpl", 0);
    if (vt.canConvert<qnzkna::framework::IRegistry *>())
    {
        qnzkna::framework::IRegistry *pIRegistry = vt.value<qnzkna::framework::IRegistry *>();
        if (pIRegistry != nullptr)
        {
            ITaskPlatMatchActivator::Get().stop(pIRegistry);
        }
    }
}

void LY_TaskPlatMatch::Reset()
{
}

bool LY_TaskPlatMatch::DrawGraph(QPainter &painter, IProj *pProj)
{
    return false;
}

bool LY_TaskPlatMatch::DrawDynamic(QPainter &painter, IProj *pProj)
{
    return false;
}

bool LY_TaskPlatMatch::OnMouseButtonDown(QMouseEvent *e)
{
    bool bReturn = false;
    return bReturn;
}

bool LY_TaskPlatMatch::OnMouseDoubleClick(QMouseEvent *e)
{
    return false;
}

bool LY_TaskPlatMatch::OnCommand(int nID)
{
    switch (nID)
    {
    case 50007:
    {
        if (m_pDlgTaskPlatMatch == NULL)
        {
            m_pDlgTaskPlatMatch = new CDlgTaskPlatMatch;

        }
        if (m_pDlgTaskPlatMatch->getTabWidget()->rowCount() > 0)
        {
            m_pDlgTaskPlatMatch->UpdataWindowData();
            m_pDlgTaskPlatMatch->getTabWidget()->item(0, 0)->setSelected(true);
            m_pDlgTaskPlatMatch->on_setMatch_clicked();
        }
    }
    break;
    default:
        break;
    }
    return false;
}

bool LY_TaskPlatMatch::OnCreate(int wParam, QTabWidget *pTab)
{
    switch (wParam)
    {
    case 0:
    {

    }
    break;
    case 1:
    {
        LyDockWidget *pMarkStrikeDlg = LyDockManager::instance()->dockWidget("TaskPlatMatch");
        if (pMarkStrikeDlg == nullptr)
        {
            pMarkStrikeDlg = LyDockManager::instance()->createDockWidget(LyDockDef::Tool, QObject::tr("PlatMatchList"), "TaskPlatMatch");

            LyDockManager::instance()->embedToolDockWidget(pMarkStrikeDlg, LyDockDef::Right);

        }
        if (m_pDlgTaskPlatMatch == nullptr)
        {
            m_pDlgTaskPlatMatch = new CDlgTaskPlatMatch;
            pMarkStrikeDlg->setCentralWidget(m_pDlgTaskPlatMatch);
            pMarkStrikeDlg->setWindowTitle(QObject::tr("PlatMatchList"));

            if (ITaskPlatMatchActivator::Get().getPTaskManageService() != NULL)
            {
                ITaskPlatMatchActivator::Get().getPTaskManageService()->Attach(m_pDlgTaskPlatMatch);
            }

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

    }
    return false;
}

bool LY_TaskPlatMatch::OnTimer(int wParam)
{
    switch (wParam)
    {
    case 1:
        LY_TaskPlatMatchInterface::Get().Refresh(1);
        if (isPlatMateMerge)
        {
            if (m_pDlgTaskPlatMatch->isVisible())
            {
                m_pDlgTaskPlatMatch->UpdataWindowData();
            }
            else
            {
                m_pDlgTaskPlatMatch->UpdataWindowData();
            }
            isPlatMateMerge = false;
        }
        break;
    case 2:
        break;
    case 3:
        break;
    }
    return true;
}

void LY_TaskPlatMatch::showtestSql()
{

}

CDlgTaskPlatMatch *LY_TaskPlatMatch::gettaskPlatMatch()
{
    return m_pDlgTaskPlatMatch;
}
