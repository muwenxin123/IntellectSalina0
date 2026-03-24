#ifndef _REALTIMEGRAPHICSSCENE_H
#define _REALTIMEGRAPHICSSCENE_H

#include <QMap>
#include <QGraphicsScene>

#include "SortViewThread.h"
#include "RealTimeGraphicsProxyWidget.h"

class VedioContainer;
class VedioSDLContainer;
class RealTimeGraphicsScene : public QGraphicsScene
{
    Q_OBJECT
public:

	// 视图的朝向
	enum EDirectType {
		Horizontal,
		Vertical
	};

	struct ViewInfo {
		long long identity = 0;
		VedioContainer* pWgt = nullptr;
		RealTimeGraphicsProxyWidget* proxyWidget = nullptr;
	};

    explicit RealTimeGraphicsScene(QObject *parent = nullptr);
    ~RealTimeGraphicsScene();

	// 开始拖动
	void StartDrag();

	// 停止拖动
	void StopDrag();

	// 超时
	void onTimeOut();

	// 设置横纵向布局
	void SetContainerDirect(const EDirectType& type);

	// 调整scenceSize
	void AdjustScenceSize(QSize& viewSize);

	bool HasChild();

	// 重置窗口可见行
	void ResetViewVisible();

signals:
	void childWgtSizeChanged();

	void entityDoubleClicked(long long identity);

	void configureChanged();

	void focalZoomChanged(int imageZoom);

	void cameraAttributeChanged(long long identity, double yawValue, double pitchValue, double rollValue);

private:
	// 获取重置大小
	void GetResizeInfo();

	// 获取子窗口大小
	QSize GetChildWidgetSize();

	// 获取可以显示的子窗口数量
	int GetChildMaxWidgetSize(QSize& childSize);

	// 更新显示
	void UpdateDisplay();

	// 更新显示
	void UpdateDisplayTryCenter();
	void UpdateDisplayTryCenterH();
	void UpdateDisplayTryCenterV();

	// 更新显示
	void UpdateDisplayCommon();
	void UpdateDisplayCommonH();
	void UpdateDisplayCommonV();

private slots:
	// 实体状态更新
	void onEntityStatusChanged(QList<long long> addIdList, QList<long long> deleteIdList);

	void SortList(QList<long long> ids);

private:
	QSize m_viewSize;
	EDirectType m_type = Horizontal;
	QMap<long long, ViewInfo> m_id2ViewMap;				// 当前 窗口列表
	QList<long long> m_idList;												// 当前 id列表
	QSize m_curChildWgtSize;												// 当前子窗口的size
	int m_curMaxShowChildSize = -1;									// 当前窗口大小一次能显示的视频的窗口数量
	SortViewThread* m_pThread = nullptr;				// id排序线程
};

#endif // VedioContainer_H
