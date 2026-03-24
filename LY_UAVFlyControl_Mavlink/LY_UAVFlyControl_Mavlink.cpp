#include "LY_UAVFlyControl_Mavlink.h"
#include "LY_UAVFlyControl_MavlinkInterface.h"

#include "IFunctionInterface.h"
#include "lyUI/LyDockManager.h"
#include "lyUI/LyWidget.h"
#include "lyUI/LyDialog.h"

#include "IUAVFlyControl_MavlinkActivator.h"

#include "UAVFlyManage_MavlinkMainDialog.h"

#include "UAVFlyManage_MavlinkMessageIndicatorDialog.h"

#include "FlyControl/UAVFlyManage_MavlinkDialog.h"
#include "FlyControl/UAVSelectDialog.h"
#include "TaskManage/UAVFlyManage_MavlinkTaskManageDialog.h"

#include "VehicleManage/GuidedGoto/UAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialog.h"

#include "QGCApplication.h"

Q_DECLARE_METATYPE(qnzkna::framework::IRegistry*)

LY_UAVFlyControl_Mavlink::LY_UAVFlyControl_Mavlink()
    : m_pTab(nullptr)
    , m_pUAVFlyManage_MavlinkMainDialog(nullptr)

    , m_pUAVSelectDialog(nullptr)
    , m_pUAVFlyManageDialog(nullptr)
    , m_pUAVFlyManage_MavlinkTaskManageLyWidgetDialog(nullptr)
    , m_pUAVFlyManage_MavlinkTaskManageDialog(nullptr)
    , m_pUAVFlyManage_MavlinkMessageIndicatorDialog(nullptr)
    , m_pUAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialogLyWidgetDialog(nullptr)
    , m_pUAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialog(nullptr)
{
    IUAVFlyControl_MavlinkActivator::Get();
}

void LY_UAVFlyControl_Mavlink::Startup()
{
    IFunctionInterface &ifi = LY_UAVFlyControl_MavlinkInterface::Get().GetFunctionInterface();
    QVariant vt = ifi.Invoke("Ly.IRegistryImpl", 0);
    if (vt.canConvert<qnzkna::framework::IRegistry*>()) {
        qnzkna::framework::IRegistry* pIRegistry = vt.value<qnzkna::framework::IRegistry*>();
        if(pIRegistry != nullptr){
            IUAVFlyControl_MavlinkActivator::Get().start(pIRegistry);
        }
    }
}

void LY_UAVFlyControl_Mavlink::Shutdown()
{
    IFunctionInterface &ifi = LY_UAVFlyControl_MavlinkInterface::Get().GetFunctionInterface();
    QVariant vt = ifi.Invoke("Ly.IRegistryImpl", 0);
    if (vt.canConvert<qnzkna::framework::IRegistry*>()) {
        qnzkna::framework::IRegistry* pIRegistry = vt.value<qnzkna::framework::IRegistry*>();
        if(pIRegistry != nullptr){
            IUAVFlyControl_MavlinkActivator::Get().stop(pIRegistry);
        }
    }

    if(MavlinkApp()){
        MavlinkApp()->deleteLater();
    }
}

void LY_UAVFlyControl_Mavlink::Reset()
{

}

bool LY_UAVFlyControl_Mavlink::DrawGraph( QPainter & , IProj * )
{
    return false;
}

bool LY_UAVFlyControl_Mavlink::DrawDynamic( QPainter &painter, IProj *pProj )
{
    if(m_pUAVFlyManage_MavlinkTaskManageDialog){
        m_pUAVFlyManage_MavlinkTaskManageDialog->DrawDynamic(painter, pProj);
    }

    if(m_pUAVFlyManage_MavlinkMainDialog){
        m_pUAVFlyManage_MavlinkMainDialog->DrawDynamic(painter, pProj);
    }

    if(m_pUAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialog){
        m_pUAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialog->DrawDynamic(painter, pProj);
    }

    return false;
}

