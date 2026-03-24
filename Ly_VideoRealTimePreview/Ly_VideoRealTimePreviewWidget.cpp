#include "Ly_VideoRealTimePreviewWidget.h"
#include "ui_Ly_VideoRealTimePreviewWidget.h"

#include <chrono>
#include <QDebug>
#include <QPainter>
#include <QWindow>
#include <QStyleOption>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QMutexLocker>

#include "VedioDetailShowWgt.h"
#include "Ly_VideoRealTimeContainterWgt.h"
#include "Ly_VideoRealTimePreview.h"

MapEventWatcher::MapEventWatcher(Ly_VideoRealTimePreviewWidget* pWgt, QWidget* pParent)
	: QWidget(pParent)
	, m_pWgt(pWgt)
{
	resize(0, 0);
	this->hide();
}

MapEventWatcher::~MapEventWatcher()
{

}

bool MapEventWatcher::eventFilter(QObject *object, QEvent *event)
{
	QWidget* pWgt = dynamic_cast<QWidget*>(object);
	if ( nullptr != pWgt && pWgt == parentWidget())
	{
		int type = event->type();
		if (event->type() == QEvent::Resize)
		{
			QResizeEvent* pEvent = dynamic_cast<QResizeEvent*>(event);
			if (nullptr != pEvent)
			{
				QSize size = pEvent->size();
				m_pWgt->ResizeIt(size);
			}
		}
		else if (event->type() == QEvent::Move)
		{
			QMoveEvent* pEvent = dynamic_cast<QMoveEvent*>(event);
			if (nullptr != pEvent)
			{
				QTimer::singleShot(1, [&]() {
					QRect rect = parentWidget()->rect();
					m_pWgt->MoveIt(rect);
				});
			}
		}
		else if (event->type() == QEvent::Show)
		{
			m_pWgt->CheckShow();
			QTimer::singleShot(1, [&]() {
				QRect rect = parentWidget()->rect();
				m_pWgt->MoveIt(rect);
			});
		}
		else if (event->type() == QEvent::Hide)
		{
			m_pWgt->hide();
		}
		else if (event->type() == QEvent::ShowToParent)
		{
			m_pWgt->CheckShow();
		}
		else if (event->type() == QEvent::HideToParent)
		{
			m_pWgt->hide();
		}
	}
	return QWidget::eventFilter(object, event);
}

Ly_VideoRealTimePreviewWidget::Ly_VideoRealTimePreviewWidget(QWidget *parent) :
	m_pParentWidget(parent),
    ui(new Ui::Ly_VideoRealTimePreviewWidget)
{
    ui->setupUi(this);

	// 初始化界面
	setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
	setAttribute(Qt::WA_TranslucentBackground, true);
	setAttribute(Qt::WA_TransparentForMouseEvents, true);
	m_pContainer = new Ly_VideoRealTimeContainterWgt;
	ui->horizontalLayout->addWidget(m_pContainer);
	connect(m_pContainer, &Ly_VideoRealTimeContainterWgt::childWgtSizeChanged, this, &Ly_VideoRealTimePreviewWidget::onVedioCountChanged);
	connect(m_pContainer, &Ly_VideoRealTimeContainterWgt::entityDoubleClicked, this, &Ly_VideoRealTimePreviewWidget::onEntityDoubleClicked);
	
	// 初始化配置
	bool showFlag = false;
	VideoRealTime::EViewLocation location = VideoRealTime::ViewBottom;
	{
		QMutexLocker locker(&(Ly_VideoRealTimePreview::Get().getConfigMutex()));
		auto pConfig = Ly_VideoRealTimePreview::Get().getConfig();
		showFlag = pConfig->showFlag;
		location = pConfig->viewLocation;
		m_detailOccupy = pConfig->detailOccupyValue;
	}
	setViewLocation(location);
	m_showFlag = showFlag;

	// 地图窗口的监视添加
	m_wathcer = new MapEventWatcher(this, m_pParentWidget);
	if (nullptr != m_pParentWidget)
	{
		connect(m_pParentWidget, &QObject::destroyed, this, &Ly_VideoRealTimePreviewWidget::onDeleteLater);
		m_pParentWidget->installEventFilter(m_wathcer);
	}

	// 细节显示窗口
	m_pVedioDetailShow = new VedioDetailShowWgt(this);
	m_pVedioDetailShow->hide();
	this->stackUnder(m_pVedioDetailShow);

	// 定时刷新
	connect(&m_timer, &QTimer::timeout, this, &Ly_VideoRealTimePreviewWidget::onTimeArrive);
	m_timer.start(150);
}

