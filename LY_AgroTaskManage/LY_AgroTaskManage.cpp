
#include "lY_AgroTaskManage.h"
#include "lY_AgroTaskManageInterface.h"
#include <IFunctionInterface.h>
#include "IProj.h"
#include "lyUI/LyDockManager.h"
#include "CreateShape.h"
#include <QSettings>
#include "IAgroTaskManagerActivator.h"
#include "QTabWidget"

Q_DECLARE_METATYPE(qnzkna::framework::IRegistry *)

#define  LINE_LENGTH			3					//描画用，航点宽度
#define  LINE_LENGTH_JUDGE		6					//判断用，选中宽度
#define  DRAW_SCAN_HZ			2

static CGetHeight *s_pCGetHeight = nullptr;
CGetHeight LY_AgroTaskManage::s_CGetHeight("../data/dem/5-0");


void LY_AgroTaskManage::X_S(double x, double y, float *l, float *b)
{
	if (m_pProj != nullptr && x < 1000000000. && x > -1000000000.)
	{
		int px, py;
		m_pProj->xyLtoD(x, y, &px, &py);
		*l = (float)px;
		*b = (float)py;
	}
}

void LY_AgroTaskManage::S_X(float x, float y, double *xx, double *yy)
{
	QPoint pt;
	pt.setX((int)x);
	pt.setY((int)y);
	if (m_pProj != nullptr)
	{
		m_pProj->Screen2Meter(pt, *xx, *yy);
	}
}

void LY_AgroTaskManage::L_X(double l, double b, double *x, double *y)
{
	float x1 = 0, y1 = 0;
	if (m_pProj != nullptr)
	{
		m_pProj->jwxy(l, b, x, y);
	}
}

void LY_AgroTaskManage::X_L(double x, double y, double *l, double *b)
{
	float l1 = 0, b1 = 0;
	if (m_pProj != nullptr)
	{
		m_pProj->xyjw(x, y, l, b);
	}
}

void LY_AgroTaskManage::L_S(double x, double y, int *sx, int *sy)
{
	if (m_pProj == nullptr)
	{
		return;
	}
	double zx, zy;
	m_pProj->jwxy((float)x, (float)y, &zx, &zy);
	m_pProj->xyLtoD((float)zx, (float)zy, sx, sy);
}

void LY_AgroTaskManage::S_L(int sx, int sy, double *x, double *y)
{
	QPoint pt;
	pt.setX((int)sx);
	pt.setY((int)sy);
	double x1 = 0, y1 = 0;
	if (m_pProj != nullptr)
	{
		m_pProj->Screen2Meter(pt, x1, y1);
		m_pProj->xyjw((float)x1, (float)y1, x, y);
	}
	else
	{
		return;
	}
}

LY_AgroTaskManage::LY_AgroTaskManage()
{
    m_pProj										= nullptr;
    m_pWnd										= nullptr;
    m_pTroopLabel							= nullptr;

	m_pDlgTaskSchedule					= nullptr;
	m_pDlgTaskEventRecord				= nullptr;
	m_pDlgAgroTaskManage			= nullptr;
	m_pDlgAgroTaskManageEdit		= nullptr;
	m_pDlgCropGrowStageWidget	= nullptr;
	m_pDlgDebug								= nullptr;
	m_pDlgAgroParameterShow		= nullptr;
	m_pDlgAgroSampleParaShow	= nullptr;
	m_pDlgAgroManureParaShow	= nullptr;
	m_pCDlgScanParaShow				= nullptr;
}


void LY_AgroTaskManage::Startup()
{
    IFunctionInterface &ifi = LY_AgroTaskManageInterface::Get().GetFunctionInterface();
    QVariant vt = ifi.Invoke("Ly.IRegistryImpl", 0);
    if (vt.canConvert<qnzkna::framework::IRegistry *>())
    {
        qnzkna::framework::IRegistry *pIRegistry = vt.value<qnzkna::framework::IRegistry *>();
        if (pIRegistry != nullptr)
        {
            IAgroTaskManagerActivator::Get().start(pIRegistry);
        }
    }
}

void LY_AgroTaskManage::Shutdown()
{
    IFunctionInterface &ifi = LY_AgroTaskManageInterface::Get().GetFunctionInterface();
    QVariant vt = ifi.Invoke("Ly.IRegistryImpl", 0);
    if (vt.canConvert<qnzkna::framework::IRegistry *>())
    {
        qnzkna::framework::IRegistry *pIRegistry = vt.value<qnzkna::framework::IRegistry *>();
        if (pIRegistry != nullptr)
        {
            IAgroTaskManagerActivator::Get().stop(pIRegistry);
        }
    }
}

void LY_AgroTaskManage::Reset()
{

}

bool LY_AgroTaskManage::DrawGraph(QPainter &painter, IProj *pProj)
{
    return true;
}

bool LY_AgroTaskManage::DrawDynamic(QPainter &painter, IProj *pProj)
{
	if (m_pDlgAgroParameterShow != nullptr)
	{
		m_pDlgAgroParameterShow->draw2DPathLine(painter, pProj);
	}

	if (m_pDlgAgroManureParaShow != nullptr)
	{
		m_pDlgAgroManureParaShow->draw2DPathLine(painter, pProj);
	}

	if (m_pCDlgScanParaShow && !m_pCDlgScanParaShow->isHidden() && m_pCDlgScanParaShow->isShow_path) 
	{
		m_pCDlgScanParaShow->DrawDynamic(&painter, pProj);
	}
    return false;
}

