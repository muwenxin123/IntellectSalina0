#include "Ly_VideoRealTimeContainterWgt.h"
#include "ui_Ly_VideoRealTimeContainterWgt.h"

#include <QThread>
#include <QPainter>
#include<QStyleOption>
#include<QResizeEvent>
#include<QMouseEvent>
#include <QGeoCoordinate>

#include "VedioContainer.h"
#include "VedioSDLContainer.h"
#include "EntityVedioData.h"
#include  "Ly_VideoRealTimePreview.h"
#include  "IVideoRealTimePreviewActivator.h"

#define  WIDTH_SCAL_HEIGHT  (2.0/3.0)
#define  HEIGHT_SCAL_WIDTH  (3.0/2.0)

Ly_VideoRealTimeContainterWgt::Ly_VideoRealTimeContainterWgt(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Ly_VideoRealTimeContainterWgt)
	, m_pThread(new SortViewThread())
{
    ui->setupUi(this);
	setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
	setAttribute(Qt::WA_TranslucentBackground, true);

	SetActiveBtnVisiable(false);
	Init();

	connect(ui->LeftBtn, &QPushButton::clicked, this, &Ly_VideoRealTimeContainterWgt::onReduceBtnClicked);
	connect(ui->topBtn, &QPushButton::clicked, this, &Ly_VideoRealTimeContainterWgt::onReduceBtnClicked);
	connect(ui->rightBtn, &QPushButton::clicked, this, &Ly_VideoRealTimeContainterWgt::onAddBtnClicked);
	connect(ui->downBtn, &QPushButton::clicked, this, &Ly_VideoRealTimeContainterWgt::onAddBtnClicked);
	bool isOk = connect(m_pThread, &SortViewThread::onViewOrderChanged, this, &Ly_VideoRealTimeContainterWgt::onIdSortChanged, Qt::QueuedConnection);
	isOk = connect(m_pThread, &SortViewThread::entityStatusChanged, this, &Ly_VideoRealTimeContainterWgt::onEntityStatusChanged, Qt::QueuedConnection);
	connect(this, &Ly_VideoRealTimeContainterWgt::configureChanged, m_pThread, &SortViewThread::onConfigureChanged, Qt::QueuedConnection);
	connect(this, &Ly_VideoRealTimeContainterWgt::focalZoomChanged, m_pThread, &SortViewThread::onFocalZoomChanged, Qt::QueuedConnection);
	connect(this, &Ly_VideoRealTimeContainterWgt::cameraAttributeChanged, m_pThread, &SortViewThread::onCameraAttributeChanged, Qt::QueuedConnection);
}

Ly_VideoRealTimeContainterWgt::~Ly_VideoRealTimeContainterWgt()
{
	m_idList.clear();

	if (nullptr != m_pThread)
	{
		delete m_pThread;
		m_pThread = nullptr;
	}

	QMap<long long, ViewInfo> dataMap;
	dataMap.swap(m_id2ViewMap);
	m_id2ViewMap.clear();
	for (auto iter_wgt = dataMap.begin(); iter_wgt != dataMap.end(); iter_wgt++)
	{
		iter_wgt->pWgt->disconnect();
		delete (iter_wgt->pWgt);
	}
	dataMap.clear();

	delete ui;
}

// 初始化
void Ly_VideoRealTimeContainterWgt::Init()
{
	GetResizeInfo();
	onTimeOut();
}

// 是否包含窗口
bool Ly_VideoRealTimeContainterWgt::HasWgt()
{
	return !m_id2ViewMap.isEmpty();
}

// 事件到达
void Ly_VideoRealTimeContainterWgt::onTimeOut()
{
	// 更新现有的
	for (auto iter_wgt = m_id2ViewMap.begin(); iter_wgt != m_id2ViewMap.end(); iter_wgt++)
	{
		auto pWgt = iter_wgt->pWgt;
		if (!pWgt->isHidden())
			emit pWgt->updateContainter();
	}
}

void Ly_VideoRealTimeContainterWgt::SetContainerDirect(const EDirectType& type)
{
	if (m_type != type)
	{
		m_type = type;
		SetActiveBtnVisiable(false);
		GetResizeInfo();
		UpdateDisplay();
	}
}
// 事件
bool Ly_VideoRealTimeContainterWgt::event(QEvent *event)
{
	return QWidget::event(event);
}

// 事件过滤器
bool Ly_VideoRealTimeContainterWgt::eventFilter(QObject *object, QEvent *event)
{
	return QWidget::eventFilter(object, event);
}

void Ly_VideoRealTimeContainterWgt::resizeEvent(QResizeEvent *event)
{
	QWidget::resizeEvent(event);
	GetResizeInfo();
	UpdateDisplay();
}

