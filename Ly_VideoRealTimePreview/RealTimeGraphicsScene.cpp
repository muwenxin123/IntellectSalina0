#include "RealTimeGraphicsScene.h"

#include <QGraphicsView>
#include <QGraphicsRotation>
#include <QGraphicsProxyWidget>
#include "RealTimeGraphicsProxyWidget.h"

#include "VedioContainer.h"
#include "VedioSDLContainer.h"
#include "EntityVedioData.h"
#include  "IVideoRealTimePreviewActivator.h"

#define  WIDTH_SCAL_HEIGHT  (2.0/3.0)
#define  HEIGHT_SCAL_WIDTH  (3.0/2.0)
#define WGT_INTERFACE 20

RealTimeGraphicsScene::RealTimeGraphicsScene(QObject *parent)
	: m_pThread(new SortViewThread())
{
	bool isOk = connect(m_pThread, &SortViewThread::onViewOrderChanged, this, &RealTimeGraphicsScene::SortList, Qt::QueuedConnection);
	isOk = connect(m_pThread, &SortViewThread::entityStatusChanged, this, &RealTimeGraphicsScene::onEntityStatusChanged, Qt::QueuedConnection);
	connect(this, &RealTimeGraphicsScene::configureChanged, m_pThread, &SortViewThread::onConfigureChanged, Qt::QueuedConnection);
	connect(this, &RealTimeGraphicsScene::focalZoomChanged, m_pThread, &SortViewThread::onFocalZoomChanged, Qt::QueuedConnection);
	connect(this, &RealTimeGraphicsScene::cameraAttributeChanged, m_pThread, &SortViewThread::onCameraAttributeChanged, Qt::QueuedConnection);
}

RealTimeGraphicsScene::~RealTimeGraphicsScene()
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
		removeItem(iter_wgt->proxyWidget);

		iter_wgt->pWgt->disconnect();
		delete iter_wgt->pWgt;

		iter_wgt->proxyWidget->disconnect();
		delete iter_wgt->proxyWidget;
	}
	dataMap.clear();
}

// 开始拖动
void RealTimeGraphicsScene::StartDrag()
{
	for (auto& entity : m_id2ViewMap)
	{
		entity.proxyWidget->startDrag(m_type);
	}
}

// 停止拖动
void RealTimeGraphicsScene::StopDrag()
{
	for (auto& entity : m_id2ViewMap)
	{
		entity.proxyWidget->endDrag(m_curChildWgtSize);
	}
}

void RealTimeGraphicsScene::onTimeOut()
{
	// 更新现有的
	for (auto iter_wgt = m_id2ViewMap.begin(); iter_wgt != m_id2ViewMap.end(); iter_wgt++)
	{
		auto pWgt = iter_wgt->pWgt;
		if (!pWgt->isHidden())
		{
			emit pWgt->updateContainter();
		}
	}
}

// 设置横纵向布局
void RealTimeGraphicsScene::SetContainerDirect(const EDirectType& type)
{
	if (m_type != type)
	{
		m_type = type;
		AdjustScenceSize(m_viewSize);
	}
}

// 调整scenceSize
void RealTimeGraphicsScene::AdjustScenceSize(QSize& viewSize)
{
	m_viewSize = viewSize;
	GetResizeInfo();
	if (m_id2ViewMap.size() <= m_curMaxShowChildSize)
	{
		setSceneRect(0, 0, viewSize.width(), viewSize.height());
	}
	else
	{
		switch (m_type)
		{
		case RealTimeGraphicsScene::Horizontal:
			setSceneRect(0, 0, (m_curChildWgtSize.width() + WGT_INTERFACE) * m_id2ViewMap.size(), viewSize.height());
			break;
		case RealTimeGraphicsScene::Vertical:
			setSceneRect(0, 0,  viewSize.width(), (m_curChildWgtSize.height() + WGT_INTERFACE) * m_id2ViewMap.size());
			break;
		default:
			break;
		}
	}

	UpdateDisplay();
}

bool RealTimeGraphicsScene::HasChild()
{
	return !m_id2ViewMap.isEmpty();
}

// 获取重置大小
void RealTimeGraphicsScene::GetResizeInfo()
{
	m_curChildWgtSize = GetChildWidgetSize();						// 当前子窗口的size
	m_curMaxShowChildSize = GetChildMaxWidgetSize(m_curChildWgtSize);			// 当前窗口大小一次能显示的视频的窗
}