bool LY_UAVFlyControl_Mavlink::OnMouseButtonDown( QMouseEvent *e)
{
    if(m_pUAVFlyManage_MavlinkTaskManageDialog){
        m_pUAVFlyManage_MavlinkTaskManageDialog->OnMouseButtonDown(e, m_pProj);
    }

    if(m_pUAVFlyManage_MavlinkMainDialog){
        m_pUAVFlyManage_MavlinkMainDialog->OnMouseButtonDown(e, m_pProj);
    }

    return false;
}

bool LY_UAVFlyControl_Mavlink::OnMouseButtonRelease( QMouseEvent *e)
{
    if(m_pUAVFlyManage_MavlinkTaskManageDialog){
        m_pUAVFlyManage_MavlinkTaskManageDialog->OnMouseButtonRelease(e, m_pProj);
    }

    if(m_pUAVFlyManage_MavlinkMainDialog){
        m_pUAVFlyManage_MavlinkMainDialog->OnMouseButtonRelease(e, m_pProj);
    }

    return false;
}

bool LY_UAVFlyControl_Mavlink::OnMouseDoubleClick(QMouseEvent *e)
{
    if(m_pUAVFlyManage_MavlinkTaskManageDialog){
        m_pUAVFlyManage_MavlinkTaskManageDialog->OnMouseDoubleClick(e, m_pProj);
    }

    if(m_pUAVFlyManage_MavlinkMainDialog){
        m_pUAVFlyManage_MavlinkMainDialog->OnMouseDoubleClick(e, m_pProj);
    }

    if(m_pUAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialog){
        m_pUAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialog->OnMouseDoubleClick(e, m_pProj);
    }

    return false;
}

bool LY_UAVFlyControl_Mavlink::OnMouseMove(QMouseEvent *e)
{
    if(m_pUAVFlyManage_MavlinkTaskManageDialog){
        m_pUAVFlyManage_MavlinkTaskManageDialog->OnMouseMove(e, m_pProj);
    }

    if(m_pUAVFlyManage_MavlinkMainDialog){
        m_pUAVFlyManage_MavlinkMainDialog->OnMouseMove(e, m_pProj);
    }

    return false;
}

bool LY_UAVFlyControl_Mavlink::OnCommand( int nID , void *lParam)
{
    switch(nID)
    {
    case 43385:
    {

    }
    break;
	case 433387:
	{
		if (m_pUAVFlyManageDialog != nullptr) {
			m_pUAVFlyManageDialog->show();
			m_pUAVFlyManageDialog->raise();
			//m_pUAVFlyManageDialog->hide();
		}


	}
	break;
    case 44777:
    {

    }
    break;
	case 900300:
	{

	}
	break;
	case 900200:
	{

	}
	break;
    case 50010:
    {
        if(m_pUAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialogLyWidgetDialog != nullptr){
            if(m_pUAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialogLyWidgetDialog->isVisible()){
                m_pUAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialogLyWidgetDialog->close();
            }
            else{
                m_pUAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialogLyWidgetDialog->show();
                m_pUAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialogLyWidgetDialog->raise();
            }
        }
    }
    break;
    case 50009:
    {
        if(m_pUAVFlyManage_MavlinkTaskManageLyWidgetDialog != nullptr){
            if(m_pUAVFlyManage_MavlinkTaskManageLyWidgetDialog->isVisible()){
                m_pUAVFlyManage_MavlinkTaskManageLyWidgetDialog->close();
            }
            else{
                m_pUAVFlyManage_MavlinkTaskManageLyWidgetDialog->show();
                m_pUAVFlyManage_MavlinkTaskManageLyWidgetDialog->raise();
            }
        }
		if (m_pUAVFlyManage_MavlinkTaskManageLyWidgetDialog != nullptr) {
			m_pUAVFlyManage_MavlinkTaskManageDialog->switchTocurrentTask("");

		}
    }
    break;
    case 50011:
    {
       
    }
    break;
    case 60001:
    {
        if(m_pUAVFlyManageDialog != nullptr){
            m_pUAVFlyManageDialog->OnCommand(nID, lParam);
        }
    }
    break;
    default:
		if (m_pUAVFlyManageDialog != nullptr)
		{
			m_pUAVFlyManageDialog->hide();
		}
        break;
    }
    return false;
}

