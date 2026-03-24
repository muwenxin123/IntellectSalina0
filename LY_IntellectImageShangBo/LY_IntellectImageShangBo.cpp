#include "LY_IntellectImageShangBo.h"
#include <lycore.h>

#include "lyUI/LyDockManager.h"
#include "IFunctionInterface.h"
#include "LY_IntellectImageShangBoInterface.h"

#include "IntellectImageShangBoDialog.h"

#include "IIntellectImageShangBoActivator.h"

Q_DECLARE_METATYPE(qnzkna::framework::IRegistry*)

LY_IntellectImageShangBo::LY_IntellectImageShangBo()
    : m_pIntellectImageShangBoDialog(nullptr)
{
    IIntellectImageShangBoActivator::Get();
}

void LY_IntellectImageShangBo::Startup()
{
    IFunctionInterface &ifi = LY_IntellectImageShangBoInterface::Get().GetFunctionInterface();
    QVariant vt = ifi.Invoke("Ly.IRegistryImpl", 0);
    if (vt.canConvert<qnzkna::framework::IRegistry*>()) {
        qnzkna::framework::IRegistry* pIRegistry = vt.value<qnzkna::framework::IRegistry*>();
        if(pIRegistry != nullptr){
            IIntellectImageShangBoActivator::Get().start(pIRegistry);
        }
    }
}

void LY_IntellectImageShangBo::Shutdown()
{
    IFunctionInterface &ifi = LY_IntellectImageShangBoInterface::Get().GetFunctionInterface();
    QVariant vt = ifi.Invoke("Ly.IRegistryImpl", 0);
    if (vt.canConvert<qnzkna::framework::IRegistry*>()) {
        qnzkna::framework::IRegistry* pIRegistry = vt.value<qnzkna::framework::IRegistry*>();
        if(pIRegistry != nullptr){
            IIntellectImageShangBoActivator::Get().stop(pIRegistry);
        }
    }
}

void LY_IntellectImageShangBo::Reset()
{

}

bool LY_IntellectImageShangBo::DrawGraph( QPainter &painter, IProj *pProj )
{

    return false;
}

bool LY_IntellectImageShangBo::DrawDynamic( QPainter &painter, IProj *pProj )
{

    return false;
}

bool LY_IntellectImageShangBo::OnMouseButtonDown( QMouseEvent *e )
{
    bool bReturn = false;
    QPoint pt = e->pos();
    switch(e->button())
    {
    case Qt::LeftButton:
    {

    }
        break;
    case Qt::RightButton:

    {

    }
        break;
    case Qt::MiddleButton:
    {

    }
        break;
    }

    return bReturn;
}

bool LY_IntellectImageShangBo::OnMouseDoubleClick(QMouseEvent *e)
{

    return true;
}

bool LY_IntellectImageShangBo::OnCommand( int nID )
{
    switch(nID)
    {
    case 43385:
    {

    }
        break;
    case 44777:
    {

    }
        return true;
    case 44778:
    {

    }
        break;
    default:
        break;
    }
    return false;
}

bool LY_IntellectImageShangBo::OnCreate( int wParam, QTabWidget *pTab )
{
    switch(wParam)
    {
    case 0:
    {
        if(m_pIntellectImageShangBoDialog == nullptr){
            LyDockWidget *pDock = LyDockManager::instance()->dockWidget("IntellectImageShangBoDialog");
            if (pDock == nullptr)
            {
                pDock = LyDockManager::instance()->createDockWidget(LyDockDef::Tool, IntellectImageShangBoDialog::tr("\xe5\x90\x8a\xe8\x88\xb1\xe6\x8e\xa7\xe5\x88\xb6"), "IntellectImageShangBoDialog");
                LyDockManager::instance()->embedToolDockWidget(pDock, LyDockDef::Right);
            }

            if(m_pIntellectImageShangBoDialog == nullptr){
                m_pIntellectImageShangBoDialog = new IntellectImageShangBoDialog();

                pDock->setCentralWidget(m_pIntellectImageShangBoDialog);
                pDock->setWindowTitle(IntellectImageShangBoDialog::tr("\xe5\x90\x8a\xe8\x88\xb1\xe6\x8e\xa7\xe5\x88\xb6"));
                LyDockWidget::connect(m_pIntellectImageShangBoDialog, &IntellectImageShangBoDialog::windowTitleChanged, pDock, &LyDockWidget::setWindowTitle);
            }
        }

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
    case 4: m_pWnd = pTab; break;
    }
    return false;
}

bool LY_IntellectImageShangBo::OnTimer( int wParam )
{
    switch (wParam)
    {
    case 1:
        LY_IntellectImageShangBoInterface::Get().Refresh(1);
        break;
    case 2:
        break;
    case 3:
        break;
    }
    return true;
}