// 获取子窗口大小
QSize RealTimeGraphicsScene::GetChildWidgetSize()
{
	QSize parentWgtSize = m_viewSize;

	QSize childSize;
	switch (m_type)
	{
	case Horizontal: {
		int curWidth = parentWgtSize.height() *HEIGHT_SCAL_WIDTH;
		childSize.setWidth(qMin(curWidth, parentWgtSize.width()));
		childSize.setHeight(parentWgtSize.height());
	}break;
	case Vertical:
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

// 获取可以显示的子窗口数量
int RealTimeGraphicsScene::GetChildMaxWidgetSize(QSize& childSize)
{
	int cnt = -1;
	if (!childSize.isValid())
		return cnt;
	switch (m_type)
	{
	case Horizontal: {
		cnt = m_viewSize.width() / qMax(1, childSize.width());
	}break;
	case Vertical:
	{
		cnt = m_viewSize.height() / (qMax(1, childSize.height()));
	}break;
	default:
		break;
	}

	cnt = qMax(cnt, 1);
	return cnt;
}

// 更新显示
void RealTimeGraphicsScene::UpdateDisplay()
{
	// 窗口大小小于最大大小时， 尽量居中显示
	if (m_id2ViewMap.size() < m_curMaxShowChildSize)
		UpdateDisplayTryCenter();
	else
		UpdateDisplayCommon();

	ResetViewVisible();
}

void RealTimeGraphicsScene::UpdateDisplayTryCenter()
{
	switch (m_type)
	{
	case Horizontal:
		UpdateDisplayTryCenterH();
		break;
	case Vertical:
		UpdateDisplayTryCenterV();
		break;
	default:
		break;
	}
}

void RealTimeGraphicsScene::UpdateDisplayTryCenterH()
{
	QSizeF curSize = sceneRect().size();
	int left = (curSize.width() - (m_curChildWgtSize.width()) * m_id2ViewMap.size()) / 2;

	for (auto& entity : m_idList)
	{
		auto iter = m_id2ViewMap.find(entity);
		if (iter != m_id2ViewMap.end())
		{
			iter->proxyWidget->setProxyViewSize(m_curChildWgtSize);
			iter->proxyWidget->setProxyViewPos(left, 0);
			if (iter->pWgt->isHidden())
				iter->pWgt->show();
			left = left + m_curChildWgtSize.width();
		}
	}
}

void RealTimeGraphicsScene::UpdateDisplayTryCenterV()
{
	QSizeF curSize = sceneRect().size();
	int height = (curSize.height() - (m_curChildWgtSize.height()) * m_id2ViewMap.size()) / 2;

	for (auto& entity : m_idList)
	{
		auto iter = m_id2ViewMap.find(entity);
		if (iter != m_id2ViewMap.end())
		{
			iter->proxyWidget->setProxyViewSize(m_curChildWgtSize);
			iter->proxyWidget->setProxyViewPos(0, height);
			if (iter->pWgt->isHidden())
				iter->pWgt->show();
			height = height + m_curChildWgtSize.height();
		}
	}
}

void RealTimeGraphicsScene::UpdateDisplayCommon()
{
	switch (m_type)
	{
	case Horizontal:
		UpdateDisplayCommonH();
		break;
	case Vertical:
		UpdateDisplayCommonV();
		break;
	default:
		break;
	}
}

void RealTimeGraphicsScene::UpdateDisplayCommonH()
{
	QSizeF curSize = sceneRect().size();
	int left = (curSize.width() - m_curChildWgtSize.width() * m_id2ViewMap.size()) / 2;

	for (auto& entity : m_idList)
	{
		auto iter = m_id2ViewMap.find(entity);
		if (iter != m_id2ViewMap.end())
		{
			auto& entity = *iter;
			iter->proxyWidget->setProxyViewSize(m_curChildWgtSize);
			iter->proxyWidget->setProxyViewPos(left, 0);
			if (entity.pWgt->isHidden())
				entity.pWgt->show();
			left = left + m_curChildWgtSize.width();
		}
	}
}

void RealTimeGraphicsScene::UpdateDisplayCommonV()
{
	QSizeF curSize = sceneRect().size();
	int height = (curSize.height() - (m_curChildWgtSize.height()) * m_id2ViewMap.size()) / 2;
	for (auto& entity : m_idList)
	{
		auto iter = m_id2ViewMap.find(entity);
		if (iter != m_id2ViewMap.end())
		{
			iter->proxyWidget->setProxyViewSize(m_curChildWgtSize);
			iter->proxyWidget->setProxyViewPos(0, height);
			if (iter->pWgt->isHidden())
				iter->pWgt->show();
			height = height + m_curChildWgtSize.height();
		}
	}
}

void RealTimeGraphicsScene::ResetViewVisible()
{
	auto viewList = views();
	for (auto pView : viewList)
	{
		auto  viewRect = pView->rect();
		auto topLeftPt = pView->mapToScene(0, 0);
		auto bottomRightPt = pView->mapToScene(viewRect.width(), viewRect.height());
		QRectF rect(topLeftPt, bottomRightPt);
		for (auto item : m_id2ViewMap)
		{
			auto itemRect = item.proxyWidget->sceneBoundingRect();
			if (rect.intersects(itemRect))
			{
				item.pWgt->setVisible(true);
				item.proxyWidget->setVisible(true);
			}
			else
			{
				item.proxyWidget->setVisible(false);
				item.pWgt->setVisible(false);
			}
		}
	}
}

void RealTimeGraphicsScene::onEntityStatusChanged(QList<long long> addIdList, QList<long long> deleteIdList)
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
			info.pWgt = new VedioContainer(ptr, nullptr);
			 info.pWgt->onUpdateContainer();
			connect(info.pWgt, &VedioContainer::entityDoubleClicked, this, &RealTimeGraphicsScene::entityDoubleClicked);
			
			QString name;
			ptr->GetEntityName(name);
			info.proxyWidget = new RealTimeGraphicsProxyWidget(name, info.pWgt, this);
			info.proxyWidget->setCacheMode(QGraphicsItem::NoCache);
			addItem(info.proxyWidget);

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
			auto pProxyWidget = iter->proxyWidget;
			m_idList.removeOne(id);
			iter = m_id2ViewMap.erase(iter);

			pWgt->hide();
			removeItem(pProxyWidget);
			pWgt->disconnect();
			delete pWgt;

			pProxyWidget->disconnect();
			delete pProxyWidget;
			isStatusChanged = true;
		}
	}

	if (isStatusChanged)
	{
		UpdateDisplay();
		emit childWgtSizeChanged();
	}
}

void RealTimeGraphicsScene::SortList(QList<long long> ids)
{
	if (!ids.isEmpty() && m_idList != ids)
	{
		m_idList = ids;
		UpdateDisplay();
	}
}