Ly_VideoRealTimePreviewWidget::~Ly_VideoRealTimePreviewWidget()
{
	if (m_timer.isActive())
		m_timer.stop();

	if (nullptr != m_pContainer)
	{
		delete m_pContainer;
		m_pContainer = nullptr;
	}

	if (nullptr != m_pVedioDetailShow)
	{
		delete m_pVedioDetailShow;
		m_pVedioDetailShow = nullptr;
	}

	if (nullptr != m_pDependentView)
		m_pDependentView->removeEventFilter(this);

	if (nullptr != m_wathcer)
	{
		delete m_wathcer;
		m_wathcer = nullptr;
	}

    delete ui;
}

void Ly_VideoRealTimePreviewWidget::setMapWidget(QWidget* pParent)
{
	if (nullptr != m_pParentWidget)
	{ 
		disconnect(m_pParentWidget, &QObject::destroyed, this, &Ly_VideoRealTimePreviewWidget::onDeleteLater);
		m_wathcer->setParent(nullptr);
		m_pParentWidget->removeEventFilter(m_wathcer);
	}

	if (nullptr != pParent)
	{
		m_pParentWidget = pParent;
		connect(m_pParentWidget, &QObject::destroyed, this, &Ly_VideoRealTimePreviewWidget::onDeleteLater);
		m_wathcer->setParent(m_pParentWidget);
		m_pParentWidget->installEventFilter(m_wathcer);
	}
}

void Ly_VideoRealTimePreviewWidget::setVisiableDependsWgt(QWidget* pParent)
{
	if (nullptr != pParent)
	{
		m_pDependentView = pParent;
		m_pDependentView->installEventFilter(this);
	}
}

void Ly_VideoRealTimePreviewWidget::ConfigureChanged()
{
	bool showFlag = false;
	VideoRealTime::EViewLocation location = VideoRealTime::ViewBottom;

	{
		QMutexLocker locker(&(Ly_VideoRealTimePreview::Get().getConfigMutex()));
		auto pConfig = Ly_VideoRealTimePreview::Get().getConfig();
		showFlag = pConfig->showFlag;
		location = pConfig->viewLocation;
		m_detailOccupy = pConfig->detailOccupyValue;
	}

	if (m_locationEnum != location)
		setViewLocation(location);

	if (m_showFlag != showFlag)
		m_showFlag = showFlag;

	if (nullptr != m_pContainer)
		emit m_pContainer->configureChanged();

	CheckShow();
	AdjustDetaiView();
}

// 初始化
void Ly_VideoRealTimePreviewWidget::Init()
{
	ResetByParent();
	CheckShow();
}

// 定时器事件到达
void Ly_VideoRealTimePreviewWidget::onTimeArrive()
{
//	static  double  s_maxCoustom = 0;
	auto start = std::chrono::high_resolution_clock::now();
	if (nullptr != m_pContainer)
		m_pContainer->onTimeOut();

	if (nullptr != m_pVedioDetailShow)
	{
		m_pVedioDetailShow->onTimeOut();
	}
	//double curCoustom = (std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start)).count();
	//s_maxCoustom = qMax(s_maxCoustom, curCoustom);
	//qDebug() << "setImage Coustom  Max: " << s_maxCoustom << "cur:" << curCoustom << QString::fromLocal8Bit("微秒");
}