bool LY_AgroTaskManage::OnMouseButtonDown(QMouseEvent *e)
{
    return false;
}

bool LY_AgroTaskManage::OnMouseButtonRelease(QMouseEvent *e)
{
    return false;
}

bool LY_AgroTaskManage::OnMouseMove(QMouseEvent *e)
{
    return false;
}

bool LY_AgroTaskManage::OnMouseDoubleClick(QMouseEvent *e)
{
    return false;
}

bool LY_AgroTaskManage::OnCommand(int nID)
{
	switch (nID)
	{
	case 433390: {//智慧农业任务管理
// 		m_pDlgAgroManureParaShow->raise();
// 		m_pDlgAgroManureParaShow->show();
	}break;
	default:
		break;
	}
    return false;
}

bool LY_AgroTaskManage::OnCreate(int wParam, QTabWidget *pTab)
{

	if (m_pDlgTaskSchedule == NULL)
	{
		m_pDlgTaskSchedule = new CDlgTaskSchedule(pTab);
		m_pDlgTaskSchedule->hide();
	}

	if (m_pDlgTaskEventRecord == NULL)
	{
		m_pDlgTaskEventRecord = new CDlgTaskEventRecord(pTab);
		m_pDlgTaskEventRecord->hide();
	}

	if (m_pDlgAgroTaskManage == NULL)
	{
		m_pDlgAgroTaskManage = new CDlgAgroTaskManage(pTab);
		m_pDlgAgroTaskManage->hide();
	}

	if (m_pDlgAgroTaskManageEdit == NULL)
	{
		m_pDlgAgroTaskManageEdit = new CDlgAgroTaskManageEdit(pTab);
		m_pDlgAgroTaskManageEdit->hide();
	}

	if (m_pDlgCropGrowStageWidget == NULL)
	{
		m_pDlgCropGrowStageWidget = new CDlgCropGrowStageWidget(pTab);
		m_pDlgCropGrowStageWidget->hide();
	}

	if (m_pDlgAgroParameterShow == nullptr)
	{
		m_pDlgAgroParameterShow = new CDlgAgroParameterShow(pTab);
		m_pDlgAgroParameterShow->hide();
	}

	if (m_pDlgAgroSampleParaShow == nullptr)
	{
		m_pDlgAgroSampleParaShow = new CDlgAgroSampleParaShow(pTab);
		m_pDlgAgroSampleParaShow->hide();
	}

	if (m_pDlgAgroManureParaShow == nullptr)
	{
		m_pDlgAgroManureParaShow = new CDlgAgroManureParaShow(pTab);
		m_pDlgAgroManureParaShow->hide();
	}

	if (m_pCDlgScanParaShow == nullptr)
	{
		m_pCDlgScanParaShow = new CDlgScanParaShow(pTab);
		m_pCDlgScanParaShow->hide();
	}
    switch (wParam)
    {
    case 0:   //右
    {
		// 测试界面
		//LyDockWidget *pDockAgroDebug = LyDockManager::instance()->dockWidget("CDlgAgroDebug");
		//if (pDockAgroDebug == nullptr)
		//{
		//	pDockAgroDebug = LyDockManager::instance()->createDockWidget(LyDockDef::Tool, tr2("智慧农业测试界面"), "CDlgAgroDebug");
		//	LyDockManager::instance()->embedToolDockWidget(pDockAgroDebug, LyDockDef::Right);
		//}

		//if (m_pDlgDebug == nullptr)
		//{
		//	m_pDlgDebug = new CDlgDebug(pTab);
		//	pDockAgroDebug->setCentralWidget(m_pDlgDebug);
		//	m_pDlgDebug->setWindowTitle(tr2("智慧农业测试界面"));
		//}
    }
    break;
    case 1:   //左
    {

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

bool LY_AgroTaskManage::OnTimer(int wParam)
{
    switch (wParam)
    {
    case 1://200ms
        LY_AgroTaskManageInterface::Get().Refresh(1);
        break;
    case 2://1s
        break;
    case 3://60s
        break;
    }
    return true;
}

CDlgTaskSchedule* LY_AgroTaskManage::getPDlgTaskSchedule() const
{
	return m_pDlgTaskSchedule;
}

CDlgTaskEventRecord* LY_AgroTaskManage::getPDlgTaskEventRecord() const
{
	return m_pDlgTaskEventRecord;
}

CDlgAgroTaskManage* LY_AgroTaskManage::getPDlgAgroTaskManage() const
{
	return m_pDlgAgroTaskManage;
}

CDlgAgroTaskManageEdit* LY_AgroTaskManage::getPDlgAgroTaskManageEdit() const
{
	return m_pDlgAgroTaskManageEdit;
}

CDlgCropGrowStageWidget* LY_AgroTaskManage::getPDlgCropGrowStageWidget() const
{
	return m_pDlgCropGrowStageWidget;
}

CDlgAgroParameterShow* LY_AgroTaskManage::getCDlgAgroParameterShow() const
{
	return m_pDlgAgroParameterShow;
}

CDlgAgroSampleParaShow* LY_AgroTaskManage::getPDlgAgroSampleParaShow() const
{
	return m_pDlgAgroSampleParaShow;
}

CDlgAgroManureParaShow* LY_AgroTaskManage::getPDlgAgroManureParaShow() const
{
	return m_pDlgAgroManureParaShow;
}

CDlgScanParaShow* LY_AgroTaskManage::getPCDlgScanParaShow() const
{
	return m_pCDlgScanParaShow;
}