QSize Ly_VideoRealTimeContainterWgt::GetChildWidgetSize()
{
	QSize parentWgtSize = ui->widget->size();

	QSize childSize;
	switch (m_type)
	{
	case Ly_VideoRealTimeContainterWgt::Horizontal:{
		int curWidth = parentWgtSize.height() *HEIGHT_SCAL_WIDTH;
		childSize.setWidth(qMin(curWidth, parentWgtSize.width()));
		childSize.setHeight(parentWgtSize.height());
	}break;
	case Ly_VideoRealTimeContainterWgt::Vertical:
	{
		int curHeight = parentWgtSize.width() *WIDTH_SCAL_HEIGHT;
		childSize.setWidth(parentWgtSize.width());
		childSize.setHeight(qMin(parentWgtSize.height(), curHeight));
	}break;
	default:
		break;
	}

	return childSize;
}

int Ly_VideoRealTimeContainterWgt::GetChildMaxWidgetSize(QSize& childSize)
{
	int cnt = -1;
	if (!childSize.isValid())
		return cnt;
	switch (m_type)
	{
	case Ly_VideoRealTimeContainterWgt::Horizontal: {
		cnt =  ui->widget->size().width() / qMax(1, childSize.width());
	}break;
	case Ly_VideoRealTimeContainterWgt::Vertical:
	{
		cnt = ui->widget->size().height() / (qMax(1,childSize.height()));
	}break;
	default:
		break;
	}

	cnt = qMax(cnt, 1);
	return cnt;
}

// 更新显示
void Ly_VideoRealTimeContainterWgt::UpdateDisplay()
{
	// 窗口数量时 0 
	if (m_id2ViewMap.size() == 0)
	{ 
		m_curShowChildIndex = -1;
		SetActiveBtnVisiable(false);
		return;
	}

	if (m_curMaxShowChildSize <= 0)
	{
		for (auto& entity : m_id2ViewMap)
		{
			if (!entity.pWgt->isHidden())
			{
				entity.pWgt->hide();
			}
		}
		return;
	}

	// 窗口数量小于最大窗口数量以及 窗口
	if (m_id2ViewMap.size() < m_curMaxShowChildSize || m_curShowChildIndex == -1)
		m_curShowChildIndex = 0;
	
	// 窗口大小小于最大大小时， 尽量居中显示
	if (m_id2ViewMap.size() < m_curMaxShowChildSize)
		UpdateDisplayTryCenter();
	else
		UpdateDisplayCommon();
	SetActiveBtnVisiable(m_id2ViewMap.size() > m_curMaxShowChildSize);
}

// 更新显示
void Ly_VideoRealTimeContainterWgt::UpdateDisplayTryCenter()
{
	switch (m_type)
	{
	case Ly_VideoRealTimeContainterWgt::Horizontal:
		UpdateDisplayTryCenterH();
		break;
	case Ly_VideoRealTimeContainterWgt::Vertical:
		UpdateDisplayTryCenterV();
		break;
	default:
		break;
	}
}

void Ly_VideoRealTimeContainterWgt::UpdateDisplayTryCenterH()
{
	QSize curSize = ui->widget->size();

	int left = (curSize.width() - m_curChildWgtSize.width() * m_id2ViewMap.size()) / 2;
	for (auto& entity  : m_idList)
	{
		auto iter = m_id2ViewMap.find(entity);
		if (iter != m_id2ViewMap.end())
		{
			iter->pWgt->setFixedSize(m_curChildWgtSize);
			iter->pWgt->move(left, 0);
			if (iter->pWgt->isHidden())
				iter->pWgt->show();
			left = left + m_curChildWgtSize.width();
		}
	}
}

void Ly_VideoRealTimeContainterWgt::UpdateDisplayTryCenterV()
{
	QSize curSize = ui->widget->size();
	int height = (curSize.height() - m_curChildWgtSize.height() * m_id2ViewMap.size()) / 2;
	for (auto& entity : m_idList)
	{
		auto iter = m_id2ViewMap.find(entity);
		if (iter != m_id2ViewMap.end())
		{
			iter->pWgt->setFixedSize(m_curChildWgtSize);
			iter->pWgt->move(0, height);
			if (iter->pWgt->isHidden())
				iter->pWgt->show();
			height = height + m_curChildWgtSize.height();
		}
	}
}

void Ly_VideoRealTimeContainterWgt::UpdateDisplayCommon()
{
	switch (m_type)
	{
	case Ly_VideoRealTimeContainterWgt::Horizontal:
		UpdateDisplayCommonH();
		break;
	case Ly_VideoRealTimeContainterWgt::Vertical:
		UpdateDisplayCommonV();
		break;
	default:
		break;
	}
}

void Ly_VideoRealTimeContainterWgt::UpdateDisplayCommonH()
{
	QSize curSize = ui->widget->size();
	int left = (curSize.width() - m_curChildWgtSize.width() * m_curMaxShowChildSize) / 2;
	if (ui->LeftBtn->isVisible()) left += ui->LeftBtn->width();

	int i = 0;
	for (auto& entity : m_idList)
	{
		auto iter = m_id2ViewMap.find(entity);
		if (iter != m_id2ViewMap.end())
		{
			auto& entity = *iter;
			if (i < m_curShowChildIndex)
			{
				if (!entity.pWgt->isHidden())
					entity.pWgt->hide();
			}
			else if (i < m_curShowChildIndex + m_curMaxShowChildSize)
			{
				// 在 m_curShowChildIndex ~m_curShowChildIndex + m_curMaxShowChildSize 显示， 其他隐藏
				entity.pWgt->setFixedSize(m_curChildWgtSize);
				entity.pWgt->move(left, 0);
				if (entity.pWgt->isHidden())
					entity.pWgt->show();
				left = left + m_curChildWgtSize.width();
			}
			else {
				if (!entity.pWgt->isHidden())
					entity.pWgt->hide();
			}
			i++;
		}
	}
}