void Ly_VideoRealTimePreviewWidget::setViewLocation(const VideoRealTime::EViewLocation& location)
{
	m_locationEnum = location;
	switch (m_locationEnum)
	{
	case VideoRealTime::ViewTop:
	case VideoRealTime::ViewBottom:
	{
		if (nullptr != m_pContainer)
		{
			
			m_pContainer->SetContainerDirect(Ly_VideoRealTimeContainterWgt::EDirectType::Horizontal);
		}
	}
		break;
	case VideoRealTime::ViewLeft:
	case VideoRealTime::ViewRight:
	{
		m_pContainer->SetContainerDirect(Ly_VideoRealTimeContainterWgt::EDirectType::Vertical);
	}
		break;
	default:
		break;
	}

	ResetByParent();
}

// 重设大小
void Ly_VideoRealTimePreviewWidget::ResizeIt(QSize& size)
{
	resize(GetViewSize(size));

	if (nullptr != m_pVedioDetailShow)
		m_pVedioDetailShow->resize(GetDetailViewSize(size));
}

// 移动
void Ly_VideoRealTimePreviewWidget::MoveIt(QRect& rect)
{
	move(GetViewPos(rect));
	if (nullptr != m_pVedioDetailShow)
		m_pVedioDetailShow->move(GetDetailViewPos(rect));
}

void Ly_VideoRealTimePreviewWidget::FocalZoomChanged(int imageZoom)
{
	if (nullptr != m_pContainer)
		emit m_pContainer->focalZoomChanged(imageZoom);
}

void Ly_VideoRealTimePreviewWidget::OberverCameraAttributeChanged(long long identity, 
	double yawValue, double pitchValue, double rollValue)
{
	if (nullptr != m_pContainer)
		emit m_pContainer->cameraAttributeChanged(identity, yawValue, pitchValue, rollValue);
}

void Ly_VideoRealTimePreviewWidget::AdjustDetaiView()
{
	QRect rect = m_pParentWidget->rect();
	if (nullptr != m_pVedioDetailShow) 
	{
		m_pVedioDetailShow->move(GetDetailViewPos(rect));
		m_pVedioDetailShow->resize(GetDetailViewSize(rect.size()));
		if (!m_pVedioDetailShow->isHidden())
		{
			m_pVedioDetailShow->raise();
		}
	}
}

void Ly_VideoRealTimePreviewWidget::UpdateMap()
{
	if (nullptr != m_pParentWidget)
	{
		m_pParentWidget->update();
	}
}

// 事件过滤器
bool Ly_VideoRealTimePreviewWidget::eventFilter(QObject *object, QEvent *event)
{
	QWidget* pWgt = dynamic_cast<QWidget*>(object);
	if (pWgt == m_pDependentView && nullptr != pWgt)
	{
		int type = event->type();
		switch (type)
		{
		//case QEvent::Enter:
		//case QEvent::Leave:
		//case QEvent::WindowActivate:
		//case QEvent::WindowDeactivate:
		//	CheckShow();
		//	break;
		case QEvent::Show:
		case QEvent::ShowToParent:
		{
			m_dependentViewShowFlag = true;
			CheckShow();
		}
			break;
		case QEvent::Hide:
		case QEvent::HideToParent:
		{
			m_dependentViewShowFlag = false;
			hide();
		}
		break;
		case QEvent::WindowStateChange:
		{
			if (pWgt->isMinimized())
			{
				m_isMiniSize = true;
				hide();
			}
			if (m_isMiniSize && !pWgt->isMinimized())
			{
				m_isMiniSize = false;
				CheckShow();
			}
		}
			break;
		default:
			break;
		}

		
	}
	return QWidget::eventFilter(object, event);
}

void Ly_VideoRealTimePreviewWidget::showEvent(QShowEvent *event)
{
	QWidget::showEvent(event);
}

QSize Ly_VideoRealTimePreviewWidget::GetViewSize(const QSize& parentSize)
{
	QSize size;
	switch (m_locationEnum)
	{
	case VideoRealTime::ViewLeft:
	case VideoRealTime::ViewRight:
	{
		size = QSize(parentSize.width() / 6, parentSize.height() - 20);
	}break;
	case VideoRealTime::ViewTop:
	case VideoRealTime::ViewBottom:
	default:
	{
		size = QSize(parentSize.width() - 20, parentSize.height() / 5);
	}
		break;
	}
	return size;
}