bool LY_UAVFlyControl_Mavlink::OnCreate( int wParam, QWidget *pTab )
{
	QSize screenSize = qApp->property("ly_screen_xy").toSize();
    switch(wParam)
    {
    case 0:
    {
        m_pTab = pTab;

        {
            if(!MavlinkApp()){
                QGCApplication* app = new QGCApplication(pTab);
                Q_CHECK_PTR(app);

            }
        }

        if(QGCApplication::qgcApplicationMainDialogShow()){
            LyDockWidget *pDock = LyDockManager::instance()->dockWidget("UAVFlyManage_MavlinkMainDialog");
            if (pDock == nullptr)
            {
                pDock = LyDockManager::instance()->createDockWidget(LyDockDef::Tool, UAVFlyManage_MavlinkMainDialog::tr("UAV Fly Manage Mavlink Main Dialog"), "UAVFlyManage_MavlinkMainDialog");
                LyDockManager::instance()->embedToolDockWidget(pDock, LyDockDef::Right);
            }

            if(m_pUAVFlyManage_MavlinkMainDialog == nullptr){
                m_pUAVFlyManage_MavlinkMainDialog = new UAVFlyManage_MavlinkMainDialog();
                m_pUAVFlyManage_MavlinkMainDialog->show();
                pDock->setCentralWidget(m_pUAVFlyManage_MavlinkMainDialog);
                pDock->setWindowTitle(UAVFlyManage_MavlinkMainDialog::tr("UAV Fly Manage Mavlink Main Dialog"));
                pDock->setWindowTitle("\xe6\x97\xa0\xe4\xba\xba\xe6\x9c\xba\xe9\xa3\x9e\xe8\xa1\x8c\xe7\xae\xa1\xe7\x90\x86\xe4\xb8\xbb\xe7\xaa\x97\xe5\x8f\xa3");
                LyDockWidget::connect(m_pUAVFlyManage_MavlinkMainDialog, &UAVFlyManage_MavlinkMainDialog::windowTitleChanged, pDock, &LyDockWidget::setWindowTitle);
            }
        }

        //if(QGCApplication::qgcApplicationMavlinkTaskManageDialogShow())
		{
            if(m_pUAVFlyManage_MavlinkTaskManageLyWidgetDialog == nullptr && m_pUAVFlyManage_MavlinkTaskManageDialog == nullptr){

                m_pUAVFlyManage_MavlinkTaskManageLyWidgetDialog = new LyDialog(tr2("航线上传"), pTab);
                m_pUAVFlyManage_MavlinkTaskManageDialog = new UAVFlyManage_MavlinkTaskManageDialog(m_pUAVFlyManage_MavlinkTaskManageLyWidgetDialog);
                if(m_pUAVFlyManage_MavlinkTaskManageLyWidgetDialog != nullptr && m_pUAVFlyManage_MavlinkTaskManageDialog != nullptr){

                    m_pUAVFlyManage_MavlinkTaskManageLyWidgetDialog->setResizable(true);
                    m_pUAVFlyManage_MavlinkTaskManageLyWidgetDialog->resize(m_pUAVFlyManage_MavlinkTaskManageDialog->size());
                    QVBoxLayout* verticalLayout = new QVBoxLayout();
                    if(verticalLayout){
                        verticalLayout->addWidget(m_pUAVFlyManage_MavlinkTaskManageDialog);
                        m_pUAVFlyManage_MavlinkTaskManageLyWidgetDialog->setLayout(verticalLayout);
                    }
                }
            }

        }

        if(QGCApplication::qgcApplicationMavlinkVehicleGuidedModeGotoLocationDialogShow()){
            if(m_pUAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialogLyWidgetDialog == nullptr && m_pUAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialog == nullptr){
                m_pUAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialogLyWidgetDialog = new LyWidget(UAVFlyManage_MavlinkTaskManageDialog::tr("Guided Mode Goto Location"), pTab);
                m_pUAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialog = new UAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialog(m_pUAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialogLyWidgetDialog);
                if(m_pUAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialogLyWidgetDialog != nullptr && m_pUAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialog != nullptr){
                    m_pUAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialogLyWidgetDialog->setResizable(true);
                    m_pUAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialogLyWidgetDialog->resize(m_pUAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialog->size());
                    QVBoxLayout* verticalLayout = new QVBoxLayout();
                    if(verticalLayout){
                        verticalLayout->addWidget(m_pUAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialog);
                        m_pUAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialogLyWidgetDialog->setLayout(verticalLayout);
                    }
                }
            }

        }
    }
    break;
    case 1000:
    {


        {
            if(!MavlinkApp()){
                QGCApplication* app = new QGCApplication(pTab);
                Q_CHECK_PTR(app);

            }
        }

        if(QGCApplication::qgcApplicationMavlinkSelectDialogShow())
        {

            if(m_pUAVSelectDialog == nullptr){
                m_pUAVSelectDialog = new UAVSelectDialog(pTab);
                m_pUAVSelectDialog->InitWeaponInfoList();

            }
        }

        if(QGCApplication::qgcApplicationMavlinkControlDialogShow())
        {
           /* LyDockWidget *pDock = LyDockManager::instance()->dockWidget("UAVFlyManageMavlinkDialog");
            if (pDock == nullptr)
            {
                pDock = LyDockManager::instance()->createDockWidget(LyDockDef::Tool, UAVFlyManage_MavlinkDialog::tr("UAV Flight Control Mavlink Mode"), "UAVFlyManageMavlinkDialog");
                LyDockManager::instance()->embedToolDockWidget(pDock, LyDockDef::Right);
            }*/

            if(m_pUAVFlyManageDialog == nullptr){
                m_pUAVFlyManageDialog = new UAVFlyManage_MavlinkDialog(pTab);
				//m_pUAVFlyManageDialog->move(QPoint(screenSize.width() -770,80)/*1155, 60*/);
				//m_pUAVFlyManageDialog->setFixedSize(QSize(770,screenSize.height() - 80/* 756, 1020*/));
				m_pUAVFlyManageDialog->move(QPoint(screenSize.width() * 1/4, screenSize.height()* 2/3)/*1155, 60*/);
				m_pUAVFlyManageDialog->resize(screenSize.width() * 3/4 , screenSize.height() * 1/ 3/* 756, 1020*/);
             /*   pDock->setCentralWidget(m_pUAVFlyManageDialog);
                pDock->setWindowTitle(UAVFlyManage_MavlinkDialog::tr("UAV Flight Control Mavlink Mode"));
                pDock->setWindowTitle("\xe6\x97\xa0\xe4\xba\xba\xe6\x9c\xba\xe6\xa8\xa1\xe5\xbc\x8f/\xe6\x8e\xa7\xe5\x88\xb6");
                LyDockWidget::connect(m_pUAVFlyManageDialog, &UAVFlyManage_MavlinkDialog::windowTitleChanged, pDock, &LyDockWidget::setWindowTitle);*/
                if(m_pUAVFlyManageDialog != nullptr && m_pUAVSelectDialog != nullptr)
				{
                    m_pUAVFlyManageDialog->setVehicleSelectedDialog(m_pUAVSelectDialog);
                }
				m_pUAVFlyManageDialog->hide();
            }
        }

        if(QGCApplication::qgcApplicationMessageIndicatorDialogShow())
        {
            LyDockWidget *pDock = LyDockManager::instance()->dockWidget("UAVFlyManage_MavlinkMessageIndicatorDialog");
            if (pDock == nullptr)
            {
                pDock = LyDockManager::instance()->createDockWidget(LyDockDef::Tool, UAVFlyManage_MavlinkMessageIndicatorDialog::tr("UAV Fly Manage Mavlink Message Indicator"), "UAVFlyManage_MavlinkMessageIndicatorDialog");
                LyDockManager::instance()->embedToolDockWidget(pDock, LyDockDef::Right);
            }

            if(m_pUAVFlyManage_MavlinkMessageIndicatorDialog == nullptr){
                m_pUAVFlyManage_MavlinkMessageIndicatorDialog = new UAVFlyManage_MavlinkMessageIndicatorDialog();
                pDock->setCentralWidget(m_pUAVFlyManage_MavlinkMessageIndicatorDialog);

                pDock->setWindowTitle("\xe6\x97\xa0\xe4\xba\xba\xe6\x9c\xba\xe6\xb6\x88\xe6\x81\xaf\xe6\x98\xbe\xe7\xa4\xba");
                LyDockWidget::connect(m_pUAVFlyManage_MavlinkMessageIndicatorDialog, &UAVFlyManage_MavlinkMessageIndicatorDialog::windowTitleChanged, pDock, &LyDockWidget::setWindowTitle);
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
    case 4: break;
    }
    return false;
}

bool LY_UAVFlyControl_Mavlink::OnTimer( int wParam )
{
    switch (wParam)
    {
    case 1:
        break;
    case 2:
        if (m_fireProt.UpdateRoute)
        {
            m_fireProt.UpdateRoute = false;
            m_pUAVFlyManage_MavlinkTaskManageDialog->AutoScriptToRoute();
        }
        else if (m_fireProt.UpDownRroute)
        {
            bool mission = m_pUAVFlyManage_MavlinkTaskManageDialog->GetUAVLinkMissionManager();
            if (mission)
            {
                m_pUAVFlyManage_MavlinkTaskManageDialog->AutoScriptToUpDown();
                m_fireProt.UpDownRroute = false;
            }
        }
        break;
    case 3:
        break;
    }

    return false;
}

bool LY_UAVFlyControl_Mavlink::OnNetRecv(int , char * )
{

    return false;
}

bool LY_UAVFlyControl_Mavlink::NetServerClientSend(const char * const , const unsigned int , const QString & )
{
    return true;
}

void LY_UAVFlyControl_Mavlink::SetScreenCenter(const double& dLon, const double& dLat)
{
    if(!m_pProj){
        return ;
    }

    if(m_pProj){
        QString str = "CenterAt(" + QString::number(dLon,'f', 8) + "," + QString::number(dLat,'f', 8) + ")";
        LY_UAVFlyControl_MavlinkInterface::Get().Script(str.toLocal8Bit());
    }

}

QWidget *LY_UAVFlyControl_Mavlink::getPTab() const
{
    return m_pTab;
}

const UAVSelectDialog *LY_UAVFlyControl_Mavlink::getPUAVSelectDialog() const
{
    return m_pUAVSelectDialog;
}

const UAVFlyManage_MavlinkDialog *LY_UAVFlyControl_Mavlink::getPUAVFlyManageDialog() const
{
    return m_pUAVFlyManageDialog;
}

UAVFlyManage_MavlinkMessageIndicatorDialog *LY_UAVFlyControl_Mavlink::getPUAVFlyManage_MavlinkMessageIndicatorDialog() const
{
    return m_pUAVFlyManage_MavlinkMessageIndicatorDialog;
}

UAVFlyManage_MavlinkTaskManageDialog * LY_UAVFlyControl_Mavlink::getPUAVFlyManage_MavlinkTaskManageDialog() const
{
    return m_pUAVFlyManage_MavlinkTaskManageDialog;
}

void LY_UAVFlyControl_Mavlink::setChangeUAVFlyManage_MavlinkTaskManageDialog(QString str)
{
    if (m_pUAVFlyManage_MavlinkTaskManageDialog != nullptr)
    {
        m_pUAVFlyManage_MavlinkTaskManageDialog->changeLineUpWidgetToStr_like(str);
    }
}