void Ly_VideoRealTimeContainterWgt::UpdateDisplayCommonV()
{
	QSize curSize = ui->widget->size();
	int height = (curSize.height() - m_curChildWgtSize.height() * m_curMaxShowChildSize) / 2;
	if (ui->topBtn->isVisible()) height += ui->topBtn->height();

	int i = 0;
	for (auto& entity : m_idList)
	{
		auto iter = m_id2ViewMap.find(entity);
		if (iter != m_id2ViewMap.end())
		{
			auto& entity = *iter;
			if (i < m_curShowChildIndex)
			{
				if (!entity.pWgt->isHidden())
					entity.pWgt->hide();
			}
			else if (i < m_curShowChildIndex + m_curMaxShowChildSize)
			{
				entity.pWgt->setFixedSize(m_curChildWgtSize);
				entity.pWgt->move(0, height);
				if (entity.pWgt->isHidden())
					entity.pWgt->show();
				height = height + m_curChildWgtSize.height();
			}
			else
			{
				if (!entity.pWgt->isHidden())
					entity.pWgt->hide();
			}
			i++;
		}
	}
}

void Ly_VideoRealTimeContainterWgt::GetResizeInfo()
{
	m_curChildWgtSize = GetChildWidgetSize();						// 当前子窗口的size
	m_curMaxShowChildSize = GetChildMaxWidgetSize(m_curChildWgtSize);			// 当前窗口大小一次能显示的视频的窗
}

void Ly_VideoRealTimeContainterWgt::SetActiveBtnVisiable(bool Visiable)
{
	switch (m_type)
	{
	case Ly_VideoRealTimeContainterWgt::Horizontal:
	{
		ui->topBtn->hide();
		ui->downBtn->hide();

		ui->LeftBtn->setVisible(Visiable);
		ui->rightBtn->setVisible(Visiable);
	}
		break;
	case Ly_VideoRealTimeContainterWgt::Vertical:
	{
		ui->topBtn->setVisible(Visiable);
		ui->downBtn->setVisible(Visiable);

		ui->LeftBtn->hide();
		ui->rightBtn->hide();
	}
		break;
	default:
		break;
	}
}

// 左键切换被按下
void Ly_VideoRealTimeContainterWgt::onReduceBtnClicked()
{
	m_curShowChildIndex--;
	if (m_curShowChildIndex < 0 && !m_id2ViewMap.isEmpty())
		m_curShowChildIndex = 0;

	UpdateDisplay();
}

// 右键切换按钮本按下
void Ly_VideoRealTimeContainterWgt::onAddBtnClicked()
{
	m_curShowChildIndex++;
	if (m_curShowChildIndex >= m_id2ViewMap.size() - m_curMaxShowChildSize)
		m_curShowChildIndex = m_id2ViewMap.size() - m_curMaxShowChildSize;

	UpdateDisplay();
}

void Ly_VideoRealTimeContainterWgt::onIdSortChanged(QList<long long> ids)
{
	if (!ids.isEmpty() && m_idList != ids)
	{
		m_idList = ids;
		UpdateDisplay();
	}
}

void Ly_VideoRealTimeContainterWgt::onEntityStatusChanged(QList<long long> addIdList, QList<long long> deleteIdList)
{
	bool isStatusChanged = false;
	for (auto id : addIdList)
	{
		if (!m_id2ViewMap.contains(id))
		{
			EntityVedioDataSPtr ptr = std::make_shared<EntityVedioData>(id);
			EntityDataManage::Get().AddEntity(ptr);
			ViewInfo info;
			info.identity = id;
			info.pWgt = new VedioContainer(ptr, this);
			info.pWgt->show();
			info.pWgt->hide();
			connect(info.pWgt, &VedioContainer::entityDoubleClicked, this, &Ly_VideoRealTimeContainterWgt::entityDoubleClicked);
			emit info.pWgt->updateContainter();
			m_id2ViewMap.insert(info.identity, info);
			m_idList.push_back(info.identity);
			isStatusChanged = true;
		}
	}

	for (auto id : deleteIdList)
	{
		auto iter = m_id2ViewMap.find(id);
		if (iter != m_id2ViewMap.end())
		{
			EntityDataManage::Get().RemoveEntity(id);
			auto pWgt = iter->pWgt;
			iter = m_id2ViewMap.erase(iter);
			m_idList.removeOne(id);

			pWgt->hide();
			pWgt->disconnect();
			pWgt->deleteLater();
			isStatusChanged = true;
		}
	}

	if (isStatusChanged)
	{
		UpdateDisplay();
		emit childWgtSizeChanged();
	}
}

void Ly_VideoRealTimeContainterWgt::paintEvent(QPaintEvent *event)
{
	return QWidget::paintEvent(event);
}
