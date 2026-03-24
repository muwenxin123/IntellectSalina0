#ifndef LY_VIDEOREALTIMECONTAINERWGT_H
#define LY_VIDEOREALTIMECONTAINERWGT_H

#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QTimer>
#include <QMap>
#include "SortViewThread.h"

class VedioContainer;
class VedioSDLContainer;
class VedioSDLContainerTmp;
namespace Ui {
class Ly_VideoRealTimeContainterWgt;
}

class Ly_VideoRealTimeContainterWgt : public QWidget
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
	};

    explicit Ly_VideoRealTimeContainterWgt(QWidget *parent = nullptr);
    ~Ly_VideoRealTimeContainterWgt();

	// 是否包含界面
	bool HasWgt();

	// 事件到达
	void onTimeOut();

	// 设置横纵向布局
	void SetContainerDirect(const EDirectType& type);

signals:
	void childWgtSizeChanged();

	void entityDoubleClicked(long long identity);

	void configureChanged();

	void focalZoomChanged(int imageZoom);

	void cameraAttributeChanged(long long identity, double yawValue, double pitchValue, double rollValue);

private:
	// 初始化
	void Init();

	// 绘制
	void paintEvent(QPaintEvent *event)override;

	// 事件
	bool event(QEvent *event)override;

	// 事件过滤器
	bool eventFilter(QObject *object, QEvent *event)override;

	// 设置界面大小
	void resizeEvent(QResizeEvent *event)override;
	
	// 获取窗口的大小
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

	// 获取重置大小后的信息
	void GetResizeInfo();

	// 设置界面可见
	void SetActiveBtnVisiable(bool Visiable);

private slots:
	// 左键切换被按下
	void onReduceBtnClicked();

	// 右键切换按钮本按下
	void onAddBtnClicked();

	// id排序
	void onIdSortChanged(QList<long long> ids);

	// 实体状态更新
	void onEntityStatusChanged(QList<long long> addIdList, QList<long long> deleteIdList);

private:
    Ui::Ly_VideoRealTimeContainterWgt *ui;
	EDirectType m_type = Horizontal;
	QMap<long long, ViewInfo> m_id2ViewMap;				// 当前 窗口列表
	QList<long long> m_idList;							// 当前 id列表
	int m_curMaxShowChildSize = -1;						// 当前窗口大小一次能显示的视频的窗口数量
	int m_curShowChildIndex = -1;						// 当前显示的窗口列表
	QSize m_curChildWgtSize;							// 当前子窗口的size
	SortViewThread* m_pThread = nullptr;				// id排序线程
};

#endif // Ly_VideoRealTimeContainterWgt_H