QPoint Ly_VideoRealTimePreviewWidget::GetViewPos(const QRect& rect)
{
	QPoint pt = GetViewRelatePos(rect);
	if (nullptr != m_pParentWidget)
		pt += m_pParentWidget->mapToGlobal(m_pParentWidget->pos());

	return pt;
}

QPoint Ly_VideoRealTimePreviewWidget::GetViewRelatePos(const QRect& rect)
{
	QPoint pt;
	switch (m_locationEnum)
	{
	case VideoRealTime::ViewLeft:
	{
		pt = QPoint(rect.left(), rect.top() + 10);
	}
	break;
	case VideoRealTime::ViewRight:
	{
		pt = QPoint(rect.width() * 5 / 6 , rect.top() + 10);
	}
	break;
	case VideoRealTime::ViewTop:
	{
		pt = QPoint(rect.left() + 10, rect.top());
	}
	break;
	case VideoRealTime::ViewBottom:
	default: {
		pt = QPoint(rect.left() + 10, rect.height() * 4 / 5);
	}
		break;
	}

	return pt;
}

QSize Ly_VideoRealTimePreviewWidget::GetDetailViewSize(const QSize& parentSize)
{
	QSize size;
	switch (m_locationEnum)
	{
	case VideoRealTime::ViewLeft:
	case VideoRealTime::ViewRight:
	{
		if (fabs(m_detailOccupy - 0.25) < 10e-6)
		{
			size = QSize(parentSize.width() / 2 - 10, parentSize.height() / 2 - 10);
		}
		else if (fabs(m_detailOccupy - 0.5) < 10e-6)
		{
			double scale = 1.0 / std::sqrt(2);
			size = QSize(int((parentSize.width() - 20) * scale),
				int((parentSize.height() - 20) * scale));
		}
		else if (fabs(m_detailOccupy - 0.8) < 10e-6)
		{
			size = QSize(int((parentSize.width() - 20) * m_detailOccupy),
				int((parentSize.height() - 20) * m_detailOccupy));
		}
		else if (fabs(m_detailOccupy - 1.0) < 10e-6)
		{
			size = QSize((parentSize.width() - 20), (parentSize.height() - 20));
		}
	}
	break;
	case VideoRealTime::ViewTop:
	case VideoRealTime::ViewBottom:
	{
		if (fabs(m_detailOccupy - 0.25) < 10e-6)
		{
			size = QSize(parentSize.width() / 2 - 10, parentSize.height() / 2 - 10);
		}
		else if (fabs(m_detailOccupy - 0.5) < 10e-6)
		{
			double scale = 1.0 / std::sqrt(2);
			size = QSize(int((parentSize.width() - 20) * scale),
				int((parentSize.height() - 20) * scale));
		}
		else if (fabs(m_detailOccupy - 0.8) < 10e-6)
		{
			size = QSize(int((parentSize.width() - 20) * m_detailOccupy),
				int((parentSize.height() - 20) * m_detailOccupy));
		}
		else if (fabs(m_detailOccupy - 1.0) < 10e-6)
		{
			size = QSize((parentSize.width()- 20), (parentSize.height() - 20));
		}
	}
	break;
	}
	return size;
}

QPoint Ly_VideoRealTimePreviewWidget::GetDetailViewPos(const QRect& rect)
{
	QPoint pt = GetDetailViewRelatePos(rect);
	if (nullptr != m_pParentWidget)
		pt += m_pParentWidget->mapToGlobal(m_pParentWidget->pos());

	return pt;
}

QPoint Ly_VideoRealTimePreviewWidget::GetDetailViewRelatePos(const QRect& rect)
{
	QPoint pt;
	switch (m_locationEnum)
	{
	case VideoRealTime::ViewLeft:
	{
		// 右下角
		if (fabs(m_detailOccupy - 0.25) < 10e-6)
		{
			pt = QPoint(rect.width() / 2, rect.height() / 2);
		}
		else if (fabs(m_detailOccupy - 0.5) < 10e-6)
		{
			double scale = 1.0 / std::sqrt(2);
			pt = QPoint((rect.width() - 20) * (1 - scale), (rect.height() - 20)*(1 - scale));
		}
		else if (fabs(m_detailOccupy - 0.8) < 10e-6)
		{
			pt = QPoint((rect.width() - 20) * (1 - m_detailOccupy), (rect.height() - 20)*(1 - m_detailOccupy));
		}
		else if (fabs(m_detailOccupy - 1.0) < 10e-6)
		{
			pt = QPoint(rect.left() + 10, rect.top() + 10);
		}
	}
	break;
	case VideoRealTime::ViewRight:
	{
		// 右下角
		if (fabs(m_detailOccupy - 0.25) < 10e-6)
		{
			pt = QPoint(rect.left() + 10, rect.height() / 2);
		}
		else if (fabs(m_detailOccupy - 0.5) < 10e-6)
		{
			double scale = 1.0 / std::sqrt(2);
			pt = QPoint(rect.left() + 10, (rect.height() - 20)*(1 - scale));
		}
		else if (fabs(m_detailOccupy - 0.8) < 10e-6)
		{
			pt = QPoint(rect.left() + 10, (rect.height() - 20)*(1 - m_detailOccupy));
		}
		else if (fabs(m_detailOccupy - 1.0) < 10e-6)
		{
			pt = QPoint(rect.left() + 10, rect.top() + 10);
		}
	}
	break;
	case VideoRealTime::ViewTop:
	{
		// 右下角
		if (fabs(m_detailOccupy - 0.25) < 10e-6)
		{
			pt = QPoint(rect.width() / 2, rect.height() / 2);
		}
		else if (fabs(m_detailOccupy - 0.5) < 10e-6)
		{
			double scale = 1.0 / std::sqrt(2);
			pt = QPoint((rect.width() - 20) * (1 - scale), (rect.height() - 20)*(1 - scale));

		}
		else if (fabs(m_detailOccupy - 0.8) < 10e-6)
		{
			pt = QPoint((rect.width() - 20) * (1 - m_detailOccupy), (rect.height() - 20)*(1 - m_detailOccupy));
		}
		else if (fabs(m_detailOccupy - 1.0) < 10e-6)
		{
			pt = QPoint(rect.left() + 10, rect.top() + 10);
		}
	}
	break;
	case VideoRealTime::ViewBottom:
	default: {
		pt = QPoint(rect.left() + 10, rect.top() + 10);
	}
	break;
	}

	return pt;
}

void Ly_VideoRealTimePreviewWidget::CheckShow()
{
	bool isShow = m_pContainer->HasWgt();

	bool isParentShow = false;
	if (nullptr != m_pDependentView)
		isParentShow = m_dependentViewShowFlag;

	if (isShow && isParentShow && m_showFlag && !m_isMiniSize)
	{
		show();
		raise();
	}
	else
		hide();
}

void Ly_VideoRealTimePreviewWidget::ResetByParent()
{
	if (nullptr != m_pParentWidget)
	{
		QRect rect = m_pParentWidget->rect();
		ResizeIt(rect.size());
		MoveIt(rect);
	}
}

void Ly_VideoRealTimePreviewWidget::onVedioCountChanged()
{
	CheckShow();
	if (nullptr != m_pVedioDetailShow && !m_pVedioDetailShow->isHidden())
	{
		m_pVedioDetailShow->raise();
	}
}

void Ly_VideoRealTimePreviewWidget::onDeleteLater(QObject* pObejct)
{
	if (pObejct == m_pParentWidget)
	{
		m_wathcer = nullptr;
		deleteLater();
	}
}

void Ly_VideoRealTimePreviewWidget::onEntityDoubleClicked(long long identity)
{
	if (nullptr != m_pVedioDetailShow)
	{
		m_pVedioDetailShow->SetEntityIdentity(identity);
	}
}

void Ly_VideoRealTimePreviewWidget::paintEvent(QPaintEvent *event)
{
	QPainter p(this);
	QStyleOption opt;
	opt.init(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
